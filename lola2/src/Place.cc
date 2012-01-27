/*!
\file Place.cc
\author Karsten
\status approved 27.01.2012
\brief Useful rooutines for place specific information
\todo Tränsläte the comments to Inglisch pliez.

All data that describe attributes of places can be found here. General
information for a place in its role as a node, ar contained in Node.*
*/

#include <cstdlib>
#include "Dimensions.h"
#include "Place.h"
#include "Net.h"

/*!
\brief collection of information related to places
*/

index_t Place::CardSignificant = 0;
hash_t* Place::Hash = NULL;
capacity_t* Place::Capacity = NULL;
cardbit_t* Place::CardBits = NULL;
index_t* Place::CardDisabled = NULL;
index_t** Place::Disabled = NULL;

/// aufräumen für valgrind
void Place::deletePlaces()
{
    free(Place::Hash);
    free(Place::Capacity);
    free(Place::CardBits);
    for (index_t i = 0; i < Net::Card[PL]; i++)
    {
        free(Place::Disabled[i]);
    }
    free(Place::Disabled);
    free(Place::CardDisabled);
}

/// Berechne CardBits aus Capacity
cardbit_t Place::Capacity2Bits(capacity_t cap)
{
    cardbit_t k = 0;
    while (cap)
    {
        k++;
        cap = cap >> 1;
    }
    return k;
}
