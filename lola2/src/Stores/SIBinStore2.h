/*!
 \file BinStore.h
 \author Karsten, Christian Koch, Gregor Behnke
 \status new
 \brief implement Store as a decision tree with bitvectors.
 */

#pragma once
#include <Core/Dimensions.h>
#include <Stores/SIStore.h>
#include <Stores/BinStore2.h>




typedef unsigned char uchar;

class SIBinStore2: public SIStore
{
    public:
        SIBinStore2(int numberOfThreads);
        ~SIBinStore2();

        bool search(NetState& ns, int threadNumber);
        bool insert(NetState& ns, int threadNumber);
    private:
        // original binstore 2 members
        /// a binary decision node
        class Decision
        {
            public:
                Decision(bitindex_t);
                bitindex_t bit;
                vectordata_t* vector;
                Decision* nextold;
                Decision* nextnew;
                //State* state;
                ~Decision();

        };
        // the inner search function, we are able to tell whether to lock the rw-mutex or not
        bool search(NetState& ns, int threadNumber, bool lock_rw_mutex);

        // the read-write mutexes
        pthread_rwlock_t* rwlocks;

        // first branch in decision tree; NULL as long as less than two elements in bucket
        Decision** branch;

        // first vector in bucket; null as long as bucket empty
        vectordata_t** firstvector;

        // precalculated bitmask for bit filtering: place_bitmask[i] gives the i least significant bits
        capacity_t place_bitmask[PLACE_WIDTH + 1];

        // precalculated bitmask for bit filtering: vector_bitmask[i] gives the i least significant bits
        vectordata_t vector_bitmask[VECTOR_WIDTH + 1];

        //////////////////////////////
        // temporary SI-bin2 members//
        //////////////////////////////

        /// If a new decision record is inserted, * anchor must point to it
        Decision*** g_anchor;

        /// the vector we are currently investigating
        vectordata_t** g_currentvector;

        /// the place where the new vector goes to
        vectordata_t*** g_newvector;

        /// the place we are currently dealing with
        index_t* g_place_index;

        /// the bits of the place's marking we have NOT dealt with so far
        bitindex_t* g_place_bitstogo; // indicates start with msb

        /// the index in the vector we are currently dealing with
        index_t* g_vector_index;

        /// the bits of the current vector's data we have NOT dealt with so far
        bitindex_t* g_vector_bitstogo;

        /// the number of bits processed until reaching the current branch
        bitindex_t* g_position;

        // neede to prepare insertion before inserting
        bool* g_prepareInsertion;

        // indicates whether the stored information for inserting are valid
        bool* g_validInsertInformation;
        hash_t* g_currentHashs;
};

SIBinStore2* createSIBinStore2(int threads);
