
#pragma once


#include <Formula/CTL/EUFormula.h>
#include <Formula/CTL/NotFormula.h>
#include <Formula/CTL/TrueFormula.h>

struct AGFormula : public DFSFormula
{
private:
    EUFormula euFormula;
    TrueFormula trueFormula;
    NotFormula notFormula;
public:
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

    // AG phi = !EF(!phi) = !E (T U !phi)
    bool check(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness)
    {
        notFormula.inner = inner;
        euFormula.phi = &trueFormula;
        euFormula.psi = &notFormula;
        euFormula.index = index;
        euFormula.dfsindex = dfsindex;
        euFormula.payloadsize = payloadsize;
        return !euFormula.check(s, ns, firelist, witness);
    }

    // LCOV_EXCL_START
    void DEBUG_print()
    {
        printf("[%u,%u,%zu]AG(", index, dfsindex, payloadsize);
        inner->DEBUG_print();
        printf(")");
    }
    // LCOV_EXCL_STOP

    void gatherPayloadInformation(index_t *numDFS, index_t *numCachedResults)
    {
        dfsindex = ((*numDFS)++) * sizeof(statenumber_t);
        index = ((*numCachedResults)++) * 2;
        inner->gatherPayloadInformation(numDFS, numCachedResults);
    }
    void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize)
    {
        index += cachedResultOffset * 8;
        payloadsize = payloadSize;
        inner->setPayloadInformation(cachedResultOffset, payloadSize);
    }
};
