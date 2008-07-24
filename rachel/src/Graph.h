/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
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


#include <string>
#include <map>
#include <vector>

#include "Formula.h"
#include "Action.h"


/******************************
 * class forward delcarations *
 ******************************/

class Edge;


/************
 * typedefs *
 ************/

typedef unsigned int Node;
typedef std::vector<Node> Nodes;
typedef std::string Label;
typedef std::vector<Label> Labels;
typedef std::vector<Edge> Assignment;
typedef std::vector<Assignment> Assignments;
typedef double Value;
typedef std::vector<Edge> Edges;


/***********
 * classes *
 ***********/

/// a labeled graph edge
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
        bool operator == (const Edge &e) const;
        bool operator <  (const Edge &e) const;
        
        Edge();
        Edge(Node mysource, Node mytarget, const Label mylabel);
};


/// a directed graph
class Graph {
    private:
        /// edges indexed by source node
        std::map<Node, Edges> edges;
    
        /// the root node
        Node root;
    
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
        Value preprocessInsertionRecursively(Node q);
    
        /// preprocess the graph with deletion values (helper)
        Value preprocessDeletionRecursively(Node q);

        /// calculate the number of characterized services (helper)
        long double countServicesRecursively(Node q);
    
        /// check if graph is cyclic (helper)
        bool isCyclicRecursively(Node q);
    
        /// returns (at most 1) successor of a node with a given label
        Edge successor(Node q, const Label &l);
        
    public:
        /// identifier string
        std::string id;

        /// the nodes
        Nodes nodes;
    
    public:
        /// which nodes where added during matching?
        std::map<Node, bool> addedNodes;
    
        /// returns outgoing edges of a node
        Edges outEdges(Node q);
        
        /// add a node to the graph
        Node addNode(Node q);
    
        /// add a new node and organize edges
        Node addNewNode(Node q, const Label l);
    
        /// add an edge to the graph
        Edge addEdge(Node q1, Node q2, const Label l, action_type type = NONE);
    
        /// sets the root node
        void setRoot(Node q);
    
        /// gets the root node
        Node getRoot() const;
        
        /// add a formula to a node
        void addFormula(Node q, Formula *f);

        /// returns all combinations of edges that satisfy a node's annotation
        Assignments sat(Node q);

        /// returns a dot annotation of the graph
        std::string toDot();

        /// returns all satisfying label vectors
        std::vector<Labels> checkSat(Node q);

        /// returns true iff the node's annotation is fulfilled by "final"
        bool isFinal(Node q);

        /// preprocess the graph with insertion values (for OGs)
        void preprocessInsertion();

        /// preprocess the graph with deletion values (for service automata)
        void preprocessDeletion();

        /// calculate the number of characterized services
        long double countServices();

        /// return insertion value (for OGs)
        Value getInsertionValue(Node q);

        /// return deletion value (for service automata)
        Value getDeletionValue(Node q);

        /// check if graph is cyclic
        bool isCyclic();

        /// coherently re-enumerate the graph's nodes
        void reenumerate();
    
        /// return the average size of the satisfying assignments
        double averageSatSize();
    
        // a hack to get the needed numbers for Luhme
        void printStatisticsForMarkings();
    
        Graph(string const id);
};

#endif
