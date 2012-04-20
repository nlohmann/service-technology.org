/*!
\author Karsten
\file ParserPTNet.h
\status approved 21.02.2012

Class definition for the result of the parsing of a low level net. This result
should be independent from the format (LoLA / PNML / ...)

\todo Mal den new-Operator in Bezug auf Exceptions ansehen.
*/

#include <config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include <cstring>
#include <cstdlib>

#include "cmdline.h"
#include "Net/Net.h"
#include "Net/Place.h"
#include "Net/Transition.h"
#include "Net/StructuralReduction.h"
#include "Net/Marking.h"
#include "Parser/ArcList.h"
#include "Parser/SymbolTable.h"
#include "Parser/ParserPTNet.h"
#include "Parser/PlaceSymbol.h"
#include "Parser/TransitionSymbol.h"
#include "Parser/Symbol.h"
#include "InputOutput/Reporter.h"


extern gengetopt_args_info args_info;
extern Reporter* rep;

ParserPTNet::ParserPTNet()
{
    PlaceTable = new SymbolTable();
    TransitionTable = new SymbolTable();
}

ParserPTNet::~ParserPTNet()
{
    delete PlaceTable;
    delete TransitionTable;
}


/// sorts array of arc (= node id) plus corresponding array of multiplicities
/// in the range of from to to (not including to)
void ParserPTNet::sort_arcs(index_t* arcs, mult_t* mults, const index_t from, const index_t to)
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

    sort_arcs(arcs, mults, from, blue);
    sort_arcs(arcs, mults, red, to);
}


