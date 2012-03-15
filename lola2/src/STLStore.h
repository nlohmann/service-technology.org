#pragma once

#include <set>
#include <vector>
#include "Dimensions.h"
#include "Store.h"

class STLStore : public Store
{
    private:
        std::set<std::vector<capacity_t> > store;

    public:
        bool searchAndInsert();
        bool searchAndInsert(State**);
};
