
#include <Exploration/CTLExploration.h>

bool CTLExploration::checkProperty(CTLFormula* formula, Store<void*> &store,
			Firelist &firelist, NetState &ns) {

	index_t numDFS = 0;
	index_t numCachedResults = 0;

	formula->gatherPayloadInformation(&numDFS,&numCachedResults);

	index_t cachedResultOffset = numDFS * sizeof(statenumber_t);
	size_t payloadSize = cachedResultOffset + (numCachedResults*2 + 7)/8;

	rep->status("CTL formula contains %d significant temporal operators and needs %d bytes of payload",numDFS,payloadSize);

	formula->setPayloadInformation(cachedResultOffset,payloadSize);

	// debug output
	printf("Formula: ");
	formula->DEBUG_print();
	printf("\n");

	witness.clear();
	return formula->check(store,ns,firelist,&witness);
}


