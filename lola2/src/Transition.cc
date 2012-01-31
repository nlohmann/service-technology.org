/*!
\file Transition.cc
\author Karsten
\status approved 27.01.2012
\brief Useful routines for transition specific information

All data that describe attributes of transitions can be found here. General
information for a transition in its role as a node, ar contained in Node.*

\todo Lohnt es sich, Integers als Referenz zu übergeben? -> Nein (Andreas)
*/

#include <cstdlib>
#include "Net.h"
#include "Place.h"
#include "Transition.h"
#include "Marking.h"
#include "Dimensions.h"
#include "FairnessAssumptions.h"
#include "BitSetC.h"

fairnessAssumption_t* Transition::Fairness = NULL;
bool* Transition::Enabled = NULL;
index_t Transition::CardEnabled = 0;
hash_t* Transition::DeltaHash = NULL;
index_t* Transition::CardDeltaT[2] = {NULL};
index_t** Transition::DeltaT[2] = {NULL};
mult_t** Transition::MultDeltaT[2] = {NULL};
index_t* Transition::CardConflicting = NULL;
index_t** Transition::Conflicting = NULL;
index_t* Transition::CardBackConflicting = NULL;
index_t** Transition::BackConflicting = NULL;
index_t* Transition::PositionScapegoat = NULL;

/*!
\brief clean up transitions for valgrind
*/
void Transition::deleteTransitions()
{
    //free(Transition::Fairness);
    free(Transition::Enabled);
    free(Transition::DeltaHash);
    for (int direction = PRE; direction <= POST; direction++)
    {
        for (index_t i = 0; i < Net::Card[TR]; i++)
        {
            free(Transition::MultDeltaT[direction][i]);
            free(Transition::DeltaT[direction][i]);
        }
        free(Transition::CardDeltaT[direction]);
        free(Transition::DeltaT[direction]);
        free(Transition::MultDeltaT[direction]);
    }
    free(Transition::CardConflicting);
    free(Transition::CardBackConflicting);
    for (index_t i = 0; i < Net::Card[TR]; i++)
    {
        free(Transition::Conflicting[i]);
        free(Transition::BackConflicting[i]);
    }
    free(Transition::Conflicting);
    free(Transition::BackConflicting);
    free(Transition::PositionScapegoat);
}


/// Check transition for activation
/// 1. scan through pre-places for testing eanbledness
/// 2. if enabled ->disabled, insert in Disabled list of scapegoat
/// 3. if disabled->enabled, Remove from Disabled list of scapegoat
/// 4. if disabled->disabled, perhaps move to other scapegoat
/// \todo swap of values to be done with XOR (ineffektiv(er) bei heutigen Compilern! Außerdem haben wir zwei Swaps verschränkt...)
void Transition::checkEnabled(index_t t)
{
    // scan through all pre-places
    for (index_t i = 0; i < Net::CardArcs[TR][PRE][t]; ++i)
    {
        if (Marking::Current[Net::Arc[TR][PRE][t][i]] < Net::Mult[TR][PRE][t][i])
        {
            // transition is disabled, Net::Arc[TR][PRE][t][i] is the
            // first place that is not sufficiently marked -> it is the new scapegpat
            const index_t scapegoat = Net::Arc[TR][PRE][t][i];
            if (Transition::Enabled[t])
            {
                // enabled --> disabled: insert to scapegoat's disabled list
                Transition::Enabled[t] = false;
                --Transition::CardEnabled;
                Place::Disabled[scapegoat][Transition::PositionScapegoat[t] = Place::CardDisabled[scapegoat]++] = t;

                // swap scapegoat to front of transition's PRE list
                if (i > 0)
                {
                    mult_t tmp = Net::Mult[TR][PRE][t][i];
                    Net::Arc[TR][PRE][t][i] = Net::Arc[TR][PRE][t][0];
                    Net::Mult[TR][PRE][t][i] = Net::Mult[TR][PRE][t][0];
                    Net::Arc[TR][PRE][t][0] = scapegoat;
                    Net::Mult[TR][PRE][t][0] = tmp;
                }
            }
            else
            {
                // disabled --> disabled: perhaps scapegoat has changed
                if (i > 0)
                {
                    // indeed, scapegoat has changed.
                    // remove from old scapegoat's Disabled list
                    const index_t old_scapegoat = Net::Arc[TR][PRE][t][0];
                    index_t c = --Place::CardDisabled[old_scapegoat];
                    if (Transition::PositionScapegoat[t] != c)
                    {
                        // transition not last in scapegoat's disabled list--> swap with last
                        const index_t other_t = Place::Disabled[old_scapegoat][c];
                        Transition::PositionScapegoat[other_t] = Transition::PositionScapegoat[t];
                        Place::Disabled[old_scapegoat][Transition::PositionScapegoat[t]] = other_t;
                    }
                    else
                    {
                        // transition last in old scapgoat's list --> decrement of CardDisabled in
                        // if condition was all we needed to do
                    }

                    // insert to new scapegoat's disabled list
                    Place::Disabled[scapegoat][Transition::PositionScapegoat[t] = Place::CardDisabled[scapegoat]++] = t;
                    // swap new scapegoat to front of transition's PRE list
                    const mult_t tmp = Net::Mult[TR][PRE][t][i];
                    Net::Arc[TR][PRE][t][i] = Net::Arc[TR][PRE][t][0];
                    Net::Mult[TR][PRE][t][i] = Net::Mult[TR][PRE][t][0];
                    Net::Arc[TR][PRE][t][0] = scapegoat;
                    Net::Mult[TR][PRE][t][0] = tmp;
                }
            }
            // case of deactivated transition is complete
            return;
        }
    }
    // for loop completed: we did not find an insufficiently marked place
    // => transition enabled
    if (!Transition::Enabled[t])
    {
        // disabled-->enabled: remove from scapegoat's disabled list
        Transition::Enabled[t] = true;
        ++Transition::CardEnabled;
        const index_t old_scapegoat = Net::Arc[TR][PRE][t][0];
        const index_t c = --Place::CardDisabled[old_scapegoat];
        if (Transition::PositionScapegoat[t] != c)
        {
            // transition not last in scapegoat's disabed list--> swap with last
            const index_t other_t = Place::Disabled[old_scapegoat][c];
            Transition::PositionScapegoat[other_t] = Transition::PositionScapegoat[t];
            Place::Disabled[old_scapegoat][Transition::PositionScapegoat[t]] = other_t;
        }
    }
}


