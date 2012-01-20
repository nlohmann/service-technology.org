/*!
\author Karsten
\file ArcList.h
\status new

\brief class definition for a symbol for a list of arcs

This class is used as an intermediate storage between parser and
the actual Petri net data structures. It collects arcs connected
in the same dircetion from/to the same transtion.

*/

#pragma once

#include "PLaceSymbol"

class ArcList
{
    public:
        /// Getter for place
        PlaceSymbol *  getPlace();
        /// Getter for multiplicity
        unsigned int getMultiplicity();
        /// Getter for next
        ArcList * getNext();
        /// Setter for next
        void setNext(Symbol*);

        /// Generate and initialize a list element
        ArcList(PlaceSymbol*,unsigned int);

    private:
        ///The place at the other end of arc (transition implicilty given)
        PlaceSymbol * place;
        /// Arcs are organized as lists.
        ArcList* next;
	/// Multiplicity
	unsigned int multiplicity;
};
