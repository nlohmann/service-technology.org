/*!

\file Transition.h
\author Karsten
\status new
\brief Global data for transition specific information

All data that describe attributes of transitions can be found here. General information for a transition
in its role as a node, ar contained in Node.*
*/

#pragma once

// Dimensions.h has a type index_type that has at least max(nr of places,nr of transitions) elements
#include "Dimensions.h"
#include "FairnessAssumptions.h"
#include "BitSetC.h"

/*!

\brief collection of information related to transitions

*/

struct Transition
{
    public:
        /// fairness specification
        static  tFairnessAssumption* Fairness;

        /// Activation status
        static bool* Enabled;

        /// number of enabled transitions
        static index_type CardEnabled;

        /// When transition i fires from m to m', DeltaHash[i] is hash(m') - hash(m).
        /// This value is independent of m and m'
        static int* DeltaHash;

        /// That many places need to be changed when transition i fires
        /// CardDeltaT[PRE] for places that lose tokens, CardDeltaT[POST] for those that gain tokens
        static index_type* CardDeltaT[2];

        /// These are the places that change their marking when transition i fires
        /// DeltaT[PRE] for places that lose tokens, DeltaT[POST] for those that gain tokens
        static index_type** DeltaT[2];

        /// This is the amount of change
        static mult_type** MultDeltaT[2];

        /// After firing transition i, that many transitions must be checked for enabledness if they
        /// have been enabled before: card(DeltaT[PRE]\bullet)
        static index_type* CardConflicting;

        /// After firing transition i, these transitions must be checked for enabledness if they
        /// have been enabled before: (DeltaT[PRE]\bullet)
        static index_type** Conflicting;

        /// After reverse firing transition i, that many transitions must be checked for enabledness if they
        /// have been enabled before: card(DeltaT[POST]\bullet)
        static index_type* CardBackConflicting;

        /// After firing transition i, these transitions must be checked for enabledness if they
        /// have been enabled before: (DeltaT[POST]\bullet)
        static index_type** BackConflicting;

        /// If transition i is disabled, this is its position in its scapegpat's Disabled list
        /// Whenever t gets disabled, we sort its Pre list such that the scapegoat is the first
        /// entry there.
        static index_type* PositionScapegoat;
};


/// Aufräumen der Knoten - Service für valgrind
extern void deleteTransitions();
