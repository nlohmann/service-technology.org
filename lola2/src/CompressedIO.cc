/*!

\file CompressedIO.cc
\author Karsten
\status new

Input and outout from/to a file in compressed format. We generate two separate files that
can be read in arbitrary order. In this version, we use an ASCII file where data are separated
by spaces and newlines

*/

#include <cstdio>
#include <cstring>
#include "CompressedIO.h"
#include "Net.h"
#include "Marking.h"
#include "ParserPTNet.h"
#include "Place.h"
#include "Transition.h"

void WriteNameFile(FILE * f)
{
	// 1. Number of places
	fprintf(f,"%u\n", Net::Card[PL]);

	// 2. Places with one name per line, oin order of indices
	for(index_t p = 0; p < Net::Card[PL]; p++)
	{
		fprintf(f,"%s\n", Net::Name[PL][p]);
	}

	// 3. Number of transition
	fprintf(f,"%u\n", Net::Card[TR]);
	
	// 4. Transitions with one name per line, in order of indices
	for(index_t t = 0; t < Net::Card[TR]; t++)
	{
		fprintf(f,"%s\n", Net::Name[TR][t]);
	}
}

void ReadNameFile(FILE * f)
{
	char buffer[10000];
	unsigned int tmp1, tmp2;

	// 1. Number of places
	fscanf(f,"%u",&tmp1);
	Net::Card[PL] = (index_t) tmp1;

	// 2. Places with one index, one name per line
	for(index_t p = 0; p < Net::Card[PL]; p++)
	{
		fscanf(f,"%s",buffer);
		Net::Name[PL][p] = (char *) malloc((strlen(buffer)+1) * sizeof(char)) ;
		strcpy(Net::Name[PL][p],buffer);
	}
	

	// 3. Number of transition
	fscanf(f,"%u",&tmp1);
	Net::Card[TR] = (index_t) tmp1;
	
	// 4. Transitions with one index, one name per line
	for(index_t t = 0; t < Net::Card[TR]; t++)
	{
		fscanf(f,"%s",buffer);
		Net::Name[TR][t] = (char *) malloc((strlen(buffer)+1) * sizeof(char)) ;
		strcpy(Net::Name[TR][t],buffer);
	}
}

void WriteNetFile(FILE * f)
{
	// 1. Number of places and significant places
	fprintf(f,"%u %u", Net::Card[PL],Place::CardSignificant);

	// 2. For each place...
	for(index_t p = 0; p < Net::Card[PL]; p++)
	{	
		fprintf(f,"\n");
		// 2.a initial marking
		fprintf(f,"%u ", Marking::Initial[p]);

		// 2.b capacity
		fprintf(f,"%u ", Place::Capacity[p]);
		
		// 2.c nr of incoming arcs
		fprintf(f,"%u ", Net::CardArcs[PL][PRE][p]);

		for(index_t i = 0; i < Net::CardArcs[PL][PRE][p]; i++)
		{
			// 2.d incoming arcs and multiplicities
			fprintf(f,"%u %u ",Net::Arc[PL][PRE][p][i],Net::Mult[PL][PRE][p][i]);
		}

		// 2.e nr of outgoing arcs
		fprintf(f,"%u ", Net::CardArcs[PL][POST][p]);

		for(index_t i = 0; i < Net::CardArcs[PL][POST][p]; i++)
		{
			// 2.f outgoing arcs and multiplicities
			fprintf(f,"%u %u ",Net::Arc[PL][POST][p][i],Net::Mult[PL][POST][p][i]);
		}
	}
	// 3. Number of transitions
	fprintf(f,"\n%u", Net::Card[TR]);

	// 2. For each transition...
	for(index_t t = 0; t < Net::Card[TR]; t++)
	{	
		fprintf(f,"\n");
		// 2.a fairness
		int fair;
		switch(Transition::Fairness[t])
		{
		case NO_FAIRNESS: fair = 0; break;
		case WEAK_FAIRNESS: fair = 1; break;
		case STRONG_FAIRNESS: fair = 2; break;
		default: fair = 4;
		}
		fprintf(f,"%u ", fair);

		// 2.b nr of incoming arcs
		fprintf(f,"%u ", Net::CardArcs[TR][PRE][t]);

		for(index_t i = 0; i < Net::CardArcs[TR][PRE][t]; i++)
		{
			// 2.d incoming arcs and multiplicities
			fprintf(f,"%u %u ",Net::Arc[TR][PRE][t][i],Net::Mult[TR][PRE][t][i]);
		}

		// 2.e nr of outgoing arcs
		fprintf(f,"%u ", Net::CardArcs[TR][POST][t]);

		for(index_t i = 0; i < Net::CardArcs[TR][POST][t]; i++)
		{
			// 2.f outgoing arcs and multiplicities
			fprintf(f,"%u %u ",Net::Arc[TR][POST][t][i],Net::Mult[TR][POST][t][i]);
		}
	}

}


