/*!
\file FirelistStubbornStatePredicate.cc
\author Karsten
\status new

\brief Class for firelist generation. Use up sets for state predicate
*/

#include <cstdlib>
#include <cstdio>
#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <Net/Marking.h>
#include <Exploration/FirelistStubbornStatePredicate.h>

FirelistStubbornStatePredicate::FirelistStubbornStatePredicate(StatePredicate* p)
{
    predicate = p;
    dfsStack = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    onStack = (bool*) calloc(Net::Card[TR] , sizeof(bool));
}

FirelistStubbornStatePredicate::~FirelistStubbornStatePredicate()
{
    free(dfsStack);
    free(onStack);
}

index_t FirelistStubbornStatePredicate::getFirelist(index_t** result)
{


    if (Transition::CardEnabled == 0)
    {
        * result = new index_t[1];
        return 0;
    }

    index_t stackpointer = predicate->getUpSet(dfsStack, onStack);
    index_t cardEnabled = 0;



    // loop until all stack elements processed
    for (index_t firstunprocessed = 0; firstunprocessed < stackpointer; ++firstunprocessed)
    {
        index_t currenttransition = dfsStack[firstunprocessed];
        index_t* mustbeincluded;
        index_t  cardmustbeincluded;
        if (Transition::Enabled[currenttransition])
        {
            ++cardEnabled;
            mustbeincluded = Transition::Conflicting[currenttransition];
            cardmustbeincluded = Transition::CardConflicting[currenttransition];
        }
        else
        {
            const index_t scapegoat = Net::Arc[TR][PRE][currenttransition][0];
            mustbeincluded = Net::Arc[PL][PRE][scapegoat];
            cardmustbeincluded = Net::CardArcs[PL][PRE][scapegoat];
        }
        for (index_t i = 0; i < cardmustbeincluded; ++i)
        {
            const index_t t = mustbeincluded[i];
            if (!onStack[t])
            {
                dfsStack[stackpointer++] = t;
                onStack[t] = true;
            }
        }

    }
    index_t size = cardEnabled;
    * result = new index_t [cardEnabled];
    for (index_t i = 0; i < stackpointer; ++i)
    {
        const index_t t = dfsStack[i];
        if (Transition::Enabled[t])
        {
            (*result)[--cardEnabled] = t;
        }
        onStack[t] = false;
    }
    return size;
}
