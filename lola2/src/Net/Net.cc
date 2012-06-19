/*
\file Net.cc
\author Karsten
\status approved 27.01.2012

\brief basic routines for handling nodes
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <Net/LinearAlgebra.h>
#include <Net/Net.h>
#include <Net/Marking.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <Parser/FairnessAssumptions.h>


index_t Net::Card[2] = {0, 0};
index_t* Net::CardArcs[2][2] = {{NULL, NULL}, {NULL, NULL}};
index_t** Net::Arc[2][2] = {{NULL, NULL}, {NULL, NULL}};
mult_t** Net::Mult[2][2] = {{NULL, NULL}, {NULL, NULL}};
char** Net::Name[2] = {NULL, NULL};

// LCOV_EXCL_START
bool Net::DEBUG__checkConsistency()
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

bool Net::DEBUG__checkArcOrdering()
{
    index_t curElem;
    for (index_t t = 0; t < Net::Card[TR]; ++t)
    {
        curElem = Net::Arc[TR][PRE][t][0];
        for (index_t p = 1; p < Net::CardArcs[TR][PRE][t]; ++p)
        {
            assert(curElem < Net::Arc[TR][PRE][t][p]);
        }
        curElem = Net::Arc[TR][POST][t][0];
        for (index_t p = 1; p < Net::CardArcs[TR][POST][t]; ++p)
        {
            assert(curElem < Net::Arc[TR][POST][t][p]);
        }
    }
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        curElem = Net::Arc[PL][PRE][p][0];
        for (index_t t = 1; t < Net::CardArcs[PL][PRE][p]; ++t)
        {
            assert(curElem < Net::Arc[PL][PRE][p][t]);
        }
        curElem = Net::Arc[PL][POST][p][0];
        for (index_t t = 1; t < Net::CardArcs[PL][POST][p]; ++t)
        {
            assert(curElem < Net::Arc[PL][POST][p][t]);
        }
    }
    return true;
}
// LCOV_EXCL_STOP

/// sorts array of arc (= node id) plus corresponding array of multiplicities
/// in the range of from to to (not including to)
void Net::sortArcs(index_t* arcs, mult_t* mults, const index_t from, const index_t to)
{
    if ((to - from) < 2)
    {
        return;    // less than 2 elements are always sorted
    }

    index_t blue = from; // points to first index where element is not known < pivot
    index_t white = from + 1; // points to first index where element is not know <= pivot
    index_t red = to; // points to last index (+1) where element is not know to be  pivot
    const index_t pivot = arcs[from];

    assert(from < to);

    while (red > white)
    {
        if (arcs[white] < pivot)
        {
            // swap white <->blue
            const index_t tmp_index = arcs[blue];
            const mult_t tmp_mult = mults[blue];
            arcs[blue] = arcs[white];
            mults[blue++] = mults[white];
            arcs[white] = tmp_index;
            mults[white++] = tmp_mult;
        }
        else
        {
            // there are no duplicates in arc list
            assert(arcs[white] > pivot);

            // swap white <->red
            const index_t tmp_index = arcs[--red];
            const mult_t tmp_mult = mults[red];
            arcs[red] = arcs[white];
            mults[red] = mults[white];
            arcs[white] = tmp_index;
            mults[white] = tmp_mult;
        }
    }

    assert(blue + 1 == red);

    sortArcs(arcs, mults, from, blue);
    sortArcs(arcs, mults, red, to);
}

void Net::sortAllArcs()
{
    for (int type = PL; type <= TR; ++type)
    {
        for (int direction = PRE; direction <= POST; direction++)
        {
            for (index_t n = 0; n < Net::Card[type] ; n ++)
            {
                sortArcs(Net::Arc[type][direction][n], Net::Mult[type][direction][n], 0, CardArcs[type][direction][n]);
            }
        }
    }
    assert(DEBUG__checkArcOrdering());
}

/// Free all allocated memory
void Net::deleteNodes()
{
    for (int type = PL; type <= TR; ++type)
    {
        for (int direction = PRE; direction <= POST; ++direction)
        {
            for (index_t i = 0; i < Net::Card[type]; i++)
            {
                free(Net::Arc[type][direction][i]);
                free(Net::Mult[type][direction][i]);
            }
            free(Net::CardArcs[type][direction]);
            free(Net::Arc[type][direction]);
            free(Net::Mult[type][direction]);
        }
        for (index_t i = 0; i < Net::Card[type]; i++)
        {
            free(Net::Name[type][i]);
        }
        free(Net::Name[type]);
    }
}

void Net::print()
{
    printf("Net\n===\n\n");

    printf("%u places,  %u  transitions.\n\n", Net::Card[PL], Net::Card[TR]);

    for (index_t i = 0; i < Net::Card[PL]; i++)
    {
        printf("Place %u :%s, %u tokens hash %ld capacity %u bits %u\n", i, Net::Name[PL][i], Marking::Initial[i], Place::Hash[i], Place::Capacity[i], Place::CardBits[i]);
        printf("From (%u):\n", Net::CardArcs[PL][PRE][i]);
        for (index_t j = 0; j < Net::CardArcs[PL][PRE][i]; j++)
        {
            printf("%s:%d ", Net::Name[TR][Net::Arc[PL][PRE][i][j]], Net::Mult[PL][PRE][i][j]);
        }
        printf("\n");
        printf("To (%u):\n", Net::CardArcs[PL][POST][i]);
        for (index_t j = 0; j < Net::CardArcs[PL][POST][i]; j++)
        {
            printf("%s:%d ", Net::Name[TR][Net::Arc[PL][POST][i][j]], Net::Mult[PL][POST][i][j]);
        }
        printf("\n");

    }
    for (index_t i = 0; i < Net::Card[TR]; i++)
    {
        printf("\nTransition %u :%s\n", i, Net::Name[TR][i]);
        switch (Transition::Fairness[i])
        {
            case NO_FAIRNESS:
                printf(" no ");
                break;
            case WEAK_FAIRNESS:
                printf(" weak ");
                break;
            case STRONG_FAIRNESS:
                printf(" strong ");
                break;
        }
        printf(" %s ", (Transition::Enabled[i] ? "enabled" : "disabled"));

        printf("From:\n");
        for (index_t j = 0; j < Net::CardArcs[TR][PRE][i]; j++)
        {
            printf("%s:%u ", Net::Name[PL][Net::Arc[TR][PRE][i][j]], Net::Mult[TR][PRE][i][j]);
        }
        printf("\n");
        printf("To:\n");
        for (index_t j = 0; j < Net::CardArcs[TR][POST][i]; j++)
        {
            printf("%s:%u ", Net::Name[PL][Net::Arc[TR][POST][i][j]], Net::Mult[TR][POST][i][j]);
        }
        printf("\n");
        printf("remove from");
        for (index_t j = 0; j < Transition::CardDeltaT[PRE][i]; j++)
        {
            printf(" %s:%u", Net::Name[PL][Transition::DeltaT[PRE][i][j]], Transition::MultDeltaT[PRE][i][j]);
        }
        printf("produce on");
        for (index_t j = 0; j < Transition::CardDeltaT[POST][i]; j++)
        {
            printf(" %s:%u", Net::Name[PL][Transition::DeltaT[POST][i][j]], Transition::MultDeltaT[POST][i][j]);
        }
        printf("\n conflicting:");
        for (index_t j = 0; j < Transition::CardConflicting[i]; j++)
        {
            printf("%s", Net::Name[TR][Transition::Conflicting[i][j]]);
        }
        printf("\n");
        for (index_t j = 0; j < Transition::CardBackConflicting[i]; j++)
        {
            printf("%s", Net::Name[TR][Transition::BackConflicting[i][j]]);
        }
    }
    printf("done\n");
}

/*!
\todo remove TargetMarking ?
*/
void Net::swapPlaces(index_t left, index_t right)
{
    // 1. Net data structures

    char* tempname = Net::Name[PL][left];
    Net::Name[PL][left] = Net::Name[PL][right];
    Net::Name[PL][right] = tempname;
    assert(DEBUG__checkConsistency());

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
    assert(DEBUG__checkConsistency());

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
    assert(DEBUG__checkConsistency());
}

