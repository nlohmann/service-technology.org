/*!
\file PluginStore.inc
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


template<typename T>
PluginStore<T>::PluginStore(NetStateEncoder* _netStateEncoder, VectorStore<T>* _vectorStore, uint32_t _number_of_threads) : Store<T>(_number_of_threads)
{
    netStateEncoder = _netStateEncoder;
    vectorStore = _vectorStore;
}

template<typename T>
PluginStore<T>::~PluginStore()
{
    delete netStateEncoder;
    delete vectorStore;
}

template<typename T>
bool PluginStore<T>::searchAndInsert(NetState &ns, T** payload, uint32_t threadIndex)
{
    // count calls
    ++(this->calls[threadIndex]);

    // fetch input vector
    bitindex_t bitlen;
    vectordata_t* input = netStateEncoder->encodeNetState(ns, bitlen, threadIndex);

    // check input vector in vector store
    bool ret = vectorStore->searchAndInsert(input, bitlen, ns.HashCurrent, payload, threadIndex);
    if (!ret) {
        ++(this->markings[threadIndex]);
    }
    return ret;
}
