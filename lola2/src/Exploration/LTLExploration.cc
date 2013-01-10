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

////////////////////////////////////////////////////////////////////////////////
//
//
//           LTL - checks the fairness assumptions and returns
//				-1 if all are fulfilled,   -2 if a weak is not fulfilled (or büchi-property is not true)
//				n (>= 0) - number of strong(on array), which is not fulfilled
///////////////////////////////////////////////////////////////////////////////

index_t LTLExploration::checkFairness(BuechiAutomata &automata,
		Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
		index_t currentAutomataState, dfsnum_t depth) {

	//rep->message("====================CF==================");
	// the stack for the search
	SearchStack<LTLFairnessStackEntry> stack;
	// get first firelist
	index_t* currentFirelist;
	// the size of the list is not a valid index (thus -1)
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist)
			- 1;

	index_t* currentStateList;
	index_t currentStateListEntry = automata.getSuccessors(ns,
			&currentStateList, currentAutomataState);
	index_t currentStateListLength = currentStateListEntry;

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
			__enabled_weak[assumptions.weak_backlist[currentFirelist[i]]] =
					true;
		else if (assumptions.strong_backlist[currentFirelist[i]] != -1)
			enabled_strong[i] = true;
	}
	// write not enabled weak back
	for (index_t i = 0; i < assumptions.card_weak; i++)
		if (!__enabled_weak[i]) {
			card_fulfilled_weak++;
			fulfilled_weak[i] = true;
		}

	while (true) // exit when trying to pop from empty stack
	{
		//rep->message("current state %d %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState);

		// calculate the next transition
		if (currentStateListEntry == 0) {
			currentStateListEntry = currentStateListLength - 1;
			currentFirelistEntry--;
		} else
			currentStateListEntry--;

		if (currentFirelistEntry != -1 && currentStateListEntry >= 0) {
			// there is a next transition that needs to be explored in current marking

			// fire this transition to produce new Marking::Current
			Transition::fire(ns, currentFirelist[currentFirelistEntry]);
			Transition::updateEnabled(ns,currentFirelist[currentFirelistEntry]);
			//rep->message("current aftrF %d %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState);

			// check forbidden transitions
			bool forbidden = false;
			for(index_t i = 0; i < card_forbidden_transtitions; i++)
				if (ns.Enabled[forbidden_transtitions[i]]){
					Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
					Transition::revertEnabled(ns, currentFirelist[currentFirelistEntry]);
					forbidden = true;
					break;
				}
			if (forbidden){
				//rep->message("Forbidden");
				continue;
			}

			// search in the store
			AutomataTree* searchResult;
			store.searchAndInsert(ns, &searchResult, 0);
			AutomataTree* nextStateEntry = 0;
			// search state in state-tree
			searchAndInsertAutomataState(
					currentStateList[currentStateListEntry], &searchResult,
					&nextStateEntry);

			//rep->message("nSE %d",nextStateEntry->dfs, -depth);

			if (nextStateEntry->dfs != -depth) {
				//rep->message("OLD");
				// state already found or does not belong to my zone
				if (nextStateEntry->dfs == -depth - 1) {
					// already found, maybe fairness
					if (assumptions.weak_backlist[currentFirelist[currentFirelistEntry]]
							!= -1) {
						// weak, found a successor in set
						if (!fulfilled_weak[assumptions.weak_backlist[currentFirelist[currentFirelistEntry]]]) {
							card_fulfilled_weak++;
							fulfilled_weak[assumptions.weak_backlist[currentFirelist[currentFirelistEntry]]] =
									true;
						}
					} else if (assumptions.strong_backlist[currentFirelist[currentFirelistEntry]]
							!= -1) {
						// strong, found a successor in set
						fulfilled_weak[assumptions.strong_backlist[currentFirelist[currentFirelistEntry]]] =
								true;
					}
				}
				Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
				Transition::revertEnabled(ns, currentFirelist[currentFirelistEntry]);
			} else {
				//rep->message("NEW");
				// switch to next state
				currentAutomataState = currentStateList[currentStateListEntry];
				// State does not exist!
				// check current marking for property
				automata.updateProperties(ns,
						currentFirelist[currentFirelistEntry]);

				// mark new state as visited
				//rep->message("s nSE %d",-depth - 1);
				nextStateEntry->dfs = -depth - 1;
				buechi_accepting_state |= automata.isAcceptingState(
						currentAutomataState);

				LTLFairnessStackEntry *stackEntry = stack.push();
				stackEntry = new (stackEntry) LTLFairnessStackEntry(
						currentFirelist, currentFirelistEntry, currentStateList,
						currentStateListEntry, currentStateListLength);

				// get Firelist and successor states
				currentFirelistEntry = firelist.getFirelist(ns,
						&currentFirelist) - 1;
				currentStateListEntry = automata.getSuccessors(ns,
						&currentStateList, currentAutomataState);
				currentStateListLength = currentStateListEntry;

				// fairness check for new state
				memset(__enabled_weak, 0, assumptions.card_weak * SIZEOF_BOOL);

				for (index_t i = 0; i <= currentFirelistEntry; i++) {
					if (assumptions.weak_backlist[currentFirelist[i]] != -1)
						__enabled_weak[assumptions.weak_backlist[currentFirelist[i]]] =
								true;
					else if (assumptions.strong_backlist[currentFirelist[i]]
							!= -1) {
						if (!fulfilled_strong[i])
							enabled_strong[i] = true;
					}
				}
				for (index_t i = 0; i < assumptions.card_weak; i++)
					if (!__enabled_weak[i]) {
						if (!fulfilled_weak[i]) {
							card_fulfilled_weak++;
							fulfilled_weak[i] = true;
						}
					}

			}
		} else {
			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			delete[] currentStateList;
			if (stack.StackPointer == 0) {

				free(fulfilled_strong);
				free(fulfilled_weak);
				free(enabled_strong);
				free(__enabled_weak);

				//rep->message("====================CF==================");
				// searched all states, calculate return value
				if (card_fulfilled_weak != assumptions.card_weak
						|| !buechi_accepting_state)
					return -2;
				for (index_t i = 0; i < assumptions.card_strong; i++)
					if (!fulfilled_strong[i] && enabled_strong[i])
						return i;
				// all fairness assumptions are fulfilled
				return -1;
			}

			// load the own predecessor
			LTLFairnessStackEntry & stackEntry = stack.top();
			currentFirelistEntry = stackEntry.current_on_firelist;
			currentFirelist = stackEntry.fl;
			currentStateListEntry = stackEntry.current_on_statelist;
			currentStateList = stackEntry.states;
			currentStateListLength = stackEntry.length_of_statelists;
			stack.pop();

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the petrinet
			Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			Transition::revertEnabled(ns,
					currentFirelist[currentFirelistEntry]);
			automata.revertProperties(ns,
					currentFirelist[currentFirelistEntry]);
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
	// at this point we assume, that all states being outside the current SCC
	// depth and DFS numbers
	// dfs numbers < initialDFS    -    unprocessed outside
	// dfs numbers > -depth        -    processed outside
	// dfs number == -depth        -    SCC to check fairness
	// dfs number == -depth-1      -    visited by initial DFS (checkFair)
	// dfs number>initialDFS<oo    -    on local tarjan stack
	// dfs number == -depth-2      -    fully processed local


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
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist)
			- 1;
	index_t* currentStateList;
	index_t currentStateListEntry = automata.getSuccessors(ns,
			&currentStateList, currentAutomataState);
	index_t currentStateListLength = currentStateListEntry;
	index_t currentLowlink = initialDFS;

	// set dfs number
	currentStateEntry->dfs = currentNextDFSNumber++;

	while (true) // exit when trying to pop from empty stack
	{
		//rep->message("current state %d %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState);
		// calculate the next transition
		if (currentStateListEntry == 0) {
			currentStateListEntry = currentStateListLength - 1;
			currentFirelistEntry--;
		} else
			currentStateListEntry--;

		if (currentFirelistEntry != -1 && currentStateListEntry >= 0) {
			// there is a next transition that needs to be explored in current marking

			// fire this transition to produce new Marking::Current
			Transition::fire(ns, currentFirelist[currentFirelistEntry]);
			//rep->message("current aftrF %d %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState);
			Transition::updateEnabled(ns, currentFirelist[currentFirelistEntry]);


			// check forbidden transitions
			bool forbidden = false;
			//rep->message("FB %d %d %d %d, %d", ns.Enabled[0], ns.Enabled[1], ns.Enabled[2], ns.Enabled[3], forbidden_transtitions[0]);
			for(index_t i = 0; i < card_forbidden_transtitions; i++)
				if (ns.Enabled[forbidden_transtitions[i]]){
					Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
					Transition::revertEnabled(ns, currentFirelist[currentFirelistEntry]);
					forbidden = true;
					break;
				}
			if (forbidden){
				//rep->message("Forbidden");
				continue;
			}

			// search in the store
			AutomataTree* searchResult;
			AutomataTree* nextStateEntry;
			//bool newStateFound;
			if (!store.searchAndInsert(ns, &searchResult, 0)){
				//newStateFound = true;
				nextStateEntry = searchResult;
				new (nextStateEntry) AutomataTree(currentStateList[currentStateListEntry]);
			} else {
				searchAndInsertAutomataState(
									currentStateList[currentStateListEntry], &searchResult,
									&nextStateEntry);
			}

			//rep->message("nSE->dfs = %d == %d",nextStateEntry->dfs,-depth-1);

			// unseen state
			if (nextStateEntry->dfs == -depth - 1) {
				//rep->message("NEW");
				// switch to next state
				currentAutomataState = currentStateList[currentStateListEntry];
				// State does not exist!
				// check current marking for property
				automata.updateProperties(ns,
						currentFirelist[currentFirelistEntry]);

				LTLStackEntry *stackEntry = stack.push();
				stackEntry = new (stackEntry) LTLStackEntry(currentFirelist,
						currentFirelistEntry, currentStateList,
						currentStateListEntry, currentStateListLength,
						currentLowlink, currentStateEntry);

				// get Firelist and successor states
				currentFirelistEntry = firelist.getFirelist(ns,
						&currentFirelist) - 1;
				currentStateListEntry = automata.getSuccessors(ns,
						&currentStateList, currentAutomataState);
				currentStateListLength = currentStateListEntry;
				// set initial low-link number and local dfs
				currentLowlink = currentNextDFSNumber;
				// create a new automata state
				currentStateEntry = nextStateEntry;
				currentStateEntry->dfs = currentNextDFSNumber++;
			} else {
				// State exists! -->backtracking to previous state

				// if on tarjan-stack(indicated by lowlink == -depth - 2)
				// this is not necessary
				// if (nextStateEntry->dfs != -1)
				if (nextStateEntry->dfs >= initialDFS
						&& currentLowlink > nextStateEntry->dfs)
					currentLowlink = nextStateEntry->dfs;
				Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
				Transition::revertEnabled(ns, currentFirelist[currentFirelistEntry]);
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

				// make preparations if this is not the counter example
				//SearchStack<AutomataTree*> __back_stack;
				//AutomataTree** __back_entry = __back_stack.push();
				//*__back_entry = currentStateEntry;

				//rep->message("SCC FOUND %d",currentLowlink);
				//rep->message("current aftrF %d %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], ns.Current[3], currentAutomataState);
				// not found the counter example, so discard the component
				while (tarjanStack.StackPointer
						&& tarjanStack.top()->dfs > currentStateEntry->dfs) {
					//rep->message("+1");
					// mark all states as visited
					tarjanStack.top()->dfs = -currentNextDepth;
					// check whether is state is an accepting one
					foundcounterexample |= automata.isAcceptingState(
							tarjanStack.top()->state);

					//__back_entry = __back_stack.push();
					//*__back_entry = tarjanStack.top();

					tarjanStack.pop();
				}
				// remove current node from tarjan stack and mark it as already visited (once) for the fairness check
				currentStateEntry->dfs = -currentNextDepth - 1;
				// if a counter example if found it must be inside a non trivial SCC, else it is none
				currentNextDepth += 3;
				if (foundcounterexample && nonTrivial) {
					// first we need to check all fairness assumptions via DFS
					index_t checkResult = checkFairness(automata, store, firelist, ns,
							currentAutomataState, currentNextDepth-3);
					//rep->message("CF %d",checkResult);
					if (checkResult == -1){
						//rep->message("=============RET T==============");
						return true;
					}
					if (checkResult != -2){
						// if a strong fairness assumption is not fulfilled -> search smaller components
						//rep->message("FORBID %d",assumptions.strong_backlist[checkResult]);
						forbidden_transtitions[card_forbidden_transtitions++] = assumptions.strong_backlist[checkResult];

						// check for fairness via lichtenstein-pnueli
						if (searchFair(automata, store, firelist, ns,
								currentAutomataState, currentStateEntry, currentNextDepth - 3,
								currentNextDFSNumber)) {
							delete[] currentFirelist;
							delete[] currentStateList;
							// TODO return counter example
							while (stack.StackPointer) {
								stack.top().~LTLStackEntry();
								stack.pop();
							}
							//rep->message("=============RET T==============");
							return true;
						}
						// the transition is not any more forbidden
						forbidden_transtitions[--card_forbidden_transtitions] = assumptions.strong_backlist[checkResult];
					}
				}
				// mark all elements as fully processed
				//while (__back_stack.StackPointer) {
				//	__back_stack.top()->dfs = -depth - 2;
				//	__back_stack.pop();
				//}
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
			delete[] currentStateList;
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
			currentStateListLength = stackEntry.length_of_statelists;
			currentLowlink = stackEntry.lowlink;
			currentStateEntry = stackEntry.dfs;
			stack.pop();

			// adjust own lowlink
			if (oldstateLowLink < currentLowlink)
				currentLowlink = oldstateLowLink;

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the petrinet
			Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			Transition::revertEnabled(ns,
					currentFirelist[currentFirelistEntry]);
			automata.revertProperties(ns,
					currentFirelist[currentFirelistEntry]);
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
		//rep->message("Transition %d: Fairness %d", i, Transition::Fairness[i]);
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
	for (index_t i = 0; i < Net::Card[TR]; i++)
		if (Transition::Fairness[i] == WEAK_FAIRNESS) {
			assumptions.weak_fairness[__card_on_wf] = i;
			assumptions.weak_backlist[i] = __card_on_wf++;
		} else
			assumptions.weak_backlist[i] = -1;

	// prepare forbidden transtitions
	forbidden_transtitions = (index_t*) calloc(assumptions.card_strong, SIZEOF_BOOL);
	card_forbidden_transtitions = 0;

	// initialize the properties
	automata.initProperties(ns);

	/// current state of the buechi-automata
	index_t currentAutomataState = 0;

	/// current global dfs number
	index_t currentNextDFSNumber = 1;

	currentNextDepth = 3;

	// get first firelist
	//index_t* currentFirelist;
	// the size of the list is not a valid index (thus -1)
	//index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist)
	//		- 1;
	//index_t* currentStateList;
	//index_t currentStateListEntry = automata.getSuccessors(ns,
	//		&currentStateList, currentAutomataState);
	//index_t currentStateListLength = currentStateListEntry;
	//index_t currentLowlink = 0;

//	rep->message("current params: FLL %d SLL %d AS %d", currentFirelistEntry, currentStateListEntry, currentAutomataState);

	// insert the initial transition and initial state of the buechi automata into the store
	AutomataTree* currentStateEntry;
	//rep->message("pointer %d",__temp_reader);
	store.searchAndInsert(ns, &currentStateEntry, 0);
	//rep->message("pointer %d",__temp_reader);
	new (currentStateEntry) AutomataTree(currentAutomataState);
	//rep->message("\t\t\t\t\t\tNEW STATE %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], currentStateList[currentStateListEntry]);

	// set dfs and lowlink number
	currentStateEntry->dfs = currentNextDFSNumber;

	return searchFair(automata, store,firelist,ns,currentAutomataState,currentStateEntry,currentNextDepth-3,currentNextDFSNumber);
}




// old version of the ltl-tarjan code
/*	// the stack for the search
	SearchStack<LTLStackEntry> stack;
	// pseudo tarjan stack, for being able to mark states as "not on tarjan-stack"
	SearchStack<AutomataTree*> tarjanStack;

	// initialize the properties
	automata.initProperties(ns);

	/// current state of the buechi-automata
	index_t currentAutomataState = 0;

	/// current global dfs number
	index_t currentNextDFSNumber = 0;

	// get first firelist
	index_t* currentFirelist;
	// the size of the list is not a valid index (thus -1)
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist)
			- 1;
	index_t* currentStateList;
	index_t currentStateListEntry = automata.getSuccessors(ns,
			&currentStateList, currentAutomataState);
	index_t currentStateListLength = currentStateListEntry;
	index_t currentLowlink = 0;

//	rep->message("current params: FLL %d SLL %d AS %d", currentFirelistEntry, currentStateListEntry, currentAutomataState);

	// insert the initial transition and initial state of the buechi automata into the store
	AutomataTree* currentStateEntry;
	//rep->message("pointer %d",__temp_reader);
	store.searchAndInsert(ns, &currentStateEntry, 0);
	//rep->message("pointer %d",__temp_reader);
	new (currentStateEntry) AutomataTree(currentAutomataState);
	//rep->message("\t\t\t\t\t\tNEW STATE %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], currentStateList[currentStateListEntry]);

	// set dfs and lowlink number
	currentStateEntry->dfs = currentNextDFSNumber;

	while (true) // exit when trying to pop from empty stack
	{
		//rep->message("current params: FLL %d SLL %d AS %d", currentFirelistEntry, currentStateListEntry, currentAutomataState);
		//rep->message("current state %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], currentAutomataState);
		// calculate the next transition
		if (currentStateListEntry == 0) {
			currentStateListEntry = currentStateListLength - 1;
			currentFirelistEntry--;
		} else
			currentStateListEntry--;

		if (currentFirelistEntry != -1 && currentStateListEntry >= 0) {
			//rep->message("Checking Transition %d(to %d) %d", currentStateListEntry, currentStateList[currentStateListEntry],currentFirelistEntry);
			// there is a next transition that needs to be explored in current marking

			// fire this transition to produce new Marking::Current
			Transition::fire(ns, currentFirelist[currentFirelistEntry]);
			//rep->message(" ==> %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], currentStateList[currentStateListEntry]);

			AutomataTree* searchResult;
			AutomataTree* nextStateEntry = 0;
			bool newStateFound;
			if (!store.searchAndInsert(ns, &searchResult, 0)){
				//rep->message("NEW");
				newStateFound = true;
				nextStateEntry = searchResult;
				new (nextStateEntry) AutomataTree(currentStateList[currentStateListEntry]);
			} else {
				newStateFound = !searchAndInsertAutomataState(
									currentStateList[currentStateListEntry], &searchResult,
									&nextStateEntry);
			}


			//rep->message("store %d",newStateFound);
			if (!newStateFound) {
				//rep->message("old one found");
				// if on tarjan-stack(indicated by lowlink == -1)
				// this is not necessary
				// if (nextStateEntry->dfs != -1)
				if (currentLowlink > nextStateEntry->dfs)
					currentLowlink = nextStateEntry->dfs;

				// State exists! -->backtracking to previous state
				Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			} else {
				//rep->message("\t\t\t\t\t\tNEW STATE %d %d %d (%d)", ns.Current[0], ns.Current[1], ns.Current[2], currentStateList[currentStateListEntry]);
				// switch to next state
				currentAutomataState = currentStateList[currentStateListEntry];
				// State does not exist!
				Transition::updateEnabled(ns,
						currentFirelist[currentFirelistEntry]);
				// check current marking for property
				automata.updateProperties(ns,
						currentFirelist[currentFirelistEntry]);

				LTLStackEntry *stackEntry = stack.push();
				stackEntry = new (stackEntry) LTLStackEntry(currentFirelist,
						currentFirelistEntry, currentStateList,
						currentStateListEntry, currentStateListLength,
						currentLowlink, currentStateEntry);

				// get Firelist and successor states
				currentFirelistEntry = firelist.getFirelist(ns,
						&currentFirelist) - 1;
				currentStateListEntry = automata.getSuccessors(ns,
						&currentStateList, currentAutomataState);
				currentStateListLength = currentStateListEntry;
				// set initial low-link number and local dfs
				currentLowlink = currentNextDFSNumber;
				// create a new automata state
				currentStateEntry = nextStateEntry;
				currentStateEntry->dfs = currentNextDFSNumber++;
			} // end else branch for "if state exists"
		} else {
			// current marking has been completely processed

			// check for the end of the SCC
			if (currentLowlink == currentStateEntry->dfs) {
				// found the begin of an SCC, so check, whether it is an counter example
				bool foundcounterexample = automata.isAcceptingState(
						currentAutomataState);
				bool nonTrivial = tarjanStack.StackPointer
						&& tarjanStack.top()->dfs > currentStateEntry->dfs;

				// make preparations if this is not the counter example
				SearchStack<AutomataTree*> __back_stack;
				AutomataTree** __back_entry = __back_stack.push();
				*__back_entry = currentStateEntry;

				// not found the counter example, so discard the component
				while (tarjanStack.StackPointer
						&& tarjanStack.top()->dfs > currentStateEntry->dfs) {
					// mark all states as visited
					tarjanStack.top()->dfs = -3;
					// check whether is state is an accepting one
					foundcounterexample |= automata.isAcceptingState(
							tarjanStack.top()->state);

					__back_entry = __back_stack.push();
					*__back_entry = tarjanStack.top();

					tarjanStack.pop();
				}
				// remove current node from tarjan stack and mark it as already visited (once) for the fairness check
				currentStateEntry->dfs = -4;
				// if a counter example if found it must be inside a non trivial SCC, else it is none
				if (foundcounterexample && nonTrivial) {
					// check for fairness via lichtenstein-pnueli
					if (isFair(automata, store, firelist, ns,
							currentAutomataState, currentStateEntry, 3,
							currentNextDFSNumber)) {
						delete[] currentFirelist;
						delete[] currentStateList;
						free(assumptions.strong_backlist);
						free(assumptions.strong_fairness);
						free(assumptions.weak_backlist);
						free(assumptions.weak_fairness);
						free(forbidden_transtitions);
						// TODO return counter example
						while (stack.StackPointer) {
							stack.top().~LTLStackEntry();
							stack.pop();
						}
						return true;
					}
				}
				// mark all elements as fully processed
				while (__back_stack.StackPointer) {
					__back_stack.top()->dfs = -1;
					__back_stack.pop();
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
			delete[] currentStateList;
			if (stack.StackPointer == 0) {
				// have completely processed initial marking --> SCC not found
				free(assumptions.strong_backlist);
				free(assumptions.strong_fairness);
				free(assumptions.weak_backlist);
				free(assumptions.weak_fairness);
				free(forbidden_transtitions);
				return false;
			}

			// load the own predecessor
			LTLStackEntry & stackEntry = stack.top();
			currentFirelistEntry = stackEntry.current_on_firelist;
			currentFirelist = stackEntry.fl;
			currentStateListEntry = stackEntry.current_on_statelist;
			currentStateList = stackEntry.states;
			currentStateListLength = stackEntry.length_of_statelists;
			currentLowlink = stackEntry.lowlink;
			currentStateEntry = stackEntry.dfs;
			stack.pop();

			// adjust own lowlink
			if (oldstateLowLink < currentLowlink)
				currentLowlink = oldstateLowLink;

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the petrinet
			Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			Transition::revertEnabled(ns,
					currentFirelist[currentFirelistEntry]);
			automata.revertProperties(ns,
					currentFirelist[currentFirelistEntry]);
			// revert the buechi automata
			currentAutomataState = currentStateList[currentStateListEntry];
		}
	}
*/
