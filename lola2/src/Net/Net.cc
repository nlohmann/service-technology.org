/*
\file Net.cc
\author Karsten
		cast progress measure to int32 on 18.12.2012
\status approved 27.01.2012

\brief basic routines for handling nodes
*/

#include <cmdline.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <Net/LinearAlgebra.h>
#include <Net/Net.h>
#include <Net/Marking.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <Parser/ParserPTNet.h>
#include <InputOutput/Reporter.h>
#include <Parser/Symbol.h>

using std::set;
using std::map;
using std::vector;
using std::cout;
using std::endl;

extern gengetopt_args_info args_info;

index_t Net::Card[2] = {0, 0};
index_t *Net::CardArcs[2][2] = {{NULL, NULL}, {NULL, NULL}};
index_t **Net::Arc[2][2] = {{NULL, NULL}, {NULL, NULL}};
mult_t **Net::Mult[2][2] = {{NULL, NULL}, {NULL, NULL}};
const char **Net::Name[2] = {NULL, NULL};

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
        if (Net::CardArcs[TR][PRE][t])
        {
            curElem = Net::Arc[TR][PRE][t][0];
            for (index_t p = 1; p < Net::CardArcs[TR][PRE][t]; ++p)
            {
                assert(curElem < Net::Arc[TR][PRE][t][p]);
            }
        }
        if (Net::CardArcs[TR][POST][t])
        {
            curElem = Net::Arc[TR][POST][t][0];
            for (index_t p = 1; p < Net::CardArcs[TR][POST][t]; ++p)
            {
                assert(curElem < Net::Arc[TR][POST][t][p]);
            }
        }
    }
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        if (Net::CardArcs[PL][PRE][p])
        {
            curElem = Net::Arc[PL][PRE][p][0];
            for (index_t t = 1; t < Net::CardArcs[PL][PRE][p]; ++t)
            {
                assert(curElem < Net::Arc[PL][PRE][p][t]);
            }
        }
        if (Net::CardArcs[PL][POST][p])
        {
            curElem = Net::Arc[PL][POST][p][0];
            for (index_t t = 1; t < Net::CardArcs[PL][POST][p]; ++t)
            {
                assert(curElem < Net::Arc[PL][POST][p][t]);
            }
        }
    }
    return true;
}
// LCOV_EXCL_STOP

