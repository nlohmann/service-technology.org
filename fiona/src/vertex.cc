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
 * \file    vertex.cc
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
#include "vertex.h"
#include "state.h"
#include "successorNodeList.h"
#include "CNF.h"
#include "cnf_formula.h"
#include "debug.h"
#include <cassert>


//! \fn vertex::vertex(int numberEvents)
//! \param numberEvents the number of events that have to be processed from this node
//! \brief constructor
vertex::vertex(int numberEvents) :
			   numberOfVertex(0),
			   color(BLUE),
			   firstClause(NULL),
			   successorNodes(NULL),
			   predecessorNodes(NULL),
			   hasFinalStateInStateSet(false),
			   finalAnalysisDone(false) {

	annotation = new CNF_formula();

	eventsUsed = new int [numberEvents];
	
	for (int i = 0; i < numberEvents; i++) {
		eventsUsed[i] = 0;
	}

	eventsToBeSeen = numberEvents;
}


//! \fn vertex::~vertex()
//! \brief destructor
vertex::~vertex() {
	trace(TRACE_5, "vertex::~vertex() : start\n");

	if (successorNodes != NULL) {
		delete successorNodes;
	}
	
	if (predecessorNodes != NULL) {
		delete predecessorNodes;
	}
	
	if (eventsUsed != NULL) {
		delete[] eventsUsed;
	}
	
	CNF * cnfTemp1 = firstClause;
	CNF * cnfTemp2;
	
	delete annotation;
	
	
	while (cnfTemp1) {
		cnfTemp2 = cnfTemp1->nextElement;
		delete cnfTemp1;	
		cnfTemp1 = cnfTemp2;
	}
	
	numberDeletedVertices++;
	trace(TRACE_5, "vertex::~vertex() : end\n");
}


//! \fn unsigned int vertex::getNumber() const
//! \return number of this node
//! \brief returns the number of this node
unsigned int vertex::getNumber() const {
    return numberOfVertex;
}


//! \fn void vertex::setNumber(unsigned int _number)
//! \param _number number of this node in the graph
//! \brief sets the number of this node
void vertex::setNumber(unsigned int _number) {
	numberOfVertex = _number;
}


//! \fn void vertex::addSuccessorNode(graphEdge * edge) 
//! \param edge pointer to the edge which is to point to the successor node
//! \brief adds the node v to the list of successor nodes of this node using the edge
//! given by the parameters
void vertex::addSuccessorNode(graphEdge * edge) {
	if (successorNodes == NULL) {
		successorNodes = new successorNodeList();
	}
	eventsToBeSeen--;
	successorNodes->addNextNode(edge);
	return;
}


//! \fn void vertex::addPredecessorNode(graphEdge * edge) 
//! \param edge pointer to the edge which is to point to the predecessor node
//! \brief adds the node v to the list of preceding nodes of this node using the edge
//! given by the parameters
void vertex::addPredecessorNode(graphEdge * edge) {
	if (predecessorNodes == NULL) {
		predecessorNodes = new successorNodeList();
	}
	predecessorNodes->addNextNode(edge);
	return;
}


//! \fn void vertex::addState(State * s) 
//! \param s pointer to the state that is to be added to this node
//! \brief adds the state s to the list of states
bool vertex::addState(State * s) {
	pair<StateSet::iterator, bool> result = reachGraphStateSet.insert(s);
    return result.second;       // returns whether the element got inserted (true) or not (false)
}


