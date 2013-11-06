#include <cmath>
#include <Stores/VectorStores/VBloomStore.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <InputOutput/Reporter.h>


extern Reporter *rep;


template<size_t SIZE>
VBloomStore<SIZE>::VBloomStore(index_t num_threads, size_t hashes) : hash_functions(hashes),
    filter(new std::bitset<SIZE>())
{
    hash_values = new uint32_t[hash_functions * num_threads];
    pthread_mutex_init(&mutex, NULL);

    // debug output causes performance issues when used with HashingWrapperStore
    //rep->status("using Bloom filter of length %lu with %d hash functions", SIZE, hash_functions);
}

template<size_t SIZE>
VBloomStore<SIZE>::~VBloomStore()
{
    delete filter;
    delete[] hash_values;
    pthread_mutex_destroy(&mutex);
}

template<size_t SIZE>
inline uint32_t VBloomStore<SIZE>::hash_sdbm(const vectordata_t *in, size_t len) const
{
    uint32_t hash = 0;

    while (len--)
    {
        hash = *(in++) + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

template<size_t SIZE>
inline uint32_t VBloomStore<SIZE>::hash_fnv(const vectordata_t *in, size_t len) const
{
    const unsigned int fnv_prime = 0x811C9DC5;
    uint32_t hash = 0;

    while (len--)
    {
        hash *= fnv_prime;
        hash ^= *(in++);
    }

    return hash;
}

template<size_t SIZE>
bool VBloomStore<SIZE>::searchAndInsert(const vectordata_t *in, bitindex_t bitlen, hash_t hash,
                                        void **payload, index_t threadIndex, bool noinsert)
{
    uint32_t *cur_hashes = hash_values + (threadIndex * hash_functions);
    size_t len = (bitlen + VECTOR_WIDTH - 1) / VECTOR_WIDTH;

    /*************************
     * calculate hash values *
     *************************/

    // the first two hash functions are given explicitly
    const uint32_t hash_0 = hash_sdbm(in, len);
    cur_hashes[0] = hash_0 % SIZE;

    const uint32_t hash_1 = hash_fnv(in, len);
    cur_hashes[1] = hash_1 % SIZE;

    // the other hash functions can be derived: h_i(x) = h_1(x) + i * h_2(x)
    for (size_t h = 2; h < hash_functions; ++h)
    {
        cur_hashes[h] = (hash_0 + h * hash_1) % SIZE;
    }


    /****************************************
     * check whether vector has been stored *
     ****************************************/

    for (size_t h = 0; h < hash_functions; ++h)
    {
        // found an unset bit -> vector is new and must be stored
        if ((*filter)[cur_hashes[h]] == 0)
        {
            if (noinsert)
            {
                return false;    // check if we want to insert at all
            }

            pthread_mutex_lock(&mutex); // some rare occasion where double-checked locking actually works
            if ((*filter)[cur_hashes[h]] == 0)
            {
                // set this and all subsequent bits to 1 (previous bits were already 1)
                while (h < hash_functions)
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

// gets and removes a vector from the store
template<size_t SIZE>
bool VBloomStore<SIZE>::popVector(vectordata_t *&, index_t threadIndex)
{
    rep->status("this store cannot return states");
    rep->abort(ERROR_COMMANDLINE);
    return false;
}

/*!
 * \brief Check if the store is empty
 * \return If the store is empty
 */
template<size_t SIZE>
bool VBloomStore<SIZE>::empty()
{
    rep->status("this store cannot return states");
    rep->abort(ERROR_COMMANDLINE);
    return false;
}

