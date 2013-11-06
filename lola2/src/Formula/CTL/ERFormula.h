
#pragma once


#include <Formula/CTL/NotFormula.h>
#include <Formula/CTL/AUFormula.h>

struct ERFormula : public DFSFormula
{
private:
    AUFormula auFormula;
    NotFormula notPhi;
    NotFormula notPsi;
public:
    CTLFormula *phi;
    CTLFormula *psi;

    void initAtomics(NetState &ns)
    {
        phi->initAtomics(ns);
        psi->initAtomics(ns);
    }
    void updateAtomics(NetState &ns, index_t t)
    {
        phi->updateAtomics(ns, t);
        psi->updateAtomics(ns, t);
    }
    void revertAtomics(NetState &ns, index_t t)
    {
        phi->revertAtomics(ns, t);
        psi->revertAtomics(ns, t);
    }

    // E (phi R psi) = !A (!phi U !psi)
    bool check(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness)
    {
        notPhi.inner = phi;
        notPsi.inner = psi;
        auFormula.phi = &notPhi;
        auFormula.psi = &notPsi;
        auFormula.index = index;
        auFormula.dfsindex = dfsindex;
        auFormula.payloadsize = payloadsize;
        return !auFormula.check(s, ns, firelist, witness);
    }

    // LCOV_EXCL_START
    void DEBUG_print()
    {
        printf("[%u,%u,%lu]E(", index, dfsindex, payloadsize);
        phi->DEBUG_print();
        printf(")R(");
        psi->DEBUG_print();
        printf(")");
    }
    // LCOV_EXCL_STOP

    void gatherPayloadInformation(index_t *numDFS, index_t *numCachedResults)
    {
        dfsindex = ((*numDFS)++) * sizeof(statenumber_t);
        index = ((*numCachedResults)++) * 2;
        phi->gatherPayloadInformation(numDFS, numCachedResults);
        psi->gatherPayloadInformation(numDFS, numCachedResults);
    }
    void setPayloadInformation(index_t cachedResultOffset, size_t payloadSize)
    {
        index += cachedResultOffset * 8;
        payloadsize = payloadSize;
        phi->setPayloadInformation(cachedResultOffset, payloadSize);
        psi->setPayloadInformation(cachedResultOffset, payloadSize);
    }
};
