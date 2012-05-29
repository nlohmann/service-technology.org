#pragma once
#include <bitset>
#include <Core/Dimensions.h>
#include <Stores/Store.h>

class BloomStore : public Store
{
    private:
        std::bitset<BLOOM_FILTER_SIZE>* filter;
        inline unsigned int hash_sdbm() const;
        inline unsigned int hash_fnv() const;

        unsigned int* hash_values;
        const size_t hash_functions;

    public:
        explicit BloomStore(size_t = 2);
        ~BloomStore();

        bool searchAndInsert();
        bool searchAndInsert(State**);
};
