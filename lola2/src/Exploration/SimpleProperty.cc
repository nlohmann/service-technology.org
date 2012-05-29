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

bool SimpleProperty::find_path(unsigned int attempts, unsigned int maxdepth, Firelist &myFirelist, EmptyStore &s)
{
    // this table counts hits for various hash buckets. This is used for steering
    // search into less frequently entered areas of the state space.
    unsigned long int* hashtable = (unsigned long int*) calloc(SIZEOF_MARKINGTABLE, sizeof(unsigned long int));

    unsigned int currentattempt = 0;

    // copy initial marking into current marking
    memcpy(Marking::Current, Marking::Initial, Net::Card[PL] * SIZEOF_INDEX_T);
    Marking::HashCurrent = Marking::HashInitial;

    // prepare property
    initProperty();

    if (value)
    {
        // initial marking satisfies property
        free(hashtable);
        return true;
    }

    // get first firelist
    index_t* currentFirelist;
    index_t currentEntry = myFirelist.getFirelist(&currentFirelist);

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
            free(hashtable);
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

            // 1. select transition
            // Selection proceeds in two phases. In phase one, we give priority to transitions
            // that 1. enter rarely visited hash buckets and 2. are early members of the fire list
            // If no transition is selected in phase 1, phase 2 selects a transition randomly.

            index_t chosen = Net::Card[TR];
            // phase 1
            for (index_t i = cardFirelist; i > 0;)
            {
                --i;
                index_t t = currentFirelist[i];
                // compute hash value for t successor
                hash_t h = (Marking::HashCurrent + Transition::DeltaHash[t]) % SIZEOF_MARKINGTABLE;
                if (((float) rand() / (float) RAND_MAX) <= 1.0 / (1.0 + hashtable[h]))
                {
                    chosen = t;
                    break;
                }
            }

            // phase 2
            if (chosen == Net::Card[TR])
            {
                chosen = currentFirelist[ rand() % cardFirelist];
            }
            free(currentFirelist);

            Transition::fire(chosen);
            ++(hashtable[Marking::HashCurrent]);
            Transition::updateEnabled(chosen);

            checkProperty(chosen);
            if (value)
            {
                free(hashtable);
                return true;
            }
        }
    }

    free(hashtable);
    return false;
}
