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
 * \file    owfn.cc
 *
 * \brief   general functions for internal Petri net representation
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "state.h"
#include "binDecision.h"
#include "options.h"
#include "debug.h"
#include "CNF.h"
#include "vertex.h"
#include "OGFromFile.h"
#include <stdlib.h>
#include <cassert>
#include <limits.h>
#include "owfnTransition.h"
#include "commGraphFormula.h"

using namespace std;

//comparison function, in order to sort the input/output place names 
int compare (const void * a, const void * b){
	owfnPlace *o1 = *((owfnPlace **)(a));
	owfnPlace *o2 = *((owfnPlace **)(b));
	assert(o1 != NULL);
	assert(o2 != NULL);
	return o1->name.compare(o2->name);
}

//! \fn oWFN::oWFN()
//! \brief constructor
oWFN::oWFN() : placeCnt(0), arcCnt(0), transCnt(0), filename(NULL),
               tempBinDecision(NULL),
               placeInputCnt(0), placeOutputCnt(0), FinalCondition(NULL),
               currentState(0), transNrEnabled(0), placeHashValue(0),
               BitVectorSize(0), startOfQuasiEnabledList(NULL),
               startOfEnabledList(NULL)
{
	unsigned int i;
  	NonEmptyHash = 0;
//	commDepth = 0;

  	try {
  		binHashTable = new binDecision * [HASHSIZE];
	} catch(bad_alloc) {
		char mess[] = "\nhash table too large!\n";
		//write(2,mess,sizeof(mess));
		cerr << mess;
		_exit(2);
	}
  	
  	for(i = 0; i < HASHSIZE; i++) {
          binHashTable[i] = (binDecision *) 0;
    } 
}


//! \fn oWFN::~oWFN()
//! \brief destructor
oWFN::~oWFN() {	
	trace(TRACE_5, "oWFN::~oWFN() : start\n");

	delete[] inputPlacesArray;
	delete[] outputPlacesArray;

	for(int i = 0; i < HASHSIZE; i++) {
		if (binHashTable[i]) {
			delete binHashTable[i];
		}
		binHashTable[i] = (binDecision *) 0;
	}	

	delete[] binHashTable;
	
	for(unsigned int i = 0; i < placeCnt; i++) {
		if (Places[i]) {
			delete Places[i];
		}
		Places[i] = NULL;
	}	
	
	delete[] Places;
	
	for(unsigned int i = 0; i < transCnt; i++) {
		if (Transitions[i]) {
			delete Transitions[i];
		}
		Transitions[i] = NULL;
	}	
	
	delete[] Transitions;
	delete[] CurrentMarking;
	delete[] FinalMarking;
	delete FinalCondition;
	delete tempBinDecision;

	trace(TRACE_5, "oWFN::~oWFN() : end\n");
}

//! \fn unsigned int oWFN::getPlaceCnt() const
//! \brief returns the number of all places of the net
unsigned int oWFN::getPlaceCnt() const {
	return placeCnt;
}


//! \fn unsigned int oWFN::getTransitionCnt()
//! \brief returns the number of transitions of the net
unsigned int oWFN::getTransitionCnt() {
	return transCnt;
}


//! \fn unsigned int oWFN::getInputPlaceCnt()
//! \brief returns the number of input places of the net
unsigned int oWFN::getInputPlaceCnt() {
	return placeInputCnt;
}


//! \fn unsigned int oWFN::getOutputPlaceCnt()
//! \brief returns the number of output places of the net
unsigned int oWFN::getOutputPlaceCnt() {
	return placeOutputCnt;
}


//! \fn oWFN::initialize()
//! \brief initializes the owfn; is called right after the parsing of the net file is done
void oWFN::initialize() {
	trace(TRACE_5, "oWFN::initialize(): start\n");
	unsigned int i;
	
	// initialize places hashes
	for(i = 0; i < placeCnt; i++) {
       Places[i]->set_hash(rand());
    }
    
    // initialize places
  	for(i=0; i < placeCnt; i++) {
		Places[i]->index = i;
        
        if ((Places[i]->type == INPUT) && (CurrentMarking[i] < Places[i]->capacity)) {
        	CurrentMarking[i] = Places[i]->capacity;
        } else {
        	CurrentMarking[i] = Places[i]->initial_marking;
        }
  	}

#ifdef STUBBORN
	// initialize array of pre-transitions
	for(i=0;i<placeCnt;i++)
	{
		for (unsigned int j = 0; j < Places[i]->NrOfArriving; j++)
		{
			Places[i]->PreTransitions.push_back(Places[i]->ArrivingArcs[j]->tr);
		}
	}
#endif
  	
	// initialize transitions
  	for(i = 0; i < transCnt; i++) {
		Transitions[i]->initialize();
  	}

    startOfEnabledList = NULL;
    PN->transNrEnabled = 0;
    startOfQuasiEnabledList = NULL;
    PN->transNrQuasiEnabled = 0;
  	
  	for(i=0; i < placeCnt; i++) {
        CurrentMarking[i] = Places[i]->initial_marking;
  	}
 
	for(unsigned int i = 0; i < transCnt; i++) {
		Transitions[i]->check_enabled(this);
  	}
  	
//  	for(i = 0, BitVectorSize = 0; i < placeCnt; i++) {
//        BitVectorSize += Places[i]->nrbits;
//  	}
  		
	unsigned int ki = 0;
	unsigned int ko = 0;
	
	inputPlacesArray = new owfnPlace* [placeInputCnt];
	outputPlacesArray = new owfnPlace* [placeOutputCnt];
	
	// get the data for those arrays from the places of the net
	for (i = 0; i < placeCnt; i++) { // getPlaceCnt(); i++) {
		if (Places[i]->type == INPUT) {
			// current place is from type input
			inputPlacesArray[ki++] = Places[i];
		} else if (Places[i]->type == OUTPUT) {
			// current place is from type output
			outputPlacesArray[ko++] = Places[i];
		}	
	}
	
	// sort the inputPlacesArray and the outputPlacesArray
	qsort (inputPlacesArray,  placeInputCnt,  sizeof(owfnPlace*), compare);
	qsort (outputPlacesArray, placeOutputCnt, sizeof(owfnPlace*), compare);	
	
	for(i = 0, BitVectorSize = 0; i < placeCnt; i++) {
        Places[i]->startbit = BitVectorSize;
        BitVectorSize += Places[i]->nrbits;
  	}
	
	trace(TRACE_5, "oWFN::initialize(): end\n");
}



void oWFN::initializeTransitions() {
	unsigned int i;
	 	
  	for(i = 0; i < transCnt; i++) {
		Transitions[i]->PrevEnabled = (i == 0 ? (owfnTransition *) 0 : Transitions[i-1]);
		Transitions[i]->NextEnabled = (i == transCnt - 1 ? (owfnTransition *) 0 : Transitions[i+1]);
 		Transitions[i]->setEnabled(true);
		Transitions[i]->PrevQuasiEnabled = (i == 0 ? (owfnTransition *) 0 : Transitions[i-1]);
		Transitions[i]->NextQuasiEnabled = (i == transCnt - 1 ? (owfnTransition *) 0 : Transitions[i+1]);
 		Transitions[i]->setQuasiEnabled(true);
  	}
	startOfEnabledList = (getTransitionCnt() > 0) ? Transitions[0] : NULL;

  	transNrEnabled = transCnt;

	startOfQuasiEnabledList = (getTransitionCnt() > 0) ? Transitions[0] : NULL;
  	transNrQuasiEnabled = transCnt; 
  	
	for(i = 0; i < transCnt; i++) {
		Transitions[i]->check_enabled(this);
  	}
}

void oWFN::removeisolated() {
	unsigned int i;

	owfnPlace * p;

	i=0;
	while(i<placeCnt) {
		if(Places[i]->references == 0) { // owfnPlace isolated
			p = Places[placeCnt - 1];
			int m = CurrentMarking[placeCnt - 1];
			Places[placeCnt - 1] = Places[i];
			CurrentMarking[placeCnt - 1] = CurrentMarking[i];
			Places[i] = p;
			CurrentMarking[i] = m;
			deletePlace(Places[placeCnt - 1]);
			// placeCnt --;
		} else {
			i++;
		}
	}
/*
	for(i=0;i<transCnt;i++) {
		Transitions[i]->nr = i;
	}
*/
	for(i=0;i<placeCnt;i++) {
		// Places[i]->nr = i;
		Places[i]->index = i;
	}
}


owfnTransition ** oWFN::firelist() {
	owfnTransition ** tl;
	owfnTransition * t;
	int i;
	tl = new owfnTransition * [transNrEnabled + 1];
	for(i = 0, t = startOfEnabledList; t; t = t->NextEnabled) {
		tl[i++] = t;
	}
	tl[i] = (owfnTransition *) 0;
	CardFireList = i;
	return tl;
}


owfnTransition ** oWFN::quasiFirelist() {
	owfnTransition ** tl;
	owfnTransition * t;
	int i;
	tl = new owfnTransition * [transNrQuasiEnabled + 1];
	for(i = 0, t = startOfQuasiEnabledList; t; t = t->NextQuasiEnabled) {
		tl[i++] = t;
	}
	tl[i] = (owfnTransition *) 0;
	CardQuasiFireList = i;

	return tl;
}


