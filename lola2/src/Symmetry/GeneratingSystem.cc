
#include <sys/time.h>
#include <sys/resource.h>

#include <math.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <sys/time.h>

#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Net/Marking.h>

#include <InputOutput/Reporter.h>

#include <Symmetry/SCC.h>
#include <Symmetry/GeneratingSystem.h>
#include <Symmetry/Permutation.h>
#include <Symmetry/PartitionRefinement.h>
#include <Symmetry/Refiners.h>

void GeneratingSystem::create()
{
    rep->status("Determine symmetry group");

    // timers
    struct timeval start, end;

    rep->status("Perform petri net refinements");
    gettimeofday(&start, NULL);

    // make partition
    Partition partition = Partition(Net::Card[PL] + Net::Card[TR]);

    // separate places and transitions
    bool *types = new bool[Net::Card[PL] + Net::Card[TR]];
    memset(types, false, sizeof(bool) * Net::Card[PL]);
    memset(&types[Net::Card[PL]], true, sizeof(bool) * Net::Card[TR]);
    refineAll<BoolArrayRefinement>(BoolArrayRefinement(&partition, types));

    // refine net by in- and outdegree
    refineAll<DegreeRefinement>(DegreeRefinement(&partition, PRE));
    refineAll<DegreeRefinement>(DegreeRefinement(&partition, POST));

    // refine net by in- and outdegree w.r.t. multiplicity
    // TODO!

    // refine net by fairness
    refineTransitions<FairnessRefinement>(FairnessRefinement(&partition));

    // refine net by initial marking
    refinePlaces<UnboundedArrayRefinement<capacity_t> >(UnboundedArrayRefinement<capacity_t>(&partition,
            Marking::Initial));

    gettimeofday(&end, NULL);
    std::cout << end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6 << " ";
    rep->status("Perform SCC refinements");
    gettimeofday(&start, NULL);
    SCC scc = SCC();
    Partition sccp = Partition(scc.sccs);

    // refine SCCs by their sizes
    refineAll<SCCSizeRefinement>(SCCSizeRefinement(&sccp, &scc));

    // refine SCCs by cardinality w.r.t. to other SCCs
    refineAll<BoundedArrayRefinement>(BoundedArrayRefinement(&sccp, scc.card[PRE]));
    refineAll<BoundedArrayRefinement>(BoundedArrayRefinement(&sccp, scc.card[POST]));

    // refine SCCs by number of arcs inside each SCC
    refineAll<BoundedArrayRefinement>(BoundedArrayRefinement(&sccp, scc.intra[PRE]));
    refineAll<BoundedArrayRefinement>(BoundedArrayRefinement(&sccp, scc.intra[POST]));

    // refine SCCs by DAG criterion
    for (int direction = PRE; direction <= POST; direction++)
    {
        DagRefinement refinement = DagRefinement(&scc, (direction_t)direction);
        for (index_t *array = refinement.layer(); array != NULL; array = refinement.layer())
        {
            refineAll<BoundedArrayRefinement>(BoundedArrayRefinement(&sccp, array));
        }
    }

    // refine SCCs using neighbourhood criterion
    // TODO!

    // refine net using the SCC partition
    refineAll<SCCPartitionRefinement>(SCCPartitionRefinement(&partition, &scc, &sccp));

    gettimeofday(&end, NULL);
    std::cout << end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6 << " ";
    rep->status("Performing neighbourhood refinement");
    gettimeofday(&start, NULL);

    // refine net by neighbourhood criterion
    refineFix(&partition);

    // initialize reduced permutation group
    gettimeofday(&end, NULL);
    std::cout << end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6 << " ";
    rep->status("Stabilize partitions");
    gettimeofday(&start, NULL);

    // build partitions stabilizing [0,i-1]
    Partition **partitions = new Partition*[partition.order];
    memset(partitions, 0, sizeof(Partition *) * partition.order);
    index_t last = -1;
    for (index_t i = 0; !partition.complete(); i++, last++)
    {
        partition.reset();
        partitions[i] = partition.copy();
        partition.fix(i);
        refineFix(&partition);
    }

    gettimeofday(&end, NULL);
    std::cout << end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6 << " ";
    rep->status("Discover symmetries");
    gettimeofday(&start, NULL);

    Perm<Bijection> **sigma = new Perm<Bijection> *[partition.order];
    memset(sigma, 0, sizeof(Perm<Bijection> *) * partition.order);

    if (last != (index_t) - 1)
    {
        //#pragma omp parallel for schedule(dynamic)
        for (index_t i = 1; i < partition.order; i++)
        {
            for (index_t j = std::min(i - 1, last); j >= 0 && j != (index_t) - 1; j--)
            {
                if (partitions[j]->representative[j] != partitions[j]->representative[i])
                {
                    continue;
                }
                Perm<Bijection> *perm = Perm<Bijection>::find(partitions[j], j, i);
                if (perm != NULL)
                {
                    sigma[i] = perm;
                    break;
                }
            }
        }
    }

    // delete partitions
    for (index_t i = 0; i < partition.order; i++)
    {
        delete partitions[i];
    }

    //    for(index_t i = 0; i < partition.order; i++) if(sigma[i] != NULL) { std::cout << "GENERATOR " << i << std::endl; sigma[i]->show(); }

    gettimeofday(&end, NULL);
    std::cout << end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6 << " ";
    rep->status("Determine tau array");
    gettimeofday(&start, NULL);

    Perm<Bijection> **tau = new Perm<Bijection> *[partition.order];
    memset(tau, 0, sizeof(Perm<Bijection> *) * partition.order);

    for (index_t j = 0; j < partition.order; j++)
    {
        if (sigma[j] == NULL)
        {
            continue;
        }
        index_t i = sigma[j]->stabilizes();
        tau[i] = (tau[i] == NULL ? sigma[i] : new Perm<Bijection>(sigma[i]->composeLeft(tau[i]->perm)));
    }

    gettimeofday(&end, NULL);
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    std::cout << end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6 << " " <<
              usage.ru_maxrss / 1024.0 << std::endl;

    int generators = 0;
    for (index_t i = 0; i < partition.order; i++) if (sigma[i] != NULL)
        {
            generators++;
        }
    rep->status("Symmetry group has %d non-trivial generators", generators);

    exit(0);

    delete[] types;
    delete[] partitions;

    return;
}
