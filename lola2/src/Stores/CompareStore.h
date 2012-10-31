/*!
\file CompareStore.h
\author Gregor Behnke
\status new

\brief a meta-store to test, whether a new store works correctly
*/

#pragma once
#include <Core/Dimensions.h>
#include <Stores/Store.h>

template <typename T> class CompareStore : public Store<T>
{
public:
    /// creates new Compare-Store using the two given Stores
	CompareStore(Store<T>* correct, Store<T>* test,uint32_t _number_of_threads);

    /// frees both components
    ~CompareStore();

    bool searchAndInsert(NetState &ns, T** payload, uint32_t threadIndex);

    /// gets and removes a state from the store
    /// @param ns NetState where the removed state will be written to
    /// @return false, if store was already empty
    virtual bool popState(NetState &ns);

private:
    /// correct store
    Store<T>* correctStore;
    /// store to be tested
    Store<T>* testStore;
};

#include <Stores/CompareStore.inc>
