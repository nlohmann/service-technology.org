
#pragma once


#include <Formula/CTL/CTLFormula.h>
#include <Exploration/StatePredicateProperty.h>

struct AtomicFormula : public CTLFormula
{
    StatePredicateProperty *inner;

    void initAtomics(NetState &ns)
    {
        inner->initProperty(ns);
    }
    void updateAtomics(NetState &ns, index_t t)
    {
        inner->checkProperty(ns, t);
    }
    void revertAtomics(NetState &ns, index_t t)
    {
        inner->updateProperty(ns, t);
    }
    bool check(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness)
    {
        return inner->getPredicate()->value;
    }

    // LCOV_EXCL_START
    void DEBUG_print()
    {
        printf("ATOMIC");
    }
    // LCOV_EXCL_STOP

    void gatherPayloadInformation(index_t *numDFS, index_t *numCachedResults)
    {
        // do nothing
    }
    void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize)
    {
        // do nothing
    }
};
