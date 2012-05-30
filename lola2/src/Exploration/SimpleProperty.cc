/*!
\file SimpleProperty.cc
\author Karsten
\status approved 23.05.2012

\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is virtual, default (base class) is full exploration
*/

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <Net/Marking.h>
#include <Net/Place.h>
#include <Net/Transition.h>
#include <Net/Net.h>
#include <Exploration/SimpleProperty.h>
#include <Exploration/Firelist.h>
#include <Exploration/ChooseTransition.h>

#include <Stores/Store.h>
#include <Stores/EmptyStore.h>
#include <InputOutput/Reporter.h>
#include <cmdline.h>

extern gengetopt_args_info args_info;
extern Reporter* rep;

void SimpleProperty::initProperty()
{
    value = false;
}

bool SimpleProperty::depth_first(Store &myStore, Firelist &myFirelist)
{
    // copy initial marking into current marking
    memcpy(Marking::Current, Marking::Initial, Net::Card[PL] * SIZEOF_INDEX_T);
    Marking::HashCurrent = Marking::HashInitial;

    // prepare property
    initProperty();

    if (value)
    {
        // initial marking satisfies property
        return true;
    }

    // add initial marking to store
    // we do not care about return value since we know that store is empty

    myStore.searchAndInsert();

    // get first firelist
    index_t* currentFirelist;
    index_t currentEntry = myFirelist.getFirelist(&currentFirelist);

    while (true) // exit when trying to pop from empty stack
    {
        if (currentEntry-- > 0)
        {
            // there is a next transition that needs to be explored in current marking

            // fire this transition to produce new Marking::Current
            Transition::fire(currentFirelist[currentEntry]);

            if (myStore.searchAndInsert())
            {
                // State exists! -->backtracking to previous state
                Transition::backfire(currentFirelist[currentEntry]);
            }
            else
            {
                // State does not exist!

                Transition::updateEnabled(currentFirelist[currentEntry]);
                // check current marking for property
                checkProperty(currentFirelist[currentEntry]);
                if (value)
                {
                    // current  marking satisfies property
                    // push put current transition on stack
                    // this way, the stack contains ALL transitions
                    // of witnss path
                    stack.push(currentEntry, currentFirelist);
                    return true;
                }

                // Here: current marking does not satisfy property --> continue search
                stack.push(currentEntry, currentFirelist);
                currentEntry = myFirelist.getFirelist(&currentFirelist);
            } // end else branch for "if state exists"
        }
        else
        {
            // firing list completed -->close state and return to previous state
            delete [] currentFirelist;
            if (stack.StackPointer == 0)
            {
                // have completely processed initial marking --> state not found
                return false;
            }
            stack.pop(&currentEntry, &currentFirelist);
            assert(currentEntry < Net::Card[TR]);
            Transition::backfire(currentFirelist[currentEntry]);
            Transition::revertEnabled(currentFirelist[currentEntry]);
            updateProperty(currentFirelist[currentEntry]);
        }
    }
}

bool SimpleProperty::find_path(unsigned int attempts, unsigned int maxdepth, Firelist &myFirelist, EmptyStore &s, ChooseTransition &c)
{
    // this table counts hits for various hash buckets. This is used for steering
    // search into less frequently entered areas of the state space.

    unsigned int currentattempt = 0;

    // get memory for path info
    index_t* path = (index_t*) malloc(SIZEOF_INDEX_T * maxdepth);
    //    // copy initial marking into current marking
    //    memcpy(Marking::Current, Marking::Initial, Net::Card[PL] * SIZEOF_INDEX_T);
    //    Marking::HashCurrent = Marking::HashInitial;
    //
    //    // prepare property
    //    initProperty();
    //
    //    if (value)
    //    {
    //        // initial marking satisfies property
    //        return true;
    //    }
    //
    //    // get first firelist
    //    index_t* currentFirelist;
    //    index_t currentEntry = myFirelist.getFirelist(&currentFirelist);

    // loop #attempts times
    while (attempts == 0 || currentattempt++ < attempts)
    {
        // register this try
        s.tries++;

        // copy initial marking into current marking
        memcpy(Marking::Current, Marking::Initial, Net::Card[PL] * SIZEOF_INDEX_T);
        Marking::HashCurrent = Marking::HashInitial;

        // reset enabledness information
        for (index_t i = 0; i < Net::Card[PL]; i++)
        {
            Marking::Current[i] = Marking::Initial[i];
            Place::CardDisabled[i] = 0;
        }
        Transition::CardEnabled = Net::Card[TR];
        for (index_t t = 0; t < Net::Card[TR]; ++t)
        {
            Transition::Enabled[t] = true;
        }

        for (index_t t = 0; t < Net::Card[TR]; ++t)
        {
            Transition::checkEnabled(t);
        }

        // prepare property
        initProperty();

        if (value)
        {
            // initial marking satisfies property
            // witness path is empty path
            return true;
        }


        // generate a firing sequence until given depth or deadlock is reached
        for (index_t depth = 0; depth < maxdepth; ++depth)
        {
            // register this transition's firing
            s.searchAndInsert();

            // get firelist
            index_t* currentFirelist;
            index_t cardFirelist = myFirelist.getFirelist(&currentFirelist);
            if (!cardFirelist)
            {
                // deadlock or empty up set (i.e. property not reachable)
                break; // go to next attempt
            }

            index_t chosen = c.choose(cardFirelist, currentFirelist);
            free(currentFirelist);

            path[depth] = chosen;
            Transition::fire(chosen);
            Transition::updateEnabled(chosen);

            checkProperty(chosen);
            if (value)
            {
                // witness path is path[0] .... path[depth-1] path[depth]
                return true;
            }
        }
    }
    return false;
}
