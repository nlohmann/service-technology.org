/*!
\file BinStore.h
\author Karsten
\status new
\brief implement Store as a decision tree with bitvectors..
*/
#pragma once

class State;

#include "Dimensions.h"
#include "Store.h"

class BinStore : public Store
{
    public:
        BinStore();
        ~BinStore();

        /// check whether current marking is stored
        virtual bool searchAndInsert();

        /// check whether current marking is sted and return state
        virtual bool searchAndInsert(State** s);
    private:
        /// a binary decision node
        class Decision
        {
            public:
                Decision(bitindex_t);
                bitindex_t bit;
                unsigned char* vector;
                Decision* nextold;
                Decision* nextnew;
                State* state;
                ~Decision();

        };
        // first branch in decision tree; NULL as long as less than two elements in bucket
        Decision* branch[SIZEOF_MARKINGTABLE];

        // first vector in bucket; null as long as bucket empty
        unsigned char* firstvector[SIZEOF_MARKINGTABLE];
};

