/*!
\author Karsten
\file ArcList.cc
\status new

\brief class implementation for a symbol for a list of arcs

This class is used as an intermediate storage between parser and
the actual Petri net data structures. It collects arcs connected
in the same dircetion from/to the same transtion.

*/

#pragma once

#include "PLaceSymbol.h"

/// Getter for place
PlaceSymbol *  getPlace()
{
	return place;
}

/// Getter for multiplicity
unsigned int ArcList::getMultiplicity()
{
	return multiplicity;
}

/// Getter for next
ArcList * ArcList::getNext()
{
	return next;
}

/// Setter for next
void ArcList::setNext(Symbol* n)
{
	next = n;
}

/// Generate and initialize a list element
ArcList::ArcList(PlaceSymbol* p,unsigned int m);

        place = p;
        next = NULL;
	multiplicity = m;
}
