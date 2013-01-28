
#include <Formula/CTL/DFSFormula.h>
#include <Exploration/SearchStack.h>
#include <Net/Transition.h>

#include <InputOutput/Reporter.h>

extern Reporter* rep;

/// finds witness path that fulfills all fairness assumptions if any exists
/// assumes that a SCC was just found that represents a potential (counter-)example, with ns being the first state of that SCC
/// returns true, if witness path is found, otherwise false
bool DFSFormula::getFairWitness(Store<void*>& s, NetState& ns,
		Firelist& firelist, std::vector<int>* witness, fairness_data& fairness) {
	void** pPayload;
	s.searchAndInsert(ns,&pPayload,0);
	statenumber_t initialDFS = getDFS(*pPayload); // all states in the SCC will be set to this DFS number to mark the area for later steps

	//rep->status("getFairWitness init at %x initialDFS %ld (pPayload: %lx, payload: %lx)",ns.HashCurrent, initialDFS, pPayload, *pPayload);

	SearchStack<DFSStackEntry> dfsStack;
	bool nonTrivial = false; // states whether current SCC contains any cycles

	index_t* currentFirelist;
	index_t currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

	// flags which weak fairness assumptions are met
	bool* fulfilled_weak = (bool*) calloc(fairness.card_weak, SIZEOF_BOOL);
	// flags which strong fairness assumptions are met
	bool* fulfilled_strong = (bool*) calloc(fairness.card_strong,
			SIZEOF_BOOL);
	// flags which strong fair transitions were enabled in at least one of the SCC states
	bool* enabled_strong = (bool*) calloc(fairness.card_strong, SIZEOF_BOOL);

	// check for weak fairness enabledness (not enabled -> fulfilled)
	for(int t = 0; t < fairness.card_weak; t++)
		if(!ns.Enabled[fairness.weak_fairness[t]])
			fulfilled_weak[fairness.weak_fairness[t]] = true;

	// check for strong fairness enabledness
	for(int t = 0; t < currentFirelistIndex; t++)
		if(fairness.strong_backlist[currentFirelist[t]] != -1)
			enabled_strong[fairness.strong_backlist[currentFirelist[t]]] = true;

	// re-initialize state for possible recursive DFS search. Will be overridden again once the fairness check is done.
	setCachedResult(*pPayload, UNKNOWN);

	while(true) {
		if(currentFirelistIndex--) {

			Transition::fire(ns, currentFirelist[currentFirelistIndex]);

			void** pNewPayload;
			s.searchAndInsert(ns,&pNewPayload,0);
			statenumber_t curDFS = getDFS(*pNewPayload);
			// test if new state belongs to this SCC

			//rep->status("getFairWitness fire %d to %x (pPayload: %lx, payload: %lx)",currentFirelist[currentFirelistIndex],ns.HashCurrent,pNewPayload,*pNewPayload);

			//rep->status("getFairWitness dfs %ld result %d",curDFS,getCachedResult(*pNewPayload));

			if(curDFS != initialDFS && (getCachedResult(*pNewPayload) != IN_PROGRESS || curDFS < initialDFS)) {
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}

			// found edge to another SCC state (maybe self-loop)
			nonTrivial = true;

			// check if used transition is needed for weak fairness
			if(fairness.weak_backlist[currentFirelist[currentFirelistIndex]] != -1)
				fulfilled_weak[fairness.weak_backlist[currentFirelist[currentFirelistIndex]]] = true;

			// check if used transition is needed for strong fairness
			if(fairness.strong_backlist[currentFirelist[currentFirelistIndex]] != -1)
				fulfilled_strong[fairness.strong_backlist[currentFirelist[currentFirelistIndex]]] = true;

			// test if state was already visited
			if(curDFS == initialDFS) {
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}

			//rep->status("getFairWitness new state");

			// update maximum DFS number seen
			//if(curDFS > maxDFS)
			//	maxDFS = curDFS;

			// mark state as visited
			setDFS(*pNewPayload, initialDFS);
			// re-initialize state for possible recursive DFS search. Will be overridden again once the fairness check is done.
			setCachedResult(*pNewPayload, UNKNOWN);

			// update enabledness for further search
			Transition::updateEnabled(ns, currentFirelist[currentFirelistIndex]);

			// store current firelist to stack
			new (dfsStack.push()) DFSStackEntry(currentFirelist,currentFirelistIndex,*pNewPayload,0);
			// get new firelists
			currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

			// check for weak fairness enabledness (not enabled -> fulfilled)
			for(int t = 0; t < fairness.card_weak; t++)
				if(!ns.Enabled[fairness.weak_fairness[t]])
					fulfilled_weak[fairness.weak_fairness[t]] = true;

			// check for strong fairness enabledness
			for(int t = 0; t < currentFirelistIndex; t++)
				if(fairness.strong_backlist[currentFirelist[t]] != -1)
					enabled_strong[fairness.strong_backlist[currentFirelist[t]]] = true;
		} else {
			delete[] currentFirelist;

			//rep->status("getFairWitness backtrack from %x",ns.HashCurrent);

			// check if there are any states to backtrack to
			if(dfsStack.StackPointer) {
				currentFirelist = dfsStack.top().fl;
				currentFirelistIndex = dfsStack.top().flIndex;
				dfsStack.pop();

				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				////rep->status("backfire %d (%x %d) to %x",currentFirelist[currentFirelistIndex],currentFirelist,currentFirelistIndex,ns.HashCurrent);
	            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
			} else
				break;
		}
	}

	bool weakFair = true;
	bool strongFair = true;

	// if area is trivial, there is no witness
	if(!nonTrivial)
		return weakFair = false;

	// test if weak fairness assumption is violated
	for(int t = 0; t < fairness.card_weak; t++)
		if(!fulfilled_weak[t])
			weakFair = false;

	// test if strong fairness assumptions are violated, add them to forbidden transitions in case
	for(int t = 0; t < fairness.card_strong; t++)
		if(!fulfilled_strong[t] && enabled_strong[t]) {
			strongFair = false;
			fairness.forbidden_transitions[fairness.card_forbidden_transitions++] = fairness.strong_fairness[t];
		}

	// construct witness path if fairness test passed
	if(weakFair && strongFair)
		constructWitness(s,ns,firelist,witness,fairness,enabled_strong);

	// clean up
	free(fulfilled_weak);
	free(fulfilled_strong);
	free(enabled_strong);

	if(!weakFair)
		return false;

	if(!strongFair)
		return subdivideFairnessCheck(s,ns,firelist,witness,fairness);
	else
		return true;
}

