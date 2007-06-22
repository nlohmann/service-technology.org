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
 * \file    communicationGraph.h
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
#include "graphEdge.h"
#include "owfn.h"
#include <fstream>

#include <set>

using namespace std;

class oWFN;
class State;
class GraphNode;


extern double global_progress;
extern int show_progress;

/* communication graph */
class communicationGraph {
private:
    void computeNumberOfNodesEdges(
        unsigned int& nNodes,
        unsigned int& nStoredStates,
        unsigned int& nEdges) const;

    void computeNumberOfNodesEdgesHelper(
        GraphNode*    v,
        bool          visitedNodes[],
        unsigned int& nNodes,
        unsigned int& nStoredStates,
        unsigned int& nEdges) const;

    void computeNumberOfBlueNodesEdges(
        unsigned int& nBlueNodes,
        unsigned int& nBlueEdges) const;

    void computeNumberOfBlueNodesEdgesHelper(
        GraphNode* v,
        bool       visitedNodes[],
        unsigned int& nBlueNodes,
        unsigned int& nBlueEdges) const;

protected:
	oWFN* PN;                            //!< pointer to the underlying petri net

	GraphNode* root;                     //!< the root node of the graph

	void addProgress(double);

public:

	communicationGraph(oWFN *);
	~communicationGraph();

	GraphNodeSet setOfVertices;

    GraphNode* getRoot() const;

    void calculateRootNode();

    unsigned int getNumberOfNodes() const;

    GraphNode * findGraphNodeInSet(GraphNode *);

    void addGraphNode(GraphNode *, GraphNode *);        // for OG
    bool addGraphNode(GraphNode *, GraphNode *, messageMultiSet, GraphEdgeType);     // for IG

    void addGraphEdge(GraphNode*, GraphNode*, oWFN::Places_t::size_type, GraphEdgeType);        // for OG

    void analyseNode(GraphNode *);

	void printProgressFirst();
	void printProgress();

    void buildGraphRandom();

    // for OG
    void calculateSuccStatesInput(unsigned int, GraphNode *, GraphNode *);
    void calculateSuccStatesOutput(unsigned int, GraphNode *, GraphNode *);

    // for IG
    void calculateSuccStatesInput(messageMultiSet, GraphNode *, GraphNode *);
    void calculateSuccStatesOutput(messageMultiSet, GraphNode *, GraphNode *);

    void printGraphToDot(GraphNode * v, fstream& os, bool[]);
    void printDotFile();

    void printNodeStatistics();
    bool stateActivatesOutputEvents(State *);

// Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
	NEW_OPERATOR(communicationGraph)
#define new NEW_NEW
};

#endif /*GRAPH_H_*/
