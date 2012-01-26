/*!
\author Karsten
\file ArcList.cc
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\brief class implementation for a symbol for a list of arcs
*/

#include <cstdlib>
#include "ArcList.h"

/// Getter for place
PlaceSymbol* ArcList::getPlace() const
{
    return place;
}

/// Getter for multiplicity
mult_type ArcList::getMultiplicity() const
{
    return multiplicity;
}

/// Getter for next
ArcList* ArcList::getNext() const
{
    return next;
}

/// Adder for multiplicity
void ArcList::addMultiplicity(mult_type i)
{
    multiplicity += i;
}

/// Setter for next
void ArcList::setNext(Symbol* n)
{
    next = reinterpret_cast<ArcList*>(n);
}

/// Generate and initialize a list element
ArcList::ArcList(PlaceSymbol* p, mult_type m) :
    place(p),
    next(NULL),
    multiplicity(m)
{
}
