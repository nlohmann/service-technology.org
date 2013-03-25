/*!
 \file LTLExploration.h
 \author Gregor
 \status new

 \brief Evaluates an LTL Property
 */

#pragma once

#include <config.h>
#include <Exploration/SearchStack.h>
#include <Exploration/ChooseTransition.h>
#include <Exploration/SimpleProperty.h>
#include <Exploration/LTLStack.h>
#include <Net/NetState.h>
#include <Exploration/Firelist.h>
#include <Stores/Store.h>
#include <Formula/LTL/BuechiAutomata.h>

/// a simpe struct for a tree used as payload of the store

const int DFS_NUMBERS_PER_PNUELI = 4;
const int DFS_INITIAL_INVALID_NUMBER = -2;

// the memory layout will be exactly fixed, so that we can do some dirty
// for correctness see stuff http://stackoverflow.com/questions/7793820/struct-members-memory-layout
struct AutomataTree {
public:
	index_t state;
	// lowlink can be maintained locally
	// uint64_t lowlink;
	dfsnum_t dfs;
	index_t* firelist; // store the firelist in every node seems to be necessary, as the partial order reduction can differ between states with same petri but different buechi configuration
	index_t cardFirelist; // TODO really???
	AutomataTree* smaller;
	AutomataTree* bigger;

	AutomataTree(){smaller = bigger = 0; firelist = NULL; dfs = DFS_INITIAL_INVALID_NUMBER;}; // -2 unvisited by outermost search
	AutomataTree(index_t _state,bool tree) {if (tree) smaller = bigger = 0; firelist = NULL; state = _state; dfs = -2;};

	~AutomataTree(){
		if (smaller) delete smaller;
		if (bigger) delete bigger;
	}
}__attribute__((packed));


struct fairness_assumptions {
	index_t card_strong;
	index_t* strong_fairness;
	index_t* strong_backlist;
	index_t card_weak;
	index_t* weak_fairness;
	index_t* weak_backlist;
};

class LTLExploration {
public:
	LTLExploration(bool mode);

	~LTLExploration();

	bool checkProperty(BuechiAutomata &automata, Store<AutomataTree*> &store,
			Firelist &firelist, NetState &ns);

	SearchStack<index_t> witness;

private:

	fairness_assumptions assumptions;
	index_t* forbidden_transitions;
	bool* is_forbidden;
	index_t card_forbidden_transitions;

	/// storage mode for automata trees, true means tree storage, false means flat storage
	bool stateStorageMode;

	dfsnum_t currentNextDepth;

	bool searchFair(BuechiAutomata &automata, Store<AutomataTree*> &store,
			Firelist &firelist, NetState &ns, index_t currentAutomataState, AutomataTree* currentEntry,
			dfsnum_t depth, index_t currentNextDFS);
	index_t checkFairness(BuechiAutomata &automata, Store<AutomataTree*> &store,
			Firelist &firelist, NetState &ns, index_t currentAutomataState,
			dfsnum_t depth, bool** enabledStrongTransitions,AutomataTree* currentStateEntry);
	void produceWitness(BuechiAutomata &automata,
			Store<AutomataTree*> &store, Firelist &firelist, NetState &ns,
			index_t currentAutomataState, AutomataTree* currentStateEntry,
			dfsnum_t depth, dfsnum_t witness_depth, bool* fulfilledWeak, bool* fulfilledStrong,
			index_t fulfilled_conditions, bool acceptingStateFound, AutomataTree* targetPointer);

	bool start_restricted_searches(BuechiAutomata &automata,
			Store<AutomataTree*> &store, Firelist &firelist, NetState &ns,
			index_t currentAutomataState, AutomataTree* currentStateEntry, dfsnum_t depth, index_t currentNextDF);

	void completeWitness(BuechiAutomata &automata,
			Store<AutomataTree*> &store, Firelist &firelist, NetState &ns,
			index_t currentAutomataState,  AutomataTree* targetPointer, dfsnum_t depth, dfsnum_t witness_depth,AutomataTree* currentStateEntry);

	// helper functions
	bool is_next_state_accepting(BuechiAutomata &automata,index_t currentAutomataState);
	inline void get_next_transition(BuechiAutomata &automata, NetState &ns,
			index_t* currentStateListEntry, index_t* currentFirelistEntry, index_t* currentFirelist,
			index_t stateListLength,index_t currentAutomataState);
	inline bool initialize_transition(NetState &ns, Firelist &firelist,BuechiAutomata &automata, index_t currentAutomataState,
			index_t* currentFirelistEntry, index_t** currentFirelist,index_t* currentStateListEntry,
			index_t* currentStateListLength, index_t** currentStateList, AutomataTree* currentStateEntry);
};