/// Creates a equation for the given transition (index) in the provided memory
void createTransitionEquation(index_t transition, index_t* variables, int64_t* coefficients, index_t &size)
{
    // index in new row
    size = 0;
    // for each place p in the preset of t
    for (index_t p = 0; p < Net::CardArcs[TR][PRE][transition]; ++p)
    {
        // store place index and the it's multiplicity (from p to t)
        variables[size] = Net::Arc[TR][PRE][transition][p];
        // positive numbers
        assert(Net::Mult[TR][PRE][transition][p] != 0);
        coefficients[size] = Net::Mult[TR][PRE][transition][p];
        // increase newSize
        ++size;
    }
    // for each place p in the postset of t
    for (index_t p = 0; p < Net::CardArcs[TR][POST][transition]; ++p)
    {
        const index_t pID = Net::Arc[TR][POST][transition][p];
        assert(Net::Mult[TR][POST][transition][p] != 0);

        // check whether the p is already in the new row
        // enumerate newVar till p is hit or not inside
        index_t possiblePosition = 0;
        for (; possiblePosition < size; ++possiblePosition)
        {
            if (variables[possiblePosition] >= pID)
            {
                break;
            }
        }
        // distinguish which case is true (hit or not in)
        if (variables[possiblePosition] == pID)
        {
            // p is already inside the new row, so subtract current multiplicity
            coefficients[possiblePosition] -= Net::Mult[TR][POST][transition][p];
            // new coefficient may be 0 now
            if (coefficients[possiblePosition] == 0)
            {
                // erase possiblePosition entry (possiblePosition) in both array
                memmove(&variables[possiblePosition], &variables[possiblePosition + 1], (size - possiblePosition) * SIZEOF_INDEX_T);
                memmove(&coefficients[possiblePosition], &coefficients[possiblePosition + 1], (size - possiblePosition) * sizeof(int64_t));
                // assumption: decreasing 0 will lead to maxInt but
                //              upcoming increase will result in 0 again
                --size;
            }
        }
        else
        {
            // p is not in new row, so add it
            // may be it is necessary to insert in between existing entrys
            memmove(&variables[possiblePosition + 1], &variables[possiblePosition], (size - possiblePosition) * SIZEOF_INDEX_T);
            memmove(&coefficients[possiblePosition + 1], &coefficients[possiblePosition], (size - possiblePosition) * sizeof(int64_t));
            // store place index
            variables[possiblePosition] = pID;
            // store the multiplicity (from transition to p)
            // negative numbers
            coefficients[possiblePosition] = -Net::Mult[TR][POST][transition][p];
            // increase newSize
            ++size;
        }
    }
}

