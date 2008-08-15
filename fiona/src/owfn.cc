/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider                                               *
 *                                                                           *
 * Copyright 2008                                                            *
 *   Peter Massuthe, Daniela Weinberg                                        *
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

#include "owfn.h"
#include "state.h"
#include "owfnTransition.h"
#include "binDecision.h"
#include "AnnotatedGraph.h"
#include "debug.h"
#include "options.h"
#include "mynew.h"
#include "GraphFormula.h"

#include <stdlib.h>
#include <cassert>
#include <algorithm>
#include <limits.h>

using namespace std;

#define MINIMUM(X,Y) ((X) < (Y) ? (X) : (Y))

//comparison function, in order to sort the input/output place names 
bool compare(const owfnPlace* lhs, const owfnPlace* rhs) {
    return lhs->name < rhs->name;
}


//! \brief constructor
oWFN::oWFN() :
    CurrentCardFireList(0), CurrentCardQuasiFireList(0),
    FinalCondition(NULL), currentState(0),
    placeHashValue(0), BitVectorSize(0) {

    filename = "";
    unsigned int i;
    NonEmptyHash = 0;

    try {
        binHashTable = new binDecision * [HASHSIZE];
    } catch(bad_alloc) {
        cerr << "\nhash table too large!\n";
        _exit(2);
    }

    for (i = 0; i < HASHSIZE; i++) {
        binHashTable[i] = (binDecision *) 0;
    }
}


//! \brief destructor
oWFN::~oWFN() {
    trace(TRACE_5, "oWFN::~oWFN() : start\n");

    for (int i = 0; i < HASHSIZE; i++) {
    	if (binHashTable[i]) {
            delete binHashTable[i];
        }
        binHashTable[i] = (binDecision *) 0;
    }

    trace(TRACE_5, "oWFN::~oWFN() : delete hashtable\n");
    delete[] binHashTable;

    trace(TRACE_5, "oWFN::~oWFN() : delete places\n");
    for (Places_t::size_type i = 0; i < getPlaceCount(); ++i) {
        delete getPlace(i);
    }

    trace(TRACE_5, "oWFN::~oWFN() : delete transitions\n");
    for (Transitions_t::size_type i = 0; i < getTransitionCount(); ++i) {
        delete getTransition(i);
    }

    trace(TRACE_5, "oWFN::~oWFN() : clear ports\n");
    ports.clear();

    trace(TRACE_5, "oWFN::~oWFN() : clear place indices\n");
    PlaceIndices.clear();
    inputPlaceIndices.clear();
    outputPlaceIndices.clear();

    trace(TRACE_5, "oWFN::~oWFN() : delete current marking\n");
    delete[] CurrentMarking;

    trace(TRACE_5, "oWFN::~oWFN() : delete final markings\n");
    for (list<unsigned int*>::iterator finalMarking = FinalMarkingList.begin(); finalMarking != FinalMarkingList.end(); finalMarking++) {
        
        delete[] *finalMarking;
    }

    trace(TRACE_5, "oWFN::~oWFN() : delete final condition\n");
    delete FinalCondition;

    trace(TRACE_5, "oWFN::~oWFN() : end\n");
}


//! \brief returns the number of places
//! \return number of places
oWFN::Places_t::size_type oWFN::getPlaceCount() const {
    return Places.size();
}


//! \brief returns the place at a given index
//! \param i index of the place
//! \return the indicated place
owfnPlace* oWFN::getPlace(Places_t::size_type i) const {
    return Places[i];
}


//! \brief returns a pointer to first owfnPlace with given name or NULL if
//!        none place was found
//! \param name a string containing the placename
//! \return a pointer to the owfnPlace or NULL
owfnPlace* oWFN::getPlace(std::string name) const {

    for (Places_t::const_iterator place = Places.begin(); place != Places.end(); place++) {
        if ( (*place)->getName() == name ) {
            return *place;
        }
    }

    return NULL;
}


//! \brief returns the number of transitions
//! \return number of transitions
oWFN::Transitions_t::size_type oWFN::getTransitionCount() const {
    return Transitions.size();
}


//! \brief returns the transition at a given index
//! \param i index of the transition
//! \return the indicated transition
owfnTransition* oWFN::getTransition(Transitions_t::size_type i) const {
    return Transitions[i];
}


//! \brief returns the number of input places
//! \return number of input places
oWFN::Places_t::size_type oWFN::getInputPlaceCount() const {
    return inputPlaces.size();
}


//! \brief returns the input place at a given index
//! \param i index of the input place
//! \return the indicated input place
owfnPlace* oWFN::getInputPlace(Places_t::size_type i) const {
    return inputPlaces[i];
}


//! \brief returns the number of output places
//! \return number of output places
oWFN::Places_t::size_type oWFN::getOutputPlaceCount() const {
    return outputPlaces.size();
}

//! \brief returns the output place at a given index
//! \param i index of the output place
//! \return the indicated output place
owfnPlace* oWFN::getOutputPlace(Places_t::size_type i) const {
    return outputPlaces[i];
}

//! \brief returns the index of a given place
//! \param Place the place to look for
//! \return i index of the given place
oWFN::Places_t::size_type oWFN::getPlaceIndex(owfnPlace *Place) {
    return(PlaceIndices[Place]);
}

//! \brief returns the index of a given input place
//! \param Place the input place to look for
//! \return i index of the given input place
oWFN::Places_t::size_type oWFN::getInputPlaceIndex(owfnPlace *inputPlace) {
    return(inputPlaceIndices[inputPlace]);
}

//! \brief returns the index of a given output place
//! \param Place the output place to look for
//! \return i index of the given output place
oWFN::Places_t::size_type oWFN::getOutputPlaceIndex(owfnPlace *outputPlace) {
    return(outputPlaceIndices[outputPlace]);
}

//! \brief initializes the owfn; is called right after the parsing of the net file is done
void oWFN::initialize() {
    trace(TRACE_5, "oWFN::initialize(): start\n");
    unsigned int i;

    // initialize places hashes
    for (i = 0; i < getPlaceCount(); i++) {
        getPlace(i)->set_hash(rand());
    }

    // initialize places
    for (i=0; i < getPlaceCount(); i++) {
        PlaceIndices[getPlace(i)] = i;

        // TODO: check if the next 5 lines of code are really necessary, because after
        // TODO: "initialize transitions" there is another loop over all places
        // TODO: which initializes CurrentMarking
        if ((getPlace(i)->type == INPUT) && (CurrentMarking[i] < getPlace(i)->capacity)) {
            CurrentMarking[i] = getPlace(i)->capacity;
        } else {
            CurrentMarking[i] = getPlace(i)->initial_marking;
        }
    }

#ifdef STUBBORN
    // initialize array of pre-transitions
    for(i=0; i<getPlaceCount(); i++) {
        for (unsigned int j = 0; j < getPlace(i)->getArrivingArcsCount(); j++) {
            getPlace(i)->PreTransitions.push_back(getPlace(i)->getArrivingArc(j)->tr);
        }
    }
#endif

    // initialize transitions
    for (Transitions_t::size_type i = 0; i < getTransitionCount(); ++i) {
        getTransition(i)->initialize();
    }

    // TODO: check if this loop is really necessary because of the loop above
    for (i=0; i < getPlaceCount(); i++) {
        CurrentMarking[i] = getPlace(i)->initial_marking;
    }

    checkEnablednessOfAllTransitions();

    // sort the inputPlacesArray and the outputPlacesArray
    sort(inputPlaces.begin(), inputPlaces.end(), compare);
    sort(outputPlaces.begin(), outputPlaces.end(), compare);

    for (i = 0, BitVectorSize = 0; i < getPlaceCount(); i++) {
        getPlace(i)->startbit = BitVectorSize;
        BitVectorSize += getPlace(i)->nrbits;
    }

    trace(TRACE_5, "oWFN::initialize(): end\n");
}

//! \brief checks for every transition if it is enabled
void oWFN::checkEnablednessOfAllTransitions() {
    for (Transitions_t::size_type i = 0; i < getTransitionCount(); ++i) {
        getTransition(i)->check_enabled(this);
    }
}

//! \brief removes all isolated places
void oWFN::removeisolated() {
    unsigned int i;

    owfnPlace * p;

    i=0;
    while (i<getPlaceCount()) {
        if (getPlace(i)->references == 0) { // owfnPlace isolated
            //TODO: CurrentMarking in Vektor umwandeln, dann loeschen eines
            //TODO: Platzes gleich mit erase(), gleichzeitiger automatischer
            //TODO: Loeschung in CurrentMarking und Reindexierung der Plaetze
            //TODO: implementieren.
            p = getPlace(getPlaceCount() - 1);
            unsigned int m = CurrentMarking[getPlaceCount() - 1];
            Places[getPlaceCount() - 1] = getPlace(i);
            CurrentMarking[getPlaceCount() - 1] = CurrentMarking[i];
            Places[i] = p;
            CurrentMarking[i] = m;
            delete getPlace(getPlaceCount() - 1);
            Places.erase(Places.end() - 1);
        } else {
            i++;
        }
    }
    /*
     for(i=0;i<transCnt;i++) {
     getTransition(i)->nr = i;
     }
     */
    for (i=0; i<getPlaceCount(); i++) {
        // getPlace(i)->nr = i;
        PlaceIndices[getPlace(i)] = i;
    }
}

//! \brief returns an array of transition pointers containing all enabled transition
//! \return firelist
owfnTransition ** oWFN::firelist() {
    owfnTransition** tl = new owfnTransition * [enabledTransitions.size() + 1];
    int i = 0;
    EnabledTransitions::ConstIterator iTrans =
        enabledTransitions.getConstIterator();
    while (iTrans->hasNext()) {
        tl[i++] = iTrans->getNext();
    }
    delete iTrans;
    tl[i] = NULL;
    CurrentCardFireList = i;
    return tl;
}


//! \brief returns an array of transition pointers containing all quasi enabled transition
//! \return quasi firelist
owfnTransition ** oWFN::quasiFirelist() {
    owfnTransition ** tl =
        new owfnTransition * [quasiEnabledTransitions.size() + 1];
    int i = 0;
    QuasiEnabledTransitions::ConstIterator iTrans =
        quasiEnabledTransitions.getConstIterator();
    while (iTrans->hasNext()) {
        tl[i++] = iTrans->getNext();
    }
    delete iTrans;
    tl[i] = NULL;
    CurrentCardQuasiFireList = i;

    return tl;
}


//! \brief decodes state, checks for message bound violation and adds successors recursively
//! \param n the node to add the states to
//! \param currentState the currently added state
void oWFN::addSuccStatesToNode(AnnotatedGraphNode* n, State * currentState) {

    assert(n != NULL);

    if (currentState != NULL) {
        assert(!n->addState(currentState)); // currentState was added to node before

        currentState->decodeShowOnly(this); // decodes currently considered state
        
        // test decoded current marking if message bound k reached
        if (violatesMessageBound()) {
            n->setColor(RED);
            trace(TRACE_3, "\t\t\t message bound violation detected in node "
                    + n->getName() + " (addSuccStatesToNode) --> node set to RED\n");
            
            return;
        }

        // add successors
        for (unsigned int i = 0; i < currentState->cardFireList; i++) {
            if (currentState->succ[i] != NULL) {
                if (n->addState(currentState->succ[i])) { // add current successor
                    // its successors need only be added if state was not yet in current node
                    addSuccStatesToNode(n, currentState->succ[i]);
                }
            } else {
                // this can only happen if currentState would have a successor
                // which violates message bound;
                // but instead currentState has NULL successor

            	// if current state has a successor, which violates the message bound
            	
                // if the above is true, then current node n must be
                // considered to violate message bound, too
                n->setColor(RED);
                trace(TRACE_3,
                      "\t\t\t message bound violation detected in node "
                              + n->getName()
                              + " (addSuccStatesToNode with NULL successor)\n");
                return;
            }
        }
    }
}


//! \brief decodes state, checks for message bound violation and adds successors recursively
//! 	   a temporary state set stores all states reached, that way we can detect a circular 
//!	       connection between states
//!        note: stateSet only stores DEADLOCK and FINALSTATE states
//! \param n the node to add the states to
//! \param stateSet set of states where the successors of the given state shall be stored in
//! \param stateSetTemp set containing all states that we reach
//! \param parentMarking
//! \param currentState state to start at (currently added state)
void oWFN::addSuccStatesToNodeStubborn(AnnotatedGraphNode* n, 
										StateSet& stateSet, 
										StateSet& stateSetTemp,
										unsigned int* parentMarking,
										State * currentState) {

	trace(TRACE_5, "void oWFN::addSuccStatesToNodeStubborn(AnnotatedGraphNode* n, StateSet& stateSet, unsigned int* parentMarking, State * currentState) : start\n");
	
    assert(n != NULL);

    currentState->decodeShowOnly(this); // decodes currently considered state
    
    // test decoded current marking if message bound k reached
    if (violatesMessageBound()) {
        n->setColor(RED);
        trace(TRACE_3, "\t\t\t message bound violation detected in node "
                + n->getName() + " (addSuccStatesToNodeStubborn) --> node set to RED\n");
        
        return;
    }

    if (currentMarkingActivatesReceivingEvent(parentMarking)) {
    	n->addState(currentState);
    	trace(TRACE_5, "added state [" + getCurrentMarkingAsString() + "] to node\n");
    }
    
    if (currentState->type == DEADLOCK || currentState->type == FINALSTATE) {
    	stateSet.insert(currentState);
        trace(TRACE_5, "added state [" + getCurrentMarkingAsString() + "] to state set\n");
    }
    
    stateSetTemp.insert(currentState);
    
    // add successors
    for (unsigned int i = 0; i < currentState->cardFireList; i++) {
        if (currentState->succ[i] != NULL && 
        		stateSetTemp.find(currentState->succ[i]) == stateSetTemp.end() &&
        		n->reachGraphStateSet.find(currentState->succ[i]) == n->reachGraphStateSet.end()) {
        	
            // its successors need only be added if state was not yet in current node
        	unsigned int * tempParentMarking = copyCurrentMarking();
        	addSuccStatesToNodeStubborn(n, stateSet, stateSetTemp, tempParentMarking, currentState->succ[i]);
        	if (tempParentMarking) {
        		delete[] tempParentMarking;
        	}
        	if (i < currentState->cardFireList) {
        		currentState->decodeShowOnly(this); // decodes currently considered state
        	}
        } 
    }
    trace(TRACE_5, "void oWFN::addSuccStatesToNodeStubborn(AnnotatedGraphNode* n, StateSet& stateSet, unsigned int* parentMarking, State * currentState) : end\n");
}

//! \brief decodes state, checks for message bound violation and adds successors recursively
//! \param n the node to add the states to
//! \param stateSet set of states where the successors of the given state shall be stored in
//! \param parentMarking
//! \param currentState state to start at (currently added state)
void oWFN::addSuccStatesToNodeStubborn(AnnotatedGraphNode* n, StateSet& stateSet, 
										unsigned int* parentMarking,
										State * currentState) {

	trace(TRACE_5, "void oWFN::addSuccStatesToNodeStubborn(AnnotatedGraphNode* n, StateSet& stateSet, unsigned int* parentMarking, State * currentState) : start\n");
	
    currentState->decodeShowOnly(this); // decodes currently considered state
	
    // test decoded current marking if message bound k reached
    if (violatesMessageBound()) {
        n->setColor(RED);
        trace(TRACE_3, "\t\t\t message bound violation detected in node "
                + n->getName() + " (addSuccStatesToNodeStubborn) --> node set to RED\n");
        
        return;
    }	
	
	StateSet stateSetTemp;
    if (currentMarkingActivatesReceivingEvent(parentMarking)) {
    	n->addState(currentState);
    	trace(TRACE_5, "added state [" + getCurrentMarkingAsString() + "] to node\n");
    }
    
    if (currentState->type == DEADLOCK || currentState->type == FINALSTATE) {
    	stateSet.insert(currentState);
        trace(TRACE_5, "added state [" + getCurrentMarkingAsString() + "] to state set\n");
    }
    
    stateSetTemp.insert(currentState);
    
    // add successors
    for (unsigned int i = 0; i < currentState->cardFireList; i++) {
        if (currentState->succ[i] != NULL && 
        		stateSetTemp.find(currentState->succ[i]) == stateSetTemp.end() &&
        		n->reachGraphStateSet.find(currentState->succ[i]) == n->reachGraphStateSet.end()) {
        	
            // its successors need only be added if state was not yet in current node
        	unsigned int * tempParentMarking = copyCurrentMarking();
        	addSuccStatesToNodeStubborn(n, stateSet, stateSetTemp, tempParentMarking, currentState->succ[i]);
        	if (tempParentMarking) {
        		delete[] tempParentMarking;
        	}
        	currentState->decodeShowOnly(this); // decodes currently considered state
        } 
    }
	stateSetTemp.clear();
	
	trace(TRACE_5, "void oWFN::addSuccStatesToNodeStubborn(AnnotatedGraphNode* n, StateSet& stateSet, unsigned int* parentMarking, State * currentState) : end\n");
}


