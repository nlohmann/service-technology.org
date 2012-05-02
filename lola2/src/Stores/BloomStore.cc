#include <cmath>
#include "Stores/BloomStore.h"
#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Marking.h"
#include "InputOutput/Reporter.h"


extern Reporter* rep;


BloomStore::BloomStore(size_t hashes) : hash_functions(hashes), hash_values(new unsigned int[hash_functions]), filter(new std::bitset<BLOOM_FILTER_SIZE>())
{
    rep->status("using Bloom filter of length %lu with %d hash functions", BLOOM_FILTER_SIZE, hash_functions);
}

BloomStore::~BloomStore()
{
    delete filter;
    delete[] hash_values;
}

inline unsigned int BloomStore::hash_sdbm() const
{
    unsigned int hash = 0;

    for (index_t p = 0; p < Place::CardSignificant; ++p)
    {
        hash = Marking::Current[p] + (hash << 6) + (hash << 16) - hash;
    }

    return hash % BLOOM_FILTER_SIZE;
}

inline unsigned int BloomStore::hash_fnv() const
{
    const unsigned int fnv_prime = 0x811C9DC5;
    unsigned int hash = 0;

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

    if (calls % 10000000 == 0)
    {
        const size_t m = BLOOM_FILTER_SIZE;
        const size_t k = hash_functions;
        const size_t n = filter->count();
        const double p = pow(1.0 - pow((1.0 - 1.0 / m), k * n) , k);
        rep->status("fill: %ld, error = %.4f", n, p) ;
    }


    /*************************
     * calculate hash values *
     *************************/

    // the first two hash functions are given explicitly
    const unsigned int hash_0 = hash_sdbm();
    hash_values[0] = hash_0;

//    if (hash_functions > 1)
//    {
        const unsigned int hash_1 = hash_fnv();
        hash_values[1] = hash_1;

        // the other hash functions can be derived: h_i(x) = h_1(x) + i * h_2(x)
        for (size_t h = 2; h < hash_functions; ++h)
        {
            hash_values[h] = (hash_0 + h * hash_1) % BLOOM_FILTER_SIZE;
        }
//    }


    /*****************************************
     * check whether marking has been stored *
     *****************************************/

    for (size_t h = 0; h < hash_functions; ++h)
    {
        // found an unset bit -> marking is new and must be stored
        if ((*filter)[hash_values[h]] == 0)
        {
            // set this and all subsequent bits to 1 (previous bits were already 1)
            for (size_t h_ = h; h_ < hash_functions; ++h_)
            {
                (*filter)[hash_values[h_]] = 1;
            }

            ++markings;
            return false;
        }
    }

    // all bits were 1 -> marking was probably stored already
    return true;
}

bool BloomStore::searchAndInsert(State**)
{
    assert(false);
    return false;
}
