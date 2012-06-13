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

#define HASHBINS 64

// constructor of the parallel stl store
ThreadSafeStore::ThreadSafeStore(SIStore* sistore, uint16_t _threadNumber)
{
    threadNumber = _threadNumber;
    // create a read-write lock
    pthread_rwlock_init(&readWriteLock, NULL);
    pthread_mutex_init(&mutex1, NULL);
    sem_init(&writeSemaphore, 0, 0);
    store = sistore;

    localStoresMarkings = (capacity_t***) calloc(threadNumber, SIZEOF_VOIDP);
    localStoreHashs = (hash_t**) calloc(threadNumber, SIZEOF_VOIDP);
    for (int i = 0; i < threadNumber; i++)
    {
        localStoresMarkings[i] = (capacity_t**) calloc(HASHBINS, SIZEOF_VOIDP);
        localStoreHashs[i] = (hash_t*) calloc(HASHBINS, SIZEOF_HASH_T);
    }

    netStates = (NetState**) malloc(sizeof(NetState*) * threadNumber);
    for (uint16_t i = 0; i < threadNumber; i++)
    {
        netStates[i] = NetState::createNetStateFromCurrent();
    }
}

ThreadSafeStore::~ThreadSafeStore()
{
    // ??
}

void ThreadSafeStore::writeToGlobalStore(int thread)
{

    NetState* ns = netStates[thread];

    for (int i = 0; i < HASHBINS; i++)
        if (localStoresMarkings[thread][i])
        {
            ns->HashCurrent = localStoreHashs[thread][i];
            ns->Current = localStoresMarkings[thread][i];

            if (store->search(ns, thread))
            {
                continue;
            }

            bool isIn = store->insert(ns, thread);
            if (!isIn)
            {
                pthread_mutex_lock(&mutex1);
                markings++;
                pthread_mutex_unlock(&mutex1);
            }


            free(localStoresMarkings[thread][i]);
            localStoresMarkings[thread][i] = 0;
            localStoreHashs[thread][i] = 0;
        }
}

bool ThreadSafeStore::searchAndInsert(NetState* ns, int thread)
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

    pthread_mutex_lock(&mutex1);
    calls++;
    pthread_mutex_unlock(&mutex1);


    // search in the private store
    if (localStoresMarkings[thread][ns->HashCurrent % HASHBINS])
    {
        // compare the current marking and the one stored in the local store
        bool equal = true;
        // if the hash is not equal the markings aren't also
        if (localStoreHashs[thread][ns->HashCurrent % HASHBINS]
                == ns->HashCurrent)
            for (int i = 0; i < Net::Card[PL]; i++)
            {
                equal &= localStoresMarkings[thread][ns->HashCurrent % HASHBINS][i]
                         == ns->Current[i];
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
        // collision, write the local markings in the global store and empty the store
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
        writeToGlobalStore(thread);
    }

    // write the current marking into the local store
    localStoreHashs[thread][ns->HashCurrent % HASHBINS] = ns->HashCurrent;
    localStoresMarkings[thread][ns->HashCurrent % HASHBINS] =
        (capacity_t*) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    memcpy(localStoresMarkings[thread][ns->HashCurrent % HASHBINS], ns->Current,
           Net::Card[PL] * SIZEOF_CAPACITY_T);

    return false;
}

// LCOV_EXCL_START
bool ThreadSafeStore::searchAndInsert(NetState* ns, void**)
{
    return searchAndInsert(ns, 0);
}
// LCOV_EXCL_STOP

void ThreadSafeStore::finalize()
{
    for (int i = 0; i < threadNumber; i++)
    {
        writeToGlobalStore(i);
    }
}
