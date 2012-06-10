/*!
\author Niels
\file BitStore.h
\status new
*/

#pragma once
#include <Stores/Store.h>


class EmptyStore : public Store
{
public:
    int tries;

    EmptyStore();
    bool searchAndInsert(NetState* ns, void** = NULL);
};
