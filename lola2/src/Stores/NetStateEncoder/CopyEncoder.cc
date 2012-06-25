/*!
\file CopyEncoder.cc
\author Christian Koch
\status new
\brief NetStateEncoder implementation that copies the marking, while ignoring capacity limitations. The copy operation isn't done at all if possible (just passing the marking pointer), otherwise memcpy is used
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/NetStateEncoder/CopyEncoder.h>
#include <InputOutput/Reporter.h>

extern Reporter* rep;


CopyEncoder::CopyEncoder(int numThreads) : PluginStore::NetStateEncoder(numThreads)
{
    insize = ((SIZEOF_CAPACITY_T*Place::CardSignificant+SIZEOF_VECTORDATA_T-1)/SIZEOF_VECTORDATA_T);

    rep->status("using %d bytes per marking, including %d wasted bytes", insize * SIZEOF_VECTORDATA_T, insize * SIZEOF_VECTORDATA_T - SIZEOF_CAPACITY_T * Place::CardSignificant);

#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T != 0
    if(insize * SIZEOF_VECTORDATA_T <= Net::Card[PL] * SIZEOF_CAPACITY_T)
    	nocopy = true;
   	else {
   		nocopy = false;
   	    inputs = (vectordata_t**) malloc(numThreads * SIZEOF_VOIDP);
   	    for(int i=0;i<numThreads;i++)
   	    {
  	    	inputs[i] = (vectordata_t*) malloc(insize * SIZEOF_VECTORDATA_T);
  	    }
  	}
#endif
}

CopyEncoder::~CopyEncoder()
{
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T != 0
	if(!nocopy) {
		for(int i=0;i<numThreads;i++)
		{
			free(inputs[i]);
    	}
    	free(inputs);
	}
#endif
}

vectordata_t* CopyEncoder::encodeNetState(NetState& ns, bitindex_t& bitlen, uint32_t threadIndex)
{
    bitlen = insize * VECTOR_WIDTH;
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T == 0
    return reinterpret_cast<vectordata_t*>(ns.Current);
#else
    if(nocopy)
        return reinterpret_cast<vectordata_t*>(ns.Current);
    memcpy(inputs[threadIndex],ns.Current,Place::CardSignificant * SIZEOF_CAPACITY_T);
    return inputs[threadIndex];
#endif
}
