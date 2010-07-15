/*****************************************************************************\
 Rachel -- Reparing Service Choreographies

 Copyright (c) 2008, 2009 Niels Lohmann

 Rachel is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Rachel.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef __MATCHING_H
#define __MATCHING_H

#include "EditDistance.h"
#include "types.h"

class ActionScript;
class Graph;


/*!
 * \brief matching
 *
 * All functions are declarated as static values: the class is used for scoping
 * and grouping functions only. No object will be created of this class.
 *
 * The entry point of the class is Matching::matching.
 */
class Matching : EditDistance {
    private:
        /// how often the function Matching::permuteEdges was called
        static unsigned int permuteEdges_calls;

        /// how often the function Matching::calcPermutations was called
        static unsigned int calcPermutations_calls;

        /// how often the function Matching::w was called
        static unsigned int w_calls;

        /// how often the function Matching::matching was called
        static unsigned int matching_calls;

    private:
        /// satisfying asignments in a state q
        static Assignments sat(Graph&, Node);

        /// create a permutation
        static Permutation permuteEdges(const Edges&, const Edges&);

        /// calculates permutations
        static Permutations calcPermutations(Node, Node, const Assignment&);

        /// preprocess OG
        static Value preprocess(Graph&, Node);

        /// helper function for Matching::matching
        static ActionScript w(Node, Node);

        /// weighted matching (helper)
        static Value matching_recursively(Node, Node);

    public:
        /// weighted matching
        static Value matching();
};

#endif
