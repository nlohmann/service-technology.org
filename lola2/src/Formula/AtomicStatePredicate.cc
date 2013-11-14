/*!
\file AtomicStatePredicate.cc
\author Karsten
\status new

\brief class implementation for atomic state predicates
*/

#include <config.h>
#include <Formula/AtomicStatePredicate.h>
#include <Net/Net.h>
#include <Net/NetState.h>
#include <Net/Marking.h>
#include <cstdlib>
#include <cstdio>
#include <InputOutput/Reporter.h>

extern Reporter *rep;

/*!
\brief creates a state predicate with a formal sum of #p places with positive factor,
#n places with negative factor, and constant #k particular places are added
using addPos and addNeg

\param p  number of places with positive factor
\param n  number of places with negative factor
\param k  constant

\todo Schleifen behandeln - können evtl. rausgenommen werden
*/
AtomicStatePredicate::AtomicStatePredicate(index_t p, index_t n, int k) :
    posPlaces(new index_t[p]),
    negPlaces(new index_t[n]),
    posMult(new capacity_t[p]),
    negMult(new capacity_t[n]),
    cardPos(p),
    cardNeg(n),
    threshold(k),
    original(true)
{
    //printf("+ %p->AtomicStatePredicate(p=%d, n=%d, k=%d)\n", this, p, n, k);

    parent = NULL;
}

AtomicStatePredicate::~AtomicStatePredicate()
{
    if (!original)
    {
        return;
    }
    delete[] posPlaces;
    delete[] negPlaces;
    delete[] posMult;
    delete[] negMult;
    free(up);
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

index_t AtomicStatePredicate::getUpSet(index_t *stack, bool *onstack)
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


void AtomicStatePredicate::update(NetState &ns, int delta)
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

void AtomicStatePredicate::evaluate(NetState &ns)
{
    sum = 0;
    for (index_t i = 0; i < cardPos; ++i)
    {
        sum += ns.Current[posPlaces[i]] * posMult[i];
    }
    for (index_t i = 0; i < cardNeg; ++i)
    {
        sum -= ns.Current[negPlaces[i]] * negMult[i];
    }
    //rep->message("SUM THR %d %d", sum, threshold);
    value = (sum <= threshold);
}

index_t AtomicStatePredicate::countAtomic() const
{
    return 1;
}

index_t AtomicStatePredicate::collectAtomic(AtomicStatePredicate **c)
{
    c[0] = this;
    return 1;
}

void AtomicStatePredicate::setUpSet()
{
    // up1 cannot be allocated with new[], because of a later realloc
    index_t *up1 = (index_t *) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    index_t *up2 = (index_t *) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
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
        index_t *tmp = up1;
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
        index_t *tmp = up1;
        up1 = up2;
        up2 = tmp;
        cardUp = c;
    }
    up = (index_t *) realloc(up1, cardUp * SIZEOF_INDEX_T);
    free(up2);
}

// LCOV_EXCL_START
bool AtomicStatePredicate::DEBUG__consistency(NetState &ns)
{
    // 1. check sum
    int s = 0;
    for (index_t i = 0; i < cardPos; i++)
    {
        s += posMult[i] * ns.Current[posPlaces[i]];
    }
    for (index_t i = 0; i < cardNeg; i++)
    {
        s -= negMult[i] * ns.Current[negPlaces[i]];
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
    /* if (this != top)
     {
         assert(parent);
     }*/
    return true;
}
// LCOV_EXCL_STOP


StatePredicate *AtomicStatePredicate::copy(StatePredicate *parent)
{
    AtomicStatePredicate *af = new AtomicStatePredicate(0, 0, 0);
    af->value = value;
    af->position = position;
    af->parent = parent;
    // we can copy the pointers, so use the same arrays as they are not changed!
    af->posPlaces = posPlaces;
    af->negPlaces = negPlaces;
    af->posMult = posMult;
    af->negMult = negMult;
    af->cardPos = cardPos;
    af->cardNeg = cardNeg;
    af->threshold = threshold;
    af->sum = sum;
    af->up = up;
    af->cardUp = cardUp;
    af->original = false;
    return af;
}

index_t AtomicStatePredicate::getSubs(const StatePredicate *const **subs) const
{
    return 0;
}


StatePredicate *AtomicStatePredicate::negate()
{
    AtomicStatePredicate *af = new AtomicStatePredicate(cardNeg, cardPos, - threshold - 1);
    for (index_t i = 0; i < cardPos; i++)
    {
        af->addNeg(i, posPlaces[i], posMult[i]);
    }
    for (index_t i = 0; i < cardNeg; i++)
    {
        af->addPos(i, negPlaces[i], negMult[i]);
    }
    return af;
}
