/*!
\file ChooseTransitionHashDriven.cc
\author Karsten
\status new

\brief Selects a transition from a fire list based on counting hash values and
a priority imposed on the fire list
*/

#include <cstdlib>
#include "Exploration/ChooseTransitionHashDriven.h"
#include "Net/Net.h"
#include "Net/Marking.h"
#include "Net/Place.h"
#include "Net/Transition.h"

ChooseTransitionHashDriven::ChooseTransitionHashDriven() : table((unsigned long int*) calloc(sizeof(unsigned long int), SIZEOF_MARKINGTABLE))
{
}

ChooseTransitionHashDriven::~ChooseTransitionHashDriven()
{
    free(table);
}

index_t ChooseTransitionHashDriven::choose(index_t cardfirelist, index_t* firelist)
{
    // Selection proceeds in two phases. In phase one, we give priority to transitions
    // that 1. enter rarely visited hash buckets and 2. are early members of the fire list
    // If no transition is selected in phase 1, phase 2 selects a transition randomly.

    // initialize with a number that we can be sure it is not an index of a transition
    index_t chosen = Net::Card[TR];

    // phase 1
    for (index_t i = cardfirelist; i > 0;)
    {
        --i;
        index_t t = firelist[i];
        // compute hash value for t successor
        hash_t h = (Marking::HashCurrent + Transition::DeltaHash[t]) % SIZEOF_MARKINGTABLE;
        if (((float) rand() / (float) RAND_MAX) <= 1.0 / (1.0 + table[h]))
        {
            chosen = t;
            break;
        }
    }

    // phase 2
    if (chosen == Net::Card[TR])
    {
        chosen = firelist[rand() % cardfirelist];
    }
    ++(table[(Marking::HashCurrent + Transition::DeltaHash[chosen]) % SIZEOF_MARKINGTABLE]);

    return chosen;
}
