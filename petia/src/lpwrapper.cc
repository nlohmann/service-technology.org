#include <cstdio>
#include <cstdlib>
#include <set>
#include <map>
#include <algorithm>

#include "pnapi/pnapi.h"
#include "lp_solve/lp_lib.h"
#include "verbose.h"
#include "lpwrapper.h"

using std::cerr;
using std::cout;
using std::endl;
using std::set;
using std::map;
using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using pnapi::Node;

	/*************************************
	* Class LPWrapper method definitions *
	*************************************/

/** Constructor for systems created from the set of places of a Petri net.
	@param pn The Petri net for which the system is created.
	@param ntype Node type of the variables (true=places, false=transitions).
	@param vb If lp_solve should be verbose.
*/
LPWrapper::LPWrapper(PetriNet& pn, bool ntype, bool vb) : verbose(vb),cols(static_cast<unsigned int>(ntype?pn.getPlaces().size():pn.getTransitions().size())),basicrows(0),net(pn),solvecall(0),nodetype(ntype),result(2) {
	lp = make_lp(0,cols);
	if (!lp) abort(12,"could not create LP model");
}

/** Destructor.
*/
LPWrapper::~LPWrapper() {
	delete_lp(lp);
}

/** Create the invariant/marking equation for lp_solve.
	@param verbose If TRUE prints information on stderr.
	@return The number of equations on success, -1 otherwise.
*/
int LPWrapper::createIEquation() {
	solvecall=0;
	npos.clear(); // probably not necessary
	nvector.clear();

	set<Node*> nset,nset2;
	set<Place*> pset(net.getPlaces());
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		if (nodetype) nset.insert(static_cast<Node*>(*pit));
		else nset2.insert(static_cast<Node*>(*pit));
	set<Transition*> tset(net.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
		if (nodetype) nset2.insert(static_cast<Node*>(*tit));
		else nset.insert(static_cast<Node*>(*tit));

	// set the column variables in lp_solve according to the set of places/transitions
	int colnr=0;
	set<Node*>::iterator nit;
	for(nit=nset.begin(); nit!=nset.end(); ++nit,++colnr)
	{
		set_col_name(lp,colnr+1,const_cast<char*>((*nit)->getName().c_str()));
		set_int(lp,colnr+1,1); // declare all variables to be integer
		npos[(*nit)] = colnr; // remember the ordering of the places/transitions

	}
	if (colnr!=(int)(cols)) abort(12,"column number mismatch for LP model");

	//lp_solve objective: minimum weight vector (for minimal invariants)
	int *colpoint = new int[cols];
	REAL *mat = new REAL[cols];
	for(unsigned int y=0; y<cols; y++)
	{
		mat[y]=1;
		colpoint[y]=y+1;
	}
	set_add_rowmode(lp,TRUE); // go to rowmode (faster)
	if (!set_obj_fnex(lp,cols,mat,colpoint)) 
	{ 
		delete[] colpoint;
		delete[] mat; 
		abort(12,"failed to construct goal for LP model");
	}
	set_minim(lp);

	//create (transposed) incidence matrix by adding rows to lp_solve
	// for every transition or place ...
	for(nit=nset2.begin(); nit!=nset2.end(); ++nit)
	{
		for(unsigned int y=0; y<cols; ++y) mat[y]=0;
		set<pnapi::Arc*>::iterator ait;
		set<pnapi::Arc*> arcs = (*nit)->getPresetArcs(); 
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// set the nodes in the preset
			Node* n = &((*ait)->getSourceNode());
			// to negative weights
			if (nodetype) mat[npos[n]] -= (*ait)->getWeight();
			else mat[npos[n]] += (*ait)->getWeight();
		}
		arcs = (*nit)->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// and the nodes in the postset
			Node* n = &((*ait)->getTargetNode());
			// to positive weights
			if (nodetype) mat[npos[n]] += (*ait)->getWeight();
			else mat[npos[n]] -= (*ait)->getWeight();
		}
		//initialize the rows
		if (!add_constraintex(lp,cols,mat,colpoint,EQ,0)) 
		{ 
			delete[] colpoint; 
			delete[] mat; 
			abort(12,"failed to add constraint to LP model");
		}
	}

	//allow only nonnegative solutions
	for(int y=1; y<=(int)(cols); ++y)
		set_lowbo(lp,y,0);

	set_add_rowmode(lp,FALSE);	
	if (verbose) write_LP(lp,stderr);
	else set_verbose(lp,CRITICAL);
	basicrows = static_cast<unsigned int>(nset2.size());
	delete[] colpoint;
	delete[] mat;
	return (int)(basicrows);
}

/** Remove all constraints not belonging to the original invariant
	equation as created by createIEquation().
	@return TRUE on success.
*/
bool LPWrapper::stripConstraints() {
	return resize_lp(lp,basicrows,cols);
}

