/*!
\file RandomWalk.cc
\author Karsten
\status new

\brief Repeatedly fire random transitions.
*/

#include <cstdio>
#include <cstdlib>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Net/NetState.h>


void randomWalk(NetState& ns, int transitions)
{
    int firedTransitions = 0;

    // repeat forever
    while (true)
    {
        // one iteration is for one execution (either infinite or ending in deadlock)
        while (ns.CardEnabled > 0)
        {
            // one iteration is for firing a single transition
            // 1. select transition
            index_t y = 0;
            for (index_t z = 0; z < Net::Card[TR]; z++)
            {
                if (ns.Enabled[z])
                {
                    y++;
                }
            }
            assert(y == ns.CardEnabled);
            index_t nr = rand() % ns.CardEnabled; // uniform distribution
            assert(nr < ns.CardEnabled);
            index_t t; // the transition to be fired
            for (t = 0; t < Net::Card[TR]; ++t)
            {
                if (ns.Enabled[t])
                {
                    if (nr == 0)
                    {
                        break;
                    }
                    --nr;
                }
            }
            assert(t < Net::Card[TR]);
            assert(ns.Enabled[t]);
            printf(" firing %s\n", Net::Name[TR][t]);
            Transition::fire(ns, t);
            Transition::updateEnabled(ns, t);

            // early abortion
            if (transitions != 0 and (++firedTransitions >= transitions))
            {
                return;
            }
        }
        printf("\n*************\n*************\n");
        // reset initial marking
        for (index_t i = 0; i < Net::Card[PL]; i++)
        {
            ns.Current[i] = Marking::Initial[i];
            ns.CardDisabled[i] = 0;
        }
        Marking::HashCurrent = Marking::HashInitial;
        ns.CardEnabled = Net::Card[TR];
        for (index_t i = 0; i < Net::Card[TR]; i++)
        {
            ns.Enabled[i] = true;
        }
        for (index_t i = 0; i < Net::Card[TR]; i++)
        {
            Transition::checkEnabled(ns, i);
        }
    }
}