//! \fn void oWFN::addSuccStatesToList(vertex * n, State * currentState)
//! \param n the node to add the states to
//! \param currentState the currently added state
//! \brief decodes state, checks for message bound violation and adds successors recursively
void oWFN::addSuccStatesToList(vertex * n, State * currentState) {
	
	if (currentState != NULL) {
		assert(!n->addState(currentState));		// currentState was added to node before

		currentState->decodeShowOnly(this);		// decodes currently considered state
	
		// test decoded current marking if message bound k reached
		if (checkMessageBound()) {
			n->setColor(RED);
			trace(TRACE_3, "\t\t\t message bound violation detected in node " + intToString(n->getNumber()) + " (addSuccStatesToList)\n");
			return;
		}
		
		// add successors
		for(unsigned int i = 0; i < currentState->CardFireList; i++) {
			if (n->addState(currentState->succ[i])) {	// add current successor
				// its successors need only be added if state was not yet in current node
				addSuccStatesToList(n, currentState->succ[i]);
			}
		}
	}
}


//! \fn void oWFN::addSuccStatesToListStubborn(StateSet & stateSet, owfnPlace * outputPlace, State * currentState, vertex * n)
//! \param stateSet 
//! \param outputPlace
//! \param currentState
//! \param n
//! \brief decodes state, checks for message bound violation and adds successors recursively
void oWFN::addSuccStatesToListStubborn(StateSet & stateSet, owfnPlace * outputPlace, State * currentState, vertex * n) {
	
	if (currentState != NULL) {
		currentState->decodeShowOnly(this);		// decodes currently considered state
	
		if (CurrentMarking[outputPlace->index] > 0) {
			stateSet.insert(currentState);
			return ;
		}
	
		// test decoded current marking if message bound k reached
		if (checkMessageBound()) {
			n->setColor(RED);
			trace(TRACE_3, "\t\t\t message bound violation detected in node " + intToString(n->getNumber()) + " (addSuccStatesToList)\n");
			return;
		}
		
		// add successors
		for(unsigned int i = 0; i < currentState->CardStubbornFireList; i++) {
			// test if successor state has not yet been added to the state set
			if (stateSet.find(currentState->succ[i]) == stateSet.end()) {
				// its successors need only be added if state was not yet in current node
				addSuccStatesToListStubborn(stateSet, outputPlace, currentState->succ[i], n);
			}
		}
	}
}

//! \fn void oWFN::addSuccStatesToListStubborn(StateSet & stateSet, messageMultiSet messages, State * currentState, vertex * n)
//! \param stateSet
//! \param messages  
//! \param currentState
//! \param n
//! \brief decodes state, checks for message bound violation and adds successors recursively
void oWFN::addSuccStatesToListStubborn(StateSet & stateSet, messageMultiSet messages, State * currentState, vertex * n) {
	
	if (currentState != NULL) {
		currentState->decodeShowOnly(this);		// decodes currently considered state
	
		bool somePlaceNotMarked = false; // remember if a place is not marked at the current marking
		
		// shall we save this state? meaning, are the correct output places marked?
		for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
			if (CurrentMarking[Places[*iter]->index] == 0) {
				somePlaceNotMarked = true;
				break;
			}
		}
	
		if (!somePlaceNotMarked) {	// if all places are appropriatly marked, we save this state
			stateSet.insert(currentState);
			
			trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) : end\n");
			// nothing else to be done here
			return ;
			
		}
	
		// test decoded current marking if message bound k reached
		if (checkMessageBound()) {
			n->setColor(RED);
			trace(TRACE_3, "\t\t\t message bound violation detected in node " + intToString(n->getNumber()) + " (addSuccStatesToList)\n");
			return;
		}
		
		// add successors
		for(unsigned int i = 0; i < currentState->CardStubbornFireList; i++) {
			// test if successor state has not yet been added to the state set
			if (stateSet.find(currentState->succ[i]) == stateSet.end()) {
				// its successors need only be added if state was not yet in current state set
				addSuccStatesToListStubborn(stateSet, messages, currentState->succ[i], n);
			}
		}
	}
}

//! \fn bool oWFN::checkMessageBound()
//! \return returns true iff current marking VIOLATES message bound
//! \brief checks if message bound is violated by the current marking (for interface places only)
bool oWFN::checkMessageBound() {
	trace(TRACE_5, "oWFN::checkMessageBound(): start\n");
	// test marking of current state if message bound k reached
	if (options[O_MESSAGES_MAX] == true) {      // k-message-bounded set
		// test input places
		for (unsigned int i = 0; i < placeInputCnt; i++) {
			if (CurrentMarking[inputPlacesArray[i]->index] > messages_manual) {
				trace(TRACE_3, "\t\t\t checkMessageBound found violation for input place " + string(inputPlacesArray[i]->name) + "\n");
				trace(TRACE_5, "oWFN::checkMessageBound(): end\n");
				return true;
			}
		}
		// test output places
		for (unsigned int i = 0; i < placeOutputCnt; i++) {
			if (CurrentMarking[outputPlacesArray[i]->index] > messages_manual) {
				trace(TRACE_3, "\t\t\t checkMessageBound found violation for output place " + string(outputPlacesArray[i]->name) + "\n");
				trace(TRACE_5, "oWFN::checkMessageBound(): end\n");
				return true;
			}
		}
	}
	trace(TRACE_5, "oWFN::checkMessageBound(): end\n");
	return false;			// no violation found
}


//! \fn void oWFN::computeAnnotationOutput(vertex * node, State * currentState)
//! \param node the node that is calculated
//! \param currentState the state for which the appropriate clause is to be created if this state is DL or FS
//! \brief computes the CNF of the current node starting with the currentState, goes recursively through
//! all of its successor states 
void oWFN::computeAnnotationOutput(vertex * node, State * currentState) {
	trace(TRACE_5, "oWFN::computeAnnotation(vertex * node, State * currentState, unsigned int * markingPreviousState): start\n");

	// store this state in the node's temp set of state (storing all states of the node)
	setOfStatesTemp.insert(currentState);		

	// get the successor states	and compute their corresponding annotation
	if (currentState != NULL) {
		for(unsigned int i = 0; i < currentState->CardFireList; i++) {
			if (currentState->succ[i]) {
				computeAnnotationOutput(node, currentState->succ[i]);
			}
		}
	}
	trace(TRACE_5, "oWFN::computeAnnotation(vertex * node, State * currentState, unsigned int * markingPreviousState): end\n");
}


//! \fn void oWFN::computeAnnotationInput(vertex * node, State * currentState, bool isCurrentMarking)
//! \param node the node that is calculated
//! \param currentState the state for which the appropriate clause is to be created if this state is DL or FS
//! \param isCurrentMarking flag, whether this state is the currentMarking or not
//! \brief computes the CNF of the current node starting with the currentState, goes recursively through
//! all of its successor states 
void oWFN::computeAnnotationInput(vertex * node, State * currentState, bool isCurrentMarking) {
	trace(TRACE_5, "oWFN::computeAnnotationInput(vertex * node, State * currentState, unsigned int * markingPreviousState): start\n");
	
	unsigned int * marking = NULL;

	// store this state in the node's temp set of state (storing all states of the node)
	setOfStatesTemp.insert(currentState);		
	
	// check, whether this state is to be added to the node or not
	// we do this right here, because of the decode function that might have been called already
	// if the currentState is just the currentMarking, then we don't decode again ;-)
	if (!isCurrentMarking && placeOutputCnt > 0) {
		currentState->decodeShowOnly(this);
	}
	
	// get the successor states	and compute their corresponding annotation
	if (currentState != NULL && currentState->succ != NULL && currentState->succ[0] != NULL) {
		marking = copyCurrentMarking();	// save the marking of the current state since it is the parent state of its successors
		for (unsigned int i = 0; i < currentState->CardFireList; i++) {
			if (currentState->succ[i]) {
				computeAnnotationInput(node, currentState->succ[i], false);
			}
		}
		if (marking) {
			delete[] marking;
			marking = NULL;
		}
	}
	trace(TRACE_5, "oWFN::computeAnnotationInput(vertex * node, State * currentState, unsigned int * markingPreviousState): end\n");
}


unsigned int * oWFN::copyCurrentMarking() {
	unsigned int * copy = new unsigned int [placeCnt];

	for (unsigned int i = 0; i < placeCnt; i++) {
		copy[i] = CurrentMarking[i];
	}	
	return copy;
}


void oWFN::copyMarkingToCurrentMarking(unsigned int * copy) {

	for (unsigned int i = 0; i < getPlaceCnt(); i++) {
		CurrentMarking[i] = copy[i];
	}	
	
//	// after decoding the new marking for a place update the final condition
//	if (PN->FinalCondition) {
//		for (int currentplacenr = 0; currentplacenr < getPlaceCnt(); currentplacenr++) {
//		    for(int j=0; j < PN->Places[currentplacenr]->cardprop; j++) {
//				if (PN->Places[currentplacenr]->proposition != NULL) {
//				    PN->Places[currentplacenr]->proposition[j] -> update(PN->CurrentMarking[currentplacenr]);
//				}
//			}
//		}
//	}

	//initializeTransitions();
}


