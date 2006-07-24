#include "mynew.h"
#include "graph.h"
#include "successorNodeList.h"
#include "vertex.h"
#include "CNF.h"

//! \fn vertex::vertex()
//! \brief constructor for OG nodes
vertex::vertex(int numberEvents) :
			   color(BLACK),
			   successorNodes(NULL),
		//	   states(NULL),
			   numberOfVertex(0),
			   annotation(NULL) {

	eventsUsed = new int [numberEvents];

	for (int i = 0; i < numberEvents; i++) {
		eventsUsed[i] = 0;
	}
	eventsToBeSeen = numberEvents;
}

//! \fn vertex::vertex()
//! \brief constructor for IG nodes
vertex::vertex() :
               color(BLACK),
               successorNodes(NULL),
        //       states(NULL),
               numberOfVertex(0),
			   annotation(NULL) {
               	
	eventsToBeSeen = 0;
	eventsUsed = NULL;
}

//! \fn vertex::~vertex()
//! \brief destructor
vertex::~vertex () {
	
	if (successorNodes != NULL) {
		delete successorNodes;
	}
//	if (states != NULL) {
//		delete states;
//	}
	if (eventsUsed != NULL) {
		delete[] eventsUsed;
	}
	
	numberDeletedVertices++;
}
	
//! \fn unsigned int vertex::getNumber()
//! \return number of this node
//! \brief returns the number of this node
unsigned int vertex::getNumber() {
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
	successorNodes->addNextNode(edge);
	eventsToBeSeen--;
}

//! \fn void vertex::addState(State * s) 
//! \param s pointer to the state that is to be added to this node
//! \brief adds the state s to the list of states
bool vertex::addState(State * s) {
	pair<StateSet::iterator, bool> result = setOfStates.insert(s);

    return result.second;       // returns whether the element got inserted (true) or not (false)
	 
}

//! \fn void vertex::addClauseElement(clause * newClause)
//! \param newClause the clause to be added to this CNF
//! \brief adds the given clause to this CNF
void vertex::addClause(clause * newClause, bool _isFinalState) {
    trace(TRACE_5, "vertex::addClause(clause * newClause) : start\n");
	
	CNF * cnfElement = annotation;
	
	cout << "adding clause to node number " << numberOfVertex << endl;
	
	if (cnfElement == NULL) {
		annotation = new CNF();
		annotation->addClause(newClause);
		annotation->isFinalState = _isFinalState;
		trace(TRACE_5, "vertex::addClause(clause * newClause) : end\n");
		return ;	
	}
	
	while (cnfElement->nextElement) {		// get the last literal of the clause
		cnfElement = cnfElement->nextElement;	
	}	  
	cnfElement->nextElement = new CNF();
	cnfElement->nextElement->addClause(newClause);	// create a new clause literal	
	cnfElement->nextElement->isFinalState = _isFinalState;

	cout << "number of elements in annotation of node " << numberOfVertex << " : " << numberOfElementsInAnnotation() << endl;
		
    trace(TRACE_5, "vertex::addClause(clause * newClause) : end\n");
}

//! \fn graphEdge * vertex::getNextEdge()
//! \return pointer to the next edge of the successor node list
//! \brief returns a pointer to the next edge of the successor node list
graphEdge * vertex::getNextEdge() {
	if (successorNodes != NULL) {
		return successorNodes->getNextElement();
	} else {
		return NULL;
	}
}

//! \fn successorNodeList * vertex::getSuccessorNodes()
//! \return pointer to the successor node list
//! \brief returns a pointer to the successor node list
successorNodeList * vertex::getSuccessorNodes() {
	return successorNodes;
}

//! \fn void vertex::resetIteratingSuccNodes()
//! \brief resets the iteration process of the successor node list
void vertex::resetIteratingSuccNodes() {
	if (successorNodes != NULL) {
		successorNodes->resetIterating();
	}
}

//! \fn void vertex::setAnnotationEdges(graphEdge * edge)
//! \param edge the edge
//! \brief sets the edge to all literals in the clauses
void vertex::setAnnotationEdges(graphEdge * edge) {
    trace(TRACE_5, "vertex::setAnnotationEdges(graphEdge * edge) : start\n");
	
	CNF * cnfElement = annotation;
	
	if (cnfElement == NULL) {
		return ;	
	}
	
	while (cnfElement) {		// get the last literal of the clause
		if (cnfElement->cl != NULL) {
			cnfElement->cl->setEdges(edge);		// let the clause set the edges to all of its literals
		}
		cnfElement = cnfElement->nextElement;	
	}	  
    trace(TRACE_5, "vertex::setAnnotationEdges(graphEdge * edge) : end\n");
	
}

