
#pragma once


#include <Formula/CTL/DFSFormula.h>
#include <Net/NetState.h>
#include <Exploration/Firelist.h>
#include <Stores/Store.h>

struct EUFormula : public DFSFormula {
	CTLFormula* phi;
	CTLFormula* psi; //phi until psi

	void updateAtomics(NetState& ns, index_t t) {
		phi->updateAtomics(ns,t);
		psi->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		phi->revertAtomics(ns,t);
		psi->revertAtomics(ns,t);
	}

	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness);


	void DEBUG_print() {
		printf("[%u,%u,%lu]E(",index,dfsindex,payloadsize);
		phi->DEBUG_print();
		printf(")U(");
		psi->DEBUG_print();
		printf(")");
	}

	void gatherPayloadInformation(index_t* numDFS, index_t* numCachedResults) {
		dfsindex = ((*numDFS)++) * sizeof(statenumber_t);
		index = ((*numCachedResults)++) * 2;
		phi->gatherPayloadInformation(numDFS,numCachedResults);
		psi->gatherPayloadInformation(numDFS,numCachedResults);
	}
	void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize) {
		index += cachedResultOffset * 8;
		payloadsize = payloadSize;
		phi->setPayloadInformation(cachedResultOffset,payloadSize);
		psi->setPayloadInformation(cachedResultOffset,payloadSize);
	}
};
