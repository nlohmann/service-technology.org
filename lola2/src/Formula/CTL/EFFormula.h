
#pragma once


#include <Formula/CTL/EUFormula.h>
#include <Formula/CTL/TrueFormula.h>

struct EFFormula : public DFSFormula {
private:
	EUFormula euFormula;
	TrueFormula trueFormula;
public:
	CTLFormula* inner;

	void updateAtomics(NetState& ns, index_t t) {
		inner->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		inner->revertAtomics(ns,t);
	}

	// EF phi  = E (T U phi)
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		euFormula.phi = &trueFormula;
		euFormula.psi = inner;
		euFormula.index = index;
		euFormula.dfsindex = dfsindex;
		euFormula.payloadsize = payloadsize;
		return euFormula.check(s,ns,firelist,witness);
	}

	void DEBUG_print() {
		printf("[%u,%u,%lu]EF(",index,dfsindex,payloadsize);
		inner->DEBUG_print();
		printf(")");
	}

	void gatherPayloadInformation(index_t* numDFS, index_t* numCachedResults) {
		dfsindex = ((*numDFS)++) * sizeof(statenumber_t);
		index = ((*numCachedResults)++) * 2;
		inner->gatherPayloadInformation(numDFS,numCachedResults);
	}
	void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize) {
		index += cachedResultOffset * 8;
		payloadsize = payloadSize;
		inner->setPayloadInformation(cachedResultOffset,payloadSize);
	}
};