//! \fn void oWFN::calculateReachableStatesOutputEvent(vertex * n) 
//! \param n the node to be calculated in case of an output event
//! \brief calculates the reduced set of states of the new vertex in case of an output event
void oWFN::calculateReachableStatesOutputEvent(vertex * n) {
	// calculates the EG starting at the current marking
	trace(TRACE_5, "oWFN::calculateReachableStatesOutputEvent(vertex * n, bool minimal): start\n");

	State * CurrentState;
  	State * NewState;
  	
	CurrentState = binSearch(this);
	
	unsigned int * tempCurrentMarking = NULL;
	unsigned int tempPlaceHashValue;
	
	if (CurrentState != NULL) {
		// marking already has a state -> put it (and all its successors) into the node
		if (n->addState(CurrentState)) {
			computeAnnotationOutput(n, CurrentState);
		}
		trace(TRACE_5, "oWFN::calculateReachableStatesOutputEvent(vertex * n, bool minimal): end\n");
		return;
	}
	
	// the other case:
	// we have a marking which has not yet a state object assigned to it
	if (CurrentState == NULL) {
		CurrentState = binInsert(this);
	}
	
	CurrentState->firelist = stubbornfirelistdeadlocks();
	CurrentState->CardFireList = CardFireList;
	if (parameters[P_IG]) {
	    if (CurrentState->quasiFirelist) {
		delete [] CurrentState->quasiFirelist;
		CurrentState->quasiFirelist = NULL;
	    }
  	    CurrentState->quasiFirelist = quasiFirelist();
	}

	CurrentState->current = 0;
	CurrentState->parent = (State *) 0;
	assert(CurrentState->succ == NULL);
	CurrentState->succ = new State * [CardFireList+1];
	for (size_t istate = 0; istate != CardFireList+1; ++istate)
	{
		CurrentState->succ[istate] = NULL;
	}

	CurrentState->placeHashValue = placeHashValue;
	CurrentState->type = typeOfState();
	
	n->addState(CurrentState);
	setOfStatesTemp.insert(CurrentState);
	  	
	// building EG in a node
  	while(CurrentState) {
 
		if ((n->reachGraphStateSet.size() % 1000) == 0) {
			trace(TRACE_2, "\t current state count: " + intToString(n->reachGraphStateSet.size()) + "\n");
		}
	  	
		// no more transition to fire from current state?
		if (CurrentState->current < CurrentState->CardFireList) {
			// there is a next state that needs to be explored

			if (tempCurrentMarking) {
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
			}
			
			tempCurrentMarking = copyCurrentMarking();
			tempPlaceHashValue = placeHashValue;
			  		
	  		trace(TRACE_5, "fire transition\n");

			CurrentState->firelist[CurrentState->current]->fire(this);
			NewState = binSearch(this);
			
	  		if(NewState != NULL) {
		  		// Current marking already in bintree 
				trace(TRACE_5, "Current marking already in bintree \n");
				computeAnnotationOutput(n, NewState);
				
				copyMarkingToCurrentMarking(tempCurrentMarking);
				
				CurrentState->firelist[CurrentState->current]->backfire(this);
				
				placeHashValue = tempPlaceHashValue;
				
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
									
				assert(CurrentState->succ[CurrentState->current] == NULL);
				CurrentState->succ[CurrentState->current] = NewState;
	     		(CurrentState->current)++;
	    	} else {
				trace(TRACE_5, "Current marking new\n");
      			NewState = binInsert(this);
      			NewState->firelist = stubbornfirelistdeadlocks();
	      		NewState->CardFireList = CardFireList;
	      		if (parameters[P_IG]) {
				    if (NewState->quasiFirelist) {
						delete [] NewState->quasiFirelist;
						NewState->quasiFirelist = NULL;
				    }
			    	NewState->quasiFirelist = quasiFirelist();
	      		}
	      		NewState->current = 0;
	      		NewState->parent = CurrentState;

				assert(NewState->succ == NULL);
				NewState->succ =  new State * [CardFireList+1];
				for (size_t istate = 0; istate != CardFireList+1; ++istate)
				{
					NewState->succ[istate] = NULL;
				}

	      		NewState->placeHashValue = placeHashValue;
	      		NewState->type = typeOfState();
	      		
				assert(CurrentState->succ[CurrentState -> current] == NULL);
	      		CurrentState->succ[CurrentState->current] = NewState;

	      		CurrentState = NewState;
		      	
				computeAnnotationOutput(n, NewState);
				
				if (tempCurrentMarking) {
					delete[] tempCurrentMarking;
					tempCurrentMarking = NULL;
				}
	    	}
		// no more transition to fire
		} else {
	  		// close state and return to previous state
			trace(TRACE_5, "close state and return to previous state\n");
	  		CurrentState = CurrentState->parent;

	  		if(CurrentState) {			// there is a father to further examine
	      		CurrentState->decode(this);
	      		CurrentState->current++;
	    	}
		}
	}
	if (tempCurrentMarking) {
		delete[] tempCurrentMarking;	
	}
	trace(TRACE_5, "oWFN::calculateReachableStatesOutputEvent(vertex * n, bool minimal): end\n");
}

//! \fn void oWFN::calculateReachableStatesInputEvent(vertex * n, bool minimal) 
//! \param n the node to be calculated in case of an input event
//! \param minimal the current state is minimal in the vertex
//! \brief calculates the reduced set of states of the new vertex in case of an input event
void oWFN::calculateReachableStatesInputEvent(vertex * n, bool minimal) {
	// calculates the EG starting at the current marking
	trace(TRACE_5, "oWFN::calculateReachableStatesInputEvent(vertex * n, bool minimal): start\n");

	State * CurrentState;
  	State * NewState;
  	
	CurrentState = binSearch(this);
	
	unsigned int * tempCurrentMarking = NULL;
	unsigned int tempPlaceHashValue;
	
	if (CurrentState != NULL) {
		// marking already has a state -> put it (and all its successors) into the node
		if (n->addState(CurrentState)) {
			computeAnnotationInput(n, CurrentState, true);
		}
		trace(TRACE_5, "oWFN::calculateReachableStatesInputEvent(vertex * n, bool minimal): end\n");
		return;
	}
	
	// the other case:
	// we have a marking which has not yet a state object assigned to it
	if (CurrentState == NULL) {
		CurrentState = binInsert(this);
	}
	
	CurrentState->firelist = stubbornfirelistdeadlocks();
	CurrentState->CardFireList = CardFireList;
	if (parameters[P_IG]) {
	    if (CurrentState->quasiFirelist) {
		delete [] CurrentState->quasiFirelist;
		CurrentState->quasiFirelist = NULL;
	    }
  	    CurrentState->quasiFirelist = quasiFirelist();
	}

	CurrentState->current = 0;
	CurrentState->parent = (State *) 0;
	assert(CurrentState->succ == NULL);
	CurrentState->succ = new State * [CardFireList + 1];
	for (unsigned int i = 0; i < CardFireList + 1; i++) {
		CurrentState->succ[i] = NULL;
	}
	CurrentState->placeHashValue = placeHashValue;
	CurrentState->type = typeOfState();
	
	setOfStatesTemp.insert(CurrentState);
	
	n->addState(CurrentState);
	computeAnnotationInput(n, CurrentState, true);
	  	
	// building EG in a node
  	while(CurrentState) {
 
		if ((n->reachGraphStateSet.size() % 1000) == 0) {
			trace(TRACE_2, "\t current state count: " + intToString(n->reachGraphStateSet.size()) + "\n");
		}
	  	
		// no more transition to fire from current state?
		if (CurrentState->current < CurrentState->CardFireList) {
			// there is a next state that needs to be explored

			if (tempCurrentMarking) {
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
			}
			
			tempCurrentMarking = copyCurrentMarking();
			tempPlaceHashValue = placeHashValue;
			  		
	  		trace(TRACE_5, "fire transition\n");

			CurrentState->firelist[CurrentState->current]->fire(this);
			minimal = isMinimal();
			NewState = binSearch(this);
			
	  		if(NewState != NULL) {
		  		// Current marking already in bintree 
				trace(TRACE_5, "Current marking already in bintree \n");
				
				computeAnnotationInput(n, NewState, true);
				
				copyMarkingToCurrentMarking(tempCurrentMarking);
				
				CurrentState->firelist[CurrentState->current]->backfire(this);
				
				placeHashValue = tempPlaceHashValue;
				
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
									
				assert(CurrentState->succ[CurrentState->current] == NULL);
		   		CurrentState->succ[CurrentState->current] = NewState;
	     		(CurrentState->current)++;
	    	} else {
				trace(TRACE_5, "Current marking new\n");

      			NewState = binInsert(this);
      			NewState->firelist = stubbornfirelistdeadlocks();
	      		NewState->CardFireList = CardFireList;
	      		if (parameters[P_IG]) {
			    if (NewState->quasiFirelist) {
				delete [] NewState->quasiFirelist;
				NewState->quasiFirelist = NULL;
			    }
			    NewState->quasiFirelist = quasiFirelist();
	      		}
	      		NewState->current = 0;
	      		NewState->parent = CurrentState;
				assert(NewState->succ == NULL);
	      		NewState->succ =  new State * [CardFireList + 1];
				for (unsigned int i = 0; i < CardFireList + 1; i++) {
					NewState->succ[i] = NULL;	
				}
	      		NewState->placeHashValue = placeHashValue;
	      		NewState->type = typeOfState();
	      		
				assert(CurrentState->succ[CurrentState->current] == NULL);
	      		CurrentState->succ[CurrentState->current] = NewState;
	      		CurrentState = NewState;
		      		
				computeAnnotationInput(n, NewState, true);
				
				if (tempCurrentMarking) {
					delete[] tempCurrentMarking;
					tempCurrentMarking = NULL;
				}
	    	}
		// no more transition to fire
		} else {
	  		// close state and return to previous state
			trace(TRACE_5, "close state and return to previous state\n");
	  		CurrentState = CurrentState->parent;

	  		if(CurrentState) {			// there is a father to further examine
	      		CurrentState->decode(this);
	      		CurrentState->current++;
	    	}
		}
	}
	if (tempCurrentMarking) {
		delete[] tempCurrentMarking;	
	}
	trace(TRACE_5, "oWFN::calculateReachableStatesInputEvent(vertex * n, bool minimal): end\n");
}

