
#pragma once


#include <Formula/CTL/CTLFormula.h>

struct DisjunctionFormula : public CTLFormula {
	CTLFormula** subs;
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
};
