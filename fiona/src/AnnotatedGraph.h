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
 * \file    AnnotatedGraph.h
 *
 * \brief   Holds an Operating Guidelines (OG) read from a OG file. We do not
 *          use the class OG for storing OGs read from a file because the class
 *          OG is tailored for computing an OG and therefore heavily depends
 *          on the existence of an underlying oWFN. If we used the class OG for
 *          storing an already computed OG, we would have to reimplement most
 *          methods anyway and the possibility of reuse would be minimal.
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


class AnnotatedGraph {
    public:

        /// Type of container passed to AnnotatedGraph::product().
        typedef std::list<AnnotatedGraph*> ogs_t;

        /// Type of container passed to AnnotatedGraph::getProductOGFilePrefix().
        typedef std::list<std::string> ogfiles_t;

        typedef set<std::string> EdgeSet;
        
        /// Type of container of transitions associated to each label
        typedef map<string, EdgeSet> TransitionMap;

    protected:
        GraphNode* root;

        typedef std::vector<GraphNode*> nodes_t;

        nodes_t setOfNodes; // needed for proper deletion of OG.

        typedef nodes_t::const_iterator nodes_const_iterator;

        typedef nodes_t::iterator nodes_iterator;

        /// remove all edges that have a given node as destination
        void removeEdgesToNodeFromAllOtherNodes(const GraphNode* nodeToDelete);

        /// remove all edges that have a given node as source
        void removeEdgesFromNodeToAllOtherNodes(GraphNode* nodeToDelete);

        /// Recursive coordinated dfs through OG and rhs OG.
        void buildProductOG(GraphNode* currentOGNode,
                            GraphNode* currentRhsNode,
                            AnnotatedGraph* productOG);

        /// checks, whether the part of an AnnotatedGraph below myNode simulates
        /// the part of an AnnotatedGraph below simNode
        bool simulatesRecursive(GraphNode* myNode, GraphNode* simNode,
            set<pair<GraphNode*, GraphNode*> >& visitedNodes);

        /// filters the current OG through a given OG below myNode (rhsNode respectively)
        /// in such a way, that the complete OG given as the operand simulates the current OG
        void filterRecursive(GraphNode* myNode,
                             GraphNode* rhsNode,
                             set<GraphNode*>* VisitedNodes);

// CODE FROM PL
        set<std::string> sendEvents;
        set<std::string> recvEvents;
	
        /// remove a node from the annotated graph
        void removeNode(GraphNode*);
        
        /// remove all nodes that have the annotation "true"
        void removeNodesAnnotatedWithTrue();
        
        /// constructs the dual service
        void constructDualService();
        
        /// applies 1st and 2nd fix to dual service
        void fixDualService();
// END OF CODE FROM PL

    public:

        /// basic constructor
        AnnotatedGraph();

        /// basic deconstructor
        ~AnnotatedGraph();

        /// adds a node to the graph
        void addNode(GraphNode* node);

        /// creates a new node in the graph
        GraphNode* addNode(const std::string& nodeName,
                           GraphFormula* annotation,
                           GraphNodeColor color = BLUE);

        /// creates a new edge in the graph
        void addEdge(const std::string& srcName,
                           const std::string& dstName,
                           const std::string& label);

        /// returns true if a node with the given name was found
        bool hasNodeWithName(const std::string& nodeName) const;

        /// returns a pointer to the root node
        GraphNode* getRoot() const;

        /// sets a new root node
        void setRoot(GraphNode* newRoot);

        /// sets the root node to the one with the given name
        void setRootToNodeWithName(const std::string& nodeName);

        /// retruns a node with the given name, or NULL else
        GraphNode* getNodeWithName(const std::string& nodeName) const;

        /// retruns true if the graph's root node is NULL
        bool hasNoRoot() const;

        /// removes all nodes that are always false
        void removeFalseNodes();

        /// Returns the product OG of all given OGs.
        static AnnotatedGraph* product(const ogs_t& ogs);

        /// Returns the product OG of this OG and the passed one
        AnnotatedGraph* product(const AnnotatedGraph* rhs);

        /// Creates and returns the annotation for the product node of the given two nodes. 
        GraphFormulaCNF* createProductAnnotation(const GraphNode* lhs,
                                                 const GraphNode* rhs) const;

        /// Produces the default prefix of the product OG output file.
        static std::string getProductOGFilePrefix(const ogfiles_t& ogfiles);

        /// Strips the OG file suffix from filename and returns the result.
        static std::string stripOGFileSuffix(const std::string& filename);

        /// dfs through the graph printing each node and edge to the output stream
        void printGraphToDot(GraphNode* v,
                             fstream& os,
                             std::map<GraphNode*, bool>&) const;

        /// creates a dot output of the graph and calls dot to create an image from it
        void printDotFile(const std::string& filenamePrefix) const;

        /// creates a dot output of the graph and calls dot to create an image from it
        void printDotFile(const std::string& filenamePrefix,
                          const std::string& dotGraphTitle) const;

        
        /// Prints this OG in OG file format 
        void printOGFile(const std::string& filenamePrefix) const;

        /// Adds the suffix for OG files to the given file name prefix 
        static std::string addOGFileSuffix(const std::string& filePrefix);

        /// checks, whether this AnnotatedGraph simulates the given simulant
        bool simulates(AnnotatedGraph* smallerOG);

        /// filters the current OG through a given OG in such a way,
        /// that the filtered current OG simulates the opernad og; 
        void filter(AnnotatedGraph* rhsOG);

        /// computes the number of Services determined by this OG
        unsigned int numberOfServices();

        /// recursive function for computing the number of services
        unsigned int numberOfServicesRecursively(set<GraphNode*> activeNodes,
                                                 map<GraphNode*, unsigned int>& followers,
                                                 map<GraphNode*, list <set<GraphNode*> > >& validFollowerCombinations,
                                                 map<set<GraphNode*>, unsigned int>& eliminateRedundantCounting,
                                                 unsigned int& instances);

        /// recursive function for computing the number of true assignments for a node
        unsigned int processAssignmentsRecursively(set<string> labels,
                                                   GraphFormulaAssignment possibleAssignment,
                                                   GraphNode* testNode,
                                                   list<GraphFormulaAssignment>& assignmentList);

        /// tests if this OG is acyclic
        bool isAcyclic();

        /// a function needed for successful deletion of the graph
        void clearNodeSet();

        /// Get all transitions from the graph, each associated to a specific label
        TransitionMap getTransitionMap();
        
        /// Create the formula describing the coverability criteria 
        GraphFormulaCNF *createCovFormula(TransitionMap tm);

// CODE FROM PL
        /// transforms the graph to the public view
        void transformToPublicView();
// END OF CODE FROM PL

};

#endif
