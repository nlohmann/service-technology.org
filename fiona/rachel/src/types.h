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

/*!
 * \file types.h
 * \brief User-defined type definitions for graphs and formulas
 */

#ifndef __TYPES_H
#define __TYPES_H

#include <string>
#include <vector>
#include <algorithm>
#include <set>

class Edge;


/// different actions for edges
enum action_type {
    NONE,       ///< no action determined yet (this is not "keep"!)
    INSERT,     ///< insert edge
    DELETE,     ///< delete egde
    MODIFY,     ///< modify edge
    KEEP        ///< keep edge
};

/// values for similarities
typedef double Value;

/// a graph's node
typedef unsigned int Node;

/// container for graph's nodes
typedef std::vector<Node> Nodes;

/// an edge's label
typedef std::string Label;

/// container for edge's labels
typedef std::vector<Label> Labels;

/// a formula's assignment
typedef std::vector<Edge> Assignment;

/// container for formula's assignments
typedef std::vector<Assignment> Assignments;

/// container for graph's edges
typedef std::vector<Edge> Edges;

/// a pair of graph's edges
typedef std::pair<Edge, Edge> EdgePair;

/// an edge permutations
typedef std::set<EdgePair> Permutation;

/// container for edge permutations
typedef std::set<Permutation> Permutations;


#endif
