/*!
\file CopyEncoder.h
\author Christian Koch
\status new
\brief NetStateEncoder implementation that copies the marking, while ignoring capacity limitations. The copy operation isn't done at all if possible (just passing the marking pointer), otherwise memcpy is used
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/PluginStore.h>

class CopyEncoder : public PluginStore::NetStateEncoder
{
public:
    CopyEncoder(int numThreads);
    ~CopyEncoder();

    vectordata_t* encodeNetState(NetState& ns, bitindex_t& bitlen, index_t threadIndex);
private:

    /// number of words in input vector
    index_t insize;

    /// instead of copying the data, the marking pointer can be casted directly into the input vector pointer if:
    /// a) the marking bit width is divisble by the input vector data width
    ///    or
    /// b) the marking vector has at least as many bytes as the needed input vector
    /// a) will be checked at compile time whereas b) will be checked at runtime. Only if both tests fail memcpy is actually used.
#if SIZEOF_CAPACITY_T % SIZEOF_VECTORDATA_T != 0
    bool nocopy;

    /// vector of input vectors that are returned from encodeNetState. Each thread has its own vector to avoid conflicts. Only needed if memcpy is actually used.
    vectordata_t** inputs;
#endif
};

