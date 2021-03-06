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
#include <Exploration/StatePredicateProperty.h>

FirelistStubbornStatePredicate::FirelistStubbornStatePredicate(StatePredicate *p)
{
    predicate = p;
    dfsStack = new index_t[Net::Card[TR]];
    onStack = new bool[Net::Card[TR]]();
}

FirelistStubbornStatePredicate::~FirelistStubbornStatePredicate()
{
    delete[] dfsStack;
    delete[] onStack;
}

index_t FirelistStubbornStatePredicate::getFirelist(NetState &ns, index_t **result)
{
    if (ns.CardEnabled == 0)
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
        index_t *mustbeincluded;
        index_t  cardmustbeincluded;
        if (ns.Enabled[currenttransition])
        {
            ++cardEnabled;
            mustbeincluded = Transition::Conflicting[currenttransition];
            cardmustbeincluded = Transition::CardConflicting[currenttransition];
        }
        else
        {
            const index_t scapegoat = ns.Arc[TR][PRE][currenttransition][0];
            mustbeincluded = ns.Arc[PL][PRE][scapegoat];
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
        if (ns.Enabled[t])
        {
            (*result)[--cardEnabled] = t;
        }
        onStack[t] = false;
    }
    return size;
}

Firelist *FirelistStubbornStatePredicate::createNewFireList(SimpleProperty *property)
{
    return new FirelistStubbornStatePredicate(((StatePredicateProperty *)property)->getPredicate());
}
