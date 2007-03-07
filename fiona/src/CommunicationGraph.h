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
#include "vertex.h"
#include "enums.h"
#include <string>
#include <fstream>
#include <iostream>

#include <set>

using namespace std;

class oWFN;
class State;

struct compareVertices {
  bool operator() (vertex const * left, vertex const * right) {
    return (*left < *right);
  }
}; // compareVertices

typedef std::multiset<unsigned int> messageMultiSet;

struct compareMessageMultiSets {
  bool operator() (messageMultiSet const left, messageMultiSet const right) {
    if (left.size() < right.size()) {
        return true;
    }
    if (left.size() > right.size()) {
        return false;
    }
    for (std::multiset<unsigned int>::iterator s1 = left.begin(); s1 != left.end(); s1++) {
        for (std::multiset<unsigned int>::iterator s2 = right.begin(); s2 != right.end(); s2++) {
            if (*s1 < *s2) {
                return true;
            }
            if (*s1 > *s2) {
                return false;
            }
        }
    }
    return false;
  }
}; // compareMessageMultiSets

typedef std::set<vertex*, compareVertices> vertexSet;

typedef std::set<messageMultiSet, compareMessageMultiSets> setOfMessages;


/* reachability graph */
class communicationGraph {
protected:
	oWFN * PN;                   	//!< pointer to the underlying petri net

	vertex * root;                   //!< the root node of the graph
	vertex * currentVertex;          //!< the vertex we are working on at a certain time

    double global_progress;
    
    unsigned int numberOfNodes;            	//!< the number of vertices of the graph
	unsigned int numberOfEdges;           	//!< the number of edges of the graph

	unsigned int numberOfBlueNodes;			//!< number of blue nodes in the graph
	unsigned int numberOfBlueEdges;			//!< number of blue edges in the graph

	unsigned int numberOfBlackNodes;		//!< number of black nodes in the graph

	unsigned int numberOfStatesAllNodes;

	unsigned int actualDepth; 				//!< actual depth in graph
  
public:
	communicationGraph(oWFN *);
	~communicationGraph();

	vertexSet setOfVertices;

    void calculateRootNode();

    vertex * getRoot() const;
    
    unsigned int getNumberOfNodes() const;
    unsigned int getNumberOfEdges() const;

    unsigned int getNumberOfBlueNodes();
    unsigned int getNumberOfBlueEdges() const;

    unsigned int getNumberOfBlackNodes() const;
    
    unsigned int getNumberOfStatesAllNodes() const;

    vertex * findVertexInSet(vertex *);

    vertex * AddVertex(vertex *, unsigned int, edgeType);
    bool AddVertex(vertex *, messageMultiSet, edgeType);

	bool terminateBuildGraph(vertex *);

    void buildGraphRandom();

    // for OG
    void calculateSuccStatesInput(unsigned int, vertex *, vertex *);
    void calculateSuccStatesOutput(unsigned int, vertex *, vertex *);

    // for IG
    void calculateSuccStatesInput(messageMultiSet, vertex *, vertex *);
    void calculateSuccStatesOutput(messageMultiSet, vertex *, vertex *);

	void printNodeStatistics();
    void printGraphToDot(vertex * v, fstream& os, bool[]);
    void printDotFile();

    void computeNumberOfBlueNodesEdges(vertex *, bool[]);
    

    bool stateActivatesOutputEvents(State *);

    analysisResult analyseNode(vertex *, bool);
};

#endif /*GRAPH_H_*/
