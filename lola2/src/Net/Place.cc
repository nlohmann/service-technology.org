/*!
\file Place.cc
\author Karsten
\status approved 27.01.2012

\brief Useful rooutines for place specific information

All data that describe attributes of places can be found here. General
information for a place in its role as a node, ar contained in Node.*

\todo Tränsläte the comments to Inglisch pliez.
*/

#include <cstdlib>
#include <Core/Dimensions.h>
#include <Net/Place.h>
#include <Net/Net.h>

/*!
\brief collection of information related to places
*/
index_t Place::CardSignificant = 0;
hash_t *Place::Hash = NULL;

/*!
This is only used for calculating a dense representation of a marking, not as
a blocker for transition activation.
*/
capacity_t *Place::Capacity = NULL;

/*!
Used for handling dense representations of markings
*/
cardbit_t *Place::CardBits = NULL;
index_t Place::SizeOfBitVector = 0;
index_t *Place::CardDisabled = NULL;

/*!
The arrays Disabled[i] are allocated for the maximum number of such
transitions which is \f$card(p^\bullet)\f$
*/
index_t **Place::Disabled = NULL;

/// aufräumen für valgrind
void Place::deletePlaces()
{
    // allocated in Net::preprocess()
    delete[] Place::Hash;
    delete[] Place::CardBits;
    delete[] Place::CardDisabled;

    // allocated in ReadNetFile(FILE *f) and ParserPTNet::symboltable2net()
    delete[] Place::Capacity;

    // allocated in Net::preprocess()
    for (index_t i = 0; i < Net::Card[PL]; i++)
    {
        delete[] Place::Disabled[i];
    }
    delete[] Place::Disabled;
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
