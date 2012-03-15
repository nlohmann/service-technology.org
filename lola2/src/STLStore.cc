#include <algorithm>

#include "STLStore.h"
#include "Net.h"
#include "Marking.h"

bool STLStore::searchAndInsert()
{
    // organize vector as intermediate data structure: set size on first call
    static std::vector<capacity_t> m;
    if (calls == 0)
    {
        m.resize(Net::Card[PL]);
    }

    ++calls;

    // copy current marking to vector
    std::copy(Marking::Current, Marking::Current + Net::Card[PL], m.begin());

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

bool STLStore::searchAndInsert(State**)
{
    assert(false);
    return false;
}
