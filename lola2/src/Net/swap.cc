/*!
\file swap.cc
\author karsten
\status new

\brief swap two place indices in all Marking, Transition, Place, and Net data
structures

\todo Diese Funktion in die Klasse Net verschieben?
*/

/// swap two places

#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <Net/Marking.h>
#include <Net/swap.h>

// LCOV_EXCL_START
bool DEBUG__isConsistent()
{
    for (int type = PL; type <= TR; ++type)
    {
        node_t othertype = (type == PL) ? TR : PL;
        for (int direction = PRE; direction <= POST; direction++)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE;
            for (index_t n = 0; n < Net::Card[type] ; n ++)
            {
                for (index_t a = 0; a < Net::CardArcs[type][direction][n]; a++)
                {
                    index_t nn = Net::Arc[type][direction][n][a];
                    index_t b;
                    for (b = 0; b < Net::CardArcs[othertype][otherdirection][nn]; b++)
                    {
                        if (Net::Arc[othertype][otherdirection][nn][b] == n)
                        {
                            break;
                        }
                    }
                    if (b >= Net::CardArcs[othertype][otherdirection][nn])
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
// LCOV_EXCL_STOP

/*!
\todo remove TargetMarking ?
*/
void swapPlaces(index_t left, index_t right)
{
    // 1. Net data structures

    char* tempname = Net::Name[PL][left];
    Net::Name[PL][left] = Net::Name[PL][right];
    Net::Name[PL][right] = tempname;
    assert(DEBUG__isConsistent());

    for (int direction = PRE; direction <= POST; ++direction)
    {
        index_t tempindex = Net::CardArcs[PL][direction][left];
        Net::CardArcs[PL][direction][left] = Net::CardArcs[PL][direction][right];
        Net::CardArcs[PL][direction][right] = tempindex;

        index_t* tempindexpointer = Net::Arc[PL][direction][left];
        Net::Arc[PL][direction][left] = Net::Arc[PL][direction][right];
        Net::Arc[PL][direction][right] = tempindexpointer;

        mult_t* tempmultpointer = Net::Mult[PL][direction][left];
        Net::Mult[PL][direction][left] = Net::Mult[PL][direction][right];
        Net::Mult[PL][direction][right] = tempmultpointer;
    }

    // the tricky part of 1.: references to left and right in transition arc lists

    // It is tricky because both places may refer to the same transition, so we do not
    // trivially known whether a change has already been done. Scanning through all transitions
    // rather than the environments of left and right is too costly, so we chose to go
    // through all transitions in the environment of one transition twice with setting temporary indices in the
    // first run.

    // Part of the trick is that all Net:: information for place left is already at index right

    // first run left
    for (int direction = PRE; direction <= POST ; ++direction)
    {
        for (index_t a = 0; a < Net::CardArcs[PL][direction][right]; ++a)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE ;
            const index_t t = Net::Arc[PL][direction][right][a];
            for (index_t b = 0; b < Net::CardArcs[TR][otherdirection][t]; b++)
            {
                if (Net::Arc[TR][otherdirection][t][b] == left)
                {
                    Net::Arc[TR][otherdirection][t][b] = Net::Card[PL];
                }
            }
        }
    }
    // only run right
    for (int direction = PRE; direction <= POST ; ++direction)
    {
        for (index_t a = 0; a < Net::CardArcs[PL][direction][left]; ++a)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE ;
            const index_t t = Net::Arc[PL][direction][left][a];
            for (index_t b = 0; b < Net::CardArcs[TR][otherdirection][t]; b++)
            {
                if (Net::Arc[TR][otherdirection][t][b] == right)
                {
                    Net::Arc[TR][otherdirection][t][b] = left;
                }
            }
        }
    }
    // second run left
    for (int direction = PRE; direction <= POST ; ++direction)
    {
        for (index_t a = 0; a < Net::CardArcs[PL][direction][right]; ++a)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE ;
            const index_t t = Net::Arc[PL][direction][right][a];
            for (index_t b = 0; b < Net::CardArcs[TR][otherdirection][t]; b++)
            {
                if (Net::Arc[TR][otherdirection][t][b] == Net::Card[PL])
                {
                    Net::Arc[TR][otherdirection][t][b] = right;
                }
            }
        }
    }
    assert(DEBUG__isConsistent());

    // 2. Place data structures

    hash_t temphash = Place::Hash[left];
    Place::Hash[left] = Place::Hash[right];
    Place::Hash[right] = temphash;

    capacity_t tempcapacity = Place::Capacity[left];
    Place::Capacity[left] = Place::Capacity[right];
    Place::Capacity[right] = tempcapacity;

    cardbit_t tempcardbit = Place::CardBits[left];
    Place::CardBits[left] = Place::CardBits[right];
    Place::CardBits[right] = tempcardbit;

    index_t tempcarddisabled = Place::CardDisabled[left];
    Place::CardDisabled[left] = Place::CardDisabled[right];
    Place::CardDisabled[right] = tempcarddisabled;

    index_t* tempdisabled = Place::Disabled[left];
    Place::Disabled[left] = Place::Disabled[right];
    Place::Disabled[right] = tempdisabled;

    // 3. Marking data structures

    capacity_t tempmarking = Marking::Initial[left];
    Marking::Initial[left] = Marking::Initial[right];
    Marking::Initial[right] = tempmarking;

    if (Marking::Current)
    {
        tempmarking = Marking::Current[left];
        Marking::Current[left] = Marking::Current[right];
        Marking::Current[right] = tempmarking;
    }

    // so far, target markings are not in use, and perhaps never will be
    // since we focus on predicates
    // LCOV_EXCL_START
    if (Marking::Target)
    {
        tempmarking = Marking::Target[left];
        Marking::Target[left] = Marking::Target[right];
        Marking::Target[right] = tempmarking;
    }
    // LCOV_EXCL_STOP

    // 4. Transition data structures

    // again, the tricky way...
    // Part of the trick is that all Net:: information for place left is already at index right

    // first run left
    for (int direction = PRE; direction <= POST ; ++direction)
    {
        for (index_t a = 0; a < Net::CardArcs[PL][direction][right]; ++a)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE ;
            const index_t t = Net::Arc[PL][direction][right][a];
            for (index_t b = 0; b < Transition::CardDeltaT[otherdirection][t]; b++)
            {
                if (Transition::DeltaT[otherdirection][t][b] == left)
                {
                    Transition::DeltaT[otherdirection][t][b] = Net::Card[PL];
                }
            }
        }
    }
    // only run right
    for (int direction = PRE; direction <= POST ; ++direction)
    {
        for (index_t a = 0; a < Net::CardArcs[PL][direction][left]; ++a)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE ;
            const index_t t = Net::Arc[PL][direction][left][a];
            for (index_t b = 0; b < Transition::CardDeltaT[otherdirection][t]; b++)
            {
                if (Transition::DeltaT[otherdirection][t][b] == right)
                {
                    Transition::DeltaT[otherdirection][t][b] = left;
                }
            }
        }
    }
    for (int direction = PRE; direction <= POST ; ++direction)
    {
        // second run left
        for (index_t a = 0; a < Net::CardArcs[PL][direction][right]; ++a)
        {
            direction_t otherdirection = (direction == PRE) ? POST : PRE ;
            const index_t t = Net::Arc[PL][direction][right][a];
            for (index_t b = 0; b < Transition::CardDeltaT[otherdirection][t]; b++)
            {
                if (Transition::DeltaT[otherdirection][t][b] == Net::Card[PL])
                {
                    Transition::DeltaT[otherdirection][t][b] = right;
                }
            }
        }
    }
    assert(DEBUG__isConsistent());
}
