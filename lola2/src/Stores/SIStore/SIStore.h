/*!
\author Gregor
\file SIStore.h
\status new
*/

#pragma once
#include <config.h>

#include "Net/NetState.h"

class SIStore
{
public:
    SIStore(int numberOfThreads): number_of_threads(numberOfThreads) {};
    virtual ~SIStore() {};

    virtual bool search(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex) = 0;
    virtual bool insert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex) = 0;
protected:
    int number_of_threads;
};