// traverses the fair SCC and finds a cycle that will represent the (infinite) the witness path
// assumes that ns is the first SCC state, the SCC is non-trivial and all SCC states are set to the same DFS number and UNKNOWN result
// assures that ns is unchanged and witness contains a fair witness path starting from the initial state

void DFSFormula::constructWitness(Store<void*>& s, NetState& ns,
		Firelist& firelist, std::vector<int>* witness, fairness_data& fairness, bool* enabled_strong) {
	void** pPayload;
	s.searchAndInsert(ns,&pPayload,0);

	//rep->status("constructWitness init at %x (pPayload: %lx, payload: %lx)",ns.HashCurrent,pPayload,*pPayload);

	bool* fulfilled_weak = (bool*) calloc(fairness.card_weak,SIZEOF_BOOL);
	bool* fulfilled_strong = (bool*) calloc(fairness.card_strong,SIZEOF_BOOL);

	index_t fulfilled_conditions = 0;

	// if a strong fair transition is never enabled, it is fulfilled
	for(index_t t = 0; t < fairness.card_strong; t++)
		if(!enabled_strong[t]) {
			fulfilled_strong[t] = true;
			fulfilled_conditions++;
		}

	// check for weak fairness enabledness (not enabled -> fulfilled)
	for(int t = 0; t < fairness.card_weak; t++)
		if(!fulfilled_weak[fairness.weak_fairness[t]] && !ns.Enabled[fairness.weak_fairness[t]]) {
			fulfilled_weak[fairness.weak_fairness[t]] = true;
			fulfilled_conditions++;
		}

	produceWitness(s,ns,firelist,witness,fairness,*pPayload,getDFS(*pPayload),fulfilled_weak,fulfilled_strong,fulfilled_conditions);

	free(fulfilled_weak);
	free(fulfilled_strong);
}

