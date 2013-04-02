#pragma once
#include <Core/Dimensions.h>

template <typename T>
class VectorStore
{
public:
  /// destructor
	virtual ~VectorStore() {}
	/// searches for a vector and inserts if not found
	/// @param in vector to be seached for or inserted
	/// @param bitlen length of vector
	/// @param hash of current NetState
	/// @param payload pointer to be set to the place where the payload of this state will be held
	/// @param threadIndex the index of the thread that requests this call. Values will range from 0 to (number_of_threads - 1). Used to allow using thread-local auxiliary data structures without locking any variables.
	/// @param noinsert if set to true only a search is done
	/// @return true, if the marking was found in the store, otherwise false.
	virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex, bool noinsert = false) = 0;

	/// gets and removes a vector from the store
	/// @param out place where the returned vector will be written to
	/// @return false, if the store was already empty, otherwise true
	virtual bool popVector(vectordata_t * & out, index_t threadIndex = 0) = 0;
};