//! \brief decodes state, figures out if state activates output event, 
//!			checks for message bound violation and adds successors recursively
//! \param stateSet set of states where the successors of the given state shall be stored in
//! \param stateSetTemp set containing all states that we reach
//! \param setOfStatesStubbornTemp set of states containing all deadlock states we calculate in case of a
//!		                           receiving event activating state
//! \param outputPlace _one_ output (place) message needed to figure out whether to store the current state or not 
//! \param currentState state to start at (currently added state)
//! \param n the current node, in case of message bound violation this node becomes red
void oWFN::addSuccStatesToListStubborn(StateSet & stateSet,
                                       StateSet & stateSetTemp,
                                       StateSet & setOfStatesStubbornTemp,
									   owfnPlace * outputPlace,
                                       State * currentState,
                                       AnnotatedGraphNode* n) {

    trace(
          TRACE_5,
          "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & stateSetTemp, StateSet & setOfStatesStubbornTemp, owfnPlace * outputPlace, State * currentState, AnnotatedGraphNode* n: end\n");
    if (currentState != NULL) {
        currentState->decodeShowOnly(this); // decodes currently considered state

        // test decoded current marking if message bound k reached
        if (violatesMessageBound()) {
            n->setColor(RED);
            trace(TRACE_3, "\t\t\t message bound violation detected in node "
                    + n->getName() + " (addSuccStatesToListStubborn)\n");
            trace(
                  TRACE_5,
                  "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & stateSetTemp, StateSet & setOfStatesStubbornTemp, owfnPlace * outputPlace, State * currentState, AnnotatedGraphNode* n: end\n");
            return;
        }
        
        // does current state activate the output event? meaning is this place marked
        if (CurrentMarking[getPlaceIndex(outputPlace)] > 0) {
            stateSet.insert(currentState);
            currentState->decode(this);
//            if (removeOutputMessage(getPlaceIndex(outputPlace))) { // remove the output message from the current marking
//            	// calc the reachable states from that marking using stubborn set method taking
//            	// care of deadlocks
//            	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n); 
//        	}
//            trace(
//                  TRACE_5,
//                  "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & stateSetTemp, StateSet & setOfStatesStubbornTemp, owfnPlace * outputPlace, State * currentState, AnnotatedGraphNode* n: end\n");
//            return;
        }

        stateSetTemp.insert(currentState);
        
        // add successors
        for (unsigned int i = 0; i < currentState->cardFireList; i++) {
            // test if successor state has not yet been added to the state set
            if (currentState->succ[i] != NULL && 
            		stateSet.find(currentState->succ[i]) == stateSet.end() &&
            		stateSetTemp.find(currentState->succ[i]) == stateSetTemp.end()) {
            	
                addSuccStatesToListStubborn(stateSet, stateSetTemp, setOfStatesStubbornTemp, outputPlace,
                                            currentState->succ[i], n);
            } 
        }
    }
    trace(
          TRACE_5,
          "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & stateSetTemp, StateSet & setOfStatesStubbornTemp, owfnPlace * outputPlace, State * currentState, AnnotatedGraphNode* n: end\n");
}

//! \brief 	helper function of addSuccStatesToListStubborn
//!			decodes state, figures out if state activates output event, 
//!			checks for message bound violation and adds successors recursively
//! \param stateSet set of states where the successors of the given state shall be stored in
//! \param setOfStatesStubbornTemp set of states containing all deadlock states we calculate in case of a
//!		                           receiving event activating state
//! \param outputPlace _one_ output (place) message needed to figure out whether to store the current state or not 
//! \param currentState state to start at (currently added state)
//! \param n the current node, in case of message bound violation this node becomes red
void oWFN::addSuccStatesToListStubborn(StateSet & stateSet,
                                       StateSet & setOfStatesStubbornTemp,
									   owfnPlace * outputPlace,
                                       State * currentState,
                                       AnnotatedGraphNode* n) {

	trace(
          TRACE_5,
          "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & setOfStatesStubbornTemp, owfnPlace * outputPlace, State * currentState, AnnotatedGraphNode* n: start\n");

    // note: we don't need to decode the currentState, since it has been calculated fully
    // 		 before, so all variables are set correctly already!	
	
    // does current state activate the output event? meaning is this place marked
    if (CurrentMarking[getPlaceIndex(outputPlace)] > 0) {
        stateSet.insert(currentState);
//        if (removeOutputMessage(getPlaceIndex(outputPlace))) { // remove the output message from the current marking
//        	// calc the reachable states from that marking using stubborn set method taking
//        	// care of deadlocks
//        	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n); 
//    	}
//        trace(
//              TRACE_5,
//              "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & setOfStatesStubbornTemp, owfnPlace * outputPlace, State * currentState, AnnotatedGraphNode* n: end\n");
//        return;
    }

    StateSet stateSetTemp;
    
    stateSetTemp.insert(currentState);
    
    // add successors
    for (unsigned int i = 0; i < currentState->cardFireList; i++) {
        // test if successor state has not yet been added to the state set
        if (currentState->succ[i] != NULL && stateSetTemp.find(currentState->succ[i]) == stateSetTemp.end()) {
            addSuccStatesToListStubborn(stateSet, stateSetTemp, setOfStatesStubbornTemp, outputPlace,
                                        currentState->succ[i], n);
        } 
    }
    
    stateSetTemp.clear();
    trace(
          TRACE_5,
          "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & setOfStatesStubbornTemp, owfnPlace * outputPlace, State * currentState, AnnotatedGraphNode* n: end\n");
}


//! \brief decodes state, figures out if state activates output event, 
//!			checks for message bound violation and adds successors recursively
//! \param stateSet set of states where the successors of the given state shall be stored in
//! \param stateSetTemp set containing all states that we reach
//! \param setOfStatesStubbornTemp set of states containing all deadlock states we calculate in case of a
//!		                           receiving event activating state
//! \param outputPlace _set_ of output (places) messages needed to figure out whether to store the current state or not 
//! \param currentState state to start at (currently added state)
//! \param n the current node, in case of message bound violation this node becomes red
void oWFN::addSuccStatesToListStubborn(StateSet & stateSet,
									   StateSet & stateSetTemp,
									   StateSet & setOfStatesStubbornTemp,
                                       messageMultiSet messages,
                                       State * currentState,
                                       AnnotatedGraphNode* n) {

    trace(
          TRACE_5,
          "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & stateSetTemp, StateSet & setOfStatesStubbornTemp, messageMultiSet messages, State * currentState, AnnotatedGraphNode* n: start\n");
	
    if (currentState != NULL) {
        currentState->decodeShowOnly(this); // decodes currently considered state

        // test decoded current marking if message bound k reached
        if (violatesMessageBound()) {
            n->setColor(RED);
            trace(TRACE_3, "\t\t\t message bound violation detected in node "
                    + n->getName() + " (addSuccStatesToListStubborn)\n");
            trace(
                  TRACE_5,
                  "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & stateSetTemp, StateSet & setOfStatesStubbornTemp, messageMultiSet messages, State * currentState, AnnotatedGraphNode* n: end\n");
            
            return;
        }        
        
        stateSetTemp.insert(currentState);
        
        bool somePlaceNotMarked = false; // remember if a place is not marked at the current marking

        // shall we save this state? meaning, are the correct output places marked?
        for (messageMultiSet::iterator iter = messages.begin(); iter
                != messages.end(); iter++) {
            if (CurrentMarking[getPlaceIndex(getPlace(*iter))] == 0) {
                somePlaceNotMarked = true;
                break;
            }
        }

        if (!somePlaceNotMarked) { // if all places are appropriatly marked, we save this state
            stateSet.insert(currentState);
//            currentState->decode(this);
//            if (removeOutputMessage(messages)) { // remove the output message from the current marking
//            	// calc the reachable states from that marking using stubborn set method taking
//            	// care of deadlocks
//            	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n); 
//        	}
//            trace(
//                  TRACE_5,
//                  "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & stateSetTemp, StateSet & setOfStatesStubbornTemp, messageMultiSet messages, State * currentState, AnnotatedGraphNode* n: end\n");
//            // nothing else to be done here
//            return;

        }

        // add successors
        for (unsigned int i = 0; i < currentState->cardFireList; i++) {
            // test if successor state has not yet been added to the state set
            if (stateSet.find(currentState->succ[i]) == stateSet.end() &&
            		stateSetTemp.find(currentState->succ[i]) == stateSetTemp.end()) {
                // its successors need only be added if state was not yet in current state set
                addSuccStatesToListStubborn(stateSet, stateSetTemp, setOfStatesStubbornTemp, messages,
                                            currentState->succ[i], n);
            }
        }
    }
    
    trace(
          TRACE_5,
          "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & stateSetTemp, StateSet & setOfStatesStubbornTemp, messageMultiSet messages, State * currentState, AnnotatedGraphNode* n: end\n");
}

//! \brief  helper function, figures out if state activates output event, 
//!			checks for message bound violation and adds successors recursively
//! \param stateSet set of states where the successors of the given state shall be stored in
//! \param setOfStatesStubbornTemp set of states containing all deadlock states we calculate in case of a
//!		                           receiving event activating state
//! \param outputPlace _set_ of output (places) messages needed to figure out whether to store the current state or not 
//! \param currentState state to start at (currently added state)
//! \param n the current node, in case of message bound violation this node becomes red
void oWFN::addSuccStatesToListStubborn(StateSet & stateSet,
									   StateSet & setOfStatesStubbornTemp,
                                       messageMultiSet messages,
                                       State * currentState,
                                       AnnotatedGraphNode* n) {

    trace(
          TRACE_5,
          "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & setOfStatesStubbornTemp, messageMultiSet messages, State * currentState, AnnotatedGraphNode* n: start\n");
	
    // note: we don't need to decode the currentState, since it has been calculated fully
    // 		 before, so all variables are set correctly already!
    
    bool somePlaceNotMarked = false; // remember if a place is not marked at the current marking

    // shall we save this state? meaning, are the correct output places marked?
    for (messageMultiSet::iterator iter = messages.begin(); iter
            != messages.end(); iter++) {
        if (CurrentMarking[getPlaceIndex(getPlace(*iter))] == 0) {
            somePlaceNotMarked = true;
            break;
        }
    }

    if (!somePlaceNotMarked) { // if all places are appropriatly marked, we save this state
        stateSet.insert(currentState);
//        if (removeOutputMessage(messages)) { // remove the output message from the current marking
//        	// calc the reachable states from that marking using stubborn set method taking
//        	// care of deadlocks
//        	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n); 
//    	}
//        trace(
//              TRACE_5,
//              "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & setOfStatesStubbornTemp, messageMultiSet messages, State * currentState, AnnotatedGraphNode* n: end\n");
//        // nothing else to be done here
//        return;

    }
    
    StateSet stateSetTemp;
    stateSetTemp.insert(currentState);

    // add successors
    for (unsigned int i = 0; i < currentState->cardFireList; i++) {
        // test if successor state has not yet been added to the state set
        if (currentState->succ[i] != NULL && stateSetTemp.find(currentState->succ[i]) == stateSetTemp.end()) {
            // its successors need only be added if state was not yet in current state set
            addSuccStatesToListStubborn(stateSet, stateSetTemp, setOfStatesStubbornTemp, messages,
                                        currentState->succ[i], n);
        }
    }

    stateSetTemp.clear();

    trace(
          TRACE_5,
          "oWFN::addSuccStatesToListStubborn(StateSet & stateSet, StateSet & setOfStatesStubbornTemp, messageMultiSet messages, State * currentState, AnnotatedGraphNode* n: end\n");
}


//! \brief checks if message bound is violated by the current marking (for interface places only)
//! \return returns true iff current marking VIOLATES message bound
bool oWFN::violatesMessageBound() {
    trace(TRACE_5, "oWFN::checkMessageBound(): start\n");
    
    // test marking of current state if message bound k reached
    if (options[O_MESSAGES_MAX] == true) { // k-message-bounded set

        Places_t::size_type i;

        // test input places
        for (i = 0; i < getInputPlaceCount(); i++) {
            if (CurrentMarking[getPlaceIndex(getInputPlace(i))] > messages_manual) {
                trace(TRACE_3, "\t\t\t checkMessageBound found violation for");
                trace(TRACE_3, " input place " + string(getInputPlace(i)->name) + "\n");
                trace(TRACE_5, "oWFN::checkMessageBound(): end\n");
                return true;
            }
        }
        // test output places
        for (i = 0; i < getOutputPlaceCount(); i++) {
            if (CurrentMarking[getPlaceIndex(getOutputPlace(i))] > messages_manual) {
                trace(TRACE_3, "\t\t\t checkMessageBound found violation for");
                trace(TRACE_3, " output place " + string(getOutputPlace(i)->name) + "\n");
                trace(TRACE_5, "oWFN::checkMessageBound(): end\n");
                return true;
            }
        }
    }
    trace(TRACE_5, "oWFN::checkMessageBound(): end\n");
    return false; // no violation found
}


//! \brief adds recursively the state s and all its successor states to the given state set
//! 	   a temporary state set stores all states reached, that way we can detect a circular 
//!	       connection between states
//!        note: stateSet only stores DEADLOCK and FINALSTATE states
//! \param stateSet set of states where the successors of the given state shall be stored in
//! \param stateSetTemp set containing all states that we reach
//! \param s state to start at
void oWFN::addSuccStatesToGivenSetOfStatesStubbornDeadlock(AnnotatedGraphNode* n, StateSet& stateSet, 
														StateSet& stateSetTemp, State* s) {
    trace(TRACE_5,
          "oWFN::addSuccStatesToGivenSetOfStatesStubbornDeadlock(AnnotatedGraphNode*, StateSet&, StateSet&, State* s): start\n");

    s->decodeShowOnly(this); // decodes currently considered state

    // test decoded current marking if message bound k reached
    if (violatesMessageBound()) {
        n->setColor(RED);
        trace(TRACE_3, "\t\t\t message bound violation detected in node "
                + n->getName() + " (addSuccStatesToList)\n");
        return;
    }
    
    if (s->type == DEADLOCK || s->type == FINALSTATE) {
    	stateSet.insert(s);
    }
    
    stateSetTemp.insert(s);

    // get the successor states	and add them, too.
    for (unsigned int i = 0; i < s->cardFireList; i++) {
        if (s->succ[i] && stateSetTemp.find(s->succ[i]) == stateSetTemp.end()) {
            addSuccStatesToGivenSetOfStatesStubbornDeadlock(n, stateSet, stateSetTemp, s->succ[i]);
        }
    }
    trace(TRACE_5,
          "oWFN::addSuccStatesToGivenSetOfStatesStubbornDeadlock(AnnotatedGraphNode*, StateSet&, StateSet&, State* s): end\n");
}

//! \brief helper function to add recursively the state s and all its successor states to the given state set
//! \param stateSet set of states where the successors of the given state shall be stored in
//! \param s state to start at
void oWFN::addSuccStatesToGivenSetOfStatesStubbornDeadlock(AnnotatedGraphNode* n, StateSet& stateSet, State* s) {
    trace(TRACE_5,
          "oWFN::addSuccStatesToGivenSetOfStatesStubbornDeadlock(AnnotatedGraphNode*, StateSet&, State* s): start\n");

    StateSet stateSetTemp;
    
    if (s->type == DEADLOCK || s->type == FINALSTATE) {
    	stateSet.insert(s);
    }
    
    stateSetTemp.insert(s); 
    
    // get the successor states	and add them, too.
    for (unsigned int i = 0; i < s->cardFireList; i++) {
        if (s->succ[i] && stateSetTemp.find(s->succ[i]) == stateSetTemp.end()) {
            addSuccStatesToGivenSetOfStatesStubbornDeadlock(n, stateSet, stateSetTemp, s->succ[i]);
        }
    }    
    
    stateSetTemp.clear();
    
    trace(TRACE_5,
          "oWFN::addSuccStatesToGivenSetOfStatesStubbornDeadlock(AnnotatedGraphNode*, StateSet&, State* s): end\n");
}

//! \brief Copies the current marking and returns it
//! \return marking as an array of unsigned integers
unsigned int * oWFN::copyCurrentMarking() {
    unsigned int * copy = new unsigned int [getPlaceCount()];

    for (unsigned int i = 0; i < getPlaceCount(); i++) {
        copy[i] = CurrentMarking[i];
    }
    return copy;
}

//! \brief Copies the given marking and returns it
//! \return marking as an array of unsigned integers
unsigned int * oWFN::copyGivenMarking(unsigned int * original) {
	assert(original != NULL);
	
	unsigned int * copy = new unsigned int [getPlaceCount()];

    if (original != NULL) {
	    for (unsigned int i = 0; i < getPlaceCount(); i++) {
	        copy[i] = original[i];
	    }
    }
    return copy;
}


//! \brief Copies a given marking to replace the current one
//! \param copy marking as an array of unsigned integers
void oWFN::copyMarkingToCurrentMarking(unsigned int * copy) {

    for (unsigned int i = 0; i < getPlaceCount(); i++) {
        CurrentMarking[i] = copy[i];
    }
}

void oWFN::storeStateInNode(State * CurrentState, AnnotatedGraphNode * node) {
	
	unsigned int * tempCurrentMarking;
	tempCurrentMarking = copyCurrentMarking();
	
    for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter
            != node->reachGraphStateSet.end(); iter++) {
    	
    	(*iter)->decode(PN);
    	// stubbornly activated transitions of current marking
    	owfnTransition ** activatedTransitions = stubbornfirelistmarking(tempCurrentMarking); 
	
    	node->addState(CurrentState);
    }
}

