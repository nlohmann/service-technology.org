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

        void removeEdgesToNodeFromAllOtherNodes(const GraphNode* nodeToDelete);

        void removeEdgesFromNodeToAllOtherNodes(GraphNode* nodeToDelete);

        /**
         * Recursive coordinated dfs through OG and rhs OG.
         * \param currentOGNode the current node of the OG
         * \param currentRhsNode the current node of the rhs OG
         * \param productOG the resulting product OG
         */
        void buildProductOG(GraphNode* currentOGNode,
                            GraphNode* currentRhsNode,
                            AnnotatedGraph* productOG);

        bool simulatesRecursive(GraphNode* myNode, GraphNode* simNode,
            set<pair<GraphNode*, GraphNode*> >& visitedNodes);

        /** 
         * filters the current OG through a given OG below myNode (rhsNode respectively)
         * in such a way, that the complete OG given as the operand simulates the current OG
         * \param myNode a node in the current OGFromFile
         * \param rhsNode a node in the operand
         * \param VisitedNodes a set of Nodes as a reminder of the already visited nodes; starts as empty
         */
        void filterRecursive(GraphNode* myNode,
                             GraphNode* rhsNode,
                             set<GraphNode*>* VisitedNodes);

// CODE FROM PL
        set<std::string> sendEvents;
        set<std::string> recvEvents;
	
        void removeNode(GraphNode*);
        void removeNodesAnnotatedWithTrue();
        void constructDualService();
        void fixDualService();
// END OF CODE FROM PL

    public:

        AnnotatedGraph();

        ~AnnotatedGraph();

        void addNode(GraphNode* node);

        GraphNode* addNode(const std::string& nodeName,
                           GraphFormula* annotation,
                           GraphNodeColor color = BLUE);

        void addEdge(const std::string& srcName,
                           const std::string& dstName,
                           const std::string& label);

        bool hasNodeWithName(const std::string& nodeName) const;

        GraphNode* getRoot() const;

        void setRoot(GraphNode* newRoot);

        void setRootToNodeWithName(const std::string& nodeName);

        GraphNode* getNodeWithName(const std::string& nodeName) const;

        bool hasNoRoot() const;

        /**
         * @todo Remove unreachable nodes after (or during) removal of false
         * nodes.
         */
        void removeFalseNodes();

        /**
         * Returns the product OG of all given OGs. The caller has to delete the
         * returned AnnotatedGraph.
         * \param ogs Given OGs. Must contain at least two OG.
         */
        static AnnotatedGraph* product(const ogs_t& ogs);

        /**
         * Returns the product OG of this OG and the passed one. The caller has to
         * delete the returned AnnotatedGraph.
         */
        AnnotatedGraph* product(const AnnotatedGraph* rhs);

        /**
         * Creates and returns the annotation for the product node of the given two
         * nodes. The caller is responsible for deleting the returned formula.
         */
        GraphFormulaCNF* createProductAnnotation(const GraphNode* lhs,
                                                 const GraphNode* rhs) const;

        /**
         * Produces from the given OG file names the default prefix of the
         * product OG output file.
         */
        static std::string getProductOGFilePrefix(const ogfiles_t& ogfiles);

        /**
         * Strips the OG file suffix from filename and returns the result.
         */
        static std::string stripOGFileSuffix(const std::string& filename);

        void printGraphToDot(GraphNode* v,
                             fstream& os,
                             std::map<GraphNode*, bool>&) const;

        void printDotFile(const std::string& filenamePrefix) const;
        void printDotFile(const std::string& filenamePrefix,
                          const std::string& dotGraphTitle) const;

        /**
         * Prints this OG in OG file format to a file with the given prefix. The
         * suffix is added automatically by this method.
         */
        void printOGFile(const std::string& filenamePrefix) const;

        /**
         * Adds the suffix for OG files to the given file name prefix and returns
         * the result. The suffix includes the . (dot).
         */
        static std::string addOGFileSuffix(const std::string& filePrefix);

        bool simulates(AnnotatedGraph* smallerOG);

        /** 
         * filters the current OG through a given OG in such a way,
         * that the filtered current OG simulates the opernad og; 
         * the current OG is created empty if such a simulation is not possible
         * \param rhsOG the operator OG
         */
        void filter(AnnotatedGraph* rhsOG);

        /**
         * computes the number of Services determined by this OG
         */
        unsigned int numberOfServices();

        unsigned int numberOfServicesRecursively(set<GraphNode*> activeNodes,
                                                 map<GraphNode*, unsigned int>& followers,
                                                 map<GraphNode*, list <set<GraphNode*> > >& validFollowerCombinations,
                                                 map<set<GraphNode*>, unsigned int>& eliminateRedundantCounting,
                                                 unsigned int& instances);

        unsigned int processAssignmentsRecursively(set<string> labels,
                                                   GraphFormulaAssignment possibleAssignment,
                                                   GraphNode* testNode,
                                                   list<GraphFormulaAssignment>& assignmentList);

        //! Tests, if this OG is acyclic
        bool isAcyclic();

        // A function needed for successful deletion of the graph
        void clearNodeSet();

        //! Get all transitions from the graph, each associated to a specific label
        TransitionMap getTransitionMap();
        
        //! Create the formula describing the ooverability criteria 
        GraphFormulaCNF *createCovFormula(TransitionMap tm);

// CODE FROM PL
        void transformToPublicView();
// END OF CODE FROM PL

};

#endif