//! \fn void vertex::addClause(literal * myFirstLiteral, bool _isFinalState)
//! \param myFirstLiteral the current clause's first literal
//! \param _isFinalState determines whether clause points to a final state or not
//! \brief adds a new clause to the CNF of the node
void vertex::addClause(literal * myFirstLiteral, bool _isFinalState) {
    trace(TRACE_5, "vertex::addClause(literal * myFirstLiteral, bool _isFinalState) : start\n");
	
	CNF * cnfElement = firstClause;
	
//	cout << "adding clause to node number " << numberOfVertex << endl;
//	cout << "finalstate: " << _isFinalState << endl;
	
	if (cnfElement == NULL) {
		firstClause = new CNF();
		firstClause->firstLiteral = myFirstLiteral;
		firstClause->isFinalState = _isFinalState;
		trace(TRACE_5, "vertex::addClause(literal * myFirstLiteral, bool _isFinalState) : end\n");
//		cout << "number of elements in annotation of node " << numberOfVertex << " : " << numberOfElementsInAnnotation() << endl;
		return;
	}

	while (cnfElement->nextElement) {		// get the last clause of the CNF
		cnfElement = cnfElement->nextElement;
	}
	cnfElement->nextElement = new CNF();
	cnfElement->nextElement->firstLiteral = myFirstLiteral;	// create a new clause literal
	cnfElement->nextElement->isFinalState = _isFinalState;

//	cout << "number of elements in annotation of node " << numberOfVertex << " : " << numberOfElementsInAnnotation() << endl;
		
    trace(TRACE_5, "vertex::addClause(literal * myFirstLiteral, bool _isFinalState) : end\n");
}


//! \fn void vertex::addClause(CommGraphFormulaMultiaryOr* myclause)
//! \param myclause the clause to be added to the annotation of the current node
//! \brief adds a new clause to the CNF formula of the node
void vertex::addClause(CommGraphFormulaMultiaryOr* myclause) {
	annotation->addClause(myclause);
}


void vertex::removeLiteralFromFormula(unsigned int i, edgeType type) {
	trace(TRACE_5, "vertex::removeLiteralFromFormula(unsigned int i, edgeType type) : start\n");
	
	if (type == sending) {
		//cout << "remove literal !" << PN->getInputPlace(i)->name << " from annotation " << annotation->asString() << " of node number " << getNumber() << endl;
		annotation->removeLiteral('!' + PN->getInputPlace(i)->name);
	} else {
		//cout << "remove literal ?" << PN->getOutputPlace(i)->name << " from annotation " << annotation->asString() << " of node number " << getNumber() << endl;
		annotation->removeLiteral('?' + PN->getOutputPlace(i)->name);
	}
	
	trace(TRACE_5, "vertex::removeLiteralFromFormula(unsigned int i, edgeType type) : end\n");
}


//! \fn graphEdge * vertex::getNextSuccEdge()
//! \return pointer to the next edge of the successor node list
//! \brief returns a pointer to the next edge of the successor node list
graphEdge * vertex::getNextSuccEdge() {
	if (successorNodes != NULL) {
		return successorNodes->getNextElement();
	} else {
		return NULL;
	}
}


//! \fn graphEdge * vertex::getNextPredEdge()
//! \return pointer to the next edge of the successor node list
//! \brief returns a pointer to the next edge of the successor node list
graphEdge * vertex::getNextPredEdge() {
	if (predecessorNodes != NULL) {
		return predecessorNodes->getNextElement();
	} else {
		return NULL;
	}
}


//! \fn void vertex::resetIteratingSuccNodes()
//! \brief resets the iteration process of the successor node list
void vertex::resetIteratingSuccNodes() {
	trace(TRACE_5, "vertex::resetIteratingSuccNodes() : start\n");
	
	if (successorNodes != NULL) {
		successorNodes->resetIterating();
	}
	
	trace(TRACE_5, "vertex::resetIteratingSuccNodes() : end\n");
}


//! \fn void vertex::resetIteratingPredNodes()
//! \brief resets the iteration process of the successor node list
void vertex::resetIteratingPredNodes() {
	trace(TRACE_5, "vertex::resetIteratingPredNodes() : start\n");
	
	if (predecessorNodes != NULL) {
		predecessorNodes->resetIterating();
	}
	
	trace(TRACE_5, "vertex::resetIteratingPredNodes() : end\n");
}


//! \fn void vertex::setAnnotationEdges(graphEdge * edge)
//! \param edge the edge
//! \brief sets the edge to all literals in the clauses
void vertex::setAnnotationEdges(graphEdge * edge) {
    trace(TRACE_5, "vertex::setAnnotationEdges(graphEdge * edge) : start\n");
	
	CNF * currentClause = firstClause;

	// iterate over all clauses
	while (currentClause) {
		if (currentClause->firstLiteral != NULL) {
			// let the clause set the edges to all of its literals
			currentClause->firstLiteral->setEdges(edge);
		}
		currentClause = currentClause->nextElement;	
	}
    trace(TRACE_5, "vertex::setAnnotationEdges(graphEdge * edge) : end\n");
}


