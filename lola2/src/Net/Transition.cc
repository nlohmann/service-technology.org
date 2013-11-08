/*!
\file Transition.cc
\author Karsten
\status approved 27.01.2012,
		changed progress measure to int32 on 18.12.2012
\brief Useful routines for transition specific information

All data that describe attributes of transitions can be found here. General
information for a transition in its role as a node, ar contained in Node.*
*/

#include <cstdlib>
#include <config.h>
#include <Core/Dimensions.h>
#include <Parser/FairnessAssumptions.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <Net/Marking.h>
#include <InputOutput/Reporter.h>

extern Reporter *rep;

fairnessAssumption_t *Transition::Fairness = NULL;
bool *Transition::Enabled = NULL;
index_t Transition::CardEnabled = 0;
hash_t *Transition::DeltaHash = NULL;
index_t *Transition::CardDeltaT[2] = {NULL};
index_t **Transition::DeltaT[2] = {NULL};
mult_t **Transition::MultDeltaT[2] = {NULL};
index_t *Transition::CardConflicting = NULL;
bool *Transition::ConflictingIsOriginal = NULL;
index_t **Transition::Conflicting = NULL;
index_t *Transition::CardBackConflicting = NULL;
index_t **Transition::BackConflicting = NULL;
bool *Transition::BackConflictingIsOriginal = NULL;
int32_t *Transition::ProgressMeasure = NULL;

/*!
Whenever t gets disabled, we sort its Pre list such that the scapegoat is the
first entry there.
*/
index_t *Transition::PositionScapegoat = NULL;

/*!
\brief clean up transitions for valgrind
*/
void Transition::deleteTransitions()
{
    free(Transition::Fairness);
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
        if (Transition::ConflictingIsOriginal[i])
        {
            free(Transition::Conflicting[i]);
        }
        if (Transition::BackConflictingIsOriginal[i])
        {
            free(Transition::BackConflicting[i]);
        }
    }
    free(Transition::Conflicting);
    free(Transition::BackConflicting);
    free(Transition::ConflictingIsOriginal);
    free(Transition::BackConflictingIsOriginal);
    free(Transition::PositionScapegoat);
    free(Transition::ProgressMeasure);
}

void Transition::checkTransitions(NetState &ns)
{
    // for places
    for (int i = 0; i < Net::Card[PL]; i++)
    {
        for (int j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            assert(ns.Arc[PL][PRE][i][j] < Net::Card[TR]);
        }
        for (int j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            assert(ns.Arc[PL][POST][i][j] < Net::Card[TR]);
        }
    }

    // for transitions
    for (int i = 0; i < Net::Card[TR]; i++)
    {
        for (int j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            assert(ns.Arc[TR][PRE][i][j] < Net::Card[PL]);
        }
        for (int j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            assert(ns.Arc[TR][POST][i][j] < Net::Card[PL]);
        }
    }
}


