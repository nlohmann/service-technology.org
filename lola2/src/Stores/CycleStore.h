/*!
\author Niels
\file CycleStore.h
\status new

\brief A wrapper for the cycle reduction that internally uses another store and only manipulates the searchAndInsert function.
*/

#pragma once
#include <Stores/Store.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <Net/LinearAlgebra.h>

template <typename T>
class CycleStore : public Store<T>
{
public:
    ~CycleStore();
    CycleStore(uint32_t number_of_threads, Store<T> *actualStore, int k);
    bool searchAndInsert(NetState &ns, T **payload, index_t thread, bool noinsert = false);
    bool empty();

private:
    /// the actual store
    Store<T> *actualStore;

    /// an array storing which transitions cover the cycles
    bool *u;

    /// a heuristic parameter
    int k;
};

template <typename T>
CycleStore<T>::~CycleStore()
{
    delete[] u;
    delete actualStore;
}

template <typename T>
CycleStore<T>::CycleStore(uint32_t number_of_threads, Store<T> *actualStore, int k) :
    Store<T>(number_of_threads), actualStore(actualStore),
    u(new bool[Net::Card[TR]]), k(k)
{
    // required to avoid an assertion error (TODO: Why?)
    Net::sortAllArcs();

    // get and reduce incidence matrix (transitions are lines)
    Matrix m = Net::getIncidenceMatrix(TR);
    m.reduce();

    size_t count = 0;
    for (index_t i = 0; i < Net::Card[TR]; i++)
    {
        u[i] = !m.isSignificant(i);
        if (u[i])
        {
            ++count;
        }
    }
    rep->status("found %d transitions to cover the cycles", count);
    rep->status("cycle heuristic: %d (%s)", k, rep->markup(MARKUP_PARAMETER, "--cycleheuristic").str());

    // to avoid two concurrent reporters, silence one
    actualStore->silence();
}

template <typename T>
inline bool CycleStore<T>::searchAndInsert(NetState &ns, T **payload, index_t thread, bool noinsert)
{
    // count calls
    ++(this->calls[thread]);

    // enabled transitions
    bool *enabled = new bool[Net::Card[TR]];

    // get enabled transitions
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        enabled[i] = true;
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            if (ns.Current[Net::Arc[TR][PRE][i][j]] < Net::Mult[TR][PRE][i][j])
            {
                enabled[i] = false;
                break;
            }
        }
    }

    // check whether state should be saved
    noinsert = true;

    for (int i = 0; i < Net::Card[TR]; i++)
    {
        if (enabled[i] && u[i])
        {
            noinsert = false;
            break;
        }
    }

    const bool ret = actualStore->searchAndInsert(ns, payload, thread, noinsert);
    if (!ret && !noinsert)
    {
        ++(this->markings[thread]);
    }

    delete[] enabled;

    return ret;
}

template <typename T>
inline bool CycleStore<T>::empty()
{
    return actualStore->empty();
}
