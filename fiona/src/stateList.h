/*!
 * 	\class reachGraphState
    \brief class for storing the information of a state

	\author Daniela Weinberg
	\par History
		- 2005-11-18 creation
		- 2005-12-03 added comments
 */

#ifndef STATELIST_H_
#define STATELIST_H_

#include <iostream>
#include "enums.h"
#include <string>
#include <set>		// set
//#include "mynew.h"

using namespace std;

class State;
class graphEdge;

class clause {
public:
	clause(graphEdge *);
	~clause();	
	
	graphEdge * edge;
	clause * nextElement; 
	
	void setEdge(graphEdge *);
};


class reachGraphState {
	
protected:
	bool isMin;							//!< state is minimal or not
	clause * firstElement;
		
public:
	reachGraphState(bool);
	reachGraphState(bool, State *);
	~reachGraphState();
	
	State * state;				//!< pointer to Karsten's state structure

	bool isMinimal() ;	
	vertexColor calcColor();
	
//	void addClauseElement(graphEdge *);
	void addClauseElement(char *);
	
	string getClause();
	void setEdge(graphEdge *);
	
	friend bool operator == (const reachGraphState& s1, const reachGraphState& s2);
//	friend bool operator < (const reachGraphState& s1, const reachGraphState& s2);

    	friend ostream& operator << (std::ostream& os, const reachGraphState& s);	
};

struct Object_Compare {

  bool operator() ( reachGraphState const * left, reachGraphState const * right) {
    return (left->state < right->state);
  }

}; // Object_Compare 


typedef std::set<reachGraphState*, Object_Compare> reachGraphStateSet;

/*! 	
	\class stateList
    \brief a list storing the states of the node

	\author Daniela Weinberg
	\par History
		- 2005-11-17 creation
		- 2005-12-03 added comments
 */

class stateList {
	
private:
    	
public:
	stateList();
	~stateList();

	reachGraphStateSet setOfReachGraphStates; 

	bool addElement(State *, bool);
	bool findElement(State *, bool isMinimal);

	int elementCount() ;
	
	friend bool operator == (const stateList& s1, const stateList& s2);
	friend bool operator < (const stateList& s1, const stateList& s2);
    friend ostream& operator << (std::ostream& os, const stateList& v);	
};

#endif /*STATELIST_H_*/
