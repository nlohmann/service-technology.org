
#pragma once


#include <Formula/CTL/AUFormula.h>
#include <Formula/CTL/TrueFormula.h>

struct AFFormula : public DFSFormula {
private:
	AUFormula auFormula;
	TrueFormula trueFormula;
public:
	CTLFormula* inner;

	void updateAtomics(NetState& ns, index_t t) {
		inner->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		inner->revertAtomics(ns,t);
	}

	// AF phi A (T U phi)
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist) {
		auFormula.phi = &trueFormula;
		auFormula.psi = inner;
		auFormula.index = index;
		auFormula.dfsindex = dfsindex;
		auFormula.payloadsize = payloadsize;
		return auFormula.check(s,ns,firelist);
	}
};
