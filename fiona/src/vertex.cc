#include "graph.h"
#include "successorNodeList.h"
#include "OG.h"
#include "main.h"

//#include "enums.h"
//#include "vertex.h"
//#include "stateList.h"
//#include "graphEdge.h"

//#include <stddef.h>
//#include <iostream>


//! \fn vertex::vertex()
//! \brief constructor for OG nodes
vertex::vertex(int numberEvents) :
				color(BLACK),
				successorNodes(NULL),
				states(NULL),
//				predecessorNodes(NULL),
				numberOfVertex(0) {

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
				states(NULL),
//				predecessorNodes(NULL),
				numberOfVertex(0) {
               	
				eventsUsed = NULL;
				eventsToBeSeen = 0;
}


//! \fn vertex::~vertex()
//! \brief destructor
vertex::~vertex () {
	if (successorNodes != NULL) {
		delete successorNodes;
	}
	if (states != NULL) {
		delete states;
	}
//	if (predecessorNodes != NULL) {
//		delete predecessorNodes;
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


//! \fn void vertex::setStateList(stateList * list)
//! \param list list of states 
//! \brief sets the vertex state list to the given list
void vertex::setStateList(stateList * list) {
	states = list;								
}


//! \fn stateList * vertex::getStateList()
//! \brief returns the states of the node
stateList * vertex::getStateList() {
	return states;
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


//! \fn successorNodeList * vertex::getPredecessorNodes()
//! \return pointer to the predecessor node list
//! \brief returns a pointer to the predecessor node list
//successorNodeList * vertex::getPredecessorNodes() {
//	return predecessorNodes;
//}


//! \fn void vertex::resetIteratingSuccNodes()
//! \brief resets the iteration process of the successor node list
void vertex::resetIteratingSuccNodes() {
	if (successorNodes != NULL) {
		successorNodes->resetIterating();
	}
}

//! \fn void vertex::addPredecessorNode(graphEdge * edge)
//! \param edge pointer to the edge which is to point to the predecessor node
//! \brief adds the node v to the list of predecessor nodes of this node using the edge
//! given by the parameters
//void vertex::addPredecessorNode(graphEdge * edge) {
//	if (predecessorNodes == NULL) {
//		predecessorNodes = new successorNodeList();	
//	}	
//	predecessorNodes->addNextNode(edge);
//}


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

//! \fn void vertex::deleteRedSuccessorNodes() 
//! \brief deletes all the edges that point to a not existing node
void vertex::deleteRedSuccessorNodes(operatingGuidelines * OG) {
	if (successorNodes) {
		graphEdge * edgeTmp1 = successorNodes->getFirstElement();
		graphEdge * edgeTmpBefore = NULL;
		
		while (edgeTmp1) {
			if (edgeTmp1->getNode() != NULL && edgeTmp1->getNode()->getColor() == RED) {
				if (edgeTmpBefore == NULL) {
					successorNodes->setFirstElement(edgeTmp1->getNextElement());
				} else {
					edgeTmpBefore->setNextElement(edgeTmp1->getNextElement());
				}
				
				OG->setOfVertices.erase(edgeTmp1->getNode());
				
				reachGraphStateSet::iterator iter;			// iterator over the stateList's elements

				// iterate over all states and determine color for each state
            	for (iter = states->setOfReachGraphStates.begin(); 
            			iter != states->setOfReachGraphStates.end(); iter++) {
            		 // we just consider the maximal states only
                	if ((*iter)->state->type == DEADLOCK || (*iter)->state->type == FINALSTATE) {
            		    (*iter)->clause1.erase(edgeTmp1);
                	}
            	}
								
				delete edgeTmp1->getNode();	// delete red node
				delete edgeTmp1;			// delete edge pointing to that red node
				OG->numberOfEdges--;
			}
			
			edgeTmpBefore = edgeTmp1;
			edgeTmp1 = edgeTmp1->getNextElement();	
		}
	}
}

//! \fn vertexColor vertex::getNumberOfDeadlocks()
//! \brief returns the number of deadlocks
int vertex::getNumberOfDeadlocks() {
	reachGraphStateSet::iterator iter;

	int count = 0;
	
	// iterate over all states of the current node 
	for (iter = states->setOfReachGraphStates.begin(); iter != states->setOfReachGraphStates.end(); iter++) {
		
		if ((*iter)->state->type == DEADLOCK) {	// state is a DEADLOCK, so count it
			count++;
		}
	}
	return count;
}


//! \fn bool operator == (vertex const& left, vertex const& right)
//! \param left left hand vertex
//! \param right right hand vertex
//! \brief implements the operator == by comparing the states of the two vertices
bool operator == (vertex const& left, vertex const& right) {
    return (*(left.states) == *(right.states));	
}


//! \fn bool operator < (vertex const& left, vertex const& right)
//! \param left left hand vertex
//! \param right right hand vertex
//! \brief implements the operator < by comparing the states of the two vertices
bool operator < (vertex const& left, vertex const& right) {
    return (*(left.states) < *(right.states));	
}


//! \fn ostream& operator << (ostream& os, const vertex& v)
//! \param os output stream
//! \param v vertex
//! \brief implements the operator << by printing the states of the vertex v to the output stream
ostream& operator << (ostream& os, const vertex& v) {			
	os << *(v.states);
	return os;
}
