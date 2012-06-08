/*!
\author Niels
\file STLStore.cc
\status new
*/

#include <algorithm>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Stores/STLStore.h>

bool STLStore::searchAndInsert(NetState* ns)
{
    // organize vector as intermediate data structure: set size on first call
    static std::vector<capacity_t> m;
    if (calls == 0)
    {
        m.resize(Place::CardSignificant);
    }

    ++calls;

    // copy current marking to vector
    std::copy(ns->Current, ns->Current + Place::CardSignificant, m.begin());

    // add vector to marking store
    const std::pair<std::set<std::vector<capacity_t> >::iterator, bool> res = store.insert(m);

    if (res.second)
    {
        // marking was new
        ++markings;
        return false;
    }
    else
    {
        // marking was old
        return true;
    }
}

// LCOV_EXCL_START
bool STLStore::searchAndInsert(NetState* ns,State**)
{
    assert(false);
    return false;
}
// LCOV_EXCL_STOP
