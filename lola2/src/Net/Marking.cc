/*!
\file Marking.cc
\author Karsten
\status approved 27.01.2012

\brief Global data for marking specific information

All data that describe attributes of markings can be found here.
*/

#include <cstdlib>
#include <Core/Dimensions.h>
#include <Net/Marking.h>


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