//! \brief calculates the reduced set of states of the new AnnotatedGraphNode 
//!        for IG and OG with state set reduction
//! \param stateSet set of states storing the states that were calculated using the stubborn set method
//! \param n the node to be calculated in case of an input event
//! \param storeStateInNode flag of whether to store the current state (at the beginning of the function) in the node or not
void oWFN::calculateReachableStatesStubbornDeadlocks(StateSet& stateSet, AnnotatedGraphNode* n) {

    // calculates the EG starting at the current marking
    trace(TRACE_5, "oWFN::calculateReachableStatesStubbornDeadlocks(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n): start\n");

    // test current marking if message bound k reached
    if (violatesMessageBound()) {
        n->setColor(RED);
        trace(TRACE_3, "\t\t\t message bound violated; color of node "
                       + n->getName()
                       + " set to RED (calculateReachableStatesFull, oben)\n");
        return;
    }
    
    trace(TRACE_5, "current marking: [" + getCurrentMarkingAsString() + "]\n");
    
    State * CurrentState;
    State * NewState;	

    CurrentState = binSearch(this);

    unsigned int * tempCurrentMarking = NULL;
    unsigned int tempPlaceHashValue;

    if (CurrentState != NULL) {
        // marking already has a state -> put it (and all its successors) into the node
        //storeStateInNode(CurrentState, n);
    	if (n->addState(CurrentState)) {
    		addSuccStatesToGivenSetOfStatesStubbornDeadlock(n, stateSet, CurrentState);
    	}
        trace(TRACE_5, "oWFN::calculateReachableStatesStubbornDeadlocks(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n): end\n");
        return;
    }

    // the other case:
    // we have a marking which has not yet a state object assigned to it
    CurrentState = binInsert(this);

    CurrentState->firelist = stubbornfirelistdeadlocks();
    CurrentState->cardFireList = CurrentCardFireList;
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
    CurrentState->succ = new State * [CurrentCardFireList];
    for (unsigned int i = 0; i < CurrentCardFireList; i++) {
        CurrentState->succ[i] = NULL;
    }
    CurrentState->placeHashValue = placeHashValue;
    CurrentState->type = typeOfState();

    assert(CurrentState != NULL);
    
    if (CurrentState->type == DEADLOCK || CurrentState->type == FINALSTATE) {
    	stateSet.insert(CurrentState);
    }
    
  //  if (storeInNode) {
    	n->addState(CurrentState);
 //   }

    // building EG in a node
    while (CurrentState) {

        if ((n->reachGraphStateSet.size() % 1000) == 0) {
            trace(TRACE_2, "\t current state count: "
                    + intToString(n->reachGraphStateSet.size()) + "\n");
        }

#ifdef CHECKCAPACITY
            try
            {
#endif
        // no more transition to fire from current state?
        if (CurrentState->current < CurrentState->cardFireList) {
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

            if (NewState != NULL) {
                // Current marking already in bintree 
                trace(TRACE_5, "Current marking [" + getCurrentMarkingAsString() + "] already in bintree \n");

                assert(CurrentState->succ[CurrentState->current] == NULL);
                CurrentState->succ[CurrentState->current] = NewState;
                
                // test current marking if message bound k reached
                if (violatesMessageBound()) {
					n->setColor(RED);
					trace(TRACE_3, "\t\t\t message bound violated; color of node "
						+ n->getName()
  						+ " set to RED (calculateReachableStatesStubbornDeadlocks, during fire)\n");
					trace(TRACE_5, "oWFN::calculateReachableStatesStubbornDeadlocks(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n) : end\n");
					delete[] tempCurrentMarking;
					return;
				} 
                
                addSuccStatesToGivenSetOfStatesStubbornDeadlock(n, stateSet, NewState);

                if (n->getColor() == RED) {
                    trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                   + n->getName()
                                   + " set to RED (calculateReachableStatesStubbornDeadlocks, during fire)\n");
                    trace(TRACE_5, "oWFN::calculateReachableStatesStubbornDeadlocks(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n) : end\n");

                    delete[] tempCurrentMarking;
                    return;
                }
                
                copyMarkingToCurrentMarking(tempCurrentMarking);

                CurrentState->firelist[CurrentState->current]->backfire(this);

                placeHashValue = tempPlaceHashValue;

                delete[] tempCurrentMarking;
                tempCurrentMarking = NULL;

                (CurrentState->current)++;
            } else {
                trace(TRACE_5, "Current marking [" + getCurrentMarkingAsString() + "] new!\n");

                NewState = binInsert(this);
                
                assert(CurrentState->succ[CurrentState->current] == NULL);
                CurrentState->succ[CurrentState->current] = NewState;
                
                // test current marking if message bound k reached
                if (violatesMessageBound()) {
					n->setColor(RED);
					trace(TRACE_3, "\t\t\t message bound violated; color of node "
						+ n->getName()
  						+ " set to RED (calculateReachableStatesStubbornDeadlocks, during fire)\n");
					trace(TRACE_5, "oWFN::calculateReachableStatesStubbornDeadlocks(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n) : end\n");
					
					delete[] tempCurrentMarking;
					return;
				}                
                
                NewState->firelist = stubbornfirelistdeadlocks();
                NewState->cardFireList = CurrentCardFireList;
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
                NewState->succ = new State * [CurrentCardFireList];
                for (unsigned int i = 0; i < CurrentCardFireList; i++) {
                    NewState->succ[i] = NULL;
                }
                NewState->placeHashValue = placeHashValue;
                NewState->type = typeOfState();

                CurrentState = NewState;

                if (NewState->type == DEADLOCK || NewState->type == FINALSTATE) {
                	stateSet.insert(NewState);
                }

                if (tempCurrentMarking) {
                    delete[] tempCurrentMarking;
                    tempCurrentMarking = NULL;
                }
            }
            // no more transition to fire
        } else {
            // close state and return to previous state
            trace(TRACE_5, "close state [" + getCurrentMarkingAsString() + "] and return to previous state\n");
            
            if (CurrentState->firelist) {
            	delete[] CurrentState->firelist;
            	CurrentState->firelist = NULL;
            }
            
            CurrentState = CurrentState->parent;

            if (CurrentState) { // there is a father to further examine
                CurrentState->decode(this);
                CurrentState->current++;
            }
        }
#ifdef CHECKCAPACITY
            }
            catch (CapacityException& ex)
            {
                trace(TRACE_1, "\t firing transition caused capacity violation on place " + ex.place + "\n");
                CurrentState = CurrentState->parent;

                if (CurrentState) 
                { // there is a father to further examine
                    CurrentState->decode(this);
                    CurrentState->current++;
                }
                throw;
            }
#endif
    }
    if (tempCurrentMarking) {
        delete[] tempCurrentMarking;
    }

    trace(TRACE_5, "oWFN::calculateReachableStatesStubbornDeadlocks(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n): end\n");
}


//! \brief calculates the reduced set of states reachable from the current marking and stores them in 
//!		   the given setOfStatesStubbornTemp
//!        this function is for state reduced OG since a single output place is considered
//!        for OG
//! \param tempBinDecision store calculated states temporarily in a binDecicion structure
//! \param stateSet store calculated states temporarily
//! \param setOfStatesStubbornTemp store calculated states that are needed to calculate the CNF later on
//! \param outputPlace the output place of the net that is associated with the receiving event for which the new AnnotatedGraphNode is calculated
//! \param n new AnnotatedGraphNode 
void oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet,
                                                StateSet& setOfStatesStubbornTemp,
                                                binDecision** tempBinDecision,
                                                owfnPlace* outputPlace,
                                                AnnotatedGraphNode* n) {

    // calculates the EG starting at the current marking
    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace, AnnotatedGraphNode * n) : start\n");

    // test current marking if message bound k reached
    if (violatesMessageBound()) {
        n->setColor(RED);
        trace(TRACE_3, "\t\t\t message bound violated; color of node "
                + n->getName()
                + " set to RED (calculateReducedSetOfReachableStates, oben)\n");
        return;
    }

	trace(TRACE_5, "current marking: [" + getCurrentMarkingAsString() + "]\n");
    
    State* CurrentState = NULL;
    State* NewState;
    
    bool storeInNode = true;

    unsigned int* tempCurrentMarking = NULL;
    unsigned int tempPlaceHashValue;

    // note: we don't need to look up the current marking in the global binDecision (CurrentState=binSearch(this))
    // we use the global binDecision to store states leading to a deadlock
    // in this function we are interested in those states that lead to a marking which activates receiving events
    CurrentState = binSearch(*tempBinDecision, this);
    CurrentState = binInsert(tempBinDecision, this); // save current state to the local binDecision 
    
    CurrentState->current = 0;
    CurrentState->parent = (State *) 0;

    // shall we save this state? meaning, is the correct output place marked?
    if (CurrentMarking[getPlaceIndex(outputPlace)] > 0) {
       // stateSet.insert(CurrentState);
    	trace(TRACE_5, "... activates receiving event " + outputPlace->getLabelForCommGraph() + "\n");
        if (removeOutputMessage(getPlaceIndex(outputPlace))) { // remove the output message from the current marking
        	// calc the reachable states from that marking using stubborn set method taking
        	// care of deadlocks
        	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n); 
    	}
        trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace* outputPlace, AnnotatedGraphNode* n) : end\n");
        // nothing else to be done here
        return;
    }
    
    // the current state does not activate any receiving event, so we check out its successor states
    CurrentState->firelist = stubbornfirelistmessage(outputPlace);
    CurrentState->cardFireList = CurrentCardFireList;
    
    if (CurrentState->succ == NULL) {
        CurrentState->succ = new State * [CurrentCardFireList];
        for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
            CurrentState->succ[istate] = NULL;
        }
    }

    CurrentState->placeHashValue = placeHashValue;
    CurrentState->type = typeOfState();

    // building EG in a node
    while (CurrentState) {

#ifdef CHECKCAPACITY
            try
            {
#endif
        // no more transition to fire from current state?
        if (CurrentState->current < CurrentState->cardFireList) {
            // there is a next state that needs to be explored

            if (tempCurrentMarking) {
                delete[] tempCurrentMarking;
                tempCurrentMarking = NULL;
            }

            tempCurrentMarking = copyCurrentMarking();
            tempPlaceHashValue = placeHashValue;

            // fire and reach next state
            CurrentState->firelist[CurrentState->current]->fire(this);
            
            NewState = binSearch(*tempBinDecision, this);

            if (NewState != NULL) {
                // Current marking already in local bintree 
                trace(TRACE_5, "Current marking [" + getCurrentMarkingAsString() +  "] already in local bintree \n");

                // shall we save this state? meaning, is the correct output place marked?
//                if (CurrentMarking[getPlaceIndex(outputPlace)] > 0) {
//                    //stateSet.insert(NewState);
//                    if (removeOutputMessage(getPlaceIndex(outputPlace))) { // remove the output message from the current marking
//                    	trace(TRACE_5, "activates receiving event " + outputPlace->getLabelForCommGraph() + "\n");
//                    	// calc the reachable states from that marking using stubborn set method taking
//                    	// care of deadlocks
//                    	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n, storeInNode);
//                    	storeInNode = false;
//                    	
//                        if (n->getColor() == RED) {
//                            trace(TRACE_3, "\t\t\t message bound violated; color of node "
//                                           + n->getName()
//                                           + " set to RED (calculateReachableStatesFull, during fire)\n");
//                            trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end\n");
//
//                            assert(CurrentState->succ[CurrentState->current] == NULL);
//                            CurrentState->succ[CurrentState->current] = NewState;
//
//                            delete[] tempCurrentMarking;
//                            return;
//                        }
//                	}
//                } else {
                    // no it is not marked, so we take a look at its successor states
                    addSuccStatesToListStubborn(stateSet, setOfStatesStubbornTemp, outputPlace, NewState, n);
                    
                    if (n->getColor() == RED) {
                        trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                       + n->getName()
                                       + " set to RED (calculateReachableStatesFull, during fire)\n");
                        trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end\n");

                        assert(CurrentState->succ[CurrentState->current] == NULL);
                        CurrentState->succ[CurrentState->current] = NewState;

                        delete[] tempCurrentMarking;
                        return;
                    }
           //     }

                CurrentState->decode(this);
                CurrentState->firelist[CurrentState->current]->backfire(this);

                placeHashValue = tempPlaceHashValue;

                delete[] tempCurrentMarking;
                tempCurrentMarking = NULL;

                assert(CurrentState->succ[CurrentState->current] == NULL);
                CurrentState->succ[CurrentState->current] = NewState;
                (CurrentState->current)++;
            } else {
                trace(TRACE_5, "Current marking [" + getCurrentMarkingAsString() +  "] new\n");
                NewState = binInsert(tempBinDecision, this);
                
                assert(CurrentState->succ[CurrentState -> current] == NULL);
                CurrentState->succ[CurrentState -> current] = NewState;
                
                // test current marking if message bound k reached
                if (violatesMessageBound()) {
                    n->setColor(RED);
                    trace(TRACE_3, "\t\t\t message bound violated;");
                    trace(TRACE_3, " color of node " + n->getName());
                    trace(TRACE_3, " set to RED (calculateReachableStates, during fire)\n");
                    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace, AnnotatedGraphNode * n) : end\n");
                    return;
                }
                
                NewState->firelist = stubbornfirelistmessage(outputPlace);
                NewState->cardFireList = CurrentCardFireList;
                
                NewState->current = 0;
                NewState->parent = CurrentState;
                if (NewState->succ == NULL) {
                    NewState->succ = new State * [CurrentCardFireList];
                    for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
                        NewState->succ[istate] = NULL;
                    }
                }
                NewState->placeHashValue = placeHashValue;
                NewState->type = typeOfState();

                // shall we save this state? meaning, is the correct output place marked?
                if (CurrentMarking[getPlaceIndex(outputPlace)] > 0) { 
                    //stateSet.insert(NewState);
                    if (removeOutputMessage(getPlaceIndex(outputPlace))) { // remove the output message from the current marking
                    	trace(TRACE_5, "activates receiving event " + outputPlace->getLabelForCommGraph() + "\n");
                    	// calc the reachable states from that marking using stubborn set method taking
                    	// care of deadlocks
#ifdef CHECKCAPACITY
                    try {
#endif                
                        // calc the reachable states from that marking
                    	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n); 
                        storeInNode = false;
#ifdef CHECKCAPACITY
                    }
                    catch (CapacityException& ex)
                    {
                        trace(TRACE_1, "\t\t\t\t\t adding input event would cause capacity bound violation on place " + ex.place + "\n");
                        trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, AnnotatedGraphNode* oldNode, AnnotatedGraphNode* newNode) : end\n");
                        n->setColor(RED);
                        throw;
                        return;
                    }
#endif                
                        
                    	if (n->getColor() == RED) {
                            trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                           + n->getName()
                                           + " set to RED (calculateReachableStatesFull, during fire)\n");
                            trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end\n");

                            //assert(CurrentState->succ[CurrentState->current] == NULL);
                            CurrentState->succ[CurrentState->current] = NewState;

                            delete[] tempCurrentMarking;
                            return;
                        }

                        CurrentState->succ[CurrentState->current] = NewState;
                    	
	                    // nothing else to be done here
	                    // so we go back to parent state
	                    // close state and return to previous state
	                    trace(TRACE_5, "close state [" + getCurrentMarkingAsString() + "] and return to previous state\n");
//	                    CurrentState = CurrentState->parent;
	
	                    if (CurrentState) { // there is a father to further examine
	                        CurrentState->decode(this);
	                        CurrentState->current++;
	                    }
                    }
                } else {
                    CurrentState->succ[CurrentState->current] = NewState;
                	CurrentState = NewState;
                }
                
                if (tempCurrentMarking) {
                    delete[] tempCurrentMarking;
                    tempCurrentMarking = NULL;
                }
            }
            // no more transition to fire
        } else {
            // close state and return to previous state
            trace(TRACE_5, "close state [" + getCurrentMarkingAsString() + "] and return to previous state\n");
            
            if (CurrentState->firelist) {
            	delete[] CurrentState->firelist;
            	CurrentState->firelist = NULL;
            }
            
            CurrentState = CurrentState->parent;

            if (CurrentState) { // there is a father to further examine
                CurrentState->decode(this);
                CurrentState->current++;
            }
        }
#ifdef CHECKCAPACITY
            }
            catch (CapacityException& ex)
            {
                trace(TRACE_1, "\t firing transition caused capacity violation on place " + ex.place + "\n");
                CurrentState = CurrentState->parent;

                if (CurrentState) 
                { // there is a father to further examine
                    CurrentState->decode(this);
                    CurrentState->current++;
                }
                throw;
            }
#endif
    }
    if (tempCurrentMarking) {
        delete[] tempCurrentMarking;
        tempCurrentMarking = NULL;
    }


    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace, AnnotatedGraphNode * n) : end\n");
    return;
}


//! \brief	checks if the current marking activates at least one of the receiving events of the given 
//!			message multi set
//! \param	messages	the event(s) for which the new AnnotatedGraphNode's EG is calculated, contains all receiving events
bool oWFN::currentMarkingActivatesNecessaryReceivingEvent(messageMultiSet messages) {
	// store the current marking 
	unsigned int* tempCurrentMarking = copyCurrentMarking();
	
	// let's see if the current marking activates a receiving event already
    bool somePlaceNotMarked = false; // remember if a place is not marked at the current marking

    // shall we save this state? meaning, are the correct output places all marked?
    for (messageMultiSet::iterator iter = messages.begin(); iter
            != messages.end(); iter++) {
        if (tempCurrentMarking[getPlaceIndex(getPlace(*iter))] == 0) {
            somePlaceNotMarked = true;
            break;
        }
        tempCurrentMarking[getPlaceIndex(getPlace(*iter))] -= 1;
    }
    
    if (tempCurrentMarking) {
        delete[] tempCurrentMarking;
        tempCurrentMarking = NULL;
    }
    
    return !somePlaceNotMarked;
}


