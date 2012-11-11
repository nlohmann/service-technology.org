#include <Formula/BuechiAutomata.h>
#include <cstdlib>


void BuechiAutomata::revertProperties(NetState &ns)
{

}

int* BuechiAutomata::getSuccessors(NetState &ns, index_t transition){
	for (int i = 0; i < cardAtomicPropositions;i++)
			atomicPropositions[i].checkProperty(ns,transition);
	// determine all possible successor-states
	return 0;
}

void BuechiAutomata::initProperties(NetState &ns){
	for (int i = 0; i < cardAtomicPropositions;i++)
		atomicPropositions[i].initProperty(ns);
}

void BuechiAutomata::revertProperties(NetState &ns, index_t transition){
	for (int i = 0; i < cardAtomicPropositions;i++)
		atomicPropositions[i].updateProperty(ns,transition);
}
