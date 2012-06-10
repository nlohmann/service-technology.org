#include <cmath>
#include <Stores/EmptyStore.h>

EmptyStore::EmptyStore() : tries(0)
{
}

inline bool EmptyStore::searchAndInsert(NetState* ns, void**)
{
    ++calls;
    return true;
}