//! \brief is only used in case of an output event and calculates the set of states reachable from 
//!		   the current marking and stores them in the given stateSet 
//!		   (this function is used for the IG only since a multiset of output places is considered for IG)
//! \param stateSet store calculated states temporarily
//! \param setOfStatesStubbornTemp store calculated states that are needed to calculate the CNF later on
//! \param tempBinDecision store calculated states temporarily in a binDecicion structure
//! \param messages the event(s) for which the new AnnotatedGraphNode's EG is calculated
//! \param n new AnnotatedGraphNode 
void oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet,
												StateSet& setOfStatesStubbornTemp, 
                                                binDecision** tempBinDecision,
                                                messageMultiSet messages,
                                                AnnotatedGraphNode* n) {

    // calculates the EG starting at the current marking
    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, messageMultiSet messages, AnnotatedGraphNode * n) : start\n");

    // test current marking if message bound k reached
    if (violatesMessageBound()) {
        n->setColor(RED);
        trace(TRACE_3, "\t\t\t message bound violated; color of node "
                       + n->getName() + " set to RED (calculateReducedSetOfReachableStates, oben)\n");
        return;
    }

    trace(TRACE_5, "current marking: [" + getCurrentMarkingAsString() + "]\n");
    
    unsigned int* tempCurrentMarking = NULL;
    unsigned int tempPlaceHashValue;

    State* CurrentState = NULL;
    State* NewState;
    
    bool storeInNode = true;
    
    // note: we don't need to look up the current marking in the global binDecision (CurrentState=binSearch(this))
    // we use the global binDecision to store states leading to a deadlock
    // in this function we are interested in those states that lead to a marking which activates receiving events
    CurrentState = binSearch(*tempBinDecision, this);
    CurrentState = binInsert(tempBinDecision, this); // save current state to the local binDecision 

    CurrentState->current = 0;
    CurrentState->parent = NULL;

    if (currentMarkingActivatesNecessaryReceivingEvent(messages)) { // if all places are appropriatly marked, we save this state in a temporary state set
        //stateSet.insert(CurrentState);
    	
        if (removeOutputMessage(messages)) { // remove the output message from the current marking
        	trace(TRACE_5, "... activates receiving event\n");
        	// calc the reachable states from that marking using stubborn set method taking
        	// care of deadlocks
        	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n); 
    	}

        trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace, AnnotatedGraphNode * n) : end\n");
        // nothing else to be done here
        if (tempCurrentMarking) {
            delete[] tempCurrentMarking;
            tempCurrentMarking = NULL;
        }
        return;
    }

    // the current state does not activate any receiving event, so we check out its successor states
    CurrentState->firelist = stubbornfirelistmessage(messages);
    CurrentState->cardFireList = CurrentCardFireList;

    if (CurrentState->succ == NULL) {
        CurrentState->succ = new State * [CurrentCardFireList];
        for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
            CurrentState->succ[istate] = NULL;
        }
    }

    CurrentState->placeHashValue = placeHashValue;
    CurrentState->type = typeOfState();

    // building EG in a node
    while (CurrentState) {

#ifdef CHECKCAPACITY
            try
            {
#endif
        // no more transition to fire from current state?
    	if (CurrentState->current < CurrentState->cardFireList) {
            // there is a next state that needs to be explored

            if (tempCurrentMarking) {
                delete[] tempCurrentMarking;
                tempCurrentMarking = NULL;
            }

            // remember the current marking
            tempCurrentMarking = copyCurrentMarking();
            tempPlaceHashValue = placeHashValue;

            // fire and reach next state
            CurrentState->firelist[CurrentState->current]->fire(this);
            
            NewState = binSearch(*tempBinDecision, this);

            if (NewState != NULL) {
                // Current marking already in local bintree 
                trace(TRACE_5, "Current marking [" + getCurrentMarkingAsString() +  "] already in local bintree \n");

                // test current marking if message bound k reached
                if (violatesMessageBound()) {
                    n->setColor(RED);
                    trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                   + n->getName()
                                   + " set to RED (calculateReducedSetOfReachableStates, during fire)\n");
                    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace, AnnotatedGraphNode * n) : end\n");
                    if (tempCurrentMarking != NULL) {
                        delete[] tempCurrentMarking;
                        tempCurrentMarking = NULL;
                    }
                    return;
                }
                
//                bool somePlaceNotMarked = false; // remember if a place is not marked at the current marking
//                
//                unsigned int* tempCurrentMarking2 = copyGivenMarking(tempCurrentMarking);
//                
//                // shall we save this state? meaning, are the correct output places marked?
//                for (messageMultiSet::iterator iter = messages.begin(); iter
//                        != messages.end(); iter++) {
//                    if (tempCurrentMarking2[getPlaceIndex(getPlace(*iter))] <= 0) {
//                        somePlaceNotMarked = true;
//                        break;
//                    }
//                   	tempCurrentMarking2[getPlaceIndex(getPlace(*iter))] -= 1;
//                }
//
//                delete[] tempCurrentMarking2;
                
//                if (!somePlaceNotMarked) { // if all places are appropriatly marked, we save this state
//                	//stateSet.insert(CurrentState);
//                	if (removeOutputMessage(messages)) { // remove the output message from the current marking
//                		trace(TRACE_5, "activates receiving events\n");
//                		// calc the reachable states from that marking using stubborn set method taking
//                    	// care of deadlocks
//                    	calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n);
//                        
//                    	if (n->getColor() == RED) {
//                            trace(TRACE_3, "\t\t\t message bound violated; color of node "
//                                           + n->getName()
//                                           + " set to RED (calculateReachableStatesFull, during fire)\n");
//                            trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end\n");
//
//                            assert(CurrentState->succ[CurrentState->current] == NULL);
//                            CurrentState->succ[CurrentState->current] = NewState;                        
//                            
//                            delete[] tempCurrentMarking;
//                            return;
//                        }
//                	} 
//                } else {
                    // no it is not marked appropriatly, so we take a look at its successor states
                    addSuccStatesToListStubborn(stateSet, setOfStatesStubbornTemp, messages, NewState, n);
                    
                    if (n->getColor() == RED) {
                        trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                       + n->getName()
                                       + " set to RED (calculateReachableStatesFull, during fire)\n");
                        trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end\n");

                        assert(CurrentState->succ[CurrentState->current] == NULL);
                        CurrentState->succ[CurrentState->current] = NewState;                        
                        
                        delete[] tempCurrentMarking;
                        return;
                    }
               // }

                // we don't need the current marking anymore so we step back to the marking before
                CurrentState->decode(this);
                CurrentState->firelist[CurrentState->current]->backfire(this);

                placeHashValue = tempPlaceHashValue;

                delete[] tempCurrentMarking;
                tempCurrentMarking = NULL;

                assert(CurrentState->succ[CurrentState->current] == NULL);
                CurrentState->succ[CurrentState->current] = NewState;
                (CurrentState->current)++;
            } else {
                trace(TRACE_5, "Current marking [" + getCurrentMarkingAsString() +  "] new\n");
                NewState = binInsert(tempBinDecision, this);
                
                assert(CurrentState->succ[CurrentState -> current] == NULL);
                CurrentState->succ[CurrentState -> current] = NewState;
                
                // test current marking if message bound k reached
                if (violatesMessageBound()) {
                    n->setColor(RED);
                    trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                   + n->getName()
                                   + " set to RED (calculateReducedSetOfReachableStates, during fire)\n");
                    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace, AnnotatedGraphNode * n) : end\n");
                    if (tempCurrentMarking != NULL) {
                        delete[] tempCurrentMarking;
                        tempCurrentMarking = NULL;
                    }
                    return;
                }
                
                NewState->firelist = stubbornfirelistmessage(messages);
                NewState->cardFireList = CurrentCardFireList;

                NewState->current = 0;
                NewState->parent = CurrentState;
                if (NewState->succ == NULL) {
                    NewState->succ = new State * [CurrentCardFireList];
                    for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
                        NewState->succ[istate] = NULL;
                    }
                }
                NewState->placeHashValue = placeHashValue;
                NewState->type = typeOfState();

                // if all places are appropriatly marked, we save this state
                if (currentMarkingActivatesNecessaryReceivingEvent(messages)) { 
                	
                	storeInNode = true;//currentMarkingActivatesReceivingEvent(tempCurrentMarking);
                	
                	//stateSet.insert(CurrentState);
                	if (removeOutputMessage(messages)) { // remove the output message from the current marking
                		trace(TRACE_5, "activates receiving events\n");
                    	// calc the reachable states from that marking using stubborn set method taking
                    	// care of deadlocks
                		calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, n);
                    	
                        if (n->getColor() == RED) {
                            trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                           + n->getName()
                                           + " set to RED (calculateReducedSetOfReachableStates, during fire)\n");
                            trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace, AnnotatedGraphNode * n) : end\n");

                            assert(CurrentState->succ[CurrentState->current] == NULL);
                            CurrentState->succ[CurrentState->current] = NewState;                        
                            
                            delete[] tempCurrentMarking;
                            return;
                        }
                        
                        CurrentState->succ[CurrentState->current] = NewState;
                        
	                    // nothing else to be done here
	                    // so we go back to parent state
	                    // close state and return to previous state
	                  //  trace(TRACE_5, "close state [" + getCurrentMarkingAsString() + "] and return to previous state\n");
//	                    CurrentState = CurrentState->parent;
	
	                    if (CurrentState) { // there is a father to further examine
	                        CurrentState->decode(this);
	                        CurrentState->current++;
	                        
	                        trace(TRACE_5, "close state [" + getCurrentMarkingAsString() + "] and calc next state\n");
	                    }
                	}
                } else {
                    CurrentState->succ[CurrentState->current] = NewState;
                	CurrentState = NewState;
                }

                if (tempCurrentMarking) {
                    delete[] tempCurrentMarking;
                    tempCurrentMarking = NULL;
                }
            }
            // no more transition to fire
        } else {
            // close state and return to previous state
            trace(TRACE_5, "close state [" + getCurrentMarkingAsString() + "] and return to previous state\n");
            
            if (CurrentState->firelist) {
            	delete[] CurrentState->firelist;
            	CurrentState->firelist = NULL;
            }
            
            CurrentState = CurrentState->parent;

            if (CurrentState) { // there is a father to further examine
                CurrentState->decode(this);
                CurrentState->current++;
            }
        }
#ifdef CHECKCAPACITY
            }
            catch (CapacityException& ex)
            {
                trace(TRACE_1, "\t firing transition caused capacity violation on place " + ex.place + "\n");
                CurrentState = CurrentState->parent;

                if (CurrentState) 
                { // there is a father to further examine
                    CurrentState->decode(this);
                    CurrentState->current++;
                }
                throw;
            }
#endif
    }
    if (tempCurrentMarking) {
        delete[] tempCurrentMarking;
        tempCurrentMarking = NULL;
    }

    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace, AnnotatedGraphNode * n) : end\n");
    return;
}

//! \brief checks if the current marking activates a "new" receiving event, so the current marking is checked
//!			whether it activates a receiving event and its parent marking is also checked if it activates the
//!			same receiving events
//!			if the current marking activates a receiving event that has not been activated by its parent the function
//!			returns true
//!	\param	parentMarking marking of the predecessor state of the current marking
//!	\return	true, if current marking activates receiving event that has not yet been activated; false if not
bool oWFN::currentMarkingActivatesReceivingEvent(unsigned int* parentMarking) {

    trace(TRACE_5, "oWFN::currentMarkingActivatesReceivingEvent(unsigned int* parentMarking) : start\n");
	
    if (parentMarking != NULL) {
    	trace(TRACE_5, "parent marking: [" + getMarkingAsString(parentMarking) + "]\n");
    } else {
    	trace(TRACE_5, "parent marking: NULL\n");
    }
    
	// if current marking has a predecessor marking
	if (parentMarking != NULL) {
		// test every possible receiving event, if it is activated in the current marking
		for (unsigned int e = 0; e < PN->getOutputPlaceCount(); e++) {
	        // test if the predecessor marking activates this receiving event, meaning is the corresponding
	        // output place marked?
	        if ((parentMarking[getPlaceIndex(getOutputPlace(e))] == 0) && 
	                          (CurrentMarking[getPlaceIndex(getOutputPlace(e))] > 0)) {
	        	// no, parent marking does not activate this receiving event, but the current marking does
	        	// !we found a receiving event that only the current marking activates for now
	    		trace(TRACE_5, "marking [" + getCurrentMarkingAsString() + "] activates some receiving event that its parent did not activate: yes\n");
	    		trace(TRACE_5, "oWFN::currentMarkingActivatesReceivingEvent(unsigned int* parentMarking) : end\n");
	    		return true;
	        }
	    }
		trace(TRACE_5, "marking [" + getCurrentMarkingAsString() + "] activates some receiving event that its parent did not activate: no\n");
		trace(TRACE_5, "oWFN::currentMarkingActivatesReceivingEvent(unsigned int* parentMarking) : end\n");
		return false;
	} else { // current marking has no predecessor marking - no parent
		// test every possible receiving event, if it is activated in the current marking
		for (unsigned int e = 0; e < PN->getOutputPlaceCount(); e++) {
	        if (CurrentMarking[getPlaceIndex(getOutputPlace(e))] > 0) {
	        	// current marking activates a receiving event, return true
	    		trace(TRACE_5, "marking [" + getCurrentMarkingAsString() + "] activates some receiving event\n");
	        	trace(TRACE_5, "oWFN::currentMarkingActivatesReceivingEvent(unsigned int* parentMarking) : end\n");
	            return true;
	        }
	    }
	}

	trace(TRACE_5, "marking [" + getCurrentMarkingAsString() + "] activates no receiving event\n");
	trace(TRACE_5, "oWFN::currentMarkingActivatesReceivingEvent(unsigned int* parentMarking) : end\n");
	return false;
}


//! \brief calculates all transition that a "stubbornly" activated 
//! \param state current state
void oWFN::calculateStubbornTransitions(State* state) {
	trace(TRACE_5, "void oWFN::calculateStubbornTransitions(State* state) : start\n");
    
	// calculate all transitions needed to reach a deadlock state
    owfnTransition ** resultDeadlocks = stubbornfirelistdeadlocks();
    unsigned int cardFireListDeadlocks = CurrentCardFireList;
    
    trace(TRACE_5, "currently " + intToString(cardFireListDeadlocks) + " transitions in stubborn firelist (deadlocks)\n");
    
    // calculate all transitions needed to reach a state that activates a receiving event
    owfnTransition ** resultAllMessages = stubbornFirelistAllMessages(); 
    unsigned int cardFireListAllMessages = CurrentCardFireList;
    
    trace(TRACE_5, "currently " + intToString(cardFireListAllMessages) + " transitions in stubborn firelist (output messages)\n");
    
    std::set<owfnTransition*> setOfTransitions;
    
    unsigned int i;

    // the interseciton of the set resultDeadlocks and the set resultAllMessages might not be empty
    // but we have to make sure that no transition is fired twice for the same state
    // so we store all the transitions in a new set and thus, remove those transitions being contained in both sets
    if (resultDeadlocks[0] != NULL) {
	    for (i = 0; i < cardFireListDeadlocks; i++) {
	    	setOfTransitions.insert(resultDeadlocks[i]);
	    }
    }
    if (resultAllMessages[0] != NULL) {
	    for (i = 0; i < cardFireListAllMessages; i++) {
	    	setOfTransitions.insert(resultAllMessages[i]);
	    }
    }
    
    // set up a new array containing all "stubbornly" activated transitions
    owfnTransition ** result = new owfnTransition * [setOfTransitions.size() + 1];
    
    trace(TRACE_5, "currently " + intToString(setOfTransitions.size()) + " transitions in stubborn firelist (all)\n");
    
    i = 0;
    for (std::set<owfnTransition*>::iterator iter = setOfTransitions.begin(); 
    											iter != setOfTransitions.end(); 
    											iter++) {
    	result[i++] = (*iter);
    }
        
    // delete the old information
    if (state->firelist) {
    	delete[] state->firelist;
    }
    
    // delete all temporarily calculated arrays
    delete[] resultAllMessages;
    delete[] resultDeadlocks;
    
    // store the new array and its size
    state->firelist = result;
    state->cardFireList = setOfTransitions.size();

	trace(TRACE_5, "void oWFN::calculateStubbornTransitions(State* state) : end\n");
}

