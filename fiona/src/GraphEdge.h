/*!
 * 	\class graphEdge
    \brief the edge between two nodes of the graph

	\author Daniela Weinberg
	\par History
		- 2005-09-13 creation
		- 2005-12-03 added comments
 */ 

#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include "enums.h"
#include <stddef.h>
#include <string>

using namespace std;


class vertex;


class graphEdge {


private:
    vertex * node;				/*!< pointer to the node this arc is pointing to */
    char * label;				/*!< label of the arc (usually the name of the event) */
    edgeType type;  			/*!< type of the arc (sending, receiving) */
    graphEdge * nextElement;	/*!< pointer to the next element in list */
    
public:
	graphEdge(vertex *, char *, edgeType);
	~graphEdge();	
	void setNextElement(graphEdge *);
	graphEdge * getNextElement();
	char * getLabel();
	edgeType getType();
	vertex * getNode();
};
#endif //GRAPHEDGE_H
