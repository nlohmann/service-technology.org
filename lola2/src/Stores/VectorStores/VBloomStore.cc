#include <cmath>
#include <Stores/VectorStores/VBloomStore.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <InputOutput/Reporter.h>


extern Reporter* rep;


VBloomStore::VBloomStore(index_t num_threads, size_t hashes) :hash_functions(hashes), filter(new std::bitset<BLOOM_FILTER_SIZE>())
{
    hash_values = new uint32_t[hash_functions*num_threads];
    pthread_mutex_init(&mutex,NULL);
    rep->status("using Bloom filter of length %lu with %d hash functions", BLOOM_FILTER_SIZE, hash_functions);
}

VBloomStore::~VBloomStore()
{
    delete filter;
    delete[] hash_values;
    pthread_mutex_destroy(&mutex);
}

inline uint32_t VBloomStore::hash_sdbm(const vectordata_t* in, size_t len) const
{
    uint32_t hash = 0;

    while(len--)
    {
        hash = *(in++) + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

inline uint32_t VBloomStore::hash_fnv(const vectordata_t* in, size_t len) const
{
    const unsigned int fnv_prime = 0x811C9DC5;
    uint32_t hash = 0;

    while(len--)
    {
        hash *= fnv_prime;
        hash ^= *(in++);
    }

    return hash;
}

bool VBloomStore::searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex)
{
    uint32_t* cur_hashes = hash_values + (threadIndex * hash_functions);
    size_t len = (bitlen+VECTOR_WIDTH-1) / VECTOR_WIDTH;

    /*************************
     * calculate hash values *
     *************************/

    // the first two hash functions are given explicitly
    const uint32_t hash_0 = hash_sdbm(in,len);
    cur_hashes[0] = hash_0 % BLOOM_FILTER_SIZE;

    const uint32_t hash_1 = hash_fnv(in,len);
    cur_hashes[1] = hash_1 % BLOOM_FILTER_SIZE;

    // the other hash functions can be derived: h_i(x) = h_1(x) + i * h_2(x)
    for (size_t h = 2; h < hash_functions; ++h)
    {
        cur_hashes[h] = (hash_0 + h * hash_1) % BLOOM_FILTER_SIZE;
    }


    /****************************************
     * check whether vector has been stored *
     ****************************************/

    for (size_t h = 0; h < hash_functions; ++h)
    {
        // found an unset bit -> vector is new and must be stored
        if ((*filter)[cur_hashes[h]] == 0)
        {
            pthread_mutex_lock(&mutex); // some rare occasion where double-checked locking actually works
            if ((*filter)[cur_hashes[h]] == 0)
            {
                // set this and all subsequent bits to 1 (previous bits were already 1)
                while(h < hash_functions)
                {
                    (*filter)[cur_hashes[h++]] = 1;
                }
                pthread_mutex_unlock(&mutex);
                return false;
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    // all bits were 1 -> vector was probably stored already
    return true;
}
