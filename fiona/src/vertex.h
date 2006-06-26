/*!
 * 	\class vertex
    \brief the nodes of the graph

	\author Daniela Weinberg
	\par History
		- 2005-09-13 creation
		- 2005-12-03 added comments
 */ 

#ifndef VERTEX_H_
#define VERTEX_H_

#include <string>
#include <ostream>
#include <map>

#include "enums.h"

#ifdef LOG_NEW
#include "mynew.h"
#endif

using namespace std;

class graphEdge;
class stateList;
class successorNodeList;


class vertex
{
protected:
	vertexColor color; 						//!< color of vertex
	successorNodeList * successorNodes;		//!< list of all the nodes succeeding this one 
											//!< including the edge between them
	stateList * states;						//!< list of states of this node

 //   successorNodeList * predecessorNodes;	//!< list of vertices that point to this vertex

    unsigned int numberOfVertex;					//!< number of this vertex in the graph
    
public:
	vertex();
	vertex(int);
	~vertex ();
	
	int * eventsUsed;
	int eventsToBeSeen;
	
    unsigned int getNumber();
    void setNumber(unsigned int);
    void setStateList(stateList *);
    // void addSuccessorNode(vertex *, char *, edgeType);
    void addSuccessorNode(graphEdge *);
    graphEdge * getNextEdge();
    successorNodeList * getSuccessorNodes();
 //   successorNodeList * getPredecessorNodes();
    
    void resetIteratingSuccNodes();
    
    stateList * getStateList();
    
   // void addPredecessorNode(vertex *, char *, edgeType);  
//    void addPredecessorNode(graphEdge *);  
    void setColor(vertexColor c);
    vertexColor getColor();
    int getNumberOfDeadlocks();
    
    friend bool operator == (vertex const&, vertex const& );		// could be changed and replaced by hash-value
    friend bool operator < (vertex const&, vertex const& );
    friend ostream& operator << (std::ostream& os, const vertex& v);

    
};

#endif /*VERTEX_H_*/
