/*!
\author Karsten
\file ArcList.h
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\brief class definition for a symbol for a list of arcs
*/

#pragma once

#include "Core/Dimensions.h"
#include "Parser/PlaceSymbol.h"

/*!
This class is used as an intermediate storage between parser and the actual
Petri net data structures. It collects arcs connected in the same dircetion
from/to the same transtion.
*/
class ArcList
{
    public:
        /// Getter for place
        PlaceSymbol* getPlace() const;
        /// Getter for multiplicity
        mult_t getMultiplicity() const;
        /// Getter for next
        ArcList* getNext() const;
        /// Setter for next
        void setNext(Symbol*);
        /// Adding some tokens to multplicity
        void addMultiplicity(mult_t);

        /// Generate and initialize a list element
        ArcList(PlaceSymbol*, mult_t);

    private:
        ///The place at the other end of arc (transition implicilty given)
        PlaceSymbol* place;
        /// Arcs are organized as lists.
        ArcList* next;
        /// Multiplicity
        mult_t multiplicity;
};