/// Calculates all signficant places and sorts them to the front of the place array
void Net::setSignificantPlaces()
{
    // arcs must be sorted
    assert(Net::DEBUG__checkArcOrdering());

    // save number of places
    const index_t cardPL = Net::Card[PL];

    if (cardPL < 2)
    {
        // nohing to do
        Place::CardSignificant = cardPL;
        return;
    }

    // request memory for one full row
    index_t* newVar = (index_t*) calloc(cardPL, SIZEOF_INDEX_T);
    int64_t* newCoef = (int64_t*) calloc(cardPL, sizeof(int64_t));
    index_t newSize;

    // create new matrix
    Matrix m(cardPL);

    // load rows into matrix
    // for each transition t
    for (index_t t = 0; t < Net::Card[TR]; ++t)
    {
        // create equation for current transition
        createTransitionEquation(t, newVar, newCoef, newSize);
        // save current arrays as new row
        m.addRow(newSize, newVar, newCoef);

        // clear used memory
        memset(newVar, 0, newSize * SIZEOF_INDEX_T);    // necessary?
        memset(newCoef, 0, newSize * sizeof(int64_t));
    }

    // free memory
    free(newVar);
    free(newCoef);

    // reduce matrix
    m.reduce();

    // gather significant places
    Place::CardSignificant = m.getSignificantColCount();
    index_t lastSignificant = cardPL - 1;
    index_t p = 0;
    while (p < Place::CardSignificant)
    {
        if (!m.isSignificant(p))
        {
            // p needs to be swapped
            // find first significant place from the right end of all places
            while (!m.isSignificant(lastSignificant))
            {
                lastSignificant--;
            }
            // swap lastSignificant with p
            Net::swapPlaces(p, lastSignificant--);
        }
        p++;
    }

    // adjust Place::SizeOfBitVector
    Place::SizeOfBitVector = 0;
    for (index_t i = 0; i < Place::CardSignificant; i++)
    {
        Place::SizeOfBitVector += Place::CardBits[i];
    }
}

