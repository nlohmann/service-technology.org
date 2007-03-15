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
 * \file    CNF.cc
 *
 * \brief   functions for annotations of OGs
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "CNF.h"

#include "graphEdge.h"
#include "vertex.h"

#include "state.h"

#include <stddef.h>
#include <string>
#include "main.h"
#include "enums.h"
#include "debug.h"

using namespace std;


//! \fn literal::literal()
//! \brief constructor
literal::literal() :
	fake(false),
	edge(NULL),
	nextElement(NULL) {

}


//! \fn literal::literal(graphEdge * _edge)
//! \param _edge the edge this literal is associated with
//! \brief constructor
literal::literal(graphEdge * _edge) :
	edge(_edge),
	nextElement(NULL) {
	
}


//! \fn literal::~literal()
//! \brief destructor
literal::~literal() {
	trace(TRACE_5, "literal::~literal() : start\n");
	if (edge != NULL && fake) {
		 delete edge;
	}
	trace(TRACE_5, "literal::~literal() : end\n");
}


//! \fn void literal::addLiteral(const string& label)
//! \param label the label to be added to the literal list
//! \brief adds the given label to the list of literals
void literal::addLiteral(const string& label) {
    trace(TRACE_5, "literal::addLiteral(char * label) : start\n");
	
	literal * currentLiteral = this;
	
	// create a fake edge temporarily since the real edge has not been created yet,
	// but we know that this state has a literal with the given label
	graphEdge * newEdge = new graphEdge(NULL, label, sending);
	
	if (!edge) {
		edge = newEdge;
		fake = true;
	    trace(TRACE_5, "literal::addLiteral(char * label) : end\n");
		return ;	
	}
	
	while (currentLiteral->nextElement) {		// get the last literal of the clause
		currentLiteral = currentLiteral->nextElement;	
	}	  
	currentLiteral->nextElement = new literal(newEdge);	// create a new clause literal	
	currentLiteral->nextElement->fake = true;			// since we don't have a real edge yet, we mark it as fake
	
    trace(TRACE_5, "literal::addLiteral(char * label) : end\n");
}


//! \fn void literal::setEdge(graphEdge * _edge)
//! \param _edge the edge that this literal shall be associated with
//! \brief adds the given edge to this literal
void literal::setEdge(graphEdge * _edge) {
    trace(TRACE_5, "literal::setEdge(graphEdge * edge) : start\n");
	
	assert(edge != NULL);
	
	if (fake) {  // in case we have stored a "fake" edge, we delete that one
		delete edge;	
	}

	fake = false;	// now we have a real edge! no fake one anymore
	edge = _edge;   // set the edge stored to the one given

    trace(TRACE_5, "literal::setEdge(graphEdge * edge) : end\n");
}


//! \fn void literal::setEdges(graphEdge * edge)
//! \param edge the edge that will replace a fake edge being stored in the literal's list
//! \brief sets the literal's edges to the given edge
void literal::setEdges(graphEdge * edge) {
    trace(TRACE_5, "literal::setEdges(graphEdge * edge) : start\n");
	
 	literal * currentLiteral = this;
 	
 	// iterate over all literals and replace all fake edges that have the label
 	// "edge" by the real edge
 	while (currentLiteral) {
		if  (currentLiteral->edge != NULL && currentLiteral->edge->getLabel() == edge->getLabel()) {
			// we have found a pseudo edge with that label, so store the correct edge right here
			currentLiteral->setEdge(edge);
			trace(TRACE_5, "literal::setEdges(graphEdge * edge) : end\n");		
			return;
		}
 		currentLiteral = currentLiteral->nextElement;
 	}
    trace(TRACE_5, "literal::setEdges(graphEdge * edge) : end\n");
}


//! \fn string literal::getClauseString()
//! \return the literal as a string
//! \brief returns the literal as a string
string literal::getClauseString() {
    trace(TRACE_5, "literal::getClauseString() : start\n");
	
    string clauseString = "(";
    bool comma = false;

	literal * currentLiteral = this;

    while (currentLiteral) {
        if (currentLiteral->edge != NULL && 
        		currentLiteral->edge->getNode() != NULL && 
        		currentLiteral->edge->getNode()->getColor() != RED && 
        		currentLiteral->edge->getNode()->reachGraphStateSet.size() > 0) {
        			
            if (comma) {
                clauseString += " + ";
            }
            if (currentLiteral->edge->getType() == sending) {
                clauseString += "!";
            } else {
                clauseString += "?";
            }
            clauseString += currentLiteral->edge->getLabel();
            comma = true;
        }
    	currentLiteral = currentLiteral->nextElement;	
    }
    
    clauseString += ")";
    
    trace(TRACE_5, "literal::getClauseString() : end\n");
    
    return clauseString;
}


//! \fn CNF::CNF()
//! \brief constructor
CNF::CNF() :
	firstLiteral(NULL),
	nextElement(NULL) {

}


