#pragma once
#include <bitset>
#include "Core/Dimensions.h"
#include "Stores/Store.h"

class BloomStore : public Store
{
    private:
        std::bitset<BLOOM_FILTER_SIZE> *filter;
        inline uint64_t hash_sdbm() const;
        inline uint64_t hash_fnv() const;

        uint64_t *hash_values;
        size_t hash_functions;

    public:
        explicit BloomStore(size_t = 2);
        ~BloomStore();

        bool searchAndInsert();
        bool searchAndInsert(State**);
};
