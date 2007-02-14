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


//! \fn clause::clause()
//! \brief constructor
clause::clause() : fake(false), edge(NULL), nextElement(NULL) {
	
}

//! \fn clause::clause(graphEdge * _edge)
//! \param _edge the edge this clause is associated with
//! \brief constructor
clause::clause(graphEdge * _edge) : edge(_edge), nextElement(NULL) {
	
}

//! \fn clause::~clause()
//! \brief destructor
clause::~clause() {
	trace(TRACE_5, "clause::~clause() : start\n");
	if (edge != NULL && fake) {
		 delete edge;	
	}
	trace(TRACE_5, "clause::~clause() : end\n");
}

//! \fn void clause::setEdge(graphEdge * _edge)
//! \param _edge the edge that this clause shall be associated with
//! \brief adds the given edge to this clause 
void clause::setEdge(graphEdge * _edge) {
    trace(TRACE_5, "clause::setEdge(graphEdge * edge) : start\n");
	
	if (edge != NULL) {
		if (fake) {  // in case we have stored a "fake" edge, we delete that one
			delete edge;	
		} 
	}
	
	fake = false;	// now we have a real edge! no fake one anymore
	edge = _edge;   // set the edge stored to the one given

    trace(TRACE_5, "clause::setEdge(graphEdge * edge) : end\n");
	
}

//! \fn void clause::addLiteral(const string& label)
//! \param label the label to be added to the clause list
//! \brief adds the given label to the clause list
void clause::addLiteral(const string& label) {
    trace(TRACE_5, "clause::addLiteral(char * label) : start\n");
	
	clause * cl = this;
	
	// create a fake edge temporarily since the real edge has not been created yet,
	// but we know, that this state has a clause with the given label
	graphEdge * newEdge = new graphEdge(NULL, label, sending);
	
	if (!edge) {
		edge = newEdge;
		fake = true;
	    trace(TRACE_5, "clause::addLiteral(char * label) : end\n");
		return ;	
	}
	
	while (cl->nextElement) {		// get the last literal of the clause
		cl = cl->nextElement;	
	}	  
	cl->nextElement = new clause(newEdge);	// create a new clause literal	
	cl->nextElement->fake = true;			// since we don't have a real edge yet, we mark it as fake
	
    trace(TRACE_5, "clause::addLiteral(char * label) : end\n");
}

//! \fn string clause::getClauseString()
//! \return the clause as a string
//! \brief returns the clause as a string
string clause::getClauseString() {
    trace(TRACE_5, "clause::getClauseString() : start\n");
	
    string clauseString = "(";
    bool comma = false;

	clause * cl = this;

    while (cl) {
        if (cl->edge != NULL && 
        		cl->edge->getNode() != NULL && 
        		cl->edge->getNode()->getColor() != RED && 
        		cl->edge->getNode()->reachGraphStateSet.size() > 0) {
        			
            if (comma) {
                clauseString += " + ";
            }
            if (cl->edge->getType() == sending) {
                clauseString += "!";
            } else {
                clauseString += "?";
            }
            clauseString += cl->edge->getLabel();
            comma = true;
        }   	
    	cl = cl->nextElement;	
    }
    
    clauseString += ")";
    
    trace(TRACE_5, "clause::getClauseString() : end\n");
    
    return clauseString;
}

//! \fn void clause::setEdges(graphEdge * edge)
//! \param edge the edge that will replace a fake edge being stored in the literal's list
//! \brief sets the literal's edges to the given edge
void clause::setEdges(graphEdge * edge) {
    trace(TRACE_5, "clause::setEdges(graphEdge * edge) : start\n");
	
 	clause * cl = this;
 	
 	while (cl) {
		if  (cl->edge != NULL && cl->edge->getLabel() == edge->getLabel()) {
			// we have found a pseudo edge with that label, so store the correct edge right here
			cl->setEdge(edge);
			trace(TRACE_5, "clause::setEdges(graphEdge * edge) : end\n");		
			return;
		}
 		cl = cl->nextElement;	
 	}
    trace(TRACE_5, "clause::setEdges(graphEdge * edge) : end\n");
	
}

//! \fn CNF::CNF()
//! \brief constructor
CNF::CNF() : cl(NULL), nextElement(NULL) {

}

//! \fn CNF::~CNF()
//! \brief destructor, iterates over all clauses and deletes each one of them
CNF::~CNF() {
	clause * clauseTemp1 = cl;
	clause * clauseTemp2;
	
	while (clauseTemp1) {
		clauseTemp2 = clauseTemp1->nextElement;
		delete clauseTemp1;	
		clauseTemp1 = clauseTemp2;
	}
}

