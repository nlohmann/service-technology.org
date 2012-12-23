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
		if (*tree == 0) {
			*tree = new AutomataTree();
			(*tree)->state = automataState;
			(*tree)->bigger = 0;
			(*tree)->smaller = 0;
			return false;
		}
		if ((*tree)->state == automataState) {
			*result = *tree;
			return true;
		}
		if ((*tree)->state > automataState)
			*tree = (*tree)->smaller;
		else
			*tree = (*tree)->bigger;
	}
}

bool LTLExploration::checkProperty(BuechiAutomata &automata, Store<AutomataTree> &store,
		Firelist &firelist, NetState &ns) {

	// the stack for the search
	SearchStack<LTLStackEntry> stack;
	// pseudo tarjan stack, for being able to mark states as "not on tarjan-stack"
	SearchStack<AutomataTree*> tarjanStack;

	// initialize the properties
	automata.initProperties(ns);

	/// current state of the buechi-automata
	index_t currentAutomataState = 0;

	/// current global dfs number
	index_t currentNextDFSNumber = 0;

	/// found counterexample
	bool foundcounterexample = false;

	// get first firelist
	index_t* currentFirelist;
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist);
	index_t* currentStateList;
	index_t currentStateListEntry = automata.getSuccessors(ns,
			&currentStateList, currentAutomataState);
	index_t currentStateListLength = currentStateListEntry;
	index_t currentLowlink = 0;
	AutomataTree* currentStateEntry;

	// insert the initial transition and initial state of the buechi automata into the store
	store.searchAndInsert(ns, &currentStateEntry, 0);
	currentStateList = 0;
	searchAndInsertAutomataState(currentAutomataState, &currentStateEntry,
			&currentStateEntry);

	// set dfs and lowlink number
	currentStateEntry->dfs = currentNextDFSNumber;

	while (true) // exit when trying to pop from empty stack
	{
		// calculate the next transition
		if (currentStateListEntry == 0) {
			currentStateListEntry = currentStateListLength;
			currentFirelistEntry--;
		} else
			currentStateListEntry--;

		if (currentFirelistEntry >= 0 && currentStateListEntry >= 0) {
			// there is a next transition that needs to be explored in current marking

			// fire this transition to produce new Marking::Current
			Transition::fire(ns, currentFirelist[currentFirelistEntry]);

			AutomataTree* searchResult;
			if (!store.searchAndInsert(ns, &searchResult, 0))
				searchResult = 0;
			AutomataTree* nextStateEntry;

			if (searchAndInsertAutomataState(currentAutomataState,
					&searchResult, &nextStateEntry)) {
				// if on tarjan-stack(indicated by lowlink == -1)
				// this is not necessary
				// if (nextStateEntry->dfs != -1)
				if (currentLowlink > nextStateEntry->dfs)
					currentLowlink = nextStateEntry->dfs;

				// State exists! -->backtracking to previous state
				Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			} else {
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
						&currentFirelist);
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
			// make check for a possible counter example
			foundcounterexample |= automata.isAcceptingState(
					currentAutomataState);

			// check for the end of the SCC
			if (currentLowlink == currentStateEntry->dfs) {
				if (foundcounterexample) {
					// check for fairness via a new DFS!
					return true;
				}
				// not found the counter example, so discard the component
				while (tarjanStack.StackPointer && tarjanStack.top()->dfs > currentStateEntry->dfs){
					// mark all states as visited
					tarjanStack.top()->dfs = -1;
					tarjanStack.pop();
				}
			}
			// push state onto tarjan stack
			// this stack contains all elements, which are on the "real" tarjan but not on the dfs stack
			AutomataTree** tarjanEntry = tarjanStack.push();
			*tarjanEntry = currentStateEntry;



			// switch back to the previous state

			// save current lowlink number
			index_t oldstateLowLink = currentLowlink;

			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			delete[] currentStateList;
			if (stack.StackPointer == 0) {
				// have completely processed initial marking --> state not found
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
			Transition::backfire(ns, currentFirelist[currentStateListEntry]);
			Transition::revertEnabled(ns,
					currentFirelist[currentStateListEntry]);
			// revert the buechi automata
			currentAutomataState = currentStateList[currentStateListEntry];
		}
	}
}
