/*!
\file LoadedBinStore.h
\status new
\brief implement Store as a decision tree with bitvectors.
*/

#pragma once
#include <cstdlib>
#include <Stores/BinStore.h>

template <typename T>
class LoadedBinStore : public BinStore
{
public:
    LoadedBinStore();
    ~LoadedBinStore();
protected:
    /// a binary decision node
    class Decision : public BinStore::Decision
    {
    public:
        Decision(bitindex_t);
        T payload;
        virtual void getPayload(void**);
    };
    // create a decision node
    virtual BinStore::Decision* createDecision(bitindex_t);

    // get payload of first vector
    virtual void getFirstPayload(hash_t, void**);
    // get payload of first vector
    virtual void getAndCreateFirstPayload(hash_t, void**);

    // payload of first states
    T** payload;
};


///////////////////////////////////////
// Implementations
///////////////////////////////////////

template <typename T>
LoadedBinStore<T>* BinStore::loadStore(const T &)
{
    delete this;
    return new LoadedBinStore<T>();
}

template <typename T>
LoadedBinStore<T>::LoadedBinStore() : BinStore()
{
    payload = (T**) calloc(SIZEOF_VOIDP, SIZEOF_MARKINGTABLE);
}

template <typename T>
LoadedBinStore<T>::~LoadedBinStore()
{
    for (hash_t i = 0; i < SIZEOF_MARKINGTABLE; i++)
    {
        if (payload[i])
        {
            delete payload[i];
        }
    }
    free(payload);
}

template <typename T>
LoadedBinStore<T>::Decision::Decision(bitindex_t index) : BinStore::Decision(index)
{
}

template <typename T>
void LoadedBinStore<T>::Decision::getPayload(void** p)
{
    if (p)
    {
        *p = &payload;
    }
}

// create a decision node
template <typename T>
BinStore::Decision* LoadedBinStore<T>::createDecision(bitindex_t b)
{
    return new Decision(b);
}

// get payload of first vector
template <typename T>
void LoadedBinStore<T>::getFirstPayload(hash_t index, void** p)
{
    if (p)
    {
        *p = payload[index];
    }
}

// get and create payload of first vector
template <typename T>
void LoadedBinStore<T>::getAndCreateFirstPayload(hash_t index, void** p)
{
    // create payload
    payload[index] = new T();
    // get payload
    getFirstPayload(index, p);
}
