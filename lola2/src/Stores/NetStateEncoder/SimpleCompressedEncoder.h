/*!
\file SimpleCompressedEncoder.h
\author Christian Koch
\status new
\brief NetStateEncoder implementation encoding each marking according to its length (in bits). A marking of length n will need 2*n bits of space.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/PluginStore.h>

class SimpleCompressedEncoder : public PluginStore::NetStateEncoder
{
public:
	SimpleCompressedEncoder(int numThreads);
    ~SimpleCompressedEncoder();

    vectordata_t* encodeNetState(NetState& ns, bitindex_t& bitlen, int threadIndex);
private:
    void addToInput(capacity_t val, bitindex_t numbits, vectordata_t*& pInput, bitindex_t& input_bitstogo);

    /// input vector filled in getInput method
    vectordata_t** inputs;
    index_t insize;
};