/*!
\todo comment me
\todo Brauchen wir wirklich Marking::Initial oder können wir das mit Marking::Current abhandeln?
*/
void ParserPTNet::symboltable2net()
{
    /*********************************************
    * 1. Allocate memory for basic net structure *
    *********************************************/

    // 1.1 set cardinalities
    const index_t cardPL = PlaceTable->getCard();
    const index_t cardTR = TransitionTable->getCard();
    Net::Card[PL] = cardPL;
    Net::Card[TR] = cardTR;
    Place::CardSignificant = cardPL; // this is the best choice until we know better

    // 1.2 allocate arrays for node (places and transitions) names, arcs, and multiplicities
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


    /********************************
    * 2. Allocate memory for places *
    *********************************/

    Place::Hash = (hash_t*) malloc(cardPL * SIZEOF_HASH_T);
    Place::Capacity = (capacity_t*) malloc(cardPL * SIZEOF_CAPACITY_T);
    Place::CardBits = (cardbit_t*) malloc(cardPL * SIZEOF_CARDBIT_T);
    Place::CardDisabled = (index_t*) calloc(cardPL , SIZEOF_INDEX_T); // use calloc: initial assumption: no transition is disabled
    Place::Disabled = (index_t**) malloc(cardPL * SIZEOF_VOIDP);


    /**********************************
    * 3. Allocate memory for markings *
    ***********************************/

    Marking::Initial = (capacity_t*) malloc(cardPL * SIZEOF_CAPACITY_T);
    Marking::Current = (capacity_t*) malloc(cardPL * SIZEOF_CAPACITY_T);
    Marking::HashInitial = 0;


    /***********************************************
    * 4. Copy data from the symbol table to places *
    ************************************************/

    // fill all information that is locally available in symbols, allocate node specific arrays
    PlaceSymbol* ps;
    index_t i;
    for ((ps = reinterpret_cast<PlaceSymbol*>(PlaceTable->first())), (i = 0); ps; ps = reinterpret_cast<PlaceSymbol*>(PlaceTable->next()), i++)
    {
        const index_t tempCardPre = ps->getCardPre();
        const index_t tempCardPost = ps->getCardPost();

        // we take care of the place name (not destructed by SymbolTable)
        Net::Name[PL][i] = ps->getKey();
        Net::CardArcs[PL][PRE][i] = tempCardPre;
        Net::CardArcs[PL][POST][i] = tempCardPost;
        ps->setIndex(i);

        // allocate memory for place's arcs (is copied later with transitions)
        Net::Arc[PL][PRE][i] = (index_t*) malloc(tempCardPre * SIZEOF_INDEX_T);
        Net::Arc[PL][POST][i] = (index_t*) malloc(tempCardPost * SIZEOF_INDEX_T);
        Net::Mult[PL][PRE][i] = (mult_t*) malloc(tempCardPre * SIZEOF_MULT_T);
        Net::Mult[PL][POST][i] = (mult_t*) malloc(tempCardPost * SIZEOF_MULT_T);

        // hash and capacity
        Place::Hash[i] = rand() % MAX_HASH;
        Place::Capacity[i] = ps->getCapacity();
        Place::SizeOfBitVector +=
            (Place::CardBits[i] = Place::Capacity2Bits(Place::Capacity[i]));

        // initially: no disabled transistions (through CardDisabled = 0)
        // correct values will be achieved by initial checkEnabled...
        Place::Disabled[i] = (index_t*) malloc(tempCardPost * SIZEOF_INDEX_T);

        // set initial marking and calculate hash
        Marking::Initial[i] = Marking::Current[i] = ps->getInitialMarking();
        Marking::HashInitial = (Marking::HashInitial + Place::Hash[i] * Marking::Initial[i]) % SIZEOF_MARKINGTABLE;
    }

    // set hash value for initial marking
    Marking::HashCurrent = Marking::HashInitial;


    /*************************************
    * 5. Allocate memory for transitions *
    **************************************/

    // allocate memory for static data
    Transition::Fairness = (fairnessAssumption_t*) malloc(cardTR * SIZEOF_FAIRNESSASSUMPTION_T);
    Transition::Enabled = (bool*) malloc(cardTR * SIZEOF_BOOL);
    Transition::DeltaHash = (hash_t*) calloc(cardTR , SIZEOF_HASH_T); // calloc: delta hash must be initially 0
    Transition::CardConflicting = (index_t*) malloc(cardTR * SIZEOF_INDEX_T);
    Transition::Conflicting = (index_t**) malloc(cardTR * SIZEOF_VOIDP);
    Transition::CardBackConflicting = (index_t*) malloc(cardTR * SIZEOF_INDEX_T);
    Transition::BackConflicting = (index_t**) malloc(cardTR * SIZEOF_VOIDP);
    Transition::CardEnabled = cardTR; // start with assumption that all transitions are enabled
    Transition::PositionScapegoat = (index_t*) malloc(cardTR * SIZEOF_INDEX_T);

    // allocate memory for deltas
    for (int direction = PRE; direction <= POST; direction++)
    {
        Transition::CardDeltaT[direction] = (index_t*) calloc(cardTR, SIZEOF_INDEX_T); // calloc: no arcs there yet
        Transition::DeltaT[direction] = (index_t**) malloc(cardTR * SIZEOF_VOIDP);
        Transition::MultDeltaT[direction] = (mult_t**) malloc(cardTR * SIZEOF_VOIDP);
    }


    /****************************************************
    * 6. Copy data from the symbol table to transitions *
    *****************************************************/

    // current_arc is used for filling in arcs and multiplicities of places
    index_t* current_arc_post = (index_t*) calloc(cardPL, SIZEOF_INDEX_T); // calloc: no arcs there yet
    index_t* current_arc_pre = (index_t*) calloc(cardPL, SIZEOF_INDEX_T); // calloc: no arcs there yet

    TransitionSymbol* ts;
    for (ts = reinterpret_cast<TransitionSymbol*>(TransitionTable->first()), i = 0; ts; ts = reinterpret_cast<TransitionSymbol*>(TransitionTable->next()), i++)
    {
        const index_t tempCardPre = ts->getCardPre();
        const index_t tempCardPost = ts->getCardPost();

        // we need to take care of the name (not destructed by SymbolTable)
        Net::Name[TR][i] = ts->getKey();
        Net::CardArcs[TR][PRE][i] = tempCardPre;
        Net::CardArcs[TR][POST][i] = tempCardPost;
        ts->setIndex(i);

        // allocate memory for transition's arcs
        Net::Arc[TR][PRE][i] = (index_t*) malloc(tempCardPre * SIZEOF_INDEX_T);
        Net::Arc[TR][POST][i] = (index_t*) malloc(tempCardPost * SIZEOF_INDEX_T);
        Net::Mult[TR][PRE][i] = (mult_t*) malloc(tempCardPre * SIZEOF_MULT_T);
        Net::Mult[TR][POST][i] = (mult_t*) malloc(tempCardPost * SIZEOF_MULT_T);

        Transition::Enabled[i] = true;
        Transition::Fairness[i] = ts->getFairness();

        // copy arcs (for transitions AND places)
        ArcList* al;
        index_t j;
        for (al = ts->getPre(), j = 0; al; al = al->getNext(), j++)
        {
            const index_t k = al->getPlace()->getIndex();
            Net::Arc[TR][PRE][i][j] = k;
            Net::Arc[PL][POST][k][current_arc_post[k]] = i;
            Net::Mult[PL][POST][k][(current_arc_post[k])++] = Net::Mult[TR][PRE][i][j] = al->getMultiplicity();
        }
        for (al = ts->getPost(), j = 0; al; al = al->getNext(), j++)
        {
            const index_t k = al->getPlace()->getIndex();
            Net::Arc[TR][POST][i][j] = k;
            Net::Arc[PL][PRE][k][current_arc_pre[k]] = i;
            Net::Mult[PL][PRE][k][(current_arc_pre[k])++] = Net::Mult[TR][POST][i][j] = al->getMultiplicity();
        }
    }


    /*********************
    * 7. Organize Deltas *
    **********************/

    // logically, current_arc_* can be freed here, physically, we just rename them to their new purpose (reuse)
    index_t* delta_pre = current_arc_pre;   // temporarily collect places where a transition has negative token balance
    index_t* delta_post = current_arc_post; // temporarily collect places where a transition has positive token balance.

    mult_t* mult_pre = (mult_t*) malloc(cardPL * SIZEOF_MULT_T);   // same for multiplicities
    mult_t* mult_post = (mult_t*) malloc(cardPL * SIZEOF_MULT_T);   // same for multiplicities

    for (index_t t = 0; t < cardTR; t++)
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
                else
                {
                    if (Net::Mult[TR][PRE][t][i] < Net::Mult[TR][POST][t][j])
                    {
                        // positive impact -->goes to delta post
                        delta_post[card_delta_post] = Net::Arc[TR][POST][t][j];
                        mult_post[card_delta_post++] = (mult_t)(Net::Mult[TR][POST][t][j] - Net::Mult[TR][PRE][t][i]);
                    }
                    else
                    {
                        // negative impact -->goes to delta pre
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
        Transition::DeltaT[PRE][t] = (index_t*) malloc(card_delta_pre * SIZEOF_INDEX_T);
        Transition::DeltaT[POST][t] = (index_t*) malloc(card_delta_post * SIZEOF_INDEX_T);
        Transition::MultDeltaT[PRE][t] = (mult_t*) malloc(card_delta_pre * SIZEOF_MULT_T);
        Transition::MultDeltaT[POST][t] = (mult_t*) malloc(card_delta_post * SIZEOF_MULT_T);

        // copy information on deltas
        memcpy(Transition::DeltaT[PRE][t], delta_pre, card_delta_pre * SIZEOF_INDEX_T);
        memcpy(Transition::MultDeltaT[PRE][t], mult_pre, card_delta_pre * SIZEOF_MULT_T);
        memcpy(Transition::DeltaT[POST][t], delta_post, card_delta_post * SIZEOF_INDEX_T);
        memcpy(Transition::MultDeltaT[POST][t], mult_post, card_delta_post * SIZEOF_MULT_T);
    }

    free(delta_pre);
    free(delta_post);
    free(mult_pre);
    free(mult_post);

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


    /**************************************
    * 8. Organize conflicting transitions *
    ***************************************/

    // initialize Conflicting arrays
    index_t* conflicting = (index_t*) calloc(cardTR, SIZEOF_INDEX_T);

    ///\todo make search for conflicting transitions a function to avoid code duplication
    for (index_t t = 0; t < cardTR; t++)
    {
        // 8.1 conflicting transitions
        index_t card_conflicting = 0;

        /// 1. collect all conflicting transitions \f$(\null^\bullet t)^\bullet\f$
        for (index_t i = 0; i < Net::CardArcs[TR][PRE][t]; i++)
        {
            // p is a pre-place
            const index_t p = Net::Arc[TR][PRE][t][i];

            for (index_t j = 0; j < Net::CardArcs[PL][POST][p]; j++)
            {
                // tt is a conflicting transition
                const index_t tt = Net::Arc[PL][POST][p][j];
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
        memcpy(Transition::Conflicting[t], conflicting, card_conflicting * SIZEOF_INDEX_T);


        // 8.2 backward conflicting transitions
        card_conflicting = 0;

        /// 1. collect all backward conflicting transitions \f$(t^\bullet)^\bullet\f$
        for (index_t i = 0; i < Net::CardArcs[TR][POST][t]; i++)
        {
            // p is a post-place
            const index_t p = Net::Arc[TR][POST][t][i];

            for (index_t j = 0; j < Net::CardArcs[PL][POST][p]; j++)
            {
                // tt is a backward conflicting transition
                const index_t tt = Net::Arc[PL][POST][p][j];
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
        memcpy(Transition::BackConflicting[t], conflicting, card_conflicting * SIZEOF_INDEX_T);
    }

    free(conflicting);

    /****************************
    * 9. Set significant places *
    ****************************/
    if (not args_info.nostructural_flag)
    {
        setSignificantPlaces();
        rep->status("%d places, %d transitions, %d significant places", Net::Card[PL], Net::Card[TR], Place::CardSignificant);
    }

    /********************************
    * 10. Initial enabledness check *
    *********************************/

    for (index_t t = 0; t < cardTR; t++)
    {
        Transition::checkEnabled(t);
    }
}
