#pragma once
#include <bitset>
#include "Core/Dimensions.h"
#include "Stores/Store.h"

class BloomStore : public Store
{
    private:
        std::bitset<BLOOM_FILTER_SIZE> *filter;
        inline uint64_t hash1() const;
        inline uint64_t hash2() const;

    public:
        BloomStore();
        ~BloomStore();

        bool searchAndInsert();
        bool searchAndInsert(State**);
};
