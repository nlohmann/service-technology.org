/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *                            Jan Bretschneider                              *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    containers.h
 *
 * \brief   some containers used at different locations
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 */

#ifndef CONTAINERS_H
#define CONTAINERS_H

#include <set>

class GraphNode;
class State;

typedef std::set<State*> StateSet;

struct compareVertices {
    bool operator()(GraphNode const * left, GraphNode const * right);
};

typedef std::set<GraphNode*, compareVertices> GraphNodeSet;

typedef std::multiset<unsigned int> messageMultiSet;

struct compareMessageMultiSets {
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
}; // compareMessageMultiSets

typedef std::set<messageMultiSet, compareMessageMultiSets> setOfMessages;

#endif
