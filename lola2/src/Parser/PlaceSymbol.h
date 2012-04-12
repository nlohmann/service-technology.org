/*!
\author Karsten
\file PlaceSymbol.h
\status approved 25.01.2012

\brief class definition for a symbol with payload for place
*/

#pragma once

#include "Dimensions.h"
#include "Parser/Symbol.h"

/*!
Place symbols carry name as key, and
- capacity
- initial marking
as payload. While capacity is in the context of place declaration (thus, part
of constructor), initial marking is specified separately (thus, setter).
Additional information is number of pre-transitions and number of
post-transitions. This information is updated while parsing transitions.

\todo Dopplungen behandeln.
*/
class PlaceSymbol: public Symbol
{
    public:
        /// Generate and initialize a symbol
        PlaceSymbol(char*, unsigned int);

        /// Getter for capacity
        capacity_t getCapacity() const;
        /// Getter for initial marking
        capacity_t getInitialMarking() const;
        /// Getter for number of pre-transitions
        index_t getCardPre() const;
        /// Getter for number of post-transitions
        index_t getCardPost() const;
        /// Adder for initial marking. If several marking specifications
        /// mention this place, tokens are added.
        void addInitialMarking(capacity_t);
        /// Incrementor for number of post-transitions
        void notifyPost();
        /// Incrementor for number of pre-transitions
        void notifyPre();

    private:
        /// The maximum number of tokens that must be representable for this place; UINT_MAX for absence of capacity
        capacity_t capacity;
        /// The initial number of tokens on this place
        capacity_t initialMarking;
        /// The number of transitions that consume from this place
        /// Used for creating arc data structures
        index_t cardPost;
        /// The number of transitions that produce on this place
        /// used for creating arc data structures
        index_t cardPre;
};