//! \fn void oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n)
//! \param stateSet set of states
//! \param outputPlace the output place of the net that is associated with the receiving event for which the new vertex is calculated
//! \param n new vertex 
//! \brief calculates the set of states reachable from the current marking and stores them in the new vertex
//! this function is for the full IG and the OG since a single output place is considered
void oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) {

	// calculates the EG starting at the current marking
	trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) : start\n");

	// test current marking if message bound k reached
	if (checkMessageBound()) {
		n->setColor(RED);
		trace(TRACE_3, "\t\t\t message bound violated; color of node " + intToString(n->getNumber()) + " set to RED (calculateReachableStates, oben)\n");
		return;
	}
	
	
	State * CurrentState;
  	State * NewState;

	tempBinDecision = (binDecision *) 0;

	CurrentState = binSearch(this);
	
	unsigned int * tempCurrentMarking = NULL;
	unsigned int tempPlaceHashValue;
	
	if (CurrentState == NULL) {
		// we have a marking which has not yet a state object assigned to it
		CurrentState = binInsert(this);		// save current state to the global binDecision
		
		CurrentState = binSearch(tempBinDecision, this);
		CurrentState = binInsert(&tempBinDecision, this);  // save current state to the local binDecision 
		
		CurrentState->current = 0;
		CurrentState->parent = (State *) 0;

		// marking already has a state -> put it (and all its successors) into the node
//		addSuccessorStatesToSetStubborn(CurrentState, stateSet);
	//	trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace): end\n");
		//return;
	} else {
		CurrentState = binSearch(tempBinDecision, this);
		CurrentState = binInsert(&tempBinDecision, this);  // save current state to the local binDecision 
	}
	
	CurrentState->stubbornFirelist = stubbornfirelistmessage(outputPlace);
	CurrentState->CardStubbornFireList = CardFireList;

	if (CurrentState->succ == NULL) {
		CurrentState->succ = new State * [CardFireList+1];
		for (size_t istate = 0; istate != CardFireList+1; ++istate) {
			CurrentState->succ[istate] = NULL;
		}
	}
	
	CurrentState->placeHashValue = placeHashValue;
	CurrentState->type = typeOfState();
		
	// shall we save this state? meaning, is the correct output place marked?
	if (CurrentMarking[outputPlace->index] > 0) {
		stateSet.insert(CurrentState);
		
		trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) : end\n");
		// nothing else to be done here
		return ;
	}

	// building EG in a node
	while(CurrentState) {
	 
		// no more transition to fire from current state?
		if (CurrentState->current < CurrentState->CardStubbornFireList) {
			// there is a next state that needs to be explored
	
			if (tempCurrentMarking) {
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
			}
				
			tempCurrentMarking = copyCurrentMarking();
			tempPlaceHashValue = placeHashValue;
				  		
			// fire and reach next state
			CurrentState->stubbornFirelist[CurrentState->current]->fire(this);
			NewState = binSearch(tempBinDecision, this);
				
	  		if(NewState != NULL) {
		  		// Current marking already in local bintree 
				trace(TRACE_5, "Current marking already in local bintree \n");
				
				// shall we save this state? meaning, is the correct output place marked?
				if (CurrentMarking[outputPlace->index] > 0) {
					stateSet.insert(NewState);
				} else {
					// no it is not marked, so we take a look at its successor states
					addSuccStatesToListStubborn(stateSet, outputPlace, NewState, n);
				}
					
				copyMarkingToCurrentMarking(tempCurrentMarking);
				
				CurrentState->stubbornFirelist[CurrentState->current]->backfire(this);
				
				placeHashValue = tempPlaceHashValue;
				
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
									
				assert(CurrentState->succ[CurrentState->current] == NULL);
		   		CurrentState->succ[CurrentState->current] = NewState;
	     		(CurrentState->current)++;
	    	} else {
				trace(TRACE_5, "Current marking new\n");
      			NewState = binInsert(&tempBinDecision, this);
      			NewState->stubbornFirelist = stubbornfirelistmessage(outputPlace);
	      		NewState->CardStubbornFireList = CardFireList;
	      		
	      		NewState->current = 0;
	      		NewState->parent = CurrentState;
				if (NewState->succ == NULL) {
			      		NewState->succ =  new State * [CardFireList+1];
					for (size_t istate = 0; istate != CardFireList+1; ++istate)
					{
						NewState->succ[istate] = NULL;
					}
				}
	      		NewState->placeHashValue = placeHashValue;
	      		NewState->type = typeOfState();
	      		
				assert(CurrentState->succ[CurrentState -> current] == NULL);
	      		CurrentState->succ[CurrentState -> current] = NewState;
	      		CurrentState = NewState;
		      		
				// test current marking if message bound k reached
				if (checkMessageBound()) {
					n->setColor(RED);
					trace(TRACE_3, "\t\t\t message bound violated; color of node " + intToString(n->getNumber()) + " set to RED (calculateReachableStates, during fire)\n");
					trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) : end\n");
					return;
				}

				// shall we save this state? meaning, is the correct output place marked?
				if (CurrentMarking[outputPlace->index] > 0) {
					stateSet.insert(NewState);
			
					trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace) : end\n");
					// nothing else to be done here
					// so we go back to parent state
					// close state and return to previous state
					trace(TRACE_5, "close state and return to previous state\n");
	  				CurrentState = CurrentState->parent;

	  				if(CurrentState) {			// there is a father to further examine
	      					CurrentState->decode(this);
	      					CurrentState->current++;
	    				}
				}
				
				if (tempCurrentMarking) {
					delete[] tempCurrentMarking;
					tempCurrentMarking = NULL;
				}
	    	}
		// no more transition to fire
		} else {
	  		// close state and return to previous state
			trace(TRACE_5, "close state and return to previous state\n");
	  		CurrentState = CurrentState->parent;

	  		if(CurrentState) {			// there is a father to further examine
	      		CurrentState->decode(this);
	      		CurrentState->current++;
	    	}
		}
	}
	if (tempCurrentMarking) {
		delete[] tempCurrentMarking;	
	}
	
	//binDeleteAll(*tempBinDecision);
	
	trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) : end\n");
	return;
	
}

