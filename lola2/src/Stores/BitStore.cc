#include <cmath>
#include <iostream>

#include "Stores/BitStore.h"
#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Marking.h"
#include "InputOutput/Reporter.h"

extern Reporter* rep;

unsigned int BitStore::bitsNeeded(int val)
{
    if (val == -1)
    {
        return 32;
    }
    else
    {
        return (unsigned int)ceil(log2(val + 1));
    }
}

BitStore::BitStore() : bit_count(0), bit_current(0), byte_count(0), byte_current(0)
{
    // determine number of Boolean variables need to store one marking
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        bit_count += bitsNeeded(Place::Capacity[p]);
    }

    byte_count = ceil(bit_count / 8.0);

    rep->status("using %d bytes per marking, wasting %d bits per marking", byte_count, bit_count % 8);
}

bool BitStore::searchAndInsert()
{
    ++calls;

    // create empty vector
    std::vector<uint8_t> m(byte_count, 0);

    bit_current = 0;
    byte_current = 0;
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        // get copy of the token number as the later loop destroys it
        capacity_t tokens = Marking::Current[p];

        // iterate to the width of the marking according to the place capacity
        // see http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
        for (size_t i = 0; i < bitsNeeded(Place::Capacity[p]); ++i, ++bit_current, tokens >>= 1)
        {
            if (bit_current % 8 == 0)
            {
                ++byte_current;
            }

            // check if bit is set
            m[byte_current] += ((tokens & 1) << (bit_current % 8));
        }
    }

    // add vector to marking store
    const std::pair<std::set<std::vector<uint8_t> >::iterator, bool> res = store.insert(m);

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

bool BitStore::searchAndInsert(State**)
{
    assert(false);
    return false;
}
