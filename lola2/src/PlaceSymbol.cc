/*!
\author Karsten
\file PlaceSymbol.cc
\status new

\brief class implementation for a symbol with payload for place
*/

#include "PlaceSymbol.h"

/// Constructor. Capacity available in context, the other ingredients are not
PlaceSymbol::PlaceSymbol(const char* k, unsigned int cap)
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
