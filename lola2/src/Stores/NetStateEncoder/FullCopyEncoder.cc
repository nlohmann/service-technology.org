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
#include <Stores/NetStateEncoder/FullCopyEncoder.h>
#include <InputOutput/Reporter.h>

extern Reporter* rep;


FullCopyEncoder::FullCopyEncoder(int numThreads) : NetStateEncoder(numThreads)
{
	// compute number of input words necessary to store all places.
    insize = ((SIZEOF_CAPACITY_T*Net::Card[PL]+SIZEOF_VECTORDATA_T-1)/SIZEOF_VECTORDATA_T);

    rep->status("using %d bytes per marking, including %d wasted bytes", insize * SIZEOF_VECTORDATA_T, insize * SIZEOF_VECTORDATA_T - SIZEOF_CAPACITY_T * Net::Card[PL]);

    // test if memcpy is actually required. See declaration of nocopy for detailed information.
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T != 0
    if(insize * SIZEOF_VECTORDATA_T <= Net::Card[PL] * SIZEOF_CAPACITY_T)
        nocopy = true;
    else {
        nocopy = false;
        inputs = (vectordata_t**) malloc(numThreads * SIZEOF_VOIDP);
        for(int i=0; i<numThreads; i++)
        {
            inputs[i] = (vectordata_t*) malloc(insize * SIZEOF_VECTORDATA_T);
        }
    }
#endif
}

FullCopyEncoder::~FullCopyEncoder()
{
	// free input vectors (if at all used)
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T != 0
    if(!nocopy) {
        for(int i=0; i<numThreads; i++)
        {
            free(inputs[i]);
        }
        free(inputs);
    }
#endif
}

vectordata_t* FullCopyEncoder::encodeNetState(NetState& ns, bitindex_t& bitlen, index_t threadIndex)
{
    bitlen = insize * VECTOR_WIDTH;

    // test if memcpy is required
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T == 0
    // it isn't (case a), cast the input vector
    return reinterpret_cast<vectordata_t*>(ns.Current);
#else
    if(nocopy) // it isn't (case b), cast the input vector
        return reinterpret_cast<vectordata_t*>(ns.Current);
    // it is, memcpy into preallocated input vector
    memcpy(inputs[threadIndex],ns.Current, Net::Card[PL] * SIZEOF_CAPACITY_T);
    return inputs[threadIndex];
#endif
}

// decodes a given encoded state and sets the netstate appropriately
void FullCopyEncoder::decodeNetState(NetState& ns, vectordata_t* data)
{
  memcpy(ns.Current, data, Net::Card[PL] * SIZEOF_CAPACITY_T);
}
