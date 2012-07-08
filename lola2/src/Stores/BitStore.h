/*!
\author Niels
\file BitStore.h
\status new
*/

#pragma once
#include <set>
#include <vector>
#include <Stores/Store.h>


class BitStore : public Store
{
private:
    unsigned int bitsNeeded(int val);

    unsigned int bit_count;
    unsigned int bit_current;
    unsigned int byte_count;
    unsigned int byte_current;

    std::set<std::vector<uint8_t> > store;

public:
    BitStore();
    bool searchAndInsert(NetState &ns, void**);
};
