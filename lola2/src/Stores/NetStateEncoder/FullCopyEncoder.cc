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


FullCopyEncoder::FullCopyEncoder(int numThreads) : NetStateEncoder(numThreads)
{
    // compute number of input words necessary to store all places.
    insize = ((SIZEOF_CAPACITY_T * Net::Card[PL] + SIZEOF_VECTORDATA_T - 1) / SIZEOF_VECTORDATA_T);

    rep->status("using %d bytes per marking, including %d wasted bytes", insize * SIZEOF_VECTORDATA_T,
                insize * SIZEOF_VECTORDATA_T - SIZEOF_CAPACITY_T * Net::Card[PL]);

    // test if memcpy is actually required. See declaration of nocopy for detailed information.
    // if condition (a) is met, none of this code is required.
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T != 0
    // test if condition (b) is met.
    if (insize * SIZEOF_VECTORDATA_T <= Net::Card[PL] * SIZEOF_CAPACITY_T)
        // condition (b) is met, no memcpy required.
    {
        nocopy = true;
    }
    else
    {
        // neither condition is met, memcpy is required.
        nocopy = false;
        // initialize auxiliary vectors that hold the memcpy result (one for each thread)
        inputs = new vectordata_t *[numThreads];
        for (int i = 0; i < numThreads; i++)
        {
            inputs[i] = new vectordata_t[insize];
        }
    }
#endif
}

FullCopyEncoder::~FullCopyEncoder()
{
    // free input vectors (if at all used)
    // if condition (a) is met, none of this code is required
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T != 0
    // check if condition (b) is met. The memory was allocated (and therefore needs to be freed) only if it is not.
    if (!nocopy)
    {
        for (int i = 0; i < numThreads; i++)
        {
            delete[] inputs[i];
        }
        delete[] inputs;
    }
#endif
}

vectordata_t *FullCopyEncoder::encodeNetState(NetState &ns, bitindex_t &bitlen, index_t threadIndex)
{
    // calculate the length (in bit) of the returned vector.
    bitlen = insize * VECTOR_WIDTH;

    // test if memcpy is actually required. See CopyEncoder.h for detailed information.
    // test condition (a)
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T == 0
    // if condition (a) is met, the marking vector can simply be casted into the input vector.
    return reinterpret_cast<vectordata_t *>(ns.Current);
#else
    // test condition (b)
    if (nocopy)
        // if condition (b) is met, the marking vector can simply be casted into the input vector.
    {
        return reinterpret_cast<vectordata_t *>(ns.Current);
    }
    // neither condition is met, memcpy into preallocated input vector
    memcpy(inputs[threadIndex], ns.Current, Net::Card[PL] * SIZEOF_CAPACITY_T);
    return inputs[threadIndex];
#endif
}

// decodes a given encoded state and sets the netstate appropriately
void FullCopyEncoder::decodeNetState(NetState &ns, vectordata_t *data)
{
    // just copy the data back to the NetState
    memcpy(ns.Current, data, Net::Card[PL] * SIZEOF_CAPACITY_T);
}