/// Check transition for activation
/// 1. scan through pre-places for testing eanbledness
/// 2. if enabled ->disabled, insert in Disabled list of scapegoat
/// 3. if disabled->enabled, Remove from Disabled list of scapegoat
/// 4. if disabled->disabled, perhaps move to other scapegoat
/// \todo swap of values to be done with XOR (ineffektiv(er) bei heutigen Compilern! Außerdem haben wir zwei Swaps verschränkt...)
void Transition::checkEnabled(NetState &ns, index_t t)
{
    // scan through all pre-places
    for (index_t i = 0; i < Net::CardArcs[TR][PRE][t]; ++i)
    {
        if (ns.Current[ns.Arc[TR][PRE][t][i]] < ns.Mult[TR][PRE][t][i])
        {
            // transition is disabled, ns.Arc[TR][PRE][t][i] is the
            // first place that is not sufficiently marked -> it is the new scapegpat
            const index_t scapegoat = ns.Arc[TR][PRE][t][i];
            if (ns.Enabled[t])
            {
                // enabled --> disabled: insert to scapegoat's disabled list
                ns.Enabled[t] = false;
                --ns.CardEnabled;
                assert(ns.CardDisabled[scapegoat] <= Net::CardArcs[PL][POST][scapegoat]);
                ns.Disabled[scapegoat][ns.PositionScapegoat[t] = ns.CardDisabled[scapegoat]++] = t;

                // swap scapegoat to front of transition's PRE list
                if (i > 0)
                {
                    mult_t tmp = ns.Mult[TR][PRE][t][i];
                    ns.Arc[TR][PRE][t][i] = ns.Arc[TR][PRE][t][0];
                    ns.Mult[TR][PRE][t][i] = ns.Mult[TR][PRE][t][0];
                    ns.Arc[TR][PRE][t][0] = scapegoat;
                    ns.Mult[TR][PRE][t][0] = tmp;
                }
            }
            else
            {
                // disabled --> disabled: perhaps scapegoat has changed
                if (i > 0)
                {
                    // indeed, scapegoat has changed.
                    // remove from old scapegoat's Disabled list
                    const index_t old_scapegoat = ns.Arc[TR][PRE][t][0];
                    index_t c = --ns.CardDisabled[old_scapegoat];
                    if (ns.PositionScapegoat[t] != c)
                    {
                        // transition not last in scapegoat's disabled list--> swap with last
                        const index_t other_t = ns.Disabled[old_scapegoat][c];
                        ns.PositionScapegoat[other_t] = ns.PositionScapegoat[t];
                        ns.Disabled[old_scapegoat][ns.PositionScapegoat[t]] = other_t;
                    }
                    else
                    {
                        // transition last in old scapgoat's list --> decrement of CardDisabled in
                        // if condition was all we needed to do
                    }

                    // insert to new scapegoat's disabled list
                    ns.Disabled[scapegoat][ns.PositionScapegoat[t] = ns.CardDisabled[scapegoat]++] = t;
                    // swap new scapegoat to front of transition's PRE list
                    const mult_t tmp = ns.Mult[TR][PRE][t][i];
                    ns.Arc[TR][PRE][t][i] = ns.Arc[TR][PRE][t][0];
                    ns.Mult[TR][PRE][t][i] = ns.Mult[TR][PRE][t][0];
                    ns.Arc[TR][PRE][t][0] = scapegoat;
                    ns.Mult[TR][PRE][t][0] = tmp;
                }
            }
            // case of deactivated transition is complete
            return;
        }
    }
    // for loop completed: we did not find an insufficiently marked place
    // => transition enabled
    if (!ns.Enabled[t])
    {
        // disabled-->enabled: remove from scapegoat's disabled list
        ns.Enabled[t] = true;
        ++ns.CardEnabled;
        const index_t old_scapegoat = ns.Arc[TR][PRE][t][0];
        const index_t c = --ns.CardDisabled[old_scapegoat];
        if (ns.PositionScapegoat[t] != c)
        {
            // transition not last in scapegoat's disabed list--> swap with last
            const index_t other_t = ns.Disabled[old_scapegoat][c];
            ns.PositionScapegoat[other_t] = ns.PositionScapegoat[t];
            ns.Disabled[old_scapegoat][ns.PositionScapegoat[t]] = other_t;
        }
    }
}


void Transition::checkEnabled_Initial(index_t t)
{
    NetState ns;
    ns.Current = Marking::Current;
    ns.HashCurrent = Marking::HashCurrent;
    ns.Enabled = Transition::Enabled;
    ns.CardEnabled = Transition::CardEnabled;
    ns.PositionScapegoat = Transition::PositionScapegoat;
    ns.Arc[0][0] = Net::Arc[0][0];
    ns.Arc[0][1] = Net::Arc[0][1];
    ns.Arc[1][0] = Net::Arc[1][0];
    ns.Arc[1][1] = Net::Arc[1][1];
    ns.Mult[0][0] = Net::Mult[0][0];
    ns.Mult[0][1] = Net::Mult[0][1];
    ns.Mult[1][0] = Net::Mult[1][0];
    ns.Mult[1][1] = Net::Mult[1][1];
    ns.CardDisabled = Place::CardDisabled;
    ns.Disabled = Place::Disabled;
    checkEnabled(ns, t);
    Marking::HashCurrent = ns.HashCurrent;
    Transition::CardEnabled = ns.CardEnabled;
    Net::Arc[0][0] = ns.Arc[0][0];
    Net::Arc[0][1] = ns.Arc[0][1];
    Net::Arc[1][0] = ns.Arc[1][0];
    Net::Arc[1][1] = ns.Arc[1][1];
    Net::Mult[0][0] = ns.Mult[0][0];
    Net::Mult[0][1] = ns.Mult[0][1];
    Net::Mult[1][0] = ns.Mult[1][0];
    Net::Mult[1][1] = ns.Mult[1][1];
}


