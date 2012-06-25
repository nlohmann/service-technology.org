#include <cmath>
#include <Stores/EmptyStore.h>

EmptyStore::EmptyStore() : Store(1), tries(0)
{
}

inline bool EmptyStore::searchAndInsert(NetState &ns, void**)
{
    ++calls[0];
    return true;
}