void DFSFormula::produceWitness(Store<void*>& s, NetState& ns,
		Firelist& firelist, std::vector<int>* witness, fairness_data& fairness, void* initialPayload, statenumber_t initialDFS, bool* fulfilled_weak, bool* fulfilled_strong,
		index_t initial_fulfilled_conditions) {

	statenumber_t myDFS = initialDFS + initial_fulfilled_conditions + 1;
	index_t current_fulfilled_conditions = initial_fulfilled_conditions;

	void** pPayload;
	s.searchAndInsert(ns,&pPayload,0);

	//rep->status("produceWitness init at %x (payload: %lx, myDFS: %ld, fc: %d)",ns.HashCurrent,*pPayload,myDFS,initial_fulfilled_conditions);

	SearchStack<DFSStackEntry> dfsStack;

	index_t* currentFirelist;
	index_t currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

	// starting state is not marked as visited to detect loops if any are present

	while(true) {
		if(currentFirelistIndex--) {
			Transition::fire(ns, currentFirelist[currentFirelistIndex]);

			void** pNewPayload;
			s.searchAndInsert(ns,&pNewPayload,0);
			statenumber_t curDFS = getDFS(*pNewPayload);

			//rep->status("produceWitness fire %d to %x (payload: %lx)",currentFirelist[currentFirelistIndex],ns.HashCurrent,*pPayload);

			//rep->status("produceWitness dfs %ld result %d",curDFS,getCachedResult(*pNewPayload));

			// test if new state belongs to this SCC
			if(getCachedResult(*pNewPayload) != UNKNOWN || curDFS < initialDFS || curDFS > myDFS) {
				//rep->status("produceWitness backfire");
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}

			// check if used transition is needed for weak fairness
			if(fairness.weak_backlist[currentFirelist[currentFirelistIndex]] != -1 && !fulfilled_weak[fairness.weak_backlist[currentFirelist[currentFirelistIndex]]]) {
				fulfilled_weak[fairness.weak_backlist[currentFirelist[currentFirelistIndex]]] = true;
				current_fulfilled_conditions++;
			}

			// check if used transition is needed for strong fairness
			if(fairness.strong_backlist[currentFirelist[currentFirelistIndex]] != -1 && !fulfilled_strong[fairness.strong_backlist[currentFirelist[currentFirelistIndex]]]) {
				fulfilled_strong[fairness.strong_backlist[currentFirelist[currentFirelistIndex]]] = true;
				current_fulfilled_conditions++;
			}

			// update enabledness for further tests
			Transition::updateEnabled(ns, currentFirelist[currentFirelistIndex]);

			// check for weak fairness enabledness (not enabled -> fulfilled)
			for(int t = 0; t < fairness.card_weak; t++)
				if(!fulfilled_weak[fairness.weak_fairness[t]] && !ns.Enabled[fairness.weak_fairness[t]]) {
					fulfilled_weak[fairness.weak_fairness[t]] = true;
					current_fulfilled_conditions++;
				}

			// test if all assumptions are met now, or at least some new
			// note that initial_fulfilled_conditions can be fairness.card_strong + fairness.card_weak as well if all fairness assumptions were met in the initial state. Still, a cycle needs to be found.
			if(current_fulfilled_conditions == fairness.card_strong + fairness.card_weak || current_fulfilled_conditions > initial_fulfilled_conditions) {

				//rep->status("produceWitness fc: %d, cs: %d, cw: %d",current_fulfilled_conditions,fairness.card_strong,fairness.card_weak);

				if(current_fulfilled_conditions == fairness.card_strong + fairness.card_weak) {
					// all assumptions are met now, find a path back to the initial node
					findWitnessPathTo(s,ns,firelist,witness,initialPayload,initialDFS,myDFS + 1);
				}
				else {
					// some new assumptions are met, recursively find the rest of the witness path that needs to fulfill fewer assumptions
					produceWitness(s, ns, firelist, witness, fairness, initialPayload, initialDFS, fulfilled_weak, fulfilled_strong, current_fulfilled_conditions);
				}

				// extend witness path with the transitions used to reach this state

				// fired transitions are not backfired since the last state we reached to complete the loop was the initial state
				witness->push_back(currentFirelist[currentFirelistIndex]);

	            delete[] currentFirelist;

				while(dfsStack.StackPointer) {
					currentFirelist = dfsStack.top().fl;
					currentFirelistIndex = dfsStack.top().flIndex;
					dfsStack.pop();

					witness->push_back(currentFirelist[currentFirelistIndex]);

		            delete[] currentFirelist;
				}
				return;
			}


			// test if new state belongs to this SCC and is unvisited
			if(curDFS == myDFS) {
				//rep->status("produceWitness backfire2");
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				Transition::revertEnabled(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}

			// mark state visited
			setDFS(*pNewPayload,myDFS);

			// store current firelist to stack
			new (dfsStack.push()) DFSStackEntry(currentFirelist,currentFirelistIndex,*pNewPayload,0);
			// get new firelists
			currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);
		} else {
			delete[] currentFirelist;

			//rep->status("produceWitness backtrack from %x",ns.HashCurrent);

			// we can't run out of stack frames since a valid witness path is guaranteed to exist
			assert(dfsStack.StackPointer);

			currentFirelist = dfsStack.top().fl;
			currentFirelistIndex = dfsStack.top().flIndex;
			dfsStack.pop();

			Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
		}
	}
}


