/*****************************************************************************\
 BSD -- generating BSD automata

 Copyright (c) 2013 Simon Heiden

 BSD is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 BSD is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with BSD.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#pragma once

#include <pnapi/pnapi.h>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include "types.h"


/*!
 \brief b-conformance check

 Conains all functions related to b-conformance checking.

*/
class confcheck {

    public:

        /*========================================================
         *------------------- b-conformance check ----------------
         *========================================================*/

        static bool check_b_conformance(parsedGraph & graph1, parsedGraph & graph2);

        static bool computeBiSimCSD(parsedGraph & graph1, parsedGraph & graph2, unsigned int node_g1, unsigned int node_g2, std::map<Label_ID, Label_ID> * mapping);

        static std::map<Label_ID, Label_ID>* computeMappingCSD(parsedGraph & graph1, parsedGraph & graph2);

        /*========================================================
         *---------------------- TEST OUTPUT ---------------------
         *========================================================*/

        static void printBiSim(parsedGraph & graph1, parsedGraph & graph2, std::list<unsigned int>* bisim);


    private:

        static std::list<unsigned int>* bisimtemp;

};

