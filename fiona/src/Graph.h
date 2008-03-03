/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe, Leonard Kern*
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
 * \file    Graph.h
 *
 * \brief   Holds the graph structure of OGs etc.. In this class there are no
 *          Annotations in the Graph or in its nodes. Use AnnotatedGraph for
 *          this purpose
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
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
    
    protected:
        GraphNode* root;

        typedef std::vector<GraphNode*> nodes_t;

        nodes_t setOfNodes; // needed for proper deletion of OG.

        nodes_t finalNodes; // final nodes of the graph

        typedef nodes_t::const_iterator nodes_const_iterator;

        typedef nodes_t::iterator nodes_iterator;

        /// remove all edges that have a given node as destination
        void removeEdgesToNodeFromAllOtherNodes(const GraphNode* nodeToDelete);

        /// remove all edges that have a given node as source
        void removeEdgesFromNodeToAllOtherNodes(GraphNode* nodeToDelete);
        
        /// transforms the service automaton to an OWFN recursively
        void Graph::transformToOWFNRecursively(GraphNode* currentNode,
        		   							   set<GraphNode*>& visitedNodes,	
        		   							   set<string>& finalNodeNames,
        		   							   PNapi::PetriNet* PN, // The resulting petri net
        		   							   unsigned int& transitionNumber, // The current transition number
        		   							   PNapi::Transition* incomingTransition // The transition that leads to this node
        		   							   );  

    public:

        /// basic constructor
        Graph();

        /// basic deconstructor
        ~Graph();

        /// adds a node to the graph
        void addNode(GraphNode* node);

        /// makes an already added node a final node
        void makeNodeFinal(GraphNode* node);

        /// creates a new node in the graph
        GraphNode* addNode(const std::string& nodeName,
                           GraphNodeColor color = BLUE);

        /// creates a new edge in the graph
        void addEdge(const std::string& srcName,
                     const std::string& dstName,
                     const std::string& label);

        /// returns true if a node with the given name was found
        bool hasNodeWithName(const std::string& nodeName) const;

        /// retruns a node with the given name, or NULL else
        GraphNode* getNodeWithName(const std::string& nodeName) const;

        /// returns a pointer to the root node
        GraphNode* getRoot() const;

        /// sets a new root node
        void setRoot(GraphNode* newRoot);

        /// sets the root node to the one with the given name
        void setRootToNodeWithName(const std::string& nodeName);

        /// retruns true if the graph's root node is NULL
        bool hasNoRoot() const;

        /// dfs through the graph printing each node and edge to the output stream
        void printGraphToDot(GraphNode* v,
                             fstream& os,
                             std::map<GraphNode*, bool>&) const;

        /// creates a dot output of the graph and calls dot to create an image from it
        void printDotFile(const std::string& filenamePrefix) const;

        /// creates a dot output of the graph and calls dot to create an image from it
        void printDotFile(const std::string& filenamePrefix,
                          const std::string& dotGraphTitle) const;
    
        /// tests if this OG is acyclic
        bool isAcyclic();

        /// transforms this service automaton into an owfn
        void transformToOWFN(PNapi::PetriNet* PN);

        /// a function needed for successful deletion of the graph
        void clearNodeSet();

        /// a function that returns the number of nodes in the graph
        unsigned int numberOfNodes();

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(Graph)
#define new NEW_NEW
};

#endif
