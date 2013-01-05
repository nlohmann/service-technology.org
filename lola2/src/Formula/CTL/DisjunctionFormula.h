
#pragma once


#include <Formula/CTL/CTLFormula.h>

struct DisjunctionFormula : public CTLFormula {
	CTLFormula** subs; //array of pointer
	index_t cardSubs;

	void updateAtomics(NetState& ns, index_t t) {
		for(index_t i = 0; i<cardSubs; i++)
			subs[i]->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		for(index_t i = 0; i<cardSubs; i++)
			subs[i]->revertAtomics(ns,t);
	}

	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		for(index_t i = 0; i<cardSubs; i++)
			if(subs[i]->check(s,ns,firelist,witness))
				return true;
			else
				witness->clear();
		return false;
	}

	void DEBUG_print() {
		printf("OR(");
		for(index_t i=0;i<cardSubs;i++)
			subs[i]->DEBUG_print();
		printf(")");
	}

	void gatherPayloadInformation(index_t* numDFS, index_t* numCachedResults) {
		for(index_t i = 0; i < cardSubs; i++)
			subs[i]->gatherPayloadInformation(numDFS,numCachedResults);
	}
	void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize) {
		for(index_t i = 0; i < cardSubs; i++)
			subs[i]->setPayloadInformation(cachedResultOffset,payloadSize);
	}
};
