/*!
\file BitEncoder.cc
\author Christian Koch
\status new
\brief NetStateEncoder implementation that copies the marking bit-perfect with no compression (analogous to BitStore).
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/NetStateEncoder/BitEncoder.h>
#include <InputOutput/Reporter.h>

extern Reporter* rep;

BitEncoder::BitEncoder(int numThreads) : PluginStore::NetStateEncoder(numThreads)
{
    insize = ((Place::SizeOfBitVector+VECTOR_WIDTH-1)/VECTOR_WIDTH);

    rep->status("using %d bytes per marking, with %d unused bits", insize * SIZEOF_VECTORDATA_T, insize * VECTOR_WIDTH - Place::SizeOfBitVector);

    inputs = (vectordata_t**) malloc(numThreads * SIZEOF_VOIDP);
    for(int i=0;i<numThreads;i++)
    {
    	inputs[i] = (vectordata_t*) malloc(insize * sizeof(vectordata_t));
    }

    memcpylen = 0;
    // memcpy can be used only if input and suffix tree vectors use the same data type
#if VECTOR_WIDTH == PLACE_WIDTH
    while(memcpylen<insize && Place::CardBits[memcpylen] == PLACE_WIDTH)
        memcpylen++;
#endif
}

BitEncoder::~BitEncoder()
{
    for(int i=0;i<numThreads;i++)
    {
    	free(inputs[i]);
    }
    free(inputs);
}

vectordata_t* BitEncoder::encodeNetState(NetState& ns, bitindex_t& bitlen, int threadIndex)
{
    bitlen = Place::SizeOfBitVector;
    vectordata_t* pCurThreadInput = inputs[threadIndex];

    // use memcpy if possible
#if VECTOR_WIDTH == PLACE_WIDTH
    memcpy(pCurThreadInput,ns.Current,memcpylen * sizeof(vectordata_t));
    if(memcpylen >= Place::CardSignificant)
    {
        return pCurThreadInput;
    }
#endif

    /// the place we are currently dealing with
    index_t place_index = memcpylen;
    /// pointer to current place
    capacity_t* pPlace = ns.Current + memcpylen;
    /// the bits of the place's marking we have NOT dealt with so far
    bitindex_t place_bitstogo = Place::CardBits[place_index]; // indicates start with msb

    /// pointer to current input word
    vectordata_t* pInput = pCurThreadInput + memcpylen;
    /// the bits of the current input word we have NOT dealt with so far
    bitindex_t input_bitstogo = VECTOR_WIDTH;

    /// clear input vector
    memset(pInput,0,(insize - memcpylen)*sizeof(vectordata_t));

    while (true)
    {
#if PLACE_WIDTH >= VECTOR_WIDTH
        *pInput |= vectordata_t((capacity_t(*pPlace << (PLACE_WIDTH - place_bitstogo)) >> (PLACE_WIDTH - input_bitstogo)));
#else
        *pInput |= vectordata_t(vectordata_t(*pPlace) << (VECTOR_WIDTH - place_bitstogo)) >> (VECTOR_WIDTH - input_bitstogo);
#endif
        if(place_bitstogo < input_bitstogo) {
            place_index++, pPlace++;
            if (place_index >= Place::CardSignificant)
            {
                break;
            }
            input_bitstogo -= place_bitstogo;
            place_bitstogo = Place::CardBits[place_index];
        } else if(place_bitstogo > input_bitstogo) {
            place_bitstogo -= input_bitstogo;
            input_bitstogo = VECTOR_WIDTH;
            ++pInput;
        } else {
            place_index++, pPlace++;
            if (place_index >= Place::CardSignificant)
            {
                break;
            }
            place_bitstogo = Place::CardBits[place_index];
            input_bitstogo = VECTOR_WIDTH;
            ++pInput;
        }
    }
    return pCurThreadInput;
}