void DFSFormula::findWitnessPathTo(Store<void*>& s, NetState& ns,
		Firelist& firelist, std::vector<int>* witness, void* destinationPayload, statenumber_t initialDFS, statenumber_t myDFS) {

	void** pPayload;
	s.searchAndInsert(ns,&pPayload,0);

	SearchStack<DFSStackEntry> dfsStack;

	index_t* currentFirelist;
	index_t currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

	witness->clear();

	//rep->status("findWitnessPathTo init at %x (payload: %lx)",ns.HashCurrent, destinationPayload);

	//rep->status("findWitnessPathTo start %lx", *pPayload);

	// starting state is the destination, witness path is empty
	if(*pPayload == destinationPayload)
		return;

	// mark state visited
	setDFS(*pPayload,myDFS);

	while(true) {
		if(currentFirelistIndex--) {
			Transition::fire(ns, currentFirelist[currentFirelistIndex]);

			void** pNewPayload;
			s.searchAndInsert(ns,&pNewPayload,0);
			statenumber_t curDFS = getDFS(*pNewPayload);

			//rep->status("findWitnessPathTo fire %d to %x (payload: %lx)",currentFirelist[currentFirelistIndex],ns.HashCurrent,*pNewPayload);

			//rep->status("findWitnessPathTo dfs %ld result %d",curDFS,getCachedResult(*pNewPayload));

			// test if new state belongs to this SCC and is unvisited
			if(getCachedResult(*pNewPayload) != UNKNOWN || curDFS < initialDFS || curDFS >= myDFS) {
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}

			// current state is the destination
			if(*pNewPayload == destinationPayload) {
				// fired transitions are not backfired since the state we just found is the initial state
				witness->push_back(currentFirelist[currentFirelistIndex]);

	            delete[] currentFirelist;

				while(dfsStack.StackPointer) {
					currentFirelist = dfsStack.top().fl;
					currentFirelistIndex = dfsStack.top().flIndex;
					dfsStack.pop();

					witness->push_back(currentFirelist[currentFirelistIndex]);

		            delete[] currentFirelist;
				}
				return;
			}

			// mark state visited
			setDFS(*pNewPayload,myDFS);

			// update enabledness for further search
			Transition::updateEnabled(ns, currentFirelist[currentFirelistIndex]);

			// store current firelist to stack
			new (dfsStack.push()) DFSStackEntry(currentFirelist,currentFirelistIndex,*pNewPayload,0);
			// get new firelists
			currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);
		} else {
			delete[] currentFirelist;

			// we can't run out of stack frames since a valid witness path is guaranteed to exist
			assert(dfsStack.StackPointer);

			currentFirelist = dfsStack.top().fl;
			currentFirelistIndex = dfsStack.top().flIndex;
			dfsStack.pop();

			Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
		}
	}
}

// traverses the SCC and starts recursive fairness checks for sub-SCCs resulting from the new forbidden transitions.
// assumes that ns is the first SCC state and all SCC states are set to the same DFS number and UNKNOWN result
// returns true, if fair witness path was found, otherwise false
// if returning false, assures that ns is unchanged and all SCC states kept their (identical) DFS number and have KNOWN_TRUE result
// if returning true, assures that ns is unchanged and witness contains a fair witness path starting from the initial state

