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

using namespace std;

class State;
class graphEdge;

struct compareChars {
  bool operator() (char const * left, char const * right) {
    return (strcmp(left, right) < 0);
  }
}; // compareVertices 


typedef std::set<graphEdge *> clauseSet;
typedef std::set<char *, compareChars> clauseSetLabel;


class reachGraphState {
	
protected:
	bool isMin;							//!< state is minimal or not
	
	clauseSetLabel clauseLabel;
		
public:
	reachGraphState(bool);
	reachGraphState(bool, State *);
	~reachGraphState();
	
	State * state;				//!< pointer to Karsten's state structure
	clauseSet clause1;
	
	bool isMinimal() ;	
	vertexColor calcColor();
	
	void addClauseElement(graphEdge *);
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
//	unsigned long checkSum;		//!< hash value of all the states in the list
    	
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
