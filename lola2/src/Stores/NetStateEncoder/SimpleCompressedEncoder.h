/*!
\file SimpleCompressedEncoder.h
\author Christian Koch, Max Görner
\status new
\brief NetStateEncoder implementation encoding each marking according to it's length (in bits). Furthermore it uses as few as possible bits in the sense that the encodings are tight. So, for example, 2 is encoded as "0100" instead of "0110" and 6 is encoded as "001000" instead of "001110".
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
    void getSubtrahendAndLength(capacity_t, int*, int*);

    /// input vector filled in getInput method
    vectordata_t** inputs;
    index_t insize;
    unsigned int numElems[32];
};

