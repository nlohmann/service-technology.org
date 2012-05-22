/*!
\author Gregor
\file ParallelSTLStore.cc
\status new
*/

#pragma once
#include <set>
#include <vector>
#include "Core/Dimensions.h"
#include "Stores/Store.h"
#include "Stores/SIStore.h"
#include <pthread.h>

class ThreadSafeStore : public Store
{
    private:
		pthread_rwlock_t readWriteLock;
		// the internal store
        SIStore* store;

    public:
        ThreadSafeStore(SIStore* sistore);
        ~ThreadSafeStore();
        bool searchAndInsert(int thread);
        bool searchAndInsert();
        bool searchAndInsert(State**);
};