//! \fn string vertex::getCNF()
//! \brief returns the annotation as a string
string vertex::getCNF() {
	string CNFString = "";
	string CNFStringTemp = "";

	bool mal = false;
	
	CNF * cl = annotation;
	
	while (cl) {
		CNFStringTemp = "";
		CNFStringTemp = cl->cl->getClauseString();
		if (mal && CNFStringTemp != "final" && CNFStringTemp != "") {
			CNFString += " * ";
		}
		if (CNFStringTemp != "final" && CNFStringTemp != "") {
			CNFString += CNFStringTemp;
		}
		mal = true;
		cl = cl->nextElement;
	}
	
	return CNFString;
}

int vertex::numberOfElementsInAnnotation() {
	CNF * cl = annotation;
	int count = 0;
	while (cl) {
		count++;
		cl = cl->nextElement;
	}
	return count;

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
	for (iter = setOfStates.begin(); iter != setOfStates.end(); iter++) {
		
		if ((*iter)->type == DEADLOCK) {	// state is a DEADLOCK, so count it
			count++;
		}
	}
	return count;
}

//! \fn analysisResult vertex::analyseNode(bool finalAnalysis)
//! \brief analyses the node and sets its color, if the node gets to be red, then TERMINATE is returned
analysisResult vertex::analyseNode(bool finalAnalysis) {

    trace(TRACE_2, "\t\t\t analysing node ");
    if (finalAnalysis) {
    	trace(TRACE_2, "(final analysis)");
    }
    trace(TRACE_2, intToString(numberOfVertex) + ": ");

    if (color != RED) {          // red nodes stay red forever
        if (setOfStates.size() == 0) {
            // we analyse an empty node; it becomes blue
            color = BLUE;
            trace(TRACE_2, "node analysed blue (empty node)");
            trace(TRACE_2, "\t ...terminate\n");
            return TERMINATE;
        } else {
            // we analyse a non-empty node

            vertexColor c = BLACK;                      // the color of the current node
            vertexColor cTmp = BLACK;                   // the color of a state of the current node
            string clause;
            bool finalState = false;

			CNF * cl = annotation;						// get pointer to the first clause of the CNF

            // iterate over all clauses of the annotation and determine color for clause
            while (cl) {
            	if (cl->isFinalState) {
            		finalState = true;
            		cout << "node : " << numberOfVertex << " has final state" << endl;
            		c = BLUE;
            	} else if (eventsToBeSeen == 0 || finalAnalysis) {
                	finalState = false;
                    cTmp = cl->calcColor();
                    switch (cTmp) {
                    case RED:   // found a red clause; so node becomes red
                        if (color == BLACK) {
                            // node was black
                            trace(TRACE_2, "node analysed red \t");
                            color = RED;
                        } else if (color == RED) {
                            // this should not be the case!
                            trace(TRACE_2, "analyseNode called when node already red!!!");
                        } else if (color == BLUE) {
                            // this should not be the case!
                            trace(TRACE_2, "analyseNode called when node already blue!!!");
                        }
                        trace(TRACE_2, "\t\t ...terminate\n");
                        return TERMINATE;
                        break;
                    case BLUE:  // found a blue state (i.e. deadlock is resolved)
                        c = BLUE;
                        break;
                    case BLACK: // no definite result of state analysis
                        if (finalAnalysis) {
                            color = RED;
                            trace(TRACE_2, "node analysed red (final analysis)");
                            trace(TRACE_2, "\t ...terminate\n");
                            return TERMINATE;           // <---------------------???
                        } else {
                            color = BLACK;
                            trace(TRACE_2, "node still indefinite \t\t ...continue\n");
                            return CONTINUE;
                        }
                        break;
                    }
                } else {
                    // still events left to resolve deadlocks...
                    color = BLACK;
                    trace(TRACE_2, "node still indefinite \t\t ...continue\n");
                    return CONTINUE;
                }
                
                cl = cl->nextElement; 	// get the next clause of the CNF
            }

            // all clauses considered
            trace(TRACE_2, "all states checked, node becomes ");
            if (c == BLACK) {
                trace(TRACE_2, "red");
            } else if (c == RED) {
                trace(TRACE_2, "red");
            } else if (c == BLUE) {
                trace(TRACE_2, "blue");
            }
            
            color = c;
            
            if (finalState) {
            	if (color == BLACK) {
            		color = BLUE;
            	}
                trace(TRACE_2, " ...terminate\n");
                return TERMINATE;
            } else {
                trace(TRACE_2, " ...continue\n");
                return CONTINUE;
            }
        }
    }
    return TERMINATE;
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
    return (left.setOfStates < right.setOfStates);	
}


////! \fn ostream& operator << (ostream& os, const vertex& v)
////! \param os output stream
////! \param v vertex
////! \brief implements the operator << by printing the states of the vertex v to the output stream
//ostream& operator << (ostream& os, const vertex& v) {			
//	os << *(v.states);
//	return os;
//}
