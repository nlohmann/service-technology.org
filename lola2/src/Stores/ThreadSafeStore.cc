/*!
 \author Gregor
 \file ParallelSTLStore.cc
 \status new
 */

#include <algorithm>
#include <iostream>
#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Marking.h"
#include "Stores/ThreadSafeStore.h"

using namespace std;

// constructor of the parallel stl store
ThreadSafeStore::ThreadSafeStore(SIStore* sistore) {
	// create a read-write lock
	pthread_rwlock_init(&readWriteLock, NULL);
	store = sistore;
}

ThreadSafeStore::~ThreadSafeStore() {
	// ??
}

bool ThreadSafeStore::searchAndInsert(int thread) {

	calls++;
	//if (calls % 10000 == 0) cout << calls << " -> " << markings << endl;
	// try to get the read lock
	pthread_rwlock_rdlock(&readWriteLock);
	uint64_t isIn = store->search(thread);
	pthread_rwlock_unlock(&readWriteLock);
	if (isIn == 0)
		return true;
	// now we need the write lock
	pthread_rwlock_wrlock(&readWriteLock);
	// if the value is not in the store, insert it
	store->insert(thread);
	pthread_rwlock_unlock(&readWriteLock);
	// we have a new marking
	markings++;
	return false;
}

bool ThreadSafeStore::searchAndInsert() {
	return searchAndInsert(0);
}

bool ThreadSafeStore::searchAndInsert(State**) {
	assert(false);
	return false;

}
