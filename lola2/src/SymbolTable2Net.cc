/*!
\file SymbolTable2Net.cc
\author Karsten
\status new
\brief Translation of the symbol tables (from parsers) into main net structures Node, Place, Transition, and Marking
*/


#include <config.h>
#include <cstdlib>
#include "SymbolTable2Net.h"
#include "Net.h"
#include "Place.h"
#include "Marking.h"
#include "PlaceSymbol.h"
#include "Transition.h"
#include "TransitionSymbol.h"
#include "Symbol.h"

/// sorts array of arc (= node id) plus corresponding array of multiplicities
/// in the range of from to to (not including to)
void sort_arcs(index_t* arcs, mult_t* mults, index_t from, index_t to)
{
    if (to - from < 2)
    {
        return;    // less than 2 elements are always sorted
    }
    index_t blue = from; // points to first index where element is not known < pivot
    index_t white = from + 1; // points to first index where element is not know <= pivot
    index_t red = to; // points to last index (+1) where element is not know to be  pivot
    index_t pivot = arcs[from];

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
    sort_arcs(arcs, mults, from, blue);
    sort_arcs(arcs, mults, red, to);
}

void symboltable2net(ParserPTNet* parser)
{
    // Allocate arrays for places and transitions
    Net::Card[PL] = parser->PlaceTable->getCard();
    Net::Card[TR] = parser->TransitionTable->getCard();
    Place::CardSignificant = Net::Card[PL]; // this is the best choice until we know better
    for (int type = PL; type <= TR; type ++)
    {
        Net::Name[type] = (char**) malloc(Net::Card[type] * SIZEOF_VOIDP);
        for (int direction = PRE; direction <= POST; direction ++)
        {
            Net::CardArcs[type][direction] = (index_t*) malloc(Net::Card[type] * SIZEOF_INDEX_T);
            Net::Arc[type][direction] = (index_t**) malloc(Net::Card[type] * SIZEOF_VOIDP);
            Net::Mult[type][direction] = (mult_t**) malloc(Net::Card[type] * SIZEOF_VOIDP);

        }
    }
    Place::Hash = (hash_t*) malloc(Net::Card[PL] * SIZEOF_HASH_T);
    Place::Capacity = (capacity_t*) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    Place::CardBits = (cardbit_t*) malloc(Net::Card[PL] * SIZEOF_CARDBIT_T);
    Place::CardDisabled = (index_t*) calloc(Net::Card[PL] , SIZEOF_INDEX_T);
    Place::Disabled = (index_t**) malloc(Net::Card[PL] * SIZEOF_VOIDP);
    Marking::Initial = (capacity_t*) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    Marking::Current = (capacity_t*) malloc(Net::Card[PL] * SIZEOF_CAPACITY_T);
    Marking::HashInitial = 0;
    // fill all information that is locally available in symbols, allocate node specific arrays
    PlaceSymbol* ps;
    index_t i;
    for ((ps = (PlaceSymbol*) parser->PlaceTable->first()), (i = 0); ps; ps = (PlaceSymbol*) parser->PlaceTable->next(), i++)
    {
        Net::Name[PL][i] = ps -> getKey();
        Net::CardArcs[PL][PRE][i] = ps -> getCardPre();
        Net::CardArcs[PL][POST][i] = ps -> getCardPost();
        ps -> setIndex(i);
        Net::Arc[PL][PRE][i] = (index_t*) malloc(ps -> getCardPre() * SIZEOF_INDEX_T);
        Net::Arc[PL][POST][i] = (index_t*) malloc(ps -> getCardPost() * SIZEOF_INDEX_T);
        Net::Mult[PL][PRE][i] = (mult_t*) malloc(ps -> getCardPre() * SIZEOF_MULT_T);
        Net::Mult[PL][POST][i] = (mult_t*) malloc(ps -> getCardPost() * SIZEOF_MULT_T);
        Place::Hash[i] = rand() % MAX_HASH;
        Place::Capacity[i] = ps -> getCapacity();
        Place::CardBits[i] = Place::Capacity2Bits(Place::Capacity[i]);
        // initially: no disabled transistions (through CardDisabled = 0)
        // correct values will be achieved by initial checkEnabled...
        Place::Disabled[i] = (index_t*) malloc(Net::CardArcs[PL][POST][i] * SIZEOF_INDEX_T);
        Marking::Initial[i] = Marking::Current[i] = ps -> getInitialMarking();
        Marking::HashInitial += Place::Hash[i] * Marking::Initial[i];
        Marking::HashInitial %= SIZEOF_MARKINGTABLE;

    }
    Marking::HashCurrent = Marking::HashInitial;

    // current_arc is used for filling in arcs and multiplicities of places
    index_t* current_arc_post = (index_t*) calloc(Net::Card[PL], SIZEOF_INDEX_T);
    index_t* current_arc_pre = (index_t*) calloc(Net::Card[PL], SIZEOF_INDEX_T);
    Transition::Fairness = (tFairnessAssumption*) malloc(Net::Card[TR] * sizeof(tFairnessAssumption));
    Transition::Enabled = (bool*) malloc(Net::Card[TR] * SIZEOF_BOOL);
    Transition::DeltaHash = (hash_t*) calloc(Net::Card[TR] , SIZEOF_HASH_T);
    Transition::CardConflicting = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    Transition::Conflicting = (index_t**) malloc(Net::Card[TR] * SIZEOF_VOIDP);
    Transition::CardBackConflicting = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    Transition::BackConflicting = (index_t**) malloc(Net::Card[TR] * SIZEOF_VOIDP);
    Transition::CardEnabled = Net::Card[TR]; // start with assumption that all transitions are enabled
    Transition::PositionScapegoat = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    for (int direction = PRE; direction <= POST; direction++)
    {
        Transition::CardDeltaT[direction] = (index_t*) calloc(Net::Card[TR], SIZEOF_INDEX_T);
        Transition::DeltaT[direction] = (index_t**) malloc(Net::Card[TR] * SIZEOF_VOIDP);
        Transition::MultDeltaT[direction] = (mult_t**) malloc(Net::Card[TR] * SIZEOF_VOIDP);
    }
    TransitionSymbol* ts;
    for (ts = (TransitionSymbol*) parser->TransitionTable->first(), i = 0; ts; ts = (TransitionSymbol*) parser->TransitionTable->next(), i++)
    {
        Net::Name[TR][i] = ts -> getKey();
        Net::CardArcs[TR][PRE][i] = ts -> getCardPre();
        Net::CardArcs[TR][POST][i] = ts -> getCardPost();
        ts -> setIndex(i);
        Net::Arc[TR][PRE][i] = (index_t*) malloc(ts -> getCardPre() * SIZEOF_INDEX_T);
        Net::Arc[TR][POST][i] = (index_t*) malloc(ts -> getCardPost() * SIZEOF_INDEX_T);
        Net::Mult[TR][PRE][i] = (mult_t*) malloc(ts -> getCardPre() * SIZEOF_MULT_T);
        Net::Mult[TR][POST][i] = (mult_t*) malloc(ts -> getCardPost() * SIZEOF_MULT_T);
        Transition::Enabled[i] = true;
        Transition::Fairness[i] = ts -> getFairness();
        ArcList* al;
        index_t j;
        for (al = ts ->getPre(), j = 0; al; al = al -> getNext(), j++)
        {
            index_t k;
            Net::Arc[TR][PRE][i][j] = (k = al -> getPlace() -> getIndex());
            Net::Arc[PL][POST][k][current_arc_post[k]] = i;
            Net::Mult[PL][POST][k][(current_arc_post[k])++] =
                Net::Mult[TR][PRE][i][j] = al -> getMultiplicity();
        }
        for (al = ts ->getPost(), j = 0; al; al = al -> getNext(), j++)
        {
            index_t k;
            Net::Arc[TR][POST][i][j] = (k = al -> getPlace() -> getIndex());
            Net::Arc[PL][PRE][k][current_arc_pre[k]] = i;
            Net::Mult[PL][PRE][k][(current_arc_pre[k])++] =
                Net::Mult[TR][POST][i][j] = al -> getMultiplicity();
        }
    }
    // logically, current_arc_* can be freed here, physically, we just rename them to
    // their new purpose
    //free(current_arc_pre);
    //free(current_arc_post);
    index_t* delta_pre = current_arc_pre;   // temporarily collect places where a transition
    // has negative token balance
    index_t* delta_post = current_arc_post; // temporarily collect places where a transition
    // has positive token balance.
    mult_t* mult_pre = (mult_t*) malloc(Net::Card[PL] * SIZEOF_MULT_T);   // same for multiplicities
    mult_t* mult_post = (mult_t*) malloc(Net::Card[PL] * SIZEOF_MULT_T);   // same for multiplicities
    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
        // initialize DeltaT structures
        index_t card_delta_pre = 0;
        index_t card_delta_post = 0;
        sort_arcs(Net::Arc[TR][PRE][t], Net::Mult[TR][PRE][t], 0, Net::CardArcs[TR][PRE][t]);
        sort_arcs(Net::Arc[TR][POST][t], Net::Mult[TR][POST][t], 0, Net::CardArcs[TR][POST][t]);
        index_t i; // parallel iteration through sorted pre and post arc sets
        index_t j;
        for (i = 0, j = 0; (i < Net::CardArcs[TR][PRE][t]) && (j < Net::CardArcs[TR][POST][t]); /* tricky increment */)
        {
            if (Net::Arc[TR][PRE][t][i] == Net::Arc[TR][POST][t][j])
            {
                // double arc, compare multiplicities
                if (Net::Mult[TR][PRE][t][i] == Net::Mult[TR][POST][t][j])
                {
                    // test arc, does not contribute to delta t
                }
                else if (Net::Mult[TR][PRE][t][i] < Net::Mult[TR][POST][t][j])
                {
                    //positive impact --> goes to delta post
                    delta_post[card_delta_post] = Net::Arc[TR][POST][t][j];
                    mult_post[card_delta_post++] = (mult_t)(Net::Mult[TR][POST][t][j] - Net::Mult[TR][PRE][t][i]);
                }
                else
                {
                    // negative impact --> goes to delta pre
                    delta_pre[card_delta_pre] = Net::Arc[TR][PRE][t][i];
                    mult_pre[card_delta_pre++] = (mult_t)(Net::Mult[TR][PRE][t][i] - Net::Mult[TR][POST][t][j]);
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
        Transition::CardDeltaT[PRE][t] = card_delta_pre;
        Transition::CardDeltaT[POST][t] = card_delta_post;
        Transition::DeltaT[PRE][t] = (index_t*) malloc(card_delta_pre * SIZEOF_INDEX_T);
        Transition::DeltaT[POST][t] = (index_t*) malloc(card_delta_post * SIZEOF_INDEX_T);
        Transition::MultDeltaT[PRE][t] = (mult_t*) malloc(card_delta_pre * SIZEOF_MULT_T);
        Transition::MultDeltaT[POST][t] = (mult_t*) malloc(card_delta_post * SIZEOF_MULT_T);
        for (i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
        {
            Transition::DeltaT[PRE][t][i] = delta_pre[i];
            Transition::MultDeltaT[PRE][t][i] = mult_pre[i];
        }
        for (j = 0; j < Transition::CardDeltaT[POST][t]; j++)
        {
            Transition::DeltaT[POST][t][j] = delta_post[j];
            Transition::MultDeltaT[POST][t][j] = mult_post[j];
        }
    }
    // initialize Conflicting arrays
    // free or rename temporary data structures
    index_t* conflicting = delta_pre;
    free(delta_post);
    free(mult_pre);
    free(mult_post);
    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
        index_t card_conflicting = 0;

        // 1. collect all conflicting transitions (bullet t)bullet
        for (index_t i = 0; i < Net::CardArcs[TR][PRE][t]; i++)
        {
            // p is a pre-place
            index_t p = Net::Arc[TR][PRE][t][i];

            for (index_t j = 0; j < Net::CardArcs[PL][POST][p]; j++)
            {
                // tt is a conflicting transition
                index_t tt = Net::Arc[PL][POST][p][j];
                if (t == tt)
                {
                    continue;    // no conflict between t and itself
                }
                bool included = false;
                for (index_t k = 0; k < card_conflicting; k++)
                {
                    if (tt == conflicting[k])
                    {
                        included = true;
                        break;
                    }
                }
                if (!included)
                {
                    conflicting[card_conflicting++] = tt;
                }

            }
        }
        Transition::CardConflicting[t] = card_conflicting;
        Transition::Conflicting[t] = (index_t*) malloc(card_conflicting * SIZEOF_INDEX_T);
        for (index_t k = 0; k < card_conflicting; k++)
        {
            Transition::Conflicting[t][k] = conflicting[k];
        }

        card_conflicting = 0;

        // 1. collect all backward conflicting transitions (t bullet)bullet
        for (index_t i = 0; i < Net::CardArcs[TR][POST][t]; i++)
        {
            // p is a post-place
            index_t p = Net::Arc[TR][POST][t][i];

            for (index_t j = 0; j < Net::CardArcs[PL][POST][p]; j++)
            {
                // tt is a backward conflicting transition
                index_t tt = Net::Arc[PL][POST][p][j];
                if (t == tt)
                {
                    continue;    // no conflict between t and itself
                }
                bool included = false;
                for (index_t k = 0; k < card_conflicting; k++)
                {
                    if (tt == conflicting[k])
                    {
                        included = true;
                        break;
                    }
                }
                if (!included)
                {
                    conflicting[card_conflicting++] = tt;
                }

            }
        }
        Transition::CardBackConflicting[t] = card_conflicting;
        Transition::BackConflicting[t] = (index_t*) malloc(card_conflicting * SIZEOF_INDEX_T);
        for (index_t k = 0; k < card_conflicting; k++)
        {
            Transition::BackConflicting[t][k] = conflicting[k];
        }
    }
    free(conflicting);
    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
        Transition::checkEnabled(t);
    }
}
