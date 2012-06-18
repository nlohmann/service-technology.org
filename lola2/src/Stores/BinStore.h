/*!
\file BinStore.h
\author Karsten
\status new
\brief implement Store as a decision tree with bitvectors.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/Store.h>

// forward declaration
template <typename T> class LoadedBinStore;

class BinStore : public Store
{
    public:
        BinStore();
        ~BinStore();

        /// check whether current marking is stored and return state
        virtual bool searchAndInsert(NetState &ns, void**);
        void pbs(unsigned int, unsigned int, unsigned char*, void*);
        void printBinStore();

        // attach a payload to each state in the store
        template <typename T>
        LoadedBinStore<T>* loadStore(const T &); // implemented in LoadedBinStore.h
    protected:
        /// a binary decision node
        class Decision
        {
            public:
                Decision(bitindex_t);
                bitindex_t bit;
                unsigned char* vector;
                Decision* nextold;
                Decision* nextnew;
                ~Decision();
                virtual void getPayload(void**) {};
        };
        // first branch in decision tree; NULL as long as less than two elements in bucket
        Decision** branch;
        // create a decision node
        virtual Decision* createDecision(bitindex_t);

        // get payload of first vector
        virtual void getFirstPayload(hash_t, void**) {};
        // get payload of first vector
        virtual void getAndCreateFirstPayload(hash_t, void**) {};

        // first vector in bucket; null as long as bucket empty
        unsigned char** firstvector;
};
