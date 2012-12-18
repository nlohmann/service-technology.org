

#pragma once

#include <vector>

class CTLExploration {
public:
	std::vector<int> witness;

	bool checkProperty(CTLFormula* formula, Store<void*> &store,
			Firelist &firelist, NetState &ns);
};
