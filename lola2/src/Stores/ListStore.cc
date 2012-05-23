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

bool ListStore::searchAndInsert()
{
    // organize vector as intermediate data structure: set size on first call
    static std::vector<capacity_t> m;
    if (calls == 0)
    {
        m.resize(Place::CardSignificant);
    }
    ++calls;

    // copy current marking to vector
    std::copy(Marking::Current, Marking::Current + Place::CardSignificant, m.begin());

    // add vector to marking store
   if(!contains(m)){
	   ++markings;
	   store.push_back(m);
	   return false; //marking was new
   }
   return true; //marking was old
}

bool ListStore::searchAndInsert(State**)
{
    assert(false);
    return false;
}

bool ListStore::contains(std::vector<capacity_t> elem){
	for(std::list<std::vector<capacity_t> >::iterator it = store.begin(); it != store.end(); it++){
		if(*it == elem) return true;
	}
	return false;
}
