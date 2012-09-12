/*!
\author Niels, Christian Koch
\file VSTLStore.cc
\status new
*/

#pragma once
#include <set>
#include <map>
#include <vector>
#include <Core/Dimensions.h>
#include <Stores/VectorStores/VectorStore.h>
#include <pthread.h>


/// prepended "V" to avoid name clash
template <typename T>
class VSTLStore : public VectorStore<T>
{
private:
    pthread_rwlock_t rwlock;

    std::map<std::vector<vectordata_t>,T> store;
    std::vector<vectordata_t>* intermediate;

    bool singleThreaded;


public:
    explicit VSTLStore(index_t num_threads);
    virtual ~VSTLStore();

    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex);
};

#include <Stores/VectorStores/VSTLStore.inc>

template <>
class VSTLStore<void> : public VectorStore<void>
{
private:
    pthread_rwlock_t rwlock;

    std::set<std::vector<vectordata_t> > store;
    std::vector<vectordata_t>* intermediate;

    bool singleThreaded;


public:
    explicit VSTLStore(index_t num_threads);
    virtual ~VSTLStore();

    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, void** payload, index_t threadIndex);
};
