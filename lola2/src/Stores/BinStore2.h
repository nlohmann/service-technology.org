/*!
\file BinStore.h
\author Karsten, Christian Koch
\status new
\brief implement Store as a decision tree with bitvectors.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/Store.h>


class BinStore2 : public Store
{
    public:
        BinStore2(uint32_t _number_of_threads);
        ~BinStore2();

        /// check whether current marking is stored and return state
        virtual bool searchAndInsert(NetState &ns, uint32_t thread);
        virtual bool searchAndInsert(NetState &ns, void**);
        //void pbs(unsigned int, unsigned int, vectordata_t*, void*);
        //void printBinStore();
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
                //State* state;
                ~Decision();

        };
        // first branch in decision tree; NULL as long as less than two elements in bucket
        Decision** branch;

        // the read-write mutexes
        pthread_rwlock_t* rwlocks;

        // first vector in bucket; null as long as bucket empty
        vectordata_t** firstvector;

        // precalculated bitmask for bit filtering: place_bitmask[i] gives the i least significant bits
        capacity_t place_bitmask[PLACE_WIDTH + 1];

        // precalculated bitmask for bit filtering: vector_bitmask[i] gives the i least significant bits
        vectordata_t vector_bitmask[VECTOR_WIDTH + 1];
};