//! \fn void oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n)
//! \param stateSet set of states
//! \param outputPlace the output place of the net that is associated with the receiving event for which the new vertex is calculated
//! \param n new vertex 
//! \brief calculates the set of states reachable from the current marking and stores them in the new vertex
//! this function is for the IG only since a multiset of output places is considered
void oWFN::calculateReachableStates(StateSet& stateSet, messageMultiSet messages, vertex * n) {

	// calculates the EG starting at the current marking
	trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, messageMultiSet messages, vertex * n) : start\n");

	// test current marking if message bound k reached
	if (checkMessageBound()) {
		n->setColor(RED);
		trace(TRACE_3, "\t\t\t message bound violated; color of node " + intToString(n->getNumber()) + " set to RED (calculateReachableStates, oben)\n");
		return;
	}
	
	State * CurrentState;
  	State * NewState;

	tempBinDecision = (binDecision *) 0;

	CurrentState = binSearch(this);
	
	unsigned int * tempCurrentMarking = NULL;
	unsigned int tempPlaceHashValue;
	
	if (CurrentState == NULL) {
		// we have a marking which has not yet a state object assigned to it
		CurrentState = binInsert(this);		// save current state to the global binDecision
		
		CurrentState = binSearch(tempBinDecision, this);
		CurrentState = binInsert(&tempBinDecision, this);  // save current state to the local binDecision 
		
		CurrentState->current = 0;
		CurrentState->parent = (State *) 0;

		// marking already has a state -> put it (and all its successors) into the node
//		addSuccessorStatesToSetStubborn(CurrentState, stateSet);
	//	trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace): end\n");
		//return;
	} else {
		CurrentState = binSearch(tempBinDecision, this);
		CurrentState = binInsert(&tempBinDecision, this);  // save current state to the local binDecision 
	}
	
	CurrentState->stubbornFirelist = stubbornfirelistmessage(messages);
	CurrentState->CardStubbornFireList = CardFireList;

	if (CurrentState->succ == NULL) {
		CurrentState->succ = new State * [CardFireList+1];
		for (size_t istate = 0; istate != CardFireList+1; ++istate) {
			CurrentState->succ[istate] = NULL;
		}
	}
	
	CurrentState->placeHashValue = placeHashValue;
	CurrentState->type = typeOfState();
		
	bool somePlaceNotMarked = false; // remember if a place is not marked at the current marking
	
	if (tempCurrentMarking) {
		delete[] tempCurrentMarking;
		tempCurrentMarking = NULL;
	}
		
	tempCurrentMarking = copyCurrentMarking();
	
	// shall we save this state? meaning, are the correct output places all marked?
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
		if (tempCurrentMarking[Places[*iter]->index] == 0) {
			somePlaceNotMarked = true;
			break;
		} 
		tempCurrentMarking[Places[*iter]->index] -= 1;
	}

	if (!somePlaceNotMarked) {	// if all places are appropriatly marked, we save this state
		stateSet.insert(CurrentState);
		
		trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) : end\n");
		// nothing else to be done here
		if (tempCurrentMarking) {
			delete[] tempCurrentMarking;
			tempCurrentMarking = NULL;
		}
		return ;
	}

	// building EG in a node
	while(CurrentState) {
	 
		// no more transition to fire from current state?
		if (CurrentState->current < CurrentState->CardStubbornFireList) {
			// there is a next state that needs to be explored
	
			if (tempCurrentMarking) {
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
			}
				
			// remember the current marking
			tempCurrentMarking = copyCurrentMarking();
			tempPlaceHashValue = placeHashValue;
				  		
			// fire and reach next state
			CurrentState->stubbornFirelist[CurrentState->current]->fire(this);
			NewState = binSearch(tempBinDecision, this);
				
	  		if(NewState != NULL) {
		  		// Current marking already in local bintree 
				trace(TRACE_5, "Current marking already in local bintree \n");
				
				bool somePlaceNotMarked = false; // remember if a place is not marked at the current marking
	
				// shall we save this state? meaning, are the correct output places marked?
				for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
					if (CurrentMarking[Places[*iter]->index] == 0) {
						somePlaceNotMarked = true;
						break;
					}
					CurrentMarking[Places[*iter]->index] -= 1;
				}
			
				if (!somePlaceNotMarked) {	// if all places are appropriatly marked, we save this state
					stateSet.insert(CurrentState);
				} else {
					// no it is not marked appropriatly, so we take a look at its successor states
					addSuccStatesToListStubborn(stateSet, messages, NewState, n);
				}
					
				// we don't need the current marking anymore so we step back to the marking before
				copyMarkingToCurrentMarking(tempCurrentMarking);
				
				CurrentState->stubbornFirelist[CurrentState->current]->backfire(this);
				
				placeHashValue = tempPlaceHashValue;
				
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
									
				assert(CurrentState->succ[CurrentState->current] == NULL);
		   		CurrentState->succ[CurrentState->current] = NewState;
	     		(CurrentState->current)++;
	    	} else {
				trace(TRACE_5, "Current marking new\n");
      			NewState = binInsert(&tempBinDecision, this);
      			NewState->stubbornFirelist = stubbornfirelistmessage(messages);
	      		NewState->CardStubbornFireList = CardFireList;
	      		
	      		NewState->current = 0;
	      		NewState->parent = CurrentState;
				if (NewState->succ == NULL) {
			      	NewState->succ =  new State * [CardFireList+1];
					for (size_t istate = 0; istate != CardFireList+1; ++istate)
					{
						NewState->succ[istate] = NULL;
					}
				}
	      		NewState->placeHashValue = placeHashValue;
	      		NewState->type = typeOfState();
	      		
				assert(CurrentState->succ[CurrentState -> current] == NULL);
	      		CurrentState->succ[CurrentState -> current] = NewState;
	      		CurrentState = NewState;
		      		
				// test current marking if message bound k reached
				if (checkMessageBound()) {
					n->setColor(RED);
					trace(TRACE_3, "\t\t\t message bound violated; color of node " + intToString(n->getNumber()) + " set to RED (calculateReachableStates, during fire)\n");
					trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) : end\n");
					if (tempCurrentMarking != NULL) {
						delete[] tempCurrentMarking;
						tempCurrentMarking = NULL;
					}
					return;
				}

				bool somePlaceNotMarked = false; // remember if a place is not marked at the current marking

				if (tempCurrentMarking != NULL) {
					delete[] tempCurrentMarking;
					tempCurrentMarking = NULL;
				}
				tempCurrentMarking = copyCurrentMarking();
	
				// shall we save this state? meaning, are the correct output places marked?
				for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
					if (tempCurrentMarking[Places[*iter]->index] == 0) {
						somePlaceNotMarked = true;
						break;
					}
					tempCurrentMarking[Places[*iter]->index] -= 1;
				}
			
				if (!somePlaceNotMarked) {	// if all places are appropriatly marked, we save this state
					stateSet.insert(CurrentState);
				
					// nothing else to be done here
					// so we go back to parent state
					// close state and return to previous state
					trace(TRACE_5, "close state and return to previous state\n");
	  				CurrentState = CurrentState->parent;

	  				if(CurrentState) {			// there is a father to further examine
      					CurrentState->decode(this);
      					CurrentState->current++;
    				}
				}
				
				if (tempCurrentMarking) {
					delete[] tempCurrentMarking;
					tempCurrentMarking = NULL;
				}
	    	}
		// no more transition to fire
		} else {
	  		// close state and return to previous state
			trace(TRACE_5, "close state and return to previous state\n");
	  		CurrentState = CurrentState->parent;

	  		if(CurrentState) {			// there is a father to further examine
	      		CurrentState->decode(this);
	      		CurrentState->current++;
	    	}
		}
	}
	if (tempCurrentMarking) {
		delete[] tempCurrentMarking;
		tempCurrentMarking = NULL;
	}
	
	//binDeleteAll(*tempBinDecision);
	
	trace(TRACE_5, "oWFN::calculateReachableStates(StateSet& stateSet, owfnPlace * outputPlace, vertex * n) : end\n");
	return;
}

//! \fn void oWFN::calculateReachableStatesFull(vertex * n)
//! \param n the node for which the reachability graph is computed
//! \brief NO REDUCTION! calculate all reachable states from the current marking
//! and store them in the node n (== vertex of communicationGraph);
//! it will color the node n RED if a given message bound is violated
void oWFN::calculateReachableStatesFull(vertex * n) {

	// calculates the EG starting at the current marking
	trace(TRACE_5, "oWFN::calculateReachableStatesFull(vertex * n) : start\n");

	// test current marking if message bound k reached
	if (checkMessageBound()) {
		n->setColor(RED);
		trace(TRACE_3, "\t\t\t message bound violated; color of node " + intToString(n->getNumber()) + " set to RED (calculateReachableStatesFull, oben)\n");
		return;
	}
	
	State * CurrentState;
  	State * NewState;
  	
	CurrentState = binSearch(this);
	
	unsigned int * tempCurrentMarking = NULL;
	unsigned int tempPlaceHashValue;
	
//	if (options[O_BDD] == false && CurrentState != NULL) {

	if (CurrentState != NULL) {
		// marking already has a state -> put it with all successors into the node

		if (n->addState(CurrentState)) {
			// successors need only be added if state was not yet in current node
			addSuccStatesToList(n, CurrentState);	// decodes and checks for message bound
		}
		trace(TRACE_5, "oWFN::calculateReachableStatesFull(vertex * n) : end (root marking of EG already in bintree; states copied only)\n");
		return;
	} else {
	
		// the other case:
		// we have a marking which has not yet a state object assigned to it
		CurrentState = binInsert(this);
	
		CurrentState->firelist = firelist();
		CurrentState->CardFireList = CardFireList;
	
		if (parameters[P_IG]) {
		    if (CurrentState->quasiFirelist) {
				delete [] CurrentState->quasiFirelist;
				CurrentState->quasiFirelist = NULL;
		    }
	  	    CurrentState->quasiFirelist = quasiFirelist();
		}
	
		CurrentState->current = 0;
		CurrentState->parent = (State *) 0;
		assert(CurrentState->succ == NULL);
	
		CurrentState->succ = new State * [CardFireList+1];
		for (size_t istate = 0; istate != CardFireList+1; ++istate)
		{
			CurrentState->succ[istate] = NULL;
		}
		
		CurrentState->placeHashValue = placeHashValue;
		CurrentState->type = typeOfState();
		n->addState(CurrentState);
		
		// building EG in a node
	  	while(CurrentState) {
	 
//			if ((n->reachGraphStateSet.size() % 1000) == 0) {
//				trace(TRACE_2, "\t current state count: " + intToString(n->reachGraphStateSet.size()) + "\n");
//			}
//		  	
			// no more transition to fire from current state?
			if (CurrentState->current < CurrentState->CardFireList) {
				// there is a next state that needs to be explored
	
				if (tempCurrentMarking) {
					delete[] tempCurrentMarking;
					tempCurrentMarking = NULL;
				}
				
				tempCurrentMarking = copyCurrentMarking();
				tempPlaceHashValue = placeHashValue;
				  		
				// fire and reach next state
				CurrentState->firelist[CurrentState->current]->fire(this);
				NewState = binSearch(this);
				
		  		if(NewState != NULL) {
			  		// Current marking already in bintree 
					trace(TRACE_5, "Current marking already in bintree \n");
					if (n->addState(NewState)) {
						addSuccStatesToList(n, NewState);
						if (n->getColor() == RED) {
							trace(TRACE_3, "\t\t\t message bound violated; color of node " + intToString(n->getNumber()) + " set to RED (calculateReachableStatesFull, during fire)\n");
							trace(TRACE_5, "oWFN::calculateReachableStatesFull(vertex * n) : end\n");
							return;
						}
					}
					
					copyMarkingToCurrentMarking(tempCurrentMarking);
					
					CurrentState->firelist[CurrentState->current]->backfire(this);
					
					placeHashValue = tempPlaceHashValue;
					
					delete[] tempCurrentMarking;
					tempCurrentMarking = NULL;
										
					assert(CurrentState->succ[CurrentState->current] == NULL);
			   		CurrentState->succ[CurrentState->current] = NewState;
		     		(CurrentState->current)++;
		    	} else {
					trace(TRACE_5, "Current marking new\n");
	      			NewState = binInsert(this);
	      			NewState->firelist = firelist();
		      		NewState->CardFireList = CardFireList;
		      		if (parameters[P_IG]) {
					    if (NewState->quasiFirelist) {
							delete [] NewState->quasiFirelist;
							NewState->quasiFirelist = NULL;
					    }
					    NewState->quasiFirelist = quasiFirelist();
		      		}
		      		NewState->current = 0;
		      		NewState->parent = CurrentState;
					assert(NewState->succ == NULL);
		      		NewState->succ =  new State * [CardFireList+1];
					for (size_t istate = 0; istate != CardFireList+1; ++istate)
					{
						NewState->succ[istate] = NULL;
					}
		      		NewState->placeHashValue = placeHashValue;
		      		NewState->type = typeOfState();
		      		
					assert(CurrentState->succ[CurrentState -> current] == NULL);
		      		CurrentState->succ[CurrentState -> current] = NewState;
		      		CurrentState = NewState;
			      		
					// test current marking if message bound k reached
					if (checkMessageBound()) {
						n->setColor(RED);
						trace(TRACE_3, "\t\t\t message bound violated; color of node " + intToString(n->getNumber()) + " set to RED (calculateReachableStatesFull, during fire)\n");
						trace(TRACE_5, "oWFN::calculateReachableStatesFull(vertex * n) : end\n");
						return;
					}

					n->addState(NewState);
					
					if (tempCurrentMarking) {
						delete[] tempCurrentMarking;
						tempCurrentMarking = NULL;
					}
		    	}
			// no more transition to fire
			} else {
		  		// close state and return to previous state
				trace(TRACE_5, "close state and return to previous state\n");
		  		CurrentState = CurrentState->parent;
	
		  		if(CurrentState) {			// there is a father to further examine
		      		CurrentState->decode(this);
		      		CurrentState->current++;
		    	}
			}
		}
		if (tempCurrentMarking) {
			delete[] tempCurrentMarking;	
		}
		trace(TRACE_5, "oWFN::calculateReachableStatesFull(vertex * n) : end\n");
		return;
	}
}


