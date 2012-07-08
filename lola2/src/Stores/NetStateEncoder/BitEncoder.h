/*!
\file BitEncoder.h
\author Christian Koch
\status new
\brief NetStateEncoder implementation that copies the marking bit-perfect with no compression (analogous to BitStore).
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/PluginStore.h>

class BitEncoder : public PluginStore::NetStateEncoder
{
public:
    BitEncoder(int numThreads);
    ~BitEncoder();

    vectordata_t* encodeNetState(NetState& ns, bitindex_t& bitlen, index_t threadIndex);
private:
    /// input vector filled in getinput method
    vectordata_t** inputs;

    /// number of words in input vector
    index_t insize;

    /// number of leading words that can be copied using memcpy.
    index_t memcpylen;
};

