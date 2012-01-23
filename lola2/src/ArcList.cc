/*!
\author Karsten
\file ArcList.cc
\status new
\ingroup g_frontend g_symboltable

\brief class implementation for a symbol for a list of arcs

This class is used as an intermediate storage between parser and
the actual Petri net data structures. It collects arcs connected
in the same dircetion from/to the same transtion.

*/

#include "ArcList.h"

/// Getter for place
PlaceSymbol* ArcList::getPlace() const
{
    return place;
}

/// Getter for multiplicity
unsigned int ArcList::getMultiplicity() const
{
    return multiplicity;
}

/// Getter for next
ArcList* ArcList::getNext() const
{
    return next;
}

/// Setter for next
void ArcList::setNext(Symbol* n)
{
    next = (ArcList*)n;
}

/// Generate and initialize a list element
ArcList::ArcList(PlaceSymbol* p, unsigned int m) :
    place(p),
    next(NULL),
    multiplicity(m)
{
}
