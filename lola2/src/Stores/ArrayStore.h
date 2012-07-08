/*!
\author Christian K.
\file ArrayStore.h
\status new
*/

#pragma once
#include "Core/Dimensions.h"
#include "Stores/Store.h"


class ArrayStore : public Store
{
private:
    capacity_t* store;
    size_t numElems;

public:
    ArrayStore();
    bool searchAndInsert(NetState &ns, void**);
};
