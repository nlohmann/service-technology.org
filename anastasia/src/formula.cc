// -*- C++ -*-

/*!
 * \file    formula.cc
 *
 * \brief   Class Formula
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/10/20
 *
 * \date    $Date: 2010-10-25 11:22:01 +0200 (Mo, 25. Okt 2010) $
 *
 * \version $Revision: 1.0 $
 */

#include "formula.h"

	/**********************************
	* Implementation of class Formula *
	**********************************/

/** Standard constructor. */ 
Formula::Formula(PetriNet& net) : verbose(0),pn(net),nrofplaces(0),ffv(1),ffsv(1),satisfied(false) {
	// construct the place ordering
	set<Place*> pset(pn.getPlaces());
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		id2p.push_back(*pit);
		p2id[*pit]=nrofplaces++;
	}
}

/** Destructor. */
Formula::~Formula() {}

/** Make a copy of the given formula (Standard assignment operator will not work).
	@param formula The formula to copy.
*/
void Formula::copyFormula(Formula& formula) {
	f = formula.f;
	ffv = formula.ffv;
	ffsv = formula.ffsv;
	vname = formula.vname;
	svname = formula.svname;
	svtov = formula.svtov;
	verbose = formula.verbose;
}

/** Remove the last clause of the formula (if any). */
void Formula::removeLastClause() { if (!f.empty()) f.pop_back(); }

/** Create a new boolean variable for the formula.
	@return An identifier for the variable.
*/
boolVar Formula::createVar() {
	return (ffv++);
}

/** Set the name of a formerly created boolean variable.
	@param v The identifier of the variable.
	@param name The name of the variable.
*/
void Formula::setVarName(boolVar v, string name) {
	vname[v]=name;
}

/** Get the name of a formerly created boolean variable.
	@param v The identifier of the variable.
	@return The name of the variable (or the empty string if unset).
*/
string Formula::getVarName(boolVar v) {
//	string empty;
	if (vname.find(v)==vname.end()) { string empty; return empty; }
	return vname[v];
}

/** Create a new variable and give it a name.
	@param name The name of the variable.
	@return The identifier of the variable.
*/
boolVar Formula::createVar(string name) {
	boolVar v(createVar());
	setVarName(v,name);
	return v;
}

/** Create a new variable and give it a name derived from a set variable.
	@param sv The set variable after which the new variable is to be named.
		If the set variable has no name, the new one will not get a name either.
	@param append The name to be appended to the set variable's name.
	@return The identifier of the new variable.
*/
boolVar Formula::createVar(setVar sv, string append) {
	boolVar v(createVar());
	if (svname.find(sv)!=svname.end())
		setVarName(v,svname[sv]+append);
	return v;
}

/** Create a new boolean variable for the formula.
	@return An identifier for the variable.
*/
setVar Formula::createSetVar() {
	setVar sv(ffsv++);
	if (svtov.find(sv)==svtov.end()) { svtov[sv]=ffv; ffv+=nrofplaces; }
	return sv;
}

/** Set the name of a formerly created set variable.
	@param sv The identifier of the set variable.
	@param name The name of the set variable.
*/
void Formula::setSetVarName(setVar sv, string name) {
	svname[sv]=name;
}

/** Get the name of a formerly created set variable.
	@param sv The identifier of set the variable.
	@return The name of set the variable (or an empty string if unset).
*/
string Formula::getSetVarName(setVar sv) {
//	string empty;
	if (svname.find(sv)==svname.end()) { string empty; return empty; }
	return svname[sv];
}

/** Create a new variable and give it a name.
	@param name The name of the variable.
	@return The identifier of the variable.
*/
setVar Formula::createSetVar(string name) {
	setVar sv(createSetVar());
	setSetVarName(sv,name);
	return sv;
}

/** Get the identifier for a place variable inside a set variable.
	@param sv The set variable.
	@param p The place.
	@return The id of the variable.
*/
boolVar Formula::getVar(setVar sv, Place& p) {
	if (p2id.find(&p)==p2id.end()) return 0;
	return (svtov[sv]+p2id[&p]);
}

