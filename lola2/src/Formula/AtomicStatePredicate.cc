/*!
\file AtomicStatePredicate.cc
\author Karsten
\status new

\brief class implementation for atomic state predicates
*/

#include <config.h>
#include <Formula/AtomicStatePredicate.h>
#include <Net/Net.h>
#include <Net/Marking.h>
#include <cstdlib>
#include <cstdio>

/*!
creates a state predicate with a formal sum of #p places with positive factor,
#n places with negative factor, and constant #k particular places are added
using addPos and addNeg

\param p  number of places with positive factor
\param n  number of places with negative factor
\param k  constant

\todo Schleifen behandeln - können evtl. rausgenommen werden
*/
AtomicStatePredicate::AtomicStatePredicate(index_t p, index_t n, int k)
{
    //printf("+ %p->AtomicStatePredicate(p=%d, n=%d, k=%d)\n", this, p, n, k);

    parent = NULL;
    posPlaces = (index_t*) malloc(p * SIZEOF_INDEX_T);
    negPlaces = (index_t*) malloc(n * SIZEOF_INDEX_T);
    posMult = (capacity_t*) malloc(p * SIZEOF_CAPACITY_T);
    negMult = (capacity_t*) malloc(n * SIZEOF_CAPACITY_T);
    cardPos = p;
    cardNeg = n;
    threshold = k;
}

AtomicStatePredicate::~AtomicStatePredicate()
{
    free(posPlaces);
    free(negPlaces);
    free(posMult);
    free(negMult);
}

void AtomicStatePredicate::addPos(index_t i, index_t p, capacity_t m)
{
    //printf("+ %p->addPos(i=%d, p=%d, m=%d)\n", this, i, p, m);

    assert(i < cardPos);
    posPlaces[i] = p;
    posMult[i] = m;
}

void AtomicStatePredicate::addNeg(index_t i, index_t p, capacity_t m)
{
    //printf("+ %p->addNeg(i=%d, p=%d, m=%d)\n", this, i, p, m);

    assert(i < cardNeg);
    negPlaces[i] = p;
    negMult[i] = m;
}

index_t AtomicStatePredicate::getUpSet(index_t* stack, bool* onstack)
{
    assert(onstack);
    index_t stackpointer = 0;
    for (index_t i = 0; i < cardUp; i++)
    {
        index_t element;
        if (!onstack[element = up[i]])
        {
            onstack[element] = true;
            stack[stackpointer++] = element;
        }
    }
    return stackpointer;
}


void AtomicStatePredicate::update(int delta)
{
    sum += delta;
    if (sum <= threshold && !value)
    {
        value = true;
        if (parent)
        {
            parent -> updateFT(position);
        }
        return;
    }
    if (sum > threshold && value)
    {
        value = false;
        if (parent)
        {
            parent -> updateTF(position);
        }
        return;
    }
}

void AtomicStatePredicate::evaluate()
{
    sum = 0;
    for (index_t i = 0; i < cardPos; ++i)
    {
        sum += Marking::Current[posPlaces[i]] * posMult[i];
    }
    for (index_t i = 0; i < cardNeg; ++i)
    {
        sum -= Marking::Current[negPlaces[i]] * negMult[i];
    }
    value = (sum <= threshold);
}

index_t AtomicStatePredicate::countAtomic()
{
    return 1;
}

index_t AtomicStatePredicate::collectAtomic(AtomicStatePredicate** c)
{
    c[0] = this;
    return 1;
}

void AtomicStatePredicate::setUpSet()
{
    index_t* up1 = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    index_t* up2 = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    cardUp = 0;

    // idea: for each p: merge up1 and post places of pos / pre places of neg into up2
    for (index_t i = 0; i < cardPos; i++)
    {
        const index_t p = posPlaces[i];
        const index_t cardP = Net::CardArcs[PL][POST][p];

        index_t a = 0; // index in up1
        index_t b = 0; // index in arc list of place
        index_t c = 0; // index in up2 (result)

        while (a < cardUp && b < cardP)
        {
            if (up1[a] < Net::Arc[PL][POST][p][b])
            {
                up2[c++] = up1[a++];
                continue;
            }
            if (up1[a] > Net::Arc[PL][POST][p][b])
            {
                up2[c++] = Net::Arc[PL][POST][p][b++];
                continue;
            }
            assert(up1[a] == Net::Arc[PL][POST][p][b]);
            up2[c++] = up1[a++];
            ++b;
        }
        for (; a < cardUp; a++)
        {
            up2[c++] = up1[a];
        }
        for (; b < cardP; b++)
        {
            up2[c++] = Net::Arc[PL][POST][p][b];
        }
        index_t* tmp = up1;
        up1 = up2;
        up2 = tmp;
        cardUp = c;
    }
    for (index_t i = 0; i < cardNeg; i++)
    {
        const index_t p = negPlaces[i];
        const index_t cardP = Net::CardArcs[PL][PRE][p];

        index_t a = 0; // index in up1
        index_t b = 0; // index in arc list of place
        index_t c = 0; // index in up2 (result)

        while (a < cardUp && b < cardP)
        {
            if (up1[a] < Net::Arc[PL][PRE][p][b])
            {
                up2[c++] = up1[a++];
                continue;
            }
            if (up1[a] > Net::Arc[PL][PRE][p][b])
            {
                up2[c++] = Net::Arc[PL][PRE][p][b++];
                continue;
            }
            assert(up1[a] == Net::Arc[PL][PRE][p][b]);
            up2[c++] = up1[a++];
            ++b;
        }
        for (; a < cardUp; a++)
        {
            up2[c++] = up1[a];
        }
        for (; b < cardP; b++)
        {
            up2[c++] = Net::Arc[PL][PRE][p][b];
        }
        index_t* tmp = up1;
        up1 = up2;
        up2 = tmp;
        cardUp = c;
    }
    up = (index_t*) realloc(up1, cardUp * SIZEOF_INDEX_T);
    free(up2);
}

// only for debugging:
// LCOV_EXCL_START
void AtomicStatePredicate::consistency()
{
    // 1. check sum
    int s = 0;
    for (index_t i = 0; i < cardPos; i++)
    {
        s += posMult[i] * Marking::Current[posPlaces[i]];
    }
    for (index_t i = 0; i < cardNeg; i++)
    {
        s -= negMult[i] * Marking::Current[negPlaces[i]];
    }
    assert(s == sum);
    if (value)
    {
        assert(sum <= threshold);
    }
    else
    {
        assert(sum > threshold);
    }
    if (this != top)
    {
        assert(parent);
    }
}

// LCOV_EXCL_STOP
