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
#include "enums.h"
#include "commGraphFormula.h"
//#include <string>
//#include <ostream>
//#include <set>


using namespace std;


class State;
class graphEdge;
class successorNodeList;
class literal;
//class CNF;
class CNF_formula;


struct StateCompare {
  bool operator() ( State const * left, State const * right) {
	    return (left < right);
  }
}; // StateCompare

typedef std::set<State*, StateCompare> StateSet;


class vertex {

protected:
    unsigned int numberOfVertex;			//!< number of this vertex in the graph

	vertexColor color; 						//!< color of vertex

  //  CNF * firstClause;						//!< annotation of this node (a CNF)
	CNF_formula* annotation;				//!< annotation of this node (a CNF) as a formula 
	
	
	successorNodeList * successorNodes;		//!< list of all the nodes succeeding this one 
											//!< including the edge between them
	
	successorNodeList * predecessorNodes;	//!< list of all blue nodes preceding this one 
											//!< including the edge between them

public:
    bool hasFinalStateInStateSet;

	// vertex management
	vertex(int);
	~vertex ();

	int * eventsUsed;
	int eventsToBeSeen;
	
    unsigned int getNumber() const;    
    void setNumber(unsigned int);
    
	// states in vertex
    bool addState(State *);
    int getNumberOfDeadlocks();
    
    StateSet reachGraphStateSet;		// this set contains only a reduced number of states in case the state reduced graph is to be build
//    StateSet setOfStatesTemp;	// this set contains all states

	// traversing successors/predecessors
    void addSuccessorNode(graphEdge *);
    void addPredecessorNode(graphEdge *);

    graphEdge * getNextSuccEdge();
    graphEdge * getNextPredEdge();

    void resetIteratingSuccNodes();
    void resetIteratingPredNodes();

	// annotation
    CNF_formula* getCNF_formula();
    string getCNFString();
    
    CommGraphFormulaAssignment* getAssignment();
    
    void addClause(CommGraphFormulaMultiaryOr*);
   
//	int numberOfElementsInAnnotation();

	// analysis   
	bool finalAnalysisDone;

    vertexColor analyseNodeByFormula();

    vertexColor getColor() const;
    void setColor(vertexColor c);

    bool isToShow(const vertex* rootOfGraph) const;

	void removeLiteralFromFormula(unsigned int, edgeType);
	
    void propagateToSuccessors();
    void propagateToPredecessors();

//	friend bool operator == (vertex const&, vertex const& );		// could be changed and replaced by hash-value
	friend bool operator < (vertex const&, vertex const& );
//	friend ostream& operator << (std::ostream& os, const vertex& v);

// Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
    NEW_OPERATOR(vertex)
#define new NEW_NEW
    
};

#endif /*VERTEX_H_*/
