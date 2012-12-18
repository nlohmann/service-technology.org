/*!
\file SweepStore.h
\author Christian Koch, Harro Wimmel
\status new
\brief VectorStore implementation using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.

	Note: not VectorStore compatible at the moment due to missing parameter (hash_t)
	in the searchAndInsert function

	Warning: popState() cannot retrieve the payload at this time. The payload is therefore
	lost when a state is transferred from one store to another. The sweepline method with
	payload is thus not functional at the moment.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/VectorStores/VectorStore.h>

template <class T>
class SweepStore //: public VectorStore<T>
{
public:
    /// constructor
    SweepStore();
    /// destructor
    virtual ~SweepStore();

    /// searches for a vector and inserts if not found
    /// @param in vector to be seached for or inserted
    /// @param bitlen length of vector
    /// @return true, if the marking was found in the store, otherwise false.
    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, T** payload, bool insert = true);

    /// gets and removes a vector from the store
    /// @param out place where the returned vector will be written to
    /// @return false, if the store was already empty, otherwise true
    virtual bool popVector(vectordata_t * & out);

    /// check if the store is empty (without getting a vector)
    /// @return true if the store is empty
    virtual bool empty();
private:
    /// a binary decision node
    class Decision
    {
    public:
        /// constructor
        /// @param b index of first bit different from previous vector
        Decision(bitindex_t b);
        /// index of first bit different from previous vector
        bitindex_t bit;
        /// remaining vector after first differing bit
        vectordata_t* vector;
        /// decision nodes differing later than this one
        Decision* nextold;
        /// decision nodes differing from the remaining vector of this node
        Decision* nextnew;
        /// destructor
        ~Decision();

    };
    // first branch in decision tree; NULL as long as less than two elements in bucket
    Decision* branch;

    // first vector in bucket; null as long as bucket empty
    vectordata_t* firstvector;

    // full vector will be reconstructed here when popping vectors
    vectordata_t * popVectorCache;
};

#include <SweepLine/SweepStore.inc>
