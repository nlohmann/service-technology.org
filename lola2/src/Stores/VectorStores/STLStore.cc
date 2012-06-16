/*!
\author Niels, Christian Koch
\file STLStore.cc
\status new
*/

#include <algorithm>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Stores/VectorStores/STLStore.h>

bool VSTLStore::searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash)
{
	index_t vectorLength = (bitlen+(VECTOR_WIDTH-1))/VECTOR_WIDTH;
    intermediate.resize(vectorLength);
	// copy current marking to vector
    std::copy(in, in + vectorLength, intermediate.begin());

    // shift away unused bits to make sure they are zero
    bitlen -= vectorLength * VECTOR_WIDTH;
    if(bitlen)
    	(intermediate[vectorLength-1] >>= (VECTOR_WIDTH-bitlen)) <<= (VECTOR_WIDTH-bitlen);

    // add vector to marking store
    const std::pair<std::set<std::vector<capacity_t> >::iterator, bool> res = store.insert(intermediate);

    if (res.second)
    {
        // marking was new
        return false;
    }
    else
    {
        // marking was old
        return true;
    }
}