//! \fn int oWFN::addInputMessage(unsigned int message)
//! \param message message to be added to the currentmarking
//! \brief adds input message to the current marking
void oWFN::addInputMessage(unsigned int message) {

	CurrentMarking[message]++;

	placeHashValue += Places[message]->hash_factor;
	placeHashValue %= HASHSIZE;

	for (int k = 0; k < Places[message]->NrOfLeaving; k++) {
		((owfnTransition *) Places[message]->LeavingArcs[k]->Destination)->check_enabled(this);
	}
	return;
}


//! \fn int oWFN::addInputMessage(messageMultiSet messages)
//! \param messages multiset of messages to be added to the currentmarking
//! \brief adds input messages to the current marking
int oWFN::addInputMessage(messageMultiSet messages) {
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
		if (Places[*iter]->type == INPUT) {
			// found that place
			CurrentMarking[*iter]++;
			placeHashValue += Places[*iter]->hash_factor;
			placeHashValue %= HASHSIZE;

			// TODO: check_enabled!!!!!!!! so richtig?!
			for (int k = 0; k < Places[*iter]->NrOfLeaving; k++) {
				((owfnTransition *) Places[*iter]->LeavingArcs[k]->Destination)->check_enabled(this);
			}
		//	return 0;
		}
	}
	return 1;		// place not found
}

void oWFN::printCurrentMarking() const {
    printMarking(CurrentMarking);
}

void oWFN::printMarking(unsigned int * marking) const {
    cout << "---------------------------------------------------------------"
         << endl;
    cout << "\t(" << marking << ")"
         << "\t[" << getMarkingAsString(marking) << "]" << endl
         << "---------------------------------------------------------------"
         << endl;
}


string oWFN::getMarkingAsString(unsigned int * marking) const {
    bool comma = false;
    string buffer;
    for (unsigned int i = 0; i < placeCnt; i++) {
        if (marking[i] > 5) {
            if (comma) {
                buffer += ", ";
            }
            buffer += Places[i]->name;
            buffer += ":";
            buffer += intToString(marking[i]);
            comma = true;
        } else {
            for (unsigned int k = 0; k < marking[i]; k++) {
                if (comma) {
                    buffer += ", ";
                }
                buffer += Places[i]->name;
                comma = true;
            }
        }
    }
    return buffer;
}

string oWFN::getCurrentMarkingAsString() const {
    return getMarkingAsString(CurrentMarking);
}


void oWFN::print_binDec(int h) {

	for (unsigned int i=0; i < placeCnt; i++) {
		cout << Places[i] -> name << ": " << Places[i] -> nrbits;
	}
	cout << endl;
	
	print_binDec(binHashTable[h], 0);
}
	
	
void oWFN::print_binDec(binDecision * d, int indent) {
	int i;
	// print bin decision table at hash entry h

	for(i=0; i<indent; i++) cout << ".";
	
	if(!d) {
		cout << " nil " << endl;
		return;
	}
	
	cout << "b " << d -> bitnr << " v ";
	for(i=0; i< (BitVectorSize - (d -> bitnr + 1)) ; i+=8) {
		cout << (unsigned int) (d -> vector)[i/8] << " " ;
	}

	for(i=0; i<BitVectorSize - (d -> bitnr+1); i++) {
		cout << (((d->vector)[i/8] >> (7-i%8))%2);
	}
	cout << endl;
	print_binDec(d -> nextold, indent+1);
	print_binDec(d -> nextnew, indent+1);
}


unsigned int oWFN::getPlaceHashValue() {
	return placeHashValue;
}


void oWFN::addTransition(unsigned int i, owfnTransition * transition) {
	transCnt++;
	Transitions[i] = transition;
	if(!(transCnt % REPORTFREQUENCY)) {
		cerr << "\n" << transCnt << "transitions parsed\n";
  	}
}


void oWFN::addPlace(unsigned int i, owfnPlace * place) {
	placeCnt++;
	if (place->type == INPUT) {
		PN->placeInputCnt++;
	} else if (place->type == OUTPUT) {
		PN->placeOutputCnt++;
	}
	Places[i] = place;
	place->index = i;
	
	if(!(placeCnt % REPORTFREQUENCY)) {
		cerr << "\n" << placeCnt << "places parsed\n";
  	}
}

void oWFN::RemoveGraph() {
	int i;

	for(i = 0; i < HASHSIZE; i++) {
		if(binHashTable[i]) {
			delete binHashTable[i];
		}
		binHashTable[i] = (binDecision *) 0;
	}
}


//! \fn bool oWFN::removeOutputMessage(unsigned int message)
//! \param message name of message to be deleted from currentmarking
//! \brief removes an output message from the current marking, if an output place was found the marking on that
//! place is decreased by one and the function returns 1
bool oWFN::removeOutputMessage(unsigned int message) {

	if (CurrentMarking[message] > 0) {
		CurrentMarking[message]--;
		placeHashValue -= Places[message]->hash_factor;
		placeHashValue %= HASHSIZE;
		
		// TODO: check_enabled!!!!!!!! so richtig?!
		for (int k = 0; k < Places[message]->NrOfLeaving; k++) {
			((owfnTransition *) Places[message]->LeavingArcs[k]->Destination)->check_enabled(this);
		}
		return true;	
	} 
	return false;
}	

//! \fn bool oWFN::removeOutputMessage(messageMultiSet messages)
//! \param messages multiset of messages to be deleted from currentmarking
//! \brief removes output messages from the current marking, if an output place was found the marking on that
//! place is decreased by one and the function returns 1
bool oWFN::removeOutputMessage(messageMultiSet messages) {
	unsigned int found = 0;
	
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {

		if (Places[*iter]->type == OUTPUT) {
			if (CurrentMarking[*iter] > 0) {
				found++;
				// found that place
				CurrentMarking[*iter]--;
				placeHashValue -= Places[*iter]->hash_factor;
				placeHashValue %= HASHSIZE;
				
				// TODO: check_enabled!!!!!!!! so richtig?!
				for (int k = 0; k < Places[*iter]->NrOfLeaving; k++) {
					((owfnTransition *) Places[*iter]->LeavingArcs[k]->Destination)->check_enabled(this);
				}	
			}
		} 
	}
	return messages.size() == found;	
}


void oWFN::deletePlace(owfnPlace * place) {
	delete place;
	placeCnt--;
}


void oWFN::deleteTransition(owfnTransition * transition) {
	delete transition;
	transCnt--;
}


//! \fn stateType oWFN::typeOfState()
//! \brief returns type of state: {TRANS, DEADLOCK, FINALSTATE, NN}
stateType oWFN::typeOfState() {
	trace(TRACE_5, "oWFN::typeOfState() : start\n");
	if (isFinal()) {			// state is final
		return FINALSTATE;
	}
	if (transNrEnabled == 0) {
		trace(TRACE_5, "oWFN::typeOfState() : end\n");	
		return DEADLOCK;		// state is an internal deadlock, no transition is enabled or quasi-enabled
	} else if (transNrEnabled != 0) {
		trace(TRACE_5, "oWFN::typeOfState() : end\n");	
		return TRANS;		// state is transient since there are transitions which are enabled
	}
	
	trace(TRACE_5, "oWFN::typeOfState() : end\n");	
	return NN;
}


