/*!
\author Gregor
\file ThreadSafeStore.cc
\status new
*/

#pragma once
#include <set>
#include <vector>
#include <Core/Dimensions.h>
#include <Stores/Store.h>
#include <Stores/SIStore.h>
#include <pthread.h>
#include <semaphore.h>

class ThreadSafeStore : public Store
{
    private:
        pthread_rwlock_t readWriteLock;
        // the internal store
        SIStore* store;
        capacity_t*** localStoresMarkings;
        hash_t** localStoreHashs;
        NetState** netStates;
        uint16_t write_finished_threads;

        sem_t writeSemaphore;
        // write back an element ofthe local store into the global one
        void writeToGlobalStore(int thread, int element);

    public:
        ThreadSafeStore(SIStore* sistore, uint16_t threadnumber);
        ~ThreadSafeStore();
        bool searchAndInsert(NetState &ns, int thread);
        virtual bool searchAndInsert(NetState &ns, void**);
        void finalize();
};
