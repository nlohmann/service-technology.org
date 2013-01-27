#include <Exploration/LTLExploration.h>
#include <Net/Transition.h>
#include <Net/Net.h>

LTLExploration::LTLExploration() {
}

LTLExploration::~LTLExploration() {

}

/// check whether an automata state is contained in an automata tree
inline bool searchAndInsertAutomataState(uint32_t automataState,
		AutomataTree** tree, AutomataTree** result) {
	while (true) {
		//rep->message("S %d %d", automataState, (*tree)?(*tree)->state:-1);
		//rep->message("sai %d %d %d", automataState, *tree, (*tree)?(*tree)->state:-1);
		if (*tree == 0) {
			*tree = new AutomataTree(automataState);
			*result = *tree;
			//rep->message("create state %d @ %d", automataState, *tree);
			return false;
		}
		if ((*tree)->state == automataState) {
			*result = *tree;
			return true;
		}
		if ((*tree)->state > automataState){
			tree = &(*tree)->smaller;
		}
		else{
			tree = &(*tree)->bigger;
		}
	}
}

inline void LTLExploration::get_first_transition(NetState &ns,
		index_t* currentFirelistEntry, index_t* currentFirelist){
	//rep->message("=========================");
	while (true){
		Transition::fire(ns, currentFirelist[*currentFirelistEntry]);
		Transition::updateEnabled(ns, currentFirelist[*currentFirelistEntry]);
		// check whether this transition goes into an non forbidden state
		//rep->message("current F: %d %d %d, %d %d %d, %d", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6]);
		bool forbidden = false;
		for(index_t i = 0; i < card_forbidden_transtitions; i++)
		if (ns.Enabled[forbidden_transtitions[i]]){
			//rep->message("FORBI %d %s", forbidden_transtitions[i], Net::Name[TR][forbidden_transtitions[i]]);
			forbidden = true; break;
		}
		if (!forbidden) break;
		//rep->message("FORB");
		Transition::backfire(ns, currentFirelist[*currentFirelistEntry]);
		Transition::revertEnabled(ns,currentFirelist[*currentFirelistEntry]);
		if (--(*currentFirelistEntry) == -1) break;
	}
}


inline void LTLExploration::get_next_transition(BuechiAutomata &automata, NetState &ns,
		index_t* currentStateListEntry, index_t* currentFirelistEntry, index_t* currentFirelist,
		index_t** currentStateList, index_t currentAutomataState){
	// calculate the next transition
	if (*currentStateListEntry == 0) {
		// revert currently fired transitions
		while (true){
			Transition::backfire(ns, currentFirelist[*currentFirelistEntry]);
			Transition::revertEnabled(ns,currentFirelist[*currentFirelistEntry]);
			//rep->message("current N: %d %d %d, %d %d %d, %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState);
			// revert the petrinet
			automata.revertProperties(ns,currentFirelist[*currentFirelistEntry]);
			delete[] *currentStateList;
			(*currentFirelistEntry)--;
			// if there is no next transition ext
			if (*currentFirelistEntry == -1) break;
			// pre-fire transition
			Transition::fire(ns, currentFirelist[*currentFirelistEntry]);
			Transition::updateEnabled(ns, currentFirelist[*currentFirelistEntry]);
			//rep->message("current F: %d %d %d, %d %d %d, %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState);
			// check current marking for property
			automata.updateProperties(ns, currentFirelist[*currentFirelistEntry]);
			// check for forbidden transition
			bool forbidden = false;
			for(index_t i = 0; i < card_forbidden_transtitions; i++)
				if (ns.Enabled[forbidden_transtitions[i]]){
					forbidden = true;
					break;
				}
			if (forbidden)
				continue;
			// set available transitions in Buechi-Automata
			*currentStateListEntry = automata.getSuccessors(currentStateList, currentAutomataState)-1;
			if (*currentStateListEntry != -1) break;
		}
	} else {
		//rep->message("DEC");
		(*currentStateListEntry)--;
	}
}



////////////////////////////////////////////////////////////////////////////////
//
//
//           LTL - checks the fairness assumptions and returns
//				-1 if all are fulfilled,   -2 if a weak is not fulfilled (or büchi-property is not true)
//				n (>= 0) - number of strong(on array), which is not fulfilled
///////////////////////////////////////////////////////////////////////////////

