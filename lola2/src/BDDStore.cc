#include <cmath>
#include <vector>

#include "BDDStore.h"
#include "Net.h"
#include "Place.h"
#include "Marking.h"

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
    manager->AutodynEnable(CUDD_REORDER_GROUP_SIFT_CONV);

    bdd = manager->bddZero();
}

BDDStore::~BDDStore()
{
    delete manager;
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
        if (tokens & 1)
        {
            temp *= manager->bddVar(currentVariable);
        }
        else
        {
            temp *= not manager->bddVar(currentVariable);
        }
    }
}

bool BDDStore::searchAndInsert()
{
    BDD temp = manager->bddOne();
    currentVariable = 0;

    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        storePlaceMarking(p, temp);
    }

    bdd += temp;

    // debug dot output
    //std::vector<BDD> nodes;
    //nodes.push_back(bdd);
    //manager->DumpDot(nodes);

    return true;
}

bool BDDStore::searchAndInsert(State** s)
{
    assert(false);
    return false;
}
