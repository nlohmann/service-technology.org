
#pragma once


#include <Formula/CTL/EUFormula.h>
#include <Formula/CTL/NotFormula.h>
#include <Formula/CTL/TrueFormula.h>

struct AGFormula : public DFSFormula {
private:
	EUFormula euFormula;
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

	// AG phi = !EF(!phi) = !E (T U !phi)
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		notFormula.inner = &inner;
		euFormula.phi = &trueFormula;
		euFormula.psi = &notFormula;
		euFormula.index = index;
		euFormula.dfsindex = dfsindex;
		euFormula.payloadsize = payloadsize;
		return !euFormula.check(s,ns,firelist,witness);
	}
};
