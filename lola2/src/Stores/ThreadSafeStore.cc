/*!
 \author Gregor
 \file ThreadSafeStore.cc
 \status new
 */

#include <algorithm>
#include <iostream>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Stores/ThreadSafeStore.h>
#include <cstdio>
#include <string.h>

using namespace std;

#define HASHBINS 1024

// constructor of the parallel stl store
ThreadSafeStore::ThreadSafeStore(SIStore* sistore, uint16_t _threadNumber) : Store(_threadNumber)
{
    // create a read-write lock
    pthread_rwlock_init(&readWriteLock, NULL);
    sem_init(&writeSemaphore, 0, 0);
    store = sistore;

    localStoresMarkings = (capacity_t***) calloc(number_of_threads, SIZEOF_VOIDP);
    localStoreHashs = (hash_t**) calloc(number_of_threads, SIZEOF_VOIDP);
    for (int i = 0; i < number_of_threads; i++)
    {
        localStoresMarkings[i] = (capacity_t**) calloc(HASHBINS, SIZEOF_VOIDP);
        localStoreHashs[i] = (hash_t*) calloc(HASHBINS, SIZEOF_HASH_T);
    }

    netStates = (NetState**) malloc(sizeof(NetState*) * number_of_threads);
    for (uint16_t i = 0; i < number_of_threads; i++)
    {
        netStates[i] = NetState::createNetStateFromInitial();
    }
}

ThreadSafeStore::~ThreadSafeStore()
{
    // ??
}

void ThreadSafeStore::writeToGlobalStore(int thread, int element)
{

    NetState* ns = netStates[thread];

        if (localStoresMarkings[thread][element])
        {
            ns->HashCurrent = localStoreHashs[thread][element];
            ns->Current = localStoresMarkings[thread][element];

            if (store->search(*ns, thread))
                return;

            bool isIn = store->insert(*ns, thread);
            if (!isIn)
                markings[thread]++;


            free(localStoresMarkings[thread][element]);
            localStoresMarkings[thread][element] = 0;
            localStoreHashs[thread][element] = 0;
        }
}

bool ThreadSafeStore::searchAndInsert(NetState &ns, int thread)
{

    /*pthread_mutex_lock(&mutex1);
     calls++;
     pthread_mutex_unlock(&mutex1);

     uint64_t in = store->search(ns, thread);
     if (in == 0)
     return true;
     in = store->insert(ns, thread);
     if (in == 0)
     return true;
     pthread_mutex_lock(&mutex1);
     markings++;
     pthread_mutex_unlock(&mutex1);

     return false;*/

    bool need_to_write_to_global = false;

    calls[thread]++;


    // search in the private store
    if (localStoresMarkings[thread][ns.HashCurrent % HASHBINS])
    {
        // compare the current marking and the one stored in the local store
        bool equal = true;
        // if the hash is not equal the markings aren't also
        if (localStoreHashs[thread][ns.HashCurrent % HASHBINS]
                == ns.HashCurrent)
            for (int i = 0; i < Net::Card[PL]; i++)
            {
                equal &= localStoresMarkings[thread][ns.HashCurrent % HASHBINS][i]
                         == ns.Current[i];
                if (!equal)
                {
                    break;
                }
            }
        else
        {
            equal = false;
        }

        if (equal)
        {
            return true;
        }
        // collision, write the colliding marking in the global store
        need_to_write_to_global = true;
    }

    // marking is not in the local store
    // search in big store
    bool isIn = store->search(ns, thread);

    // if this store says 0, the element is contained
    if (isIn)
    {
        return true;
    }

    // the element is in none of the two stores, insert it into my local one
    if (need_to_write_to_global)
    {
        writeToGlobalStore(thread, ns.HashCurrent % HASHBINS);
    }

    // write the current marking into the local store
    localStoreHashs[thread][ns.HashCurrent % HASHBINS] = ns.HashCurrent;
    localStoresMarkings[thread][ns.HashCurrent % HASHBINS] =
        (capacity_t*) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    memcpy(localStoresMarkings[thread][ns.HashCurrent % HASHBINS], ns.Current,
           Net::Card[PL] * SIZEOF_CAPACITY_T);

    return false;
}

// LCOV_EXCL_START
bool ThreadSafeStore::searchAndInsert(NetState &ns, void**)
{
    return searchAndInsert(ns, 0);
}
// LCOV_EXCL_STOP

void ThreadSafeStore::finalize()
{
	for (int i = 0; i < number_of_threads; i++)
		for (int j = 0; j < HASHBINS; j++)
			writeToGlobalStore(i,j);
}
