#pragma once

#include <set>
#include <vector>
#include "Core/Dimensions.h"
#include "Stores/Store.h"

class STLStore : public Store
{
    private:
        std::set<std::vector<capacity_t> > store;

    public:
        bool searchAndInsert();
        bool searchAndInsert(State**);
};