//! \brief calculates the reduced set of states of the new AnnotatedGraphNode 
//!        for IG and OG with state set reduction
//!		   --> by applying the "representative" approach
//! \param stateSet set of states storing the states that were calculated using the stubborn set method
//!		   needed to calculate the CNF later on
//! \param n the node to be calculated in case of an input event
void oWFN::calculateReducedSetOfReachableStatesStoreInNode(StateSet& stateSet, AnnotatedGraphNode* n) {

    // calculates the EG starting at the current marking
    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n): start\n");

    // test current marking if message bound k reached
    if (violatesMessageBound()) {
        n->setColor(RED);
        trace(TRACE_3, "\t\t\t message bound violated; color of node "
                       + n->getName()
                       + " set to RED (calculateReachableStatesFull, oben)\n");
        return;
    }
    
    State * CurrentState;
    State * NewState;	

    CurrentState = binSearch(this);

    unsigned int * tempCurrentMarking = NULL;
    unsigned int tempPlaceHashValue;

    if (CurrentState != NULL) {
        // marking already has a state -> put it (and all its successors) into the node
        if (n->addState(CurrentState)) {
            addSuccStatesToNodeStubborn(n, stateSet, NULL, CurrentState);
        }
        trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n): end\n");
        return;
    }

    assert(CurrentState == NULL);
    
    // the other case:
    // we have a marking which has not yet a state object assigned to it
    CurrentState = binInsert(this);
    
    // calculate which transitions are activated in the current marking
    // here the stubborn set method is used
    // --> preserving deadlocks and states that activate a receiving event
    calculateStubbornTransitions(CurrentState);
    
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
    CurrentState->succ = new State * [CurrentState->cardFireList];
    for (unsigned int i = 0; i < CurrentState->cardFireList; i++) {
        CurrentState->succ[i] = NULL;
    }
    CurrentState->placeHashValue = placeHashValue;
    CurrentState->type = typeOfState();

    if (CurrentState->type == DEADLOCK || CurrentState->type == FINALSTATE) {
    	stateSet.insert(CurrentState);
    }
    
    n->addState(CurrentState);

    // building EG in a node
    while (CurrentState) {

        if ((n->reachGraphStateSet.size() % 1000) == 0) {
            trace(TRACE_2, "\t current state count: "
                    + intToString(n->reachGraphStateSet.size()) + "\n");
        }

#ifdef CHECKCAPACITY
            try
            {
#endif
        // no more transition to fire from current state?
        if (CurrentState->current < CurrentState->cardFireList) {
            // there is a next state that needs to be explored

            if (tempCurrentMarking) {
                delete[] tempCurrentMarking;
                tempCurrentMarking = NULL;
            }

            tempCurrentMarking = copyCurrentMarking();
            tempPlaceHashValue = placeHashValue;

            trace(TRACE_5, "in state [" + getCurrentMarkingAsString() + "] (current = " + intToString(CurrentState->current) + ") fire transition " + CurrentState->firelist[CurrentState->current]->getLabelForMatching() + "\n");

            CurrentState->firelist[CurrentState->current]->fire(this);
            
            NewState = binSearch(this);

            if (NewState != NULL) {
                // Current marking already in bintree 
                trace(TRACE_5, "Current marking [" + getCurrentMarkingAsString() +  "] already in bintree \n");

                assert(CurrentState->succ[CurrentState->current] == NULL);
                CurrentState->succ[CurrentState->current] = NewState;

                addSuccStatesToNodeStubborn(n, stateSet, tempCurrentMarking, NewState);

                if (n->getColor() == RED) {
                    trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                   + n->getName()
                                   + " set to RED (calculateReducedSetOfReachableStatesStoreInNode, during fire; known state)\n");
                    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n) : end\n");
                    delete[] tempCurrentMarking;
                    return;
                }
                
                copyMarkingToCurrentMarking(tempCurrentMarking);

                CurrentState->firelist[CurrentState->current]->backfire(this);

                placeHashValue = tempPlaceHashValue;

                delete[] tempCurrentMarking;
                tempCurrentMarking = NULL;

                (CurrentState->current)++;
            } else {
                trace(TRACE_5, "Current marking [" + getCurrentMarkingAsString() +  "] new!\n");
                
                NewState = binInsert(this);

                assert(CurrentState->succ[CurrentState -> current] == NULL);
                CurrentState->succ[CurrentState -> current] = NewState;
                
                // test current marking if message bound k reached
                if (violatesMessageBound()) {
                	n->setColor(RED);
                	trace(TRACE_3, "\t\t\t message bound violated; color of node "
                			+ n->getName()
                			+ " set to RED (calculateReducedSetOfReachableStatesStoreInNode, during fire; new state)\n");
                	trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n) : end\n");
                	delete[] tempCurrentMarking;
                	return;
                }
                
                // calculate which transitions are activated in the current marking
                // here the stubborn set method is used
                // --> preserving deadlocks and states that activate a receiving event
                calculateStubbornTransitions(NewState);
                
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
                NewState->succ = new State * [NewState->cardFireList];
                for (unsigned int i = 0; i < NewState->cardFireList; i++) {
                    NewState->succ[i] = NULL;
                }
                NewState->placeHashValue = placeHashValue;
                NewState->type = typeOfState();
                
                CurrentState = NewState;

                // store each marking that activates a receiving event in the new node
                // make sure, it activates one that its parent did not activate
                if (currentMarkingActivatesReceivingEvent(tempCurrentMarking)) {
                	n->addState(CurrentState);
                }
                
                // store each deadlock in state set 
                // --> needed to calculate the CNF of the new node later on
                if (CurrentState->type == DEADLOCK || CurrentState->type == FINALSTATE) {
                	stateSet.insert(CurrentState);
                }

                if (tempCurrentMarking) {
                    delete[] tempCurrentMarking;
                    tempCurrentMarking = NULL;
                }
            }
            // no more transition to fire
        } else {
            // close state and return to previous state
            trace(TRACE_5, "close state [" + getCurrentMarkingAsString() + "] and return to previous state\n");
            
            if (CurrentState->firelist) {
            	delete[] CurrentState->firelist;
            	CurrentState->firelist = NULL;
            }
            
            CurrentState = CurrentState->parent;

            if (CurrentState) { // there is a father to further examine
                CurrentState->decode(this);
                CurrentState->current++;
            }
        }
#ifdef CHECKCAPACITY
            }
            catch (CapacityException& ex)
            {
                trace(TRACE_1, "\t firing transition caused capacity violation on place " + ex.place + "\n");
                CurrentState = CurrentState->parent;

                if (CurrentState) 
                { // there is a father to further examine
                    CurrentState->decode(this);
                    CurrentState->current++;
                }
                throw;
            }
#endif
    }
    if (tempCurrentMarking) {
        delete[] tempCurrentMarking;
    }

    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(StateSet& stateSet, binDecision**, AnnotatedGraphNode* n): end\n");
}

//! \brief is only used in case of an output event and calculates the set of states reachable from 
//!		   the current marking and stores them in the given stateSet 
//!		   (this function is used for the IG only since a multiset of output places is considered for IG)
//! \param tempBinDecision store calculated states temporarily in a binDecicion structure
//! \param stateSet store calculated states in this state set
//! \param messages the event(s) for which the new AnnotatedGraphNode's EG is calculated
//! \param n new AnnotatedGraphNode 
//void oWFN::calculateReducedSetOfReachableStatesStoreInNode(binDecision** tempBinDecision,
//                                                			AnnotatedGraphNode* n) {
//
//    // calculates the EG starting at the current marking
//    trace(TRACE_0, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(binDecision** tempBinDecision, AnnotatedGraphNode * n) : start\n");
//
//    // test current marking if message bound k reached
//    if (violatesMessageBound()) {
//        n->setColor(RED);
//        trace(TRACE_3, "\t\t\t message bound violated; color of node "
//                       + n->getName() + " set to RED (calculateReducedSetOfReachableStates, oben)\n");
//        return;
//    }
//
//    State* CurrentState;
//    State* NewState;
//
//    CurrentState = binSearch(this);
//
//    unsigned int* tempCurrentMarking = NULL;
//    unsigned int tempPlaceHashValue;
//
//    if (CurrentState == NULL) {
//        // we have a marking which has not yet a state object assigned to it
//        CurrentState = binInsert(this); // save current state to the global binDecision
//
//      //  CurrentState = binSearch(*tempBinDecision, this);
//      //  CurrentState = binInsert(tempBinDecision, this); // save current state to the local binDecision 
//
//        CurrentState->current = 0;
//        CurrentState->parent = NULL;
//
//        // marking already has a state -> put it (and all its successors) into the node
//        //		addSuccessorStatesToSetStubborn(CurrentState, stateSet);
//        //	trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStates(StateSet& stateSet, owfnPlace * outputPlace): end\n");
//        //return;
//    } else {
//    	
//    	if (currentMarkingActivatesReceivingEvent(NULL)) { // if all places are appropriatly marked, we save this state
//          //  stateSet.insert(CurrentState);
//            if (!n->addState(CurrentState)) {
//            	addSuccStatesToNodeStubborn(n, NewState);
//            }
//            
//            return ;
//        }
//    	
//     //   CurrentState = binSearch(*tempBinDecision, this);
//
//        // save current state to the local binDecision 
//     //   CurrentState = binInsert(tempBinDecision, this);
//    }
//
//    //CurrentState->stubbornFirelist = stubbornfirelistmessage(messages);
//    //CurrentState->cardStubbornFireList = CurrentCardFireList;
//    
//    CurrentState->current = 0;
//    CurrentState->firelist = stubbornFirelistAllMessages();
//    CurrentState->cardFireList = CurrentCardFireList;
//    cout << "current state [" << getMarkingAsString(CurrentMarking) << "] activates transitions: " << endl;
//    for (unsigned int i = 0; i < CurrentState->cardFireList; i++) {
//    	cout << CurrentState->firelist[i]->getLabelForMatching() << ", " << endl;
//    }
//    cout << "CurrentState->current: " << CurrentState->current << endl;
//    
//    if (CurrentState->succ == NULL) {
//        CurrentState->succ = new State * [CurrentCardFireList];
//        for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
//            CurrentState->succ[istate] = NULL;
//        }
//    }
//
//    CurrentState->placeHashValue = placeHashValue;
//    CurrentState->type = typeOfState();
//
//    if (currentMarkingActivatesReceivingEvent(NULL)) { // if all places are appropriatly marked, we save this state
//     //   stateSet.insert(CurrentState);
//    	cout << "marking activates receiving event!!!" << endl;
//    	printCurrentMarking();
//        n->addState(CurrentState);
//    }
//
//    // building EG in a node
//    while (CurrentState) {
//
//        // no more transition to fire from current state?
//    	if (CurrentState->current < CurrentState->cardFireList) {
//        //if (CurrentState->current < CurrentState->cardStubbornFireList) {
//            // there is a next state that needs to be explored
//
//            if (tempCurrentMarking) {
//                delete[] tempCurrentMarking;
//                tempCurrentMarking = NULL;
//            }
//
//            // remember the current marking
//            tempCurrentMarking = copyCurrentMarking();
//            tempPlaceHashValue = placeHashValue;
//
//            cout << "fire transition " << CurrentState->firelist[CurrentState->current]->getLabelForMatching() << endl;
//            
//            // fire and reach next state
//            CurrentState->firelist[CurrentState->current]->fire(this);
//
//            cout << "... got new marking [" << getMarkingAsString(CurrentMarking) << "]" << endl;
//            
//            NewState = binSearch(this);
//
//            if (NewState != NULL) {
//                // Current marking already in bintree 
//                trace(TRACE_0, "Current marking already in bintree \n");
//
//                if (currentMarkingActivatesReceivingEvent(tempCurrentMarking)) { // if all places are appropriatly marked, we save this state
//                  //  stateSet.insert(CurrentState);
//                    n->addState(CurrentState);
//                }
//                
//                    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////                } else {
//                	StateSet stateSetTemp;
//                    // no it is not marked appropriatly, so we take a look at its successor states
//                    addSuccStatesToNodeStubborn(n, NewState);
//                    stateSetTemp.clear();
//                    
//                    if (n->getColor() == RED) {
//                        trace(TRACE_3, "\t\t\t message bound violated; color of node "
//                                       + n->getName()
//                                       + " set to RED (calculateReachableStatesFull, during fire)\n");
//                        trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(binDecision** tempBinDecision, AnnotatedGraphNode * n) : end\n");
//
//                        assert(CurrentState->succ[CurrentState->current] == NULL);
//                        CurrentState->succ[CurrentState->current] = NewState;                        
//                        
//                        delete[] tempCurrentMarking;
//                        return;
//                    }
//                
//                
//                // we don't need the current marking anymore so we step back to the marking before
//                copyMarkingToCurrentMarking(tempCurrentMarking);
//
//                CurrentState->firelist[CurrentState->current]->backfire(this);
//
//                placeHashValue = tempPlaceHashValue;
//
//                delete[] tempCurrentMarking;
//                tempCurrentMarking = NULL;
//
//                assert(CurrentState->succ[CurrentState->current] == NULL);
//                CurrentState->succ[CurrentState->current] = NewState;
//                (CurrentState->current)++;
//            } else {
//                trace(TRACE_0, "Current marking new\n");
//
//                if (currentMarkingActivatesReceivingEvent(tempCurrentMarking)) { // if all places are appropriatly marked, we save this state
//                	NewState = binInsert(this);
//                //	stateSet.insert(NewState);
//                	n->addState(NewState);
//                	cout << "marking activates receiving event!!!" << endl;
//                	    	printCurrentMarking();
//                	
//                } else {
//                	NewState = binSearch(*tempBinDecision, this);
//                	if (NewState == NULL) {
//                		NewState = binInsert(tempBinDecision, this);
//                	}
//                }
//                	NewState->firelist = stubbornFirelistAllMessages();
//		                NewState->cardFireList = CurrentCardFireList;
//		
//		                cout << "current state [" << getMarkingAsString(CurrentMarking) << "] activates transitions: " << endl;
//		                for (unsigned int i = 0; i < NewState->cardFireList; i++) {
//		                	cout << NewState->firelist[i]->getLabelForMatching() << ", " << endl;
//		                }
//		                
//		                NewState->current = 0;
//		                NewState->parent = CurrentState;
//		                if (NewState->succ == NULL) {
//		                    NewState->succ = new State * [CurrentCardFireList];
//		                    for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
//		                        NewState->succ[istate] = NULL;
//		                    }
//		                }
//		                NewState->placeHashValue = placeHashValue;
//		                NewState->type = typeOfState();
//		
//		                assert(CurrentState->succ[CurrentState -> current] == NULL);
//		                CurrentState->succ[CurrentState -> current] = NewState;
//		                CurrentState = NewState;
//		
//		                // test current marking if message bound k reached
//		                if (violatesMessageBound()) {
//		                    n->setColor(RED);
//		                    trace(TRACE_3, "\t\t\t message bound violated; color of node "
//		                                   + n->getName()
//		                                   + " set to RED (calculateReducedSetOfReachableStates, during fire)\n");
//		                    trace(TRACE_5, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(binDecision** tempBinDecision, AnnotatedGraphNode * n) : end\n");
//		                    if (tempCurrentMarking != NULL) {
//		                        delete[] tempCurrentMarking;
//		                        tempCurrentMarking = NULL;
//		                    }
//		                    return;
//		                }
//
////                    // nothing else to be done here
////                    // so we go back to parent state
////                    // close state and return to previous state
////                    trace(TRACE_5, "close state and return to previous state\n");
////                    CurrentState = CurrentState->parent;
////
////                    if (CurrentState) { // there is a father to further examine
////                        CurrentState->decode(this);
////                        CurrentState->current++;
////                    }
//                }
//
//                if (tempCurrentMarking) {
//                    delete[] tempCurrentMarking;
//                    tempCurrentMarking = NULL;
//                }
//           // }
//            // no more transition to fire
//        } else {
//            // close state and return to previous state
//            trace(TRACE_0, "close state and return to previous state\n");
//            CurrentState = CurrentState->parent;
//
//            if (CurrentState) { // there is a father to further examine
//                CurrentState->decode(this);
//                CurrentState->current++;
//            }
//        }
//    }
//    if (tempCurrentMarking) {
//        delete[] tempCurrentMarking;
//        tempCurrentMarking = NULL;
//    }
//
//    trace(TRACE_0, "oWFN::calculateReducedSetOfReachableStatesStoreInNode(binDecision** tempBinDecision, AnnotatedGraphNode * n) : end\n");
//    return;
//}


//! \brief NO REDUCTION! calculate all reachable states from the current marking
//!        and store them in the node n (== AnnotatedGraphNode of CommunicationGraph);
//!        it will color the node n RED if a given message bound is violated
//! \param n the node for which the reachability graph is computed
void oWFN::calculateReachableStatesFull(AnnotatedGraphNode* n) {

    // calculates the EG starting at the current marking
    trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode* n) : start\n");

    if (violatesMessageBound()) {
        n->setColor(RED);
        trace(TRACE_3, "\t\t\t message bound violated; color of node "
                       + n->getName()
                       + " set to RED (calculateReachableStatesFull, oben)\n");
        return;
    }

    State * CurrentState;
    CurrentState = binSearch(this);

    unsigned int * tempCurrentMarking = NULL;
    unsigned int tempPlaceHashValue;

    //	if (options[O_BDD] == false && CurrentState != NULL) {

    if (CurrentState != NULL) {
        // marking already has a state -> put it with all successors into the node

        if (n->addState(CurrentState)) {
            // successors need only be added if state was not yet in current node
            addSuccStatesToNode(n, CurrentState); // decodes and checks for message bound
        }
        trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end (root marking of EG already in bintree; states copied only)\n");
        return;
    } else {

        // the other case:
        // we have a marking which has not yet a state object assigned to it
        CurrentState = binInsert(this);

        CurrentState->firelist = firelist();
        CurrentState->cardFireList = CurrentCardFireList;

        // quasiFireList is needed for autonomous controllability
        if (parameters[P_IG] || parameters[P_AUTONOMOUS]) {
            if (CurrentState->quasiFirelist) {
                delete [] CurrentState->quasiFirelist;
                CurrentState->quasiFirelist = NULL;
            }
            CurrentState->quasiFirelist = quasiFirelist();
        }

        CurrentState->current = 0;
        CurrentState->parent = (State *) 0;
        assert(CurrentState->succ == NULL);

        CurrentState->succ = new State * [CurrentCardFireList];
        for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
            CurrentState->succ[istate] = NULL;
        }

        CurrentState->placeHashValue = placeHashValue;
        CurrentState->type = typeOfState();
        assert(CurrentState != NULL);
        n->addState(CurrentState);

        State * NewState;

