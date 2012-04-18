#include "Stores/BloomStore.h"
#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Marking.h"

BloomStore::BloomStore()
{
    filter = new std::bitset<BLOOM_FILTER_SIZE>();
}

BloomStore::~BloomStore()
{
    delete filter;
}

/// SDBM
inline uint64_t BloomStore::hash1() const
{
    uint64_t hash = 0;

    for (index_t p = 0; p < Place::CardSignificant; ++p)
    {
        hash = Marking::Current[p] + (hash << 6) + (hash << 16) - hash;
    }

    return hash % BLOOM_FILTER_SIZE;
}

/// FNV
inline uint64_t BloomStore::hash2() const
{
    const unsigned int fnv_prime = 0x811C9DC5;
    uint64_t hash = 0;

    for (index_t p = 0; p < Place::CardSignificant; ++p)
    {
        hash *= fnv_prime;
        hash ^= Marking::Current[p];
    }

    return hash % BLOOM_FILTER_SIZE;
}

bool BloomStore::searchAndInsert()
{
    ++calls;

    const size_t h1 = hash1();
    const size_t h2 = hash2();

    if ((*filter)[h1] == 1 and (*filter)[h2] == 1)
    {
        return true;
    }
    else
    {
        (*filter)[h1] = 1;
        (*filter)[h2] = 1;

        ++markings;

        return false;
    }
}

bool BloomStore::searchAndInsert(State**)
{
    assert(false);
    return false;
}
