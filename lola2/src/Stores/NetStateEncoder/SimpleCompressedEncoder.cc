/*!
\file SimpleCompressedEncoder.cc
\author Christian Koch
\status new
\brief NetStateEncoder implementation encoding each marking according to it's length (in bits). Furthermore it uses as few as possible bits in the sense that the encodings are tight. So, for example, 2 is encoded as "0100" instead of "0110" and 6 is encoded as "001000" instead of "001110".
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/NetStateEncoder/SimpleCompressedEncoder.h>


SimpleCompressedEncoder::SimpleCompressedEncoder(int numThreads) : PluginStore::NetStateEncoder(numThreads)
{
	insize = ((2*Place::CardSignificant*PLACE_WIDTH+VECTOR_WIDTH-1)/VECTOR_WIDTH) * sizeof(vectordata_t);

	inputs = (vectordata_t**) malloc(numThreads * SIZEOF_VOIDP);
    for(int i=0;i<numThreads;i++)
    {
    	inputs[i] = (vectordata_t*) malloc(insize);
    }
}

SimpleCompressedEncoder::~SimpleCompressedEncoder()
{
    for(int i=0;i<numThreads;i++)
    {
    	free(inputs[i]);
    }
    free(inputs);
}

/// adds the <numbits> least significant bits of <val> to the input vector
void SimpleCompressedEncoder::addToInput(capacity_t val, bitindex_t numbits, vectordata_t*& pInput, bitindex_t& input_bitstogo) {
	while(numbits) {
#if PLACE_WIDTH >= VECTOR_WIDTH
        *pInput |= vectordata_t((capacity_t(val << (PLACE_WIDTH - numbits)) >> (PLACE_WIDTH - input_bitstogo)));
#else
        *pInput |= vectordata_t(vectordata_t(val) << (VECTOR_WIDTH - numbits)) >> (VECTOR_WIDTH - input_bitstogo);
#endif
        if(input_bitstogo <= numbits) {
			numbits-=input_bitstogo;
			pInput++;
			input_bitstogo = VECTOR_WIDTH;
        } else {
        	input_bitstogo-=numbits;
        	return;
        }
	}
}

vectordata_t* SimpleCompressedEncoder::encodeNetState(NetState& ns, bitindex_t& bitlen, uint32_t threadIndex)
{
    bitlen = 0;
    vectordata_t* pCurThreadInput = inputs[threadIndex];

    vectordata_t* pInput = pCurThreadInput;
    bitindex_t input_bitstogo = VECTOR_WIDTH;

    memset(pCurThreadInput,0,insize);

    for(index_t place_index = 0; place_index < Place::CardSignificant; place_index++) {
    	capacity_t curMarking = ns.Current[place_index];

    	//Calculating correct number of needed bits and a subtrahend
    	index_t prefix_length = 1;
    	capacity_t num_elems_with_prefix = 2;
    	while(curMarking >= num_elems_with_prefix) prefix_length++, curMarking -= num_elems_with_prefix, num_elems_with_prefix<<=1;

    	addToInput(1,prefix_length,pInput,input_bitstogo);
    	addToInput(ns.Current[place_index],prefix_length,pInput,input_bitstogo);
    	bitlen += prefix_length << 1;
    }
    return pCurThreadInput;
}