/** After the formula has been solved, delivers the assignment for a set variable
	by returning those places that are set to true by MiniSat.
	@param sv The set variable identifier.
	@return The set of places set to true in this variable.
*/
set<Place*> Formula::getAssignment(setVar sv) {
	set<Place*> result;
	if (sv==0 || !satisfied) return result;
	boolVar xmin(svtov[sv]);
	boolVar xmax(svtov[sv]+nrofplaces);
	for(boolVar i=xmin; i<xmax; ++i)
		if ((mssolution)[i-1]) result.insert(id2p[i-xmin]);
	return result;
}

/** Get the assignment of a variable in the MiniSat solution.
	@param v The identifier of the variable.
	@return If the variable is set.
*/
bool Formula::getVarAssignment(boolVar v) {
	return (mssolution)[v];
}

/** Solve the formula.
	@return If there is a solution.
*/
bool Formula::solve() {
	vector<int> conflict;
	satisfied = minisat(f,mssolution,conflict);
	return satisfied;
}

/* Print the satisfying assignment in toto, using variable names where available.
	@param out The stream to print to.
	@return Returns false if there is no satisfying assignment.

bool Formula::printAssignment(ostream& out) {
	if (!satisfied) return false;
	for(boolVar j=1; j<mssolution.size(); ++j)
		if ((mssolution)[j])
		{
			if (vname.find(j)!=vname.end())
			{ 
				out << vname[j] << " "; 
				continue; 
			}
			map<setVar,string>::iterator svit;
			for(svit=svname.begin(); svit!=svname.end(); ++svit)
				if (svtov[svit->first]<=j && svtov[svit->first]+nrofplaces>j) break;
			if (svit==svname.end()) out << j << " ";
			else out << svname[svit->first] << ":" << id2p[j-svtov[svit->first]]->getName() << " ";
		}
	out << endl;
	return true;
}
*/

/** Print all the clauses in the formula, using variable names where available.
	@param out The stream to print to.
*/
void Formula::printClauses(ostream& out) {
	for(unsigned int j=0; j<f.size(); ++j)
	{
		out << "Clause " << j << ": ";
		for(unsigned int k=0; k<f[j].size(); ++k)
		{
			Literal num(f[j][k]);
			if (num<0) { num=-num; out << "-"; }
			else if (num==0) break;
			if (vname.find(num)!=vname.end())
			{ out << vname[num] << " "; continue; }
			map<setVar,string>::iterator svit;
			for(svit=svname.begin(); svit!=svname.end(); ++svit)
				if (svtov[svit->first]<=(unsigned int)num && svtov[svit->first]+nrofplaces>(unsigned int)num) break;
			if (svit==svname.end()) out << num << " ";
			else out << svname[svit->first] << ":" << id2p[num-svtov[svit->first]]->getName() << " ";
		}
		out << endl;
	}
}

/** Set a variable of the formula to true or false.
	@param v The identifier of the variable.
	@param value If the variable should be true or false.
*/
void Formula::SetTo(boolVar v, bool value) {
	vector<Literal> clause;
	clause.push_back((value?v:-v));
	clause.push_back(0);
	f.push_back(clause);
}

/** Negation of a variable.
	@param v The variable.
	@return The negated variable.
*/
Literal Formula::Not(boolVar v) { return -v; }

/** Logical And over two literals/boolVars.
	@param l1 First literal.
	@param l2 Second literal.
	@return A variable representing the logical And.
*/
boolVar Formula::And(Literal l1, Literal l2) {
	vector<Literal> lits;
	lits.push_back(l1);
	lits.push_back(l2);
	return (And(lits));
}

/** Logical And over (possibly) more than two literals.
	@param lits A vector of literals (boolVar or negated boolVar).
	@return A variable representing the logical And.
*/
boolVar Formula::And(vector<Literal> lits) {
	boolVar x(createVar());
	if (verbose) {
		stringstream sstr;
		sstr << "And_" << x;
		string str;
		sstr >> str;
		setVarName(x,str);
	}
	vector<int> pclause,nclause;
	nclause.push_back(x);
	pclause.push_back(0);
	pclause.push_back(-x);
	pclause.push_back(0);
	for(unsigned int i=0; i<lits.size(); ++i)
	{
		if (lits[i]==0) continue;
		pclause[0]=lits[i];
		f.push_back(pclause);
		nclause.push_back(-lits[i]);
	}
	nclause.push_back(0);
	f.push_back(nclause);
	return x;
}

