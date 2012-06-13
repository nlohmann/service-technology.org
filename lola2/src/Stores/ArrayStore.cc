/*!
\author Christian K.
\file ArrayStore.cc
\status new
*/

#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Marking.h"
#include "Stores/ArrayStore.h"
#include <cstring>
#include <cstdlib>


ArrayStore::ArrayStore()
{
    store = (capacity_t*)malloc((size_t)2 * 1024 * 1024 * 1024);
    numElems = 0;
}

bool ArrayStore::searchAndInsert(NetState* ns, void**)
{
    ++calls;
    size_t stateBytes = Place::CardSignificant * sizeof(capacity_t);
    for (int i = 0; i < numElems; i++)
    {
        if (!memcmp(store + (i * Place::CardSignificant), ns->Current, stateBytes))
        {
            return true;
        }
    }
    memcpy(store + (numElems * Place::CardSignificant), ns->Current, stateBytes);
    numElems++;
    ++markings;
    return false;
}
