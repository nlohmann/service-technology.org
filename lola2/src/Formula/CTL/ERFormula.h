
#pragma once


#include <Formula/CTL/NotFormula.h>
#include <Formula/CTL/AUFormula.h>

struct ERFormula : public DFSFormula {
private:
	AUFormula auFormula;
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

	// E (phi R psi) = !A (!phi U !psi)
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		notPhi.inner = phi;
		notPsi.inner = psi;
		auFormula.phi = &notPhi;
		auFormula.psi = &notPsi;
		auFormula.index = index;
		auFormula.dfsindex = dfsindex;
		auFormula.payloadsize = payloadsize;
		return !auFormula.check(s,ns,firelist,witness);
	}
};
