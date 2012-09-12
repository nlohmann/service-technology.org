#pragma once
#include <Core/Dimensions.h>
#include <Net/NetState.h>

class NetStateEncoder
{
protected:
	int numThreads;
public:
	NetStateEncoder(int _numThreads) : numThreads(_numThreads) {}
	virtual ~NetStateEncoder() {}
	virtual vectordata_t* encodeNetState(NetState &ns, bitindex_t &bitlen, index_t threadIndex) = 0;
};
