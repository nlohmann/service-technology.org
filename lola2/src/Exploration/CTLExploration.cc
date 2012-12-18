
#include <Exploration/CTLExploration.h>

bool CTLExploration::checkProperty(CTLFormula* formula, Store<void*> &store,
			Firelist &firelist, NetState &ns) {

	// TODO: initialize formula meta information, ideally while unparsing

	witness.clear();
	return formula->check(store,ns,firelist,&witness);
}


