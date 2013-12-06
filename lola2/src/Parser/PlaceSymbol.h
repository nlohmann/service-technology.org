/*!
\author Karsten
\file
\status approved 25.01.2012
\ingroup g_symboltable
\brief definition of class PlaceSymbol
*/

#pragma once
#include <Core/Dimensions.h>
#include <Parser/Symbol.h>


/*!
\brief a symbol table entry for a place

Place symbols carry their name as key and capacity and an initial marking as
payload. While capacity is in the context of place declaration (thus, part
of constructor), initial marking is specified separately (thus, setter).
Additional information is number of pre-transitions and number of
post-transitions. This information is updated while parsing transitions.

\note The members cardPost and cardPre are used for later data structures for
arcs.
\note The capacity UINT_MAX denotes the absence of a capacity.
\note This class does not allocated dynamic memory.

\ingroup g_symboltable

\todo Dopplungen behandeln.
*/
class PlaceSymbol: public Symbol
{
public:
    /// generate and initialize a symbol
    PlaceSymbol(char *, unsigned int);

    /// getter for capacity
    capacity_t getCapacity() const;
    /// getter for initial marking
    capacity_t getInitialMarking() const;
    /// getter for number of pre-transitions
    index_t getCardPre() const;
    /// getter for number of post-transitions
    index_t getCardPost() const;

    /// adder for initial marking
    void addInitialMarking(capacity_t);
    /// incrementor for number of post-transitions
    void notifyPost();
    /// incrementor for number of pre-transitions
    void notifyPre();

private:
    /// the maximum number of tokens that must be representable for this place
    capacity_t capacity;
    /// the initial number of tokens on this place
    capacity_t initialMarking;
    /// the number of transitions that consume from this place
    index_t cardPost;
    /// the number of transitions that produce on this place
    index_t cardPre;
};
