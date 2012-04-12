/*!

\file Firelist.cc
\author Karsten
\status new
\brief class for firelist generation. Default is firelist consisting of all enabled transitions.

*/

#include "Core/Dimensions.h"
#include "Net/Net.h"
#include "Net/Transition.h"
#include "Exploration/Firelist.h"

index_t Firelist::getFirelist(index_t** result)
{
    assert(Transition::CardEnabled <= Net::Card[TR]);
    * result = new index_t[Transition::CardEnabled];
    index_t i = 0;
    for (index_t t = 0; t < Net::Card[TR]; ++t)
    {
        if (Transition::Enabled[t])
        {
            assert(i < Transition::CardEnabled);
            (*result)[i++] = t;
        }
    }
    return Transition::CardEnabled;
}

