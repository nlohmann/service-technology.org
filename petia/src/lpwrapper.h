// -*- C++ -*-

/*!
 * \file    lpwrapper.h
 *
 * \brief   Class LPWrapper
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2011/07/13
 *
 * \date    $Date: 2011-07-13 14:47:01 +0200 (Mi, 13. Jul 2011) $
 *
 * \version $Revision: 1.0 $
 */

#ifndef LPWRAPPER_H
#define LPWRAPPER_H

#include <map>
#include "lp_solve/lp_lib.h"
#include "pnapi/pnapi.h"

using pnapi::Place;
using pnapi::Transition;
using pnapi::Node;
using pnapi::PetriNet;
using std::map;

/*! \brief For wrapping the needed functionality of lp_solve

	This class contains methods for an easy transfer of data from and to lp_solve.
	It specifically supports construction of the marking equation from the Petri net
	and its repeated modification by constraints including a reset capability.
*/
class LPWrapper {

public:

	/// Constructor
	LPWrapper(PetriNet& pn, bool ntype, bool vb);

	/// Destructor
	~LPWrapper();

	/// Create a linear system for the place invariant equation
	int createIEquation();

	/// Remove all constraints not belonging to the place invariant equation
	bool stripConstraints();
/*
	/// Add a new constraint to the linear system
	bool addConstraint(REAL*, int, REAL);
*/
	/// Add a new constraint to the linear system (b=true means p=0, b=false means p>=1)
	bool addConstraint(Node* n, bool b);

	/// Solve the linear system
	int solveSystem();

	/// Get a variable name for a column
	char* getColName(int);

	/// Get the values of the variables in the solution
	unsigned char getVariables(REAL*);

	/// Get a solution vector
	map<Node*,int>& getNVector();

	/// Get the number of calls to solveSystem() so far
	int getCalls() const;

	/// Level of verbosity of lp_solve
	int verbose;

private:
	/// The lp_solve structure
	lprec* lp;

	/// The number of columns (places) in the system of equations
	unsigned int cols;

	/// The number of rows in the system created from the invariant equation
	unsigned int basicrows;

	/// Internal variables for position of places/transitions in the system
	map<Node*,int> npos;

	/// Internal variables for the solution
	map<Node*,int> nvector;

	/// The Petri net for which the equation is computed
	PetriNet& net;

	/// The number of calls to solveSystem() so far
	int solvecall;

	bool nodetype;
};

#endif