//! \fn bool oWFN::isMinimal()
//! \brief returns true, if current state is minimal
bool oWFN::isMinimal() {
	for (unsigned int i = 0; i < placeCnt; i++) {
		if (Places[i]->type == OUTPUT && CurrentMarking[i] > 0) {
			return true;
		}
	}
	return false;
}


//! \fn bool oWFN::isMaximal()
//! \brief returns true, if current state is maximal
bool oWFN::isMaximal() {
	return transNrEnabled == 0;			
}


//! \fn bool oWFN::isFinal() const
//! \brief checks if the current marking satisfies final condition or final marking, resp.
//! if a final condition is given, a possible final marking is ignored
bool oWFN::isFinal() const {
	trace(TRACE_5, "oWFN::bool oWFN::isFinal() : start\n");
	if(FinalCondition) {
		for (unsigned int currentplacenr = 0; currentplacenr < getPlaceCnt(); currentplacenr++) {
		    for (unsigned int j=0; j < PN->Places[currentplacenr]->cardprop; j++) {
				if (PN->Places[currentplacenr]->proposition != NULL) {
				    PN->Places[currentplacenr]->proposition[j] -> update(PN->CurrentMarking[currentplacenr]);
				}
			}
		}
		trace(TRACE_5, "oWFN::bool oWFN::isFinal() : end\n");
		return FinalCondition -> value;
	} else {
		for (unsigned int i = 0; i < getPlaceCnt(); i++) {
			if (Places[i]->type != INTERNAL && CurrentMarking[i] > 0) {
				trace(TRACE_5, "oWFN::bool oWFN::isFinal() : end\n");
				return false;
			}
//			if (FinalMarking[i] != 0 && marking[i] != FinalMarking[i]) {
			if (FinalMarking[i] != 0 && CurrentMarking[i] < FinalMarking[i]) {
				trace(TRACE_5, "oWFN::bool oWFN::isFinal() : end\n");
				return false;	
			}
		}
		trace(TRACE_5, "oWFN::bool oWFN::isFinal() : end\n");
		return true;
	}
	trace(TRACE_5, "oWFN::bool oWFN::isFinal() : end\n");
}


string oWFN::createLabel(messageMultiSet m) const {
	string label;
	bool comma = false;
	
	for (messageMultiSet::iterator iter1 = m.begin(); iter1 != m.end(); iter1++) {
		if (comma) {
			label += ", ";
		}
		label += Places[*iter1]->name;
		comma = true;
	}

	return label;
}


// ****************** Implementation of Stubborn Set Calculation *******************

#ifdef STUBBORN


void stubbornclosure(oWFN * net)
{
	owfnTransition * current;

	net->NrStubborn = 0;
	for(current = net -> StartOfStubbornList;current; current = current -> NextStubborn)
	{
		if (current->isEnabled())
		{
			net -> NrStubborn ++;
		}
		for (unsigned int i = 0; i != current->mustbeincluded.size(); i++)
		{
			if(!current -> mustbeincluded[i]->instubborn)
			{
				current -> mustbeincluded[i]->instubborn = true;
				current -> mustbeincluded[i]->NextStubborn  = (owfnTransition *) 0;
				net -> EndOfStubbornList -> NextStubborn = current -> mustbeincluded[i];
				net -> EndOfStubbornList = current -> mustbeincluded[i];
			}
		}
	}
}
	

void stubborninsert(oWFN * net, owfnTransition* t)
{
	if(t -> instubborn) return;
	t -> instubborn = true;
	t -> NextStubborn = (owfnTransition *) 0;
	if(net -> StartOfStubbornList)
	{
		net -> EndOfStubbornList -> NextStubborn = t;
		net -> EndOfStubbornList = t;
	}
	else
	{
		net -> StartOfStubbornList = net -> EndOfStubbornList = t;
	}
}
		
//! \fn owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess)
//! \param mess output message that is to be received
//! \brief returns transitions to be fired for reduced state space for message successors		
owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess) {
	
	trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): begin\n");
	
	owfnTransition ** result;
	owfnTransition * t;
	unsigned int i;
	
	if (mess->PreTransitions.empty())
	{	
		result = new owfnTransition * [1];
		result[0] = (owfnTransition *) 0;
		trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): end\n");
		return result;
	}
	StartOfStubbornList = (owfnTransition *) 0;
	for(i = 0; i != mess->PreTransitions.size(); i++)
	{
		stubborninsert(this,mess->PreTransitions[i]);
	}
	stubbornclosure(this);
	result = new owfnTransition * [NrStubborn + 1];
	for(t=StartOfStubbornList, i=0;t; t = t -> NextStubborn)
	{
		t -> instubborn = false;
		if (t->isEnabled())
		{
			result[i++] = t;
		}
	}
	result[i] = (owfnTransition *)0;
	CardFireList = NrStubborn;
	StartOfStubbornList = (owfnTransition *) 0;
	
	trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): end\n");
	return result;
}

//! \fn owfnTransition ** oWFN::stubbornfirelistmessage(messageMultiSet messages)
//! \param messages multiset of output messages that are to be received
//! \brief returns transitions to be fired for reduced state space for messages successors
owfnTransition ** oWFN::stubbornfirelistmessage(messageMultiSet messages) {
	
	trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): begin\n");
	
	owfnTransition ** result;
	owfnTransition * t;
	unsigned int i;
	
	bool noPreTransitions = true;
	
	// check if any of the output messages has a pre-transition associated with it
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
		if (!Places[*iter]->PreTransitions.empty()) {
			noPreTransitions = false;
			break;
		}
	}
	
	// none of the transitions that correspond to the output messages has a pre-transition
	if (noPreTransitions) {
		result = new owfnTransition * [1];
		result[0] = (owfnTransition *) 0;
		trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): end\n");
		return result;
	}
	
	StartOfStubbornList = (owfnTransition *) 0;
	
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
		for(i = 0; i != Places[*iter]->PreTransitions.size(); i++) {
			stubborninsert(this, Places[*iter]->PreTransitions[i]);
		}
	}
	
	stubbornclosure(this);
	result = new owfnTransition * [NrStubborn + 1];
	for(t=StartOfStubbornList, i=0;t; t = t -> NextStubborn) {
		t -> instubborn = false;
		if (t->isEnabled()) {
			result[i++] = t;
		}
	}
	result[i] = (owfnTransition *)0;
	CardFireList = NrStubborn;
	StartOfStubbornList = (owfnTransition *) 0;
	
	trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): end\n");
	return result;
}	
	
unsigned int StubbStamp = 0;

void NewStubbStamp(oWFN * PN) {
	if(StubbStamp < UINT_MAX) {
		StubbStamp ++;
	}
	else {
		unsigned int i;
		for(i=0;i < PN->getTransitionCnt();i++) {
			PN->Transitions[i]-> stamp =0;
		}
		StubbStamp = 1;
	}
}

#define MINIMUM(X,Y) ((X) < (Y) ? (X) : (Y))
owfnTransition ** oWFN::stubbornfirelistdeadlocks() {
	trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistdeadlocks(): start\n"); 
	
	owfnTransition ** result;
	unsigned int maxdfs;
	owfnTransition * current, * next;

	// computes stubborn set without goal orientation.
	// The TSCC based optimisation is included

	// 1. start with enabled transition
	if(TarjanStack = startOfEnabledList)
	{
		maxdfs = 0;
		NewStubbStamp(this);
		TarjanStack -> nextontarjanstack = TarjanStack;
		TarjanStack -> stamp  = StubbStamp;
		TarjanStack -> dfs = TarjanStack -> min = maxdfs++;
		TarjanStack -> mbiindex = 0;
		current = TarjanStack;
		CallStack = current;
		current -> nextoncallstack = (owfnTransition *) 0;
	}
	else
	{
		result = new owfnTransition * [1];
		result[0] = (owfnTransition *) 0;
		CardFireList = 0;

		trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistdeadlocks(): end\n");
		return result;
	}
	while(current)
	{
	//	cout << "current: " << current->name << endl;
	/*	cout << "current->mustbeincluded: " << current->mustbeincluded << endl;
		cout << "current->mustbeincluded[0]: " << current->mustbeincluded[0] << endl;		*/
		if (current->mustbeincluded.size() > current->mbiindex)
		{
			next = current->mustbeincluded[current->mbiindex];
			// Successor exists
			if(next->stamp == StubbStamp)
			{
				// already visited
				if(next -> nextontarjanstack)
				{
					// next still on stack
					current -> min = MINIMUM(current -> min, next -> dfs);
				}
				current -> mbiindex++;
			}
			else
			{
				// not yet visited
				next -> nextontarjanstack = TarjanStack;
				TarjanStack = next;
				next -> min = next -> dfs = maxdfs++;
				next -> stamp = StubbStamp;
				next -> mbiindex = 0;
				next -> nextoncallstack = current;
				CallStack = next;
				current = next;
			}
		}
		else
		{
			// no more successors -> scc detection and backtracking
			if(current -> dfs == current -> min)
			{
				// remove all states behind current from Tarjanstack;
				// if enabled -> final sequence
				while(1)
				{
					if (TarjanStack->isEnabled())
					{
						// final sequence
						unsigned int cardstubborn;
						owfnTransition * t;

						cardstubborn = 0;
						for(t = TarjanStack;;t = t -> nextontarjanstack)
						{
							if (t->isEnabled()) cardstubborn ++;
							if(t == current) break;
						}
						result = new owfnTransition * [cardstubborn + 1];
						cardstubborn = 0;
						for(t = TarjanStack;;t = t -> nextontarjanstack)
						{
							if (t->isEnabled())
							{
								result[cardstubborn++] = t;
							}
							if(t == current)
							{
								result[cardstubborn] = (owfnTransition *) 0;
								CardFireList = cardstubborn;
								trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistdeadlocks(): end\n");
								return(result);
							}
						}
					}
					else
					{
						if(TarjanStack == current) break;
						TarjanStack = TarjanStack -> nextontarjanstack;
					}
				}
			}
		//	cout << "current -> nextoncallstack: " << current -> nextoncallstack << endl;
			// backtracking to previous state
			next = current -> nextoncallstack;
			next -> min = MINIMUM(current -> min, next -> min);
			next -> mbiindex++;
			current = next;
		}
	}
	trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistdeadlocks(): end\n");
}
#endif


