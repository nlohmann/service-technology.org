#include <Formula/BuechiAutomata.h>
#include <cstdlib>
#include <vector>

int BuechiAutomata::getSuccessors(NetState &ns, index_t** list,
		index_t currentState) {
	index_t cardTransitionList = cardTransitions[currentState];
	uint32_t** transitionsList = transitions[currentState];

	*list = new index_t[cardEnabled[currentState]];
	int curCard = 0;

	for (int i = 0; i < cardTransitionList; i++){
		//rep->message("checking %d (%d) -> %d",currentState, atomicPropositions[transitionsList[i][0]]->getPredicate()->value,transitionsList[i][1]);
		if (atomicPropositions[transitionsList[i][0]]->getPredicate()->value)
			(*list)[curCard++] = transitionsList[i][1];
	}
	return curCard;
}

void BuechiAutomata::updateProperties(NetState &ns, index_t transition) {
	for (int i = 0; i < cardStates; i++)
		cardEnabled[i] = 0;
	for (int i = 0; i < cardAtomicPropositions; i++)
		if(atomicPropositions[i]->checkProperty(ns, transition))
			cardEnabled[atomicPropotions_backlist[i]] ++;
}

void BuechiAutomata::initProperties(NetState &ns) {
	for (int i = 0; i < cardAtomicPropositions; i++)
		if (atomicPropositions[i]->initProperty(ns))
			cardEnabled[atomicPropotions_backlist[i]]++;
}

void BuechiAutomata::revertProperties(NetState &ns, index_t transition) {
	for (int i = 0; i < cardStates; i++)
		cardEnabled[i] = 0;
	for (int i = 0; i < cardAtomicPropositions; i++)
		if (atomicPropositions[i]->updateProperty(ns, transition))
			cardEnabled[atomicPropotions_backlist[i]] ++;
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
