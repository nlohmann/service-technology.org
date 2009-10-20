/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008, 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
 Rachel is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Rachel (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

#ifndef __GRAPH_H
#define __GRAPH_H

#include <map>
#include <string>
#include <set>
#include <vector>
#include "types.h"
#include "Formula.h"


/***********
 * classes *
 ***********/

/// a labeled graph edge
/// \todo make stuff private
class Edge {
    public:
        /// the source of the edge
        Node source;

        /// the target of the edge
        Node target;

        /// the label of the edge
        Label label;

        /// the type of the edge (for edit distance)
        action_type type;

    public:
        bool operator == (const Edge&) const;
        bool operator <  (const Edge&) const;

        Edge();
        Edge(Node, Node, const Label&);
};


/// a directed graph
/// \todo let OG and IG derive from here
class Graph {
    private:
        /// edges indexed by source node
        std::map<Node, Edges> edges;

        /// the root node
        Node root;

        /// the final nodes (IG only)
        std::map<Node, bool> finalNode;

        /// the node with the maximal value
        Node max_value;

        /// value of the rest of the OG if inserted
        std::map<Node, Value> insertionValue;

        /// value of the rest of the service automaton if deleted
        std::map<Node, Value> deletionValue;

        /// the node formulas
        std::map<Node, Formula*> formulas;


    private:
        /// preprocess the graph with insertion values (helper)
        Value preprocessInsertionRecursively(Node);

        /// preprocess the graph with deletion values (helper)
        Value preprocessDeletionRecursively(Node);

        /// calculate the number of characterized services (helper)
        long double countServicesRecursively(Node);

        /// check if graph is cyclic (helper)
        bool isCyclicRecursively(Node);

        /// returns (at most 1) successor of a node with a given label
        Edge successor(Node, const Label&);

        /// returns all satisfying label vectors
        std::vector<Labels> checkSat(Node);

        /// writes a dot annotation of the graph
        void toDot(std::ostream&, bool = true);


    public:
        /// identifier string
        std::string id;

        /// the nodes
        Nodes nodes;

    public:
        /// returns true iff the node's annotation is fulfilled by "final"
        bool isFinal(Node);

        /// which nodes where added during matching?
        std::map<Node, bool> addedNodes;

        /// returns outgoing edges of a node
        Edges outEdges(Node);

        /// add a node to the graph
        Node addNode(Node);

        /// add a new node and organize edges
        Node addNewNode(Node, const Label&);

        /// add an edge to the graph
        Edge addEdge(Node, Node, const Label&, action_type = NONE);

        /// sets the root node
        void setRoot(Node);

        /// adds this node to the final nodes (IG only)
        void setFinal(Node);

        /// gets the root node
        Node getRoot() const;

        /// add a formula to a node
        void addFormula(Node, Formula*);

        /// returns all combinations of edges that satisfy a node's annotation
        Assignments sat(Node);

        /// writes a Dot file to disk
        void createDotFile();

        /// preprocess the graph with insertion values (for OGs)
        void preprocessInsertion();

        /// preprocess the graph with deletion values (for service automata)
        void preprocessDeletion();

        /// calculate the number of characterized services
        long double countServices();

        /// return insertion value (for OGs)
        Value getInsertionValue(Node);

        /// return deletion value (for service automata)
        Value getDeletionValue(Node);

        /// check if graph is cyclic
        bool isCyclic();

        /// coherently re-enumerate the graph's nodes
        void reenumerate();

        /// return the average size of the satisfying assignments
        double averageSatSize();

        explicit Graph(const std::string&);
};

#endif
