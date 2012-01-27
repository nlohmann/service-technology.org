/*!
\file Place.cc
\author Karsten
\status new
\brief Useful rooutines for place specific information

All data that describe attributes of places can be found here. General
information for a place in its role as a node, ar contained in Node.*
*/

#include <cstdlib>
#include "Dimensions.h"
#include "Place.h"
#include "Node.h"

/*!
\brief collection of information related to places
*/

index_type Place::CardSignificant = 0;
unsigned int* Place::Hash = NULL;
capacity_type* Place::Capacity = NULL;
uint8_t* Place::CardBits = NULL;
index_type* Place::CardDisabled = NULL;
index_type** Place::Disabled = NULL;

/// aufräumen für valgrind
void deletePlaces()
{
    free(Place::Hash);
    free(Place::Capacity);
    free(Place::CardBits);
    for (index_type i = 0; i < Node::Card[PL]; i++)
    {
        free(Place::Disabled[i]);
    }
    free(Place::Disabled);
    free(Place::CardDisabled);
}

/// Berechne CardBits aus Capacity
uint8_t Capacity2Bits(capacity_type cap)
{
    uint8_t k = 0;
    while (cap)
    {
        k++;
        cap = cap >> 1;
    }
    return k;
}
