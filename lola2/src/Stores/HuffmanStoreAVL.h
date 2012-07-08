/*!
 \author Max Görner
 \file HuffmanStoreAVL.h
 \status new
 */

#pragma once
#include <vector>
#include <set>
//#include <Core/Dimensions.h>
#include <Stores/Store.h>
#include <Compressors/Huffman.h>

class HuffmanStoreAVL: public Store
{

private:
    //Store* store2;
    HuffmanEncoding he;
    std::set<std::vector<bool> > store;

public:
    HuffmanStoreAVL();
    ~HuffmanStoreAVL();
    bool searchAndInsert(NetState &ns, void**);
};
