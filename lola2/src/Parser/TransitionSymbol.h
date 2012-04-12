/*!
\author Karsten
\file TransitionSymbol.h
\status approved 25.01.2012
\ingroup g_frontend g_symboltable

\brief class definition for a symbol with payload for transition
*/

#pragma once

#include "Parser/Symbol.h"
#include "Parser/FairnessAssumptions.h"
#include "Parser/ArcList.h"

/*!
Transition symbols carry name as key, and
- list of incoming arcs
- list of outgoing arcs
- a fairness assumption
as payload.
All information is available in the context of transition definition, thus all
information is provided in constructor

\todo Doppelte Plätze behandeln.
*/
class TransitionSymbol: public Symbol
{
    public:
        /// Generate and initialize a symbol
        TransitionSymbol(char*, fairnessAssumption_t, ArcList*, ArcList*);
        /// Delete Transition symbol
        ~TransitionSymbol();

        /// Getter for number of incoming arcs
        unsigned int getCardPre() const;
        /// Getter for number of outgoing arcs
        unsigned int getCardPost() const;
        /// Getter for incoming arcs
        ArcList* getPre() const;
        /// Getter for number of post-places
        ArcList* getPost() const;
        /// Getter for fairness assumption
        fairnessAssumption_t getFairness() const;

    private:
        /// The fairness assumption specified for this tarnsition
        fairnessAssumption_t fairness;

        /// The number of transitions that consume from this place
        /// Used for creating arc data structures
        unsigned int cardPost;

        /// The number of transitions that produce on this place
        /// used for creating arc data structures
        unsigned int cardPre;

        /// The number of transitions that consume from this place
        /// Used for creating arc data structures
        ArcList* Post;

        /// The number of transitions that produce on this place
        /// used for creating arc data structures
        ArcList* Pre;
};