//! \fn void CNF::addClause(clause * newClause)
//! \param newClause the clause to be added to this CNF
//! \brief adds the given clause to this CNF
void CNF::addClause(clause * newClause) {
	cl = newClause;
}

//! \fn vertexColor CNF::calcClauseColor()
//! \return RED, if the state is a bad state (it is red ;-)), BLUE if it shurely is good, BLACK otherwise
//! \brief RED, if the state is a bad state (it is red ;-)), BLUE if it shurely is good, BLACK otherwise
//! a state is RED, if all events it activates lead to bad nodes
vertexColor CNF::calcClauseColor() {

	trace(TRACE_5, "CNF::calcClauseColor(): start\n");

	if (cl == NULL) {		// since there is no clause we can't conclude anything
		return RED;			// is not intended to happen
	}
	clause * literal = cl;			// point to the first literal of the clause
	clause * literalPrev = NULL;
	vertexColor clauseColor = BLACK;
	
	while (literal) {					// check the clause stored

		// first case: the literal points to a blue node
		if (literal->edge != NULL &&
				literal->edge->getNode() != NULL && 
				(literal->edge->getNode()->getColor() == BLUE
					|| literal->edge->getNode()->getColor() == BLACK)) {
		
            clauseColor = BLUE;
        }
		// second case: the literal points to a red node, so we delete that literal
        if (literal->edge != NULL && 
        		literal->edge->getNode() != NULL && 
        		literal->edge->getNode()->getColor() == RED) {
            // delete that literal in the clause since it points to a red node

            if (literalPrev != NULL) {
            	literalPrev->nextElement = literal->nextElement;
            } else if (cl == literal) {
            	cl = literal->nextElement;
            	if (cl == NULL) {
            		delete literal;
            		trace(TRACE_5, "CNF::calcClauseColor(): end\n");
            		return RED;	
            	}
            }
            
            clause * literalTemp = literal->nextElement;	// remember the next literal in list
      
	        delete literal;
            literal = literalTemp;		// get the remembered literal
            continue ;
        }
        
		// third case: current literal cannot be evaluated
		// therefore clause cannot be evaluated and is indefinite
		// however, still removing red literals from clause
        
        literalPrev = literal;			// remember this literal
		literal = literal->nextElement;	
	}
	
	trace(TRACE_5, "CNF::calcClauseColor(): end\n");
	
//	if (indefinite) {
//		return BLACK;
//	} else {
//	    return RED;
//	}
	return clauseColor;
}

//! \fn string CNF::getCNFString()
//! \return the CNF as a string
//! \brief returns the CNF as a string
string CNF::getCNFString() {
	
    string clauseString = "";

	if (cl == NULL) {		// since theres is no clause we can't conclude anything
		return "(false)";	
	}
	
	if (isFinalState) {
		return "(true)";
	}

	clause * literal = cl;  // get the first literal of the clause

 //   while (literal) {
 //       if (moreThanOne) {
 //       	clauseString += " + ";	
 //       }
        clauseString += literal->getClauseString();
 //       moreThanOne = true;
            	
 //   	literal = literal->nextElement;	
 //   }
    
    return clauseString;
}

//! \fn void CNF::setEdge(graphEdge * edge)
//! \param edge
//! \brief sets the the clause's edge to the given edge
void CNF::setEdge(graphEdge * edge) {
 	clause * clauseTemp = cl;
 	clause * clausePrev = NULL;
 	
 	if (edge->getNode() && edge->getNode()->getColor() != RED) {
	
	 	while (clauseTemp) {
			if  (clauseTemp->edge != NULL && clauseTemp->edge->getLabel() == edge->getLabel()) {
				// we have found a pseudo edge with that label, so store the correct edge right here
				if (clausePrev == NULL) {
					cl = clauseTemp->nextElement;	
				} else {
					clausePrev->nextElement = clauseTemp->nextElement;
				}				
			}
			clausePrev = clauseTemp;		// remember this clause
	 		clauseTemp = clauseTemp->nextElement;	
	 	}
 		return;	
 	}
 	
 	while (clauseTemp) {
		clauseTemp->setEdges(edge);				
 		clauseTemp = clauseTemp->nextElement;	
 	}
}

int CNF::numberOfElements() {
	clause * literal = cl;
 	int count = 0;

 	while (literal) {
 		count++;
		literal = literal->nextElement;	
 	}
	
	return count;
}
