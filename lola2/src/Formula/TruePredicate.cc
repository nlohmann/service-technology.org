/*!
\file TruePredicate.cc
\author Karsten
\status new

\brief derives constant predicate TRUE
*/

#include <Core/Dimensions.h>
#include <Formula/TruePredicate.h>
#include <Formula/FalsePredicate.h>

TruePredicate::TruePredicate()
{
    value = true;
}

index_t TruePredicate::getUpSet(index_t* stack, bool* onstack)
{
    return 0;
}

index_t TruePredicate::countAtomic() const
{
    return 0;
}

index_t TruePredicate::collectAtomic(AtomicStatePredicate**)
{
    return 0;
}

StatePredicate* TruePredicate::copy(StatePredicate* parent)
{
    TruePredicate* p = new TruePredicate();
    p->parent = parent;
    p->position = position;
    return p;
}

index_t TruePredicate::getSubs(const StatePredicate* const** subs) const
{
	return 0;
}

StatePredicate* TruePredicate::negate(){
	return new FalsePredicate();
}

