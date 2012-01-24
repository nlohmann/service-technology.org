/*!
\author Karsten
\file ArcList.h
\status new
\ingroup g_frontend g_symboltable

\brief class definition for a symbol for a list of arcs

This class is used as an intermediate storage between parser and
the actual Petri net data structures. It collects arcs connected
in the same dircetion from/to the same transtion.

*/

#pragma once

#include "PlaceSymbol.h"

class ArcList
{
    public:
        /// Getter for place
        PlaceSymbol* getPlace() const;
        /// Getter for multiplicity
        unsigned int getMultiplicity() const;
        /// Getter for next
        ArcList* getNext() const;
        /// Setter for next
        void setNext(Symbol*);

        /// Generate and initialize a list element
        ArcList(PlaceSymbol*, unsigned int);
	/// Delete a list element
	~ArcList();

    private:
        ///The place at the other end of arc (transition implicilty given)
        PlaceSymbol* place;
        /// Arcs are organized as lists.
        ArcList* next;
        /// Multiplicity
        unsigned int multiplicity;
};
