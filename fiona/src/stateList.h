// *****************************************************************************\
// * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
// *                                                                           *
// * This file is part of Fiona.                                               *
// *                                                                           *
// * Fiona is free software; you can redistribute it and/or modify it          *
// * under the terms of the GNU General Public License as published by the     *
// * Free Software Foundation; either version 2 of the License, or (at your    *
// * option) any later version.                                                *
// *                                                                           *
// * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
// * more details.                                                             *
// *                                                                           *
// * You should have received a copy of the GNU General Public License along   *
// * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
// * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
// *****************************************************************************/

/*!
 * \file    stateList.h
 *
 * \brief   functions for handling of states inside nodes of IG / OG
 *          aka "knowledge function"
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef STATELIST_H_
#define STATELIST_H_

#include "mynew.h"
#include <iostream>
#include "enums.h"
#include <string>
#include <set>		// set

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

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(clause)
#define new NEW_NEW
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

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(reachGraphState)
#define new NEW_NEW
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

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(stateList)
#define new NEW_NEW
};

#endif /*STATELIST_H_*/
