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

