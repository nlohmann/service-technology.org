#pragma once

#include <cuddObj.hh>
#include "Store.h"
#include "Dimensions.h"

class BDDStore : public Store
{
    private:
        unsigned int bitsNeeded(int val);

    private:
        unsigned int variables;
        unsigned int currentVariable;

        Cudd* manager;
        BDD bdd;

        void storePlaceMarking(index_t, BDD &);

    public:
        BDDStore();
        ~BDDStore();
        bool searchAndInsert();
        bool searchAndInsert(State**);
};
