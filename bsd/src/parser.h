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
#include <map>
#include "types.h"


/*!
 \brief dot file parser

 Contains all functions needed to parse BSD and CSD automata from given dot files.
 The format should be like the following:

 There have to be parameters in the comments specifying the bound, the node count and the event count:

 	 bound:             x

 	 nodes:             y

 	 labels (total):    z

 The initial node of the graph is assumed to be the first node parsed. There is a test on correct label
 and node count. However, there is no test on correct graph input, meaning that the parser assumes that
 for each node there is given all z labels. If some labels are missing for some nodes then they are
 obviously also missing in the parsed graph. This may lead to wrong results or segmentation faults.

 The format of the edges should be: "(node).lambda" -> "(node).lambda" [...,label="...",...];
*/
class parser {

    public:

        /*========================================================
         *--------------------- DOT to BSD parser ----------------
         *========================================================*/

        static parsedGraph * dot2graph_parse(std::istream & is);

        static void Tokenize(const std::string& str, std::list<std::string>& tokens, const std::string& delimiters);

        /*========================================================
         *---------------------- TEST OUTPUT ---------------------
         *========================================================*/

        static void printParsedGraph(parsedGraph & graph);

    private:

};