#ifdef TSCC
//        CurrentState->succ = new State * [CurrentCardFireList + 1];
        CurrentState->dfs = CurrentState->lowlink = CurrentState->state_count - 1;
        CurrentState->nexttar = CurrentState->prevtar = CurrentState;
        TarStack = CurrentState;
        
        MinBookmark = CurrentState->state_count;
#endif        
        
        // building EG in a node
        while (CurrentState) 
        {
#ifdef CHECKCAPACITY
            try
            {
#endif
            
            	// no more transition to fire from current state?
            if (CurrentState->current < CurrentState->cardFireList) {
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

                if (NewState != NULL) {
                    // Current marking already in bintree 
                    trace(TRACE_5, "Current marking already in bintree \n"); 
                    if (n->addState(NewState)) {
                    	addSuccStatesToNode(n, NewState);
                        if (n->getColor() == RED) {
                            trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                           + n->getName()
                                           + " set to RED (calculateReachableStatesFull, during fire (known state)\n");
                            trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end\n");

                            assert(CurrentState->succ[CurrentState->current] == NULL);
                            CurrentState->succ[CurrentState->current] = NewState;
                            
                            delete[] tempCurrentMarking;
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
                    
#ifdef TSCC                    
                    CurrentState->succ[CurrentState->current] = NewState;
                    if(!(NewState->tarlevel)) {
	      				CurrentState->lowlink = MINIMUM(CurrentState->lowlink, NewState->lowlink);
                    }
#endif                    
                    
	      			(CurrentState->current)++;
                } else {
                    trace(TRACE_5, "Current marking new\n");	
                    NewState = binInsert(this);
                    
                    assert(CurrentState->succ[CurrentState -> current] == NULL);
                    CurrentState->succ[CurrentState -> current] = NewState;
                    
                    // test current marking if message bound k reached
                    if (violatesMessageBound()) {
                        n->setColor(RED);
                        trace(TRACE_3, "\t\t\t message bound violated; color of node "
                                       + n->getName()
                                       + " set to RED (calculateReachableStatesFull, during fire (new state))\n");
                        trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end\n");
                        delete[] tempCurrentMarking;
                        return;
                    }
                    
                    NewState->firelist = firelist();
                    
#ifdef TSCC
                    NewState->dfs = NewState->lowlink = CurrentState->state_count - 1;
					NewState->prevtar = TarStack;
					NewState->nexttar = TarStack->nexttar;
					TarStack = TarStack ->nexttar = NewState;
					NewState->nexttar->prevtar = NewState;
#endif                    
                    
                    NewState->cardFireList = CurrentCardFireList;
                    if (parameters[P_IG]) {
                        if (NewState->quasiFirelist) {
                            delete[] NewState->quasiFirelist;
                            NewState->quasiFirelist = NULL;
                        }
                        NewState->quasiFirelist = quasiFirelist();
                    }
                    NewState->current = 0;
                    NewState->parent = CurrentState;
                    
                    assert(NewState->succ == NULL);
                    NewState->succ = new State* [CurrentCardFireList];
                    for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
                        NewState->succ[istate] = NULL;
                    }
                    NewState->placeHashValue = placeHashValue;
                    NewState->type = typeOfState();

                    CurrentState = NewState;

                    assert(NewState != NULL);
                    n->addState(NewState);

                    if (tempCurrentMarking) {
                        delete[] tempCurrentMarking;
                        tempCurrentMarking = NULL;
                    }
                }
                // no more transition to fire
            } else {
#ifdef TSCC                    
            	// state is part of a TSCC and it is the representative of it
            	if((CurrentState->dfs == CurrentState->lowlink) 
            			&& (CurrentState->dfs >= MinBookmark)) {

            		MinBookmark = CurrentState->state_count;
            		
            		// remember that the current state is the representative of a TSCC
            		CurrentState->repTSCC = true;
            	}                     

            	if(CurrentState->parent) {
            		CurrentState->parent->lowlink = 
            			MINIMUM(CurrentState->lowlink, CurrentState->parent->lowlink);
            	}
#endif
            	
            	// close state and return to previous state
                trace(TRACE_5, "close state [" + getCurrentMarkingAsString() + "] and return to previous state\n");
                CurrentState = CurrentState->parent;
               
                if (CurrentState) { // there is a father to further examine
                    CurrentState->decode(this);
                    CurrentState->current++;
                }
            }
#ifdef CHECKCAPACITY
            }
            catch (CapacityException& ex)
            {
                trace(TRACE_1, "\t firing transition caused capacity violation on place " + ex.place + "\n");
                CurrentState = CurrentState->parent;

                if (CurrentState) 
                { // there is a father to further examine
                    CurrentState->decode(this);
                    CurrentState->current++;
                }
                throw;
            }
#endif
        }
        if (tempCurrentMarking) {
            delete[] tempCurrentMarking;
        }
        
        trace(TRACE_5, "oWFN::calculateReachableStatesFull(AnnotatedGraphNode * n) : end\n");
        return;
    }
}


//! \brief adds input message to the current marking
//! \param message message to be added to the currentmarking
void oWFN::addInputMessage(unsigned int message) {

    CurrentMarking[message]++;

    placeHashValue += getPlace(message)->hash_factor;
    placeHashValue %= HASHSIZE;

    for (Node::Arcs_t::size_type k = 0; k < getPlace(message)->getLeavingArcsCount(); k++) {
        ((owfnTransition *) getPlace(message)->getLeavingArc(k)->Destination)->check_enabled(this);
    }
    return;
}


//! \brief adds input messages to the current marking
//! \param messages multiset of messages to be added to the currentmarking
void oWFN::addInputMessage(messageMultiSet messages) {
    for (messageMultiSet::iterator iter = messages.begin(); iter
            != messages.end(); iter++) {
        if (getPlace(*iter)->type == INPUT) {
            // found that place
            CurrentMarking[*iter]++;
            placeHashValue += getPlace(*iter)->hash_factor;
            placeHashValue %= HASHSIZE;

            for (Node::Arcs_t::size_type k = 0; k < getPlace(*iter)->getLeavingArcsCount(); k++) {
                ((owfnTransition *) getPlace(*iter)->getLeavingArc(k)->Destination)->check_enabled(this);
            }
        }
    }
}


//! \brief calls the printMarking() function with the current marking
void oWFN::printCurrentMarking() const {
    printMarking(CurrentMarking);
}


// \brief prints a given marking to cout
//! \param marking marking to be printed
void oWFN::printMarking(unsigned int * marking) const {
    cout << "---------------------------------------------------------------"
         << endl;
    cout << "\t("<< marking << ")"<< "\t["<< getMarkingAsString(marking) << "]"
         << endl
         << "---------------------------------------------------------------"
         << endl;
}


//! \brief Returns the label of the given marking, that means the label
//!        consists of the names of the places of the net that have tokens
//!        (is a multiset => occurance of name == number of tokens used in
//!        dotFile creation (CommunicationGraph::printGraphToDot).
//! \param marking The marking to be printed out.
//! \return the string to be printed
string oWFN::getMarkingAsString(unsigned int * marking) const {
    bool comma = false;
    string buffer = "";
    for (unsigned int i = 0; i < getPlaceCount(); i++) {
        if (marking[i] > 5) {
            if (comma) {
                buffer += ", ";
            }
            buffer += getPlace(i)->name;
            buffer += ":";
            buffer += intToString(marking[i]);
            comma = true;
        } else {
            for (unsigned int k = 0; k < marking[i]; k++) {
                if (comma) {
                    buffer += ", ";
                }
                buffer += getPlace(i)->name;
                comma = true;
            }
        }
    }
    return buffer;
}

//! \brief Returns the label of the CurrentMarking. See
//!        getMarkingAsString() for more information.
//! \return the string to be printed
string oWFN::getCurrentMarkingAsString() const {
    return getMarkingAsString(CurrentMarking);
}


//! \brief DESCRIPTION
//! \param b DESCRIPTION
void oWFN::print_binDec(int h) {

    for (unsigned int i=0; i < getPlaceCount(); i++) {
        cout << getPlace(i) -> name<< ": "<< getPlace(i) -> nrbits << "; ";
    }
    cout << endl;

    print_binDec(binHashTable[h], 0);
}


//! \brief DESCRIPTION
//! \param b DESCRIPTION
//! \param indent DESCRIPTION
void oWFN::print_binDec(binDecision * d, int indent) {
    int i;
    // print bin decision table at hash entry h

    for (i=0; i<indent; i++)
        cout << ".";

    if (!d) {
        cout << " nil "<< endl;
        return;
    }

    cout << "b "<< d -> bitnr<< " v ";
    for (i=0; i< (BitVectorSize - (d -> bitnr + 1)); i+=8) {
        cout << (unsigned int) (d -> vector)[i/8] << " ";
    }

    for (i=0; i<BitVectorSize - (d -> bitnr+1); i++) {
        cout << (((d->vector)[i/8] >> (7-i%8))%2);
    }
    cout << endl;
    print_binDec(d -> nextold, indent+1);
    print_binDec(d -> nextnew, indent+1);
}


//! \brief returns the place hash value
//! \return place hash value
unsigned int oWFN::getPlaceHashValue() {
    return placeHashValue;
}


//! \brief adds a given transition to the owfn
//! \param transition transition to be added
//! \return returns true, if the transition was successfully added, else false
bool oWFN::addTransition(owfnTransition* transition) {
    for (Transitions_t::size_type i = 0; i != Transitions.size(); ++i) {
        if (getTransition(i)->name == transition->name)
            return false;
    }

    Transitions.push_back(transition);
    if (!(getTransitionCount() % REPORTFREQUENCY)) {
        cerr << "\n"<< getTransitionCount() << "transitions parsed\n";
    }

    return true;
}


//! \brief adds a given place to the owfn
//! \param place place to be added
void oWFN::addPlace(owfnPlace *place) {
    if (place->type == INPUT) {
        inputPlaceIndices[place] = inputPlaces.size();
        inputPlaces.push_back(place);
    } else if (place->type == OUTPUT) {
        outputPlaceIndices[place] = outputPlaces.size();
        outputPlaces.push_back(place);
    }
    PlaceIndices[place] = Places.size();
    Places.push_back(place);

    if (!(Places.size() % REPORTFREQUENCY)) {
        cerr << "\n"<< Places.size() << "places parsed\n";
    }
}


//! \brief removes an output message from the current marking,
//!        if an output place was found, the marking on that place is
//!        decreased by one and the function returns true
//! \param message name of message to be deleted from currentmarking
bool oWFN::removeOutputMessage(unsigned int message) {

    if (CurrentMarking[message] > 0) {
        CurrentMarking[message]--;
        placeHashValue -= getPlace(message)->hash_factor;
        placeHashValue %= HASHSIZE;

        for (Node::Arcs_t::size_type k = 0; k < getPlace(message)->getLeavingArcsCount(); k++) {
            ((owfnTransition *) getPlace(message)->getLeavingArc(k)->Destination)->check_enabled(this);
        }
        return true;
    }
    return false;
}


//! \brief removes output messages from the current marking,
//!        if an output place was found, the marking on that place is
//!        decreased by one and the function returns 1
//! \param messages multiset of messages to be deleted from currentmarking
bool oWFN::removeOutputMessage(messageMultiSet messages) {
    unsigned int found = 0;

    for (messageMultiSet::iterator iter = messages.begin(); iter
            != messages.end(); iter++) {

        if (getPlace(*iter)->type == OUTPUT) {
            if (CurrentMarking[*iter] > 0) {
                found++;
                // found that place
                CurrentMarking[*iter]--;
                placeHashValue -= getPlace(*iter)->hash_factor;
                placeHashValue %= HASHSIZE;

                for (Node::Arcs_t::size_type k = 0; k < getPlace(*iter)->getLeavingArcsCount(); k++) {
                    ((owfnTransition *) getPlace(*iter)->getLeavingArc(k)->Destination)->check_enabled(this);
                }
            }
        }
    }
    return messages.size() == found;
}


//! \brief returns type of state: {TRANS, DEADLOCK, FINALSTATE, NN}
//! \return state type
stateType oWFN::typeOfState() {
    trace(TRACE_5, "oWFN::typeOfState() : start\n");
    if (isFinal()) { // state is final
        return FINALSTATE;
    }

    if (enabledTransitions.isEmpty()) {
        trace(TRACE_5, "oWFN::typeOfState() : end\n");
        return DEADLOCK; // state is an internal deadlock, no transition is enabled or quasi-enabled
    }

    // otherwise, state must be transient
    trace(TRACE_5, "oWFN::typeOfState() : end\n");
    return TRANS;
}


//! \brief checks if the current marking satisfies final condition or final marking, resp.
//!        if a final condition is given, a possible final marking is ignored
//! \return if the owfn is currently in a final state
bool oWFN::isFinal() const {
    trace(TRACE_5, "bool oWFN::isFinal() : start\n");
    if (FinalCondition) {
        for (unsigned int currentplacenr = 0; currentplacenr < getPlaceCount(); currentplacenr++) {
            for (unsigned int j=0; j < PN->getPlace(currentplacenr)->cardprop; j++) {
                if (PN->getPlace(currentplacenr)->proposition != NULL) {
                    PN->getPlace(currentplacenr)->proposition[j] -> update(PN->CurrentMarking[currentplacenr]);
                }
            }
        }
        trace(TRACE_5, "bool oWFN::isFinal() : end\n");
        return FinalCondition -> value;
    } else {
        for (list<unsigned int *>::const_iterator FinalMarking = FinalMarkingList.begin();
             FinalMarking != FinalMarkingList.end(); FinalMarking++) {
            bool finalMarkingFits = true;
            for (unsigned int i = 0; i < getPlaceCount(); i++) {
                if (CurrentMarking[i] != (*FinalMarking)[i]) {
                    finalMarkingFits = false;
                }
            }
            if (finalMarkingFits) {

                trace(TRACE_5, "bool oWFN::isFinal() : end\n");
                return true;
            }
        }
        trace(TRACE_5, "bool oWFN::isFinal() : end\n");
        return false;
    }
}

//! \brief creates a label from a message multi set
//! \param m messageMultiSet
//! \return the created label
string oWFN::createLabel(messageMultiSet m) const {
    string label;
    bool comma = false;

    for (messageMultiSet::iterator iter1 = m.begin(); iter1 != m.end(); iter1++) {
        if (comma) {
            label += ", ";
        }
        
        label += getPlace(*iter1)->getLabelForCommGraph();
         
        comma = true;
    }

    return label;
}


//! \brief Matches this oWFN with the given operating guideline (OG).
//! \param og Operating guideline against this oWFN should be matched.
//! \param reasonForFailedMatch In case of a failed match, holds a text
//!        describing why the matching failed.
//! \return true If this oWFN matches with given OG.
//!         retval false Otherwise.
bool oWFN::matchesWithOG(const AnnotatedGraph* og, string& reasonForFailedMatch) {
    // Check whether the initial marking violates the message bound and exit
    // with an error message if it does.
    if (violatesMessageBound()) {
        reasonForFailedMatch = "Current marking: '"
                               + getCurrentMarkingAsString()
                               + "' violated message bound.";
        return false;
    }

    // Initialize the currentState with the initial marking.
    State* currentState = binInsert(this);
    currentState->firelist = firelist();
    currentState->cardFireList = CurrentCardFireList;
    currentState->current = 0;
    currentState->parent = NULL;
    currentState->succ = new State*[CurrentCardFireList];
    for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
        currentState->succ[istate] = NULL;
    }
    currentState->placeHashValue = placeHashValue;
    currentState->type = typeOfState();

    // remember that we checked the currentState and the root node of the graph
    StateNodesAssoc_t stateNodesAssoc;
    
    // Initialize the currentOGNode with the root node of the OG.
    AnnotatedGraphNode* currentOGNode = og->getRoot();
    stateNodesAssoc[currentState].insert(currentOGNode->getNumber());
    
    if (currentOGNode->isRed()) {
        reasonForFailedMatch = "The OG is empty (its root node is red).";
        return false;
    }

    // We return true if (and only if) the recusive method succeeded.
    return matchesWithOGRecursive(currentOGNode, currentState,
                                  reasonForFailedMatch, stateNodesAssoc);
}