index_t LTLExploration::checkFairness(BuechiAutomata &automata,
		Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
		index_t currentAutomataState, dfsnum_t depth, bool** enabledStrongTransitions) {

	//rep->message("====================CF==================");
	// the stack for the search
	SearchStack<LTLFairnessStackEntry> stack;
	// get first firelist
	index_t* currentFirelist;
	// the size of the list is not a valid index (thus -1)
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist) - 1;
	index_t* currentStateList;
	index_t currentStateListEntry;

	bool buechi_accepting_state = automata.isAcceptingState(
			currentAutomataState);

	// fairness datastructures
	// weak
	index_t card_fulfilled_weak = 0;
	bool* fulfilled_weak = (bool*) calloc(assumptions.card_weak, SIZEOF_BOOL);
	bool* fulfilled_strong = (bool*) calloc(assumptions.card_strong,
			SIZEOF_BOOL);
	bool* enabled_strong = (bool*) calloc(assumptions.card_strong, SIZEOF_BOOL);

	// fairness check for initial
	bool* __enabled_weak = (bool*) calloc(assumptions.card_weak, SIZEOF_BOOL);
	for (index_t i = 0; i <= currentFirelistEntry; i++) {
		if (assumptions.weak_backlist[currentFirelist[i]] != -1)
			__enabled_weak[assumptions.weak_backlist[currentFirelist[i]]] = true;
		else if (assumptions.strong_backlist[currentFirelist[i]] != -1)
			enabled_strong[i] = true;
	}
	// write not enabled weak back
	for (index_t i = 0; i < assumptions.card_weak; i++)
		if (!__enabled_weak[i]) {
			card_fulfilled_weak++;
			fulfilled_weak[i] = true;
		}


	// prefire first transition, this will be possible, as checkFairness will not be called on the empty SCC
	Transition::fire(ns, currentFirelist[currentFirelistEntry]);
	Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);
	// check current marking for property
	automata.updateProperties(ns,currentFirelist[currentFirelistEntry]);
	// set available transitions in Buechi-Automata
	currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);


	while (true) // exit when trying to pop from empty stack
	{
		// calculate the next transition
		if (currentStateListEntry == 0) {
			// revert currently fired transitions
			while (true){
				Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
				Transition::revertEnabled(ns,
									currentFirelist[currentFirelistEntry]);
				// revert the petrinet
				automata.revertProperties(ns,
						currentFirelist[currentFirelistEntry]);
				delete[] currentStateList;
				currentFirelistEntry--;
				// if there is no next transition ext
				if (currentFirelistEntry == -1) break;
				// pre-fire transition to produce new Marking::Current
				Transition::fire(ns, currentFirelist[currentFirelistEntry]);
				Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);
				// check forbidden transitions
				bool forbidden = false;
				for(index_t i = 0; i < card_forbidden_transtitions; i++)
					if (ns.Enabled[forbidden_transtitions[i]]){
						forbidden = true; break;
					}
				if (forbidden)
					continue;
				// check current marking for property
				automata.updateProperties(ns, currentFirelist[currentFirelistEntry]);
				// set available transitions in Buechi-Automata
				currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState)-1;
				if (currentStateListEntry != -1) break;
			}
		} else
			currentStateListEntry--;

		// there is a next transition that needs to be explored in current marking
		if (currentFirelistEntry != -1 && currentStateListEntry >= 0) {
			//rep->message("current state %d %d %d, %d %d %d, %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3],ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState);
			// search in the store
			AutomataTree* searchResult;
			store.searchAndInsert(ns, &searchResult, 0);
			AutomataTree* nextStateEntry = 0;
			// search state in state-tree
			searchAndInsertAutomataState(
					currentStateList[currentStateListEntry], &searchResult,
					&nextStateEntry);

			//rep->message("nSE %d",nextStateEntry->dfs, -depth);

			if (nextStateEntry->dfs == -depth || nextStateEntry->dfs == -depth-1){
				// maybe the transition itself does fulfill something
				if (assumptions.weak_backlist[currentFirelist[currentFirelistEntry]] != -1) {
					// weak, found a successor in set
					if (!fulfilled_weak[assumptions.weak_backlist[currentFirelist[currentFirelistEntry]]]) {
						card_fulfilled_weak++;
						fulfilled_weak[assumptions.weak_backlist[currentFirelist[currentFirelistEntry]]] = true;
					}
				} else if (assumptions.strong_backlist[currentFirelist[currentFirelistEntry]] != -1) {
					// strong, found a successor in set
					fulfilled_strong[assumptions.strong_backlist[currentFirelist[currentFirelistEntry]]] = true;
				}

				if (nextStateEntry->dfs == -depth){

					// switch to next state
					currentAutomataState = currentStateList[currentStateListEntry];

					// mark new state as visited
					nextStateEntry->dfs = -depth - 1;
					buechi_accepting_state |= automata.isAcceptingState(
							currentAutomataState);

					LTLFairnessStackEntry *stackEntry = stack.push();
					stackEntry = new (stackEntry) LTLFairnessStackEntry(
							currentFirelist, currentFirelistEntry, currentStateList,
							currentStateListEntry);

					// get Firelist and successor states
					currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist) - 1;

					// fairness check for new state
					memset(__enabled_weak, 0, assumptions.card_weak * SIZEOF_BOOL);
					for (index_t i = 0; i <= currentFirelistEntry; i++)
						if (assumptions.weak_backlist[currentFirelist[i]] != -1)
							__enabled_weak[assumptions.weak_backlist[currentFirelist[i]]] = true;
						else if (assumptions.strong_backlist[currentFirelist[i]] != -1 && !enabled_strong[assumptions.strong_backlist[currentFirelist[i]]])
							enabled_strong[assumptions.strong_backlist[currentFirelist[i]]] = true;
					for (index_t i = 0; i < assumptions.card_weak; i++)
						if (!__enabled_weak[i] && !fulfilled_weak[i]){
							card_fulfilled_weak++;
							fulfilled_weak[i] = true;
						}
					// end of fairness adjustments for this transition



					// the transition-list will be non-empty, as the current zone is an SCC
					Transition::fire(ns, currentFirelist[currentFirelistEntry]);
					Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);
					// check current marking for property
					automata.updateProperties(ns,
							currentFirelist[currentFirelistEntry]);
					// set available transitions in Buechi-Automata
					currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);
				}
			}
		} else {
			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			if (stack.StackPointer == 0) {

				//rep->message("====================RES==================");
				// searched all states, calculate return value
				if (card_fulfilled_weak != assumptions.card_weak
						|| !buechi_accepting_state){
					free(fulfilled_strong);
					free(fulfilled_weak);
					free(enabled_strong);
					free(__enabled_weak);
					return -2;
				}
				for (index_t i = 0; i < assumptions.card_strong; i++){
					bool bad = false;
					for(index_t j = 0; j < card_forbidden_transtitions; j++)
						if (forbidden_transtitions[j] == i){
							bad = true;
							break;
						}
					//rep->message("STRONG %d (%s): %d %d %d", i, Net::Name[TR][assumptions.strong_fairness[i]], enabled_strong[i], fulfilled_strong[i], bad);
					if (enabled_strong[i] && (!fulfilled_strong[i] || bad )){
						free(fulfilled_strong);
						free(fulfilled_weak);
						free(enabled_strong);
						free(__enabled_weak);
						//rep->message("Forbid %d %s",assumptions.strong_fairness[i], Net::Name[TR][assumptions.strong_fairness[i]]);
						if (bad)
							return -2;
						return i;
					}
				}
				free(fulfilled_strong);
				free(fulfilled_weak);
				free(__enabled_weak);
				// return the set of enabled strong transitions to main routine, so that we can search for a path on which all these lie
				*enabledStrongTransitions = enabled_strong;
				// all fairness assumptions are fulfilled
				return -1;
			}

			// load the own predecessor
			LTLFairnessStackEntry & stackEntry = stack.top();
			currentFirelistEntry = stackEntry.current_on_firelist;
			currentFirelist = stackEntry.fl;
			currentStateListEntry = stackEntry.current_on_statelist;
			currentStateList = stackEntry.states;
			stack.pop();

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the buechi automata
			currentAutomataState = currentStateList[currentStateListEntry];
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//
//
//           LTL - interates over all elements of an SCC, so that we can start all the searches
//				assumption: all markings of current component have DFS = -depth-1
//
///////////////////////////////////////////////////////////////////////////////

bool LTLExploration::iterateSCC(BuechiAutomata &automata,
		Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
		index_t currentAutomataState, AutomataTree* currentStateEntry, dfsnum_t depth, index_t currentNextDF) {
	// all elements in current SCC have DFS = -depth - 1 (those, which have not been visited by a SCC-search)
	// will set on all visited markings DFS = -depth -3


	//rep->message("===========================SS======================");
	//rep->message("START SEARCH %d %d %d %d (%d) %d", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState, depth);
	// current markings belongs to a new SCC
	bool forbidden = false;
	//rep->message("FB %d %d %d %d, %d", ns.Enabled[0], ns.Enabled[1], ns.Enabled[2], ns.Enabled[3], forbidden_transtitions[0]);
	for(index_t i = 0; i < card_forbidden_transtitions; i++)
		if (ns.Enabled[forbidden_transtitions[i]]){
			//Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			//Transition::revertEnabled(ns, currentFirelist[currentFirelistEntry]);
			forbidden = true;
			break;
		}
	if (!forbidden){
		//rep->message("===> @ BEGIN");
		if (searchFair(automata, store, firelist, ns, currentAutomataState, currentStateEntry,depth,currentNextDF)){
			//rep->message("========== SCC =========");
			return true;
		}
	}
	//rep->message("===========================NF======================");
	// if this SCC does not fulfill the property and the fairness assumption, search for non-fain-connected SCC's


	// the stack for the search
	SearchStack<LTLStackEntry> stack;
	// get first firelist
	index_t* currentFirelist;
	// the size of the list is not a valid index (thus -1)
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist)
			- 1;
	index_t* currentStateList;
	index_t currentStateListEntry;

	// pre-fire first transition
	Transition::fire(ns, currentFirelist[currentFirelistEntry]);
	Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);
	// check current marking for property
	automata.updateProperties(ns,
			currentFirelist[currentFirelistEntry]);
	currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);

	while (true) // exit when trying to pop from empty stack
	{
		// calculate the next transition
		if (currentStateListEntry == 0) {
			// revert currently fired transitions
			while (true){
				Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
				Transition::revertEnabled(ns,
									currentFirelist[currentFirelistEntry]);
				// revert the petrinet
				automata.revertProperties(ns,
						currentFirelist[currentFirelistEntry]);
				delete[] currentStateList;
				currentFirelistEntry--;
				// if there is no next transition ext
				if (currentFirelistEntry == -1) break;
				// pre-fire transition
				Transition::fire(ns, currentFirelist[currentFirelistEntry]);
				Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);
				// check current marking for property
				automata.updateProperties(ns,
						currentFirelist[currentFirelistEntry]);
				// set available transitions in Buechi-Automata
				currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState)-1;
				if (currentStateListEntry != -1) break;
			}
		} else
			currentStateListEntry--;

		if (currentFirelistEntry != -1 && currentStateListEntry >= 0) {
			// there is a next transition that needs to be explored in current marking

			// fire this transition to produce new Marking::Current
			//Transition::fire(ns, currentFirelist[currentFirelistEntry]);

			// search in the store
			AutomataTree* searchResult;
			store.searchAndInsert(ns, &searchResult, 0);
			AutomataTree* nextStateEntry = 0;
			// search state in state-tree
			searchAndInsertAutomataState(
					currentStateList[currentStateListEntry], &searchResult,
					&nextStateEntry);

			// greater means, that the component is marking is 'outside' of this component
			if (nextStateEntry->dfs <= - depth - 1 && nextStateEntry->dfs != -depth - 3){
				//Transition::updateEnabled(ns,currentFirelist[currentFirelistEntry]);

				// switch to next state
				currentAutomataState = currentStateList[currentStateListEntry];
				// State does not exist!
				// check current marking for property
				//automata.updateProperties(ns,
				//		currentFirelist[currentFirelistEntry]);
				// possible to start new SCC
				if (nextStateEntry->dfs == -depth - 1){
					// check forbidden transitions
					forbidden = false;
					for(index_t i = 0; i < card_forbidden_transtitions; i++)
						if (ns.Enabled[forbidden_transtitions[i]]){
							forbidden = true;
							break;
						}
					if (!forbidden){
						//rep->message("===========================SS======================");
						//rep->message("START SEARCH %d %d %d %d (%d) %d", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState, depth);
						// found a fair example
						if (searchFair(automata, store, firelist, ns, currentAutomataState, nextStateEntry,depth,currentNextDF)){
							// produce witness path
							*(witness.push()) = currentFirelist[currentFirelistEntry];
							while (stack.StackPointer) {
								*(witness.push()) = stack.top().fl[stack.top().current_on_firelist];
								stack.top().~LTLStackEntry();
								stack.pop();
							}
							delete[] currentFirelist;
							delete[] currentStateList;
							//rep->message("========== SCC =========");
							return true;
						}
					} //else {
					//	rep->message("SCC forb");
					//	rep->message("FB %d %d %d %d, %d", ns.Enabled[0], ns.Enabled[1], ns.Enabled[2], ns.Enabled[3], forbidden_transtitions[0]);
					//}
				}
				// mark this state as visited
				nextStateEntry->dfs = -depth - 3;


				LTLStackEntry *stackEntry = stack.push();
				stackEntry = new (stackEntry) LTLStackEntry(currentFirelist,
						currentFirelistEntry, currentStateList,
						currentStateListEntry,
						-1, currentStateEntry);

				// get Firelist and successor states
				currentFirelistEntry = firelist.getFirelist(ns,
						&currentFirelist) - 1;
				currentStateEntry = nextStateEntry;


				if (currentFirelistEntry != -1){
					Transition::fire(ns, currentFirelist[currentFirelistEntry]);
					Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);
					// check current marking for property
					automata.updateProperties(ns,
							currentFirelist[currentFirelistEntry]);
					currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);
				} else {
					// firelist is empty, this node forms its own SCC, and is thus not important
					delete[] currentFirelist;
					LTLStackEntry & stackEntry = stack.top();
					currentFirelistEntry = stackEntry.current_on_firelist;
					currentFirelist = stackEntry.fl;
					currentStateListEntry = stackEntry.current_on_statelist;
					currentStateList = stackEntry.states;
					currentStateEntry = stackEntry.dfs;
					stack.pop();
				}

			}
		} else {
			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			// no example has been found
			if (stack.StackPointer == 0){
				//rep->message("========== NO SCC =========");
				return false;
			}

			// load the own predecessor
			LTLStackEntry & stackEntry = stack.top();
			currentFirelistEntry = stackEntry.current_on_firelist;
			currentFirelist = stackEntry.fl;
			currentStateListEntry = stackEntry.current_on_statelist;
			currentStateList = stackEntry.states;
			currentStateEntry = stackEntry.dfs;
			stack.pop();

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the petrinet
			//Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			//Transition::revertEnabled(ns,
			//		currentFirelist[currentFirelistEntry]);
			//automata.revertProperties(ns,
			//		currentFirelist[currentFirelistEntry]);
			// revert the buechi automata
			currentAutomataState = currentStateList[currentStateListEntry];
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//
//           LTL-Fairness-Checker
//
//
///////////////////////////////////////////////////////////////////////////////
bool LTLExploration::searchFair(BuechiAutomata &automata,
		Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
		index_t currentAutomataState, AutomataTree* currentStateEntry,
		dfsnum_t depth, index_t initialDFS) {

	//rep->message("=====================CALL===========================");
	//rep->message("current BEGIN: %d %d %d, %d %d %d, %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState);
	// at this point we assume, that all states being outside the current SCC
	// depth and DFS numbers
	// dfs numbers < initialDFS    -    unprocessed outside
	// dfs numbers > -depth        -    processed outside
	// dfs number == -depth        -    SCC to check fairness
	// dfs number == -depth-1      -    visited by initial DFS (checkFair)
	// dfs number>initialDFS<oo    -    on local tarjan stack
	// dfs number == -depth-2      -    fully processed local
	// dfs number == -depth-3      -    visited by iteration over SCC (search all SCS)


	// start the new Tarjan DFS from the point of the current marking (the checkFairness will have put us there)
	// the initial next DFS number is the current one of the outer search
	// the stack for the search
	SearchStack<LTLStackEntry> stack;
	// pseudo tarjan stack, for being able to mark states as "not on tarjan-stack"
	SearchStack<AutomataTree*> tarjanStack;

	/// current global dfs number
	index_t currentNextDFSNumber = initialDFS;

	// get first firelist
	index_t* currentFirelist;
	// the size of the list is not a valid index (thus -1)
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist) - 1;
	index_t* currentStateList;
	index_t currentStateListEntry;
	index_t currentLowlink = initialDFS;

	// pre-fire first transition
	if (currentFirelistEntry != -1)
		while (true){
			Transition::fire(ns, currentFirelist[currentFirelistEntry]);
			Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);
			//rep->message("check state %d %d %d, %d %d %d, %d", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6]);
			// check whether this transition goes into an non forbidden state
			bool forbidden = false;
			for(index_t i = 0; i < card_forbidden_transtitions; i++)
			if (ns.Enabled[forbidden_transtitions[i]]){
				forbidden = true; break;
			}
			if (!forbidden) break;
			Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			Transition::revertEnabled(ns,currentFirelist[currentFirelistEntry]);
			if (--currentFirelistEntry == -1) break;
		}
	else
		if (isAcceptingStateReachable(automata,currentAutomataState)){
			delete[] currentFirelist;
			return true;
		}

	if (currentFirelistEntry != -1){
		automata.updateProperties(ns,currentFirelist[currentFirelistEntry]);
		// set available transitions in Buechi-Automata
		currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);
	} else
		return false;

	//rep->message("FIRE %s", Net::Name[TR][currentFirelist[currentFirelistEntry]]);

	// set dfs number
	currentStateEntry->dfs = currentNextDFSNumber++;
	//rep->message("\t\t\t\tPointer %d %d %d %d (%d) = %d", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState, currentStateEntry);
	//rep->message("current state %d %d %d %d (%d) %d [%d,%d]", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState, depth, currentStateEntry->dfs,currentLowlink);

	while (true) // exit when trying to pop from empty stack
	{
		//rep->message("STAT %d %d %d", currentFirelistEntry, currentStateListEntry, currentStateListLength);
		get_next_transition(automata,ns,&currentStateListEntry,&currentFirelistEntry,currentFirelist,&currentStateList, currentAutomataState);

		//rep->message("STAT %d %d", currentFirelistEntry, currentStateListEntry);

		if (currentFirelistEntry != -1 && currentStateListEntry != -1) {
			// there is a next transition that needs to be explored in current marking

			// fire this transition to produce new Marking::Current

			//rep->message("current aftrF %d %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState);

			// search in the store
			AutomataTree* searchResult;
			AutomataTree* nextStateEntry;
			//bool newStateFound;
			if (!store.searchAndInsert(ns, &searchResult, 0)){
				//newStateFound = true;
				nextStateEntry = searchResult;
				new (nextStateEntry) AutomataTree(currentStateList[currentStateListEntry]);
				//rep->message("\t\t\t\tPointer %d %d %d %d (%d) = %d", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentStateList[currentStateListEntry], nextStateEntry);
			} else {
				searchAndInsertAutomataState(
									currentStateList[currentStateListEntry], &searchResult,
									&nextStateEntry);
				//rep->message("\t\t\t\tPointer %d %d %d %d (%d) = %d", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentStateList[currentStateListEntry], nextStateEntry);
			}

			//rep->message("nSE->dfs = %d == %d",nextStateEntry->dfs,-depth-1);

			// unseen state
			if (nextStateEntry->dfs == -depth - 1) {

				//rep->message("NEW");
				// switch to next state
				currentAutomataState = currentStateList[currentStateListEntry];
				// State does not exist!

				LTLStackEntry *stackEntry = stack.push();
				stackEntry = new (stackEntry) LTLStackEntry(currentFirelist,
						currentFirelistEntry, currentStateList,
						currentStateListEntry,
						currentLowlink, currentStateEntry);

				// get Firelist and successor states
				currentFirelistEntry = firelist.getFirelist(ns,
						&currentFirelist) - 1;
				currentStateListEntry = automata.getSuccessors(
						&currentStateList, currentAutomataState);
				// set initial low-link number and local dfs
				currentLowlink = currentNextDFSNumber;
				// create a new automata state
				currentStateEntry = nextStateEntry;
				currentStateEntry->dfs = currentNextDFSNumber++;
				// pre fire transition
				if (currentFirelistEntry != -1){
					while (true){
						Transition::fire(ns, currentFirelist[currentFirelistEntry]);
						Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);
						// check whether this transition goes into an non forbidden state
						bool forbidden = false;
						for(index_t i = 0; i < card_forbidden_transtitions; i++)
						if (ns.Enabled[forbidden_transtitions[i]]){
							forbidden = true; break;
						}
						if (!forbidden) break;
						Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
						Transition::revertEnabled(ns,currentFirelist[currentFirelistEntry]);
						if (--currentFirelistEntry == -1) break;
					}
				} else if (isAcceptingStateReachable(automata,currentAutomataState)){
					// current marking has no successor and is in an accepting state
					// ==> finite counter example
					while (stack.StackPointer) {
						*(witness.push()) = stack.top().fl[stack.top().current_on_firelist];
						stack.top().~LTLStackEntry();
						stack.pop();
					}
					delete[] currentFirelist;
					//rep->message("=============== RET T ==============");
					return true;
				}


				if (currentFirelistEntry != -1) {
					// check current marking for property
					automata.updateProperties(ns, currentFirelist[currentFirelistEntry]);
					// set available transitions in Buechi-Automata
					currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);
				} else {
					// firelist is empty, this node forms its own SCC, and is thus not important
					delete[] currentFirelist;
					LTLStackEntry & stackEntry = stack.top();
					currentFirelistEntry = stackEntry.current_on_firelist;
					currentFirelist = stackEntry.fl;
					currentStateListEntry = stackEntry.current_on_statelist;
					currentStateList = stackEntry.states;
					currentLowlink = stackEntry.lowlink;
					currentStateEntry = stackEntry.dfs;
					stack.pop();
				}
			} else {
				// State exists! -->backtracking to previous state
				// if on tarjan-stack(indicated by lowlink == -depth - 2)
				// this is not necessary
				// if (nextStateEntry->dfs != -1)
				//rep->message("Adjust DFS from %d %d %d %d (%d) %d [%d,%d]", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState, depth,nextStateEntry->dfs,currentLowlink);
				if (nextStateEntry->dfs >= initialDFS
						&& currentLowlink > nextStateEntry->dfs)
					currentLowlink = nextStateEntry->dfs;
				//Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
				// Transition::revertEnabled(ns, currentFirelist[currentFirelistEntry]);
			}
		} else {
			// current marking has been completely processed

			// check for the end of the SCC
			if (currentLowlink == currentStateEntry->dfs) {
				// found the begin of an SCC, so check, whether it is an counter example
				bool foundcounterexample = automata.isAcceptingState(
						currentAutomataState);
				bool nonTrivial = tarjanStack.StackPointer
						&& tarjanStack.top()->dfs > currentStateEntry->dfs;



				if (!nonTrivial){
					// check whether a "trivial" SCC is indeed a cycle
					index_t* tempFireList;
					index_t cardTempFireList = firelist.getFirelist(ns, &tempFireList);
					for (index_t i = 0; i < cardTempFireList; i++)
						if (Transition::isCycle(ns,tempFireList[i])){
							nonTrivial = true;
							break;
						}
					delete[] tempFireList;
				}


				// make preparations if this is not the counter example
				//SearchStack<AutomataTree*> __back_stack;
				//AutomataTree** __back_entry = __back_stack.push();
				//*__back_entry = currentStateEntry;

				//rep->message("SCC FOUND %d",currentLowlink);
				//rep->message("current aftrF: %d %d %d, %d %d %d, %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState);
				// not found the counter example, so discard the component
				while (tarjanStack.StackPointer
						&& tarjanStack.top()->dfs > currentStateEntry->dfs) {
					//rep->message("+1");
					// mark all states as visited
					tarjanStack.top()->dfs = -currentNextDepth;
					// check whether is state is an accepting one
					foundcounterexample |= automata.isAcceptingState(
							tarjanStack.top()->state);
					tarjanStack.pop();
				}

				// remove current node from tarjan stack and mark it as already visited (once) for the fairness check
				// all markings in current SCC have DFS = -currentNextDepth
				// the initial marking shall have DFS = -currentNextDepth -1 (already visited)
				currentStateEntry->dfs = -currentNextDepth - 1;
				// if a counter example if found it must be inside a non trivial SCC, else it is none
				currentNextDepth += 4;
				if (foundcounterexample && nonTrivial) {
					// first we need to check all fairness assumptions via DFS
					bool* enabled_strong_fair;
					index_t checkResult = checkFairness(automata, store, firelist, ns, currentAutomataState, currentNextDepth-4, &enabled_strong_fair);
					//rep->message("CF %d", checkResult);

					// now all markings current SCC have DFS = -(currentNextDepth-3) - 1;
					if (checkResult == -1){
						// prepare data-structures for finding a witness path
						bool* fulfilledWeak = (bool*) calloc(assumptions.card_weak, SIZEOF_BOOL);
						bool* fulfilledStrong = (bool*) calloc(assumptions.card_strong, SIZEOF_BOOL);
						index_t fulfilled_conditions = 0;
						for (index_t i = 0; i < assumptions.card_strong; i++)
							if (!enabled_strong_fair[i]) fulfilled_conditions++, fulfilledStrong[i] = true;
						if (automata.isAcceptingState(currentAutomataState)) fulfilled_conditions++;
						// update weak conditions
						bool* __enabled_weak = (bool*) calloc(assumptions.card_weak, SIZEOF_BOOL);
						for (index_t i = 0; i <= ns.CardEnabled; i++)
							if (assumptions.weak_backlist[ns.Enabled[i]] != -1)
								__enabled_weak[assumptions.weak_backlist[ns.Enabled[i]]]++;
						// write not enabled weak back
						for (index_t i = 0; i < assumptions.card_weak; i++)
							if (!__enabled_weak[i])
								fulfilled_conditions++,fulfilledWeak[i] = true;
						// produce the witness path inside the SCC
						if (fulfilled_conditions == assumptions.card_strong + assumptions.card_weak + 1)
							completeWitness(automata,store,firelist,ns,currentAutomataState,currentStateEntry,currentNextDepth-4, currentNextDepth);
						else
							produceWitness(automata,store,firelist,ns,currentAutomataState,currentStateEntry,currentNextDepth-4,currentNextDepth,
								fulfilledWeak,fulfilledStrong,fulfilled_conditions,automata.isAcceptingState(currentAutomataState),currentStateEntry);

						// free needed datastructures
						free(fulfilledWeak);
						free(fulfilledStrong);
						delete[] currentFirelist;
						// add path from the initial marking to the SCC
						*(witness.push()) = -1;
						while (stack.StackPointer) {
							*(witness.push()) = stack.top().fl[stack.top().current_on_firelist];
							stack.top().~LTLStackEntry();
							stack.pop();
						}
						//rep->message("=============== RET T ==============");
						return true;
					}
					if (checkResult != -2){
						// if a strong fairness assumption is not fulfilled -> search smaller components
						//rep->message("FORBID %d %d",card_forbidden_transtitions, assumptions.strong_fairness[checkResult]);
						forbidden_transtitions[card_forbidden_transtitions++] = assumptions.strong_fairness[checkResult];

						// check for fairness via lichtenstein-pnueli
						if (iterateSCC(automata, store, firelist, ns,
								currentAutomataState, currentStateEntry, currentNextDepth - 4,
								currentNextDFSNumber)) {
							delete[] currentFirelist;
							while (stack.StackPointer) {
								*(witness.push()) = stack.top().fl[stack.top().current_on_firelist];
								stack.top().~LTLStackEntry();
								stack.pop();
							}
							//rep->message("============= RET T ==============");
							return true;
						}
						// the transition is not any more forbidden
						--card_forbidden_transtitions;
					}
				}
			} else {
				// push state onto tarjan stack
				// this stack contains all elements, which are on the "real" tarjan but not on the dfs stack
				AutomataTree** tarjanEntry = tarjanStack.push();
				*tarjanEntry = currentStateEntry;
			}

			// switch back to the previous state
			// save current lowlink number
			index_t oldstateLowLink = currentLowlink;

			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			if (stack.StackPointer == 0) {
				// have completely processed initial marking --> SCC not found
				// rep->message("=============RET F==============");
				return false;
			}

			// load the own predecessor
			LTLStackEntry & stackEntry = stack.top();
			currentFirelistEntry = stackEntry.current_on_firelist;
			currentFirelist = stackEntry.fl;
			currentStateListEntry = stackEntry.current_on_statelist;
			currentStateList = stackEntry.states;
			currentLowlink = stackEntry.lowlink;
			currentStateEntry = stackEntry.dfs;
			stack.pop();

			// adjust own lowlink
			if (oldstateLowLink < currentLowlink)
				currentLowlink = oldstateLowLink;

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the buechi automata
			currentAutomataState = currentStateList[currentStateListEntry];
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
//
//            LTL-Modelchecker
//
//
///////////////////////////////////////////////////////////////////////////////
bool LTLExploration::checkProperty(BuechiAutomata &automata,
		Store<AutomataTree> &store, Firelist &firelist, NetState &ns) {

	//rep->message("Names %s %s %s, %s %s %s, %s", Net::Name[PL][0],Net::Name[PL][1],Net::Name[PL][2],Net::Name[PL][3],Net::Name[PL][4],Net::Name[PL][5],Net::Name[PL][6]);

	// prepare strong fairness assumptions
	assumptions.card_strong = 0;
	for (index_t i = 0; i < Net::Card[TR]; i++)
		if (Transition::Fairness[i] == STRONG_FAIRNESS)
			assumptions.card_strong++;
	assumptions.strong_fairness = (index_t*) calloc(assumptions.card_strong,
			SIZEOF_INDEX_T);
	assumptions.strong_backlist = (index_t*) calloc(Net::Card[TR],
			SIZEOF_INDEX_T);
	// put all strong fair transitions into an array
	index_t __card_on_sf = 0;
	for (index_t i = 0; i < Net::Card[TR]; i++){
		//rep->message("Transition (%s) %d: Fairness %d", Net::Name[TR][i], i, Transition::Fairness[i]);
		if (Transition::Fairness[i] == STRONG_FAIRNESS) {
			assumptions.strong_fairness[__card_on_sf] = i;
			assumptions.strong_backlist[i] = __card_on_sf++;
		} else
			assumptions.strong_backlist[i] = -1;
	}

	// put all weak fair transitions into an array
	assumptions.card_weak = 0;
	for (index_t i = 0; i < Net::Card[TR]; i++)
		if (Transition::Fairness[i] == WEAK_FAIRNESS)
			assumptions.card_weak++;
	assumptions.weak_fairness = (index_t*) calloc(assumptions.card_weak,
			SIZEOF_INDEX_T);
	assumptions.weak_backlist = (index_t*) calloc(Net::Card[TR],
			SIZEOF_INDEX_T);
	index_t __card_on_wf = 0;
	for (index_t i = 0; i < Net::Card[TR]; i++){
		if (Transition::Fairness[i] == WEAK_FAIRNESS) {
			assumptions.weak_fairness[__card_on_wf] = i;
			assumptions.weak_backlist[i] = __card_on_wf++;
		} else
			assumptions.weak_backlist[i] = -1;
	}

	// prepare forbidden transtitions
	forbidden_transtitions = (index_t*) calloc(assumptions.card_strong, SIZEOF_BOOL);
	//rep->message("FORB %x",forbidden_transtitions);
	card_forbidden_transtitions = 0;

	// initialize the properties
	automata.initProperties(ns);

	/// current state of the buechi-automata
	index_t currentAutomataState = 0;

	/// current global dfs number
	index_t currentNextDFSNumber = 1;

	currentNextDepth = 4;

	// iterate over all possible initial states
	index_t* currentStateList;
	index_t stateListLenght = automata.getSuccessors(&currentStateList, currentAutomataState);

	bool result = false;
	/*for (index_t i = 0 ; i < stateListLenght; i++){
		// insert the initial transition and initial state of the buechi automata into the store
		AutomataTree* currentStateEntry;
		if (!store.searchAndInsert(ns, &currentStateEntry, 0)){
			new (currentStateEntry) AutomataTree(currentStateList[i]);
		} else {
			AutomataTree* nextStateEntry;
			searchAndInsertAutomataState(currentStateList[i], &currentStateEntry,&nextStateEntry);
			currentStateEntry = nextStateEntry;
		}

		// set dfs number
		currentStateEntry->dfs = currentNextDFSNumber;
		result = searchFair(automata, store,firelist,ns,currentStateList[i],currentStateEntry,currentNextDepth-4,currentNextDFSNumber);
		if (result) break;
	}*/


	AutomataTree* currentStateEntry;
	if (!store.searchAndInsert(ns, &currentStateEntry, 0)){
		new (currentStateEntry) AutomataTree(currentAutomataState);
	} else {
		AutomataTree* nextStateEntry;
		searchAndInsertAutomataState(currentAutomataState, &currentStateEntry,&nextStateEntry);
		currentStateEntry = nextStateEntry;
	}

	// set dfs number
	currentStateEntry->dfs = currentNextDFSNumber;
	result = searchFair(automata, store,firelist,ns,currentAutomataState,currentStateEntry,currentNextDepth-4,currentNextDFSNumber);


	// cleanup
	free(assumptions.strong_backlist);
	free(assumptions.strong_fairness);
	free(assumptions.weak_backlist);
	free(assumptions.weak_fairness);
	//rep->message("FORB %x",forbidden_transtitions);
	free(forbidden_transtitions);

	return result;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Construct a witness path
//		-> all states in current SCC have		DFS = -depth - 1
//
//	need to find a path which fulfills all weak fairness assumptions and all strong ones (not visiting forbidden ones will give them)
//
//	==>		witnessdepth = depth + 4
//
//	all visited states if a DFS have		 	DFS = -witnessdepth
//	the final one uses 							DFS = -depth - 2
//
//
//
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
//
//
//           LTL - produces the witness path
//				assumption: all markings of current component have DFS = -depth-1
//
///////////////////////////////////////////////////////////////////////////////

void LTLExploration::produceWitness(BuechiAutomata &automata,
		Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
		index_t currentAutomataState, AutomataTree* currentStateEntry,
		dfsnum_t depth, dfsnum_t witness_depth, bool* fulfilled_weak, bool* fulfilled_strong,
		index_t fulfilled_conditions, bool acceptingStateFound, AutomataTree* targetPointer) {

	//rep->message("==================== PW =================== %d", fulfilled_conditions);
	// the stack for the search
	SearchStack<LTLStackEntry> stack;
	// get first firelist
	index_t* currentFirelist;
	// the size of the list is not a valid index (thus -1)
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist) - 1;
	// this will be possible, as we are inside a non-trivial SCC
	get_first_transition(ns,&currentFirelistEntry,currentFirelist);
	//rep->message("current F: %d %d %d, %d %d %d, %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState);
	// check current marking for property
	automata.updateProperties(ns,currentFirelist[currentFirelistEntry]);
	// set available transitions in Buechi-Automata
	index_t* currentStateList;
	index_t currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);

	while (true) // exit when trying to pop from empty stack
	{
		// calculate the next transition
		get_next_transition(automata,ns,&currentStateListEntry,&currentFirelistEntry,currentFirelist,&currentStateList, currentAutomataState);

		// there is a next transition that needs to be explored in current marking
		if (currentFirelistEntry != -1 && currentStateListEntry >= 0) {
			// search in the store
			AutomataTree* searchResult;
			store.searchAndInsert(ns, &searchResult, 0);
			AutomataTree* nextStateEntry = 0;
			// search state in state-tree
			searchAndInsertAutomataState(currentStateList[currentStateListEntry], &searchResult,&nextStateEntry);

			//rep->message("Store: %d %d %d, %d %d %d, %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState);
			//rep->message("Store: %d %d", nextStateEntry->dfs, -depth - 1);

			// if state belongs to the current SCC
			if (nextStateEntry->dfs <= - depth - 1 && nextStateEntry->dfs >= witness_depth){

				bool newly_fulfilled = false;
				// maybe the transition itself has made us fair
				if (assumptions.weak_backlist[currentFirelist[currentFirelistEntry]] != -1) {
					// found a successor
					if (!fulfilled_weak[assumptions.weak_backlist[currentFirelist[currentFirelistEntry]]]) {
						fulfilled_conditions++; newly_fulfilled = true;
						//rep->message("FC Weak %d",currentFirelist[currentFirelistEntry]);
						fulfilled_weak[assumptions.weak_backlist[currentFirelist[currentFirelistEntry]]] = true;
					}
				} else if (assumptions.strong_backlist[currentFirelist[currentFirelistEntry]] != -1) {
					// strong, found a successor in set
					//rep->message("FC Strong %d %s",currentFirelist[currentFirelistEntry], Net::Name[TR][currentFirelist[currentFirelistEntry]]);
					//rep->message("FC Strong: %d %d %d, %d %d %d, %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState);
					fulfilled_strong[assumptions.strong_backlist[currentFirelist[currentFirelistEntry]]] = true;
					fulfilled_conditions++; newly_fulfilled = true;
				}

				// new to this search
				if (nextStateEntry->dfs == - depth - 1 || nextStateEntry->dfs != - witness_depth - 1){
					// check for possible new assumptions to be fulfilled
					bool* __enabled_weak = (bool*) calloc(assumptions.card_weak,SIZEOF_BOOL);
					for (index_t i = 0; i <= currentFirelistEntry; i++)
						if (assumptions.weak_backlist[currentFirelist[i]] != -1)
							__enabled_weak[assumptions.weak_backlist[currentFirelist[i]]] = true;
					// check for newly fulfilled conditions
					bool newly_fulfilled = false;
					for (index_t i = 0; i < assumptions.card_weak; i++)
						if (!__enabled_weak[i] && !fulfilled_weak[i]) {
								fulfilled_weak[i] = true;
								fulfilled_conditions++;
								newly_fulfilled = true;
							}
					free(__enabled_weak);
					if (automata.isAcceptingState(currentStateList[currentStateListEntry]) && !acceptingStateFound){
						acceptingStateFound = true; fulfilled_conditions++; newly_fulfilled = true;
					}
				}

				// more fairness assumptions are fulfilled
				if (newly_fulfilled){
					// if everything is fulfilled search for the begin of the SCC and finish
					if (fulfilled_conditions == assumptions.card_strong + assumptions.card_weak + 1){
						if (nextStateEntry != targetPointer)
							completeWitness(automata,store,firelist, ns,currentStateList[currentStateListEntry], targetPointer, depth,witness_depth);
					} else {
						// not all are fulfilled, but new ones
						produceWitness(automata,store,firelist, ns, currentStateList[currentStateListEntry], currentStateEntry, depth, witness_depth + 1,
							fulfilled_weak, fulfilled_strong, fulfilled_conditions, acceptingStateFound, targetPointer);
					}
					*(witness.push()) = currentFirelist[currentFirelistEntry];
					while (stack.StackPointer) {
						*(witness.push()) = stack.top().fl[stack.top().current_on_firelist];
						stack.top().~LTLStackEntry();
						stack.pop();
					}
					delete[] currentFirelist;
					delete[] currentStateList;
					return;
				}
				// nothing newly fulfilled
				if (nextStateEntry->dfs == - depth - 1 || nextStateEntry->dfs != - witness_depth){
					// switch to next state
					currentAutomataState = currentStateList[currentStateListEntry];
					// mark new state as visited
					nextStateEntry->dfs = -witness_depth;

					LTLStackEntry *stackEntry = stack.push();
					stackEntry = new (stackEntry) LTLStackEntry(currentFirelist,
							currentFirelistEntry, currentStateList,
							currentStateListEntry,
							-1, currentStateEntry);

					// get Firelist and successor states
					currentFirelistEntry = firelist.getFirelist(ns,&currentFirelist) - 1;
					get_first_transition(ns,&currentFirelistEntry,currentFirelist);

					// check current marking for property
					automata.updateProperties(ns,currentFirelist[currentFirelistEntry]);
					// set available transitions in Buechi-Automata
					currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);
					currentStateEntry = nextStateEntry;
				}
			}
		} else {
			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			// this will not happen
			assert(stack.StackPointer);

			// load the own predecessor
			LTLStackEntry & stackEntry = stack.top();
			currentFirelistEntry = stackEntry.current_on_firelist;
			currentFirelist = stackEntry.fl;
			currentStateListEntry = stackEntry.current_on_statelist;
			currentStateList = stackEntry.states;
			currentStateEntry = stackEntry.dfs;
			stack.pop();

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the buechi automata
			currentAutomataState = currentStateList[currentStateListEntry];
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//
//
//           LTL - produces the witness path
//				assumption: all markings of current component have DFS = -depth-1
//					except: on stack=> DFS = -depth-2, removed from stack DFS = -depth-3
///////////////////////////////////////////////////////////////////////////////

void LTLExploration::completeWitness(BuechiAutomata &automata,
		Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
		index_t currentAutomataState,  AutomataTree* targetPointer, dfsnum_t depth, dfsnum_t witness_depth) {

	//rep->message("==================== CW ===================");
	// the stack for the search
	SearchStack<LTLFairnessStackEntry> stack;
	// get first firelist
	index_t* currentFirelist;
	// the size of the list is not a valid index (thus -1)
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist) - 1;

	// pre fire first transition
	get_first_transition(ns,&currentFirelistEntry,currentFirelist);
	automata.updateProperties(ns,currentFirelist[currentFirelistEntry]);

	index_t* currentStateList;
	index_t currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);

	while (true) // exit when trying to pop from empty stack
	{
		get_next_transition(automata,ns,&currentStateListEntry,&currentFirelistEntry,currentFirelist,&currentStateList, currentAutomataState);

		if (currentFirelistEntry != -1 && currentStateListEntry >= 0) {
			// there is a next transition that needs to be explored in current marking

			// search in the store
			AutomataTree* searchResult;
			store.searchAndInsert(ns, &searchResult, 0);
			AutomataTree* nextStateEntry = 0;
			// search state in state-tree
			searchAndInsertAutomataState(currentStateList[currentStateListEntry], &searchResult, &nextStateEntry);
			if (nextStateEntry == targetPointer){
				// found the state we are looking for
				*(witness.push()) = currentFirelist[currentFirelistEntry];
				while (stack.StackPointer) {
					*(witness.push()) = stack.top().fl[stack.top().current_on_firelist];
					stack.top().~LTLFairnessStackEntry();
					stack.pop();
				}
				delete[] currentFirelist;
				delete[] currentStateList;
				return;
			}

			if (nextStateEntry->dfs == - depth - 1 || (nextStateEntry->dfs < - depth - 2 && nextStateEntry->dfs >= -witness_depth)){
				//rep->message("ENTER: %d %d %d, %d %d %d, %d (%d) %d", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], ns.Current[4], ns.Current[5], ns.Current[6], currentAutomataState, nextStateEntry->dfs);
				// switch to next state
				currentAutomataState = currentStateList[currentStateListEntry];
				// mark new state as visited
				nextStateEntry->dfs = -depth - 2;

				LTLFairnessStackEntry *stackEntry = stack.push();
				stackEntry = new (stackEntry) LTLFairnessStackEntry(currentFirelist,
						currentFirelistEntry, currentStateList,
						currentStateListEntry);

				// get Firelist and successor states
				currentFirelistEntry = firelist.getFirelist(ns,&currentFirelist) - 1;
				get_first_transition(ns,&currentFirelistEntry,currentFirelist);
				// check current marking for property
				automata.updateProperties(ns,currentFirelist[currentFirelistEntry]);
				// set available transitions in Buechi-Automata
				currentStateListEntry = automata.getSuccessors(&currentStateList, currentAutomataState);
			}
		} else {
			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			// this will not happen
			assert(stack.StackPointer);

			// load the own predecessor
			LTLFairnessStackEntry & stackEntry = stack.top();
			currentFirelistEntry = stackEntry.current_on_firelist;
			currentFirelist = stackEntry.fl;
			currentStateListEntry = stackEntry.current_on_statelist;
			currentStateList = stackEntry.states;
			stack.pop();

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the buechi automata
			currentAutomataState = currentStateList[currentStateListEntry];
		}
	}
}



bool LTLExploration::isAcceptingStateReachable(BuechiAutomata &automata,index_t currentAutomataState){
	if (automata.isAcceptingState(currentAutomataState))
		return true;

	bool* visitedStates = new bool[automata.cardStates];
	for(index_t i = 0; i < automata.cardStates; i++) visitedStates[i] = false;
	index_t* stateQueue = new index_t[automata.cardStates];
	index_t currentBegin = 0;
	index_t currentNext = 1;
	stateQueue[0] = currentAutomataState;

	while (currentBegin != currentNext){
		// explore current State
		index_t* nextStates;
		index_t cardNext = automata.getSuccessors(&nextStates, stateQueue[currentBegin++]);
		for (index_t i = 0; i < cardNext; i++)
			if (!visitedStates[nextStates[i]]){
				if (automata.isStateAccepting[nextStates[i]]) return true;
				visitedStates[nextStates[i]] = true;
				stateQueue[currentNext++] = nextStates[i];
			}
	}

	return false;
}
