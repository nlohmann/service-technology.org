#include <Formula/StatePredicate.h>
#include <cstdlib>

StatePredicate *StatePredicate::top = (StatePredicate *) 0;


StatePredicate *StatePredicate::copy()
{
    return copy(NULL);
}

bool StatePredicate::isOrNode() const
{
    return false;
}

index_t StatePredicate::countUnsatisfied() const
{
    return 0;
}
