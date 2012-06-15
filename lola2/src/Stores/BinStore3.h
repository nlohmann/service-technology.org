/*!
\file BinStore3.h
\author Christian Koch
\status new
\brief SuffixTreeStore implementation that copies the marking bit-perfect with no compression (analogous to BinStore2).
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/SuffixTreeStore.h>

class BinStore3 : public SuffixTreeStore
{
public:
    BinStore3();
    ~BinStore3();

    virtual input_t* getInput(NetState& ns, bitindex_t& bitlen);
private:
    /// input vector filled in getinput method
    input_t* in;

    /// number of words in input vector
    bitindex_t insize;

    /// number of leading words that can be copied using memcpy.
    index_t memcpylen;
};

