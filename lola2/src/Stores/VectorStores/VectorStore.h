#pragma once
#include <Core/Dimensions.h>

template <typename T>
class VectorStore
{
public:
	virtual ~VectorStore() {}
	virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex) = 0;

	// gets and removes a vector from the store
	virtual bool popVector(vectordata_t * & out) = 0;
};
