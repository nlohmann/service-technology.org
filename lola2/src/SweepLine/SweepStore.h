/*!
\file SweepStore.h
\author Christian Koch, Harro Wimmel
\status new
\brief VectorStore implementation using binary prefix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/VectorStores/VectorStore.h>
#include <pthread.h>
#include <cstdlib>
#include <cstring>
#include <Net/Net.h>

template <typename T>
class SweepStore //: public VectorStore<T>
{
public:
    /// constructor for multithreading
	SweepStore(index_t nr_of_threads = 1);
    /// destructor
    virtual ~SweepStore();

    /// searches for a vector and inserts if not found
    /// @param in vector to be seached for or inserted
    /// @param bitlen length of vector
    /// @param hash of current NetState
    /// @param payload pointer to be set to the place where the payload of this state will be held
    /// @param threadIndex the index of the thread that requests this call. Values will range from 0 to (number_of_threads - 1). Used to allow using thread-local auxiliary data structures without locking any variables.
    /// @return true, if the marking was found in the store, otherwise false.
    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex, bool noinsert = false);

    /// gets and removes a vector from the store
    /// @param out place where the returned vector will be written to
    /// @return false, if the store was already empty, otherwise true
    virtual bool popVector(vectordata_t * & out, index_t threadIndex = 0);

    /// check if the store is empty (without getting a vector)
    /// @return true if the store is empty
    virtual bool empty();

private:

    size_t getPayloadSize();

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
    // first branch in decision tree; NULL as long as less than two elements
    Decision* branch;

    // the read-write mutex
    pthread_rwlock_t rwlock;

    // first vector; null as long as empty
    vectordata_t* firstvector;

    // full vector will be reconstructed here when popping vectors
    vectordata_t** popVectorCache;

	// number of threads that can access this store simultaneously
	index_t nr_of_threads;
};

// forward declaration to register function specialization
template<>
size_t SweepStore<void>::getPayloadSize();


#include <SweepLine/SweepStore.inc>
