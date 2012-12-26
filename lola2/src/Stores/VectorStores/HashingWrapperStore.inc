/*!
\file SuffixTreeStore.cc
\author Christian Koch
\status new
\brief VectorStore implementation using binary suffix trees. Based on BinStore. Relies on the assumption that different input vectors (possibly of different length) are not prefix of another.
*/

#include <Stores/VectorStores/HashingWrapperStore.h>



template <typename T>
HashingWrapperStore<T>::HashingWrapperStore(VectorStoreCreator<T>* _storeCreator)
{
	storeCreator = _storeCreator;

    buckets = (VectorStore<T>**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
    rwlocks =(pthread_rwlock_t*) calloc(sizeof(pthread_rwlock_t), SIZEOF_MARKINGTABLE);

    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
        pthread_rwlock_init(rwlocks+i, NULL);

    currentPopBucket = 0;
}

template <typename T>
HashingWrapperStore<T>::~HashingWrapperStore()
{
    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
    {
        if (buckets[i])
        {
            delete buckets[i];
        }
    }
    free(buckets);

    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
        pthread_rwlock_destroy(rwlocks+i);
    free(rwlocks);
    
    delete storeCreator;
}

/// search for an input vector in the suffix tree and insert it, if it is not present
template <typename T>
bool HashingWrapperStore<T>::searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex)
{
    // lock the current bucket to ensure thread safety
    pthread_rwlock_wrlock(rwlocks + hash);

    if(!buckets[hash])
    	buckets[hash] = (*storeCreator)();

    bool ret = buckets[hash]->searchAndInsert(in,bitlen,hash,payload,threadIndex);

    pthread_rwlock_unlock(rwlocks + hash);

    return ret;
}

/*!
 * \brief  gets and removes a vector from the store
 */
template <typename T>
bool HashingWrapperStore<T>::popVector(vectordata_t * & out)
{
	index_t startingPopBucket = currentPopBucket;
	do {
		if(buckets[currentPopBucket]->popVector(out))
			return true;
		currentPopBucket = (currentPopBucket+1)%SIZEOF_MARKINGTABLE;
	}
	while(currentPopBucket != startingPopBucket);
    return false;
}
