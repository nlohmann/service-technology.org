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
    /// input vector filled in getinput method
    vectordata_t** inputs;

    /// number of words in input vector
    index_t insize;

    /// determines if marking itself is returned instead of copying
    bool nocopy;
};

