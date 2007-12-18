/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg               *
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

#ifndef COMMUNICATIONGRAPH_H_
#define COMMUNICATIONGRAPH_H_

#include "mynew.h"
#include "AnnotatedGraphEdge.h"
#include "GraphNode.h" // for GraphNodeDiagnosisColor_enum
#include "AnnotatedGraphNode.h"
#include "AnnotatedGraph.h" // parent class
#include "owfn.h"
#include <fstream>
#include <set>

using namespace std;


extern double global_progress;
extern int show_progress;

/* communication graph */
class CommunicationGraph : public AnnotatedGraph {
    private:

        /// Computes the total number of all states stored in all nodes and the
        /// number of all edges in this graph.
        void computeNumberOfStatesAndEdges();

        /// Helps computeNumberOfStatesAndEdges to computes the total number of all
        /// states stored in all nodes and the number of all edges in this graph.
        void computeNumberOfStatesAndEdgesHelper(AnnotatedGraphNode* v,
                                                 std::map<AnnotatedGraphNode*, bool>& visitedNodes);

        /// Computes the number of all blue to be shown nodes and edges in this
        /// graph.
        void computeNumberOfBlueNodesEdges();

        /// Helps computeNumberOfBlueNodesEdges() to computes the number of all blue
        /// to be shown nodes and edges in this graph.
        void computeNumberOfBlueNodesEdgesHelper(AnnotatedGraphNode* v, std::map<AnnotatedGraphNode*, bool>& visitedNodes);

        /// The total number of all states stored in all nodes in this graph.
        /// Is computed by computeNumberOfStatesAndEdges().
        unsigned int nStoredStates;

        /// The number of all edges in this graph.
        /// Is computed by computeNumberOfStatesAndEdges().
        unsigned int nEdges;

        /// The number of blue to be shown nodes in this graph.
        /// Is computed by computeNumberOfBlueNodesEdges().
        unsigned int nBlueNodes;

        /// The number of blue to be shown edges in this graph.
        /// Is computed by computeNumberOfBlueNodesEdges().
        unsigned int nBlueEdges;

    protected:
        oWFN* PN; //!< pointer to the underlying petri net

        void addProgress(double);

    public:

        /// basic constructor
        CommunicationGraph(oWFN *);
        
        /// basic deconstructor
        ~CommunicationGraph();

        GraphNodeSet setOfSortedNodes;

        /// this set contains all states of the newly calculated node
        StateSet setOfStatesStubbornTemp; 
        
        /// calculate a root node for the graph
        void calculateRootNode();

        /// Returns the number of nodes in this graph.
        unsigned int getNumberOfNodes() const;

        /// Returns the total number of all states stored in all nodes in this
        /// graph. May only be called after computeGraphStatistics().
        unsigned int getNumberOfStoredStates() const;

        /// Returns the number of all edges in this graph. May only be called after
        /// computeGraphStatistics().
        unsigned int getNumberOfEdges() const;

        /// Returns the number of all blue to be shown nodes in this graph. May only
        /// be called after computeGraphStatistics().
        unsigned int getNumberOfBlueNodes() const;

        /// Returns the number of all blue to be shown edges in this graph. May only
        /// be called after computeGraphStatistics().
        unsigned int getNumberOfBlueEdges() const;

        /// checks if the given node is in the graphs node set
        AnnotatedGraphNode* findGraphNodeInSet(AnnotatedGraphNode*);

        /// prints the current global progress value depending whether the value
        /// changed significantly and depending on the debug-level set
        void printProgressFirst();

        /// prints the current global progress value depending whether the value
        /// changed significantly and depending on the debug-level set
        void printProgress();

        void buildGraphRandom();
        void analyseNode(AnnotatedGraphNode*);

        /// brief creates a dot file of the graph
        void printGraphToDot();

        /// recursive helper function for printGraphToDot()
        void printGraphToDotRecursively(AnnotatedGraphNode* v,
                                        fstream& os,
                                        std::map<AnnotatedGraphNode*, bool>&);

        /// function to create an STG representation of the IG or OG
        void printGraphToSTG();

        /// recursive helper function for printGraphToSTG()
        void printGraphToSTGRecursively(AnnotatedGraphNode* v,
                                        fstream& os,
                                        std::map<AnnotatedGraphNode*, bool>&);

        /// function to annotate the OG for distributed controllability
        bool annotateGraphDistributedly();

        /// recursive helper function for annotateGraphDistributedly()
        bool annotateGraphDistributedlyRecursively(AnnotatedGraphNode* v,
                                                   std::map<AnnotatedGraphNode*, bool>&);
        /// helper function for annotateGraphDistributedly()
        void removeLabeledSuccessor(AnnotatedGraphNode* v, std::string label);

        /// gives a diagnosis of the graph
        void diagnose();

        /// recursive helper function for diagnose()
        GraphNodeDiagnosisColor_enum diagnose_recursively(AnnotatedGraphNode* v,
                                                          std::map<AnnotatedGraphNode*, bool>& visitedNodes);

        /// Computes statistics about this graph
        void computeGraphStatistics();

        /// Prints statistics about this graph. May only be called after
        /// computeGraphStatistics().
        void printGraphStatistics();

        /// checks wethe a state activates output events
        bool stateActivatesOutputEvents(State*);

        /// returns the oWFN Filename this graph was created from
        string returnOWFnFilename();

        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(CommunicationGraph)
#define new NEW_NEW
};

#endif /*GRAPH_H_*/