/// fire a transition and
void Transition::fire(NetState &ns, index_t t)
{
    //rep->message("F %d",t);

    if (!ns.Enabled[t])
    {
        rep->message("=================ERRROR==================");
        rep->message("TRY TO FIRE %s", Net::Name[TR][t]);
        rep->message("current marking %s:%d %s:%d %s:%d %s:%d", Net::Name[PL][0], ns.Current[0],
                     Net::Name[PL][1], ns.Current[1], Net::Name[PL][2], ns.Current[2], Net::Name[PL][3], ns.Current[3]);
    }

    //  Don't even think about firing a disabled transition!
    assert(ns.Enabled[t]);

    // 1. Update current marking
    for (index_t i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
    {
        // there should be enough tokens to fire this transition
        assert(ns.Current[Transition::DeltaT[PRE][t][i]] >= Transition::MultDeltaT[PRE][t][i]);

        ns.Current[Transition::DeltaT[PRE][t][i]] -= Transition::MultDeltaT[PRE][t][i];
    }
    for (index_t i = 0; i < Transition::CardDeltaT[POST][t]; i++)
    {
        ns.Current[Transition::DeltaT[POST][t][i]] += Transition::MultDeltaT[POST][t][i];
    }
    // 2. update hash value
    ns.HashCurrent += Transition::DeltaHash[t];
    ns.HashCurrent %= SIZEOF_MARKINGTABLE;
    if (UNLIKELY(ns.HashCurrent < 0))
    {
        // just safety belt, if % returns negative value
        // LCOV_EXCL_START
        ns.HashCurrent += SIZEOF_MARKINGTABLE;
        // LCOV_EXCL_STOP
    }
}


bool Transition::isCycle(NetState &ns, index_t t)
{
    index_t *delta_value = new index_t[Net::Card[PL]]();

    for (index_t i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
    {
        delta_value[Transition::DeltaT[PRE][t][i]] = Transition::MultDeltaT[PRE][t][i];
    }
    for (index_t i = 0; i < Transition::CardDeltaT[POST][t]; i++)
    {
        if (delta_value[Transition::DeltaT[POST][t][i]] != Transition::MultDeltaT[POST][t][i])
        {
            delete[] delta_value;
            return false;
        }
    }

    delete[] delta_value;
    return true;
}

/// update enabledness information after having fired a transition
void Transition::updateEnabled(NetState &ns, index_t t)
{
    //rep->message("UE %d",t);
    // 1. check conflicting enabled transitions (tt) for enabledness
    for (index_t i = 0; i < Transition::CardConflicting[t]; i++)
    {
        const index_t tt = Transition::Conflicting[t][i];
        if (ns.Enabled[tt])
        {
            checkEnabled(ns, tt);
        }
    }

    // 2. check those transitions where the scapegoat received additional tokens
    for (index_t i = 0; i < Transition::CardDeltaT[POST][t]; i++)
    {
        // one place that got new tokens
        const index_t p = Transition::DeltaT[POST][t][i];
        for (index_t j = 0; j < ns.CardDisabled[p]; /* tricky increment handling */)
        {
            const index_t tt = ns.Disabled[p][j];
            checkEnabled(ns, tt);
            if (ns.Disabled[p][j] == tt)
            {
                j++; /* tricky increment handling */
            }
        }
    }

}

/// fire a transition in reverse direction (for backtracking) and update enabledness of all transitions
void Transition::backfire(NetState &ns, index_t t)
{
    //rep->message("B %d",t);
    // 1. Update current marking
    for (index_t i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
    {
        ns.Current[Transition::DeltaT[PRE][t][i]] += Transition::MultDeltaT[PRE][t][i];
    }
    for (index_t i = 0; i < Transition::CardDeltaT[POST][t]; i++)
    {
        // there should be enough tokens to backfire this transition
        assert(ns.Current[Transition::DeltaT[POST][t][i]] >= Transition::MultDeltaT[POST][t][i]);

        ns.Current[Transition::DeltaT[POST][t][i]] -= Transition::MultDeltaT[POST][t][i];
    }
    // 2. update hash value
    ns.HashCurrent -= Transition::DeltaHash[t];
    ns.HashCurrent %= SIZEOF_MARKINGTABLE;
    while (ns.HashCurrent < 0)
    {
        ns.HashCurrent += SIZEOF_MARKINGTABLE;
    }
}

/// update enabledness after having backfired a transition
void Transition::revertEnabled(NetState &ns, index_t t)
{
    //rep->message("RE %d",t);
    // 1. check backward conflicting enabled transitions for enabledness
    for (index_t i = 0; i < Transition::CardBackConflicting[t]; i++)
    {
        const index_t tt = Transition::BackConflicting[t][i];
        if (ns.Enabled[tt])
        {
            checkEnabled(ns, tt);
        }
    }
    // t is not necessarily contained in its own back-conflicting set (unlike the conflicting set)
    checkEnabled(ns, t);

    // 2. check those transitions where the scapegoat received additional tokens
    for (index_t i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
    {
        const index_t p = Transition::DeltaT[PRE][t][i]; // one place that got new tokens
        for (index_t j = 0; j < ns.CardDisabled[p]; /* tricky increment handling */)
        {
            const index_t tt = ns.Disabled[p][j];
            checkEnabled(ns, tt);
            if (ns.Disabled[p][j] == tt)
            {
                j++; /* tricky increment handling */
            }
        }
    }

}

// LCOV_EXCL_START
bool DEBUG__testEnabled(NetState &ns, index_t t)
{
    if (ns.Enabled[t])
    {
        for (index_t i = 0; i < Net::CardArcs[TR][PRE][t]; i++)
        {
            index_t p = ns.Arc[TR][PRE][t][i];
            if (ns.Current[p] < ns.Mult[TR][PRE][t][i])
            {
                return false;
            }
        }
    }
    else
    {
        index_t p = ns.Arc[TR][PRE][t][0];
        if (ns.Current[p] >= ns.Mult[TR][PRE][t][0])
        {
            return false;
        }
    }
    return true;
}
// LCOV_EXCL_STOP
