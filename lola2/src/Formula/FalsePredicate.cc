/*!
\file FalsePredicate.cc
\author Karsten
\status new

\brief derives constant predicate FALSE
*/

#include <Core/Dimensions.h>
#include <Formula/FalsePredicate.h>

FalsePredicate::FalsePredicate()
{
    value = false;
}

index_t FalsePredicate::getUpSet(index_t* stack, bool* onstack)
{
    return 0;
}

index_t FalsePredicate::countAtomic()
{
    return 0;
}

index_t FalsePredicate::collectAtomic(AtomicStatePredicate**)
{
    return 0;
}

StatePredicate* FalsePredicate::copy(StatePredicate* parent)
{
    StatePredicate* p = new FalsePredicate();
    p->parent = parent;
    p->position = position;
    return p;
}
