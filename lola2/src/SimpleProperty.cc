/*!
\file SimpleProperty.cc
\author Karsten
\status new
\brief Evaluates simple property (only SET of states needs to be computed).
Actual property is virtual, default (base class) is full exploration
*/

#include <cstring>
#include <cstdio>

#include "SimpleProperty.h"
#include "SearchStack.h"
#include "Store.h"
#include "Firelist.h"
#include "Marking.h"
#include "Transition.h"
#include "Net.h"


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
    // initialize search stack
    SearchStack stack;

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
