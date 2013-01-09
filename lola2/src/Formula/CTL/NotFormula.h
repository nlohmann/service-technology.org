
#pragma once


#include <Formula/CTL/CTLFormula.h>

struct NotFormula : public CTLFormula {
	CTLFormula* inner;

	void initAtomics(NetState& ns) {
		inner->initAtomics(ns);
	}
	void updateAtomics(NetState& ns, index_t t) {
		inner->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		inner->revertAtomics(ns,t);
	}

	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		return !inner->check(s,ns,firelist, witness);
	}


	void DEBUG_print() {
		printf("NOT(");
		inner->DEBUG_print();
		printf(")");
	}

	void gatherPayloadInformation(index_t* numDFS, index_t* numCachedResults) {
		inner->gatherPayloadInformation(numDFS,numCachedResults);
	}
	void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize) {
		inner->setPayloadInformation(cachedResultOffset,payloadSize);
	}
};