bool oWFN::matchesWithOG(const OGFromFile& og, string& reasonForFailedMatch) {
    // A temporary copy of the CurrentMarking. Used to revert to the
    // CurrentMarking if firing of a transition leads to an already seen
    // marking.
    unsigned int* tmpCurrentMarking = NULL;
    unsigned int tmpPlaceHashValue;

    // Check whether the initial marking violates the message bound and exit
    // with an error message if it does.
    if (checkMessageBound()) {
        reasonForFailedMatch = "Current marking: '" +
            getCurrentMarkingAsString() + "' violated message bound.";
        return false;
    }

    // Initialize the currentState with the initial marking.
    State* currentState = binInsert(this);
    currentState->firelist = firelist();
    currentState->CardFireList = CardFireList;
    currentState->current = 0;
    currentState->parent = NULL;
    currentState->succ = new State*[CardFireList + 1];
    for (size_t istate = 0; istate != CardFireList + 1; ++istate) {
        currentState->succ[istate] = NULL;
    }
    currentState->placeHashValue = placeHashValue;
    currentState->type = typeOfState();

    // Initialize the currentOGNode with the root node of the OG.
    OGFromFileNode* currentOGNode = og.getRoot();

    // In this loop, we build the reachability graph of the oWFN and check
    // whether it matches with the OG.
    while (currentState) {
        // currentState is non-NULL. So we continue the depth first search from
        // currentState.

        // Check whether there are still enabled transitions to be fired in
        // currentState (transitions that have not been processed yet while
        // building the reachability graph). currentState->current holds the
        // index of the last fired transition in the array of to be fired
        // transitions currentState->firelist[].
        if (currentState->current < currentState->CardFireList) {
            // There is a not yet fired transition in currentState.

            // Retrieve the transition leaving the current state that should be
            // fired next.
            owfnTransition* transition =
                currentState->firelist[currentState->current];

            // Save the current marking, so we can easily revert to it if
            // firing the transition that we will try in this if-branch leads
            // us to a state we have already seen.
            if (tmpCurrentMarking != NULL) {
                delete[] tmpCurrentMarking;
                tmpCurrentMarking = NULL;
            }
            tmpCurrentMarking = copyCurrentMarking();
            tmpPlaceHashValue = placeHashValue;

            // Check whether the transition to be fired is also present in the
            // OG. If not, exit this function returning false, because the oWFN
            // does not match with the OG.
            if (transition->hasNonTauLabelForMatching() &&
                !currentOGNode->hasTransitionWithLabel(
                    transition->getLabelForMatching()))
            {
                reasonForFailedMatch = "A transition labeled with '" +
                    transition->getLabelForMatching() +
                    "' leaves the marking '" +
                    getCurrentMarkingAsString() +
                    "' in the oWFN, but not the node '" +
                    currentOGNode->getName() + "' in the OG.";

                return false;
            }

            // Fire the to be tried transition. The thereby reached marking is
            // saved to CurrentMarking.
            transition->fire(this);

            // Save the currentOGNode to a temporary copy, so we can easily
            // revert to it if the state we reached to firing the current
            // transition lead us to an already seen state.
            OGFromFileNode* oldOGNode = currentOGNode;

            // Fire the transition in the OG that belongs to the transition we
            // just fired in the oWFN.
            currentOGNode = currentOGNode->fireTransitionWithLabel(
                transition->getLabelForMatching());

            // Determine whether we have already seen the state we just
            // reached.
            State* newState = binSearch(this);
            if (newState != NULL) {
                // We have already seen the state we just reaching by firing
                // the transition above. So we have to revert to the state that
                // the transition left.
                copyMarkingToCurrentMarking(tmpCurrentMarking);

                // owfnTransition::backfire() does not actually backfire.
                // It merely rechecks enabledness of all transitions in
                // question with respect to CurrentMarking. Therefore you have
                // to restore the CurrentMarking first and then call
                // backfire() to undo the effect of a transition.
                transition->backfire(this);
                placeHashValue = tmpPlaceHashValue;
                delete[] tmpCurrentMarking;
                tmpCurrentMarking = NULL;
                currentState->succ[currentState->current] = newState;

                // Increment current to the index of the next to be fired
                // transition.
                currentState->current++;

                // OGFromFileNode::backfire...() works as expected.
                currentOGNode = currentOGNode->backfireTransitionWithLabel(
                    transition->getLabelForMatching());
            } else {
                // The state we reached by firing the above transition is new.
                // So we have to initialize this newly seen state.
                newState = binInsert(this);
                newState->firelist = firelist();
                newState->CardFireList = CardFireList;
                newState->current = 0;
                newState->parent = currentState;
                newState->succ = new State*[CardFireList + 1];
                for (size_t istate = 0; istate != CardFireList + 1; ++istate) {
                    newState->succ[istate] = NULL;
                }
                newState->placeHashValue = placeHashValue;
                newState->type = typeOfState();
                currentState->succ[currentState->current] = newState;
                currentState = newState;

                currentOGNode->setDepthFirstSearchParent(oldOGNode);

                // Clean up the temporary copy of the former CurrentMarking
                // because we do not longer need to be able to revert to it as
                // we have a found a _new_ not yet seen state.
                if (tmpCurrentMarking != NULL) {
                    delete[] tmpCurrentMarking;
                    tmpCurrentMarking = NULL;
                }

                // Check whether the initial marking violates the message bound
                // and exit with an error message if it does.
                if (checkMessageBound()) {
                    reasonForFailedMatch = "Current marking: '" +
                        getCurrentMarkingAsString() +
                        "' violated message bound.";
                    return false;
                }
            }
        } else {
            // There are no transitions left to fire. So we are about to
            // backtrack to the parent of the currentState. But before we
            // do this, we have to check whether the currentState satisfies
            // the annotation of the corresponding OG node. So we construct
            // an assignment for this node such that all propositions
            // corresponding to currently enabled transitions are set to
            // true and all others to false; furthermore the literal
            // 'final' is set to true iff the currentState is a final
            // state. Because in a OGFromFileFormulaAssignment every
            // unmentioned literal is considered false, we only set those
            // literals that should be considered true.
            CommGraphFormulaAssignment assignment =
                makeAssignmentForOGMatchingForState(currentState);

            if (!currentOGNode->assignmentSatisfiesAnnotation(assignment)) {
                // Clean up the temporary copy of the former CurrentMarking
                // just to be sure.
                if (tmpCurrentMarking != NULL) {
                    delete[] tmpCurrentMarking;
                    tmpCurrentMarking = NULL;
                }

                reasonForFailedMatch = "The marking '" +
                    getCurrentMarkingAsString() +
                    "' of the oWFN does not satisfy the annotation '" +
                    currentOGNode->getAnnotationAsString() +
                    "' of the corresponding node '" +
                    currentOGNode->getName() + "' in the OG.";

                return false;
            }

            currentState = currentState->parent;
            if (currentState != NULL) {
                // Decode currentState into CurrentMarking such that
                // currentState and CurrentMarking again denote the same
                // state of the oWFN.
                currentState->decode(this);
                currentState->current++;
                currentOGNode = currentOGNode->getDepthFirstSearchParent();
            }
        }
    }

    // Clean up before we return from the function.
    if (tmpCurrentMarking != NULL) {
        delete[] tmpCurrentMarking;
        tmpCurrentMarking = NULL;
    }

    // We found no reason for the oWFN not to match with the OG. So it matches.
    reasonForFailedMatch = "";
    return true;
}


CommGraphFormulaAssignment oWFN::makeAssignmentForOGMatchingForState(const State* currentState) const {
    CommGraphFormulaAssignment assignment;

    for (unsigned int itransition = 0;
         itransition != currentState->CardFireList; ++itransition)
    {
        owfnTransition* transition = currentState->firelist[itransition];
        assignment.setToTrue(transition->getLabelForMatching());
    }

    if (isFinal()) {
        assignment.setToTrue(CommGraphFormulaAssignment::FINAL);
    }

    return assignment;
}
