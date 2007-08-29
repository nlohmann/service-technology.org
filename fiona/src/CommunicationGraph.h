/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
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
 * \file    CommunicationGraph.h
 *
 * \brief   common functions for IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include "mynew.h"
#include "GraphEdge.h"
#include "GraphNode.h" // for GraphNodeDiagnosisColor_enum
#include "owfn.h"
#include <fstream>

#include <set>

using namespace std;

class oWFN;
class State;

extern double global_progress;
extern int show_progress;

/* communication graph */
class CommunicationGraph {
    private:

        /**
         * Computes the total number of all states stored in all nodes and the
         * number of all edges in this graph.
         */
        void computeNumberOfStatesAndEdges();

        /**
         * Helps computeNumberOfStatesAndEdges to computes the total number of all
         * states stored in all nodes and the number of all edges in this graph.
         * This is done recursively (dfs).
         * @param v Current node in the iteration process.
         * @param visitedNodes[] Array of bool storing the nodes that we have
         *     already looked at.
         */
        void computeNumberOfStatesAndEdgesHelper(GraphNodeCommon<GraphNode>* v,
                                                 std::map<GraphNodeCommon<GraphNode>*, bool>& visitedNodes);

        /**
         * Computes the number of all blue to be shown nodes and edges in this
         * graph.
         */
        void computeNumberOfBlueNodesEdges();

        /**
         * Helps computeNumberOfStatesAndEdges() to computes the number of all blue
         * to be shown nodes and edges in this graph.
         * This is done recursively (dfs).
         * @param v Current node in the iteration process.
         * @param visitedNodes[] Array of bool storing the nodes that we have
         *     already looked at.
         */
        void computeNumberOfBlueNodesEdgesHelper(GraphNodeCommon<GraphNode>* v, std::map<GraphNodeCommon<GraphNode>*, bool>& visitedNodes);

        /**
         * The total number of all states stored in all nodes in this graph.
         * Is computed by computeNumberOfStatesAndEdges().
         */
        unsigned int nStoredStates;

        /**
         * The number of all edges in this graph.
         * Is computed by computeNumberOfStatesAndEdges().
         */
        unsigned int nEdges;

        /**
         * The number of blue to be shown nodes in this graph.
         * Is computed by computeNumberOfBlueNodesEdges().
         */
        unsigned int nBlueNodes;

        /**
         * The number of blue to be shown edges in this graph.
         * Is computed by computeNumberOfBlueNodesEdges().
         */
        unsigned int nBlueEdges;

    protected:
        oWFN* PN; //!< pointer to the underlying petri net

        GraphNodeCommon<GraphNode>* root; //!< the root node of the graph

        void addProgress(double);

    public:

        CommunicationGraph(oWFN *);
        ~CommunicationGraph();

        GraphNodeSet setOfVertices;

        GraphNodeCommon<GraphNode>* getRoot() const;

        void calculateRootNode();

        /**
         * Returns the number of nodes in this graph.
         */
        unsigned int getNumberOfNodes() const;

        /**
         * Returns the total number of all states stored in all nodes in this
         * graph. May only be called after computeGraphStatistics().
         */
        unsigned int getNumberOfStoredStates() const;

        /**
         * Returns the number of all edges in this graph. May only be called after
         * computeGraphStatistics().
         */
        unsigned int getNumberOfEdges() const;

        /**
         * Returns the number of all blue to be shown nodes in this graph. May only
         * be called after computeGraphStatistics().
         */
        unsigned int getNumberOfBlueNodes() const;

        /**
         * Returns the number of all blue to be shown edges in this graph. May only
         * be called after computeGraphStatistics().
         */
        unsigned int getNumberOfBlueEdges() const;

        GraphNodeCommon<GraphNode>* findGraphNodeInSet(GraphNodeCommon<GraphNode>*);

        void analyseNode(GraphNodeCommon<GraphNode>*);

        void printProgressFirst();
        void printProgress();

        void buildGraphRandom();

        void printGraphToDot();
        void printGraphToDotRecursively(GraphNodeCommon<GraphNode>* v,
                                        fstream& os,
                                        std::map<GraphNodeCommon<GraphNode>*, bool>&);

        // functions to create an STG representation of the IG or OG
        void printGraphToSTG();
        void printGraphToSTGRecursively(GraphNodeCommon<GraphNode>* v,
                                        fstream& os,
                                        std::map<GraphNodeCommon<GraphNode>*, bool>&);

        // functions to annotate the OG for distributed controllability
        bool annotateGraphDistributedly();
        bool annotateGraphDistributedlyRecursively(GraphNodeCommon<GraphNode>* v,
                                                   std::map<GraphNodeCommon<GraphNode>*, bool>&);
        void removeLabeledSuccessor(GraphNodeCommon<GraphNode>* v, std::string label);

        // functions for diagnosis
        void diagnose();
        GraphNodeDiagnosisColor_enum diagnose_recursively(GraphNodeCommon<GraphNode>* v,
                                                          std::map<GraphNodeCommon<GraphNode>*, bool>& visitedNodes);

        /**
         * Computes statistics about this graph. They can be printed by
         * printGraphStatistics().
         */
        void computeGraphStatistics();

        /**
         * Prints statistics about this graph. May only be called after
         * computeGraphStatistics().
         */
        void printGraphStatistics();

        bool stateActivatesOutputEvents(State*);

        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(CommunicationGraph)
#define new NEW_NEW
};

#endif /*GRAPH_H_*/
