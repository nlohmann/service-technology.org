/*!

\file Marking.cc
\author Karsten
\status new
\brief Global data for marking specific information

All data that describe attributes of markings can be found here.
*/

#include <cstdlib>
#include "Marking.h"

// Dimensions.h has a type index_type that has at least max(nr of places,nr of transitions) elements
#include "Dimensions.h"

/*!

\brief collection of information related to markings

*/

void deleteMarkings()
{
    free(Marking::Initial);
    free(Marking::Current);
    free(Marking::Target);
}

capacity_type* Marking::Initial = NULL;
unsigned int Marking::HashInitial = 0;
capacity_type* Marking::Current = NULL;
unsigned int Marking::HashCurrent = 0;
capacity_type* Marking::Target = NULL;
unsigned int Marking::HashTarget = 0;
