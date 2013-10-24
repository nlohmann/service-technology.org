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
 \brief CSD automata generation

 Contains all functions needed to generate CSD automata from BSD automata.
*/
class CSD {

    public:

        /*========================================================
         *-------------------- CSD computation -------------------
         *========================================================*/

        static void computeCSD(BSDgraph & graph);

        /*========================================================
         *------------------- maximal b-partner ------------------
         *========================================================*/

        static void computeMaxPartner(parsedGraph & graph);

    private:


};

#define TAU 0
#define BOUND 1
