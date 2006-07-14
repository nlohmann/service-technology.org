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
#include <set>

#include "enums.h"

#ifdef LOG_NEW
#include "mynew.h"
#endif

using namespace std;

class State;
class graphEdge;
class successorNodeList;
class clause;
class CNF;


struct StateCompare {

  bool operator() ( State const * left, State const * right) {
    return (left < right);
  }

}; // StateCompare 


typedef std::set<State*, StateCompare> StateSet;


class vertex
{
protected:
	vertexColor color; 						//!< color of vertex
	successorNodeList * successorNodes;		//!< list of all the nodes succeeding this one 
											//!< including the edge between them

 //   successorNodeList * predecessorNodes;	//!< list of vertices that point to this vertex

    unsigned int numberOfVertex;					//!< number of this vertex in the graph
    
    CNF * annotation;						//!< annotation of this node (a CNF)
    
public:
	vertex();
	vertex(int);
	~vertex ();
	
	int * eventsUsed;
	int eventsToBeSeen;
	
    unsigned int getNumber();
    
    StateSet setOfStates;
    
    void setNumber(unsigned int);
    void addSuccessorNode(graphEdge *);
    
    bool addState(State *);
    
    void addClause(clause *);
    void setAnnotationEdges(graphEdge *);
    
    graphEdge * getNextEdge();
    successorNodeList * getSuccessorNodes();
    
    void resetIteratingSuccNodes();
   
    void setColor(vertexColor c);
    vertexColor getColor();
    
    string getCNF();
    
    int getNumberOfDeadlocks();
    analysisResult analyseNode(bool);
    
    
//    friend bool operator == (vertex const&, vertex const& );		// could be changed and replaced by hash-value
    friend bool operator < (vertex const&, vertex const& );
//    friend ostream& operator << (std::ostream& os, const vertex& v);

    
};

#endif /*VERTEX_H_*/
