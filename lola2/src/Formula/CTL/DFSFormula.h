
#pragma once

#include <Formula/CTL/CTLFormula.h>
#include <Core/Dimensions.h>

struct DFSStackEntry
{
    index_t * fl; // array to take a firelist
    index_t flIndex; // index of first processed element of fl
    void* payload;
    statenumber_t lowlink;

    /// ordinary constructor for entry
	DFSStackEntry(index_t * _fl, index_t _flIndex, void* _payload, statenumber_t _lowlink) {
        assert(_fl);    //firelist, first processed in firelist
        fl = _fl;
        flIndex = _flIndex;
        payload = _payload;
        lowlink = _lowlink;
    }
    ~DFSStackEntry() {
        if (fl) delete[] fl;
        fl = NULL;
    }
};

struct DFSFormula : public CTLFormula {
	index_t dfsindex; // byte index to dfs number

	statenumber_t getDFS(void* payload) {
		return *(reinterpret_cast<statenumber_t*>(payload)+dfsindex);
	}

	void setDFS(void* payload, statenumber_t dfs) {
		*(reinterpret_cast<statenumber_t*>(payload)+dfsindex) = dfs;
	}
};
