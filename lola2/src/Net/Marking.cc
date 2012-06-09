/*!
\file Marking.cc
\author Karsten
\status approved 27.01.2012

\brief Global data for marking specific information

All data that describe attributes of markings can be found here.
*/

#include <cstdlib>
#include <cstdio>
#include <Core/Dimensions.h>
#include <Net/Marking.h>
#include <Net/Net.h>


capacity_t* Marking::Initial = NULL;
hash_t Marking::HashInitial = 0;
capacity_t* Marking::Current = NULL;
hash_t Marking::HashCurrent = 0;
capacity_t* Marking::Target = NULL;
hash_t Marking::HashTarget = 0;

/// collection of information related to markings
void Marking::deleteMarkings()
{
    free(Marking::Initial);
    free(Marking::Current);
    free(Marking::Target);
}

void Marking::printMarking(capacity_t* marking) {
    return;
    printf("PRINTING MARKING %x:\n",marking);
    for (int i = 0; i < Net::Card[PL]; i++)
        printf("\t%s:%d\n",Net::Name[PL][i], marking[i]);
    printf("==============\n");
}
