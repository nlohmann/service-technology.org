/*!
\file HashingWrapperStore.inc
\author Christian Koch
\status new
\brief VectorStore wrapper for bucketing.
*/

#include <Stores/VectorStores/HashingWrapperStore.h>



template <typename T>
HashingWrapperStore<T>::HashingWrapperStore(VectorStoreCreator<T>* storeCreator, index_t _number_of_buckets, index_t _number_of_threads)
{
	//storeCreator = _storeCreator;
	number_of_buckets = _number_of_buckets;

    buckets = (VectorStore<T>**) calloc(SIZEOF_VOIDP, number_of_buckets);
    //rwlocks =(pthread_rwlock_t*) calloc(sizeof(pthread_rwlock_t), number_of_buckets);

    for (hash_t i = 0; i < number_of_buckets; i++) {
    	buckets[i] = (*storeCreator)();
        //pthread_rwlock_init(rwlocks+i, NULL);
    }

    currentPopBucket = (index_t*) calloc(SIZEOF_INDEX_T, _number_of_threads);
    
    // all initialization done, delete the given creator (for comfortable one-line creation)
    delete storeCreator;
}

template <typename T>
HashingWrapperStore<T>::~HashingWrapperStore()
{
    for (hash_t i = 0; i < number_of_buckets; i++)
    {
        //if (buckets[i])
        //{
            delete buckets[i];
        //}
    }
    free(buckets);

    //for (hash_t i = 0; i < number_of_buckets; i++)
    //    pthread_rwlock_destroy(rwlocks+i);
    //free(rwlocks);

	free(currentPopBucket);
}

/// search for an input vector in the suffix tree and insert it, if it is not present
template <typename T>
bool HashingWrapperStore<T>::searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex, bool noinsert)
{
	/*if (!buckets[hash]) {
	    // lock the current bucket to ensure thread safety
	    pthread_rwlock_wrlock(rwlocks + hash);

	    if(!buckets[hash])
	    	buckets[hash] = (*storeCreator)();

	    pthread_rwlock_unlock(rwlocks + hash);
	}*/

    return buckets[hash]->searchAndInsert(in,bitlen,hash,payload,threadIndex,noinsert);
}

/*!
 * \brief  gets and removes a vector from the store
 */
template <typename T>
bool HashingWrapperStore<T>::popVector(vectordata_t * & out, index_t threadIndex)
{
	hash_t startingPopBucket = currentPopBucket[threadIndex];
	do {
		if(buckets[currentPopBucket[threadIndex]]->popVector(out,threadIndex))
			return true;
		currentPopBucket[threadIndex] = (currentPopBucket[threadIndex]+1)%number_of_buckets;
	}
	while(currentPopBucket[threadIndex] != startingPopBucket);
    return false;
}

/*!
 * \brief Check if the store is empty.
 * \return If the store is empty
 */
template <typename T>
bool HashingWrapperStore<T>::empty()
{
	// A better solution is needed if the number of buckets can be large
	// At this time, only the SweepLine method uses empty(), and only
	// with a very small number of buckets (per progress value and mode)
	for(hash_t i=0; i<number_of_buckets; ++i)
		if(!buckets[i]->empty())
			return false;
    return true;
}

