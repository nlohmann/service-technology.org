
#pragma once


#include <Formula/CTL/CTLFormula.h>

struct NotFormula : public CTLFormula {
	CTLFormula* inner;

	void updateAtomics(NetState& ns, index_t t) {
		inner->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		inner->revertAtomics(ns,t);
	}

	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		return !inner->check(s,ns,firelist, witness);
	}
};
