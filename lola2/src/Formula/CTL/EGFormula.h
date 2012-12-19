
#pragma once


#include <Formula/CTL/AUFormula.h>
#include <Formula/CTL/NotFormula.h>
#include <Formula/CTL/TrueFormula.h>

struct EGFormula : public DFSFormula {
private:
	AUFormula auFormula;
	TrueFormula trueFormula;
	NotFormula notFormula;
public:
	CTLFormula* inner;

	void updateAtomics(NetState& ns, index_t t) {
		inner->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		inner->revertAtomics(ns,t);
	}

	// EG phi = !AF(!phi) = !A (T U !phi)
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		notFormula.inner = inner;
		auFormula.phi = &trueFormula;
		auFormula.psi = &notFormula;
		auFormula.index = index;
		auFormula.dfsindex = dfsindex;
		auFormula.payloadsize = payloadsize;
		return !auFormula.check(s,ns,firelist,witness);
	}
};
