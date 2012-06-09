#include <Formula/StatePredicate.h>
#include <cstdlib>

StatePredicate* StatePredicate::top = (StatePredicate*) 0;


StatePredicate* StatePredicate::copy() {
    return copy(NULL);
}
