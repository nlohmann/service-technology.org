/*!
\author Karsten
\file PlaceSymbol.cc
\status new

\brief class implementation for a symbol with payload for place

Place symbols carry name as key, and
- capacity
- initial marking
as payload. While capacity is in the context of place declaration (thus,
part of constructor), initial marking is specified separately (thus, setter).
Additional information is number of pre-transitions and number of post-transitions. This information is updated while parsing transitions.
*/

#include "PlaceSymbol.h"

/// Constructor. Capacity available in context, the other ingredients are not
PlaceSymbol::PlaceSymbol(char* k, unsigned int cap)
    :
    Symbol(k),
    capacity(cap),
    initialMarking(0),
    cardPost(0),
    cardPre(0)
{
}

/// Getter for capacity
unsigned int PlaceSymbol::getCapacity() const
{
    return capacity;
}

/// Adder for initial marking
void PlaceSymbol::addInitialMarking(unsigned int newTokens)
{
    initialMarking += newTokens;
}

/// Getter for number of pre-transitions
unsigned int PlaceSymbol::getCardPre() const
{
    return cardPre;
}

/// Getter for number of post-transitions
unsigned int PlaceSymbol::getCardPost() const
{
    return cardPost;
}

/// Incrementor for number of post-transitions
void PlaceSymbol::notifyPost()
{
    ++cardPost;
}

/// Incrementor for number of pre-transitions
void PlaceSymbol::notifyPre()
{
    ++cardPre;
}

/// Delete PlaceSymbol
PlaceSymbol::~PlaceSymbol()
{
}