//! \fn string vertex::getCNFString()
//! \brief returns the annotation as a string
string vertex::getCNFString() {
	string CNFString = "";
	string CNFStringTemp = "";

	bool mal = false;
	
	CNF * cl = firstClause;

	if ((cl == NULL) && (getColor() == RED))
		return "(false)";
	
	if ((cl == NULL) && (getColor() == BLUE))
		return "(true)";
	
	while (cl) {
		CNFStringTemp = "";
		CNFStringTemp = cl->getClauseString();
		
		if (mal && CNFStringTemp != "()") {
			CNFString += " * ";
		}
		if (CNFStringTemp != "()") {
			CNFString += CNFStringTemp;
		}
		mal = true;
		cl = cl->nextElement;
	}


	/*
	 * The following three calls were added by Niels to simplify the string
	 * representation of CNF formulas in the DOT output of an OG. All
	 * needed code is implemented in files "cnf_formula.h" and
	 * "cnf_formula.cc". If the following three calls are removed,
	 * everything is (as ugly) as before.
	 */

	// 1. create a CNF_Formula object from the CNF string representation
	CNF_Formula formula = CNF_Formula(CNFString);

	// 2. simplify the CNF formula inside the CNF_Formula object
	formula.simplify();

	// 3. create a string representation of the simplified formula
	CNFString = formula.to_string();	


	return CNFString;
}


// returns the CNF formula that is the annotation of a node as a pointer to class CNF
CNF * vertex::getAnnotation() {
	return firstClause;
}


// returns the CNF formula that is the annotation of a node as a Boolean formula
CNF_formula* vertex::getCNF_formula() {
	return annotation;
}


// return the assignment that is imposed by present or absent arcs leaving node v
CommGraphFormulaAssignment* vertex::getAssignment() {
	
	trace(TRACE_5, "computing annotation of node " + intToString(getNumber()) + "\n");

	CommGraphFormulaAssignment* myassignment = new CommGraphFormulaAssignment();
	
	// traverse outgoing edges and set the corresponding literals
	// to true if the respective node is BLUE
    this->resetIteratingSuccNodes();
    graphEdge* edge;
    while ((edge = this->getNextSuccEdge()) != NULL) {
		if (edge->getNode()->getColor() == BLUE) {
			if (edge->getType() == sending) {
				myassignment->setToTrue('!' + edge->getLabel());
			} else {
				myassignment->setToTrue('?' + edge->getLabel());
			}
		}
	}
	
	// only if node has final state, set assignment of literal final to true
	if (this->hasFinalStateInStateSet == true) {
		myassignment->setToTrue(CommGraphFormulaAssignment::FINAL);
	}
	
	//cout << "ende" << endl;
	return myassignment;

}


//! \fn void vertex::setColor(vertexColor c)
//! \param c color of vertex
//! \brief sets the color of the vertex to the given color
void vertex::setColor(vertexColor c) {
	color = c;
}

//! \fn vertexColor vertex::getColor()
//! \brief returns the color of the vertex
vertexColor vertex::getColor() {
	return color;
}


//! \fn vertexColor vertex::getNumberOfDeadlocks()
//! \brief returns the number of deadlocks
int vertex::getNumberOfDeadlocks() {
	StateSet::iterator iter;

	int count = 0;
	
	// iterate over all states of the current node 
	for (iter = reachGraphStateSet.begin(); iter != reachGraphStateSet.end(); iter++) {
		
		if ((*iter)->type == DEADLOCK) {	// state is a DEADLOCK, so count it
			count++;
		}
	}
	return count;
}

//! \fn void vertex::propagateToSuccessors() 
//! \brief in case the current node got the color in the finalanalysis, this decision is propagated to its
//! successor nodes
void vertex::propagateToSuccessors() {
	resetIteratingSuccNodes();
	graphEdge * element;

    while ((element = getNextSuccEdge()) != NULL) {
    	if (element->getNode()->getColor() != RED) {
    		element->getNode()->analyseNode();	// analyse the successor node again
    	}
    }
}

