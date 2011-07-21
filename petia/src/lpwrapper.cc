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

	/*************************************
	* Class LPWrapper method definitions *
	*************************************/

/** Constructor for systems created from the set of places of a Petri net.
	@param pn The Petri net for which the system is created.
	@param vb If lp_solve should be verbose.
*/
LPWrapper::LPWrapper(PetriNet& pn, bool vb) : verbose(vb),cols(pn.getPlaces().size()),basicrows(0),net(pn),solvecall(0) {
	lp = make_lp(0,cols);
	if (!lp) abort(12,"could not create LP model");
}

/** Destructor.
*/
LPWrapper::~LPWrapper() {
	delete_lp(lp);
}

/** Create the invariant equation for lp_solve.
	@param verbose If TRUE prints information on stderr.
	@return The number of equations on success, -1 otherwise.
*/
int LPWrapper::createIEquation() {
	solvecall=0;
	ppos.clear(); // probably not necessary
	pvector.clear();

	// set the column variables in lp_solve according to the set of places
	int colnr=0;
	set<Place*> pset(net.getPlaces());
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit,++colnr)
	{
		set_col_name(lp,colnr+1,const_cast<char*>((*pit)->getName().c_str()));
		set_int(lp,colnr+1,1); // declare all variables to be integer
		ppos[(*pit)] = colnr; // remember the ordering of the places

	}
	if (colnr!=(int)(cols)) abort(12,"column number mismatch for LP model");

	//lp_solve objective: minimum weight vector (better idea?)
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

	//create transposed incidence matrix by adding rows to lp_solve
	set<Transition*> tset(net.getTransitions());
	set<Transition*>::iterator tit;
	// for every transition ...
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		for(unsigned int y=0; y<cols; ++y) mat[y]=0;
		set<pnapi::Arc*>::iterator ait;
		set<pnapi::Arc*> arcs = (*tit)->getPresetArcs(); 
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// set the places in the preset
			Place* p = &((*ait)->getPlace());
			// to negative weights
			mat[ppos[p]] -= (*ait)->getWeight();
		}
		arcs = (*tit)->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			// and the places in the postset
			Place* p = &((*ait)->getPlace());
			// to positive weights
			mat[ppos[p]] += (*ait)->getWeight();
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
	basicrows = tset.size();
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

/** Forward a constraint to the internal data structure lprec.
	@param row A constraint.
	@param constr_type LE, EQ oder GE.
	@param rhs The right-hand-side of the constraint.
	@return If adding the constraint succeeded.
*/
bool LPWrapper::addConstraint(REAL* row, int constr_type, REAL rhs) {
	return add_constraint(lp,row,constr_type,rhs);
}

/** Forward a constraint to the internal data structure lprec.
	@param p A place.
	@param b Whether the place variable should equal zero.
	@return If adding the constraint succeeded.
*/
bool LPWrapper::addConstraint(Place* p, bool b) {
	REAL row[1];
	int colno[1];
	row[0]=1;
	colno[0]=ppos[p]+1;
	return add_constraintex(lp,1,row,colno,(b?EQ:GE),(b?0:1));
}

/** Wrapper for solving the system.
	@return The result of solve(lp), usually 0 or 1 for success, 2 for unsolvable.
*/
int LPWrapper::solveSystem() {
	++solvecall;
	return solve(lp);
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
map<Place*,int>& LPWrapper::getPVector() {
	REAL *sol = new REAL[cols];
	getVariables(sol);
	if (verbose>1) cerr << "LPSOLVE: ";
	for(int y=0; y<(int)(cols); ++y)
	{
		if (verbose>1 && sol[y]>0) cerr << getColName(y+1) << "=" << sol[y] << " ";
		pvector[net.findPlace(getColName(y+1))] = static_cast<int>(sol[y]);
	}
	if (verbose>1) cerr << endl;
	delete[] sol;
	return pvector;
}

/** Get the number of calls to solveSystem() so far.
	@return The number of calls.
*/
int LPWrapper::getCalls() const { return solvecall; }

