/*!
\file CompressedIO.cc
\author Karsten
\status approved 21.02.2012

\brief Input and outout from/to a file in compressed format. We generate two
separate files that can be read in arbitrary order. In this version, we use an
ASCII file where data are separated by spaces and newlines
*/

#include <config.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <InputOutput/CompressedIO.h>
#include <Parser/ParserPTNet.h>
#include <Parser/PlaceSymbol.h>
#include <Parser/TransitionSymbol.h>
#include <Parser/FairnessAssumptions.h>
#include <Net/Net.h>
#include <Net/Marking.h>
#include <Net/Place.h>
#include <Net/Transition.h>


void WriteNameFile(FILE *f)
{
    // 1. Number of places
    fprintf(f, "%u\n", Net::Card[PL]);

    // 2. Places with one name per line, oin order of indices
    for (index_t p = 0; p < Net::Card[PL]; p++)
    {
        fprintf(f, "%s\n", Net::Name[PL][p]);
    }

    // 3. Number of transition
    fprintf(f, "%u\n", Net::Card[TR]);

    // 4. Transitions with one name per line, in order of indices
    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
        fprintf(f, "%s\n", Net::Name[TR][t]);
    }
}

/*!
\pre Memory of Name[TR] and Name[PL] is already allocated.

\todo Scanf mit Maximalbreite nutzen um cppcheck-Fehler zu umgehen.
*/
void ReadNameFile(FILE *f, ParserPTNet *symboltables)
{
    assert(Net::Name[PL]);
    assert(Net::Name[TR]);

    char buffer[10000];
    unsigned int tmp1;

    // 1. Number of places
    fscanf(f, "%u", &tmp1);
    Net::Card[PL] = (index_t) tmp1;

    // 2. Places with one index, one name per line
    for (index_t p = 0; p < Net::Card[PL]; p++)
    {
        fscanf(f, "%s", buffer);
        Net::Name[PL][p] = strdup(buffer);
        PlaceSymbol *ps = new PlaceSymbol(Net::Name[PL][p], Place::Capacity[p]);
        ps -> setIndex(p);
        symboltables->PlaceTable->insert(ps);
    }

    // 3. Number of transition
    fscanf(f, "%u", &tmp1);
    Net::Card[TR] = (index_t) tmp1;

    // 4. Transitions with one index, one name per line
    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
        fscanf(f, "%s", buffer);
        Net::Name[TR][t] = strdup(buffer);
        TransitionSymbol *ts = new TransitionSymbol(Net::Name[TR][t], Transition::Fairness[t], NULL, NULL);
        ts -> setIndex(t);
        symboltables->TransitionTable->insert(ts);
    }
}

void WriteNetFile(FILE *f)
{
    // 1. Number of places and significant places
    fprintf(f, "%u %u", Net::Card[PL], Place::CardSignificant);

    // 2. For each place...
    for (index_t p = 0; p < Net::Card[PL]; p++)
    {
        // 2.a initial marking
        // 2.b capacity
        // 2.c nr of incoming arcs
        fprintf(f, "\n%u %u %u ",
                Marking::Initial[p],
                (Place::Capacity[p] == MAX_CAPACITY ? 0 : Place::Capacity[p]),
                Net::CardArcs[PL][PRE][p]);

        for (index_t i = 0; i < Net::CardArcs[PL][PRE][p]; i++)
        {
            // 2.d incoming arcs and multiplicities
            fprintf(f, "%u %u ", Net::Arc[PL][PRE][p][i], Net::Mult[PL][PRE][p][i]);
        }

        // 2.e nr of outgoing arcs
        fprintf(f, "%u ", Net::CardArcs[PL][POST][p]);

        for (index_t i = 0; i < Net::CardArcs[PL][POST][p]; i++)
        {
            // 2.f outgoing arcs and multiplicities
            fprintf(f, "%u %u ", Net::Arc[PL][POST][p][i], Net::Mult[PL][POST][p][i]);
        }
    }
    // 3. Number of transitions
    fprintf(f, "\n%u", Net::Card[TR]);

    // 2. For each transition...
    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
        fprintf(f, "\n");
        // 2.a fairness
        // 2.b nr of incoming arcs
        fprintf(f, "%u %u ", Transition::Fairness[t], Net::CardArcs[TR][PRE][t]);

        for (index_t i = 0; i < Net::CardArcs[TR][PRE][t]; i++)
        {
            // 2.d incoming arcs and multiplicities
            fprintf(f, "%u %u ", Net::Arc[TR][PRE][t][i], Net::Mult[TR][PRE][t][i]);
        }

        // 2.e nr of outgoing arcs
        fprintf(f, "%u ", Net::CardArcs[TR][POST][t]);

        for (index_t i = 0; i < Net::CardArcs[TR][POST][t]; i++)
        {
            // 2.f outgoing arcs and multiplicities
            fprintf(f, "%u %u ", Net::Arc[TR][POST][t][i], Net::Mult[TR][POST][t][i]);
        }
    }
}



