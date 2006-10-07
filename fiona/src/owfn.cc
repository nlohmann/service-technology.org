#include "mynew.h"
#include "state.h"
#include "binDecision.h"
#include "options.h"
#include "debug.h"
#include "CNF.h"
#include "vertex.h"
#include <cassert>

using namespace std;


//! \fn oWFN::oWFN()
//! \brief constructor
oWFN::oWFN() : arcCnt(0), placeHashValue(0), placeCnt(0),
               placeInputCnt(0), placeOutputCnt(0), transCnt(0),
               transNrEnabled(0), BitVectorSize(0), currentState(0),
               startOfEnabledList(NULL), startOfQuasiEnabledList(NULL),
               FinalCondition(NULL), filename(NULL)
{
	startOfEnabledList = (owfnTransition *) 0;

	unsigned int i;
  	NonEmptyHash = 0;
	commDepth = 0;

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
	delete[] inputPlacesArray;
	delete[] outputPlacesArray;

	for(int i = 0; i < HASHSIZE; i++) {
		if (binHashTable[i]) {
			delete binHashTable[i];
		}
		binHashTable[i] = (binDecision *) 0;
	}	

	delete[] binHashTable;
	
	for(int i = 0; i < placeCnt; i++) {
		if (Places[i]) {
			delete Places[i];
		}
		Places[i] = NULL;
	}	
	delete[] Places;
	
	for(int i = 0; i < transCnt; i++) {
		if (Transitions[i]) {
			delete Transitions[i];
		}
		Transitions[i] = NULL;
	}	
	delete[] Transitions;
	delete[] CurrentMarking;
	delete[] FinalMarking;
	delete FinalCondition;
}

