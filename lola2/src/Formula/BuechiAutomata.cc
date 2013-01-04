#include <Formula/BuechiAutomata.h>
#include <cstdlib>
#include <vector>

int BuechiAutomata::getSuccessors(NetState &ns, index_t** list,
		index_t currentState) {
	index_t cardTransitionList = cardTransitions[currentState];
	uint32_t** transitionsList = transitions[currentState];
	// TODO this is only temporarily, will be changed to an efficient version later
	std::vector<index_t> succ;
	for (int i = 0; i < cardTransitionList; i++){
		//rep->message("checking %d (%d) -> %d",currentState, atomicPropositions[transitionsList[i][0]]->getPredicate()->value,transitionsList[i][1]);
		if (atomicPropositions[transitionsList[i][0]]->getPredicate()->value)
			succ.push_back(transitionsList[i][1]);
	}
	*list = new index_t[succ.size()];
	for (int i = 0; i < succ.size(); i++)
		(*list)[i] = succ[i];
	return succ.size();
}

void BuechiAutomata::updateProperties(NetState &ns, index_t transition) {
	for (int i = 0; i < cardAtomicPropositions; i++)
		atomicPropositions[i]->checkProperty(ns, transition);
}

void BuechiAutomata::initProperties(NetState &ns) {
	for (int i = 0; i < cardAtomicPropositions; i++){
		atomicPropositions[i]->initProperty(ns);
	}
}

void BuechiAutomata::revertProperties(NetState &ns, index_t transition) {
	for (int i = 0; i < cardAtomicPropositions; i++)
		atomicPropositions[i]->updateProperty(ns, transition);
}

bool BuechiAutomata::isAcceptingState(index_t state){
	return isStateAccepting[state];
}

BuechiAutomata::~BuechiAutomata(){
	for (index_t i = 0; i < cardAtomicPropositions; i++){
		delete atomicPropositions[i]->getPredicate();
		delete atomicPropositions[i];
	}
	free(atomicPropositions);

	for (index_t i = 0; i < cardStates; i++){
		for (index_t j = 0; j < cardTransitions[i]; j++)
			free(transitions[i][j]);
		free(transitions[i]);
	}
	free(transitions);

	free(cardTransitions);
	free(isStateAccepting);
}
