/*!
\file SuffixTreeStore.h
\author Christian Koch & Gregor Behnke
\status new
\brief VectorStore implementation using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
This Version has been transformed into an search and insert store.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/SIStore/SIStore.h>
#include <pthread.h>

class SISuffixTreeStore : public SIStore
{
public:
	SISuffixTreeStore(int numberOfThreads);
    virtual ~SISuffixTreeStore();

    // inserts input vector into suffix tree
    virtual bool search(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex);
    virtual bool insert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex);
private:
    bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex, bool doinsert);
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


    // the internal state of an search operation, which can be used to make the corresponding insert
    struct Search_State{
        /// If a new decision record is inserted, * anchor must point to it
        Decision** anchor;

        /// the place where the new vector goes to
        vectordata_t** newvector;

        /// the input word we are currently dealing with
        index_t input_index;

        /// the last input word to consider.
        index_t max_input_index ;

        /// the number of significant bits in the last input word (aligned at msb)
        index_t max_input_numbits;

        /// the bits of the current input word we have NOT dealt with so far
        bitindex_t input_bitstogo; // indicates start with msb

        // pointer to the current input word
        const vectordata_t* pInput;

        /// the vector word we are currently investigating
        vectordata_t* pVector;

        /// the bits of the current vector word we have NOT dealt with so far
        bitindex_t vector_bitstogo; // indicates start with msb

        /// the number of bits processed since reaching the current branch
        bitindex_t position;

        /// if this is true the insertion needs to be prepared
        bool need_to_prepare_insertion;
    };

    // first branch in decision tree; NULL as long as less than two elements in bucket
    Decision** branch;

    // the current search states of all the threads
    Search_State* search_states;

    // for each hash bucket we need to know which thread was the last to make an false-search operation on this bucket, and so can insert now
    index_t* last_search_not_found_thread;

    // the read-write mutexes
    pthread_rwlock_t* rwlocks;

    // first vector in bucket; null as long as bucket empty
    vectordata_t** firstvector;
};