//! \brief helper for matchesWithOG
//! \param currentOGNode current node in the OG, so we know where we are
//! \param currentState current oWFN state
//! \param reasonForFailedMatch In case of a failed match, holds a text
//!        describing why the matching failed.
//! \return true If this oWFN matches with given OG.
//!         false Otherwise.
bool oWFN::matchesWithOGRecursive(AnnotatedGraphNode* currentOGNode,
                                  State* currentState,
                                  string& reasonForFailedMatch,
                                  StateNodesAssoc_t& stateNodesAssoc) {

    // A temporary copy of the CurrentMarking. Used to revert to the
    // CurrentMarking if firing of a transition leads to an already seen
    // marking.
    unsigned int* tmpCurrentMarking = NULL;
    unsigned int tmpPlaceHashValue;

    // If currentState becomes NULL we don't need to look any further.
    if (currentState == NULL) {
        reasonForFailedMatch = "";
        return true;
    }

    // Check whether there are still enabled transitions to be fired in
    // currentState (transitions that have not been processed yet while
    // building the reachability graph). currentState->current holds the
    // index of the last fired transition in the array of to be fired
    // transitions currentState->firelist[].
    while (currentState->current < currentState->cardFireList) {
        // There is a not yet fired transition in currentState.

        // Retrieve the transition leaving the current state that should be
        // fired next.
        owfnTransition* transition = currentState->firelist[currentState->current];

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
            !currentOGNode->hasBlueEdgeWithLabel(transition->getLabelForMatching())) {

            reasonForFailedMatch = "A transition labeled with \""
                                   + transition->getLabelForMatching()
                                   + "\" leaves the marking ["
                                   + getCurrentMarkingAsString()
                                   + "] in the oWFN, but no blue edge with the same label"
                                   + " leaves the node \"" + currentOGNode->getName()
                                   + "\" in the OG.";

            delete[] tmpCurrentMarking;
            return false;
        }

        // Fire the to be tried transition. The thereby reached marking is
        // saved to CurrentMarking.
        transition->fire(this);

        // Save the currentOGNode to a temporary copy, so we can easily
        // revert to it if the state we reached to firing the current
        // transition lead us to an already seen state.
        AnnotatedGraphNode* oldOGNode = currentOGNode;

        // if net makes a visible step, the OG node does so, too
        if (transition->hasNonTauLabelForMatching()) {
            // Fire the transition in the OG that belongs to the transition we
            // just fired in the oWFN.
            currentOGNode = currentOGNode->followEdgeWithLabel(transition->getLabelForMatching());
        } else {
            // if net just made a silent step, the OG node stays unchanged
            // so do nothing.
        }

        // Determine whether we have already seen the state we just
        // reached.
        State* newState = binSearch(this);
        
        // let's see if we have not yet checked the current state 
        // and if we have not yet checked the current state and the current node already
        if (newState != NULL && 
        		(stateNodesAssoc[newState].find(currentOGNode->getNumber()) != stateNodesAssoc[newState].end())) {

        	trace(TRACE_2, "marking [" + getCurrentMarkingAsString() + "] already seen with node ");
        	trace(TRACE_2, currentOGNode->getName() + "\n");
        	trace(TRACE_2, "  backtracking from node " + currentOGNode->getName());
        	trace(TRACE_2, " with annotation " + currentOGNode->getAnnotationAsString() + "\n");
            
            // We have already seen the state we just reaching by firing
            // the transition above. So we have to revert to the state that
            // the transition left from.
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

            if (transition->hasNonTauLabelForMatching()) {
                // go back to the previously considered node
                currentOGNode = oldOGNode;
            } else {
                // if net makes a silent (back) step, the OG node stays unchanged
                // so do nothing.
            }
        } else {
            // The state we reached by firing the above transition is new.
            // So we have to initialize this newly seen state.

        	trace(TRACE_1, "found new marking [" + getCurrentMarkingAsString() + "] at node ");
        	trace(TRACE_1, currentOGNode->getName() + "\n");
        	
            newState = binInsert(this);
            newState->firelist = firelist();
            newState->cardFireList = CurrentCardFireList;
            newState->current = 0;
            newState->parent = currentState;
            newState->succ = new State*[CurrentCardFireList];
            for (size_t istate = 0; istate != CurrentCardFireList; ++istate) {
                newState->succ[istate] = NULL;
            }
            newState->placeHashValue = placeHashValue;
            newState->type = typeOfState();
            currentState->succ[currentState->current] = newState;

            // Clean up the temporary copy of the former CurrentMarking
            // because we do not longer need to be able to revert to it as
            // we have a found a _new_ not yet seen state.
            if (tmpCurrentMarking != NULL) {
                delete[] tmpCurrentMarking;
                tmpCurrentMarking = NULL;
            }

            // remember that we have checked the current state with a certain (current) node
            stateNodesAssoc[newState].insert(currentOGNode->getNumber());
            
            // Check whether the initial marking violates the message bound
            // and exit with an error message if it does.
            if (violatesMessageBound()) {
                reasonForFailedMatch = "Current marking: ["
                                       + getCurrentMarkingAsString()
                                       + "] violated message bound.";
                return false;
            }

            // Continue the matching recursively using newState and
            // currentOGNode.
            // Upon success, don't forget to continue to work with oldOGNode.
            if (!matchesWithOGRecursive(currentOGNode, newState,
                                        reasonForFailedMatch,
                                        stateNodesAssoc)) {
                return false;
            } else {
                currentOGNode = oldOGNode;
            }
        }
    }

    // There are no transitions left to fire. So we are about to
    // backtrack to the parent of the currentState. But before we
    // do this, we have to check whether the currentState satisfies
    // the annotation of the corresponding OG node. So we construct
    // an assignment for this node such that all propositions
    // corresponding to currently enabled transitions are set to
    // true and all others to false; furthermore the literal
    // 'final' is set to true iff the currentState is a final
    // state. Because in a GraphFormulaAssignment every
    // unmentioned literal is considered false, we only set those
    // literals that should be considered true.
    //
    // If the currentState has a leaving tau transition, we can skip
    // checking the annotation because it is satisfied for sure.
    if (!currentState->hasLeavingTauTransitionForMatching()) {
        GraphFormulaAssignment
                assignment = makeAssignmentForOGMatchingForState(currentState);

        if (!currentOGNode->assignmentSatisfiesAnnotation(assignment)) {
            // Clean up the temporary copy of the former CurrentMarking
            // just to be sure.
            if (tmpCurrentMarking != NULL) {
                delete[] tmpCurrentMarking;
                tmpCurrentMarking = NULL;
            }

            reasonForFailedMatch = "The marking ["
                                   + getCurrentMarkingAsString()
                                   + "] of the oWFN does not satisfy the annotation \""
                                   + currentOGNode->getAnnotationAsString()
                                   + "\" of the corresponding node \""
                                   + currentOGNode->getName()
                                   + "\" in the OG.";

            return false;
        } else {
        	trace(TRACE_4, "marking [" + getCurrentMarkingAsString() + "] satisfied\n");
        	trace(TRACE_4, "  annotation " + currentOGNode->getAnnotationAsString());
        	trace(TRACE_4, " of node " + currentOGNode->getName() + "\n");
        }
    }

    // Although we won't use currentState after tracking back (it will
    // be lost anyway), we set it to its parent here for convenience.
    currentState = currentState->parent;
    if (currentState != NULL) {
        // Decode currentState into CurrentMarking such that
        // currentState and CurrentMarking again denote the same
        // state of the oWFN.
        currentState->decode(this);
        currentState->current++;
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


//! \brief Creates an assignment for the given state of the oWFN used in the
//!        matching algorithm. In this assignment the labels of transitions
//!        leaving the given state are used as propositions and all transitions
//!        that are enabled are assigned to true. Furthermore the proposition
//!        'final' is assigned to true iff the given state is a final state.
//!        All other propositions are implicetly taken to be false. This
//!        implicit behaviour should be (and is) implemented by the class
//!        GraphFormulaAssignment.
//! \param currentState The state of this oWFN an assignment should be
//!        created for.
//! \return returns The above described assignment for the given state.
GraphFormulaAssignment oWFN::makeAssignmentForOGMatchingForState(const State* currentState) const {
    GraphFormulaAssignment assignment;

    for (unsigned int itransition = 0; itransition
            != currentState->cardFireList; ++itransition) {
        owfnTransition* transition = currentState->firelist[itransition];
        assignment.setToTrue(transition->getLabelForMatching());
    }

    if (isFinal()) {
        assignment.setToTrue(GraphFormulaLiteral::FINAL);
    }

    return assignment;
}


// ****************** Implementation of Stubborn Set Calculation *******************
#ifdef STUBBORN

//! \brief DESCRIPTION		
//! \param net DESCRIPTION
void stubbornclosure(oWFN * net) {
    owfnTransition * current;

    net->NrStubborn = 0;
    for (current = net -> StartOfStubbornList; current;
         current = current -> NextStubborn) {

        if (current->isEnabled()) {
            net -> NrStubborn ++;
        }
        for (unsigned int i = 0; i != current->mustbeincluded.size(); i++) {
            if (!current -> mustbeincluded[i]->instubborn) {
                current -> mustbeincluded[i]->instubborn = true;
                current -> mustbeincluded[i]->NextStubborn
                        = (owfnTransition *) 0;
                net -> EndOfStubbornList -> NextStubborn
                        = current -> mustbeincluded[i];
                net -> EndOfStubbornList = current -> mustbeincluded[i];
            }
        }
    }
}

//! \brief DESCRIPTION		
//! \param net DESCRIPTION
//! \param t DESCRIPTION
void stubborninsert(oWFN * net, owfnTransition* t) {
    if (t -> instubborn) {
        return;
    }
    t -> instubborn = true;
    t -> NextStubborn = (owfnTransition *) 0;
    if (net -> StartOfStubbornList) {
        net -> EndOfStubbornList -> NextStubborn = t;
        net -> EndOfStubbornList = t;
    } else {
        net -> StartOfStubbornList = net -> EndOfStubbornList = t;
    }
}


//! \brief returns transitions to be fired for reduced state space for message successors		
//! \param mess output message that is to be received
owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess) {

    trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): begin\n");

    owfnTransition ** result;
    owfnTransition * t;
    unsigned int i;

    if (mess->PreTransitions.empty()) {
        result = new owfnTransition * [1];
        result[0] = (owfnTransition *) 0;
        trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): end\n");
        return result;
    }
    StartOfStubbornList = (owfnTransition *) 0;
    for (i = 0; i != mess->PreTransitions.size(); i++) {
        stubborninsert(this, mess->PreTransitions[i]);
    }
    stubbornclosure(this);
    result = new owfnTransition * [NrStubborn + 1];
    for (t=StartOfStubbornList, i=0; t; t = t -> NextStubborn) {
        t -> instubborn = false;
        if (t->isEnabled()) {
            result[i++] = t;
        }
    }
    result[i] = (owfnTransition *)0;
    CurrentCardFireList = NrStubborn;
    StartOfStubbornList = (owfnTransition *) 0;

    trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): end\n");
    return result;
}

//! \brief returns transitions to be fired for reduced state space for message successors		
//! \param mess output message that is to be received
owfnTransition ** oWFN::stubbornfirelistmarking(unsigned int * marking) {

    trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmarking(): begin\n");

    bool inserted = false;
    owfnTransition ** result;
    owfnTransition * t;
    unsigned int i;
    
    StartOfStubbornList = (owfnTransition *) 0;
    
    for (unsigned int k = 0; k < getPlaceCount(); k++) {
        if (marking[k] > 0) {
            for (i = 0; i != getPlace(k)->PreTransitions.size(); i++) {
                stubborninsert(this, getPlace(k)->PreTransitions[i]);
                inserted = true;
            }
        }
    }
            
    if (!inserted) {
        result = new owfnTransition * [1];
        result[0] = (owfnTransition *) 0;
        trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmarking(): end\n");
        return result;
    }
    
    stubbornclosure(this);
    result = new owfnTransition * [NrStubborn + 1];
    for (t=StartOfStubbornList, i=0; t; t = t -> NextStubborn) {
        t -> instubborn = false;
        if (t->isEnabled()) {
            result[i++] = t;
        }
    }
    result[i] = (owfnTransition *)0;
    CurrentCardFireList = NrStubborn;
    StartOfStubbornList = (owfnTransition *) 0;

    trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmarking(): end\n");
    return result;
}

