/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
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
 * \file    state.h
 *
 * \brief   functions for Petri net states
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef STATE_H
#define STATE_H

#include "owfn.h"


class binDecision;
class owfnTransition;


// State objects are nodes in the state graph.
class State {
public:
	State();
	~State();
	static unsigned int card;         // number of states
	unsigned int CardFireList;        // number of transitions to be fired
	unsigned int CardStubbornFireList;        // number of transitions to be fired
	owfnTransition ** firelist;       // Transitions to be fired
	owfnTransition ** stubbornFirelist; // Transitions to be fired in the stubborn set
	owfnTransition ** quasiFirelist;  // Transitions that are quasi enabled
	unsigned int current;             // Nr of last already fired element of firelist
  	unsigned int * myMarking;       // current marking at that state
	binDecision * my_entry;           // last branch in binary decision tree that represents marking
	unsigned int placeHashValue;      // hashValue of that state (necessary for binDecision)
	State ** succ;                    // successor states in graph
	State * parent;                   // state responsible for first generation
	stateType type;                   // type of state (Deadlock, Final, Transient)
	void decode(oWFN *);      		// decode state into given marking vector
	void decodeShowOnly(oWFN *);		// decode state into given marking vector just for showing the marking!!!

// Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
  NEW_OPERATOR(State)
#define new NEW_NEW
};

#endif
