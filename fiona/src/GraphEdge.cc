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
 * \file    graphEdge.cc
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
#include "graphEdge.h"
#include <string>
#include "vertex.h"
#include "enums.h"

//! \fn  graphEdge::graphEdge(vertex * nodeP, char * labelP, edgeType typeP) 
//! \param nodeP pointer to the node this edge is pointing to
//! \param labelP label of this edge. A copy is made. Caller should delete/free
//!      his copy. graphEdge will in turn dealloc _its_ copy.
//! \param typeP type of this edge (receiving, sending)
//! \brief constructor
graphEdge::graphEdge(vertex * nodeP, char * labelP, edgeType typeP) {
	node = nodeP;
	label = strdup(labelP);
	type = typeP; 
	nextElement = NULL;	
}
 
//! \fn graphEdge::~graphEdge()
//! \brief destructor
graphEdge::~graphEdge() {
    free(label);
}

//! \fn void graphEdge::setNextElement(graphEdge * element)
//! \param element pointer to an edge
//! \brief sets the pointer of the nextelement of this edge to the edge given by the parameter
void graphEdge::setNextElement(graphEdge * element) {
 	nextElement = element;	
}

//! \fn void graphEdge::setNode(vertex * _node)
//! \param _node
//! \brief sets the pointer of the node to the node given by the parameter
void graphEdge::setNode(vertex * _node) {
 	node = _node;	
}

//! \fn  graphEdge * graphEdge::getNextElement()
//! \return pointer to the next edge in list
//! \brief returns pointer to the next edge in list
graphEdge * graphEdge::getNextElement() {
 	return nextElement;	
}
 
//! \fn string graphEdge::getLabel()
//! \return label of this edge
//! \brief returns the label of this edge
char * graphEdge::getLabel() {
 	return label;
}
 
//! \fn edgeType graphEdge::getType()
//! \return type of this edge (receiving, sending)
//! \brief returns the type of this edge (receiving, sending)
edgeType graphEdge::getType() {
 	return type;	
}
 
//! \fn vertex * graphEdge::getNode()
//! \return the node this edge is pointing to
//! \brief returns the node this edge is pointing to
vertex * graphEdge::getNode() {
 	return node;
} 

