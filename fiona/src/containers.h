/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    containers.h
 *
 * \brief   some containers used at different locations
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef CONTAINERS_H
#define CONTAINERS_H

#include <set>

class AnnotatedGraphNode;
class State;

typedef std::set<State*> StateSet;

/// Used as compare criterion for GraphNodeSet.
struct CompareGraphNodes {
    /// compares two graphnodes
    bool operator()(AnnotatedGraphNode const * left, AnnotatedGraphNode const * right);
};


/// In a GraphNodeSet, all GraphNodes with exactly the same states in their
/// reachGraphStateSets are considered equal. That follows from the criterion
/// used by CompareGraphNodes to define a GraphNode* less than another one. This
/// allows for quickly checking whether a newly calculated GraphNode (with a
/// reachGraphStateSet) has the same reachGraphStateSet as an already seen one.
typedef std::set<AnnotatedGraphNode*, CompareGraphNodes> GraphNodeSet;

/// multiset containing messages (input and output events), thus only the index of the corresponding
/// interface place is stored in the multiset 
typedef std::multiset<unsigned int> messageMultiSet;

struct compareMessageMultiSets {
    /// 'left' is less than 'right', if the messageMultiSet of 'left' is
    /// lexicographical less than 'right's messageMultiSet. Consequently,
    /// 'left' and 'right' are equal iff both their messageMultiSet contain
    /// the same states.
    /// @returns true left is less than right.
    /// @returns false left is greater or equal to right.
    bool operator()(messageMultiSet const left, messageMultiSet const right) {
        if (left.size() < right.size()) {
            return true;
        }
        if (left.size() > right.size()) {
            return false;
        }
        for (std::multiset<unsigned int>::iterator s1 = left.begin(); s1
                != left.end(); s1++) {
            for (std::multiset<unsigned int>::iterator s2 = right.begin(); s2
                    != right.end(); s2++) {
                if (*s1 < *s2) {
                    return true;
                }
                if (*s1 > *s2) {
                    return false;
                }
            }
        }
        return false;
    }
}; /// compareMessageMultiSets

/// set of multisets with its respective compare operator
/// for now only the IG uses this set to store the activated events of each node
typedef std::set<messageMultiSet, compareMessageMultiSets> setOfMessages;

#endif