/** Logical Or over two literals/boolVars.
	@param l1 First literal.
	@param l2 Second literal.
	@return A variable representing the logical Or.
*/
boolVar Formula::Or(Literal l1, Literal l2) {
	vector<Literal> lits;
	lits.push_back(l1);
	lits.push_back(l2);
	return (Or(lits));
}

/** Logical Or over (possibly) more than two literals.
	@param lits A vector of literals (boolVar or negated boolVar).
	@return A variable representing the logical Or.
*/
boolVar Formula::Or(vector<Literal> lits) {
	boolVar x(createVar());
	if (verbose) {
		stringstream sstr;
		sstr << "And_" << x;
		string str;
		sstr >> str;
		setVarName(x,str);
	}
	vector<int> pclause,nclause;
	pclause.push_back(-x);
	nclause.push_back(0);
	nclause.push_back(x);
	nclause.push_back(0);
	for(unsigned int i=0; i<lits.size(); ++i)
	{
		if (lits[i]==0) continue;
		nclause[0]=-lits[i];
		f.push_back(nclause);
		pclause.push_back(lits[i]);
	}
	pclause.push_back(0);
	f.push_back(pclause);
	return x;
}

/** Creates a variable representing the implication l1->l2.
	@param l1 A literal.
	@param l2 A literal.
	@return A variable for l1->l2.
*/
boolVar Formula::Implies(Literal l1, Literal l2) {
	boolVar x(createVar());
	if (verbose) {
		stringstream sstr;
		sstr << "Impl_" << x;
		string str;
		sstr >> str;
		setVarName(x,str);
	}
	vector<Literal> clause;
	clause.push_back(-x);
	clause.push_back(-l1);
	clause.push_back(l2);
	clause.push_back(0);
	f.push_back(clause);
	clause.clear();
	clause.push_back(x);
	clause.push_back(l1);
	clause.push_back(0);
	f.push_back(clause);
	clause[1] = -l2;
	f.push_back(clause);
	return x;
}

/** Creates a variable representing the equivalence l1<->l2.
	@param l1 A literal.
	@param l2 A literal.
	@return A variable for l1<->l2.
*/
boolVar Formula::Equals(Literal l1, Literal l2) {
	boolVar x(createVar());
	if (verbose) {
		stringstream sstr;
		sstr << "Impl_" << x;
		string str;
		sstr >> str;
		setVarName(x,str);
	}
	vector<Literal> clause;
	clause.push_back(-x);
	clause.push_back(-l1);
	clause.push_back(l2);
	clause.push_back(0);
	f.push_back(clause);
	clause[1]=l1;
	clause[2]=-l2;
	f.push_back(clause);
	clause[0] = x;
	clause[2] = l2;
	f.push_back(clause);
	clause[1] = -l1;
	clause[2] = -l2;
	f.push_back(clause);
	return x;
}

/** Delivers a variable equivalent to the property Empty of a set variable, i.e. all
	places in the set variable set to false.
	@param sv The set variable.
	@return The property Empty.
*/
boolVar Formula::Empty(setVar sv) {
	if (sv==0) return 0;
	boolVar base(svtov[sv]);
	boolVar x;
	if (verbose) x=createVar(sv,"_empty");
	else x=createVar();
	vector<Literal> clause;
	for(unsigned int j=0; j<nrofplaces; ++j)
		clause.push_back(base+j);
	clause.push_back(x);
	clause.push_back(0);
	f.push_back(clause);
	clause.clear();
	clause.push_back(0);
	clause.push_back(-x);
	clause.push_back(0);
	for(unsigned int j=0; j<nrofplaces; ++j)
	{
		clause[0] = -(base+j);
		f.push_back(clause);
	}
	return x;
}

