/*!
 \author Max Görner
 \file HuffmanStoreList.h
 \status new
 */

#pragma once
#include <vector>
//#include <Core/Dimensions.h>
#include <Stores/Store.h>
#include <Compressors/Huffman.h>

class HuffmanStoreList: public Store {

private:
    Store* store2;
    HuffmanEncoding he;
    std::vector<bool> store;

public:
    HuffmanStoreList();
    ~HuffmanStoreList();
    bool searchAndInsert(NetState* ns);
    bool searchAndInsert(NetState* ns,State**);
};
