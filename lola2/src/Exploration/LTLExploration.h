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
class AutomataTree {
public:
	index_t state;
	// lowlink can be maintained locally
//	uint64_t lowlink;
	dfsnum_t dfs;
	AutomataTree* smaller;
	AutomataTree* bigger;

	AutomataTree(){smaller = 0; bigger = 0; dfs = -2;}; // -2 unvisited by outermost search
	AutomataTree(index_t _state) { smaller = 0; bigger = 0; state = _state; dfs = -2;};

	~AutomataTree(){
		if (smaller) delete smaller;
		if (bigger) delete bigger;
	}
};


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
	LTLExploration();

	~LTLExploration();

	bool checkProperty(BuechiAutomata &automata, Store<AutomataTree> &store,
			Firelist &firelist, NetState &ns);

	SearchStack<index_t> witness;

private:

	fairness_assumptions assumptions;
	index_t* forbidden_transitions;
	bool* is_forbidden;
	index_t card_forbidden_transitions;

	dfsnum_t currentNextDepth;

	bool searchFair(BuechiAutomata &automata, Store<AutomataTree> &store,
			Firelist &firelist, NetState &ns, index_t currentAutomataState, AutomataTree* currentEntry,
			dfsnum_t depth, index_t currentNextDFS);
	index_t checkFairness(BuechiAutomata &automata, Store<AutomataTree> &store,
			Firelist &firelist, NetState &ns, index_t currentAutomataState,
			dfsnum_t depth, bool** enabledStrongTransitions);
	void produceWitness(BuechiAutomata &automata,
			Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
			index_t currentAutomataState, AutomataTree* currentStateEntry,
			dfsnum_t depth, dfsnum_t witness_depth, bool* fulfilledWeak, bool* fulfilledStrong,
			index_t fulfilled_conditions, bool acceptingStateFound, AutomataTree* targetPointer);

	bool iterateSCC(BuechiAutomata &automata,
			Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
			index_t currentAutomataState, AutomataTree* currentStateEntry, dfsnum_t depth, index_t currentNextDF);

	void completeWitness(BuechiAutomata &automata,
			Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
			index_t currentAutomataState,  AutomataTree* targetPointer, dfsnum_t depth, dfsnum_t witness_depth);

	// helper functions
	bool isAcceptingLoopReachable(BuechiAutomata &automata,index_t currentAutomataState);
	bool isStateReachable(BuechiAutomata &automata,index_t currentAutomataState, index_t targetState);
	inline void get_next_transition(BuechiAutomata &automata, NetState &ns,
			index_t* currentStateListEntry, index_t* currentFirelistEntry, index_t* currentFirelist,
			index_t stateListLength,index_t currentAutomataState);
	inline bool get_first_transition(NetState &ns, Firelist &firelist,BuechiAutomata &automata, index_t currentAutomataState,
			index_t* currentFirelistEntry, index_t** currentFirelist,index_t* currentStateListEntry,
			index_t* currentStateListLength, index_t** currentStateList);
};