//! \fn void vertex::propagateToPredecessors() 
//! \brief in case the current node got the color in the finalanalysis, this decision is propagated to its
//! predecessor nodes
void vertex::propagateToPredecessors() {
	resetIteratingPredNodes();
	graphEdge * element;

    while ((element = getNextPredEdge()) != NULL) {
    	if (element->getNode()->getColor() != RED && element->getNode()->finalAnalysisDone) {
    		element->getNode()->analyseNode();	// analyse the preceding node again
    	}
    	if (element->getNode()->getColor() == RED) {
    		predecessorNodes->removeNodeFromList(element->getNode(), true);			
    	}
    }   
}


//! \fn analysisResult vertex::analyseNode()
//! \brief analyses the node and returns its color
vertexColor vertex::analyseNode() {

    trace(TRACE_5, "vertex::analyseNode() : start\n");
    
    trace(TRACE_3, "\t\t\t analysing node ");
    trace(TRACE_3, intToString(numberOfVertex) + ": ");

	assert (getColor() == BLUE);
	
    if (reachGraphStateSet.size() == 0) {
        // we analyse an empty node; it becomes blue
        trace(TRACE_3, "\t\t\t node analysed blue (empty node)");
        trace(TRACE_5, "vertex::analyseNode() : end\n");
        return BLUE;
    } else {
        // we analyse a non-empty node
		CNF * cl = firstClause;					// get pointer to the first clause of the CNF

        // iterate over all clauses of the annotation
        // if there is any RED clause, the node becomes RED as well
        // otherwise, the node stays BLUE
        while (cl) {
        	if (cl->isFinalState == false) {
				vertexColor cTmp = cl->calcClauseColor();

                if (cTmp == RED) {
					// found a red clause; so node becomes red

	            	propagateToSuccessors();	
	            	propagateToPredecessors();	

			        trace(TRACE_3, "\t\t\t node analysed red (there was a red clause)");
                    trace(TRACE_5, "vertex::analyseNode() : end\n");

                    return RED;
                    break;
                }
            }
            
            cl = cl->nextElement; 	// get the next clause of the CNF
        }

        // all clauses considered and no red clause found
        trace(TRACE_3, "\t\t\t node analysed blue (there was no red clause)");
		trace(TRACE_5, "vertex::analyseNode() : end\n");
		return BLUE;
    }
}


//! \fn vertexColor vertex::analyseNodeByFormula()
//! \result the color of the node
//! \brief analyses the node, sets its color, and returns the new color
vertexColor vertex::analyseNodeByFormula() {

    trace(TRACE_5, "vertex::analyseNodeByFormula() : start\n");
    
	// computing the assignment given by outgoing edges (to blue nodes)
    CommGraphFormulaAssignment* myassignment = this->getAssignment();
	bool result = this->getCNF_formula()->value(*myassignment);

   	trace(TRACE_5, "vertex::analyseNodeByFormula() : end\n");

	if (result) {
        trace(TRACE_3, "\t\t\t node analysed blue, formula " + this->getCNF_formula()->asString());
		return BLUE;
	} else {
        trace(TRACE_3, "\t\t\t node analysed red, formula " + this->getCNF_formula()->asString());
		return RED;
	}
}


//
//
////! \fn bool operator == (vertex const& left, vertex const& right)
////! \param left left hand vertex
////! \param right right hand vertex
////! \brief implements the operator == by comparing the states of the two vertices
//bool operator == (vertex const& left, vertex const& right) {
//    return (*(left.states) == *(right.states));	
//}


//! \fn bool operator < (vertex const& left, vertex const& right)
//! \param left left hand vertex
//! \param right right hand vertex
//! \brief implements the operator < by comparing the states of the two vertices
bool operator < (vertex const& left, vertex const& right) {
    return (left.reachGraphStateSet < right.reachGraphStateSet);	
}


////! \fn ostream& operator << (ostream& os, const vertex& v)
////! \param os output stream
////! \param v vertex
////! \brief implements the operator << by printing the states of the vertex v to the output stream
//ostream& operator << (ostream& os, const vertex& v) {			
//	os << *(v.states);
//	return os;
//}
