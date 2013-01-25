#include <Formula/LTL/BuechiAutomata.h>
#include <cstdlib>
#include <vector>
#include <math.h>

int BuechiAutomata::getSuccessors(index_t** list,
		index_t currentState) {
	index_t cardTransitionList = cardTransitions[currentState];
	uint32_t** transitionsList = transitions[currentState];

	*list = new index_t[cardEnabled[currentState]];
	int curCard = 0;

	//rep->message("BEGIN");
	for (int i = 0; i < cardTransitionList; i++){
		//rep->message("checking %d (%d) -> %d",currentState, atomicPropositions[transitionsList[i][0]]->getPredicate()->value,transitionsList[i][1]);
		if (atomicPropositions[transitionsList[i][0]]->getPredicate()->value){
			//rep->message("List %d",transitionsList[i][1]);
			(*list)[curCard++] = transitionsList[i][1];
		}
	}
	//rep->message("END");
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
	for (int i = 0; i < cardAtomicPropositions; i++){
		//rep->message("INIT %d",i);
		if (atomicPropositions[i]->initProperty(ns)){

			cardEnabled[atomicPropotions_backlist[i]]++;
			//rep->message("TRUE %d",cardEnabled[atomicPropotions_backlist[i]]);
		} //else
		//	rep->message("FALSE");
	}
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


int current_next_string_index_number = 1;

char* produce_next_string(int* val){
	current_next_string_index_number++;
	int length = log10(current_next_string_index_number) + 2;
	char* buf = (char*) calloc(length, sizeof(char));
	sprintf(buf,"%d",current_next_string_index_number);
	*val = current_next_string_index_number;
	return buf;
}
