/*
 * HuffmanStoreAVL.cc
 *
 *  Created on: 31.05.2012
 *      Author: Max Görner
 */

#include <Stores/HuffmanStoreAVL.h>
#include <Net/Place.h>
#include <iostream>
#include <map>
//#include <Stores/BinStore2.h>

HuffmanStoreAVL::HuffmanStoreAVL() {
    //store2 = new BinStore2();
    std::map<capacity_t,int> countMap; //temp table to successfully call the huffmanStore constructor.
    he = HuffmanEncoding(&countMap);
}

HuffmanStoreAVL::~HuffmanStoreAVL() {
    delete &he;
}

bool HuffmanStoreAVL::searchAndInsert(NetState* ns, void**) {
    calls++;
    std::vector<bool> currentMarking;
    he.encodeMarking(ns->Current,&currentMarking);
    std::pair<std::set<std::vector<bool> >::iterator,bool> ret = store.insert(currentMarking);
    //bool erg = store2->searchAndInsert(ns);
    if(ret.second) {
        //Element was inserted
        markings++;
        if(markings%5000==0) {
            he.recode(&store);
        }
        //assert(!erg);
        return false; //marking wasn't seen
    }
    //assert(erg);
    return true; //marking was seen
}
