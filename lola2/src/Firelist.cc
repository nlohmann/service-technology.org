/*!

\file Firelist.cc
\author Karsten
\status new
\brief class for firelist generation. Default is firelist consisting of all enabled transitions.

*/

#include "Dimensions.h"
#include "Net.h"
#include "Firelist.h"
#include "Transition.h"

index_t Firelist::getFirelist(index_t ** result)
{
	* result = new index_t[Transition::CardEnabled];
	index_t i = 0;
	for(index_t t = 0; t < Net::Card[TR];++t)
	{
		if(Transition::Enabled[t])
		{
			*result[i++] = t;
		}
	}
	return Transition::CardEnabled;
}