/** Declares a set variable to be empty. Has a lower complexity than Empty().
	@param sv The set variable.
*/
void Formula::XEmpty(setVar sv) {
	if (sv==0) return;
	boolVar base(svtov[sv]);
	vector<Literal> clause(2);
	clause[1]=0;
	for(unsigned int j=0; j<nrofplaces; ++j)
	{
		clause[0]=-base-j;
		f.push_back(clause);
	}
}

/** Declares a set variable to be non-empty. Has a lower complexity than Empty().
	@param sv The set variable.
*/
void Formula::XNEmpty(setVar sv) {
	if (sv==0) return;
	boolVar base(svtov[sv]);
	vector<Literal> clause;
	for(unsigned int j=0; j<nrofplaces; ++j)
		clause.push_back(base+j);
	clause.push_back(0);
	f.push_back(clause);
}

/** Checks if the set of places given by sv is marked.
	@param sv The set variable to be checked.
	@return A variable representing the marked status of sv.
*/
boolVar Formula::Marked(setVar sv) {
	if (sv==0) return 0;
	boolVar offset(svtov[sv]);
	vector<Literal> clause;
	set<Place*> pset(pn.getPlaces());
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		if ((*pit)->getTokenCount()>0) clause.push_back(p2id[*pit]+offset);
	return (Or(clause));
}

/** Declares a set variable to be marked. Has a lower complexity than Marked().
	@param sv The set variable to be marked.
*/
void Formula::XMarked(setVar sv) {
	if (sv==0) return;
	boolVar offset(svtov[sv]);
	vector<Literal> clause;
	set<Place*> pset(pn.getPlaces());
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		if ((*pit)->getTokenCount()>0) clause.push_back(p2id[*pit]+offset);
	clause.push_back(0);
	f.push_back(clause);
}

/** Declares a set variable to be unmarked. Has a lower complexity than Marked().
	@param sv The set variable to be unmarked.
*/
void Formula::XNMarked(setVar sv) {
	if (sv==0) return;
	boolVar offset(svtov[sv]);
	vector<Literal> clause(2);
	clause[1]=0;
	set<Place*> pset(pn.getPlaces());
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		if ((*pit)->getTokenCount()>0) 
		{
			clause[0]=-(p2id[*pit]+offset);
			f.push_back(clause);
		}
}

/** Checks whether sv1 represents a subset of sv2.
	@param sv1 A set variable.
	@param sv2 A set variable.
	@return A variable representing the inclusion of sv1 in sv2.
*/
boolVar Formula::Subset(setVar sv1, setVar sv2) {
	if (sv1==0 || sv2==0) return 0;
	boolVar offset1(svtov[sv1]);
	boolVar offset2(svtov[sv2]);
	boolVar x(createVar());
	if (verbose && svname.find(sv1)!=svname.end() && svname.find(sv2)!=svname.end())
		setVarName(x,svname[sv1]+"_subseteq_"+svname[sv2]);
	for(unsigned int j=0; j<nrofplaces; ++j)
	{
		vector<Literal> clause;
		clause.push_back(-x);
		clause.push_back(-j-offset1);
		clause.push_back(j+offset2);
		clause.push_back(0);
		f.push_back(clause);
		clause.clear();
		clause.push_back(x);
		clause.push_back(j+offset1);
		clause.push_back(0);
		f.push_back(clause);
		clause[1] = -j-offset2;
		f.push_back(clause);
	}
	return x;
}

/** Forces sv1 to be a subset of sv2.
	@param sv1 A set variable.
	@param sv2 A set variable.
*/
void Formula::XSubset(setVar sv1, setVar sv2) {
	if (sv1==0 || sv2==0) return;
	boolVar offset1(svtov[sv1]);
	boolVar offset2(svtov[sv2]);
	vector<Literal> clause(3);
	clause[2]=0;
	for(unsigned int j=0; j<nrofplaces; ++j)
	{
		clause[0]=-j-offset1;
		clause[1]=j+offset2;
		f.push_back(clause);
	}
}

