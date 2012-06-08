#include <cmath>
#include <Stores/EmptyStore.h>

EmptyStore::EmptyStore() : tries(0)
{
}

inline bool EmptyStore::searchAndInsert(NetState* ns)
{
    ++calls;
    return true;
}

// LCOV_EXCL_STOP
bool EmptyStore::searchAndInsert(NetState* ns, State**) { return true; }