bool DFSFormula::subdivideFairnessCheck(Store<void*>& s, NetState& ns,
		Firelist& firelist, std::vector<int>* witness, fairness_data& fairness) {
	void** pPayload;
	s.searchAndInsert(ns,&pPayload,0);
	statenumber_t initialDFS = getDFS(*pPayload); // all states in the SCC are assumed to have this DFS number (set by getFairWitness)

	SearchStack<DFSStackEntry> dfsStack;

	index_t* currentFirelist;
	index_t currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

	// start SCC search from this state
	if(fairSCC(s,ns,firelist,witness,fairness))
		return true;
	setCachedResult(*pPayload,KNOWN_TRUE);

	while(true) {
		if(currentFirelistIndex--) {
			Transition::fire(ns, currentFirelist[currentFirelistIndex]);

			void** pNewPayload;
			s.searchAndInsert(ns,&pNewPayload,0);
			statenumber_t curDFS = getDFS(*pNewPayload);
			// test if new state belongs to this SCC
			if(curDFS != initialDFS) {
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}

			CTLFormulaResult newResult = getCachedResult(*pNewPayload);

			// test if state was already visited by this DFS
			if(newResult == KNOWN_TRUE) {
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}

			// update enabledness for further search
			Transition::updateEnabled(ns, currentFirelist[currentFirelistIndex]);

			// test if node not already visited by a previous SCC search
			if(newResult == UNKNOWN) {
				// start SCC search from this state
				if(fairSCC(s,ns,firelist,witness,fairness)) {
					// extend witness path with all transitions that were fired to get here; revert ns to initial state; cleanup

					// add transition that was used to get here to witness
					Transition::backfire(ns, currentFirelist[currentFirelistIndex]);
		            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
					witness->push_back(currentFirelist[currentFirelistIndex]);

					delete[] currentFirelist;

					// add all transitions on DFS stack to witness
					while(dfsStack.StackPointer) {
						currentFirelist = dfsStack.top().fl;
						currentFirelistIndex = dfsStack.top().flIndex;
						dfsStack.pop();

						Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
			            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
						witness->push_back(currentFirelist[currentFirelistIndex]);

						delete[] currentFirelist;
					}

					return true;
				}
			}
			// getCachedResult(*pNewPayload) is set to KNOWN_FALSE by one of the fairSCC searches at this point
			assert(getCachedResult(*pNewPayload) == KNOWN_FALSE);

			// mark state visited
			setCachedResult(*pNewPayload,KNOWN_TRUE);

			// store current firelist to stack
			new (dfsStack.push()) DFSStackEntry(currentFirelist,currentFirelistIndex,*pNewPayload,0);
			// get new firelists
			currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);
		} else {
			delete[] currentFirelist;

			// check if there are any states to backtrack to
			if(dfsStack.StackPointer) {
				currentFirelist = dfsStack.top().fl;
				currentFirelistIndex = dfsStack.top().flIndex;
				dfsStack.pop();

				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				////rep->status("backfire %d (%x %d) to %x",currentFirelist[currentFirelistIndex],currentFirelist,currentFirelistIndex,ns.HashCurrent);
	            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
			} else {
				// SCC fully explored and no fair path was found
				return false;
			}
		}
	}
}


