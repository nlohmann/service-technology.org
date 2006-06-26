#include "successorNodeList.h"
#include "graphEdge.h"
#include "vertex.h"
#include "enums.h"
#include <string>
#include <iostream>

#ifdef LOG_NEW
#include "mynew.h"
#endif

using namespace std;

//! \fn successorNodeList::successorNodeList()
//! \brief constructor 
successorNodeList::successorNodeList() : 
	firstElement(NULL), 
	nextElement(NULL) 
{
	end = false;		
}

//! \fn successorNodeList::~successorNodeList()
//! \brief destructor
successorNodeList::~successorNodeList(){
	deleteList(firstElement);
}

//! \fn void successorNodeList::deleteList(graphEdge * element)
//! \param element pointer to the next element
//! \brief deletes the whole list recursively
void successorNodeList::deleteList(graphEdge * element) {
	if (element == NULL) {
		return ;
	} else {
		deleteList(element->getNextElement());
	}
	if (element != NULL) {
		delete element;
	}
}

//! \fn bool successorNodeList::addNextNode(graphEdge * transition)
//! \param transition the transition/ edge which is to be added to the list
//! \return  true, if everything is fine, false otherwise
//! \brief adding an additional node to the end of the list 
bool successorNodeList::addNextNode(graphEdge * transition){
/* TODO: return true if everythings okay, false otherwise */
	
	//graphEdge * transition = new graphEdge(node, label, type);
	if (firstElement == NULL) {
		firstElement = transition;
		return true;	
	}

	graphEdge * tmpEdge;
	
	tmpEdge = firstElement;
	
	while (tmpEdge) {
		if (tmpEdge->getNextElement() != NULL) {
			tmpEdge = tmpEdge->getNextElement();	
		} else {
			tmpEdge->setNextElement(transition);
			return true;
		}
	}
	
	/* not good ;-) - to be changed */
	return true;
}

//! \fn int successorNodeList::elementCount()
//! \return the number of elements in the list
//! \brief returns the number of elements in the list
int successorNodeList::elementCount() {
	int count = 0;
	
	graphEdge * tmpEdge = firstElement;
	
	while (tmpEdge) {
		count++;
		if (tmpEdge->getNextElement() != NULL) {
			tmpEdge = tmpEdge->getNextElement();	
		} 
	}
	
	
	return count;
}

//! \fn void successorNodeList::setFirstElement(graphEdge * _edge)
//! \param _edge the new first element
//! \brief sets the first element of the list to _edge
void successorNodeList::setFirstElement(graphEdge * _edge) {
	firstElement = _edge;
}

//! \fn graphEdge * successorNodeList::getFirstElement()
//! \return the first element of the list
//! \brief returns the first element of the list
graphEdge * successorNodeList::getFirstElement(){
	return firstElement;
}

//! \fn graphEdge * successorNodeList::findElement(vertex * node)
//! \param node pointer to the node to be searched for
//! \return a pointer to the corresponding graphEdge, or NULL if the search did not succeed
//! \brief searches for an element in the list ??? do we need this function ??? no implementation yet!
graphEdge * successorNodeList::findElement(vertex * node) {
	return NULL;
}

//! \fn void successorNodeList::resetIterating()
//! \brief resets the iteration through the list, so one can start a whole new iteration
void successorNodeList::resetIterating() {
	nextElement = NULL;
	end = false;	
}

//! \fn graphEdge * successorNodeList::getNextElement()
//! \return a pointer to the next graphEdge in case of iterating through the list, NULL if the end of the list has been reached
//! \brief returns a pointer to the next graphEdge in case of iterating through the list, NULL if the end of the list has been reached
graphEdge * successorNodeList::getNextElement() {
	if (!end) {
		if (nextElement == NULL) {
			nextElement = firstElement;
		} else {
			nextElement = nextElement->getNextElement();
			if (nextElement == NULL) {
				end = true;
			}
		}
		return nextElement;
	}
	return NULL;
}
