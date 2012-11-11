/*!
\file BuechiAutomata.h
\author Gregor
\status new

\brief a buechi class for the description of an buechi automata
*/

#pragma once

#include <set>
#include <Core/Dimensions.h>
#include <Net/NetState.h>
#include <Exploration/StatePredicateProperty.h>


/// A Büchi-Automata
class BuechiAutomata
{
public:

	/// the number of the current state
	uint32_t currentState;

	/// returns an array with all successor-states of the current netstate
	/// this function will also call automatically updateProperty
	int* getSuccessors(NetState &ns, index_t transition);

	/// reverts all Properties
	void revertProperties(NetState &ns, index_t transition);

	/// initialize all atomic properties
	void initProperties(NetState &ns);

private:
	/// number of atomic propositions
	int cardAtomicPropositions;

	/// an array containing the atomic propositions used by this buechi automata
	StatePredicateProperty* atomicPropositions;

	/// accepting set
	std::set<uint32_t>* acceptingsets;
};
