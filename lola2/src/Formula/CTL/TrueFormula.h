
#pragma once


#include <Formula/CTL/CTLFormula.h>

struct TrueFormula : public CTLFormula {
	void updateAtomics(NetState& ns, index_t t) {}
	void revertAtomics(NetState& ns, index_t t) {}
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		return true;
	}

	void DEBUG_print() {
		printf("TRUE");
	}

	void initAtomics(NetState& ns) {
		// do nothing
	}
	void gatherPayloadInformation(index_t* numDFS, index_t* numCachedResults) {
		// do nothing
	}
	void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize) {
		// do nothing
	}
};
