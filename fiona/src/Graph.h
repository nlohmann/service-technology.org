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
 * \file    Graph.h
 *
 * \brief   Holds the graph structure of OGs etc.. In this class there are no
 *          Annotations in the Graph or in its nodes. Use AnnotatedGraph for
 *          this purpose
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <fstream>
#include <string>
#include <set>
#include <utility>
#include <map>
#include <list>
#include "GraphNode.h"

using namespace std;


class Graph {

    public:
        typedef std::vector<GraphNode*> nodes_t;

        typedef std::map< GraphNode*, GraphNode::LeavingEdges > predecessorMap;

    protected:
        GraphNode* root;

        nodes_t setOfNodes; // needed for proper deletion of OG.

        typedef nodes_t::const_iterator nodes_const_iterator;

        typedef nodes_t::iterator nodes_iterator;

        /// remove all edges that have a given node as destination
        void removeEdgesToNodeFromAllOtherNodes(const GraphNode* nodeToDelete);

        /// remove all edges that have a given node as source
        void removeEdgesFromNodeToAllOtherNodes(GraphNode* nodeToDelete);
        
        /// transforms the service automaton to an OWFN recursively
        void transformToOWFNRecursively(GraphNode* currentNode,
                                        set<GraphNode*>& visitedNodes,
                                        set<string>& finalNodeNames,
                                        PNapi::PetriNet* PN, // The resulting petri net
                                        unsigned int& transitionNumber, // The current transition number
                                        PNapi::Transition* incomingTransition // The transition that leads to this node
        );


        /// Recursive helper for createDotFile; runs a dfs through the graph printing each node and edge to the output stream
        void createDotFileRecursively(GraphNode* v,
                                      fstream& os,
                                      std::map<GraphNode*, bool>&) const;
    public:

        /// basic constructor
        Graph();

        /// basic deconstructor
        virtual ~Graph();

        /// adds a node to the graph
        void addNode(GraphNode* node);

        /// creates a new node in the graph
        GraphNode* addNode(const string& nodeName, GraphNodeColor color = BLUE);

        /// creates a new edge in the graph
        void addEdge(const string& srcName,
                     const string& dstName,
                     const string& label);

        /// returns true if a node with the given name was found
        bool hasNodeWithName(const string& nodeName) const;

        /// retruns a node with the given name, or NULL else
        GraphNode* getNodeWithName(const string& nodeName) const;

        /// returns a pointer to the root node
        GraphNode* getRoot() const;

        /// sets a new root node
        void setRoot(GraphNode* newRoot);

        /// sets the root node to the one with the given name
        void setRootToNodeWithName(const string& nodeName);

        /// retruns true if the graph's root node is NULL
        bool hasNoRoot() const;



        /// Creates a dot output (.out) of the graph, using the filename as title.
        virtual string createDotFile(string& filenamePrefix) const;

        /// creates a dot output (.out) of the graph, using a specified title.
        virtual string createDotFile(string& filenamePrefix,
                                     const string& dotGraphTitle) const;

		/// calls dot to create an image file (.png) of a given dot file (.out)
		virtual string createPNGFile(string& filenamePrefix,
                         		     string& dotFileName) const;

		/// tests if this OG is acyclic
        bool isAcyclic();

        /// transforms this service automaton into an owfn. In case that there
        /// existed events which do not appear in the graph's transitions anymore
        /// those events need to be given in order to keep the same interface for
        /// the public view
        void transformToOWFN(PNapi::PetriNet* PN, set<string>& setOfInputs, set<string>& setOfOutputs);

        /// a function needed for successful deletion of the graph
        void clearNodeSet();

        /// a function that returns the number of nodes in the graph
        unsigned int numberOfNodes();

        /// function that returns a map from nodes to a vector of nodes
        /// so that every node in the vector is a predecessor of the
        /// corresponding node
        void getPredecessorRelation(Graph::predecessorMap& resultMap);

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(Graph)
#define new NEW_NEW
};

#endif
