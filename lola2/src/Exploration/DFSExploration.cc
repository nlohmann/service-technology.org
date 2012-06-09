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
#include <Exploration/DFSExploration.h>
#include <Stores/Store.h>
#include <Stores/EmptyStore.h>
#include <InputOutput/Reporter.h>
#include <cmdline.h>

extern gengetopt_args_info args_info;
extern Reporter* rep;


bool DFSExploration::depth_first(SimpleProperty& property,Store &myStore, FireListCreator& firelistcreator, int threadNumber)
{
	Firelist &myFirelist = *firelistcreator.createFireList(&property);
    // copy initial marking into current marking
    memcpy(Marking::Current, Marking::Initial, Net::Card[PL] * SIZEOF_INDEX_T);
    Marking::HashCurrent = Marking::HashInitial;

	NetState* ns = NetState::createNetStateFromCurrent();

    // prepare property
    property.value = property.initProperty(*ns);

    if (property.value)
    {
        // initial marking satisfies property
        free(Marking::Current);
        Marking::Current = ns->Current;
        myStore.finalize();
        return true;
    }

    // add initial marking to store
    // we do not care about return value since we know that store is empty

    myStore.searchAndInsert(ns);

    // get first firelist
    index_t* currentFirelist;
    index_t currentEntry = myFirelist.getFirelist(ns,&currentFirelist);

    while (true) // exit when trying to pop from empty stack
    {
        if (currentEntry-- > 0)
        {
            // there is a next transition that needs to be explored in current marking

            // fire this transition to produce new Marking::Current
            Transition::fire(ns,currentFirelist[currentEntry]);

            if (myStore.searchAndInsert(ns))
            {
                // State exists! -->backtracking to previous state
                Transition::backfire(ns,currentFirelist[currentEntry]);
            }
            else
            {
                // State does not exist!

                Transition::updateEnabled(ns,currentFirelist[currentEntry]);
                // check current marking for property
                property.value = property.checkProperty(*ns,currentFirelist[currentEntry]);
                if (property.value)
                {
                    // current  marking satisfies property
                    // push put current transition on stack
                    // this way, the stack contains ALL transitions
                    // of witness path
                	property.stack.push(currentEntry, currentFirelist);
                    free(Marking::Current);
                    Marking::Current = ns->Current;
                    myStore.finalize();
                   return true;
                }

                // Here: current marking does not satisfy property --> continue search
                property.stack.push(currentEntry, currentFirelist);
                currentEntry = myFirelist.getFirelist(ns,&currentFirelist);
            } // end else branch for "if state exists"
        }
        else
        {
            // firing list completed -->close state and return to previous state
            delete [] currentFirelist;
            if (property.stack.StackPointer == 0)
            {
                // have completely processed initial marking --> state not found
                free(Marking::Current);
                Marking::Current = ns->Current;
                myStore.finalize();
                return false;
            }
            property.stack.pop(&currentEntry, &currentFirelist);
            assert(currentEntry < Net::Card[TR]);
            Transition::backfire(ns,currentFirelist[currentEntry]);
            Transition::revertEnabled(ns,currentFirelist[currentEntry]);
            property.value = property.updateProperty(*ns,currentFirelist[currentEntry]);
        }
    }
}

bool DFSExploration::find_path(SimpleProperty& property, unsigned int attempts, unsigned int maxdepth, Firelist &myFirelist, EmptyStore &s, ChooseTransition &c)
{
    // this table counts hits for various hash buckets. This is used for steering
    // search into less frequently entered areas of the state space.

	NetState* ns = NetState::createNetStateFromCurrent();
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
        memcpy(ns->Current, Marking::Initial, Net::Card[PL] * SIZEOF_INDEX_T);
        ns->HashCurrent = Marking::HashInitial;

        // reset enabledness information
        for (index_t i = 0; i < Net::Card[PL]; i++)
        {
        	ns->Current[i] = Marking::Initial[i];
        	ns->CardDisabled[i] = 0;
        }
        ns->CardEnabled = Net::Card[TR];
        for (index_t t = 0; t < Net::Card[TR]; ++t)
        {
        	ns->Enabled[t] = true;
        }

        for (index_t t = 0; t < Net::Card[TR]; ++t)
        {
            Transition::checkEnabled(ns,t);
        }

        // prepare property
        property.value = property.initProperty(*ns);

        if (property.value)
        {
            // initial marking satisfies property
            // witness path is empty path
            // initial marking satisfies property
            free(Marking::Current);
            Marking::Current = ns->Current;

            return true;
        }


        // generate a firing sequence until given depth or deadlock is reached
        for (index_t depth = 0; depth < maxdepth; ++depth)
        {
            // register this transition's firing
            s.searchAndInsert(ns);

            // get firelist
            index_t* currentFirelist;
            index_t cardFirelist = myFirelist.getFirelist(ns,&currentFirelist);
            if (!cardFirelist)
            {
                // deadlock or empty up set (i.e. property not reachable)
                break; // go to next attempt
            }

            index_t chosen = c.choose(ns,cardFirelist, currentFirelist);
            free(currentFirelist);

            path[depth] = chosen;
            Transition::fire(ns,chosen);
            Transition::updateEnabled(ns,chosen);

            property.value = property.checkProperty(*ns,chosen);
            if (property.value)
            {
                // witness path is path[0] .... path[depth-1] path[depth]
                // initial marking satisfies property
                free(Marking::Current);
                Marking::Current = ns->Current;

                return true;
            }
        }
    }
    // initial marking satisfies property
    free(Marking::Current);
    Marking::Current = ns->Current;
    return false;
}
