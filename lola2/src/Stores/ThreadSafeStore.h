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
        uint16_t threadNumber;
        pthread_rwlock_t readWriteLock;
        // the internal store
        SIStore* store;
        capacity_t*** localStoresMarkings;
        hash_t** localStoreHashs;
        NetState** netStates;
        uint16_t write_finished_threads;

        pthread_mutex_t mutex1;


        sem_t writeSemaphore;
        void writeToGlobalStore(int thread);

    public:
        ThreadSafeStore(SIStore* sistore, uint16_t threadnumber);
        ~ThreadSafeStore();
        bool searchAndInsert(NetState& ns, int thread);
        virtual bool searchAndInsert(NetState& ns, void**);
        void finalize();
};
