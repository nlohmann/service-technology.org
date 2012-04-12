/*!
\file Transition.h
\author Karsten
\status approved 27.01.2012
\brief Global data for transition specific information

All data that describe attributes of transitions can be found here. General
information for a transition in its role as a node, ar contained in Node.*
*/

#pragma once

#include "Core/Dimensions.h"
#include "Parser/FairnessAssumptions.h"

/*!
\brief collection of information related to transitions
*/
struct Transition
{
    public:
        /// fairness specification
        static fairnessAssumption_t* Fairness;

        /// Activation status
        static bool* Enabled;

        /// number of enabled transitions
        static index_t CardEnabled;

        /// When transition i fires from m to m', DeltaHash[i] is hash(m') - hash(m). This value is independent of m and m'
        static hash_t* DeltaHash;

        /// That many places need to be changed when transition i fires CardDeltaT[PRE] for places that lose tokens, CardDeltaT[POST] for those that gain tokens
        static index_t* CardDeltaT[2];

        /// These are the places that change their marking when transition i fires DeltaT[PRE] for places that lose tokens, DeltaT[POST] for those that gain tokens
        static index_t** DeltaT[2];

        /// This is the amount of change
        static mult_t** MultDeltaT[2];

        /// After firing transition i, that many transitions must be checked for enabledness if they have been enabled before: \f$card(DeltaT[PRE]^\bullet)\f$
        static index_t* CardConflicting;

        /// After firing transition i, these transitions must be checked for enabledness if they have been enabled before: \f$(DeltaT[PRE]^\bullet)\f$
        static index_t** Conflicting;

        /// After reverse firing transition i, that many transitions must be checked for enabledness if they have been enabled before: \f$card(DeltaT[POST]^\bullet)\f$
        static index_t* CardBackConflicting;

        /// After firing transition i, these transitions must be checked for enabledness if they have been enabled before: \f$(DeltaT[POST]^\bullet)\f$
        static index_t** BackConflicting;

        /// If transition i is disabled, this is its position in its scapegpat's Disabled list
        static index_t* PositionScapegoat;

        /// Check transition for activation
        static void checkEnabled(index_t t);

        /// fire a transition
        static void fire(index_t t);

        /// update enabledness of all transitions
        static void updateEnabled(index_t t);

        /// fire a transition in reverse direction (for backtracking)
        static void backfire(index_t t);

        /// update enabledness of all transitions after backfiring
        static void revertEnabled(index_t t);

        /// Aufräumen der Knoten - Service für valgrind
        static void deleteTransitions();
};