/* Forward a constraint to the internal data structure lprec.
	@param row A constraint.
	@param constr_type LE, EQ oder GE.
	@param rhs The right-hand-side of the constraint.
	@return If adding the constraint succeeded.

bool LPWrapper::addConstraint(REAL* row, int constr_type, REAL rhs) {
	return add_constraint(lp,row,constr_type,rhs);
}
*/

/** Forward a constraint to the internal data structure lprec.
	@param n A node (place or transition).
	@param b Whether the node variable should equal zero.
	@return If adding the constraint succeeded.
*/
bool LPWrapper::addConstraint(Node* n, bool b) {
	REAL row[1];
	int colno[1];
	row[0]=1;
	colno[0]=npos[n]+1;
	return add_constraintex(lp,1,row,colno,(b?EQ:GE),(b?0:1));
}

/** Forward a constraint of the form n1-n2>0 to the internal data structure lprec.
	@param n1 A node (place or transition).
	@param n2 A second node.
	@return If adding the constraint succeeded.
*/
bool LPWrapper::addConstraint(Node* n1, Node* n2) {
	REAL row[2];
	int colno[2];
	row[0]=1;
	row[1]=-1;
	colno[0]=npos[n1]+1;
	colno[1]=npos[n2]+1;
	return add_constraintex(lp,2,row,colno,GE,1);
}

/** Wrapper for solving the system.
	@return The result of solve(lp), usually 0 or 1 for success, 2 for unsolvable.
*/
int LPWrapper::solveSystem() {
	++solvecall;
	result = solve(lp);
//write_LP(lp,stdout);
	return result;
}

/** Get the name of a place belonging to a column, wrapper for get_col_name.
	@param col Column number.
	@return The result of get_col_name(lp,col).
*/
char* LPWrapper::getColName(int col) {
	return get_col_name(lp,col);
}

/** Wrapper for get_variables
	@param solution Array of REAL with at least number of places elements
		to contain a solution of lp_solve. Will be filled by the method.
	@return TRUE if successful.
*/
unsigned char LPWrapper::getVariables(REAL* solution) {
	return get_variables(lp,solution);
}

/** Return the solution of lp_solve as a map from places to multiplicities.
	@return The solution map
*/
map<Node*,int>& LPWrapper::getNVector() {
	REAL *sol = new REAL[cols];
	getVariables(sol);
	if (verbose>1) cerr << "LPSOLVE: ";
	for(int y=0; y<(int)(cols); ++y)
	{
		if (verbose>1 && sol[y]>0) cerr << getColName(y+1) << "=" << sol[y] << " ";
		nvector[net.findNode(getColName(y+1))] = static_cast<int>(sol[y]);
	}
	if (verbose>1) cerr << endl;
	delete[] sol;
	return nvector;
}

/** Get the number of calls to solveSystem() so far.
	@return The number of calls.
*/
int LPWrapper::getCalls() const { return solvecall; }

/** Check if the last solution is a trivial invariant, and if so, 
	exclude it from further considerations.
	@return If there was a solution and it was trivial.
*/
bool LPWrapper::excludeTrivial() {
	if (result>=2) return false; // no solution found
	map<Node*,int> inv(getNVector()); // get the solution
	map<Node*,int>::iterator mit;
	Node* n1(NULL);
	Node* n2(NULL);
	for(mit=inv.begin(); mit!=inv.end(); ++mit)
		if (mit->second!=0)
		{
			if (n2) return false; // more than two entries in the solution
			n2 = n1;
			n1 = mit->first;
		}
	if (!n1) return true; // no entry at all
	if (inv[n1]!=1) return false; // wrong weight for trivial invariant
	if (!n2) // trivial invariant is a singleton
	{
		trivials[n1].clear(); // zero it in future solutions
		return true;
	}
	if (inv[n2]!=1) return false; // wrong weight for trivial invariant
	trivials[n1].insert(n2); // memorize this trivial invariant
	trivials[n2].insert(n1); // for both occurring nodes
	return true;
}

/** Add constraints that forbid trivial invariants.
	@param n A node that must occur with at least weight one in solutions.
		Trivial invariants containing this node will be forbidden.
	@return If the constraints were successfully built.
*/
bool LPWrapper::addTrivialsConstraints(Node* n) {
	if (trivials.find(n)==trivials.end()) return true; // n is not in any trivial invariant
	set<Node*> nset(trivials[n]); // get all the trivial invariants containing n
	set<Node*>::iterator nit;
	REAL row[1];
	int colno[1];
	row[0]=1;
	if (nset.empty()) // if n is a singleton invariant, forbid n>0 as solution
	{
		colno[0]=npos[n]+1;
		return add_constraintex(lp,1,row,colno,EQ,0);
	}
	bool done(true);
	// since n should occur in the next solution, all nodes being
	// in a trivial invariant with n must not occur (otherwise
	// the solution is not minimal as the trivial invariant could
	// be subtracted)
	for(nit=nset.begin(); nit!=nset.end(); ++nit)
	{
		colno[0]=npos[*nit]+1;
		done &= add_constraintex(lp,1,row,colno,EQ,0);
	}
	return done;
}

