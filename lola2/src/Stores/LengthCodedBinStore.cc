/*!
\file LengthCodedBinStore.cc
\author Christian Koch
\status new
\brief SuffixTreeStore implementation encoding each marking according to its length (in bits). A marking of length n will need 2*n bits of space.
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Net/Marking.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Stores/LengthCodedBinStore.h>


LengthCodedBinStore::LengthCodedBinStore()
{
	insize = ((2*Place::CardSignificant*PLACE_WIDTH+INPUT_WIDTH-1)/INPUT_WIDTH) * sizeof(input_t);
    in = (input_t*) malloc(insize);
}

LengthCodedBinStore::~LengthCodedBinStore()
{
    free(in);
}

void LengthCodedBinStore::addToInput(capacity_t val, bitindex_t numbits) {
	while(numbits) {
#if PLACE_WIDTH >= INPUT_WIDTH
        in[input_index] |= input_t((capacity_t(val << (PLACE_WIDTH - numbits)) >> (PLACE_WIDTH - input_bitstogo)));
#else
        in[input_index] |= input_t(input_t(val) << (INPUT_WIDTH - numbits)) >> (INPUT_WIDTH - input_bitstogo);
#endif
        if(input_bitstogo <= numbits) {
			numbits-=input_bitstogo;
			input_index++;
			input_bitstogo = INPUT_WIDTH;
        } else {
        	input_bitstogo-=numbits;
        	return;
        }
	}
}

input_t* LengthCodedBinStore::getInput(NetState* ns, bitindex_t& bitlen)
{
    bitlen = 0;
    input_index = 0;
    input_bitstogo = INPUT_WIDTH;

    memset(in,0,insize);

    for(index_t place_index = 0; place_index < Place::CardSignificant; place_index++) {
    	int marking_length = 0;
    	capacity_t curMarking = ns->Current[place_index];
    	do {
    		curMarking >>= 1, marking_length++;
    	} while(curMarking);
    	addToInput(1,marking_length);
    	addToInput(ns->Current[place_index],marking_length);
    	bitlen += 2*marking_length;
    }
    return in;
}
