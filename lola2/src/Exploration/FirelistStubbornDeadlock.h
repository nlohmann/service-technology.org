/*!
\file Firelist.h
\author Karsten
\status new

\brief class for firelist generation. Default is firelist consisting of all
enabled transitions.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Exploration/Firelist.h>
#include <pthread.h>


class FirelistStubbornDeadlock : public Firelist
{
public:
    FirelistStubbornDeadlock();
    ~FirelistStubbornDeadlock();
    /// return value contains number of elements in fire list, argument is reference
    /// parameter for actual list
    virtual index_t getFirelist(NetState* ns,index_t**);

private:
    index_t* dfsStack;
    index_t* dfs;
    index_t* lowlink;
    index_t* currentIndex;
    index_t* TarjanStack;
    index_t** mustBeIncluded;
    uint32_t* visited;
    uint32_t* onTarjanStack;
    uint32_t stamp;
    void newStamp();
};

class FireListStubbornDeadlockCreator : public FireListCreator {
public:
    virtual Firelist* createFireList(SimpleProperty*);
};