/** The property of a given set variable representing a siphon.
	@param sv The set variable.
	@return A variable representing if sv is a siphon.
*/
boolVar Formula::Siphon(setVar sv) {
	if (sv==0) return 0; // no set variable given
	boolVar base(svtov[sv]); // offset for sv
	boolVar x; // the final result
	if (verbose) x=createVar(sv,"_siphon");
	else x=createVar();
	vector<Literal> yclause; // a clause for "all transitions must fulfill to the siphon property"
	yclause.push_back(x);
	set<Transition*> tset(pn.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)	
	{ // clauses for each transition are constructed now
		boolVar y; // helper variable for "this transition contradicts the siphon"
		if (verbose) {
			string yname = "_nosiphon_" + (*tit)->getName();
			y = createVar(sv,yname); 
		} else y=createVar();
		vector<Literal> clause;
		clause.push_back(0);
		clause.push_back(-x);
		set<pnapi::Arc*>::iterator ait;
		const set<pnapi::Arc*>& prearcs((*tit)->getPresetArcs());
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
			clause.push_back(base+p2id[&((*ait)->getPlace())]);
		clause.push_back(0);
		const set<pnapi::Arc*>& postarcs((*tit)->getPostsetArcs());
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
		{
			clause[0] = -(base+p2id[&((*ait)->getPlace())]);
			f.push_back(clause);
			clause[1] = y;
			f.push_back(clause);
			clause[1] = -x;
		}		
		clause.clear();
		clause.push_back(-y);
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
			clause.push_back(base+p2id[&((*ait)->getPlace())]);
		clause.push_back(0);
		f.push_back(clause);
		clause.clear();
		clause.push_back(0);
		clause.push_back(-y);		
		clause.push_back(0);
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
		{
			clause[0] = -(base+p2id[&((*ait)->getPlace())]);
			f.push_back(clause);
		}
		yclause.push_back(y);
	}
	yclause.push_back(0);
	f.push_back(yclause);	
	return x;
}

/** Declare a set variable to represent a siphon. Has lower complexity than Siphon().
	@param sv The set variable.
*/
void Formula::XSiphon(setVar sv) {
	if (sv==0) return; // no set variable given
	boolVar base(svtov[sv]); // offset for sv
	set<Transition*> tset(pn.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)	
	{ // clauses for each transition are constructed now
		vector<Literal> clause;
		clause.push_back(0);
		set<pnapi::Arc*>::iterator ait;
		const set<pnapi::Arc*>& prearcs((*tit)->getPresetArcs());
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
			clause.push_back(base+p2id[&((*ait)->getPlace())]);
		clause.push_back(0);
		const set<pnapi::Arc*>& postarcs((*tit)->getPostsetArcs());
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
		{
			clause[0] = -(base+p2id[&((*ait)->getPlace())]);
			f.push_back(clause);
		}		
	}
}

/** The property of a given set variable representing a trap.
	@param sv The set variable.
	@return A variable representing if the set variable sv represents a trap.
*/
boolVar Formula::Trap(setVar sv) {
	if (sv==0) return 0; // no set variable
	boolVar base(svtov[sv]); // offset for sv
	boolVar x; // the final result
	if (verbose) x=createVar(sv,"_trap");
	else x=createVar();
	vector<Literal> yclause; // a clause for "all transitions must comply to the trap property"
	yclause.push_back(x);
	set<Transition*> tset(pn.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)	
	{ // for each transition the clauses for the trap property are formulated
		boolVar y; // helper variable for "not a trap due to this transition"
		if (verbose) {
			string yname = "_notrap_" + (*tit)->getName();
			y=createVar(sv,yname);
		} else y=createVar();
		vector<Literal> clause;
		clause.push_back(0);
		clause.push_back(-x);
		set<pnapi::Arc*>::iterator ait;
		const set<pnapi::Arc*>& postarcs((*tit)->getPostsetArcs());
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
			clause.push_back(base+p2id[&((*ait)->getPlace())]);
		clause.push_back(0);
		const set<pnapi::Arc*>& prearcs((*tit)->getPresetArcs());
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
		{
			clause[0] = -(base+p2id[&((*ait)->getPlace())]);
			f.push_back(clause);
			clause[1] = y;
			f.push_back(clause);
			clause[1] = -x;
		}		
		clause.clear();
		clause.push_back(-y);
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
			clause.push_back(base+p2id[&((*ait)->getPlace())]);
		clause.push_back(0);
		f.push_back(clause);
		clause.clear();
		clause.push_back(0);
		clause.push_back(-y);		
		clause.push_back(0);
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
		{
			clause[0] = -(base+p2id[&((*ait)->getPlace())]);
			f.push_back(clause);
		}
		yclause.push_back(y);
	}
	yclause.push_back(0);
	f.push_back(yclause);	
	return x;
}

