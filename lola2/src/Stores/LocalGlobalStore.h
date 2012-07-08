/*!
\author Gregor
\file LocalGlobalStore.cc
\status new

 \brief This is a generic store, which introduces a buffer between before the actual store.
        The assumption is, that a marking may be found the neighbourhood of itself again more often than anywhere else.
        So if a marking is searched first, the store will look into the local store (based on a hashing table) and then in the global store.
        If the marking is found true will be returned. If not the search will be continued in the global store.
        If is found there true will be returned, if not it will be either
        	a) inserted directly if a VectorStore was given in the constructor
        	b) inserted on the last possible moment (if it is displaced form the local store) if an SIStore was given
        If the local hash-table is full the latter option will be identical to the first.
        At the end finalize() must be called to get the correct number of markings.
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