// performs a SCC search (Tarjan) starting from ns and considering all forbidden transitions. For each SCC, the fairness conditions are checked.
// assumes that all states that need to be explored have the same DFS number and UNKNOWN result
// returns true, if fair witness path was found, otherwise false
// if returning false, it assures that ns is unchanged and all states that were explored kept their (identical) DFS number and have KNOWN_FALSE result
// if returning true, it assures that ns is unchanged and the witness variable contains a fair witness path starting from the initial state of the search
bool DFSFormula::fairSCC(Store<void*>& s, NetState& ns,
		Firelist& firelist, std::vector<int>* witness, fairness_data& fairness) {
	void** pInitialPayload;
	s.searchAndInsert(ns,&pInitialPayload,0);
	void* payload = *pInitialPayload;
	statenumber_t initialDFS = getDFS(payload); // all states in the considered area are assumed to have this DFS number

	// dfs stack will contain all gray nodes
	SearchStack<DFSStackEntry> dfsStack;

	// tarjan stack will contain all _black_ nodes the SCC of which is not yet finished
	// see doc/Tarjan for an explanation on how and why this works
	SearchStack<void*> tarjanStack;

	statenumber_t currentDFSNumber = initialDFS; // starting with initialDFS+1 to leave initialDFS for recognizing uninitialized values
	statenumber_t currentLowlink;

	// fetch firelist
	index_t* currentFirelist;
	index_t currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

	// initialize dfs number, lowlink; mark state to be on tarjan stack
	setDFS(payload, ++currentDFSNumber);
	currentLowlink = currentDFSNumber;
	setCachedResult(payload,IN_PROGRESS);

	while(true) {
		if(currentFirelistIndex--) {
			Transition::fire(ns, currentFirelist[currentFirelistIndex]);

			void** pNewPayload;
			s.searchAndInsert(ns,&pNewPayload,0);

			CTLFormulaResult newCachedResult = getCachedResult(*pNewPayload);
			statenumber_t newDFS = getDFS(*pNewPayload);

			if(newCachedResult & 2) { // result is KNOWN_TRUE or KNOWN_FALSE
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}

			// check if new state belongs to this area and is UNKNOWN
			if(newCachedResult == UNKNOWN && newDFS == initialDFS) {

				// updating enabledness is necessary for further tests
	            Transition::updateEnabled(ns, currentFirelist[currentFirelistIndex]);

	            // check if new state enables forbidden transitions
				bool forbidden = false;
				for(index_t i = 0; i < fairness.card_forbidden_transitions; i++)
					if (ns.Enabled[fairness.forbidden_transitions[i]]){
						forbidden = true;
						break;
					}
				if (forbidden) {
					Transition::backfire(ns, currentFirelist[currentFirelistIndex]);
					Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
					continue;
				}

				// recursive descent
				new (dfsStack.push()) DFSStackEntry(currentFirelist,currentFirelistIndex,payload,currentLowlink);

	            // get new firelist
				payload = *pNewPayload;
				currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

				setDFS(payload, ++currentDFSNumber);
				currentLowlink = currentDFSNumber;
				setCachedResult(payload,IN_PROGRESS);
				continue;
			} else {
				if(newCachedResult == IN_PROGRESS || newDFS >= initialDFS) { // test if state belongs to this search (there may also be other running SCC searches on the call stack)
					// update lowlink
					if(newDFS < currentLowlink)
						currentLowlink = newDFS;
				}
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				continue;
			}
		} else { // if(currentFirelistIndex--)
			// free memory for finished firelist
			delete[] currentFirelist;

			// check if SCC is finished
			statenumber_t dfs = getDFS(payload);
			if(dfs == currentLowlink) {

				// SCC finished, test if fair witness can be found
				if(getFairWitness(s,ns,firelist,witness,fairness)) {
					// fair witness found, extend witness path and cleanup

					// add all transitions on DFS stack to witness
					while(dfsStack.StackPointer) {
						currentFirelist = dfsStack.top().fl;
						currentFirelistIndex = dfsStack.top().flIndex;
						payload = dfsStack.top().payload;
						dfsStack.pop();

						Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
			            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
						witness->push_back(currentFirelist[currentFirelistIndex]);

						delete[] currentFirelist;
					}
					return true;
				}
				// SCC contained no fair witness, setting all nodes to KNOWN_FALSE and reset their DFS number to initialDFS
				// all elements on tarjan stack that have a higher dfs number then ours belong to the finished SCC -> formula true
				while(tarjanStack.StackPointer && getDFS(tarjanStack.top()) > dfs) {
					setCachedResult(tarjanStack.top(),KNOWN_FALSE);
					setDFS(tarjanStack.top(),initialDFS);
					tarjanStack.pop();
				}
				setCachedResult(payload,KNOWN_FALSE);
				setDFS(payload,initialDFS);
			} else {
				// SCC not yet finished, push self onto Tarjan stack
				assert(dfs > currentLowlink);
				*tarjanStack.push() = payload;
			}

			// check if there are any states to backtrack to
			if(dfsStack.StackPointer) {
				currentFirelist = dfsStack.top().fl;
				currentFirelistIndex = dfsStack.top().flIndex;
				payload = dfsStack.top().payload;
				if(currentLowlink > dfsStack.top().lowlink) // propagate lowlink to parent
					currentLowlink = dfsStack.top().lowlink;
				dfsStack.pop();

				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
	            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
				revertAtomics(ns,currentFirelist[currentFirelistIndex]);
			} else {
				assert(!tarjanStack.StackPointer); // tarjan stack empty
				assert(dfs == currentLowlink); // first node is always start of SCC
				assert(*pInitialPayload == payload); // returned to initial state

				// no fair witness path found. The result of all states have been set to KNOWN_FALSE and the DFS numbers were reset to initialDFS.
				return false;
			}
		}
	}
}
