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
 * \file    successorNodeList.cc
 *
 * \brief   functions for traversing IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */
 
#include "mynew.h"
#include "successorNodeList.h"
#include "graphEdge.h"
#include "vertex.h"
#include "enums.h"
#include <string>
#include <iostream>

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
successorNodeList::~successorNodeList() {
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
		// test if this edge is already stored
		if (tmpEdge->getNode() == transition->getNode() && 
				tmpEdge->getType() == transition->getType() &&
				tmpEdge->getLabel() == transition->getLabel()) {
			// yes, we have such an edge already -> return false, because no annotation shall be added either
			return false;	
		}
		if (tmpEdge->getNextElement() != NULL) {
			tmpEdge = tmpEdge->getNextElement();	
		} else {
			tmpEdge->setNextElement(transition);
			return true;
		}
	}
	return true;
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
graphEdge * successorNodeList::findElement(vertex * /*node*/) {
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
