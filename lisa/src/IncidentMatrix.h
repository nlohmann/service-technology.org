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

#ifndef _INCIDENTMATRIX_H
#define _INCIDENTMATRIX_H

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif

#include <vector>
#include <math.h>


using pnapi::PetriNet;
using pnapi::Transition;
using pnapi::Place;
using pnapi::Marking;
using std::set;
using std::map;
using std::vector;

/*!
 \brief Implementation of incident matrices
  
*/

class IncidentMatrix {

public:
	/// Constructor - initializes matrix and sets underlying petri net
	IncidentMatrix(PetriNet& net);

	/// Calculates incident matrix
	void calculateMatrix();

	// Getter for underlying petri net
	PetriNet& getPetriNet();

	/// Set petri net
	void setPetriNet(PetriNet& net);

	/// Prints out the incidence matrix
	void printMatrix();

private:
	PetriNet& _net;
	vector<vector<int> > _matrix;
	int _columnsCount;
	int _rowsCount;


};

#endif
