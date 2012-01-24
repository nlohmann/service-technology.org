/*!
\author Karsten
\file TransitionSymbol.h
\status new
\ingroup g_frontend g_symboltable

\brief class definition for a symbol with payload for transition

Transition symbols carry name as key, and
- list of incoming arcs
- list of outgoing arcs
- a fairness assumption
as payload.
All information is available in the context of transition definition, thus
all information is provided in constructor
*/

#pragma once

#include "Symbol.h"
#include "FairnessAssumptions.h"
#include "ArcList.h"

class TransitionSymbol: public Symbol
{
    public:
        /// Generate and initialize a symbol
        TransitionSymbol(char*, tFairnessAssumption, ArcList*, ArcList*);
        /// Delete Transition symbol
        virtual ~TransitionSymbol();

        /// Getter for number of incoming arcs
        unsigned int getCardPre() const;
        /// Getter for number of outgoing arcs
        unsigned int getCardPost() const;
        /// Getter for incoming arcs
        ArcList* getPre() const;
        /// Getter for number of post-places
        ArcList* getPost() const;

    private:
        /// The fairness assumption specified for this tarnsition
        tFairnessAssumption fairness;
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
