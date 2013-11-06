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

extern Reporter *rep;

BitEncoder::BitEncoder(int numThreads) : NetStateEncoder(numThreads)
{
    // compute number of input words necessary to store all significant bits.
    insize = ((Place::SizeOfBitVector + VECTOR_WIDTH - 1) / VECTOR_WIDTH);

    rep->status("using %d bytes per marking, with %d unused bits", insize * SIZEOF_VECTORDATA_T,
                insize * VECTOR_WIDTH - Place::SizeOfBitVector);

    // allocate auxiliary input vectors
    inputs = (vectordata_t **) malloc(numThreads * SIZEOF_VOIDP);
    for (int i = 0; i < numThreads; i++)
    {
        inputs[i] = (vectordata_t *) malloc(insize * sizeof(vectordata_t));
    }

    // determine the number of leading places suitable for memcpying (see declaration of memcpylen for details)
    memcpylen = 0;
#if VECTOR_WIDTH == PLACE_WIDTH
    while (memcpylen < insize && Place::CardBits[memcpylen] == PLACE_WIDTH)
    {
        memcpylen++;
    }
#endif
}

BitEncoder::~BitEncoder()
{
    // free all auxiliary input vectors
    for (int i = 0; i < numThreads; i++)
    {
        free(inputs[i]);
    }
    free(inputs);
}

vectordata_t *BitEncoder::encodeNetState(NetState &ns, bitindex_t &bitlen, index_t threadIndex)
{
    // number of bits in input vector will be exactly the number of bits necessary to store all bits of the significant places.
    bitlen = Place::SizeOfBitVector;
    // fetch input vector for this thread
    vectordata_t *pCurThreadInput = inputs[threadIndex];

    // use memcpy when possible
#if VECTOR_WIDTH == PLACE_WIDTH
    memcpy(pCurThreadInput, ns.Current, memcpylen * sizeof(vectordata_t));
    // test if we are already done
    if (memcpylen >= Place::CardSignificant)
    {
        return pCurThreadInput;
    }
#endif

    /// the place we are currently dealing with (skipping all memcpyed places)
    index_t place_index = memcpylen;
    /// pointer to current place (skipping all memcpyed places)
    capacity_t *pPlace = ns.Current + memcpylen;
    /// the bits of the place's marking we have NOT dealt with so far
    bitindex_t place_bitstogo = Place::CardBits[place_index]; // start with msb

    /// pointer to current input word (skipping all memcpyed places)
    vectordata_t *pInput = pCurThreadInput + memcpylen;
    /// the bits of the current input word we have NOT dealt with so far
    bitindex_t input_bitstogo = VECTOR_WIDTH;

    // clear input vector (we are going to use bitwise or later on, so all bits have to be zero initially)
    memset(pInput, 0, (insize - memcpylen)*sizeof(vectordata_t));

    while (true)
    {
        // fill all bits into the input vector that are in the current word of both the input vector and the current place.
        // uses preprocessor to optimize operation (since this is the most time consuming step)
#if PLACE_WIDTH >= VECTOR_WIDTH
        *pInput |= vectordata_t((capacity_t(*pPlace << (PLACE_WIDTH - place_bitstogo)) >>
                                 (PLACE_WIDTH - input_bitstogo)));
#else
        *pInput |= vectordata_t(vectordata_t(*pPlace) << (VECTOR_WIDTH - place_bitstogo)) >>
                   (VECTOR_WIDTH - input_bitstogo);
#endif

        // update position variables
        if (place_bitstogo < input_bitstogo)  // all place bits inserted, go to next place
        {
            place_index++, pPlace++;
            if (place_index >= Place::CardSignificant)
            {
                break;
            }
            input_bitstogo -= place_bitstogo;
            place_bitstogo = Place::CardBits[place_index];
        }
        else if (place_bitstogo > input_bitstogo)    // current input word full, go to next word
        {
            place_bitstogo -= input_bitstogo;
            input_bitstogo = VECTOR_WIDTH;
            ++pInput;
        }
        else     // both current input word and current place are done, go to next ones.
        {
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
