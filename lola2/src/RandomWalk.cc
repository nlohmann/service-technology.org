/*!
\file RandomWalk.cc
\author Karsten
\status new

repeatedly fire random transitions
*/

#include <cstdio>
#include <cstdlib>
#include "Net.h"
#include "Transition.h"
#include "Place.h"
#include "Marking.h"

void randomWalk(int transitions)
{
    int firedTransitions = 0;

    // repeat forever
    while (true)
    {
        // one iteration is for one execution (either infinite or ending in deadlock)
        while (Transition::CardEnabled > 0)
        {
            // one iteration is for firing a single transition

            // 1. select transition
            index_t y = 0;
            for (index_t z = 0; z < Net::Card[TR]; z++)
            {
                if (Transition::Enabled[z])
                {
                    y++;
                }
            }
            assert(y == Transition::CardEnabled);
            index_t nr = rand() % Transition::CardEnabled; // uniform distribution
            assert(nr < Transition::CardEnabled);
            index_t t; // the transition to be fired
            for (t = 0; t < Net::Card[TR]; ++t)
            {
                if (Transition::Enabled[t])
                {
                    if (nr == 0)
                    {
                        break;
                    }
                    --nr;
                }
            }
            assert(t < Net::Card[TR]);
            assert(Transition::Enabled[t]);
            printf(" firing %s\n", Net::Name[TR][t]);
            Transition::fire(t);
            Transition::updateEnabled(t);

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
            Marking::Current[i] = Marking::Initial[i];
            Place::CardDisabled[i] = 0;
        }
        Marking::HashCurrent = Marking::HashInitial;
        Transition::CardEnabled = Net::Card[TR];
        for (index_t i = 0; i < Net::Card[TR]; i++)
        {
            Transition::Enabled[i] = true;
        }
        for (index_t i = 0; i < Net::Card[TR]; i++)
        {
            Transition::checkEnabled(i);
        }
    }
}