/** Declare a set variable to represent a trap.
	@param sv The set variable.
*/
void Formula::XTrap(setVar sv) {
	if (sv==0) return; // no set variable
	boolVar base(svtov[sv]); // offset for sv
	set<Transition*> tset(pn.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)	
	{ // for each transition the clauses for the trap property are formulated
		vector<Literal> clause;
		clause.push_back(0);
		set<pnapi::Arc*>::iterator ait;
		const set<pnapi::Arc*>& postarcs((*tit)->getPostsetArcs());
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
			clause.push_back(base+p2id[&((*ait)->getPlace())]);
		clause.push_back(0);
		const set<pnapi::Arc*>& prearcs((*tit)->getPresetArcs());
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
		{
			clause[0] = -(base+p2id[&((*ait)->getPlace())]);
			f.push_back(clause);
		}		
	}
}

/** Calculate the maximal trap inside a given set.
	@param sv The given set (by its set variable).
	@param trap The maximal trap inside the set.
	@return A variable representing if trap is a maximal trap inside sv.
*/
boolVar Formula::MaxTrap(setVar sv, setVar trap) {
	if (sv==0 || trap==0) return 0; 
	vector<Literal> clause,cl2,ydetailed;
	boolVar zoff(svtov[sv]);
	boolVar zoffold;
	setVar av(sv); // the active set variable
	boolVar yoff,y; // variable representing equality between zij and and its recursive declaration
	set<Transition*> tset(pn.getTransitions());
	set<Transition*>::iterator tit;
	for(unsigned int j=0; j<nrofplaces; ++j)
	{
		if (verbose) { // create the set variable for the j-th iteration equality formula
			stringstream sstr;
			string avname;
			sstr << "MaxTrap_";
			if (svname.find(sv)!=svname.end()) sstr << svname[sv] << "_"; 
			if (svname.find(trap)!=svname.end()) sstr << svname[trap] << "_"; 
			sstr << "EQ_" << j << "[" << av << "]";
			sstr >> avname;
			y = createSetVar(avname); // yij
		} else y = createSetVar();
		yoff = svtov[y];
		zoffold = zoff; // offset for zij-1
		if (j+1<nrofplaces) { // create the set variable for the j-th iteration
			if (verbose) {
				stringstream sstr;
				string avname;
				sstr << "MaxTrap_";
				if (svname.find(sv)!=svname.end()) sstr << svname[sv] << "_"; 
				if (svname.find(trap)!=svname.end()) sstr << svname[trap] << "_"; 
				sstr << "Zi" << j << "[" << av << "]";
				sstr >> avname;
				av = createSetVar(avname); // zij
			} else av = createSetVar();
		} else av = trap; // zin
		zoff = svtov[av];
		map<Transition*,int> xtj; // variable numbers for xtj (helpers)
		for(tit=tset.begin(); tit!=tset.end(); ++tit)
		{
			boolVar x;
			if (verbose) x=createVar(av,"_OR_post_"+(*tit)->getName());
			else x=createVar();
			xtj[*tit]=x;
			clause.clear(); // clause xtj or OR(k) zkj-1 [k over postset of t]
			clause.push_back(x);
			set<pnapi::Arc*> tarcs((*tit)->getPostsetArcs());
			set<pnapi::Arc*>::iterator tait;
			for(tait=tarcs.begin(); tait!=tarcs.end(); ++tait)
			{
				Place* p(&((*tait)->getPlace()));
				clause.push_back(p2id[p]+zoffold);
				cl2.clear(); // clauses in AND(k) (not xtj or not zkj-1)
				cl2.push_back(-x);
				cl2.push_back(-p2id[p]-zoffold);
				cl2.push_back(0);
				f.push_back(cl2);
			}
			clause.push_back(0);
			f.push_back(clause);
		}
		for(unsigned int i=0; i<nrofplaces; ++i)
		{
			ydetailed.push_back(i+yoff); // remember all yij
			clause.clear(); // clause not yij or not zij or zij-1
			clause.push_back(-i-yoff);
			clause.push_back(-i-zoff);
			clause.push_back(i+zoffold);
			clause.push_back(0);
			f.push_back(clause);
			clause[0] = i+yoff; // yij or zij or zij-1
			clause[1] = i+zoff;
			f.push_back(clause);
			clause.clear(); // clause not yij or zij or not zij-1 or OR(t) xtj [t over postset of i]
			clause.push_back(-i-yoff);
			clause.push_back(i+zoff);
			clause.push_back(-i-zoffold);
			const set<pnapi::Arc*>& arcs(id2p[i]->getPostsetArcs());
			set<pnapi::Arc*>::iterator ait,tait;
			for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			{
				Transition* t(&(*ait)->getTransition());
				clause.push_back(xtj[t]);
				cl2.clear(); // clauses in AND(t) (not yij or not zij or not xtj)
				cl2.push_back(-i-yoff);
				cl2.push_back(-i-zoff);
				cl2.push_back(-xtj[t]);
				cl2.push_back(0);
				f.push_back(cl2); 
				cl2[0] = i+yoff; // clauses in AND(t) (yij or zij or not xtj)
				cl2[1] = i+zoff;
				f.push_back(cl2);
			}
			clause.push_back(0);
			f.push_back(clause);
			clause[0] = i+yoff; // clause yij or not zij or not zij-1 or OR(t) xtj
			clause[1] = -i-zoff;
			f.push_back(clause);
		}
	}
	// clause for all the yij (AND)
	boolVar z(And(ydetailed));
	if (verbose && svname.find(sv)!=svname.end() && svname.find(trap)!=svname.end())
		setVarName(z,"MaxTrap_"+svname[sv]+"_"+svname[trap]);
	return z;
}

