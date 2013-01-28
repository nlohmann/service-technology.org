
#include <Formula/CTL/AUFormula.h>
#include <Exploration/SearchStack.h>
#include <Net/Transition.h>
#include <Net/Net.h>



bool AUFormula::check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {

	void** pInitialPayload;
	if(!s.searchAndInsert(ns, &pInitialPayload, 0))
		*pInitialPayload = calloc(payloadsize,1); // all-zeros is starting state for all values
	void* payload = *pInitialPayload;

	CTLFormulaResult cachedResult = getCachedResult(payload);
	if(cachedResult & 2) // value known
		return cachedResult & 1; // return result
	assert(cachedResult != IN_PROGRESS); // impossible for first state

	// psi -> A phi U psi
	if(psi->check(s,ns,firelist,witness)) {
		setCachedResult(payload,KNOWN_TRUE);
		return true;
	}

	witness->clear();
	// !phi and !psi -> !E phi U psi
	if(!phi->check(s,ns,firelist,witness)) {
		setCachedResult(payload,KNOWN_FALSE);
		return false;
	}

	//rep->status("starting AU check at %x (payload: %lx)",ns.HashCurrent,payload);

	// initialize fairness data
	fairness_data fairness;

	// prepare strong fairness assumptions
	fairness.card_strong = 0;
	for (index_t i = 0; i < Net::Card[TR]; i++)
		if (Transition::Fairness[i] == STRONG_FAIRNESS)
			fairness.card_strong++;
	fairness.strong_fairness = (index_t*) calloc(fairness.card_strong,SIZEOF_INDEX_T);
	fairness.strong_backlist = (index_t*) calloc(Net::Card[TR],SIZEOF_INDEX_T);
	// put all strong fair transitions into an array
	index_t __card_on_sf = 0;
	for (index_t i = 0; i < Net::Card[TR]; i++){
		if (Transition::Fairness[i] == STRONG_FAIRNESS) {
			fairness.strong_fairness[__card_on_sf] = i;
			fairness.strong_backlist[i] = __card_on_sf++;
		} else
			fairness.strong_backlist[i] = -1;
	}

	// prepare weak fairness assumptions
	fairness.card_weak = 0;
	for (index_t i = 0; i < Net::Card[TR]; i++)
		if (Transition::Fairness[i] == WEAK_FAIRNESS)
			fairness.card_weak++;
	fairness.weak_fairness = (index_t*) calloc(fairness.card_weak,SIZEOF_INDEX_T);
	fairness.weak_backlist = (index_t*) calloc(Net::Card[TR],SIZEOF_INDEX_T);
	// put all weak fair transitions into an array
	index_t __card_on_wf = 0;
	for (index_t i = 0; i < Net::Card[TR]; i++){
		if (Transition::Fairness[i] == WEAK_FAIRNESS) {
			fairness.weak_fairness[__card_on_wf] = i;
			fairness.weak_backlist[i] = __card_on_wf++;
		} else
			fairness.weak_backlist[i] = -1;
	}
	fairness.forbidden_transitions = (index_t*) calloc(fairness.card_strong, SIZEOF_INDEX_T);
	fairness.card_forbidden_transitions = 0;


	// dfs stack will contain all gray nodes
	SearchStack<DFSStackEntry> dfsStack;

	// tarjan stack will contain all _black_ nodes the SCC of which is not yet finished
	// see doc/Tarjan for an explanation on how and why this works
	SearchStack<void*> tarjanStack;

	statenumber_t currentDFSNumber = 0; // starting with 1 to leave 0 for recognizing uninitialized values
	statenumber_t currentLowlink;

	// fetch firelist
	index_t* currentFirelist;
	index_t currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

	// initialize dfs number, lowlink; mark state to be on tarjan stack
	setDFS(payload, ++currentDFSNumber);
	currentLowlink = currentDFSNumber;
	setCachedResult(payload,IN_PROGRESS);

	bool revertEnabledNeeded = false;
	bool backfireNeeded = true;

	while(true) {
		if(currentFirelistIndex--) {
			Transition::fire(ns, currentFirelist[currentFirelistIndex]);
			// don't update enabledness and atomics yet, since it'll maybe not be needed at all.

			void** pNewPayload;
			if(!s.searchAndInsert(ns,&pNewPayload,0))
				*pNewPayload = calloc(payloadsize,1); // all-zeros is starting state for all values
			void* newpayload = *pNewPayload;

			//rep->status("AU check fire %d to %x (payload: %lx)",currentFirelist[currentFirelistIndex],ns.HashCurrent,newpayload);

			CTLFormulaResult newCachedResult = getCachedResult(newpayload);
			if(newCachedResult == UNKNOWN) {

				// update enabledness and atomic propositions for current state (needed for further checking)
	            Transition::updateEnabled(ns, currentFirelist[currentFirelistIndex]);
				updateAtomics(ns, currentFirelist[currentFirelistIndex]);

				witness->clear();
				if(psi->check(s,ns,firelist,witness)) {
					setCachedResult(newpayload,KNOWN_TRUE);
					// continue;
					Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
		            Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
					revertAtomics(ns,currentFirelist[currentFirelistIndex]);
					continue;
				}

				witness->clear();
				if(!phi->check(s,ns,firelist,witness)) {
					setCachedResult(newpayload,KNOWN_FALSE);
					// we updated the enabledness, so it needs to be reverted
					revertEnabledNeeded = true;
					// break; set all nodes to false
					break;
				}

				// recursive descent
	            DFSStackEntry* entry = dfsStack.push();
				new (entry) DFSStackEntry(currentFirelist,currentFirelistIndex,payload,currentLowlink);

	            // get new firelist
				payload = newpayload;
				currentFirelistIndex = firelist.getFirelist(ns,&currentFirelist);

				setDFS(newpayload, ++currentDFSNumber);
				currentLowlink = currentDFSNumber;
				setCachedResult(newpayload,IN_PROGRESS);
				continue;
			} else if(newCachedResult == KNOWN_FALSE) {
				// break; set all nodes to false
				break;
			} else if(newCachedResult == IN_PROGRESS) {
				//if(true) { // TODO: fairness check
					// break; set all nodes to false
				//	break;
				//}

				// update lowlink and continue
				statenumber_t newdfs = getDFS(newpayload);
				if(newdfs < currentLowlink)
					currentLowlink = newdfs;
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				// enabledness and atomics weren't updated, so no revert needed
				continue;
			} else { // KNOWN_TRUE
				// continue;
				Transition::backfire(ns,currentFirelist[currentFirelistIndex]);
				// enabledness and atomics weren't updated, so no revert needed
				continue;
			}
		} else { // if(currentFirelistIndex--)
			// free memory for finished firelist
			delete[] currentFirelist;

			// check if SCC is finished
			statenumber_t dfs = getDFS(payload);
			if(dfs == currentLowlink) {

				// SCC finished, test if fair witness can be found
				fairness.card_forbidden_transitions = 0; // reset fairness data
				if(getFairWitness(s,ns,firelist,witness,fairness)) {
					// no need to pop elements from Tarjan stack to set them to KNOWN_FALSE, this is done later anyway
					backfireNeeded = false;
					break;
				}

				// all elements on tarjan stack that have a higher dfs number then ours belong to the finished SCC -> formula true
				while(tarjanStack.StackPointer && getDFS(tarjanStack.top()) >= dfs) {
					setCachedResult(tarjanStack.top(),KNOWN_TRUE);
					tarjanStack.pop();
				}
				setCachedResult(payload,KNOWN_TRUE);
			} else {
				// SCC not yet finished, push self onto tarjan stack
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

				// no (negative) witness path found
				witness->clear();

				free(fairness.strong_backlist);
				free(fairness.strong_fairness);
				free(fairness.weak_backlist);
				free(fairness.weak_fairness);
				free(fairness.forbidden_transitions);

				return true;
			}
		}
	}
	// revert transition that brought us to the counterexample state
	if(backfireNeeded) {
		Transition::backfire(ns,currentFirelist[currentFirelistIndex]);

		// add transition to witness path
		witness->push_back(currentFirelist[currentFirelistIndex]);

		if(revertEnabledNeeded) {
			Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
			revertAtomics(ns,currentFirelist[currentFirelistIndex]);
		}
	}

	// current state can reach counterexample state -> formula false
	setCachedResult(payload,KNOWN_FALSE);

	// all elements that are still on tarjan stack can reach this state -> formula false
	while(tarjanStack.StackPointer) {
		setCachedResult(tarjanStack.top(),KNOWN_FALSE);
		tarjanStack.pop();
	}

	// all elements that are still on dfs stack (and hence on tarjan stack)
	// can reach this state -> formula false
	// revert all the transitions to restore original NetState
	while(dfsStack.StackPointer) {
		setCachedResult(dfsStack.top().payload,KNOWN_FALSE);
		Transition::backfire(ns,dfsStack.top().fl[dfsStack.top().flIndex]);
        Transition::revertEnabled(ns, dfsStack.top().fl[dfsStack.top().flIndex]);
		revertAtomics(ns,dfsStack.top().fl[dfsStack.top().flIndex]);

		witness->push_back(dfsStack.top().fl[dfsStack.top().flIndex]);

		dfsStack.pop();
	}
	// (negative) witness found

	free(fairness.strong_backlist);
	free(fairness.strong_fairness);
	free(fairness.weak_backlist);
	free(fairness.weak_fairness);
	free(fairness.forbidden_transitions);

	return false;
}
