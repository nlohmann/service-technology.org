/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg               *
 *                                                                           *
 * Copyright 2008                                                            *
 *   Peter Massuthe, Daniela Weinberg                                        *
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
#include "binDecision.h"
#include <fstream>
#include <set>

using namespace std;


extern double global_progress;
extern int show_progress;

/* communication graph */
class CommunicationGraph : public AnnotatedGraph {
    private:

        /// recursive helper function for diagnose()
        GraphNodeDiagnosisColor_enum diagnose_recursively(AnnotatedGraphNode* v,
                                                          std::map<AnnotatedGraphNode*, bool>& visitedNodes);


        /// recursive helper function for printGraphToDot()
        void printGraphToDotRecursively(AnnotatedGraphNode* v,
                                        fstream& os,
                                        std::map<AnnotatedGraphNode*, bool>&);

        /// recursive helper function for printGraphToSTG()
        void printGraphToSTGRecursively(AnnotatedGraphNode* v,
                                        fstream& os,
                                        std::map<AnnotatedGraphNode*, bool>&);

        /// recursive helper function for annotateGraphDistributedly()
        bool annotateGraphDistributedlyRecursively(AnnotatedGraphNode* v,
                                                   std::map<AnnotatedGraphNode*, bool>&);

        /// helper function for annotateGraphDistributedly()
        void removeLabeledSuccessor(AnnotatedGraphNode* v, std::string label);


    protected:

        /// Pointer to the underlying petri net.
        oWFN* PN;

        /// store temporarily calculated states in binDecision structure
        binDecision * tempBinDecision;

        /// this set contains all states of the newly calculated node
        StateSet setOfStatesStubbornTemp;

        /// A sorted set of the nodes in the graph. Should always contain the same nodes as the vector, though iteration order can differ.
        GraphNodeSet setOfSortedNodes;

        void addProgress(double);

    public:

        /// basic constructor
        CommunicationGraph(oWFN *);

        /// basic destructor
        virtual ~CommunicationGraph();



        /// Returns the number of nodes in this graph.
        virtual unsigned int getNumberOfNodes() const;

        /// Adds a node to the CommunicationGraph. The node is inserted in both sets.
        virtual void addNode(AnnotatedGraphNode*);

        /// remove a node from the annotated graph
        virtual void removeNode(AnnotatedGraphNode*);

        /// checks if the given node is in the graphs node set
        AnnotatedGraphNode* findGraphNodeInSet(AnnotatedGraphNode*);



        /// calculate a root node for the graph
        void calculateRootNode();

        /// function to annotate the OG for distributed controllability
        bool annotateGraphDistributedly();

        /// gives a diagnosis of the graph
        void diagnose();

        /// checks wether a state activates output events
        bool stateActivatesOutputEvents(State*);


        /// prints the current global progress value depending whether the value
        /// changed significantly and depending on the debug-level set
        void printProgressFirst();

        /// prints the current global progress value depending whether the value
        /// changed significantly and depending on the debug-level set
        void printProgress();

        /// brief creates a dot file of the graph
        void printGraphToDot();

        /// function to create an STG representation of the IG or OG
        void printGraphToSTG();


        /// deletes the corresponding oWFN
        void deleteOWFN();

        /// Neither implemented nor used somewhere:
        void buildGraphRandom();


        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(CommunicationGraph)
#define new NEW_NEW
};

#endif /*GRAPH_H_*/