/** Calculate the maximal trap inside a given set.
	@param sv The given set (by its set variable).
	@param trap The maximal trap inside the set.
*/
void Formula::XMaxTrap(setVar sv, setVar trap) {
	if (sv==0 || trap==0) return; 
	vector<Literal> clause;
	vector<Literal> cl2(3);
	cl2[2]=0;
	boolVar zoff(svtov[sv]);
	boolVar zoffold;
	set<Transition*> tset(pn.getTransitions());
	set<Transition*>::iterator tit;
	set<pnapi::Arc*>::iterator ait;
	setVar av(sv); // the active set variable
	for(unsigned int j=0; j<nrofplaces; ++j)
	{
		zoffold = zoff; // offset for zij-1
		if (j+1<nrofplaces) { // create the set variable for the j-th iteration
			if (verbose) {
				stringstream sstr;
				string avname;
				sstr << "MaxTrap_";
				if (svname.find(sv)!=svname.end()) sstr << svname[sv] << "_"; 
				if (svname.find(trap)!=svname.end()) sstr << svname[trap] << "_"; 
				sstr << "Zi" << j << "[" << av << "]";
				sstr >> avname;
				av = createSetVar(avname); // zij
			} else av = createSetVar(); // zij
		} else av = trap; // zin
		zoff = svtov[av];
		map<Transition*,int> xtj; // variable numbers for xtj (helpers)
		for(tit=tset.begin(); tit!=tset.end(); ++tit)
		{
			boolVar x;
			if (verbose) x=createVar(av,"_OR_post_"+(*tit)->getName());
			else x=createVar();
			xtj[*tit]=x;
			clause.clear(); // clause xtj or OR(k) zkj-1 [k over postset of t]
			clause.push_back(x);
			const set<pnapi::Arc*>& ttarcs((*tit)->getPostsetArcs());
			for(ait=ttarcs.begin(); ait!=ttarcs.end(); ++ait)
			{
				Place* p(&((*ait)->getPlace()));
				clause.push_back(p2id[p]+zoffold);
				// clauses in AND(k) (not xtj or not zkj-1)
				cl2[0]=-x;
				cl2[1]=-p2id[p]-zoffold;
				f.push_back(cl2);
			}
			clause.push_back(0);
			f.push_back(clause);
		}
		for(unsigned int i=0; i<nrofplaces; ++i)
		{
			// clause not zij or zij-1
			cl2[0]=-i-zoff;
			cl2[1]=i+zoffold;
			f.push_back(cl2);
			clause.clear(); // clause zij or not zij-1 or OR(t) xtj [t over postset of i]
			clause.push_back(i+zoff);
			clause.push_back(-i-zoffold);
			const set<pnapi::Arc*>& parcs(id2p[i]->getPostsetArcs());
			for(ait=parcs.begin(); ait!=parcs.end(); ++ait)
			{
				Transition* t(&(*ait)->getTransition());
				clause.push_back(xtj[t]);
				// clauses in AND(t) (not zij or not xtj)
				cl2[1]=-xtj[t];
				f.push_back(cl2);
			}
			clause.push_back(0);
			f.push_back(clause);
		}
	}
}

