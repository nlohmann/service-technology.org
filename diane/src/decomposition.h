/*****************************************************************************\
 Diane -- Decomposition of Petri nets.

 Copyright (C) 2009  Stephan Mennicke <stephan.mennicke@uni-rostock.de>

 Diane is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Diane is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Diane.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#ifndef DECOMPOSITION_H
#define DECOMPOSITION_H

#include <map>
#include <vector>
#include "pnapi/petrinet.h"

using pnapi::Node;
using pnapi::PetriNet;
using std::map;
using std::vector;


namespace decomposition {

    /// makes a singleton set of the given element
    void MakeSet(int, int*);

    /// makes a union of both given parameters
    void Union(int, int, int*);

    /// finds the parent of the given parameter
    int Find(int, int*);


    int computeComponentsByUnionFind(PetriNet&, int*, int, int, map<int, Node*> &);

    void createOpenNetComponentsByUnionFind(vector<PetriNet*> &, int*, int, int, map<int, Node*> &);

} /* namespace decomposition */

#endif /* DECOMPOSITION_H */
