/*****************************************************************************\
 Lisa -- Implementing invariant calculus, state equation, reachability check

 Copyright (c) <<-- 2010 Georg Straube -->>

 Lisa is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Lisa is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Lisa.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef _INVARIANTFINDER_H
#define _INVARIANTFINDER_H

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif

#include "lp_solve/lp_lib.h"
#include "lpwrapper.h"
#include "Output.h"
#include "verbose.h"

/*!
 \brief Calculating T- and P-invariants
  
*/

class InvariantFinder{


public:

	/// Constructor
	InvariantFinder(PetriNet* net, unsigned int);
	
	// Calculate a deterministic order for places and transitions
	bool calcPTOrder();

	// Find t-invariant
	void findTInvariant();

	// Find p-invariant
	void findPInvariant();

private:

	PetriNet* _net;
	LPWrapper lpw;

};
#endif
