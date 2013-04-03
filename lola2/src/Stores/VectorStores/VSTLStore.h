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
    /// mutex
    pthread_rwlock_t rwlock;

    /// actual store
    std::map<std::vector<vectordata_t>,T> store;
    /// one cache for each thread
    std::vector<vectordata_t>* intermediate;

    /// whether we only have one thread
    bool singleThreaded;


public:
    /// constructor
    /// @param num_threads the maximum number of threads this store has to work with
    explicit VSTLStore(index_t num_threads);
    /// destructor
    virtual ~VSTLStore();

    /// searches for a vector and inserts if not found
    /// @param in vector to be seached for or inserted
    /// @param bitlen length of vector
    /// @param hash of current NetState
    /// @param payload pointer to be set to the place where the payload of this state will be held
    /// @param threadIndex the index of the thread that requests this call. Values will range from 0 to (number_of_threads - 1). Used to allow using thread-local auxiliary data structures without locking any variables.
	/// @param noinsert if set to true only a search is done
    /// @return true, if the marking was found in the store, otherwise false.
    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, T** payload, index_t threadIndex, bool noinsert = false);

    /// gets and removes a vector from the store
    /// @param out place where the returned vector will be written to
    /// @return false, if the store was already empty, otherwise true
    virtual bool popVector(vectordata_t * & out, index_t threadIndex = 0);

	/// check if the store is empty
	/// @return true, if the store is empty
	virtual bool empty();
};

#include <Stores/VectorStores/VSTLStore.inc>

template <>
class VSTLStore<void> : public VectorStore<void>
{
private:
    /// mutex
    pthread_rwlock_t rwlock;

    /// actual store
    std::set<std::vector<vectordata_t> > store;
    /// one cache for each thread
    std::vector<vectordata_t>* intermediate;

    /// whether we only have one thread
    bool singleThreaded;


public:
    /// constructor
    /// @param num_threads the maximum number of threads this store has to work with
    explicit VSTLStore(index_t num_threads);
    /// destructor
    virtual ~VSTLStore();

    /// searches for a vector and inserts if not found
    /// @param in vector to be seached for or inserted
    /// @param bitlen length of vector
    /// @param hash of current NetState
    /// @param payload pointer to be set to the place where the payload of this state will be held
    /// @param threadIndex the index of the thread that requests this call. Values will range from 0 to (number_of_threads - 1). Used to allow using thread-local auxiliary data structures without locking any variables.
	/// @param noinsert if set to true only a search is done
    /// @return true, if the marking was found in the store, otherwise false.
    virtual bool searchAndInsert(const vectordata_t* in, bitindex_t bitlen, hash_t hash, void** payload, index_t threadIndex, bool noinsert);

    /// gets and removes a vector from the store
    /// @param out place where the returned vector will be written to
    /// @return false, if the store was already empty, otherwise true
    virtual bool popVector(vectordata_t * & out, index_t threadIndex = 0);

	/// check if the store is empty
	/// @return true, if the store is empty
	virtual bool empty();
};
