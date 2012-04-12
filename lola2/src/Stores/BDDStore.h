#pragma once

#include <cuddObj.hh>
#include "Dimensions.h"
#include "Stores/Store.h"

class BDDStore : public Store
{
    private:
        unsigned int bitsNeeded(int val);

    private:
        unsigned int variables;
        unsigned int currentVariable;

        Cudd* manager;
        BDD bdd;

    public:
        BDDStore();
        ~BDDStore();
        bool searchAndInsert();
        bool searchAndInsert(State**);
};
