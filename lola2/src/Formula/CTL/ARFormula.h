
#pragma once


#include <Formula/CTL/NotFormula.h>
#include <Formula/CTL/EUFormula.h>

struct ARFormula : public DFSFormula {
private:
	EUFormula euFormula;
	NotFormula notPhi;
	NotFormula notPsi;
public:
	CTLFormula* phi;
	CTLFormula* psi;

	void updateAtomics(NetState& ns, index_t t) {
		phi->updateAtomics(ns,t);
		psi->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		phi->revertAtomics(ns,t);
		psi->revertAtomics(ns,t);
	}

	// A (phi R psi) = !E (!phi U !psi)
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		notPhi.inner = phi;
		notPsi.inner = psi;
		euFormula.phi = &notPhi;
		euFormula.psi = &notPsi;
		euFormula.index = index;
		euFormula.dfsindex = dfsindex;
		euFormula.payloadsize = payloadsize;
		return !euFormula.check(s,ns,firelist,witness);
	}
};