//! \fn CNF::~CNF()
//! \brief destructor, iterates over all literals and deletes each one of them
CNF::~CNF() {
	literal * literalTemp1 = firstLiteral;
	literal * literalTemp2;
	
	while (literalTemp1) {
		literalTemp2 = literalTemp1->nextElement;
		delete literalTemp1;
		literalTemp1 = literalTemp2;
	}
}


//! \fn vertexColor CNF::calcClauseColor()
//! \return RED, if the state is a bad state (it is red ;-)), BLUE if it shurely is good
//! \brief RED, if the state is a bad state (it is red ;-)), BLUE if it shurely is good
//! a state is RED, if all events it activates lead to bad nodes
vertexColor CNF::calcClauseColor() {

	trace(TRACE_5, "CNF::calcClauseColor(): start\n");

	if (firstLiteral == NULL) {
		// no literals means empty disjunction
		return RED;
	}

	literal * currentLiteral = firstLiteral;	// point to the first literal of the clause
	literal * lastConsideredLiteral = NULL;
	vertexColor clauseColor = RED;				// initially empty disjunction
	
	while (currentLiteral) {
		// check each literal of clause,
		// clause gets BLUE if at least one literal points to a blue node

		// first case: the literal points to a blue node
		if (currentLiteral->edge != NULL && currentLiteral->edge->getNode() != NULL
		   && (currentLiteral->edge->getNode()->getColor() == BLUE)) {

			clauseColor = BLUE;
			break;
        }

		// second case: the literal points to a red node, so we delete that literal
		if (currentLiteral->edge != NULL && currentLiteral->edge->getNode() != NULL
        	     && currentLiteral->edge->getNode()->getColor() == RED) {
			
            if (lastConsideredLiteral != NULL) {
            	lastConsideredLiteral->nextElement = currentLiteral->nextElement;
            } else if (firstLiteral == currentLiteral) {
            	firstLiteral = currentLiteral->nextElement;
            	if (firstLiteral == NULL) {
            		delete currentLiteral;
            		trace(TRACE_5, "CNF::calcClauseColor(): end\n");
            		return RED;
            	}
            }
            
            literal * literalTemp = currentLiteral->nextElement;	// remember the next literal in list
      
	        delete currentLiteral;
            currentLiteral = literalTemp;		// get the remembered literal
            continue;
        }

		// third case: current literal cannot be evaluated
		// therefore clause cannot be evaluated and is indefinite
		// however, still removing red literals from clause
		// assert(false);
		
//		trace(TRACE_5, "literal points to a black node " + intToString(literal->edge->getNode()->getNumber()) + "\n");
        lastConsideredLiteral = currentLiteral;			// remember this literal
		currentLiteral = currentLiteral->nextElement;
	}
	
	trace(TRACE_5, "CNF::calcClauseColor(): end\n");

	return clauseColor;
}


//! \fn string CNF::getCNFString()
//! \return the clause as a string
//! \brief returns the current clause as a string
string CNF::getClauseString() {
	
	if (isFinalState) {
		return "(true)";
	}

	if (firstLiteral == NULL) {
		// no literal means empty disjunction (which is equal to false)
		return "(false)";
	}

//	// rekursively going down
//	literal * currentLiteral = firstLiteral;  // get the first literal of the clause
//	clauseString += currentLiteral->getClauseString();
//	return clauseString;

	// this clause's string is a disjunction of its literals
	return firstLiteral->getClauseString();
}


////! \fn void CNF::setEdge(graphEdge * edge)
////! \param edge
////! \brief sets the the clause's edge to the given edge
//void CNF::setEdge(graphEdge * edge) {
// 	literal * literalTemp = firstLiteral;
// 	literal * literalPrev = NULL;
// 	
// 	if (edge->getNode() && edge->getNode()->getColor() != RED) {
//
//	 	while (literalTemp) {
//			if  (literalTemp->edge != NULL && literalTemp->edge->getLabel() == edge->getLabel()) {
//				// we have found a pseudo edge with that label, so store the correct edge right here
//				if (literalPrev == NULL) {
//					firstLiteral = literalTemp->nextElement;
//				} else {
//					literalPrev->nextElement = literalTemp->nextElement;
//				}				
//			}
//			literalPrev = literalTemp;		// remember this clause
//	 		literalTemp = literalTemp->nextElement;
//	 	}
// 		return;
// 	}
// 	
// 	while (literalTemp) {
//		literalTemp->setEdges(edge);
// 		literalTemp = literalTemp->nextElement;	
// 	}
//}


//int CNF::getNumberOfElements() {
//	literal * currentLiteral = firstLiteral;
// 	int count = 0;
//
// 	while (currentLiteral) {
// 		count++;
//		currentLiteral = currentLiteral->nextElement;
// 	}
//	
//	return count;
//}
