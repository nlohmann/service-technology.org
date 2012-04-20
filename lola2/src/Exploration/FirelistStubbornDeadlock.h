/*!
\file Firelist.h
\author Karsten
\status new

\brief class for firelist generation. Default is firelist consisting of all
enabled transitions.
*/

#pragma once
#include "Core/Dimensions.h"
#include "Exploration/Firelist.h"


class FirelistStubbornDeadlock : public Firelist
{
    public:
        FirelistStubbornDeadlock();
            /// return value contains number of elements in fire list, argument is reference
            /// parameter for actual list
        virtual index_t getFirelist(index_t**);

    private: 
        index_t * dfsStack;
        index_t * dfs;
        index_t * lowlink;
        index_t * currentIndex;
        index_t * TarjanStack;
        index_t ** mustBeIncluded;
        uint32_t * visited;
        uint32_t * onTarjanStack;
        uint32_t stamp;
        uint32_t newStamp();
};
