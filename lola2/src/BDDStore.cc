#include <cmath>
#include <vector>

#include "BDDStore.h"
#include "Net.h"
#include "Place.h"
#include "Marking.h"
#include "Reporter.h"

extern Reporter* rep;

unsigned int BDDStore::bitsNeeded(int val)
{
    if (val == -1)
    {
        return 32;
    }
    else
    {
        return (unsigned int)ceil(log2(val + 1));
    }
}

BDDStore::BDDStore() : variables(0), currentVariable(0), manager(NULL)
{
    // determine number of Boolean variables need to store one marking
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        variables += bitsNeeded(Place::Capacity[p]);
    }

    // create an empty BDD
    manager = new Cudd(variables);
//    manager->AutodynEnable(CUDD_REORDER_GROUP_SIFT_CONV);

    bdd = manager->bddZero();
    rep->status("created BDD with %d nodes and %d variables", bdd.nodeCount(), variables);
}

BDDStore::~BDDStore()
{
//    delete manager;
}

void BDDStore::storePlaceMarking(index_t p, BDD &temp)
{
    // get copy of the token number as the later loop destroys it
    capacity_t tokens = Marking::Current[p];

    // iterate to the width of the marking according to the place capacity
    // see http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
    for (size_t bit = 0; bit < bitsNeeded(Place::Capacity[p]); ++bit, ++currentVariable, tokens >>= 1)
    {
        // check if bit is set
        temp *= (tokens & 1)
            ? manager->bddVar(currentVariable)
            : not manager->bddVar(currentVariable);
    }
}

bool BDDStore::searchAndInsert()
{
    ++calls;

    BDD temp = manager->bddOne();

    currentVariable = 0;
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        storePlaceMarking(p, temp);
    }

    BDD bdd_old = bdd;
    bdd += temp;

    if (bdd_old == bdd) {
        return true;
    }

    ++markings;

    if (markings % 10000 == 0 and markings > 0) {
        const int nodeCountOld = bdd.nodeCount();
        manager->ReduceHeap(CUDD_REORDER_GROUP_SIFT_CONV, 0);
        const int nodeCountNew = bdd.nodeCount();
        rep->status("reordering BDD: saved %d nodes - %d nodes now", nodeCountOld - nodeCountNew, nodeCountNew);
    }

    return false;
}

bool BDDStore::searchAndInsert(State** s)
{
    assert(false);
    return false;
}
