/*!
\author Niels
\file ListStore.cc
\status new
*/

#include <algorithm>
#include <iostream>
#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Marking.h"
#include "Stores/ListStore.h"

ListStore::ListStore() {
    store = (capacity_t*) malloc(SIZE);
    elemCount = 0;
}

ListStore::~ListStore() {
    free(store);
}

bool ListStore::searchAndInsert(NetState* ns, void**)
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
    if(!contains(m)) {
        ++markings;
        storeElements(m);
        return false; //marking was new
    }
    return true; //marking was old
}

bool ListStore::contains(std::vector<capacity_t> elem) {
    std::vector<capacity_t> tempVector;
    for(int i = 0; i < elemCount; i++) {
        tempVector.push_back(*(store+i));
        if(tempVector.size() == Place::CardSignificant) {
            if(tempVector == elem) {
                return true;
            } else
                tempVector.clear();
        }
    }
    return false;
}

void ListStore::storeElements(std::vector<capacity_t> elems) {
    for(std::vector<capacity_t>::iterator it = elems.begin(); it != elems.end(); it++) {
        *(store+elemCount) = *it;
        elemCount++;
    }
}