/// Calculates the progress measure for all transitions
void Net::setProgressMeasure()
{
    // arcs must be sorted
    /// \todo: remove sortAllArcs() here?
    Net::sortAllArcs();
    assert(Net::DEBUG__checkArcOrdering());

    // save number of places
    const index_t cardPL = Net::Card[PL];
    // save number of transitions
    const index_t cardTR = Net::Card[TR];
    // save number of nodes
    const index_t cardNO = cardPL + cardTR;

    /// \todo: handle special cases (only a few places/transitions)

    // request memory for one full row
    index_t* newVar = (index_t*) calloc(cardPL + 1, SIZEOF_INDEX_T);
    int64_t* newCoef = (int64_t*) calloc(cardPL + 1 + cardTR, sizeof(int64_t));
    index_t newSize;

    // create new matrix
    Matrix m(cardNO);

    // load rows into matrix
    // for each transition t
    for (index_t t = 0; t < Net::Card[TR]; ++t)
    {
        // create equation for current transition
        createTransitionEquation(t, newVar, newCoef, newSize);

        // add entry for the identity matrix
        newVar[newSize] = t + cardPL;
        newCoef[newSize] = 1;
        ++newSize;

        // save current arrays as new row
        m.addRow(newSize, newVar, newCoef, t);

        // clear used memory
        memset(newVar, 0, newSize * SIZEOF_INDEX_T);    // necessary?
        memset(newCoef, 0, newSize * sizeof(int64_t));
    }

    // free memory
    free(newVar);
    free(newCoef);

    // reduce matrix
    assert(m.getRowCount() == cardTR);
    m.reduce();
    assert(m.getRowCount() == cardTR);

    // calculate progress measure
    Transition::ProgressMeasure = (int64_t*) calloc(cardTR, sizeof(int64_t));
    int64_t* denominatorValue = (int64_t*) calloc(cardTR, sizeof(int64_t));

    for (index_t t = 0; t < cardNO; ++t)
    {
        if (m.isSignificant(t))
        {
            // entry exists
            const Matrix::Row* curRow = m.getRow(t);
            const index_t curReference = m.getReference(t);

            assert(curReference < cardTR);

            if (curRow->variables[0] < cardPL)
            {
                // entry is linear independent
                Transition::ProgressMeasure[curReference] = 1;
                denominatorValue[curReference] = 1;
            }
            else
            {
                // entry is linear dependent
                for (index_t v = 0; v < curRow->varCount; ++v)
                {
                    if (curRow->variables[v] == curReference + cardPL)
                    {
                        // current variable is current transition (=reference)
                        assert(curRow->coefficients[v] != 0);
                        denominatorValue[curReference] = curRow->coefficients[v];
                    }
                    else
                    {
                        // current variable is any other transition
                        Transition::ProgressMeasure[curReference] -= curRow->coefficients[v];
                    }
                }
                // normalize current progress measure
                if (Transition::ProgressMeasure[curReference] != 0)
                {
                    assert(denominatorValue[curReference] != 0);
                    Transition::ProgressMeasure[curReference] /= denominatorValue[curReference];
                    denominatorValue[curReference] /= ggt(Transition::ProgressMeasure[curReference], denominatorValue[curReference]);
                    if (denominatorValue[curReference] < 0)
                    {
                        denominatorValue[curReference] *= -1;
                        Transition::ProgressMeasure[curReference] *= -1;
                    }
                }
            }
        }
    }

    // multiply all values with gcd
    int64_t gcd = 1;
    for (index_t t = 0; t < cardTR; ++t)
    {
        gcd = (gcd / ggt(gcd, denominatorValue[t]));
        gcd *= denominatorValue[t];
    }

    // save progress measures
    for (index_t p = 0; p < cardNO; ++p)
    {
        if (m.isSignificant(p))
        {
            const index_t curReference = m.getReference(p);
            Transition::ProgressMeasure[curReference] *= (gcd / denominatorValue[curReference]);
        }
    }

    // free memory
    free(denominatorValue);
}
