/*!
\file SuffixTreeStore.h
\author Christian Koch
\status new
\brief VectorStore implementation using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/PluginStore.h>

class SuffixTreeStore : public PluginStore::VectorStore
{
public:
    SuffixTreeStore();
    virtual ~SuffixTreeStore();

    // inserts input vector into suffix tree
    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash);
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
};