/// fire a transition and update enabledness of all transitions
void Transition::fire(index_t t)

{
    //  Don't even think about firing a disabled transition!
    assert(Transition::Enabled[t]);
    // 1. Update current marking
    for (index_t i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
    {
        // there should be enough tokens to fire this transition
        assert(Marking::Current[Transition::DeltaT[PRE][t][i]] >= Transition::MultDeltaT[PRE][t][i]);

        Marking::Current[Transition::DeltaT[PRE][t][i]] -= Transition::MultDeltaT[PRE][t][i];
    }
    for (index_t i = 0; i < Transition::CardDeltaT[POST][t]; i++)
    {
        Marking::Current[Transition::DeltaT[POST][t][i]] += Transition::MultDeltaT[POST][t][i];
    }

    // 2. check conflicting enabled transitions (tt) for enabledness
    for (index_t i = 0; i < Transition::CardConflicting[t]; i++)
    {
        const index_t tt = Transition::Conflicting[t][i];
        if (Transition::Enabled[tt])
        {
            checkEnabled(tt);
        }
    }
    // 2a. Don't forget to check t itself! It is not member of the conflicting list!
    checkEnabled(t);

    // 3. check those transitions where the scapegoat received additional tokens
    for (index_t i = 0; i < Transition::CardDeltaT[POST][t]; i++)
    {
        const index_t p = Transition::DeltaT[POST][t][i]; // one place that got new tokens
        for (index_t j = 0; j < Place::CardDisabled[p]; /* tricky increment handling */)
        {
            const index_t tt = Place::Disabled[p][j];
            checkEnabled(tt);
            if (Place::Disabled[p][j] == tt)
            {
                j++; /* tricky increment handling */
            }
        }
    }

    // 4. update hash value
    Marking::HashCurrent += Transition::DeltaHash[t];
    Marking::HashCurrent %= SIZEOF_MARKINGTABLE;
}

/// fire a transition in reverse direction (for backtracking) and update enabledness of all transitions
void Transition::backfire(index_t t)
{
    // 1. Update current marking
    for (index_t i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
    {
        Marking::Current[Transition::DeltaT[PRE][t][i]] += Transition::MultDeltaT[PRE][t][i];
    }
    for (index_t i = 0; i < Transition::CardDeltaT[POST][t]; i++)
    {
        // there should be enough tokens to fire this transition
        assert(Transition::DeltaT[POST][t][i] >= Transition::MultDeltaT[POST][t][i]);

        Marking::Current[Transition::DeltaT[POST][t][i]] -= Transition::MultDeltaT[POST][t][i];
    }

    // 2. check backward conflicting enabled transitions for enabledness
    for (index_t i = 0; i < Transition::CardBackConflicting[t]; i++)
    {
        const index_t tt = Transition::BackConflicting[t][i];
        if (Transition::Enabled[tt])
        {
            checkEnabled(tt);
        }
    }

    // 3. check those transitions where the scapegoat received additional tokens
    for (index_t i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
    {
        for (index_t j = 0; j < Place::CardDisabled[Transition::DeltaT[PRE][t][i]]; /* tricky increment handling */)
        {
            const index_t tt = Transition::DeltaT[PRE][t][i];
            checkEnabled(Place::Disabled[tt][j]);
            if (!Transition::Enabled[tt])
            {
                j++; /* tricky increment handling */
            }
            else
            {
                // in this case, checkEnabled decremente CardDisabled and swaps
                // another transition to position j
            }
        }
    }

    // 4. update hash value
    Marking::HashCurrent -= Transition::DeltaHash[t];
    Marking::HashCurrent %= SIZEOF_MARKINGTABLE;
}
