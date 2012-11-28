
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

	bool check(Store<void*>& s, NetState& ns, Firelist& firelist);
};
