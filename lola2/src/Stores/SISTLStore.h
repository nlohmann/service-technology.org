/*!
\author Niels
\file STLStore.cc
\status new
*/

#pragma once
#include <set>
#include <vector>
#include "Core/Dimensions.h"
#include "Stores/SIStore.h"


class SISTLStore : public SIStore
{
    private:
        std::set<std::vector<capacity_t> > store;
        std::set<std::vector<capacity_t> >::iterator* insertInfo;
        std::vector<capacity_t>* m;

    public:
        SISTLStore(int threadnum);
        //~SISTLStore();

        bool search(NetState* ns, int threadNumber);
        bool insert(NetState* ns, int threadNumber);
};
