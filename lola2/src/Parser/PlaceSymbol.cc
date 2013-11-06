/*!
\author Karsten
\file PlaceSymbol.cc
\status approved 25.01.2012

\brief class implementation for a symbol with payload for place
*/

#include <Parser/PlaceSymbol.h>

/// Constructor. Capacity available in context, the other ingredients are not
PlaceSymbol::PlaceSymbol(char *k, capacity_t cap)
    :
    Symbol(k),
    capacity(cap),
    initialMarking(0),
    cardPost(0),
    cardPre(0)
{
}

/// Getter for capacity
capacity_t PlaceSymbol::getCapacity() const
{
    return capacity;
}

/// Getter for initial marking
capacity_t PlaceSymbol::getInitialMarking() const
{
    return initialMarking;
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
