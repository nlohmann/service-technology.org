
#pragma once

#include <Net/NetState.h>
#include <Exploration/Firelist.h>
#include <Stores/Store.h>

enum CTLFormulaResult {
	UNKNOWN = 0,     // implies white/unvisited
	IN_PROGRESS = 1, // implies gray or on tarjan stack (depending on search method)
	KNOWN_FALSE = 2, // implies black/visited
	KNOWN_TRUE = 3   // implies black/visited
};

struct CTLFormula {
	index_t index; // bit index in state payload. known flag is 2*index, value flag is 2*index+1
//	CTLFormula* parent;
	size_t payloadsize;

	CTLFormulaResult getCachedResult(void* payload) {
		return static_cast<CTLFormulaResult>(((*(reinterpret_cast<uint8_t*>(payload)+(index>>3))) >> (index&7))&3);
	}

	void setCachedResult(void* payload, CTLFormulaResult result) {
		uint8_t cachedValue = *(reinterpret_cast<uint8_t*>(payload)+(index>>3));
		if(result&1)
			cachedValue |= (1 << index);
		else
			cachedValue &= ~(1 << index);
		if(result&2)
			cachedValue |= (1 << (index+1));
		else
			cachedValue &= ~(1 << (index+1));
		*(reinterpret_cast<uint8_t*>(payload)+(index>>3)) = cachedValue;
	}

	virtual void updateAtomics(NetState& ns, index_t t) = 0;
	virtual void revertAtomics(NetState& ns, index_t t) = 0;
	virtual bool check(Store<void*>& s, NetState& ns, Firelist& firelist) = 0;
};
