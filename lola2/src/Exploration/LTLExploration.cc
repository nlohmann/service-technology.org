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

bool checkProperty(BuechiAutomata &automata, Store<AutomataTree> &store,
		Firelist &firelist, NetState &ns) {

	// the stack for the search
	SearchStack<LTLStackEntry> stack;

	// initialize the properties
	automata.initProperties(ns);

	/// current state of the buechi-automata
	index_t currentAutomataState = 42;

	/// current global dfs number
	index_t currentDFSNumber = 0;

	// get first firelist
	index_t* currentFirelist;
	index_t currentFirelistEntry = firelist.getFirelist(ns, &currentFirelist);
	index_t* currentStateList;
	index_t currentStateListEntry = automata.getSuccessors(ns,
			&currentStateList, currentAutomataState);
	index_t currentStateListLength = currentStateListEntry;
	AutomataTree* currentStateEntry;

	// insert the initial transition and initial state of the buechi automata into the store
	store.searchAndInsert(ns, &currentStateEntry, 0);
	searchAndInsertAutomataState(currentAutomataState, &currentStateEntry,
			&currentStateEntry);

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
			store.searchAndInsert(ns, &searchResult, 0);

			AutomataTree* nextStateEntry;
			// TODO handle Buechi-Automata states correctly
			if (searchAndInsertAutomataState(currentAutomataState,
					&searchResult, &nextStateEntry)) {
				// TODO if on tarjan-stack
				if (true)
					if (nextStateEntry->lowlink < currentStateEntry->lowlink)
						currentStateEntry->lowlink = nextStateEntry->dfs;

				// State exists! -->backtracking to previous state
				Transition::backfire(ns, currentFirelist[currentFirelistEntry]);
			} else {
				// switch to next state
				currentAutomataState += currentStateList[currentStateListEntry];
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
						currentStateEntry);

				// get Firelist and successor states
				currentFirelistEntry = firelist.getFirelist(ns,
						&currentFirelist);
				currentStateListEntry = automata.getSuccessors(ns,
						&currentStateList, currentAutomataState);
				currentStateListLength = currentStateListEntry;
				// create a new automata state
				currentStateEntry = nextStateEntry;
				currentStateEntry->dfs = currentDFSNumber++;
				currentStateEntry->lowlink = currentStateEntry->dfs;
			} // end else branch for "if state exists"
		} else {
			// if dfs == lowlink, we have found an scc, and have to analyse it
			if (currentStateEntry->dfs == currentStateEntry->lowlink){
				// detected SCC
			}




			// firing list completed -->close state and return to previous state
			delete[] currentFirelist;
			delete[] currentStateList;
			if (stack.StackPointer == 0) {
				// have completely processed initial marking --> state not found
				return false;
			}
			// save current lowlink number
			index_t nextStateLowlinkNumber = currentStateEntry->lowlink;

			LTLStackEntry & stackEntry = stack.top();
			currentFirelistEntry = stackEntry.current_on_firelist;
			currentFirelist = stackEntry.fl;
			currentStateListEntry = stackEntry.current_on_statelist;
			currentStateList = stackEntry.states;
			currentStateListLength = stackEntry.length_of_statelists;
			currentStateEntry = stackEntry.dfs_lowlink;
			stack.pop();

			// adjust own lowlink
			if (nextStateLowlinkNumber < currentStateEntry->lowlink)
				currentStateEntry->lowlink = nextStateLowlinkNumber;

			assert(currentFirelistEntry < Net::Card[TR]);
			// revert the petrinet
			Transition::backfire(ns, currentFirelist[currentStateListEntry]);
			Transition::revertEnabled(ns,
					currentFirelist[currentStateListEntry]);
			// revert the buechi automata
			currentAutomataState -= currentStateList[currentStateListEntry];
		}
	}
}
