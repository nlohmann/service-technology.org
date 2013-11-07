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

#include <cmdline.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <Net/Marking.h>
#include <Parser/ArcList.h>
#include <Parser/SymbolTable.h>
#include <Parser/ParserPTNet.h>
#include <Parser/PlaceSymbol.h>
#include <Parser/TransitionSymbol.h>
#include <Parser/Symbol.h>
#include <InputOutput/Reporter.h>

extern gengetopt_args_info args_info;
extern Reporter *rep;

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
    Place::CardSignificant = -1; // mark as "still to be computed"

    // 1.2 allocate arrays for node (places and transitions) names, arcs, and multiplicities
    for (int type = PL; type <= TR; type ++)
    {
        Net::Name[type] = new char*[Net::Card[type]];
        for (int direction = PRE; direction <= POST; direction ++)
        {
            Net::CardArcs[type][direction] = new index_t[Net::Card[type]];
            Net::Arc[type][direction] = new index_t *[Net::Card[type]];
            Net::Mult[type][direction] = new mult_t *[Net::Card[type]];
        }
    }


    /********************************
    * 2. Allocate memory for places *
    *********************************/

    Place::Capacity = new capacity_t[cardPL];


    /**********************************
    * 3. Allocate memory for markings *
    ***********************************/

    Marking::Initial = new capacity_t[cardPL];
    Marking::Current = new capacity_t[cardPL];


    /***********************************************
    * 4. Copy data from the symbol table to places *
    ************************************************/

    // fill all information that is locally available in symbols, allocate node specific arrays
    PlaceSymbol *ps;
    index_t i;
    for ((ps = reinterpret_cast<PlaceSymbol *>(PlaceTable->first())), (i = 0); ps;
            ps = reinterpret_cast<PlaceSymbol *>(PlaceTable->next()), i++)
    {
        const index_t tempCardPre = ps->getCardPre();
        const index_t tempCardPost = ps->getCardPost();

        // we take care of the place name (not destructed by SymbolTable)
        Net::Name[PL][i] = ps->getKey();
        Net::CardArcs[PL][PRE][i] = tempCardPre;
        Net::CardArcs[PL][POST][i] = tempCardPost;
        ps->setIndex(i);

        // allocate memory for place's arcs (is copied later with transitions)
        Net::Arc[PL][PRE][i] = new index_t[tempCardPre];
        Net::Arc[PL][POST][i] = new index_t[tempCardPost];
        Net::Mult[PL][PRE][i] = new mult_t[tempCardPre];
        Net::Mult[PL][POST][i] = new mult_t[tempCardPost];

        // capacity
        Place::Capacity[i] = ps->getCapacity();

        // set initial marking and calculate hash
        Marking::Initial[i] = Marking::Current[i] = ps->getInitialMarking();
    }

    /*************************************
    * 5. Allocate memory for transitions *
    **************************************/

    // allocate memory for static data
    Transition::Fairness = new fairnessAssumption_t[cardTR];


    /****************************************************
    * 6. Copy data from the symbol table to transitions *
    *****************************************************/

    // current_arc is used for filling in arcs and multiplicities of places
    // calloc: no arcs there yet
    index_t *current_arc_post = new index_t[cardPL]();
    index_t *current_arc_pre = new index_t[cardPL]();

    TransitionSymbol *ts;
    for (ts = reinterpret_cast<TransitionSymbol *>(TransitionTable->first()), i = 0; ts;
            ts = reinterpret_cast<TransitionSymbol *>(TransitionTable->next()), i++)
    {
        const index_t tempCardPre = ts->getCardPre();
        const index_t tempCardPost = ts->getCardPost();

        // we need to take care of the name (not destructed by SymbolTable)
        Net::Name[TR][i] = ts->getKey();
        Net::CardArcs[TR][PRE][i] = tempCardPre;
        Net::CardArcs[TR][POST][i] = tempCardPost;
        ts->setIndex(i);

        // allocate memory for transition's arcs
        Net::Arc[TR][PRE][i] = new index_t[tempCardPre];
        Net::Arc[TR][POST][i] = new index_t[tempCardPost];
        Net::Mult[TR][PRE][i] = new mult_t[tempCardPre];
        Net::Mult[TR][POST][i] = new mult_t[tempCardPost];

        Transition::Fairness[i] = ts->getFairness();

        // copy arcs (for transitions AND places)
        ArcList *al;
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

    //rep->message("Names %s %s %s %s", Net::Name[PL][0],Net::Name[PL][1],Net::Name[PL][2],Net::Name[PL][3]);
    delete[] current_arc_pre;
    delete[] current_arc_post;


    /**********************
    * 6. Do preprocessing *
    **********************/

    // net is read completely now, time to preprocess
    Net::preprocess();
}
