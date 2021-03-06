
#pragma once


#include <Formula/CTL/CTLFormula.h>
#include <Net/Transition.h>

struct EXFormula : public CTLFormula
{
    CTLFormula *inner;

    void initAtomics(NetState &ns)
    {
        inner->initAtomics(ns);
    }
    void updateAtomics(NetState &ns, index_t t)
    {
        inner->updateAtomics(ns, t);
    }
    void revertAtomics(NetState &ns, index_t t)
    {
        inner->revertAtomics(ns, t);
    }

    bool check(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness)
    {
        index_t *fl;
        index_t cardfl = firelist.getFirelist(ns, &fl);
        bool result;
        while (cardfl--)
        {
            Transition::fire(ns, fl[cardfl]);
            updateAtomics(ns, fl[cardfl]);

            result = inner->check(s, ns, firelist, witness);

            Transition::backfire(ns, fl[cardfl]);
            revertAtomics(ns, fl[cardfl]);

            if (result)
            {
                witness->push_back(fl[cardfl]);
                return true;
            }
            else
            {
                witness->clear();
            }
        }
        return false;
    }

    // LCOV_EXCL_START
    void DEBUG_print()
    {
        printf("EX(");
        inner->DEBUG_print();
        printf(")");
    }
    // LCOV_EXCL_STOP

    void gatherPayloadInformation(index_t *numDFS, index_t *numCachedResults)
    {
        inner->gatherPayloadInformation(numDFS, numCachedResults);
    }
    void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize)
    {
        inner->setPayloadInformation(cachedResultOffset, payloadSize);
    }
};
