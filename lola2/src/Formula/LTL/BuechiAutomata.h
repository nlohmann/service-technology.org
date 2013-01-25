/*!
\file BuechiAutomata.h
\author Gregor
\status new

\brief a buechi class for the description of an buechi automata
*/

#pragma once

#include <set>
#include <map>
#include <Core/Dimensions.h>
#include <Net/NetState.h>
#include <Exploration/StatePredicateProperty.h>

// datastructure for parsing buechi automatons from LTL formulae
extern std::map<int,StatePredicate*> predicateMap;
extern std::map<int,StatePredicate*> negpredicateMap;

char* produce_next_string(int* );

/// A Büchi-Automata
class BuechiAutomata
{
public:

	/// returns an array with all successor-states of the current netstate in the parameter
	/// this function will also call automatically updateProperty
	/// @return number of successors
	int getSuccessors(index_t** list, index_t currentState);

	/// updates all properties
	void updateProperties(NetState &ns, index_t transition);

	/// reverts all Properties
	void revertProperties(NetState &ns, index_t transition);

	/// initialize all atomic properties
	void initProperties(NetState &ns);

	/// returns true if the given state is accepting
	bool isAcceptingState(index_t state);

	/// default destructor
	~BuechiAutomata();

public:
	/// number of atomic propositions
	uint32_t cardAtomicPropositions;
	/// an array containing the atomic propositions used by this buechi automata
	StatePredicateProperty** atomicPropositions;
	/// contains for each atomic proposition the
	index_t* atomicPropotions_backlist;

	/// number of states
	uint32_t cardStates;
	/// number of possible transitions per automata state
	uint32_t* cardTransitions;
	/// the transition "matrix" -- first index=state, second=number of transition, third= 0->proposition 1->next state
	uint32_t*** transitions;
	/// current number of successor states
	index_t* cardEnabled;
	/// an array, indicating at each position i, whether the state i is an accepting one
	bool* isStateAccepting;
};