//! \brief returns transitions to be fired for reduced state space for messages successors
//! \param messages multiset of output messages that are to be received
owfnTransition ** oWFN::stubbornfirelistmessage(messageMultiSet messages) {

    trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): begin\n");

    owfnTransition ** result;
    owfnTransition * t;
    unsigned int i;

    bool noPreTransitions = true;

    // check if any of the output messages has a pre-transition associated with it
    for (messageMultiSet::iterator iter = messages.begin(); iter
            != messages.end(); iter++) {
        if (!getPlace(*iter)->PreTransitions.empty()) {
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

    for (messageMultiSet::iterator iter = messages.begin(); iter
            != messages.end(); iter++) {
        for (i = 0; i != getPlace(*iter)->PreTransitions.size(); i++) {
            stubborninsert(this, getPlace(*iter)->PreTransitions[i]);
        }
    }

    stubbornclosure(this);
    result = new owfnTransition * [NrStubborn + 1];
    for (t=StartOfStubbornList, i=0; t; t = t -> NextStubborn) {
        t -> instubborn = false;
        if (t->isEnabled()) {
            result[i++] = t;
        }
    }
    result[i] = (owfnTransition *)0;
    CurrentCardFireList = NrStubborn;
    StartOfStubbornList = (owfnTransition *) 0;

    trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistmessage(owfnPlace * mess): end\n");
    return result;
}

//! \brief returns transitions to be fired for reduced state space for messages successors
owfnTransition ** oWFN::stubbornFirelistAllMessages() {

    trace(TRACE_5, "owfnTransition ** oWFN::stubbornFirelistAllMessages(): begin\n");

    owfnTransition ** result;
    owfnTransition * t;
    unsigned int i;

    bool noPreTransitions = true;

    // check if any of the output messages has a pre-transition associated with it
    for (unsigned int e = 0; e < PN->getOutputPlaceCount(); e++) {
        if (!getOutputPlace(e)->PreTransitions.empty()) {
            noPreTransitions = false;
            break;
        }
    }

    // none of the transitions that correspond to the output messages has a pre-transition
    if (noPreTransitions) {
        result = new owfnTransition * [1];
        result[0] = (owfnTransition *) 0;
        trace(TRACE_5, "owfnTransition ** oWFN::stubbornFirelistAllMessages(): end\n");
        return result;
    }

    StartOfStubbornList = (owfnTransition *) 0;

    for (Places_t::iterator iter = outputPlaces.begin(); iter
            != outputPlaces.end(); iter++) {
        for (i = 0; i != (*iter)->PreTransitions.size(); i++) {
            stubborninsert(this, (*iter)->PreTransitions[i]);
        }
    }

    stubbornclosure(this);
    result = new owfnTransition * [NrStubborn + 1];
    for (t=StartOfStubbornList, i=0; t; t = t -> NextStubborn) {
        t -> instubborn = false;
        if (t->isEnabled()) {
            result[i++] = t;
        }
    }
    result[i] = (owfnTransition *)0;
    CurrentCardFireList = NrStubborn;
    StartOfStubbornList = (owfnTransition *) 0;

    trace(TRACE_5, "owfnTransition ** oWFN::stubbornFirelistAllMessages(): end\n");
    return result;
}

unsigned int StubbStamp = 0;

//! \brief DESCRIPTION		
//! \param PN DESCRIPTION
void NewStubbStamp(oWFN * PN) {
    if (StubbStamp < UINT_MAX) {
        StubbStamp ++;
    } else {
        for (oWFN::Transitions_t::size_type i = 0; i < PN->getTransitionCount(); ++i) {
            PN->getTransition(i)-> stamp =0;
        }
        StubbStamp = 1;
    }
}




//! \brief DESCRIPTION		
//! \return DESCRIPTION
owfnTransition ** oWFN::stubbornfirelistdeadlocks() {
    trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistdeadlocks(): start\n");

    owfnTransition ** result;
    unsigned int maxdfs;
    owfnTransition * current, * next;

    // computes stubborn set without goal orientation.
    // The TSCC based optimisation is included

    // 1. start with enabled transition
    if (!enabledTransitions.isEmpty()) {
        TarjanStack = enabledTransitions.get(0);
        maxdfs = 0;
        NewStubbStamp(this);
        TarjanStack->nextontarjanstack = TarjanStack;
        TarjanStack->stamp = StubbStamp;
        TarjanStack->dfs = TarjanStack->min = maxdfs++;
        TarjanStack->mbiindex = 0;
        current = TarjanStack;
        CallStack = current;
        current->nextoncallstack = (owfnTransition *) 0;
    } else {
        TarjanStack = NULL;
        result = new owfnTransition * [1];
        result[0] = (owfnTransition *) 0;
        CurrentCardFireList = 0;

        trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistdeadlocks(): end\n");
        return result;
    }

    while (current) {
        //  cout << "current: " << current->name << endl;
        //  cout << "current->mustbeincluded: " << current->mustbeincluded << endl;
        //  cout << "current->mustbeincluded[0]: " << current->mustbeincluded[0] << endl;
        if (current->mustbeincluded.size() > current->mbiindex) {
            next = current->mustbeincluded[current->mbiindex];
            // Successor exists
            if (next->stamp == StubbStamp) {
                // already visited
                if (next -> nextontarjanstack) {
                    // next still on stack
                    current -> min = MINIMUM(current->min, next->dfs);
                }
                current -> mbiindex++;
            } else {
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
        } else {
            // no more successors -> scc detection and backtracking
            if (current->dfs == current->min) {
                // remove all states behind current from Tarjanstack;
                // if enabled -> final sequence
                while (true) {
                    if (TarjanStack->isEnabled()) {
                        // final sequence
                        unsigned int cardstubborn;
                        owfnTransition * t;

                        cardstubborn = 0;
                        for (t = TarjanStack;; t = t->nextontarjanstack) {
                            if (t->isEnabled())
                                cardstubborn ++;
                            if (t == current)
                                break;
                        }
                        result = new owfnTransition * [cardstubborn + 1];
                        cardstubborn = 0;
                        for (t = TarjanStack;; t = t->nextontarjanstack) {
                            if (t->isEnabled()) {
                                result[cardstubborn++] = t;
                            }
                            if (t == current) {
                                result[cardstubborn] = (owfnTransition *) 0;
                                CurrentCardFireList = cardstubborn;
                                trace(TRACE_5, "owfnTransition ** oWFN::stubbornfirelistdeadlocks(): end\n");
                                return (result);
                            }
                        }
                    } else {
                        if (TarjanStack == current) {
                            break;
                        }
                        TarjanStack = TarjanStack -> nextontarjanstack;
                    }
                }
            }
            //	cout << "current -> nextoncallstack: " << current -> nextoncallstack << endl;
            // backtracking to previous state
            next = current -> nextoncallstack;
            next -> min = MINIMUM(current->min, next->min);
            next -> mbiindex++;
            current = next;
        }
    }

    assert(false);
    cerr << "You reached an unreachable line in function "
         << "owfnTransition ** oWFN::stubbornfirelistdeadlocks())"<< endl;
    exit(1);
}
#endif


//! \brief adds a place to a port
//! \param place place to be added
//! \param port port to be added to
void oWFN::add_place_to_port(owfnPlace *place, std::string port) {
    assert(place != NULL);

    trace(TRACE_3, "adding place `" + place->name+ "' to port `"+ port + "'\n");
    ports[port].push_back(place);
    place->set_port(port);
}




/*!
 * \brief  returns the port of a given interface place
 *
 * \param  place  a place
 * \return the name of the place's port of "" if no port was found (e.g., if
 *         the place is internal or the oWFN has no ports)
 *
 * \todo   The handling of ports should be implemented more efficiently, for
 *         example using also a mapping from places to port names This would
 *         make this function unnecessary.
 *
 * \author Niels Lohmann <niels.lohmann@uni-rostock.de>
 */
std::string oWFN::get_port_from_place(const owfnPlace *place) const {
    assert(place != NULL);
    
    // iterate the ports
    for ( std::map<std::string, Places_t>::const_iterator port = ports.begin();
         port != ports.end(); port++) {

        // look if the given place is of this port
        Places_t::const_iterator p_it = find(port->second.begin(), port->second.end(), place);
        
        // if place is found in the port, return the port's name
        if (p_it != port->second.end())
            return port->first;
    }
    
    // port not found
    return "";
}




//! \brief returns the number of ports
//! \return number of ports
unsigned int oWFN::getPortCount() const {
    return ports.size();
}


//! \brief returns this net as an PNapi net
//! \return PNapi version of the owfn
PNapi::PetriNet* oWFN::returnPNapiNet() {

    PNapi::PetriNet* PN = new PNapi::PetriNet();

    // translate all input places
    for (Places_t::const_iterator place = inputPlaces.begin(); place
            != inputPlaces.end(); place++) {
        PNapi::Place* p = PN->newPlace((*place)->name, PNapi::IN, (*place)->capacity, get_port_from_place(*place));
        // Warning: tokens of a place in PNapi is private. The number of tokens is not translated correctly!
        if ((*place)->initial_marking >= 1) {
            p->mark((*place)->initial_marking);
        }
    }

    // translate all output places
    for (Places_t::const_iterator place = outputPlaces.begin(); place
            != outputPlaces.end(); place++) {
        PNapi::Place* p = PN->newPlace((*place)->name, PNapi::OUT, (*place)->capacity, get_port_from_place(*place));
        // Warning: tokens of a place in PNapi is private. The number of tokens is not translated correctly!
        if ((*place)->initial_marking >= 1) {
            p->mark((*place)->initial_marking);
        }
    }

    // translate all places which are neither input nor output places
    for (Places_t::const_iterator place = Places.begin(); place != Places.end(); place++) {
        if (PN->findPlace((*place)->name) == NULL) {
            PNapi::Place* p = PN->newPlace((*place)->name);
            // Warning: tokens of a place in PNapi is private. The number of tokens is not translated correctly!
            if ((*place)->initial_marking >= 1) {
                p->mark((*place)->initial_marking);
            }
        }
    }

    // translate list of final markings
    for (list<unsigned int*>::iterator FinalMarking = FinalMarkingList.begin(); FinalMarking
                    != FinalMarkingList.end(); FinalMarking++) {

        std::set< pair< PNapi::Place *, unsigned int> > apiFinalMarking;

        for (unsigned int i = 0; i < getPlaceCount(); i++) {

            if ((*FinalMarking)[i] >= 1) {
                PNapi::Place * p = PN->findPlace(getPlace(i)->name);
                p->isFinal = true;
                apiFinalMarking.insert(pair<PNapi::Place *, unsigned int>(p,
                                (*FinalMarking)[i]));
            }
        }
        PN->final_set_list.push_back(apiFinalMarking);
    }

    // translate all transitions and generate all arcs
    for (Transitions_t::const_iterator transition = Transitions.begin(); transition
            != Transitions.end(); transition++) {
        PNapi::Transition* t = PN->newTransition((*transition)->name);
        Arc::Arc* arc;

        // translate all leaving arcs
        for (Node::Arcs_t::size_type i = 0; i < (*transition)->Node::getLeavingArcsCount(); ++i) {
            arc = (*transition)->Node::getLeavingArc(i);
            PN->newArc(t, PN->findPlace(arc->Destination->name),
                       PNapi::STANDARD, arc->Multiplicity);
        }

        // translate all arriving arcs
        for (Node::Arcs_t::size_type i = 0; i < (*transition)->Node::getArrivingArcsCount(); ++i) {
            arc = (*transition)->Node::getArrivingArc(i);
            PN->newArc(PN->findPlace(arc->Source->name), t, PNapi::STANDARD,
                       arc->Multiplicity);
        }

    }

    // translate the final condition into final markings of the PNapi-net
    // This needs all operators to be either AND, OR or EQ and all compare
    // values to be 1 due to the structure of the finalmarkings in the PNapi
    // so far this is a TODO

    return PN;
}


//! \brief returns this net as an PNapi net
//! \return PNapi version of the owfn
oWFN* oWFN::returnNormalOWFN() {

    trace(TRACE_5, "oWFN* oWFN::returnNormalOWFN() : start\n");
    const std::string suffix = "_normalized";

    // create new oWFN
    oWFN* result = new oWFN();


    // set filename
    result->filename = filename;


    // create all places from this oWFN as internal places in normal oWFN
    for (Places_t::const_iterator place = Places.begin(); place != Places.end(); place++) {

        // WARNING: NAME CREATION FOR PLACES - HANDLE WITH CARE
        std::string name = (*place)->getName() + (((*place)->getType() != INTERNAL) ? suffix : "");

        trace(TRACE_5, "handle place: " + (*place)->getName() + ", create place: " + name + "\n");
        owfnPlace* newPlace = new owfnPlace(name, INTERNAL, result);
        newPlace->initial_marking = (*place)->initial_marking;
        result->addPlace(newPlace);
    }


    // create all transitions from this oWFN as transitions in normal oWFN and create all arcs
    for (Transitions_t::const_iterator transition = Transitions.begin(); transition != Transitions.end(); transition++) {
        trace(TRACE_5, "handle and create transition: " + (*transition)->getName() + "\n");

        owfnTransition* newTransition = new owfnTransition( (*transition)->getName() );
        result->addTransition(newTransition);

        // arriving arcs
        trace(TRACE_5, "    found " + intToString((*transition)->Node::getArrivingArcsCount()) + " arriving arcs\n");
        for (Node::Arcs_t::size_type i = 0; i < (*transition)->Node::getArrivingArcsCount(); i++) {

            trace(TRACE_5, "        handle arriving arc " + intToString(i) + "\n");
            Arc::Arc* arc = (*transition)->Node::getArrivingArc(i);

            owfnPlace* source = NULL;
            if (arc->pl->getType() == INTERNAL) {
                source = result->getPlace( arc->pl->getName() );
            } else {
                source = result->getPlace( arc->pl->getName() + suffix );
            }
            assert(source != NULL);

            trace(TRACE_5, "        create arriving arc " + intToString(i) + "\n");
            Arc::Arc* newArc = new Arc(*transition, source, true, arc->Multiplicity);
            newTransition->addArrivingArc(newArc);
            source->addLeavingArc(newArc);
        }

        // leaving arcs
        trace(TRACE_5, "    found " + intToString((*transition)->Node::getLeavingArcsCount()) + " leaving arcs\n");
        for (Node::Arcs_t::size_type i = 0; i < (*transition)->Node::getLeavingArcsCount(); i++) {

            trace(TRACE_5, "        handle leaving arc " + intToString(i) + "\n");
            Arc::Arc* arc = (*transition)->Node::getLeavingArc(i);

            owfnPlace* destination = NULL;
            if (arc->pl->getType() == INTERNAL) {
                destination = result->getPlace( arc->pl->getName() );
            } else {
                destination = result->getPlace( arc->pl->getName() + suffix );
            }
            assert(destination != NULL);

            trace(TRACE_5, "        create leaving arc " + intToString(i) + "\n");
            Arc::Arc* newArc = new Arc(*transition, destination, false, arc->Multiplicity);
            destination->addArrivingArc(newArc);
            newTransition->addLeavingArc(newArc);
        }
    }


    // create new places, transitions and arcs for former input places
    for (Places_t::const_iterator place = inputPlaces.begin(); place != inputPlaces.end(); place++) {
        trace(TRACE_5, "create new inputplace: " + (*place)->getName() + "\n"); 

        // create inputplace
        owfnPlace* newPlace = new owfnPlace( (*place)->getName(), INPUT, result);
        assert(newPlace != NULL);
        result->addPlace(newPlace);

        // create transition
        std::string name = (*place)->getName();
        owfnTransition* newTransition = new owfnTransition("t" + name + suffix);
        assert(newTransition != NULL);
        result->addTransition(newTransition);

        // first arc
        Arc::Arc* inArc = new Arc( newTransition, newPlace, true, 1 );
        assert(inArc != NULL);
        newTransition->addArrivingArc(inArc);
        newPlace->addLeavingArc(inArc);

        // second arc
        owfnPlace* destination = result->getPlace( (*place)->getName() + suffix);
        assert(destination != NULL);
        Arc::Arc* outArc = new Arc( newTransition, destination, false, 1 );
        assert(outArc != NULL);
        destination->addArrivingArc(outArc);
        newTransition->addLeavingArc(outArc);
    }


    // create new places, transitions and arcs for former output places
    for (Places_t::const_iterator place = outputPlaces.begin(); place != outputPlaces.end(); place++) {
        trace(TRACE_5, "create new outputplace: " + (*place)->getName() + "\n"); 

        // create outputplace
        owfnPlace* newPlace = new owfnPlace( (*place)->getName(), OUTPUT, result);
        assert(newPlace != NULL);
        result->addPlace(newPlace);

        // create transition
        std::string name = (*place)->getName();
        owfnTransition* newTransition = new owfnTransition("t" + name + suffix);
        assert(newTransition != NULL);
        result->addTransition(newTransition);

        // first arc
        owfnPlace* destination = result->getPlace( (*place)->getName() + suffix);
        assert(destination != NULL);
        Arc::Arc* inArc = new Arc( newTransition, destination, true, 1 );
        assert(inArc != NULL);
        newTransition->addArrivingArc(inArc);
        destination->addLeavingArc(inArc);

        // second arc
        Arc::Arc* outArc = new Arc( newTransition, newPlace, false, 1 );
        assert(outArc != NULL);
        newPlace->addArrivingArc(outArc);
        newTransition->addLeavingArc(outArc);
    }


    // copy current marking
    result->CurrentMarking = copyGivenMarking(CurrentMarking);


    // create new list of final markings
    for (list<unsigned int*>::iterator marking = FinalMarkingList.begin(); marking != FinalMarkingList.end(); marking++) {
        trace(TRACE_5, "create new final marking\n");


        // create new final marking for new count of places and initialize with 0
        unsigned int * copy = new unsigned int [result->getPlaceCount()];
        for (unsigned int i = 0; i < result->getPlaceCount(); i++) {
            copy[i] = 0;
        }

        // copy former final marking information
        for (unsigned int i = 0; i < getPlaceCount(); i++) {
            owfnPlace* oldPlace = getPlace(i);
            std::string newName = oldPlace->getName() + ((oldPlace->getType() != INTERNAL) ? suffix : "");
            owfnPlace* newPlace = result->getPlace( newName );

            copy[ result->getPlaceIndex(newPlace) ] = (*marking)[ i ];
        }

        result->FinalMarkingList.push_back( copy );
    }


    trace(TRACE_5, "oWFN* oWFN::returnNormalOWFN() : end\n");
    return result;
}


//! \brief returns the port of the interface place identified by a label
//! \param label label to be found at a port
//! \return a port with the corresponding label
//! \note  This function is implemented just for the functionality and not for
//!        any performance issues. As long as I don't know which functions I
//!        need, I will program with such stubs.
std::string oWFN::getPortForLabel(std::string label) const {
    for (map<std::string, Places_t>::const_iterator port = ports.begin(); port
            != ports.end(); port++) {
        for (unsigned int i = 0; i < port->second.size(); i++) {
            if (port->second[i]->name == label.substr(1, label.length())) {
                return port->first;
            }
        }
    }

    // no port found
    return "";
}


//! \brief returns the port identified by its name
//! \param label name of the port
//! \return port with the given name
//! \note  This function is implemented just for the functionality and not for
//!        any performance issues. As long as I don't know which functions I
//!        need, I will program with such stubs.
std::set<std::string> oWFN::getPort(std::string name) const {
    Places_t places = ports.find(name)->second;
    set<string> temp;

    // collect the labels of the places of the given port
    for (Places_t::const_iterator place = places.begin(); place != places.end(); place++) {
        temp.insert((*place)->getLabelForCommGraph() );
    }

    return temp;
}


/*!
 * \brief  detaches an interface place from the oWFN
 *
 * This function detaches and interface place from the oWFN by removing all
 * arcs that connect the interface place with the inner of the net.
 *
 * \param  p  the place to detach
 *
 * \todo Remove debug output or change to appropriate trace function.
 *
 * \author Niels Lohmann <niels.lohmann@uni-rostock.de>
 */
void oWFN::detachInterfacePlace(owfnPlace *p) {
    assert(p != NULL);
    assert(p->type != INTERNAL);
    
    owfnTransition *t = NULL;
    
    // find the connected transitions and remove the connecting arcs
    if (p->type == INPUT) {        
        for (unsigned i = 0; i < p->getLeavingArcsCount(); i++) {
            Arc *a = p->getLeavingArc(i);
            assert(a != NULL);

            t = static_cast<owfnTransition*>(a->Destination);
            assert(t != NULL);            
            t->removeArrivingArc(a);            
            p->removeLeavingArc(a);
            
            // remember that this transition was connected to an input place of
            // a port that has been detached
            t->isConnectedToOtherPort = true;
            cerr << t->name << " is connected to the other port " << endl;
            
            delete a;
        }
    }

    if (p->type == OUTPUT) {        
        for (unsigned i = 0; i < p->getArrivingArcsCount(); i++) {
            Arc *a = p->getArrivingArc(i);
            assert(a != NULL);
            
            t = static_cast<owfnTransition*>(a->Source);
            assert(t != NULL);
            t->removeLeavingArc(a);
            p->removeArrivingArc(a);

            delete a;
        }
    }
}


/*!
 * \brief  restricts the oWFN to a given port (i.e., removes all other ports)
 *
 * The function does not delete the interface, but only removes all arcs from
 * or to the interface places of the ports that are not mentioned. The function
 * is needed to check for autonomous controllability.
 *
 * \param  portName  the name of a port
 *
 * \warning If an invalid port name is given (e.g., the empty string or a port
 *          name that is not present in the net), the whole interface is cut
 *          from the net.
 *
 * \todo Remove debug output or change to appropriate trace function.
 *
 * \author Niels Lohmann <niels.lohmann@uni-rostock.de>
 */
void oWFN::restrictToPort(const std::string &portName) {
    // iterate the ports
    for ( std::map<std::string, Places_t>::const_iterator port = ports.begin();
         port != ports.end(); port++) {
        
        // if the current port is not the given port, change interface places
        // to internal places and remove the leaving/arrving arcs to the
        // transitions
        if (port->first != portName) {
            cerr << "detaching port " << port->first << endl;
            for ( unsigned int i = 0; i < port->second.size(); i++ ) {
                assert(port->second[i]->type != INTERNAL);
                detachInterfacePlace( port->second[i] );
            }
        }

        // if the current port is the port to that the net should be restricted,
        // traverse all input places of the port and mark the receiving
        // transitions
        if (port->first == portName) {
            for ( unsigned int i = 0; i < port->second.size(); i++ ) {
                if (port->second[i]->type == INPUT) {
                    owfnPlace *p = port->second[i];
                    assert(p != NULL);
                
                    for (unsigned int j = 0; j < p->getLeavingArcsCount(); j++) {
                        Arc *a = p->getLeavingArc(j);
                        assert(a != NULL);
                        owfnTransition *t = static_cast<owfnTransition*>(a->Destination);
                        assert(t != NULL);
                        t->isConnectedToMyPort = true;
                        cerr << t->name << " is connected to my port " << portName << endl;
                    }
                }
            }
        }
    }
}
