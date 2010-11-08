// -*- C++ -*-

/*!
 * \file    formula.h
 *
 * \brief   Class Formula
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/10/20
 *
 * \date    $Date: 2010-10-20 11:22:00 +0200 (Mi, 20. Okt 2010) $
 *
 * \version $Revision: 1.0 $
 */

#ifndef FORMULA_H
#define FORMULA_H

#include <set>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <pnapi/pnapi.h>

using std::set;
using std::vector;
using std::map;
using std::string;
using std::ostream;
using std::stringstream;
using std::cout;
using std::endl;
using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;

typedef unsigned int setVar;
typedef unsigned int boolVar;
typedef int Literal;

/// MiniSat entry function
extern bool minisat(vector< vector <int> > &, vector<bool>&, vector<int>&);
extern vector<bool>* minisat2(vector< vector <int> > &);

/*! \brief A Formula in CNF for export to MiniSat. */
class Formula {
public:
	/// Standard constructor.
	Formula(PetriNet& net);

	/// Destructor.
	~Formula();

	/// Make a copy of another formula
	void copyFormula(Formula& f);

	/// Remove the last clause of the formula (if any)
	void removeLastClause();

	/// Create a variable
	boolVar createVar();

	/// Set a name for a variable
	void setVarName(boolVar v, string name);

	/// Set a name for a variable
	string getVarName(boolVar v);

	/// Create a named variable
	boolVar createVar(string name);

	/// Create a variable with a name derived from a set variable
	boolVar createVar(setVar sv, string append);

	/// Create a set variable
	setVar createSetVar();

	/// Set a name for a set variable
	void setSetVarName(setVar sv, string name);

	/// Set a name for a set variable
	string getSetVarName(setVar sv);

	/// Create a named set variable
	setVar createSetVar(string name);

	/// Obtain the identifier for the variable of place p in the set sv
	boolVar getVar(setVar sv, Place& p);

	/// Get the assignment of a set variable after solving the formula
	set<Place*> getAssignment(setVar sv);

	/// Get the assignment of a variable after solving the formula
	bool getVarAssignment(boolVar v);

	/// Solve a formula
	bool solve();

	/// Print the satisfying assignment in toto
	bool printAssignment(ostream& out);

	/// Print all the clauses of the formula
	void printClauses(ostream& out);

	/// set a variable to true or false (in the formula)
	void SetTo(boolVar v, bool value);

	/// Negate a variable
	Literal Not(boolVar v);

	/// Logical And of two literals
	boolVar And(Literal l1, Literal l2);

	/// And of several boolVar and/or negated boolVar
	boolVar And(vector<Literal> lits);

	/// Logical Or of two literals
	boolVar Or(Literal l1, Literal l2);

	/// Or of several boolVar and/or negated boolVar
	boolVar Or(vector<Literal> lits);

	/// Implication l1->l2
	boolVar Implies(Literal l1, Literal l2);

	/// Equality of two literals
	boolVar Equals(Literal l1, Literal l2);

	/// The property Empty for a set variable, encapsulated in a variable
	boolVar Empty(setVar sv);

	/// The property Empty for a set variable
	void XEmpty(setVar sv);

	/// The property Non-Empty for a set variable
	void XNEmpty(setVar sv);

	/// The property of the set of places given by sv being marked, encapsulated in a variable
	boolVar Marked(setVar sv);

	/// The property of the set of places given by sv being marked
	void XMarked(setVar sv);

	/// The property of the set of places given by sv being unmarked
	void XNMarked(setVar sv);

	/// The property of sv1 being a subset (or equal to) sv2, encapsulated in a variable
	boolVar Subset(setVar sv1, setVar sv2);

	/// The property of sv1 being a subset (or equal to) sv2
	void XSubset(setVar sv1, setVar sv2);

	/// The property Siphon for a set variable, encapsulated in a variable
	boolVar Siphon(setVar sv);

	/// The property Siphon for a set variable
	void XSiphon(setVar sv);

	/// The property Trap for a set variable, encapsulated in a variable
	boolVar Trap(setVar sv);

	/// The property Trap for a set variable
	void XTrap(setVar sv);

	/// The property of trap being the maximal trap in sv, encapsulated in a variable
	boolVar MaxTrap(setVar sv, setVar trap);

	/// The property of trap being the maximal trap in sv
	void XMaxTrap(setVar sv, setVar trap);

	/// Minimize or maximize the solution of the formula regarding a set variable
	void MinMax(setVar sv, bool maximize, set<Place*>* enforce=NULL, set<Place*>* forbid=NULL, Place* pgen=NULL);

	/// Verbosity of 1 will allow variable naming
	int verbose;
private:
	/// The formula in CNF
	vector<vector<Literal> > f;

	/// The Petri net the formula works on
	PetriNet& pn;

	/// Number of places in the Petri net
	unsigned int nrofplaces;

	/// The first free variable number (not in f)
	boolVar ffv;

	/// The first free set variable number (not created yet)
	setVar ffsv;

	/// Names of the variables, if set
	map<boolVar,string> vname;

	/// Names of the set variables, if set
	map<setVar,string> svname;

	/// mapper from set variables to their offset in the normal variables
	map<setVar,boolVar> svtov;

	/// Map from places to their ids(offsets)
	map<Place*,unsigned int> p2id;

	/// Map from ids(offsets) to places (starting at 1)
	vector<Place*> id2p;

	/// Copy of MiniSats assignment after the formula has been solved
	vector<bool> mssolution;

	/// If the formula was satisfiable
	bool satisfied;
};

#endif

