
#pragma once


#include <Formula/CTL/CTLFormula.h>

struct TrueFormula : public CTLFormula {
	void updateAtomics(NetState& ns, index_t t) {}
	void revertAtomics(NetState& ns, index_t t) {}
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist) {
		return true;
	}
};
