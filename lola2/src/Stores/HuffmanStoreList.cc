/*
 * HuffmanStoreList.cc
 *
 *  Created on: 31.05.2012
 *      Author: max
 */

#include <Stores/HuffmanStoreList.h>
#include <Net/Place.h>
#include <iostream>
#include <map>
#include <Stores/BinStore2.h>

HuffmanStoreList::HuffmanStoreList()
{
    store2 = new BinStore2();
    std::map<capacity_t, int> countMap; //temp table to successfully call the huffmanStore constructor. TODO remove!
    countMap[0] = 1;
    countMap[1] = 1;
    he = HuffmanEncoding(&countMap);
}

HuffmanStoreList::~HuffmanStoreList()
{
    delete &he;
}

bool HuffmanStoreList::searchAndInsert(NetState* ns, void**)
{
    bool erg = store2->searchAndInsert(ns);
    if (!he.contains(ns->Current, &store))
    {
        markings++;
        he.encodeMarking(ns->Current, &store);
        if (markings % 100 == 0)
        {
            he.recode(&store);
        }
        assert(!erg);
        return false; //marking wasn't seen
    }
    assert(erg);
    return true; //marking was seen
}
