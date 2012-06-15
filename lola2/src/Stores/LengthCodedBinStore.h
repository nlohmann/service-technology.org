/*!
\file LengthCodedBinStore.h
\author Christian Koch
\status new
\brief SuffixTreeStore implementation encoding each marking according to its length (in bits). A marking of length n will need 2*n bits of space.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/SuffixTreeStore.h>

class LengthCodedBinStore : public SuffixTreeStore
{
public:
	LengthCodedBinStore();
    ~LengthCodedBinStore();

    virtual input_t* getInput(NetState& ns, bitindex_t& bitlen);
private:
    void addToInput(capacity_t val, bitindex_t numbits);
    index_t input_index;
    bitindex_t input_bitstogo;

    /// input vector filled in getInput method
    input_t* in;
    index_t insize;
};

