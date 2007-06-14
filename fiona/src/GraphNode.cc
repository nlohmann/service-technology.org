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
 * \file    GraphNode.cc
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

#include "mynew.h"
#include "GraphNode.h"
#include "state.h"
#include "successorNodeList.h"
#include "cnf_formula.h"
#include "debug.h"
#include "options.h"    
#include <cassert>


//! \param numberEvents the number of events that have to be processed from this node
//! \brief constructor
GraphNode::GraphNode(int numberEvents) :
			   number(0),
			   color(BLUE),
			   successorNodes(NULL),
			   hasFinalStateInStateSet(false) {

	annotation = new CNF_formula();

	eventsUsed = new int [numberEvents];
	
	for (int i = 0; i < numberEvents; i++) {
		eventsUsed[i] = 0;
	}

	eventsToBeSeen = numberEvents;
}


//! \brief destructor
GraphNode::~GraphNode() {
	trace(TRACE_5, "GraphNode::~GraphNode() : start\n");

	if (successorNodes != NULL) {
		delete successorNodes;
	}
	
	if (eventsUsed != NULL) {
		delete[] eventsUsed;
	}
	
	delete annotation;

	numberDeletedVertices++;
	trace(TRACE_5, "GraphNode::~GraphNode() : end\n");
}


//! \return number of this node
//! \brief returns the number of this node
unsigned int GraphNode::getNumber() const {
    return number;
}


//! \param _number number of this node in the graph
//! \brief sets the number of this node
void GraphNode::setNumber(unsigned int _number) {
	number = _number;
}


//! \param edge pointer to the edge which is to point to the successor node
//! \brief adds the node v to the list of successor nodes of this node using the edge
//! given by the parameters
void GraphNode::addSuccessorNode(GraphEdge * edge) {
	if (successorNodes == NULL) {
		successorNodes = new successorNodeList();
	}
	eventsToBeSeen--;
	successorNodes->addNextNode(edge);
	return;
}


//! \param s pointer to the state that is to be added to this node
//! \brief adds the state s to the list of states
bool GraphNode::addState(State * s) {
	pair<StateSet::iterator, bool> result = reachGraphStateSet.insert(s);
    return result.second;       // returns whether the element got inserted (true) or not (false)
}


//! \param myclause the clause to be added to the annotation of the current node
//! \brief adds a new clause to the CNF formula of the node
void GraphNode::addClause(CommGraphFormulaMultiaryOr* myclause) {
	annotation->addClause(myclause);
}


void GraphNode::removeLiteralFromFormula(unsigned int i, GraphEdgeType type) {
    trace(TRACE_5, "GraphNode::removeLiteralFromFormula(unsigned int i, GraphEdgeType type) : start\n");

    if (type == SENDING) {
        //cout << "remove literal " << PN->getInputPlace(i)->getLabelForCommGraph() << " from annotation " << annotation->asString() << " of node number " << getNumber() << endl;
        annotation->removeLiteral(PN->getInputPlace(i)->getLabelForCommGraph());
    } else {
        //cout << "remove literal " << PN->getOutputPlace(i)->getLabelForCommGraph() << " from annotation " << annotation->asString() << " of node number " << getNumber() << endl;
        annotation->removeLiteral(PN->getOutputPlace(i)->getLabelForCommGraph());
    }

    trace(TRACE_5, "GraphNode::removeLiteralFromFormula(unsigned int i, GraphEdgeType type) : end\n");
}


//! \return pointer to the next edge of the successor node list
//! \brief returns a pointer to the next edge of the successor node list
GraphEdge * GraphNode::getNextSuccEdge() {
	if (successorNodes != NULL) {
		return successorNodes->getNextElement();
	} else {
		return NULL;
	}
}


//! \brief resets the iteration process of the successor node list
void GraphNode::resetIteratingSuccNodes() {
	trace(TRACE_5, "GraphNode::resetIteratingSuccNodes() : start\n");
	
	if (successorNodes != NULL) {
		successorNodes->resetIterating();
	}
	
	trace(TRACE_5, "GraphNode::resetIteratingSuccNodes() : end\n");
}


// returns the CNF formula that is the annotation of a node as a Boolean formula
CNF_formula* GraphNode::getCNF_formula() {
	return annotation;
}


// return the assignment that is imposed by present or absent arcs leaving node v
CommGraphFormulaAssignment* GraphNode::getAssignment() {
	
	trace(TRACE_5, "computing annotation of node " + intToString(getNumber()) + "\n");

	CommGraphFormulaAssignment* myassignment = new CommGraphFormulaAssignment();
	
	// traverse outgoing edges and set the corresponding literals
	// to true if the respective node is BLUE
    this->resetIteratingSuccNodes();
    GraphEdge* edge;
    while ((edge = this->getNextSuccEdge()) != NULL) {
		if (edge->getDstNode()->getColor() == BLUE) {
            myassignment->setToTrue(edge->getLabel());
		}
	}
	
	// only if node has final state, set assignment of literal final to true
	if (this->hasFinalStateInStateSet == true) {
		myassignment->setToTrue(CommGraphFormulaLiteral::FINAL);
	}
	
	//cout << "ende" << endl;
	return myassignment;

}


//! \param c color of GraphNode
//! \brief sets the color of the GraphNode to the given color
void GraphNode::setColor(GraphNodeColor c) {
	color = c;
}


//! \brief returns the color of the GraphNode
GraphNodeColor GraphNode::getColor() const {
	return color;
}


bool GraphNode::isToShow(const GraphNode* rootOfGraph) const
{
	if ( parameters[P_SHOW_ALL_NODES] ||
	    (parameters[P_SHOW_NO_RED_NODES] && (getColor() != RED)) ||
	    (!parameters[P_SHOW_NO_RED_NODES] && (getColor() == RED)) ||
	    (getColor() == BLUE) ||
	    (this == rootOfGraph) )
	{
		return (parameters[P_SHOW_EMPTY_NODE] ||
		        reachGraphStateSet.size() != 0);
	}
	else
	{
		return false;
	}
}


//! \result the color of the node
//! \brief analyses the node, sets its color, and returns the new color
GraphNodeColor GraphNode::analyseNodeByFormula() {

	trace(TRACE_5, "GraphNode::analyseNodeByFormula() : start\n");

	// computing the assignment given by outgoing edges (to blue nodes)
	CommGraphFormulaAssignment* myassignment = this->getAssignment();
	bool result = this->getCNF_formula()->value(*myassignment);
	delete myassignment;

	trace(TRACE_5, "GraphNode::analyseNodeByFormula() : end\n");

	if (result) {
		trace(TRACE_3, "\t\t\t node analysed blue, formula " + this->getCNF_formula()->asString());
		return BLUE;
	} else {
		trace(TRACE_3, "\t\t\t node analysed red, formula " + this->getCNF_formula()->asString());
		return RED;
	}
}

//! \param left left hand GraphNode
//! \param right right hand GraphNode
//! \brief implements the operator < by comparing the states of the two vertices
bool operator < (GraphNode const& left, GraphNode const& right) {
    return (left.reachGraphStateSet < right.reachGraphStateSet);	
}
