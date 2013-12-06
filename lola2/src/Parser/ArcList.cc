/*!
\author Karsten
\file
\status approved 25.01.2012
\ingroup g_symboltable

\brief class implementation for a symbol for a list of arcs
*/

#include <cstdlib>
#include <Parser/ArcList.h>
#include <Parser/PlaceSymbol.h>

PlaceSymbol *ArcList::getPlace() const
{
    return place;
}

mult_t ArcList::getMultiplicity() const
{
    return multiplicity;
}

ArcList *ArcList::getNext() const
{
    return next;
}

void ArcList::addMultiplicity(mult_t i)
{
    multiplicity += i;
}

void ArcList::setNext(Symbol *n)
{
    next = reinterpret_cast<ArcList *>(n);
}

ArcList::ArcList(PlaceSymbol *p, mult_t m)
    : place(p), next(NULL), multiplicity(m)
{
}
