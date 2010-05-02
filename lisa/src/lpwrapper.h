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
//#include "partialsolution.h"
//#include "problem.h"

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
	
	int calcTInvariant(bool b);

	int calcPInvariant(bool b);

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

	/// internal variables for positions of places in the system and for the solution
	map<Place*,int> ppos, pvector;
	

};

#endif
