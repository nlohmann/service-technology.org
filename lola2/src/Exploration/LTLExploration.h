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
#include <Formula/BuechiAutomata.h>

/// a simpe struct for a tree used as payload of the store
class AutomataTree {
public:
	index_t state;
	// lowlink can be maintained locally
//	uint64_t lowlink;
	dfsnum_t dfs;
	AutomataTree* smaller;
	AutomataTree* bigger;

	AutomataTree(){smaller = 0; bigger = 0; dfs = -1;};
	AutomataTree(index_t _state) { smaller = 0; bigger = 0; state = _state; dfs = -1;};

	~AutomataTree(){
		if (smaller) delete smaller;
		if (bigger) delete bigger;
	}
};


struct fairness_assumptions {
	index_t card_strong;
	//index_t* strong_fairness;
	index_t* strong_backlist;
	index_t card_weak;
	//index_t* weak_fairness;
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
	index_t* forbidden_transtitions;
	index_t card_forbidden_transtitions;

	dfsnum_t currentNextDepth;

	bool searchFair(BuechiAutomata &automata, Store<AutomataTree> &store,
			Firelist &firelist, NetState &ns, index_t currentAutomataState, AutomataTree* currentEntry,
			dfsnum_t depth, index_t currentNextDFS);
	index_t checkFairness(BuechiAutomata &automata, Store<AutomataTree> &store,
			Firelist &firelist, NetState &ns, index_t currentAutomataState,
			dfsnum_t depth);
	void produceWitness(BuechiAutomata &automata,
			Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
			index_t currentAutomataState, AutomataTree* currentStateEntry,  dfsnum_t depth);

	void completeWitness(BuechiAutomata &automata,
			Store<AutomataTree> &store, Firelist &firelist, NetState &ns,
			index_t currentAutomataState, dfsnum_t depth);
};
