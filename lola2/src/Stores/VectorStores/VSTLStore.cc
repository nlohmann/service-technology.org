/*!
\author Niels, Christian Koch
\file VSTLStore.cc
\status new
*/

#include <algorithm>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Stores/VectorStores/VSTLStore.h>

VSTLStore::VSTLStore(index_t num_threads)
{
    singleThreaded = (num_threads == 1);
    intermediate = new std::vector<vectordata_t>[num_threads];
    pthread_rwlock_init(&rwlock, NULL);
}

VSTLStore::~VSTLStore()
{
    pthread_rwlock_destroy(&rwlock);
    delete[] intermediate;
}

bool VSTLStore::searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, index_t threadIndex)
{
    index_t vectorLength = (bitlen+(VECTOR_WIDTH-1))/VECTOR_WIDTH;

    std::vector<vectordata_t> &ci = intermediate[threadIndex];

    ci.resize(vectorLength);
    // copy current marking to vector
    std::copy(in, in + vectorLength, ci.begin());

    // shift away unused bits to make sure they are zero
    bitlen -= vectorLength * VECTOR_WIDTH;
    if(bitlen)
        (ci[vectorLength-1] >>= (VECTOR_WIDTH-bitlen)) <<= (VECTOR_WIDTH-bitlen);

    // avoid locking overhead if working in single-threaded mode
    if(singleThreaded)
        return !store.insert(ci).second;

    // test if already inserted
    pthread_rwlock_rdlock(&rwlock);
    int found = store.count(ci);
    pthread_rwlock_unlock(&rwlock);
    if(found) {
        return true;
    }

    // add vector to marking store
    pthread_rwlock_wrlock(&rwlock);
    bool ret;
    //if(!store.empty())
    //	store.insert(--lbi, ci);
    //else
    ret = !store.insert(ci).second;
    pthread_rwlock_unlock(&rwlock);

    return ret;
}
