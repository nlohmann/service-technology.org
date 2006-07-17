#include "mynew.h"
#include "CNF.h"

#include "graphEdge.h"
#include "vertex.h"
#include "graph.h"

#include <stddef.h>
#include <string>
#include "main.h"
#include "enums.h"
#include "debug.h"

using namespace std;

clause::clause() : edge(NULL), nextElement(NULL)  {
	
}

clause::clause(graphEdge * _edge) : edge(_edge), nextElement(NULL)  {
	
}

clause::~clause() {
	if (edge->getNode() == NULL) {
		delete edge;	
	}
}

void clause::setEdge(graphEdge * _edge) {
    trace(TRACE_5, "clause::setEdge(graphEdge * edge) : start\n");
	
	if (edge != NULL) {
		if (edge->getNode() == NULL) {  // in case we have stored a "fake" edge, we delete that one
			delete edge;	
		} 
		edge = _edge;					// set the edge stored to the one given
	} else {
		edge = _edge;
	}	
    trace(TRACE_5, "clause::setEdge(graphEdge * edge) : end\n");
	
}

//! \fn void clause::addLiteral(char * clauseLabel)
//! \param clauseLabel the label to be added this clause list
//! \brief adds the given label to this clause list
void clause::addLiteral(char * label) {
    trace(TRACE_5, "clause::addLiteral(char * label) : start\n");
	
	clause * cl = this;
	
	graphEdge * newEdge = new graphEdge(NULL, label, sending);
	
	if (!edge) {
		edge = newEdge;
	    trace(TRACE_5, "clause::addLiteral(char * label) : end\n");
		return ;	
	}
	
	while (cl->nextElement) {		// get the last literal of the clause
		cl = cl->nextElement;	
	}	  
	cl->nextElement = new clause(newEdge);	// create a new clause literal	
	
    trace(TRACE_5, "clause::addLiteral(char * label) : end\n");
}

//! \fn string clause::getClauseString()
//! \return the clause as a string
//! \brief returns the clause as a string
string clause::getClauseString() {
	
    string clauseString = "";
    bool comma = false;

	clause * cl = this;

    while (cl) {
        if (cl->edge->getNode() != NULL && cl->edge->getNode()->getColor() != RED && 
        			cl->edge->getNode()->setOfStates.size() > 0) {
            if (comma) {
                clauseString += "+";
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
    
    return clauseString;
}

//! \fn void clause::setEdges(graphEdge * edge)
//! \param edge
//! \brief sets the the clause's edges to the given edge
void clause::setEdges(graphEdge * edge) {
    trace(TRACE_5, "clause::setEdges(graphEdge * edge) : start\n");
	
 	clause * cl = this;
 	
 	while (cl) {
		if  (cl->edge != NULL && strcmp(cl->edge->getLabel(), edge->getLabel()) == 0) {
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
CNF::CNF() : nextElement(NULL), cl(NULL) {

}

//! \fn void CNF::addClause(clause * newClause)
//! \param newClause the clause to be added to this CNF
//! \brief adds the given clause to this CNF
void CNF::addClause(clause * newClause) {
	cl = newClause;
}

//! \fn vertexColor CNF::calcColor()
//! \return RED, if the state is a bad state (it is red ;-)), BLUE if it shurely is good, BLACK otherwise
//! \brief RED, if the state is a bad state (it is red ;-)), BLUE if it shurely is good, BLACK otherwise
//! a state is RED, if all events it activates lead to bad nodes
vertexColor CNF::calcColor() {

	trace(TRACE_5, "CNF::calcColor(): start\n");

	if (cl == NULL) {		// since there is no clause we can't conclude anything
		return BLACK;	
	}

	clause * literal = cl;			// point to the first literal of the clause
	clause * literalPrev = NULL;
	bool indefinite = false;
	
	while (literal) {					// check the clause stored
		if (literal->edge != NULL &&
				literal->edge->getNode() != NULL && 
				literal->edge->getNode()->getColor() == BLUE) {
					
			trace(TRACE_5, "CNF::calcColor(): end\n");
            return BLUE;
        } 
        if (literal->edge == NULL ||
        		literal->edge->getNode() == NULL || literal->edge->getNode()->getColor() == BLACK) {
            indefinite = true;
        } else {
	        if (literal->edge != NULL && 
	        		literal->edge->getNode() != NULL && 
	        		literal->edge->getNode()->getColor() == RED) {
	        			
	            // delete that literal in the clause since it points to a red node
	            if (literalPrev != NULL) {
	            	literalPrev->nextElement = literal->nextElement;
	            } else if (cl == literal) {
	            	cl = literal->nextElement;
	            }
	            
	            clause * literalTemp = literal->nextElement;	// remember the next literal in list
          
	            delete literal;							// delete literal
	            literal = literalTemp;						// get the remembered literal
	            continue ;
	        } 
        }
        literalPrev = literal;			// remember this literal
		literal = literal->nextElement;	
	}
	
	trace(TRACE_5, "CNF::calcColor(): end\n");
	
	if (indefinite) {
		return BLACK;
	} else {
	    return RED;
	}
}

//! \fn string CNF::getCNF()
//! \return the CNF as a string
//! \brief returns the CNF as a string
string CNF::getCNF() {
	
    string clauseString = "";
    bool moreThanOne = false;

	if (cl == NULL) {		// since theres is no clause we can't conclude anything
		return "NULL";	
	}

	clause * literal = cl;

    while (literal) {
        if (moreThanOne) {
        	clauseString += " + ";	
        }
        clauseString += literal->getClauseString();
        moreThanOne = true;
            	
    	literal = literal->nextElement;	
    }
    
    return clauseString;
}

//! \fn void CNF::setEdge(graphEdge * edge)
//! \param edge
//! \brief sets the the clause's edge to the given edge
void CNF::setEdge(graphEdge * edge) {
 	clause * literal = cl;
 	
 	while (literal) {
		literal->setEdges(edge);				
 		literal = literal->nextElement;	
 	}
}
