/*!
\author Niels
\file ListStore.cc
\status new
*/

#pragma once
#include <vector>
#include <list>
#include "Core/Dimensions.h"
#include "Stores/Store.h"
#define SIZE 1024*1024*32


class ListStore : public Store
{
private:
    long long elemCount;
    capacity_t* store;
    bool contains(std::vector<capacity_t>);
    void storeElements(std::vector<capacity_t>);

public:
    ListStore();
    ~ ListStore();
    bool searchAndInsert(NetState* ns);
    bool searchAndInsert(NetState* ns,State**);
};
