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

#include "mynew.h"
#include <string>
#include <ostream>
#include <set>

#include "enums.h"

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
    
    void addClause(clause *, bool);
    void setAnnotationEdges(graphEdge *);
   
	int numberOfElementsInAnnotation();
 
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

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(vertex)
#define new NEW_NEW
    
};

#endif /*VERTEX_H_*/
