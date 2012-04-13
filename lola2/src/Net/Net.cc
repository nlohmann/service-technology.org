/*
\file Net.cc
\author Karsten
\status approved 27.01.2012

\brief basic routines for handling nodes
*/

#include <cstdio>
#include <cstdlib>
#include "Net/Net.h"
#include "Net/Marking.h"
#include "Net/Place.h"
#include "Net/Transition.h"
#include "Parser/FairnessAssumptions.h"


index_t Net::Card[2] = {0, 0};
index_t* Net::CardArcs[2][2] = {{NULL, NULL}, {NULL, NULL}};
index_t** Net::Arc[2][2] = {{NULL, NULL}, {NULL, NULL}};
mult_t** Net::Mult[2][2] = {{NULL, NULL}, {NULL, NULL}};
char** Net::Name[2] = {NULL, NULL};

/// Aufräumen der Netzstruktur -- service für valgrind
void Net::deleteNodes()
{
    for (int type = PL; type <= TR; ++type)
    {
        for (int direction = PRE; direction <= POST; ++direction)
        {
            for (index_t i = 0; i < Net::Card[type]; i++)
            {
                free(Net::Arc[type][direction][i]);
                free(Net::Mult[type][direction][i]);
            }
            free(Net::CardArcs[type][direction]);
            free(Net::Arc[type][direction]);
            free(Net::Mult[type][direction]);
        }
        for (index_t i = 0; i < Net::Card[type]; i++)
        {
            free(Net::Name[type][i]);
        }
        free(Net::Name[type]);
    }
}

void Net::print()
{
    printf("Net\n===\n\n");

    printf("%u places,  %u  transitions.\n\n", Net::Card[PL], Net::Card[TR]);

    for (index_t i = 0; i < Net::Card[PL]; i++)
    {
        printf("Place %u :%s, %u tokens hash %lld capacity %u bits %u\n", i, Net::Name[PL][i], Marking::Initial[i], Place::Hash[i], Place::Capacity[i], Place::CardBits[i]);
        printf("From (%u):\n", Net::CardArcs[PL][PRE][i]);
        for (index_t j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            printf("%s:%d ", Net::Name[TR][Net::Arc[PL][PRE][i][j]], Net::Mult[PL][PRE][i][j]);
        }
        printf("\n");
        printf("To (%u):\n", Net::CardArcs[PL][POST][i]);
        for (index_t j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            printf("%s:%d ", Net::Name[TR][Net::Arc[PL][POST][i][j]], Net::Mult[PL][POST][i][j]);
        }
        printf("\n");

    }
    for (index_t i = 0; i < Net::Card[TR]; i++)
    {
        printf("\nTransition %u :%s\n", i, Net::Name[TR][i]);
        switch (Transition::Fairness[i])
        {
            case NO_FAIRNESS:
                printf(" no ");
                break;
            case WEAK_FAIRNESS:
                printf(" weak ");
                break;
            case STRONG_FAIRNESS:
                printf(" strong ");
                break;
            default:
                printf("error");
        }
        printf(" %s ", (Transition::Enabled[i] ? "enabled" : "disabled"));

        printf("From:\n");
        for (index_t j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            printf("%s:%u ", Net::Name[PL][Net::Arc[TR][PRE][i][j]], Net::Mult[TR][PRE][i][j]);
        }
        printf("\n");
        printf("To:\n");
        for (index_t j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            printf("%s:%u ", Net::Name[PL][Net::Arc[TR][POST][i][j]], Net::Mult[TR][POST][i][j]);
        }
        printf("\n");
        printf("remove from");
        for (index_t j = 0; j < Transition::CardDeltaT[PRE][i]; j++)
        {
            printf(" %s:%u", Net::Name[PL][Transition::DeltaT[PRE][i][j]], Transition::MultDeltaT[PRE][i][j]);
        }
        printf("produce on");
        for (index_t j = 0; j < Transition::CardDeltaT[POST][i]; j++)
        {
            printf(" %s:%u", Net::Name[PL][Transition::DeltaT[POST][i][j]], Transition::MultDeltaT[POST][i][j]);
        }
        printf("\n conflicting:");
        for (index_t j = 0; j < Transition::CardConflicting[i]; j++)
        {
            printf("%s", Net::Name[TR][Transition::Conflicting[i][j]]);
        }
        printf("\n");
        for (index_t j = 0; j < Transition::CardBackConflicting[i]; j++)
        {
            printf("%s", Net::Name[TR][Transition::BackConflicting[i][j]]);
        }
    }
    printf("done\n");
}
