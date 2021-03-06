/*!
\file FirelistStubbornStatePredicate.h
\author Karsten
\status new
\brief class for firelist generation. Use up sets.
*/

#pragma once
#include <Core/Dimensions.h>
#include <Formula/StatePredicate.h>
#include <Exploration/Firelist.h>


/// a stubborn firelist for the search for a marking fulfilling a state predicate
class FirelistStubbornStatePredicate : public Firelist
{
public:
    FirelistStubbornStatePredicate(StatePredicate *);
    ~FirelistStubbornStatePredicate();

    /// return value contains number of elements in fire list, argument is reference parameter for actual list
    virtual index_t getFirelist(NetState &, index_t **);

    /// create a new StubbornStatePredicate-Firelist form the current one
    virtual Firelist *createNewFireList(SimpleProperty *property);

private:
    StatePredicate *predicate;
    index_t *dfsStack;
    bool *onStack;
};
