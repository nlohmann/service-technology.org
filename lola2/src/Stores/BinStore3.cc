/*!
\file BinStore3.cc
\author Christian Koch
\status new
\brief SuffixTreeStore implementation that copies the marking bit-perfect with no compression (analogous to BinStore2).
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/BinStore3.h>


BinStore3::BinStore3()
{
    insize = ((Place::SizeOfBitVector+INPUT_WIDTH-1)/INPUT_WIDTH);
    in = (input_t*) malloc(insize * sizeof(input_t));

    memcpylen = 0;
    // memcpy can be used only if input and suffix tree vectors use the same data type
#if INPUT_WIDTH == PLACE_WITDH
    while(memcpylen<insize && Place::CardBits[memcpylen] == PLACE_WIDTH)
        memcpylen++;
#endif
}

BinStore3::~BinStore3()
{
    free(in);
}

input_t* BinStore3::getInput(NetState* ns, bitindex_t& bitlen)
{
    bitlen = Place::SizeOfBitVector;

    // use memcpy if possible
#if INPUT_WIDTH == PLACE_WITDH
    memcpy(in,ns->Current,memcpylen * sizeof(input_t));
    if(memcpylen >= Place::CardSignificant)
    {
        return in;
    }
#endif

    /// the place we are currently dealing with
    index_t place_index = memcpylen;
    /// pointer to current place
    capacity_t* pPlace = ns->Current+memcpylen;
    /// the bits of the place's marking we have NOT dealt with so far
    bitindex_t place_bitstogo = Place::CardBits[place_index]; // indicates start with msb

    /// pointer to current input word
    input_t* pInput = in+memcpylen;
    /// the bits of the current input word we have NOT dealt with so far
    bitindex_t input_bitstogo = INPUT_WIDTH;

    /// clear input vector
    memset(pInput,0,(insize - memcpylen)*sizeof(input_t));

    while (true)
    {
#if PLACE_WIDTH >= INPUT_WIDTH
        *pInput |= input_t((capacity_t(*pPlace << (PLACE_WIDTH - place_bitstogo)) >> (PLACE_WIDTH - input_bitstogo)));
#else
        *pInput |= input_t(input_t(*pPlace) << (INPUT_WIDTH - place_bitstogo)) >> (INPUT_WIDTH - input_bitstogo);
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
            input_bitstogo = INPUT_WIDTH;
            ++pInput;
        } else {
            place_index++, pPlace++;
            if (place_index >= Place::CardSignificant)
            {
                break;
            }
            place_bitstogo = Place::CardBits[place_index];
            input_bitstogo = INPUT_WIDTH;
            ++pInput;
        }
    }
    return in;
}
