/*!

\file Place.cc
\author Karsten
\status new
\brief Useful rooutines for place specific information

All data that describe attributes of places can be found here. General information for a place
in its role as a node, ar contained in Node.*
*/


// Dimensions.h has a type index_type that has at least max(nr of places,nr of transitions) elements

#include "Dimensions.h"
#include "Place.h"
#include "Node.h"
#include <cstdlib>

/*!

\brief collection of information related to places

*/

/// aufräumen für valgrind
void deletePlaces()
{
	free(Place::Hash);
	free(Place::Capacity);
	free(Place::CardBits);
	for(index_type i=0;i<Node::Card[PL];i++)
	{
		free(Place::Disabled[i]);
	}
	free(Place::Disabled);
	free(Place::CardDisabled);
}

/// Berechne CardBits aus Capacity
short int Capacity2Bits(capacity_type cap)
{
	short int k = 0;
   	while (cap) {
        k++;
        cap = cap >> 1;
    }
    return k;
}


index_type Place::CardSignificant = 0;
unsigned int * Place::Hash = NULL;
capacity_type * Place::Capacity = NULL;
unsigned short int *  Place::CardBits = NULL;
index_type * Place::CardDisabled = NULL;
index_type ** Place::Disabled = NULL;
