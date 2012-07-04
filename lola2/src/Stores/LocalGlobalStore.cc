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
#include <Stores/LocalGlobalStore.h>
#include <cstdio>
#include <string.h>

using namespace std;

#define HASHBINS 1024

void LocalGlobalStore::initLocalGlobalStore(PluginStore::NetStateEncoder* encoder) {
	netStateEncoder = encoder;
	// create a read-write lock
	pthread_rwlock_init(&readWriteLock, NULL);
	sem_init(&writeSemaphore, 0, 0);

	localStoresMarkings = (capacity_t***) calloc(number_of_threads,
			SIZEOF_VOIDP);
	localStoreHashs = (hash_t**) calloc(number_of_threads, SIZEOF_VOIDP);
	for (int i = 0; i < number_of_threads; i++) {
		localStoresMarkings[i] = (capacity_t**) calloc(HASHBINS, SIZEOF_VOIDP);
		localStoreHashs[i] = (hash_t*) calloc(HASHBINS, SIZEOF_HASH_T);
	}

	netStates = (NetState**) malloc(sizeof(NetState*) * number_of_threads);
	for (uint16_t i = 0; i < number_of_threads; i++)
		netStates[i] = NetState::createNetStateFromInitial();
}

LocalGlobalStore::LocalGlobalStore(SIStore* _sistore, uint16_t _threadNumber,
		PluginStore::NetStateEncoder* encoder) :
		Store(_threadNumber) {
	sistore = _sistore;
	globalStore = NULL;
	initLocalGlobalStore(encoder);
}

LocalGlobalStore::LocalGlobalStore(PluginStore::VectorStore* store, uint16_t _threadNumber,
		PluginStore::NetStateEncoder* encoder) :
		Store(_threadNumber) {
	globalStore = store;
	sistore = NULL;
	initLocalGlobalStore(encoder);
}

LocalGlobalStore::~LocalGlobalStore() {
	if (sistore)
		delete sistore;
	else
		delete globalStore;
	delete netStateEncoder;

	for (int i = 0; i < number_of_threads; i++){
		for (int j = 0; j < HASHBINS; j++) free(localStoresMarkings[i][j]);
		free(localStoresMarkings[i]);
		free(localStoreHashs[i]);
	}
	free(localStoresMarkings);
	free(localStoreHashs);

	for (uint16_t i = 0; i < number_of_threads; i++)
		delete netStates[i];
	free(netStates);
}

void LocalGlobalStore::writeToGlobalStore(int thread, int element) {

	NetState* ns = netStates[thread];
	capacity_t* old_current  = ns->Current;

	if (localStoresMarkings[thread][element]) {
		ns->HashCurrent = localStoreHashs[thread][element];
		ns->Current = localStoresMarkings[thread][element];

		bitindex_t bitlen;
		vectordata_t* input = netStateEncoder->encodeNetState(*ns, bitlen,
				thread);

		if (sistore->search(input, bitlen, ns->HashCurrent, thread)){
			ns->Current = old_current;
			return;
		}

		bool isIn = sistore->insert(input, bitlen, ns->HashCurrent, thread);
		if (!isIn)
			markings[thread]++;

		free(localStoresMarkings[thread][element]);
		localStoresMarkings[thread][element] = 0;
		localStoreHashs[thread][element] = 0;
	}
	ns->Current = old_current;
}

bool LocalGlobalStore::searchAndInsert(NetState &ns, index_t thread) {

	calls[thread]++;

	bool need_to_write_to_global = false;

	// search in the private store
	if (localStoresMarkings[thread][ns.HashCurrent % HASHBINS]) {
		// compare the current marking and the one stored in the local store
		bool equal;
		// if the hash is not equal the markings aren't also
		if (localStoreHashs[thread][ns.HashCurrent % HASHBINS]
				== ns.HashCurrent)
				equal = !memcmp(localStoresMarkings[thread][ns.HashCurrent % HASHBINS], ns.Current, Net::Card[PL]* SIZEOF_CAPACITY_T);
		else
			equal = false;

		if (equal)
			return true;
		// collision, write the colliding marking in the global store
		need_to_write_to_global = true;
	}

	bitindex_t bitlen;
	vectordata_t* input = netStateEncoder->encodeNetState(ns, bitlen, thread);

	// marking is not in the local store
	// search in big store
	bool isIn;
	if (sistore){
		 isIn = sistore->search(input, bitlen, ns.HashCurrent, thread);

		if (isIn)
			return true;

		// the element is in none of the two stores, insert it into my local one
		if (need_to_write_to_global)
			writeToGlobalStore(thread, ns.HashCurrent % HASHBINS);
	} else
		isIn = globalStore->searchAndInsert(input, bitlen, ns.HashCurrent, thread);

	// write the current marking into the local store
	localStoreHashs[thread][ns.HashCurrent % HASHBINS] = ns.HashCurrent;
	if (!localStoresMarkings[thread][ns.HashCurrent % HASHBINS])
		localStoresMarkings[thread][ns.HashCurrent % HASHBINS] =
			(capacity_t*) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
	memcpy(localStoresMarkings[thread][ns.HashCurrent % HASHBINS], ns.Current,
			Net::Card[PL] * SIZEOF_CAPACITY_T);

	if (!sistore && !isIn)
		markings[thread]++;
	return isIn;
}

// LCOV_EXCL_START
bool LocalGlobalStore::searchAndInsert(NetState &ns, void**) {
	return searchAndInsert(ns, (index_t)0);
}
// LCOV_EXCL_STOP

void LocalGlobalStore::finalize() {
	if (globalStore)
		return;
	for (int i = 0; i < number_of_threads; i++)
		for (int j = 0; j < HASHBINS; j++)
			writeToGlobalStore(i, j);
}
