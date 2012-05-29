/*!
\author Niels
\file ListStore.cc
\status new
*/

#pragma once
#include <vector>
#include <list>
#include <Core/Dimensions.h>
#include <Stores/Store.h>


class ListStore : public Store
{
    private:
        std::list<std::vector<capacity_t> > store;
        bool contains(std::vector<capacity_t>);

    public:
        bool searchAndInsert();
        bool searchAndInsert(State**);
};