//! \fn unsigned int oWFN::getPlaceCnt()
//! \brief returns the number of all places of the net
unsigned int oWFN::getPlaceCnt() {
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


//! \fn unsigned int oWFN::getTransitionCnt()
//! \brief returns the number of transitions of the net
unsigned int oWFN::getCommDepth() {
	return commDepth;
}


unsigned int oWFN::getCardFireList() {
	assert(false);		// cout << "oWFN::getCardFireList() never used" << endl;
	return CardFireList;
}


//! \fn oWFN::initialize()
//! \brief initializes the owfn; is called right after the parsing of the net file is done
void oWFN::initialize() {
	int i;
	
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
  	
	// initialize transitions
  	for(i = 0; i < transCnt; i++) {
		Transitions[i]->enabled = false;
		Transitions[i]->quasiEnabled = false;
  	}
  	
  	for(i = 0; i < transCnt; i++) {
		Transitions[i]->initialize(this);
  	}
  	
  	for(i=0; i < placeCnt; i++) {
        CurrentMarking[i] = Places[i]->initial_marking;
  	}
 
	for(int i = 0; i < transCnt; i++) {
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
	
	for(i = 0, BitVectorSize = 0; i < placeCnt; i++) {
        Places[i]->startbit = BitVectorSize;
        BitVectorSize += Places[i]->nrbits;
  	}
	
	trace(TRACE_5, "oWFN initialized\n");
	
}



void oWFN::initializeTransitions() {
	unsigned int i;
	 	
  	for(i = 0; i < transCnt; i++) {
		Transitions[i]->PrevEnabled = (i == 0 ? (owfnTransition *) 0 : Transitions[i-1]);
		Transitions[i]->NextEnabled = (i == transCnt - 1 ? (owfnTransition *) 0 : Transitions[i+1]);
 		Transitions[i]->enabled = true;
		Transitions[i]->PrevQuasiEnabled = (i == 0 ? (owfnTransition *) 0 : Transitions[i-1]);
		Transitions[i]->NextQuasiEnabled = (i == transCnt - 1 ? (owfnTransition *) 0 : Transitions[i+1]);
 		Transitions[i]->quasiEnabled = true;
  	}
  	startOfEnabledList = Transitions[0];
  	transNrEnabled = transCnt;

  	startOfQuasiEnabledList = Transitions[0];
  	transNrQuasiEnabled = transCnt; 
  	
	for(i = 0; i < transCnt; i++) {
		Transitions[i]->check_enabled(this);
  	}
}

void oWFN::removeisolated() {
	unsigned int i,j;

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



//void oWFN::addStateToList(vertex * n, State * NewState, bool minimal) {
//
//	if (parameters[P_CALC_ALL_STATES]
// 		|| NewState->type == DEADLOCK
//		|| minimal
//		|| NewState->type == FINALSTATE) { 
//		
//		n->addState(NewState);
//	}
//	
//	for(int i = 0; i < NewState->CardFireList; i++) {
//		addStateToList(n, NewState->succ[i]);
//	}
//}

void oWFN::addSuccStatesToList(vertex * n, State * NewState) {
	if (NewState != NULL) {
		
		// test current marking if message bound k reached
		vertexColor beforeViolationCheck = n->getColor();
		checkMessageBound(n);
		if (n->getColor() != beforeViolationCheck) {
			cout << "checkMessageBound changed color for node " << n << " (addSuccStatesToList)" << endl;
		}
		
		// add successors
		for(int i = 0; i < NewState->CardFireList; i++) {
			addStateToList(n, NewState->succ[i]);
		}
	}
}


void oWFN::addStateToList(vertex * n, State * currentState) {

	currentState->decodeShowOnly(this);
		
	// test current marking if message bound k reached
	vertexColor beforeViolationCheck = n->getColor();
	checkMessageBound(n);
	if (n->getColor() != beforeViolationCheck) {
		cout << "checkMessageBound changed color for node " << n << " (addStateToList)" << endl;
	}

	n->addState(currentState);
	
	if (currentState != NULL) {
		for(int i = 0; i < currentState->CardFireList; i++) {
			addStateToList(n, currentState->succ[i]);
		}
	}
}


//! \fn void oWFN::checkMessageBound()
//! \return returns true iff current marking VIOLATES message bound
//! \brief checks if message bound is violated by the current marking (for interface places only)
bool oWFN::checkMessageBound() {
	trace(TRACE_5, "oWFN::checkMessageBound(): start\n");
	// test marking of current state if message bound k reached
	if (options[O_MESSAGES_MAX] == true) {      // k-message-bounded set
		// test input places
		for (int i = 0; i < placeInputCnt; i++) {
			if (CurrentMarking[inputPlacesArray[i]->index] > messages_manual) {
				cout << "\t checkMessageBound found violation for input place " << inputPlacesArray[i]->name << endl;
				trace(TRACE_5, "oWFN::checkMessageBound(): end\n");
				return true;
			}
		}
		// test output places
		for (int i = 0; i < placeOutputCnt; i++) {
			if (CurrentMarking[outputPlacesArray[i]->index] > messages_manual) {
				cout << "\t checkMessageBound found violation for output place " << outputPlacesArray[i]->name << endl;
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

	if (options[O_CALC_ALL_STATES]) {
		node->addState(currentState);
	} else {
		// store this state in the node's temp set of state (storing all states of the node)
		setOfStatesTemp.insert(currentState);		
	}
	// get the successor states	and compute their corresponding annotation
	if (currentState != NULL) {
		for(int i = 0; i < currentState->CardFireList; i++) {
			if (currentState->succ[i]) {
				computeAnnotationOutput(node, currentState->succ[i]);
			}
		}
	}
	trace(TRACE_5, "oWFN::computeAnnotation(vertex * node, State * currentState, unsigned int * markingPreviousState): end\n");
}


//! \fn void oWFN::computeAnnotationInput(vertex * node, State * currentState, unsigned int * markingPreviousState, bool isCurrentMarking)
//! \param node the node that is calculated
//! \param currentState the state for which the appropriate clause is to be created if this state is DL or FS
//! \param markingPreviousState the marking of the parent state, needed to check whether this state needs to be added to the node's states
//! \param isCurrentMarking flag, whether this state is the currentMarking or not
//! \brief computes the CNF of the current node starting with the currentState, goes recursively through
//! all of its successor states 
void oWFN::computeAnnotationInput(vertex * node, State * currentState, unsigned int * markingPreviousState, bool isCurrentMarking) {
	trace(TRACE_5, "oWFN::computeAnnotationInput(vertex * node, State * currentState, unsigned int * markingPreviousState): start\n");
	
	unsigned int * marking = NULL;

	if (options[O_CALC_ALL_STATES]) {
		node->addState(currentState);
	} else {
		// store this state in the node's temp set of state (storing all states of the node)
		setOfStatesTemp.insert(currentState);		
	}
	
	// check, whether this state is to be added to the node or not
	// we do this right here, because of the decode function that might have been called already
	// if the currentState is just the currentMarking, then we don't decode again ;-)
	if (!isCurrentMarking && placeOutputCnt > 0) {
		currentState->decodeShowOnly(this);
	}
	
	bool storeState = false;	// flag indicating whether this state shall be stored in the node or not
	
	for (int i = 0; i < placeOutputCnt; i++) {
		// get the activated output events
		if (CurrentMarking[outputPlacesArray[i]->index] > 0 && 
				(markingPreviousState == NULL || markingPreviousState[outputPlacesArray[i]->index] == 0)) {
			storeState = true;
			if (visitedStates.find(currentState) == visitedStates.end()) {
				// no, we have not yet visited this state
				node->addState(currentState);
			}
		}	
	}	

	if (!storeState && markingPreviousState != NULL && !options[O_CALC_ALL_STATES]) {
		node->reachGraphStateSet.erase(currentState);  // remove this state from the node's state list, because it should not be stored
	}

	visitedStates.insert(currentState);	// remember that we have visited this state
	
	// get the successor states	and compute their corresponding annotation
	if (currentState != NULL && currentState->succ[0] != NULL) {
		marking = copyCurrentMarking();	// save the marking of the current state since it is the parent state of its successors
		for(int i = 0; i < currentState->CardFireList; i++) {
			if (currentState->succ[i]) {
				computeAnnotationInput(node, currentState->succ[i], marking, false);
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

	for (int i = 0; i < placeCnt; i++) {
		copy[i] = CurrentMarking[i];
	}	
	return copy;
}


void oWFN::copyMarkingToCurrentMarking(unsigned int * copy) {

	for (int i = 0; i < getPlaceCnt(); i++) {
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


//! \fn void oWFN::calculateReachableStatesOutputEvent(vertex * n, bool minimal) 
//! \param n the node to be calculated in case of an output event
//! \param minimal the current state is minimal in the vertex
//! \brief calculates the reduced set of states of the new vertex in case of an output event
void oWFN::calculateReachableStatesOutputEvent(vertex * n, bool minimal) {
	// calculates the EG starting at the current marking
	trace(TRACE_5, "oWFN::calculateReachableStatesOutputEvent(vertex * n, bool minimal): start\n");

	State * CurrentState;
  	unsigned int i;
  	State * NewState;
  	stateType type;
  	
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
	ASSERT(CurrentState->succ == NULL);
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
									
				ASSERT(CurrentState->succ[CurrentState->current] == NULL);
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

				ASSERT(NewState->succ == NULL);
				NewState->succ =  new State * [CardFireList+1];
				for (size_t istate = 0; istate != CardFireList+1; ++istate)
				{
					NewState->succ[istate] = NULL;
				}

	      		NewState->placeHashValue = placeHashValue;
	      		NewState->type = typeOfState();
	      		
				ASSERT(CurrentState->succ[CurrentState -> current] == NULL);
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
  	unsigned int i;
  	State * NewState;
  	stateType type;
  	
	CurrentState = binSearch(this);
	
	unsigned int * tempCurrentMarking = NULL;
	unsigned int tempPlaceHashValue;
	
	if (CurrentState != NULL) {
		// marking already has a state -> put it (and all its successors) into the node
		if (n->addState(CurrentState)) {
			computeAnnotationInput(n, CurrentState, NULL, true);
		}
		trace(TRACE_5, "oWFN::calculateReachableStatesInputEvent(vertex * n, bool minimal): end\n");
		return;
	}
	
	// the other case:
	// we have a marking which has not yet a state object assigned to it
	if (CurrentState == NULL) {
		CurrentState = binInsert(this);
	}
	
//	cout << "new state: " << printCurrentMarkingForDot() << " with " << CardQuasiFireList << " in quasiFirelist" << endl;
	
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
	ASSERT(CurrentState->succ == NULL);
	CurrentState->succ = new State * [CardFireList + 1];
	for (int i = 0; i < CardFireList + 1; i++) {
		CurrentState->succ[i] = NULL;
	}
	CurrentState->placeHashValue = placeHashValue;
	CurrentState->type = typeOfState();
	
	setOfStatesTemp.insert(CurrentState);
	
	n->addState(CurrentState);
	computeAnnotationInput(n, CurrentState, NULL, true);
	  	
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
				
				computeAnnotationInput(n, NewState, tempCurrentMarking, true);
				
				copyMarkingToCurrentMarking(tempCurrentMarking);
				
				CurrentState->firelist[CurrentState->current]->backfire(this);
				
				placeHashValue = tempPlaceHashValue;
				
				delete[] tempCurrentMarking;
				tempCurrentMarking = NULL;
									
				ASSERT(CurrentState->succ[CurrentState->current] == NULL);
		   		CurrentState->succ[CurrentState->current] = NewState;
	     		(CurrentState->current)++;
	    	} else {
				trace(TRACE_5, "Current marking new\n");

//	cout << "new state: " << printCurrentMarkingForDot() << " with " << CardQuasiFireList << " in quasiFirelist" << endl;


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
				ASSERT(NewState->succ == NULL);
	      		NewState->succ =  new State * [CardFireList + 1];
				for (int i = 0; i < CardFireList + 1; i++) {
					NewState->succ[i] = NULL;	
				}
	      		NewState->placeHashValue = placeHashValue;
	      		NewState->type = typeOfState();
	      		
				ASSERT(CurrentState->succ[CurrentState->current] == NULL);
	      		CurrentState->succ[CurrentState->current] = NewState;
	      		CurrentState = NewState;
		      		
				computeAnnotationInput(n, NewState, tempCurrentMarking, true);
				
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

//! \fn void oWFN::calculateReachableStatesFull(vertex * listOfStates, bool minimal)
//! \param listOfStates list containing all reachable states from the current marking (after function is done)
//! \param minimal the current state is minimal in the vertex
//! \brief NO REDUCTION! calculate all reachable states from the current marking
//! and store them in the list being passed as parameter (== vertex of communicationGraph)
// returns true iff a message violation occured -> caller shall delete current node
void oWFN::calculateReachableStatesFull(vertex * n, bool minimal) {

	// calculates the EG starting at the current marking
	trace(TRACE_5, "oWFN::calculateReachableStatesFull(vertex * n, bool minimal) : start\n");

	// test current marking if message bound k reached
	if (checkMessageBound()) {
		n->setColor(RED);
		cout << "message bound violated; color of node " << n << " set to RED (calculateReachableStatesFull)" << endl;
		return;
	}
	
	State * CurrentState;
  	State * NewState;
  	stateType type;
  	
	CurrentState = binSearch(this);
	
	unsigned int * tempCurrentMarking = NULL;
	unsigned int tempPlaceHashValue;
	
//	if (options[O_BDD] == false && CurrentState != NULL) {

	if (CurrentState != NULL) {
		// marking already has a state
		// -> put it (and all its successors) into the node
		if (n->addState(CurrentState)) {
			CurrentState->decodeShowOnly(PN);
			addSuccStatesToList(n, CurrentState);
		}
		trace(TRACE_5, "oWFN::calculateReachableStatesFull(vertex * n, bool minimal) : end (root marking of EG already in bintree; states copied only)\n");
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
		ASSERT(CurrentState->succ == NULL);
	
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
				  		
				// fire and reach next state
				CurrentState->firelist[CurrentState->current]->fire(this);
				minimal = isMinimal();
				NewState = binSearch(this);
				
		  		if(NewState != NULL) {
			  		// Current marking already in bintree 
					trace(TRACE_5, "Current marking already in bintree \n");
					if (n->addState(NewState)) {
						addSuccStatesToList(n, NewState);
					}
					
					copyMarkingToCurrentMarking(tempCurrentMarking);
					
					CurrentState->firelist[CurrentState->current]->backfire(this);
					
					placeHashValue = tempPlaceHashValue;
					
					delete[] tempCurrentMarking;
					tempCurrentMarking = NULL;
										
					ASSERT(CurrentState->succ[CurrentState->current] == NULL);
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
					ASSERT(NewState->succ == NULL);
		      		NewState->succ =  new State * [CardFireList+1];
					for (size_t istate = 0; istate != CardFireList+1; ++istate)
					{
						NewState->succ[istate] = NULL;
					}
		      		NewState->placeHashValue = placeHashValue;
		      		NewState->type = typeOfState();
		      		
					ASSERT(CurrentState->succ[CurrentState -> current] == NULL);
		      		CurrentState->succ[CurrentState -> current] = NewState;
		      		CurrentState = NewState;
			      		
					// test current marking if message bound k reached
					vertexColor beforeViolationCheck = n->getColor();
					checkMessageBound(n);
					if (n->getColor() != beforeViolationCheck) {
						cout << "checkMessageBound changed color for node " << n << " (calculateReachableStatesFull, unten)" << endl;
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
		trace(TRACE_5, "oWFN::calculateReachableStatesFull(vertex * n, bool minimal) : end\n");
		return;
	}
}


//! \fn int oWFN::addInputMessage(unsigned int message)
//! \param message name of message to be added to the currentmarking
//! \brief adds input message to the current marking
int oWFN::addInputMessage(unsigned int message) {

	// found that place
	CurrentMarking[message]++;

	placeHashValue += Places[message]->hash_factor;
	placeHashValue %= HASHSIZE;

	for (int k = 0; k < Places[message]->NrOfLeaving; k++) {
		((owfnTransition *) Places[message]->LeavingArcs[k]->Destination)->check_enabled(this);
	}
	return 0;		// place found
}			


//! \fn int oWFN::addInputMessage(char * messages)
//! \param message name of message to be added to the currentmarking
//! \brief adds input message to the current marking
int oWFN::addInputMessage(messageMultiSet messages) {
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
		if (Places[*iter]->type == INPUT) {
			trace(TRACE_3, string(Places[*iter]->name) + " ");
			
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


// prints the CurrentMarking
void oWFN::printmarking() {
	bool comma = false;
	cout << "---------------------------------------------------------------" << endl;
	cout << "\t(" << CurrentMarking << ")" << "\t[";
	
	for (int i = 0; i < placeCnt; i++) {
		if (CurrentMarking[i] > 0) {
			if (CurrentMarking[i] > 5) {
				if (comma) {
					cout << ", ";
				}
				cout << Places[i]->name;
				cout << ":";
				cout << CurrentMarking[i];
				comma = true;
			} else {
				for (int k = 0; k < CurrentMarking[i]; k++) {
					if (comma) {
						cout << ", ";
					}
					cout << Places[i]->name;
					comma = true;
				}
			}
		}	
	}
	cout << "]\n---------------------------------------------------------------" << endl;
}

// prints a given marking
void oWFN::printmarking(unsigned int * marking) {
	bool comma = false;
	cout << "---------------------------------------------------------------" << endl;
	cout << "\t(" << marking << ")" << "\t[";
	
	for (int i = 0; i < placeCnt; i++) {
		if (marking[i] > 0) {
			if (marking[i] > 5) {
				if (comma) {
					cout << ", ";
				}
				cout << Places[i]->name;
				cout << ":";
				cout << marking[i];
				comma = true;
			} else {
				for (int k = 0; k < marking[i]; k++) {
					if (comma) {
						cout << ", ";
					}
					cout << Places[i]->name;
					comma = true;
				}
			}
		}	
	}
	cout << "]\n---------------------------------------------------------------" << endl;
}


//! \fn char * oWFN::printMarking(unsigned int * myMarking) 
//! \param myMarking the marking to be printed out
//! \brief returns the label of the given marking, that means the label consists of the names of the 
//! places of the net that have tokens (is a multiset => occurance of name == number of tokens
//
// used in dotFile creation (communicationGraph::printGraphToDot)
char * oWFN::printMarking(unsigned int * marking) {
	bool comma = false;

	try {
		char * buffer = new char[256];
		strcpy(buffer, "");
		for (int i = 0; i < placeCnt; i++) {
			if (marking[i] > 5) {
				if (comma) {
					strcat(buffer, ", ");
				}
				strcat(buffer, Places[i]->name);
				strcat(buffer, ":");
				sprintf(buffer, "%s%d", buffer, marking[i]);
				comma = true;
			} else {
				for (int k = 0; k < marking[i]; k++) {
					if (comma) {
						strcat(buffer, ", ");
					}
					strcat(buffer, Places[i]->name);
					comma = true;
				}
			}
		}
		return buffer;
	} catch(bad_alloc) {
		char mess[] = "\n! oWFN::printMarking failed !\n";
		//write(2,mess,sizeof(mess));
		cerr << mess;
		_exit(2);
	}

}

char * oWFN::printCurrentMarkingForDot() {
	bool comma = false;

	try {
		char * buffer = new char[256];
		strcpy(buffer, "");
		for (int i = 0; i < placeCnt; i++) {
			if (CurrentMarking[i] > 5) {
				if (comma) {
					strcat(buffer, ", ");
				}
				strcat(buffer, Places[i]->name);
				strcat(buffer, ":");
				sprintf(buffer, "%s%d", buffer, CurrentMarking[i]);
				comma = true;
			} else {
				for (int k = 0; k < CurrentMarking[i]; k++) {
					if (comma) {
						strcat(buffer, ", ");
					}
					strcat(buffer, Places[i]->name);
					comma = true;
				}
			}
		}
		return buffer;
	} catch(bad_alloc) {
		char mess[] = "\n! oWFN::printMarking failed !\n";
		//write(2,mess,sizeof(mess));
		cerr << mess;
		_exit(2);
	}

}

void oWFN::print_binDec(int h) {

	for(int i=0; i < placeCnt; i++) {
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


void oWFN::addArc(Arc *arc) {
}


void oWFN::addPlace(unsigned int i, owfnPlace * place) {
	placeCnt++;
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


//! \fn int oWFN::removeOutputMessage(unsigned int message)
//! \param messageName name of message to be deleted from currentmarking
//! \brief removes an output message from the current marking, if an output place was found the marking on that
//! place is decreased by one and the function returns 1
int oWFN::removeOutputMessage(unsigned int message) {

	if (CurrentMarking[message] > 0) {
		CurrentMarking[message]--;
		placeHashValue -= Places[message]->hash_factor;
		placeHashValue %= HASHSIZE;
		
		// TODO: check_enabled!!!!!!!! so richtig?!
		for (int k = 0; k < Places[message]->NrOfLeaving; k++) {
			((owfnTransition *) Places[message]->LeavingArcs[k]->Destination)->check_enabled(this);
		}
		return 1;	
	} 
	return 0;
}	

//! \fn int oWFN::removeOutputMessage(messageMultiSet messages)
//! \param message index of message to be deleted from currentmarking
//! \brief removes an output message from the current marking, if an output place was found the marking on that
//! place is decreased by one and the function returns 1
int oWFN::removeOutputMessage(messageMultiSet messages) {
	unsigned int found = 0;
	
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {

		if (Places[*iter]->type == OUTPUT) {
			trace(TRACE_3, string(Places[*iter]->name) + " ");
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
//! \brief returns true, if current state is transient; that means the number of enabled transitions is > 0
stateType oWFN::typeOfState() {
	trace(TRACE_5, "oWFN::typeOfState() : start\n");
	if (isFinal()) {			// state is final
		return FINALSTATE;
	}
	if (transNrEnabled == 0) {
		return DEADLOCK;		// state is an internal deadlock, no transition is enabled or quasi-enabled
	} else if (transNrEnabled != 0) {
		return TRANS;		// state is transient since there are transitions which are enabled
	}
	return NN;
	trace(TRACE_5, "oWFN::typeOfState() : end\n");	
}

//! \fn bool oWFN::isMinimal()
//! \brief returns true, if current state is minimal
bool oWFN::isMinimal() {
	
	int i;
	int k = 0;
	for (i = 0; i < placeCnt; i++) {
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

//! \fn bool oWFN::isFinalMarking(unsigned int * marking) 
//! \param marking marking to be checked
//! \brief checks if the marking given as parameter is the final marking of the net
//bool oWFN::isFinalMarking(unsigned int * marking) {
//	for (int i = 0; i < getPlaceCnt(); i++) {
//		if (Places[i]->type != INTERNAL && marking[i] > 0) {
//			return false;
//		}
////		if (FinalMarking[i] != 0 && marking[i] != FinalMarking[i]) {
//		if (FinalMarking[i] != 0 && marking[i] < FinalMarking[i]) {
//			return false;	
//		}
//	}
//	return true;
//}


bool oWFN::isFinal() {
	trace(TRACE_5, "oWFN::bool oWFN::isFinal() : start\n");
	if(FinalCondition) {
		// update formula for each place
		// TODO: update should always be done after firing (in owfnTransition::fire)
		//                                        decoding (in State::decode and State::decodeShowOnly)
		//                            or copying a marking (oWFN::copyCurrentMarking and oWFN::copyMarkingToCurrentMarking)
		for (int currentplacenr = 0; currentplacenr < getPlaceCnt(); currentplacenr++) {
		    for(int j=0; j < PN->Places[currentplacenr]->cardprop; j++) {
				if (PN->Places[currentplacenr]->proposition != NULL) {
				    PN->Places[currentplacenr]->proposition[j] -> update(PN->CurrentMarking[currentplacenr]);
				}
			}
		}
		return FinalCondition -> value;
	} else {
		for (int i = 0; i < getPlaceCnt(); i++) {
			if (Places[i]->type != INTERNAL && CurrentMarking[i] > 0) {
				return false;
			}
//			if (FinalMarking[i] != 0 && marking[i] != FinalMarking[i]) {
			if (FinalMarking[i] != 0 && CurrentMarking[i] < FinalMarking[i]) {
				return false;	
			}
		}
		return true;
	}
	trace(TRACE_5, "oWFN::bool oWFN::isFinal() : end\n");
}


char * oWFN::createLabel(messageMultiSet m) {
	char * label = new char[256];
	char * actualMessage;
	bool comma = false;
	
	strcpy(label, "");
	
	for (messageMultiSet::iterator iter1 = m.begin(); iter1 != m.end(); iter1++) {
		if (comma) {
			strcat(label, ", ");
		}
		actualMessage = new char[strlen(Places[*iter1]->name)];
		strcpy(actualMessage, Places[*iter1]->name);
		strcat(label, actualMessage);
		comma = true;
	}
	
	return label;
}

