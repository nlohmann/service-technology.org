/*!
\author Karsten
\file PlaceSymbol.h
\status new

\brief class definition for a symbol with payload for place

Place symbols carry name as key, and
- capacity
- initial marking
as payload. While capacity is in the context of place declaration (thus,
part of constructor), initial marking is specified separately (thus, setter).
Additional information is number of pre-transitions and number of post-transitions. This information is updated while parsing transitions.
*/

#pragma once

#include "Symbol.h"

class PlaceSymbol: public Symbol
{
    public:
        /// Generate and initialize a symbol
        PlaceSymbol(string, unsigned int);

        /// Getter for capacity
        unsigned int getCapacity() const;
        /// Getter for initial marking
        unsigned int getInitialMarking();
        /// Getter for number of pre-transitions
        unsigned int getCardPre() const;
        /// Getter for number of post-transitions
        unsigned int getCardPost() const;
        /// Adder for initial marking. If several marking specifications
        /// mention this place, tokens are added.
        void addInitialMarking(unsigned int);
        /// Incrementor for number of post-transitions
        void notifyPost();
        /// Incrementor for number of pre-transitions
        void notifyPre();

    private:
        /// The maximum number of tokens that msut be representable for this place; UINT_MAX for absence of capacity
        unsigned int capacity;
        /// The initial number of tokens on this place
        unsigned int initialMarking;
        /// The number of transitions that consume from this place
        /// Used for creating arc data structures
        unsigned int cardPost;
        /// The number of transitions that produce on this place
        /// used for creating arc data structures
        unsigned int cardPre;
};
