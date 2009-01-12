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
        bool operator == (const Edge &e) const;
        bool operator <  (const Edge &e) const;
        
        Edge();
        Edge(Node mysource, Node mytarget, const Label mylabel);
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
        
        /// bit representation of the formulas
        std::map<Node, FormulaBits> formulaBits;
    
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
                
        /// returns the outgoing receive labels of a node
        std::set<Label> receiveLabels(Node q);

        /// returns the outgoing send labels of a node
        std::set<Label> sendLabels(Node q);
        
        /// returns all satisfying label vectors
        std::vector<Labels> checkSat(Node q);

        
    public:
        /// identifier string
        std::string id;

        /// the nodes
        Nodes nodes;
        
    public:
        /// returns true iff the node's annotation is fulfilled by "final"
        bool isFinal(Node q);

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
        
        /// adds this node to the final nodes (IG only)
        void setFinal(Node q);
    
        /// gets the root node
        Node getRoot() const;
        
        /// add a formula to a node
        void addFormula(Node q, Formula *f);

        /// returns all combinations of edges that satisfy a node's annotation
        Assignments sat(Node q);

        /// returns a dot annotation of the graph
        std::string toDot();

        /// returns a dot annotation of the graph with formula bits
        std::string toDotAnnotated(bool reduced);

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
    
        /// calculate a compact represenation of the OG's formulae
        void calculateCompactAnnotations();
    
        /// BPMN output of a service automaton
        void bpmnOutput();
    
        Graph(const std::string id);
};

#endif
