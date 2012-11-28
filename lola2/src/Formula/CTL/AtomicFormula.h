
#pragma once


#include <Formula/CTL/CTLFormula.h>
#include <Exploration/StatePredicateProperty.h>

struct AtomicFormula : public CTLFormula {
	StatePredicateProperty* inner;

	void updateAtomics(NetState& ns, index_t t) {
		inner->checkProperty(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		inner->updateProperty(ns,t);
	}
	bool check(Store<void*>& s, NetState& ns, Firelist& firelist) {
		return inner->value;
	}
};
