/*!
\author Niels and Max G.
\file Store.h
\status new
*/

#pragma once
#include <config.h>
#include <pthread.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "Net/NetState.h"

/*!
\example

State *s = NULL;

if (searchAndInsert(&s))
{
    // already there - work with *s;
}
else
{
    // *s has just been created
}
*/
template <typename T>
class Store
{
private:
    /// a thread that runs the frequent reports
    pthread_t reporter_thread;

    /// the reporter function
    void* reporter_internal(void);

    /// a helper function to start the reporter thread
    static void* reporter_helper(void* context);

protected:
    /// the number of threads this store has to work with
    uint32_t number_of_threads;

    /// the number of stored markings, an array one value per thread
    uint64_t* markings;

    /// the number of calls to searchAndInsert(), an array one value per thread
    uint64_t* calls;

public:

    // functions to retrieve the current number of markings and calls to this store
    // while running the search, this functions may differ slightly (+- number_of_threads) fromthe actual value due to threading issues.
    // after the end of the search there values will be correct
    uint64_t get_number_of_markings();
    uint64_t get_number_of_calls();


    Store(uint32_t number_of_threads);

    virtual ~Store();

    /// check whether current marking is stored
    /// threaded version, this have not to be implemented by each store
    virtual bool searchAndInsert(NetState &ns, T** payload, index_t thread) = 0;

    /// with this method the store will calculate the actual number of markings
    /// needed as e.g. the thread-safe-store (local hashing and global bin store) must write back all the local markings to know the exact number
    virtual void finalize();
};

#include <Stores/Store.inc>
