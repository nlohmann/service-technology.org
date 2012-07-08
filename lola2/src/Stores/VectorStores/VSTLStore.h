/*!
\author Niels, Christian Koch
\file VSTLStore.cc
\status new
*/

#pragma once
#include <set>
#include <vector>
#include <Core/Dimensions.h>
#include <Stores/PluginStore.h>


/// prepended "V" to avoid name clash
class VSTLStore : public PluginStore::VectorStore
{
private:
    pthread_rwlock_t rwlock;

    std::set<std::vector<vectordata_t> > store;
    std::vector<vectordata_t>* intermediate;

    bool singleThreaded;


public:
    explicit VSTLStore(index_t num_threads);
    virtual ~VSTLStore();

    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex);
};