/// sorts array of arc (= node id) plus corresponding array of multiplicities
/// in the range of from to to (not including to)
void Net::sortArcs(index_t *arcs, mult_t *mults, const index_t from, const index_t to)
{
    if ((to - from) < 2)
    {
        return;    // less than 2 elements are always sorted
    }

    // points to first index where element is not known < pivot
    index_t blue = from;
    // points to first index where element is not know <= pivot
    index_t white = from + 1;
    // points to last index (+1) where element is not know to be  pivot
    index_t red = to;
    const index_t pivot = arcs[from];

    assert(from < to);

    while (red > white)
    {
        if (arcs[white] < pivot)
        {
            // swap white <-> blue
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

            // swap white <-> red
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

// note: this invalidates the disabled lists and scapegoats and is therefore only allowed BEFORE calling Transition::checkEnabled_initial
void Net::sortAllArcs()
{
    for (int type = PL; type <= TR; ++type)
    {
        for (int direction = PRE; direction <= POST; direction++)
        {
            for (index_t n = 0; n < Net::Card[type] ; n ++)
            {
                sortArcs(Net::Arc[type][direction][n], Net::Mult[type][direction][n], 0,
                         CardArcs[type][direction][n]);
            }
        }
    }
    assert(DEBUG__checkArcOrdering());
}

/// Free all allocated memory
/// \note memory is allocated in ParserPTNet.cc, ParserPTNet::symboltable2net()
void Net::deleteNodes()
{
    for (int type = PL; type <= TR; ++type)
    {
        for (int direction = PRE; direction <= POST; ++direction)
        {
            for (index_t i = 0; i < Net::Card[type]; i++)
            {
                delete[] Net::Arc[type][direction][i];
                delete[] Net::Mult[type][direction][i];
            }
            delete[] Net::CardArcs[type][direction];
            delete[] Net::Arc[type][direction];
            delete[] Net::Mult[type][direction];
        }
        for (index_t i = 0; i < Net::Card[type]; i++)
        {
            // names are allocated via strdup
            free(const_cast<char *>(Net::Name[type][i]));
        }
        delete[] Net::Name[type];
    }
}

void Net::print()
{
    printf("Net\n===\n\n");

    printf("%u places,  %u  transitions.\n\n", Net::Card[PL], Net::Card[TR]);

    for (index_t i = 0; i < Net::Card[PL]; i++)
    {
        printf("Place %u :%s, %u tokens hash %lld capacity %u bits %u\n", i, Net::Name[PL][i],
               Marking::Initial[i], Place::Hash[i], Place::Capacity[i], Place::CardBits[i]);
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

extern ParserPTNet *symbolTables;

void Net::swapPlaces(index_t left, index_t right)
{
    // 1. Net data structures

    int ixdLeft = symbolTables->PlaceTable->lookup(Net::Name[PL][left])->getIndex();
    symbolTables->PlaceTable->lookup(Net::Name[PL][left])->setIndex(symbolTables->PlaceTable->lookup(
                Net::Name[PL][right])->getIndex());
    symbolTables->PlaceTable->lookup(Net::Name[PL][right])->setIndex(ixdLeft);


    const char *tempname = Net::Name[PL][left];
    Net::Name[PL][left] = Net::Name[PL][right];
    Net::Name[PL][right] = tempname;



    assert(DEBUG__checkConsistency());

    for (int direction = PRE; direction <= POST; ++direction)
    {
        index_t tempindex = Net::CardArcs[PL][direction][left];
        Net::CardArcs[PL][direction][left] = Net::CardArcs[PL][direction][right];
        Net::CardArcs[PL][direction][right] = tempindex;

        index_t *tempindexpointer = Net::Arc[PL][direction][left];
        Net::Arc[PL][direction][left] = Net::Arc[PL][direction][right];
        Net::Arc[PL][direction][right] = tempindexpointer;

        mult_t *tempmultpointer = Net::Mult[PL][direction][left];
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

    // enabledness is not yet computed, so we can safely ignore this
    // in fact we must not do this, since the memory is not yet allocated
    /*  index_t tempcarddisabled = Place::CardDisabled[left];
        Place::CardDisabled[left] = Place::CardDisabled[right];
        Place::CardDisabled[right] = tempcarddisabled;

        index_t* tempdisabled = Place::Disabled[left];
        Place::Disabled[left] = Place::Disabled[right];
        Place::Disabled[right] = tempdisabled;*/

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

/// Creates an equation for the given transition (index) in the provided memory
void createTransitionEquation(index_t transition, index_t *variables, int64_t *coefficients,
                              index_t &size, bool dual = false)
{
    // check if we have transition or places rows
    index_t ND(dual ? PL : TR);

    // index in new row
    size = 0;
    // for each place p in the preset of t
    for (index_t p = 0; p < Net::CardArcs[ND][PRE][transition]; ++p)
    {
        // store place index and the it's multiplicity (from p to t)
        variables[size] = Net::Arc[ND][PRE][transition][p];
        // positive numbers
        assert(Net::Mult[ND][PRE][transition][p] != 0);
        coefficients[size] = Net::Mult[ND][PRE][transition][p];
        // increase newSize
        ++size;
    }
    // for each place p in the postset of t
    for (index_t p = 0; p < Net::CardArcs[ND][POST][transition]; ++p)
    {
        const index_t pID = Net::Arc[ND][POST][transition][p];
        assert(Net::Mult[ND][POST][transition][p] != 0);

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
            coefficients[possiblePosition] -= Net::Mult[ND][POST][transition][p];
            // new coefficient may be 0 now
            if (coefficients[possiblePosition] == 0)
            {
                // erase possiblePosition entry (possiblePosition) in both array
                memmove(&variables[possiblePosition], &variables[possiblePosition + 1],
                        (size - possiblePosition) * SIZEOF_INDEX_T);
                memmove(&coefficients[possiblePosition], &coefficients[possiblePosition + 1],
                        (size - possiblePosition) * SIZEOF_INT64_T);
                // assumption: decreasing 0 will lead to maxInt but
                //              upcoming increase will result in 0 again
                --size;
            }
        }
        else
        {
            // p is not in new row, so add it
            // may be it is necessary to insert in between existing entrys
            memmove(&variables[possiblePosition + 1], &variables[possiblePosition],
                    (size - possiblePosition) * SIZEOF_INDEX_T);
            memmove(&coefficients[possiblePosition + 1], &coefficients[possiblePosition],
                    (size - possiblePosition) * SIZEOF_INT64_T);
            // store place index
            variables[possiblePosition] = pID;
            // store the multiplicity (from transition to p)
            // negative numbers
            coefficients[possiblePosition] = -Net::Mult[ND][POST][transition][p];
            // increase newSize
            ++size;
        }
    }
}

Matrix Net::getIncidenceMatrix(node_t line_type)
{
    // the row type is the dual to the line type
    const node_t row_type = (line_type == PL) ? TR : PL;

    // cet the cardinalities of lines and rows (the dimensions of the matrix)
    const index_t line_card = Net::Card[line_type];
    const index_t row_card = Net::Card[row_type];

    // arcs must be sorted
    assert(Net::DEBUG__checkArcOrdering());

    // request memory for one full row
    index_t *newVar = new index_t[line_card]();
    int64_t *newCoef = new int64_t[line_card]();
    index_t newSize;

    // create new matrix
    Matrix m(line_card);

    // load rows into matrix
    // for each transition t
    for (index_t t = 0; t < row_card; ++t)
    {
        // create equation for current transition
        createTransitionEquation(t, newVar, newCoef, newSize, (line_type == TR));
        // save current arrays as new row
        m.addRow(newSize, newVar, newCoef);

        // clear used memory
        memset(newVar, 0, newSize * SIZEOF_INDEX_T);    // necessary?
        memset(newCoef, 0, newSize * SIZEOF_INT64_T);
    }

    // free memory
    delete[] newVar;
    delete[] newCoef;

    return m;
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

    // get incidence matrix (places are lines)
    Matrix m = getIncidenceMatrix(PL);

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
    assert(Net::DEBUG__checkArcOrdering());

    // get incidence matrix (places are lines)
    Matrix m = getIncidenceMatrix(TR);

    // reduce matrix
    m.diagonalise();

    // save number of transitions
    const index_t cardTR = Net::Card[TR];

    // calculate progress measure
    int64_t *progressMeasure = new int64_t[cardTR]();
    for (index_t t = 0; t < cardTR; ++t)
        if (m.isSignificant(t))
        {
            const Matrix::Row *curRow = m.getRow(t);
            progressMeasure[curRow->variables[0]] = curRow->coefficients[0];
            for (index_t v = 1; v < curRow->varCount; ++v)
                if (!m.isSignificant(curRow->variables[v]))
                {
                    progressMeasure[curRow->variables[v]] += curRow->coefficients[v];
                }
        }

    // try for a local optimisation (reduce number of transitions with negative progress)
    while (true)
    {
        index_t tindex(INDEX_T_MAX);
        int32_t changes(-1), ctmp;

        for (index_t t = 0; t < cardTR; ++t)
            if (m.isSignificant(t))
            {
                ctmp = 0;
                const Matrix::Row *curRow = m.getRow(t);
                for (index_t v = 0; v < curRow->varCount; ++v)
                    if (curRow->coefficients[v] < 0 && progressMeasure[curRow->variables[v]] < 0)
                    {
                        if (2 * curRow->coefficients[v] <= progressMeasure[curRow->variables[v]])
                        {
                            ++ctmp;
                        }
                    }
                    else if (curRow->coefficients[v] > 0 && progressMeasure[curRow->variables[v]] > 0)
                        if (2 * curRow->coefficients[v] > progressMeasure[curRow->variables[v]])
                        {
                            --ctmp;
                        }
                if (ctmp > changes)
                {
                    if (ctmp > 0 || curRow->coefficients[0] > 0)
                    {
                        changes = ctmp;
                        tindex = t;
                    }
                }
            }
        if (tindex == INDEX_T_MAX)
        {
            break;
        }

        const Matrix::Row *curRow = m.getRow(tindex);
        for (index_t v = 0; v < curRow->varCount; ++v)
        {
            progressMeasure[curRow->variables[v]] -= 2 * curRow->coefficients[v];
            curRow->coefficients[v] -= curRow->coefficients[v];
        }
    }

    if (args_info.sweeplinespread_arg > 1)
    {
        // try for another local optimisation (spread progress values better)
        // first, save the progress measures so far
        int64_t *progressCopy = new int64_t[cardTR];
        memcpy(progressCopy, progressMeasure, cardTR * SIZEOF_INT64_T);

        index_t threads(args_info.threads_arg), tries(cardTR), fullbucket(cardTR / threads + 1),
                maxbucket(cardTR);
        std::set<index_t> done;
        while (--tries > 0)
        {
            std::map<int64_t, index_t> pbuckets;
            index_t highbucket(1);
            for (index_t t = 0; t < cardTR; ++t)
                if (++pbuckets[progressMeasure[t]] > highbucket)
                {
                    ++highbucket;
                }
            if (fullbucket * fullbucket > cardTR)
            {
                --fullbucket;
            }
            if (highbucket <= maxbucket)
            {
                maxbucket = highbucket - 1;
            }
            if (maxbucket <= fullbucket)
            {
                break;
            }
            std::set<int64_t> tryvalues;
            for (std::map<int64_t, index_t>::iterator it = pbuckets.begin(); it != pbuckets.end(); ++it)
                if (it->second > maxbucket)
                {
                    tryvalues.insert(it->first);
                }
            /*
            		cout << "progress values with too many transitions(" << maxbucket << "," << fullbucket << "): ";
            		for(std::set<int64_t>::iterator it=tryvalues.begin(); it!=tryvalues.end(); ++it)
            			cout << *it << "(" << pbuckets[*it] << ") ";
            		cout << endl;
            */
            index_t t;
            for (t = 0; t < cardTR; ++t)
                if (m.isSignificant(t))
                {
                    if (done.find(t) != done.end())
                    {
                        continue;
                    }
                    const Matrix::Row *curRow = m.getRow(t);
                    if (tryvalues.find(curRow->coefficients[0]) == tryvalues.end())
                    {
                        continue;
                    }

                    int64_t mult;
                    for (mult = 2; mult <= args_info.sweeplinespread_arg; ++mult)
                    {
                        int32_t ctmp = 0;
                        for (index_t v = 0; v < curRow->varCount; ++v)
                            if (curRow->coefficients[v] > 0 && progressMeasure[curRow->variables[v]] < 0)
                            {
                                if (progressMeasure[curRow->variables[v]] + (mult - 1)*curRow->coefficients[v] >= 0)
                                {
                                    ++ctmp;
                                }
                            }
                            else if (curRow->coefficients[v] < 0 && progressMeasure[curRow->variables[v]] > 0)
                                if (progressMeasure[curRow->variables[v]] + (mult - 1)*curRow->coefficients[v] < 0)
                                {
                                    --ctmp;
                                }
                        if (ctmp < 0)
                        {
                            continue;
                        }

                        int64_t toofull(0);
                        for (index_t v = 0; v < curRow->varCount; ++v)
                        {
                            if (pbuckets[progressMeasure[curRow->variables[v]]] > fullbucket)
                            {
                                --toofull;
                            }
                            if (pbuckets[progressMeasure[curRow->variables[v]] + (mult - 1)*curRow->coefficients[v]] >
                                    fullbucket)
                            {
                                ++toofull;
                            }
                        }
                        if (toofull >= 0)
                        {
                            continue;
                        }

                        //					cout << "row t=" << Net::Name[TR][t] << " mult=" << mult << endl;
                        done.insert(t);

                        for (index_t v = 0; v < curRow->varCount; ++v)
                        {
                            progressMeasure[curRow->variables[v]] += (mult - 1) * curRow->coefficients[v];
                            curRow->coefficients[v] *= mult;
                        }

                        break;
                    }
                    if (mult <= args_info.sweeplinespread_arg)
                    {
                        break;
                    }
                }
            if (t == cardTR)
            {
                ++fullbucket;
            }
        }

        // check if the optimisation uses the buckets in a better way (especially more buckets)
        std::set<int64_t> oldcnt, newcnt;
        for (index_t t = 0; t < cardTR; ++t)
        {
            oldcnt.insert(progressCopy[t]);
            newcnt.insert(progressMeasure[t]);
        }
        if (newcnt.size() <= oldcnt.size())
        {
            memcpy(progressMeasure, progressCopy, cardTR * SIZEOF_INT64_T);
        }
        else
        {
            float oldrange, newrange, oldsize, newsize;
            oldrange = (float)(*(oldcnt.rbegin()) - * (oldcnt.begin()) + 1);
            newrange = (float)(*(newcnt.rbegin()) - * (newcnt.begin()) + 1);
            oldsize = (float)(oldcnt.size());
            newsize = (float)(newcnt.size());
            //cout << "oldrange=" << oldrange << " newrange=" << newrange << " oldsize=" << oldsize << " newsize=" << newsize << endl;
            if (newrange / oldrange >= newsize * newsize / (oldsize * oldsize))
            {
                memcpy(progressMeasure, progressCopy, cardTR * SIZEOF_INT64_T);
            }
            else
            {
                cout << "progress adapted" << endl;
            }
        }

        delete[] progressCopy;
    }

    // remove gcd from progress values
    int64_t gcd(0);
    for (index_t t = 0; t < cardTR; ++t)
        if (gcd == 0)
        {
            gcd = progressMeasure[t];
        }
        else if (progressMeasure[t] != 0)
        {
            gcd = ggt(gcd, progressMeasure[t]);
        }
    if (gcd < 0)
    {
        gcd = -gcd;
    }
    if (gcd != 0)
        for (index_t t = 0; t < cardTR; ++t)
        {
            progressMeasure[t] /= gcd;
        }

    //    for(index_t t=0; t<cardTR; ++t)
    //        rep->status("progress[%s]=%lld", Net::Name[TR][t], progressMeasure[t]);

    // cast progress measures to 32bit
    Transition::ProgressMeasure = new int32_t[cardTR]();
    for (index_t t = 0; t < cardTR; ++t)
    {
        Transition::ProgressMeasure[t] = (int32_t) progressMeasure[t];
    }

    // free memory
    delete[] progressMeasure;
}

// calculates DeltaT and DeltaHash for each transition
void Net::preprocess_organizeDeltas()
{
    const index_t cardPL = Net::Card[PL];
    const index_t cardTR = Net::Card[TR];

    // temporarily collect places where a transition has negative token balance
    index_t *delta_pre = new index_t[cardPL]();
    // temporarily collect places where a transition has positive token balance.
    index_t *delta_post = new index_t[cardPL]();

    // same for multiplicities
    mult_t *mult_pre = new mult_t[cardPL];
    // same for multiplicities
    mult_t *mult_post = new mult_t[cardPL];

    for (index_t t = 0; t < cardTR; t++)
    {
        // initialize DeltaT structures
        index_t card_delta_pre = 0;
        index_t card_delta_post = 0;
        Net::sortArcs(Net::Arc[TR][PRE][t], Net::Mult[TR][PRE][t], 0, Net::CardArcs[TR][PRE][t]);
        Net::sortArcs(Net::Arc[TR][POST][t], Net::Mult[TR][POST][t], 0, Net::CardArcs[TR][POST][t]);

        // parallel iteration through sorted pre and post arc sets
        index_t i, j;
        for (i = 0, j = 0; (i < Net::CardArcs[TR][PRE][t])
                && (j < Net::CardArcs[TR][POST][t]); /* tricky increment */)
        {
            if (Net::Arc[TR][PRE][t][i] == Net::Arc[TR][POST][t][j])
            {
                // double arc, compare multiplicities
                if (Net::Mult[TR][PRE][t][i] == Net::Mult[TR][POST][t][j])
                {
                    // test arc, does not contribute to delta t
                }
                else
                {
                    if (Net::Mult[TR][PRE][t][i] < Net::Mult[TR][POST][t][j])
                    {
                        // positive impact --> goes to delta post
                        delta_post[card_delta_post] = Net::Arc[TR][POST][t][j];
                        mult_post[card_delta_post++] = (mult_t)(Net::Mult[TR][POST][t][j] - Net::Mult[TR][PRE][t][i]);
                    }
                    else
                    {
                        // negative impact --> goes to delta pre
                        delta_pre[card_delta_pre] = Net::Arc[TR][PRE][t][i];
                        mult_pre[card_delta_pre++] = (mult_t)(Net::Mult[TR][PRE][t][i] - Net::Mult[TR][POST][t][j]);
                    }
                }
                ++i;
                ++j;
            }
            else
            {
                if (Net::Arc[TR][PRE][t][i] < Net::Arc[TR][POST][t][j])
                {
                    // single arc goes to PRE
                    delta_pre[card_delta_pre] = Net::Arc[TR][PRE][t][i];
                    mult_pre[card_delta_pre++] = Net::Mult[TR][PRE][t][i++];
                }
                else
                {
                    // single arc goes to POST
                    delta_post[card_delta_post] = Net::Arc[TR][POST][t][j];
                    mult_post[card_delta_post++] = Net::Mult[TR][POST][t][j++];
                }
            }
        }

        // empty nonempty lists
        for (; i < Net::CardArcs[TR][PRE][t]; i++)
        {
            // single arc goes to PRE
            delta_pre[card_delta_pre] = Net::Arc[TR][PRE][t][i];
            mult_pre[card_delta_pre++] = Net::Mult[TR][PRE][t][i];
        }
        for (; j < Net::CardArcs[TR][POST][t]; j++)
        {
            // single arc goes to POST
            delta_post[card_delta_post] = Net::Arc[TR][POST][t][j];
            mult_post[card_delta_post++] = Net::Mult[TR][POST][t][j];
        }


        /*********************
        * 7a. Copy Deltas *
        **********************/

        // allocate memory for deltas
        Transition::CardDeltaT[PRE][t] = card_delta_pre;
        Transition::CardDeltaT[POST][t] = card_delta_post;
        Transition::DeltaT[PRE][t] = new index_t[card_delta_pre];
        Transition::DeltaT[POST][t] = new index_t[card_delta_post];
        Transition::MultDeltaT[PRE][t] = new mult_t[card_delta_pre];
        Transition::MultDeltaT[POST][t] = new mult_t[card_delta_post];

        // copy information on deltas
        memcpy(Transition::DeltaT[PRE][t], delta_pre, card_delta_pre * SIZEOF_INDEX_T);
        memcpy(Transition::MultDeltaT[PRE][t], mult_pre, card_delta_pre * SIZEOF_MULT_T);
        memcpy(Transition::DeltaT[POST][t], delta_post, card_delta_post * SIZEOF_INDEX_T);
        memcpy(Transition::MultDeltaT[POST][t], mult_post, card_delta_post * SIZEOF_MULT_T);
    }

    delete[] delta_pre;
    delete[] delta_post;
    delete[] mult_pre;
    delete[] mult_post;

    /*********************
    * 7b. Set DeltaHash *
    **********************/

    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
        for (index_t i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
        {
            Transition::DeltaHash[t] = (Transition::DeltaHash[t] - Transition::MultDeltaT[PRE][t][i] *
                                        Place::Hash[Transition::DeltaT[PRE][t][i]]) % SIZEOF_MARKINGTABLE;
        }
        for (index_t i = 0; i < Transition::CardDeltaT[POST][t]; i++)
        {
            Transition::DeltaHash[t] = (Transition::DeltaHash[t] + Transition::MultDeltaT[POST][t][i] *
                                        Place::Hash[Transition::DeltaT[POST][t][i]]) % SIZEOF_MARKINGTABLE;
        }
    }
}


/// auxiliary comparator object needed for efficient conflict set caching
struct conflictset_comparator
{
    conflictset_comparator(index_t _len): len(_len) {};
    index_t len;
    bool operator () (index_t *const &a, index_t *const &b) const
    {
        return (memcmp(a, b, len * SIZEOF_INDEX_T) < 0);
    }
};


// old version without much magic improvements. Can be deleted once the new version (below) proves to work.
#if 0
void Net::preprocess_organizeConflictingTransitions()
{

    const index_t cardPL = Net::Card[PL];
    const index_t cardTR = Net::Card[TR];

    // initialize Conflicting arrays
    index_t *conflicting = new index_t[cardTR]();
    index_t *new_conflicting = new index_t[cardTR]();

    // initialize conflict cache array. There is a set for every possible size of the conflict set.
    std::set<index_t *, conflictset_comparator> **conflictcache = new
    std::set<index_t *, conflictset_comparator> *[cardTR + 1];
    for (index_t i = 0; i <= cardTR; i++)
    {
        conflictcache[i] = new std::set<index_t *, conflictset_comparator>(conflictset_comparator(i));
    }
    index_t num_different_conflicts = 0;
    // iterator used to temporarily store the result of a find() operation. It will point either to the cache element or the end() of the conflictcache
    std::set<index_t *, conflictset_comparator>::iterator it;

    ///\todo make search for conflicting transitions a function to avoid code duplication
    for (index_t t = 0; t < cardTR; t++)
    {
        if (t > 0 and t % 10000 == 0)
        {
            rep->status("processed %d of %d transitions (%d conflict sets found so far)", t, cardTR,
                        num_different_conflicts);
        }

        // 8.1 conflicting transitions
        index_t card_conflicting = 0;

        /// 1. collect all conflicting transitions \f$(\null^\bullet t)^\bullet\f$
        for (index_t i = 0; i < Net::CardArcs[TR][PRE][t]; i++)
        {
            // p is a pre-place
            const index_t p = Net::Arc[TR][PRE][t][i];
            card_conflicting = std::set_union(conflicting, conflicting + card_conflicting,
                                              Net::Arc[PL][POST][p], Net::Arc[PL][POST][p] + Net::CardArcs[PL][POST][p],
                                              new_conflicting) - new_conflicting;
            std::swap(new_conflicting, conflicting);
        }

        Transition::CardConflicting[t] = card_conflicting;
        // try to find conflict set in cache
        if (conflictcache[card_conflicting]->end() != (it = conflictcache[card_conflicting]->find(
                    conflicting)))
        {
            // success! use cached set
            Transition::Conflicting[t] = *it;
            Transition::ConflictingIsOriginal[t] = false;
        }
        else
        {
            // failure! allocate memory for new conflict set and add it to the cache
            Transition::Conflicting[t] = new index_t[card_conflicting];
            memcpy(Transition::Conflicting[t], conflicting, card_conflicting * SIZEOF_INDEX_T);
            conflictcache[card_conflicting]->insert(Transition::Conflicting[t]);
            Transition::ConflictingIsOriginal[t] = true;
            num_different_conflicts++;
        }



        // 8.2 backward conflicting transitions
        card_conflicting = 0;

        /// 1. collect all backward conflicting transitions \f$(t^\bullet)^\bullet\f$
        for (index_t i = 0; i < Net::CardArcs[TR][POST][t]; i++)
        {
            // p is a post-place
            const index_t p = Net::Arc[TR][POST][t][i];
            card_conflicting = std::set_union(conflicting, conflicting + card_conflicting,
                                              Net::Arc[PL][POST][p], Net::Arc[PL][POST][p] + Net::CardArcs[PL][POST][p],
                                              new_conflicting) - new_conflicting;
            std::swap(new_conflicting, conflicting);
        }

        Transition::CardBackConflicting[t] = card_conflicting;
        // try to find conflict set in cache
        if (conflictcache[card_conflicting]->end() != (it = conflictcache[card_conflicting]->find(
                    conflicting)))
        {
            // success! use cached set
            Transition::BackConflicting[t] = *it;
            Transition::BackConflictingIsOriginal[t] = false;
        }
        else
        {
            // failure! allocate memory for new conflict set and add it to the cache
            Transition::BackConflicting[t] = new index_t[card_conflicting];
            memcpy(Transition::BackConflicting[t], conflicting, card_conflicting * SIZEOF_INDEX_T);
            conflictcache[card_conflicting]->insert(Transition::BackConflicting[t]);
            Transition::BackConflictingIsOriginal[t] = true;
            num_different_conflicts++;
        }
    }

    free(conflicting);
    free(new_conflicting);
    for (index_t i = 0; i <= cardTR; i++)
    {
        delete conflictcache[i];
    }
    delete conflictcache;


    rep->status("%d transition conflict sets", num_different_conflicts);
}
#endif


// moves all elements in the range [first1,last1), that are also in [first2,last2), to result.
// returns the number of elements moved.
index_t Net::set_moveall(index_t *first1, index_t *last1,
                         index_t *first2, index_t *last2,
                         index_t *result)
{
    index_t *res = result;
    index_t *retain = first1;
    index_t *lb = first1;
    while (first1 != last1 && first2 != last2)
    {
        lb = std::lower_bound(lb, last1, *first2);
        if (lb == last1)
        {
            break;
        }
        if (*lb == *first2++)
        {
            memmove(retain, first1, (lb - first1)*SIZEOF_INDEX_T);
            retain += lb - first1;
            *res++ = *lb++;
            first1 = lb;
        }
    }
    memmove(retain, first1, (last1 - first1)*SIZEOF_INDEX_T);
    return res - result;
}

/// calculates the set of conflicting transitions for each transition
void Net::preprocess_organizeConflictingTransitions()
{
    const index_t cardPL = Net::Card[PL];
    const index_t cardTR = Net::Card[TR];

    ////
    // allocate a bunch of temporary arrays
    ////

    // stackpos_place_done[p] states whether place p is already included in the current conflict set (the one at the current stack position)
    bool *stackpos_place_done = new bool[cardPL]();
    // stack_place_used[i] states which place has been processed at stack position i. Needed to keep stackpos_place_done updated.
    index_t *stack_place_used = new index_t[cardPL];

    // stack_conflictset[i] stores the current conflict set at stack position i
    // calloc: null-pointer tests to dynamically allocate new segments
    index_t **stack_conflictset = new index_t *[(cardPL + 1)]();
    index_t *stack_card_conflictset = new index_t[(cardPL + 1)]();

    // stack_transitions[i] stores all transitions the conflict set at stack position i applies to. Every transition appears exactly once.
    // calloc: null-pointer tests to dynamically allocate new segments
    index_t **stack_transitions = new index_t *[(cardPL + 1)]();
    index_t *stack_card_transitions = new index_t[(cardPL + 1)]();
    // index of the current transition for each stack position
    index_t *stack_transitions_index = new index_t[(cardPL + 1)]();

    // temporary array needed to do pseudo-"in-place" operations.
    index_t *tmp_array = new index_t[cardTR]();

    // initialize conflict cache array. There is a set for every possible size of the conflict set.
    std::set<index_t *, conflictset_comparator> **conflictcache = new
    std::set<index_t *, conflictset_comparator> *[cardTR + 1];
    for (index_t i = 0; i <= cardTR; i++)
    {
        conflictcache[i] = new std::set<index_t *, conflictset_comparator>(conflictset_comparator(i));
    }
    index_t num_different_conflicts = 0;
    // iterator used to temporarily store the result of a find() operation. It will point either to the cache element or the end() of the conflictcache
    std::set<index_t *, conflictset_comparator>::iterator it;

    ////
    // compute (forward-)conflicting sets
    ////

    // only for status output
    index_t num_finished_transitions = 0;

    /// init stack
    stack_conflictset[0] = new index_t[cardTR]();
    stack_transitions[0] = new index_t[cardTR]();
    index_t stack_index = 0;
    stack_card_transitions[0] = cardTR;

    // all transitions start at stack position 0
    for (index_t i = 0; i < cardTR; i++)
    {
        stack_transitions[0][i] = i;
    }
    while (true)
    {
        /// check if there are still transitions left the current stack position
        if (stack_transitions_index[stack_index] >= stack_card_transitions[stack_index])
        {
            /// all transitions are done, pop current position from stack
            // reset stack position
            stack_transitions_index[stack_index] = 0;
            // check if already at bottom of stack
            if (stack_index <= 0)
            {
                break;
            }
            stack_index--;
            stackpos_place_done[stack_place_used[stack_index]] = false;
            continue;
        }

        /// grab a transition
        index_t active_transition = stack_transitions[stack_index][stack_transitions_index[stack_index]];

        /// iterate over all its unprocessed pre-places p
        for (index_t i = 0; i < Net::CardArcs[TR][PRE][active_transition]; i++)
        {
            const index_t p = Net::Arc[TR][PRE][active_transition][i];
            if (stackpos_place_done[p])
            {
                continue;
            }

            // allocate next stack segment if not already done
            if (!stack_conflictset[stack_index + 1])
            {
                stack_conflictset[stack_index + 1] = new index_t[cardTR]();
                stack_transitions[stack_index + 1] = new index_t[cardTR]();
            }

            // compute new conflict set
            stack_card_conflictset[stack_index + 1] = std::set_union(stack_conflictset[stack_index],
                    stack_conflictset[stack_index] + stack_card_conflictset[stack_index], Net::Arc[PL][POST][p],
                    Net::Arc[PL][POST][p] + Net::CardArcs[PL][POST][p],
                    stack_conflictset[stack_index + 1]) - stack_conflictset[stack_index + 1];

            // compute all remaining transitions at the new stack position
            stack_card_transitions[stack_index + 1] = set_moveall(stack_transitions[stack_index],
                    stack_transitions[stack_index] + stack_card_transitions[stack_index], Net::Arc[PL][POST][p],
                    Net::Arc[PL][POST][p] + Net::CardArcs[PL][POST][p], stack_transitions[stack_index + 1]);
            stack_card_transitions[stack_index] -= stack_card_transitions[stack_index + 1];

            // mark p as used
            stack_place_used[stack_index] = p;
            stackpos_place_done[p] = true;

            // go to new stack position
            stack_index++;
        }

        // status output
        if (++num_finished_transitions % 10000 == 0)
        {
            rep->status("processed %d of %d transitions, pass 1/2 (%d conflict sets)", num_finished_transitions,
                        cardTR, num_different_conflicts);
        }

        // all pre-places of active_transition are now done, the current stack position holds the resulting conflict set
        Transition::CardConflicting[active_transition] = stack_card_conflictset[stack_index];
        // try to find conflict set in cache
        if (conflictcache[stack_card_conflictset[stack_index]]->end() != (it =
                    conflictcache[stack_card_conflictset[stack_index]]->find(stack_conflictset[stack_index])))
        {
            // success! use cached set
            Transition::Conflicting[active_transition] = *it;
            Transition::ConflictingIsOriginal[active_transition] = false;
        }
        else
        {
            // failure! allocate memory for new conflict set and add it to the cache
            Transition::Conflicting[active_transition] = new index_t[stack_card_conflictset[stack_index]];
            memcpy(Transition::Conflicting[active_transition], stack_conflictset[stack_index],
                   stack_card_conflictset[stack_index] * SIZEOF_INDEX_T);
            conflictcache[stack_card_conflictset[stack_index]]->insert(
                Transition::Conflicting[active_transition]);
            Transition::ConflictingIsOriginal[active_transition] = true;
            num_different_conflicts++;
        }
        // move on to next transition at current stack position
        stack_transitions_index[stack_index]++;
    }

    ////
    // compute back-conflicting sets
    ////

    num_finished_transitions = 0;

    // re-init stack
    stack_index = 0;
    stack_card_transitions[0] = cardTR;
    for (index_t i = 0; i < cardTR; i++)
    {
        stack_transitions[0][i] = i;
    }
    while (true)
    {
        /// check if there are still transitions left the current stack position
        if (stack_transitions_index[stack_index] >= stack_card_transitions[stack_index])
        {
            /// all transitions are done, pop current position from stack
            // reset stack position
            stack_transitions_index[stack_index] = 0;
            // check if already at bottom of stack
            if (stack_index <= 0)
            {
                break;
            }
            stack_index--;
            stackpos_place_done[stack_place_used[stack_index]] = false;
            continue;
        }

        /// grab a transition
        index_t active_transition = stack_transitions[stack_index][stack_transitions_index[stack_index]];

        /// iterate over all its unprocessed post-places p
        for (index_t i = 0; i < Net::CardArcs[TR][POST][active_transition]; i++)
        {
            const index_t p = Net::Arc[TR][POST][active_transition][i];
            if (stackpos_place_done[p])
            {
                continue;
            }

            // allocate next stack segment if not already done
            if (!stack_conflictset[stack_index + 1])
            {
                stack_conflictset[stack_index + 1] = new index_t[cardTR]();
                stack_transitions[stack_index + 1] = new index_t[cardTR]();
            }

            // compute new conflict set
            stack_card_conflictset[stack_index + 1] = std::set_union(stack_conflictset[stack_index],
                    stack_conflictset[stack_index] + stack_card_conflictset[stack_index], Net::Arc[PL][POST][p],
                    Net::Arc[PL][POST][p] + Net::CardArcs[PL][POST][p],
                    stack_conflictset[stack_index + 1]) - stack_conflictset[stack_index + 1];

            // compute all remaining transitions at the new stack position
            stack_card_transitions[stack_index + 1] = set_moveall(stack_transitions[stack_index],
                    stack_transitions[stack_index] + stack_card_transitions[stack_index], Net::Arc[PL][PRE][p],
                    Net::Arc[PL][PRE][p] + Net::CardArcs[PL][PRE][p], stack_transitions[stack_index + 1]);
            stack_card_transitions[stack_index] -= stack_card_transitions[stack_index + 1];

            // mark p as used
            stack_place_used[stack_index] = p;
            stackpos_place_done[p] = true;

            // go go new stack position
            stack_index++;
        }

        // status output
        if (++num_finished_transitions % 10000 == 0)
        {
            rep->status("processed %d of %d transitions, pass 2/2 (%d conflict sets)", num_finished_transitions,
                        cardTR, num_different_conflicts);
        }

        // all post-places of active_transition are now done, the current stack position holds the resulting conflict set
        Transition::CardBackConflicting[active_transition] = stack_card_conflictset[stack_index];
        // try to find conflict set in cache
        if (conflictcache[stack_card_conflictset[stack_index]]->end() != (it =
                    conflictcache[stack_card_conflictset[stack_index]]->find(stack_conflictset[stack_index])))
        {
            // success! use cached set
            Transition::BackConflicting[active_transition] = *it;
            Transition::BackConflictingIsOriginal[active_transition] = false;
        }
        else
        {
            // failure! allocate memory for new conflict set and add it to the cache
            Transition::BackConflicting[active_transition] = new index_t[stack_card_conflictset[stack_index]];
            memcpy(Transition::BackConflicting[active_transition], stack_conflictset[stack_index],
                   stack_card_conflictset[stack_index] * SIZEOF_INDEX_T);
            conflictcache[stack_card_conflictset[stack_index]]->insert(
                Transition::BackConflicting[active_transition]);
            Transition::BackConflictingIsOriginal[active_transition] = true;
            num_different_conflicts++;
        }
        // move on to next transition at current stack position
        stack_transitions_index[stack_index]++;
    }

    ////
    // cleanup: free temporary arrays
    ////

    delete[] tmp_array;
    for (index_t i = 0; i <= cardTR; i++)
    {
        delete conflictcache[i];
    }
    delete[] conflictcache;
    for (index_t i = 0; i <= cardPL; i++)
    {
        delete[] stack_conflictset[i];
    }
    delete[] stack_conflictset;
    delete[] stack_card_conflictset;
    for (index_t i = 0; i <= cardPL; i++)
    {
        delete[] stack_transitions[i];
    }
    delete[] stack_transitions;
    delete[] stack_card_transitions;
    delete[] stack_transitions_index;
    delete[] stackpos_place_done;
    delete[] stack_place_used;

    rep->status("%d transition conflict sets", num_different_conflicts);
}

/// assumes that raw net is read and places, transitions and the edges in-between are set properly. Computes additional net information used to speed up the simulation.
void Net::preprocess()
{
    const index_t cardPL = Net::Card[PL];
    const index_t cardTR = Net::Card[TR];

    /************************************
    * 1. Compute bits needed for places *
    ************************************/
    Place::CardBits = new cardbit_t[cardPL];
    Place::SizeOfBitVector = 0;
    for (index_t p = 0; p < cardPL; p++)
    {
        Place::SizeOfBitVector +=
            (Place::CardBits[p] = Place::Capacity2Bits(Place::Capacity[p]));
    }

    /********************
    * 2. Compute Hashes *
    ********************/
    Place::Hash = new hash_t[cardPL];
    Marking::HashInitial = 0;
    for (index_t p = 0; p < cardPL; p++)
    {
        Place::Hash[p] = rand() % MAX_HASH;
        Marking::HashInitial = (Marking::HashInitial + Place::Hash[p] * Marking::Initial[p]) %
                               SIZEOF_MARKINGTABLE;
    }
    // set hash value for initial marking
    Marking::HashCurrent = Marking::HashInitial;

    /*********************
    * 3. Organize Deltas *
    *********************/
    // calloc: delta hash must be initially 0
    Transition::DeltaHash = new hash_t[cardTR]();
    // allocate memory for deltas
    for (int direction = PRE; direction <= POST; direction++)
    {
        // calloc: no arcs there yet
        Transition::CardDeltaT[direction] = new index_t[cardTR]();
        Transition::DeltaT[direction] = new index_t *[cardTR];
        Transition::MultDeltaT[direction] = new mult_t *[cardTR];
    }
    Net::preprocess_organizeDeltas();

    /**************************************
    * 4. Organize conflicting transitions *
    **************************************/
    Transition::CardConflicting = new index_t[cardTR];
    Transition::Conflicting = new index_t *[cardTR];
    Transition::ConflictingIsOriginal = new bool[cardTR];
    Transition::CardBackConflicting = new index_t[cardTR];
    Transition::BackConflicting = new index_t *[cardTR];
    Transition::BackConflictingIsOriginal = new bool[cardTR];

    Net::preprocess_organizeConflictingTransitions();

    /****************************
    * 5. Set significant places *
    *****************************/
    // test whether computation actually needed
    if (Place::CardSignificant == -1)
    {
        Net::setSignificantPlaces();
    }
    rep->status("%d places, %d transitions, %d significant places",
                Net::Card[PL], Net::Card[TR], Place::CardSignificant);

    // sort all arcs. Needs to be done before enabledness check in order to not mess up the disabled lists and scapegoats, but after determining the significant places since it swaps places and destroys any arc ordering.
    Net::sortAllArcs();

    /**************************
    * 6. Set progress measure *
    **************************/
    // we always calcuate a progress measure as it is reasonably cheap and beside the sweep line algorithm, also the deletion algorithm uses it
    if (args_info.search_arg == search_arg_sweepline or args_info.stubborn_arg == stubborn_arg_deletion)
    {
        rep->status("calculating the progress measure");
        Net::setProgressMeasure();
    }

    /*******************************
    * 7. Initial enabledness check *
    *******************************/
    // use calloc: initial assumption: no transition is disabled
    Place::CardDisabled = new index_t[cardPL]();
    Place::Disabled = new index_t *[cardPL];
    for (index_t p = 0; p < cardPL; p++)
    {
        // initially: no disabled transistions (through CardDisabled = 0)
        // correct values will be achieved by initial checkEnabled...
        Place::Disabled[p] = new index_t[Net::CardArcs[PL][POST][p]];
    }
    Transition::PositionScapegoat = new index_t[cardTR];
    Transition::Enabled = new bool[cardTR];
    // start with assumption that all transitions are enabled
    Transition::CardEnabled = cardTR;
    for (index_t t = 0; t < cardTR; t++)
    {
        Transition::Enabled[t] = true;
    }

    for (index_t t = 0; t < cardTR; t++)
    {
        Transition::checkEnabled_Initial(t);
    }
}
