
#pragma once


#include <Formula/CTL/CTLFormula.h>
#include <Net/Transition.h>

struct AXFormula : public CTLFormula {
	CTLFormula* inner;

	void updateAtomics(NetState& ns, index_t t) {
		inner->updateAtomics(ns,t);
	}
	void revertAtomics(NetState& ns, index_t t) {
		inner->revertAtomics(ns,t);
	}

	bool check(Store<void*>& s, NetState& ns, Firelist& firelist, std::vector<int>* witness) {
		index_t* fl;
		index_t cardfl = firelist.getFirelist(ns,&fl);
		bool result;
		while(cardfl--) {
			Transition::fire(ns,fl[cardfl]);
			updateAtomics(ns,fl[cardfl]);

			result = inner->check(s,ns,firelist,null,witness);

			Transition::backfire(ns,fl[cardfl]);
			revertAtomics(ns,fl[cardfl]);

			if(!result) {
				witness->push_back(fl[cardfl]);
				return false;
			}
			else
				witness->clear();
		}
		return true;
	}
};
