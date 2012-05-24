#include <cmath>
#include "Stores/EmptyStore.h"

EmptyStore::EmptyStore() : tries(0)
{
}

inline bool EmptyStore::searchAndInsert()
{
    ++calls;
    return true;
}

bool EmptyStore::searchAndInsert(State**) { return true; }