#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif


void ReadNetFile(FILE * f)
{
    // read number of places
    unsigned int tmp1, tmp2;
    fscanf(f,"%u %u",&tmp1,&tmp2);
     Net::Card[PL] = (index_t) tmp1;
     Place::CardSignificant = (index_t) tmp2;
    // allocate place arrays
        Net::Name[PL] = (char**) calloc(Net::Card[PL] , SIZEOF_VOIDP);
        for (int direction = PRE; direction <= POST; direction ++)
        {
            Net::CardArcs[PL][direction] = (index_t*) malloc(Net::Card[PL] * SIZEOF_INDEX_T);
            Net::Arc[PL][direction] = (index_t**) malloc(Net::Card[PL] * SIZEOF_VOIDP);
            Net::Mult[PL][direction] = (mult_t**) malloc(Net::Card[PL] * SIZEOF_VOIDP);

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
    for (index_t p = 0; p < Net::Card[PL]; p++)
    {
        Place::Hash[p] = rand() % MAX_HASH;
	fscanf(f,"%u",&tmp1);
	Marking::Initial[p] = (capacity_t) tmp1;
	Marking::Current[p] = Marking::Initial[p];
        Marking::HashInitial += Place::Hash[p] * Marking::Initial[p];
        Marking::HashInitial %= SIZEOF_MARKINGTABLE;
	fscanf(f,"%u",&tmp1);
	Place::Capacity[p] = (capacity_t) tmp1;
        Place::CardBits[p] = Place::Capacity2Bits(Place::Capacity[p]);
        fscanf(f,"%u",&tmp1);
	Net::CardArcs[PL][PRE][p] = (index_t) tmp1;
        Net::Arc[PL][PRE][p] = (index_t*) malloc(Net::CardArcs[PL][PRE][p] * SIZEOF_INDEX_T);
        Net::Mult[PL][PRE][p] = (mult_t*) malloc(Net::CardArcs[PL][PRE][p] * SIZEOF_MULT_T);
	for(index_t i = 0; i < Net::CardArcs[PL][PRE][p];i++)
	{
		fscanf(f,"%u %u", &tmp1,&tmp2);
		Net::Arc[PL][PRE][p][i] = (index_t) tmp1;
		Net::Mult[PL][PRE][p][i] = (mult_t) tmp2;
	}
        fscanf(f,"%u",&tmp1);
	Net::CardArcs[PL][POST][p] = (index_t) tmp1;
        Net::Arc[PL][POST][p] = (index_t*) malloc(Net::CardArcs[PL][POST][p] * SIZEOF_INDEX_T);
        Net::Mult[PL][POST][p] = (mult_t*) malloc(Net::CardArcs[PL][POST][p] * SIZEOF_MULT_T);
	for(index_t i = 0; i < Net::CardArcs[PL][POST][p];i++)
	{
		fscanf(f,"%u %u", &tmp1,&tmp2);
		Net::Arc[PL][POST][p][i] = (index_t) tmp1;
		Net::Mult[PL][POST][p][i] = (mult_t) tmp2;
	}
        // initially: no disabled transistions (through CardDisabled = 0)
        // correct values will be achieved by initial checkEnabled...
        Place::Disabled[p] = (index_t*) malloc(Net::CardArcs[PL][POST][p] * SIZEOF_INDEX_T);

    }
    Marking::HashCurrent = Marking::HashInitial;
    // Allocate arrays for places and transitions

    fscanf(f,"%u",&tmp1);
    Net::Card[TR] = (index_t) tmp1;

        Net::Name[TR] = (char**) calloc(Net::Card[TR] , SIZEOF_VOIDP);
        for (int direction = PRE; direction <= POST; direction ++)
        {
            Net::CardArcs[TR][direction] = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
            Net::Arc[TR][direction] = (index_t**) malloc(Net::Card[TR] * SIZEOF_VOIDP);
            Net::Mult[TR][direction] = (mult_t**) malloc(Net::Card[TR] * SIZEOF_VOIDP);

        }

    index_t* current_arc_post = (index_t*) calloc(Net::Card[PL], SIZEOF_INDEX_T);
    index_t* current_arc_pre = (index_t*) calloc(Net::Card[PL], SIZEOF_INDEX_T);
    Transition::Fairness = (fairnessAssumption_t*) malloc(Net::Card[TR] * SIZEOF_FAIRNESSASSUMPTION_T);
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
    for (index_t t = 0; t < Net::Card[TR]; t++)
    {
	unsigned int fair;
	fscanf(f,"%u",&fair);
	switch(fair)
	{
	case 0: Transition::Fairness[t] = NO_FAIRNESS; break;
	case 1: Transition::Fairness[t] = WEAK_FAIRNESS; break;
	case 2: Transition::Fairness[t] = STRONG_FAIRNESS; break;
	default: Transition::Fairness[t] = NO_FAIRNESS;
	}
        fscanf(f,"%u",&tmp1); 
	Net::CardArcs[TR][PRE][t] = (index_t) tmp1;
        Net::Arc[TR][PRE][t] = (index_t*) malloc(Net::CardArcs[TR][PRE][t] * SIZEOF_INDEX_T);
        Net::Mult[TR][PRE][t] = (mult_t*) malloc(Net::CardArcs[TR][PRE][t] * SIZEOF_MULT_T);
        Transition::Enabled[t] = true;
	for(index_t i = 0; i < Net::CardArcs[TR][PRE][t]; i++)
	{
		fscanf(f,"%u %u",&tmp1,&tmp2);
		Net::Arc[TR][PRE][t][i] = (index_t) tmp1;
		Net::Mult[TR][PRE][t][i] = (mult_t) tmp2;
	}
        fscanf(f,"%u",&tmp1); 
	Net::CardArcs[TR][POST][t] = (index_t) tmp1;
        Net::Arc[TR][POST][t] = (index_t*) malloc(Net::CardArcs[TR][POST][t] * SIZEOF_INDEX_T);
        Net::Mult[TR][POST][t] = (mult_t*) malloc(Net::CardArcs[TR][POST][t] * SIZEOF_MULT_T);
	for(index_t i = 0; i < Net::CardArcs[TR][POST][t]; i++)
	{
		fscanf(f,"%u %u",&tmp1,&tmp2);
		Net::Arc[TR][POST][t][i] = (index_t) tmp1;
		Net::Mult[TR][POST][t][i] = (mult_t) tmp2;
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
        ParserPTNet::sort_arcs(Net::Arc[TR][PRE][t], Net::Mult[TR][PRE][t], 0, Net::CardArcs[TR][PRE][t]);
        ParserPTNet::sort_arcs(Net::Arc[TR][POST][t], Net::Mult[TR][POST][t], 0, Net::CardArcs[TR][POST][t]);
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
