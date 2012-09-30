/*!
\file SuffixTreeStore.h
\author Christian Koch
\status new
\brief VectorStore implementation using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/VectorStores/VectorStore.h>
#include <pthread.h>

template <typename T>
class SuffixTreeStore : public VectorStore<T>
{
public:
    SuffixTreeStore();
    virtual ~SuffixTreeStore();

    // inserts input vector into suffix tree
    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex);

    // gets and removes a vector from the store
    virtual bool popVector(vectordata_t * & out);
private:
    /// a binary decision node
    class Decision
    {
    public:
        Decision(bitindex_t);
        bitindex_t bit;
        vectordata_t* vector;
        Decision* nextold;
        Decision* nextnew;
        ~Decision();

    };
    // first branch in decision tree; NULL as long as less than two elements in bucket
    Decision** branch;

    // the read-write mutexes
    pthread_rwlock_t* rwlocks;

    // first vector in bucket; null as long as bucket empty
    vectordata_t** firstvector;

    // full vector will be reconstructed here when popping vectors
    vectordata_t * popVectorCache;
    // index cache, so we don't have to search the whole store on each pop
    hash_t currentBucket;
};

#include <Stores/VectorStores/SuffixTreeStore.inc>
