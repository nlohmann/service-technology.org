/*!
\file PluginStore.cc
\author Christian Koch
\status new
\brief general-purpose base store consisting of two components: a NetStateEncoder, that converts the current state into a input vector, and a VectorStore, that stores the input vectors and checks whether they are already known. Implementations for both components can be combined arbitrarily. Specific components may have some functional or compatibility limitations, though.
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/PluginStore.h>


PluginStore::NetStateEncoder::NetStateEncoder(int _numThreads) : numThreads(_numThreads)
{
}

/// creates new Store using the specified components. The given components are assumed to be used exclusively and are freed once the PluggableStore gets destructed.
PluginStore::PluginStore(NetStateEncoder* _netStateEncoder, VectorStore* _vectorStore, uint32_t _number_of_threads) : Store(_number_of_threads)
{
    pthread_mutex_init(&inc_mutex, NULL);
    netStateEncoder = _netStateEncoder;
    vectorStore = _vectorStore;
}

/// frees both components
PluginStore::~PluginStore()
{
    pthread_mutex_destroy(&inc_mutex);
    delete netStateEncoder;
    delete vectorStore;
}

/// search for a state and insert it, if it is not present
bool PluginStore::searchAndInsert(NetState &ns, void** s)
{
    return searchAndInsert(ns, 0);
}

/// search for a state and insert it, if it is not present
bool PluginStore::searchAndInsert(NetState &ns, int threadIndex)
{
    ++calls[threadIndex];

    // fetch input vector
    bitindex_t bitlen;
    vectordata_t* input = netStateEncoder->encodeNetState(ns, bitlen, threadIndex);

    // check input vector in vector store
    bool ret = vectorStore->searchAndInsert(input, bitlen, ns.HashCurrent);
    if (!ret)
        ++markings[threadIndex];
    return ret;
}
