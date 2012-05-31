#include <cmath>
#include <Stores/EmptyStore.h>

EmptyStore::EmptyStore() : tries(0)
{
}

inline bool EmptyStore::searchAndInsert()
{
    ++calls;
    return true;
}

// LCOV_EXCL_START
bool EmptyStore::searchAndInsert(State**)
{
    return true;
}
// LCOV_EXCL_STOP
