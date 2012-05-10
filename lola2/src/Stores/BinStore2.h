/*!
\file BinStore.h
\author Karsten, Christian Koch
\status new
\brief implement Store as a decision tree with bitvectors.
*/

#pragma once
#include "Core/Dimensions.h"
#include "Stores/Store.h"

typedef unsigned char uchar;


class BinStore2 : public Store
{
    public:
        BinStore2();
        ~BinStore2();

        /// check whether current marking is stored
        virtual bool searchAndInsert();

        /// check whether current marking is sted and return state
        virtual bool searchAndInsert(State** s);
        void pbs(unsigned int, unsigned int, uchar*, void*);
        void printBinStore();
    private:
        /// a binary decision node
        class Decision
        {
            public:
                Decision(bitindex_t);
                bitindex_t bit;
                uchar* vector;
                Decision* nextold;
                Decision* nextnew;
                State* state;
                ~Decision();

        };
        // first branch in decision tree; NULL as long as less than two elements in bucket
        Decision** branch;

        // first vector in bucket; null as long as bucket empty
        uchar** firstvector;
};

