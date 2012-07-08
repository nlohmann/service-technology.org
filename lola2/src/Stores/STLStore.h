/*!
\author Niels
\file STLStore.cc
\status new
*/

#pragma once
#include <set>
#include <vector>
#include <Core/Dimensions.h>
#include <Stores/Store.h>


class STLStore : public Store
{
private:
    std::set<std::vector<capacity_t> > store;

public:
    STLStore() : Store(1) {};
    bool searchAndInsert(NetState &ns, void**);
};
