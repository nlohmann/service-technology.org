#pragma once
#include <bitset>
#include <Core/Dimensions.h>
#include <Stores/VectorStores/VectorStore.h>

class VBloomStore : public VectorStore<void>
{
private:
    std::bitset<BLOOM_FILTER_SIZE>* filter;
    inline uint32_t hash_sdbm(const vectordata_t* in, size_t len) const;
    inline uint32_t hash_fnv(const vectordata_t* in, size_t len) const;

    pthread_mutex_t mutex;

    uint32_t* hash_values;
    const size_t hash_functions;

public:
    explicit VBloomStore(index_t num_threads, size_t = 2);
    ~VBloomStore();

    bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, void** payload, index_t threadIndex);
};
