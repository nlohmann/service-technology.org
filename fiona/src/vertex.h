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
 * \file    vertex.h
 *
 * \brief   functions for handling of nodes of IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef VERTEX_H_
#define VERTEX_H_

#include "mynew.h"
#include <string>
#include <ostream>
#include <set>

#include "enums.h"

using namespace std;

class State;
class graphEdge;
class successorNodeList;
class clause;
class CNF;


struct StateCompare {
  bool operator() ( State const * left, State const * right) {
	    return (left < right);
  }
}; // StateCompare

typedef std::set<State*, StateCompare> StateSet;


class vertex {

protected:
	vertexColor color; 						//!< color of vertex
	successorNodeList * successorNodes;		//!< list of all the nodes succeeding this one 
											//!< including the edge between them
	
	successorNodeList * predecessorNodes;	//!< list of all blue nodes preceding this one 
											//!< including the edge between them

    unsigned int numberOfVertex;			//!< number of this vertex in the graph

public:
	vertex(int);
	~vertex ();
	
    CNF * annotation;						//!< annotation of this node (a CNF)

	int * eventsUsed;
	int eventsToBeSeen;
	bool finalAnalysisDone;
	
    unsigned int getNumber() const;
    
    StateSet reachGraphStateSet;		// this set contains only a reduced number of states in case the state reduced graph is to be build
//    StateSet setOfStatesTemp;	// this set contains all states
    
    void setNumber(unsigned int);
    bool addSuccessorNode(graphEdge *);
    bool addPredecessorNode(graphEdge *);
    
    bool addState(State *);
    
    void addClause(clause *, bool);
    void setAnnotationEdges(graphEdge *);
   
	int numberOfElementsInAnnotation();
 
    graphEdge * getNextSuccEdge();
    graphEdge * getNextPredEdge();

    successorNodeList * getSuccessorNodes();
    
    void resetIteratingSuccNodes();
    void resetIteratingPredNodes();
   
    void setColor(vertexColor c);
    vertexColor getColor();
    
    string getCNF();
    CNF * getAnnotation();
    
    int getNumberOfDeadlocks();
    analysisResult analyseNode(bool);
    
    void propagateToSuccessors();
    void propagateToPredecessors();
    
    
//    friend bool operator == (vertex const&, vertex const& );		// could be changed and replaced by hash-value
    friend bool operator < (vertex const&, vertex const& );
//    friend ostream& operator << (std::ostream& os, const vertex& v);

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(vertex)
#define new NEW_NEW
    
};

#endif /*VERTEX_H_*/
