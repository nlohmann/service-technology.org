/*!
\author Niels
\author Max Görner
\author Christian Koch
\file Store.h
\status new
*/

#pragma once
#include <config.h>
#include <pthread.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <Net/NetState.h>

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
    void *reporter_internal(void);

    /// a helper function to start the reporter thread
    static void *reporter_helper(void *context);

protected:
    /// the maximum number of threads this store has to work with.
    /// The value is used to create thread-local auxiliary data structures if needed.
    const uint32_t number_of_threads;

    /// the number of stored markings. It is stored as an array of integers, one value per thread, to avoid lost updates when counting concurrently. The sum of all values is the actual total number of markings found.
    uint64_t *markings;

    /// the number of calls to searchAndInsert(). It is stored as an array of integers, one value per thread, to avoid lost updates when counting concurrently. The sum of all values is the actual total number of calls made.
    uint64_t *calls;

public:
    /// retrieves the current total number of markings stored.
    /// If the Store is used while calling this function, the return value might differ
    /// from the actual value (that continues increasing concurrently).
    /// The value is exact when the search is finished.
    uint64_t get_number_of_markings();

    /// retrieves the current total number of calls to searchAndInsert().
    /// If the Store is used while calling this function, the return value might differ
    /// from the actual value (that continues increasing concurrently).
    /// The value is exact when the search is finished.
    uint64_t get_number_of_calls();

    /// constructor; initialize auxiliary data structures if needed.
    /// @param number_of_threads maximum number of threads that may work with this Store concurrently.
    Store(uint32_t number_of_threads);

    /// silence the reporter thread (public, because it is used by the
    /// CycleStore)
    void silence();

    /// destructor; frees all memory used for auxiliary data structures
    virtual ~Store();

    /// checks whether the marking represented by the given NetState is already stored.
    /// @param ns NetState that needs to be checked
    /// @param payload reference to address of payload data. If the marking is found, the address is set to the (already existing) payload entry of that marking, if a new marking was added the address will be set to a newly allocated payload entry.
    /// @param thread the index of the thread that requests this call. Values will range from 0 to (number_of_threads - 1). Used to allow using thread-local auxiliary data structures without locking any variables.
    /// @param noinsert if set to true only a search is done
    /// @return true, if the marking was found in the store, otherwise false.
    virtual bool searchAndInsert(NetState &ns, T **payload, index_t thread, bool noinsert = false) = 0;

    /// gets and removes a state from the store
    /// @param ns NetState where the removed state will be written to
    /// @return false, if store was already empty
    virtual bool popState(NetState &ns);
};

#include <Stores/Store.inc>
