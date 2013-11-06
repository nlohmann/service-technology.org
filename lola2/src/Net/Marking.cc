/*!
\file Marking.cc
\author Karsten
\status approved 27.01.2012

\brief Global data for marking specific information

All data that describe attributes of markings can be found here.
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <Core/Dimensions.h>
#include <Net/Marking.h>
#include <Net/Net.h>


capacity_t *Marking::Initial = NULL;
hash_t Marking::HashInitial = 0;
capacity_t *Marking::Current = NULL;
hash_t Marking::HashCurrent = 0;
capacity_t *Marking::Target = NULL;
hash_t Marking::HashTarget = 0;

/// init for the behavioral part of the net
/* not needed anymore, this is done with NetStates now
void Marking::init()
{
    assert(Net::Card[PL] > 0);

    memcpy(Current, Initial, Net::Card[PL] * SIZEOF_INDEX_T);
    HashCurrent = HashInitial;
}
*/

/// collection of information related to markings
void Marking::deleteMarkings()
{
    free(Marking::Initial);
    free(Marking::Current);
    free(Marking::Target);
}

// LCOV_EXCL_START
void Marking::DEBUG__printMarking(capacity_t *marking)
{
    return;
    printf("PRINTING MARKING %lx:\n", (unsigned long) marking);
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        printf("\t%s:%d\n", Net::Name[PL][i], marking[i]);
    }
    printf("==============\n");
}
// LCOV_EXCL_STOP
