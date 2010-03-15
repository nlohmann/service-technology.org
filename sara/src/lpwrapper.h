// -*- C++ -*-

/*!
 * \file    lpwrapper.h
 *
 * \brief   Class LPWrapper
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/07
 *
 * \date    $Date: 2009-10-07 14:47:01 +0200 (Mi, 6. Okt 2009) $
 *
 * \version $Revision: -1 $
 */

#ifndef LPWRAPPER_H
#define LPWRAPPER_H

#include <map>
#include "lp_solve/lp_lib.h"
#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "partialsolution.h"
#include "problem.h"

using pnapi::Marking;
using pnapi::Place;
using pnapi::Transition;
using pnapi::PetriNet;
using std::map;



class LPWrapper {

public:

	/// Constructor (int=number of columns)
	LPWrapper(unsigned int);

	/// Destructor
	~LPWrapper();

	/// Create a linear system for the marking equation including cover/bound information
	int createMEquation(Marking& m0, Marking& mf, map<Place*,int>& cover, Problem& pb, bool b);

	/// Remove all constraints not belonging to the marking equation
	bool stripConstraints();

	/// add a new constraint to the linear system
	bool addConstraint(REAL*, int, REAL);

	/// solve the linear system
	int solveSystem();

	/// Get a variable name for a column
	char* getColName(int);

	/// Get the values of the variables in the solution
	unsigned char getVariables(REAL*);

	/// Create linear equations from a partial solution and add them to the system
	bool addConstraints(PartialSolution& ps);

	/// Get a solution vector
	map<Transition*,int>& getTVector(PetriNet& pn);

	/// Find the nearest initial/final marking that makes the marking equation solvable
	bool findNearest();

	/// Level of verbosity of lp_solve
	int verbose;

private:
	/// The lp_solve structure
	lprec* lp;

	/// The number of columns (transitions) in the system of equations
	unsigned int cols;

	/// The number of rows in the system created from the marking equation
	unsigned int basicrows;

	/// internal variables for positions of transitions in the system and for the solution
	map<Transition*,int> tpos, tvector;
};

#endif
