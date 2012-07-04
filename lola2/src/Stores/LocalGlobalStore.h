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
#include <Stores/SIStore/SIStore.h>
#include <Stores/PluginStore.h>
#include <pthread.h>
#include <semaphore.h>

class LocalGlobalStore : public Store
{
    private:
        pthread_rwlock_t readWriteLock;
        // the internal store
        SIStore* sistore;
        PluginStore::VectorStore* globalStore;
        capacity_t*** localStoresMarkings;
        hash_t** localStoreHashs;
        NetState** netStates;
        uint16_t write_finished_threads;
        sem_t writeSemaphore;

        // the encoder for the netstates
        PluginStore::NetStateEncoder* netStateEncoder;

        // write back an element ofthe local store into the global one
        void writeToGlobalStore(int thread, int element);

        void initLocalGlobalStore(PluginStore::NetStateEncoder* encoder);
    public:
        LocalGlobalStore(SIStore* _sistore, uint16_t threadnumber, PluginStore::NetStateEncoder* encoder);
        LocalGlobalStore(PluginStore::VectorStore* storestore, uint16_t threadnumber, PluginStore::NetStateEncoder* encoder);
        ~LocalGlobalStore();
        virtual bool searchAndInsert(NetState &ns, index_t thread);
        virtual bool searchAndInsert(NetState &ns, void**);
        void finalize();
};