/** Minimize or maximize the result given by MiniSAT.
	@param sv The set variable to minimize or maximize.
	@param maximize If the set variable should be maximized.
	@param enforce A set of places that must remain in the result under all circumstances.
	@param forbid A set of places that must not appear in the result under any circumstances.
	@param pgen A place that was used to generate the result (may be NULL).
*/
void Formula::MinMax(setVar sv, bool maximize, set<Place*>* enforce, set<Place*>* forbid, Place* pgen) {
	if (!satisfied) return; // no solution, no maximizing or minimizing
	set<Place*> all(pn.getPlaces());
	set<Place*> pset(getAssignment(sv));
	set<Place*>::iterator pit;
	// set the places that can be in- or excluded from the previous solution
	if (!maximize) all.swap(pset);
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		if (maximize ^ (all.find(*pit)==all.end()))
			SetTo(getVar(sv,**pit),maximize);
	if (!maximize) all.swap(pset);
	// now check for each place if it can be in- or excluded explicitly
	for(pit=all.begin(); pit!=all.end(); ++pit)
	{
		if (*pit == pgen) continue; // the generating place cannot be excluded
		if (maximize ^ (pset.find(*pit)==pset.end())) continue; // the place already has been in/excluded
		if (maximize && forbid && (*forbid).find(*pit)!=(*forbid).end()) continue; // the place must not be included
		if (!maximize && enforce && (*enforce).find(*pit)!=(*enforce).end()) continue; // the place must not be excluded
		SetTo(getVar(sv,**pit),maximize); // it's okay, so do it
		if (pgen) SetTo(getVar(sv,*pgen),true); // the generation place must explicitly be included
		bool b = solve(); // resolve the formula
		if (pgen) removeLastClause(); // remove the inclusion of the generating place
		removeLastClause(); // remove the statement about the current place
		if (b) { // if there is a solution, adapt the formula to it
			set<Place*> pset2(getAssignment(sv));
			set<Place*>::iterator pit2;
			if (!maximize) pset.swap(pset2);
			for(pit2=pset2.begin(); pit2!=pset2.end(); ++pit2)
				if (pset.find(*pit2)==pset.end()) // a place to be in/excluded is found
					SetTo(getVar(sv,**pit2),maximize); // so do it
			if (maximize) pset.swap(pset2);
		}
	}
	if (!satisfied) { // if the last try to solve the formula failed, the solution must be recalculated
		if (pgen) SetTo(getVar(sv,*pgen),true); // don't forget the generating place
		solve(); // resolve the formula
		if (pgen) removeLastClause(); // and remove the clause of the generating place again 
	}
}