/*!
\todo Collapse fscanf calls when possible
*/
void ReadNetFile(FILE *f)
{
    // read number of places
    unsigned int tmp1, tmp2;
    fscanf(f, "%u %u", &tmp1, &tmp2);
    Net::Card[PL] = (index_t) tmp1;
    Place::CardSignificant = (index_t) tmp2;

    // allocate place arrays
    // we set place names to NULL right now
    Net::Name[PL] = (char **) calloc(Net::Card[PL] , SIZEOF_VOIDP);
    for (int direction = PRE; direction <= POST; direction ++)
    {
        Net::CardArcs[PL][direction] = (index_t *) malloc(Net::Card[PL] * SIZEOF_INDEX_T);
        Net::Arc[PL][direction] = (index_t **) malloc(Net::Card[PL] * SIZEOF_VOIDP);
        Net::Mult[PL][direction] = (mult_t **) malloc(Net::Card[PL] * SIZEOF_VOIDP);
    }

    Place::Capacity = (capacity_t *) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    Marking::Initial = (capacity_t *) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    Marking::Current = (capacity_t *) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);

    // fill all information that is locally available in symbols, allocate node specific arrays
    for (index_t p = 0; p < Net::Card[PL]; p++)
    {
        // read initial marking
        fscanf(f, "%u", &tmp1);
        Marking::Initial[p] = (capacity_t) tmp1;
        Marking::Current[p] = Marking::Initial[p];

        // read capacity
        fscanf(f, "%u", &tmp1);
        Place::Capacity[p] = (capacity_t)(tmp1 == 0 ? MAX_CAPACITY : tmp1);

        // read number of prearcs
        fscanf(f, "%u", &tmp1);
        Net::CardArcs[PL][PRE][p] = (index_t) tmp1;
        Net::Arc[PL][PRE][p] = (index_t *) malloc(Net::CardArcs[PL][PRE][p] * SIZEOF_INDEX_T);
        Net::Mult[PL][PRE][p] = (mult_t *) malloc(Net::CardArcs[PL][PRE][p] * SIZEOF_MULT_T);

        // read prearcs
        for (index_t i = 0; i < Net::CardArcs[PL][PRE][p]; i++)
        {
            fscanf(f, "%u %u", &tmp1, &tmp2);
            Net::Arc[PL][PRE][p][i] = (index_t) tmp1;
            Net::Mult[PL][PRE][p][i] = (mult_t) tmp2;
        }

        // read number of postarcs
        fscanf(f, "%u", &tmp1);
        Net::CardArcs[PL][POST][p] = (index_t) tmp1;
        Net::Arc[PL][POST][p] = (index_t *) malloc(Net::CardArcs[PL][POST][p] * SIZEOF_INDEX_T);
        Net::Mult[PL][POST][p] = (mult_t *) malloc(Net::CardArcs[PL][POST][p] * SIZEOF_MULT_T);

        // read postarcs
        for (index_t i = 0; i < Net::CardArcs[PL][POST][p]; i++)
        {
            fscanf(f, "%u %u", &tmp1, &tmp2);
            Net::Arc[PL][POST][p][i] = (index_t) tmp1;
            Net::Mult[PL][POST][p][i] = (mult_t) tmp2;
        }
    }
    // Allocate arrays for places and transitions

    // read number of transitions
    fscanf(f, "%u", &tmp1);
    Net::Card[TR] = (index_t) tmp1;

    // transition names are set to NULL here
    Net::Name[TR] = (char **) calloc(Net::Card[TR], SIZEOF_VOIDP);
    for (int direction = PRE; direction <= POST; direction ++)
    {
        Net::CardArcs[TR][direction] = (index_t *) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
        Net::Arc[TR][direction] = (index_t **) malloc(Net::Card[TR] * SIZEOF_VOIDP);
        Net::Mult[TR][direction] = (mult_t **) malloc(Net::Card[TR] * SIZEOF_VOIDP);
    }

    Transition::Fairness = (fairnessAssumption_t *) malloc(Net::Card[TR] * SIZEOF_FAIRNESSASSUMPTION_T);

    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
        // read fairness
        fscanf(f, "%u", &tmp1);
        Transition::Fairness[t] = (fairnessAssumption_t)tmp1;

        // read number of prearcs
        fscanf(f, "%u", &tmp1);
        Net::CardArcs[TR][PRE][t] = (index_t) tmp1;
        Net::Arc[TR][PRE][t] = (index_t *) malloc(Net::CardArcs[TR][PRE][t] * SIZEOF_INDEX_T);
        Net::Mult[TR][PRE][t] = (mult_t *) malloc(Net::CardArcs[TR][PRE][t] * SIZEOF_MULT_T);

        // read prearcs
        for (index_t i = 0; i < Net::CardArcs[TR][PRE][t]; i++)
        {
            fscanf(f, "%u %u", &tmp1, &tmp2);
            Net::Arc[TR][PRE][t][i] = (index_t) tmp1;
            Net::Mult[TR][PRE][t][i] = (mult_t) tmp2;
        }

        // read number of postarcs
        fscanf(f, "%u", &tmp1);
        Net::CardArcs[TR][POST][t] = (index_t) tmp1;
        Net::Arc[TR][POST][t] = (index_t *) malloc(Net::CardArcs[TR][POST][t] * SIZEOF_INDEX_T);
        Net::Mult[TR][POST][t] = (mult_t *) malloc(Net::CardArcs[TR][POST][t] * SIZEOF_MULT_T);

        // read postarcs
        for (index_t i = 0; i < Net::CardArcs[TR][POST][t]; i++)
        {
            fscanf(f, "%u %u", &tmp1, &tmp2);
            Net::Arc[TR][POST][t][i] = (index_t) tmp1;
            Net::Mult[TR][POST][t][i] = (mult_t) tmp2;
        }
    }

    // net is read completely now, time to preprocess
    Net::preprocess();
}
