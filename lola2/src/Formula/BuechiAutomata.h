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


/// A Büchi-Automata
class BuechiAutomata
{
public:

	/// returns an array with all successor-states of the current netstate in the parameter
	/// this function will also call automatically updateProperty
	/// @return number of successors
	int getSuccessors(NetState &ns, index_t** list, index_t currentState);

	/// updates all properties
	void updateProperties(NetState &ns, index_t transition);

	/// reverts all Properties
	void revertProperties(NetState &ns, index_t transition);

	/// initialize all atomic properties
	void initProperties(NetState &ns);

	/// returns true if the given state is accepting
	bool isAcceptingState(index_t state);

private:
	/// number of atomic propositions
	int cardAtomicPropositions;

	/// an array containing the atomic propositions used by this buechi automata
	StatePredicateProperty* atomicPropositions;

public: //TODO make private + add constructors
  /// transition rules - maps a state + an atomic formula to another state
  std::map<std::pair<uint32_t,StatePredicate*>,uint32_t>* transitions;

	/// accepting set
	std::set<uint32_t>* acceptingset;
};
