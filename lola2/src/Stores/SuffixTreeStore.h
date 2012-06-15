/*!
\file SuffixTreeStore.h
\author Christian Koch
\status new
\brief abstract Store using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/Store.h>

class SuffixTreeStore : public Store
{
public:
    SuffixTreeStore();
    ~SuffixTreeStore();

    /// check whether current marking is stored
    virtual bool searchAndInsert(NetState& ns, void** s = NULL);

    /// converts current NetState to input vector.
    /// This method can basically do anything as long as it obeys two rules:
    /// * different NetStates are mapped to different input vectors
    /// * no input vector is prefix of a different input vector
    virtual input_t* getInput(NetState& ns, bitindex_t& bitlen) = 0;

    // inserts input vector into suffix tree
    virtual bool searchAndInsert(input_t* in, bitindex_t bitlen, hash_t hash);
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

    // a mutex for incrementing
    pthread_mutex_t inc_mutex;
};

