
#pragma once


#include <Formula/CTL/DFSFormula.h>
#include <Net/NetState.h>
#include <Exploration/Firelist.h>
#include <Stores/Store.h>

struct AUFormula : public DFSFormula
{
    CTLFormula *phi;
    CTLFormula *psi; //phi until psi

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

    bool check(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness);


    // LCOV_EXCL_START
    void DEBUG_print()
    {
        printf("[%u,%u,%lu]A(", index, dfsindex, payloadsize);
        phi->DEBUG_print();
        printf(")U(");
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


    bool getFairWitness(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness,
                        fairness_data &fairness);
    bool subdivideFairnessCheck(Store<void *> &s, NetState &ns, Firelist &firelist,
                                std::vector<int> *witness, fairness_data &fairness);
    bool fairSCC(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness,
                 fairness_data &fairness);
    void constructWitness(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness,
                          fairness_data &fairness, bool *enabled_strong);
    void produceWitness(Store<void *> &s, NetState &ns, Firelist &firelist, std::vector<int> *witness,
                        fairness_data &fairness, void *initialPayload, statenumber_t initialDFS, bool *fulfilled_weak,
                        bool *fulfilled_strong, index_t initial_fulfilled_conditions);
    void findWitnessPathTo(Store<void *> &s, NetState &ns, Firelist &firelist,
                           std::vector<int> *witness, void *destinationPayload, statenumber_t initialDFS, statenumber_t myDFS);
};
