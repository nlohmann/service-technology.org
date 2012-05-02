/*!
\file Firelist.cc
\author Karsten
\status new

\brief Class for firelist generation. Default is firelist consisting of all enabled transitions.
*/

#include <cstdlib>
#include "Core/Dimensions.h"
#include "Net/Net.h"
#include "Net/Transition.h"
#include "Exploration/FirelistStubbornDeadlock.h"

FirelistStubbornDeadlock::FirelistStubbornDeadlock()
{
    dfsStack = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    dfs = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    lowlink = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    currentIndex = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    TarjanStack = (index_t*) malloc(Net::Card[TR] * SIZEOF_INDEX_T);
    mustBeIncluded = (index_t**) malloc(Net::Card[TR] * SIZEOF_VOIDP);
    visited = (uint32_t*) calloc(Net::Card[TR] , sizeof(uint32_t));
    onTarjanStack = (uint32_t*) calloc(Net::Card[TR] , sizeof(uint32_t));
    stamp = 0;
}

uint32_t FirelistStubbornDeadlock::newStamp()
{
    if (++stamp == 0xFFFFFFFF)
    {
        for (index_t i = 0; i < Net::Card[TR]; ++i)
        {
            visited[i] = 0;
            onTarjanStack[i] = 0;
        }
        stamp = 1;
    }
}

index_t FirelistStubbornDeadlock::getFirelist(index_t** result)
{

    index_t nextDfs = 1;
    index_t stackpointer = 0;
    index_t tarjanstackpointer = 0;
    index_t dfsLastEnabled;
    newStamp();

    // 1. find enabled transition and initialize stacks with it

    if (Transition::CardEnabled == 0)
    {
        * result = new index_t[1];
        return 0;
    }
    int firstenabled;
    for (firstenabled = 0; !Transition::Enabled[firstenabled]; ++firstenabled);
    // Transition #firstenabled is root for search
    dfs[firstenabled] = lowlink[firstenabled] = 0;
    dfsStack[0] = TarjanStack[0] = firstenabled;
    visited[firstenabled] = onTarjanStack[firstenabled] = stamp;
    mustBeIncluded[0] = Transition::Conflicting[firstenabled];
    dfsLastEnabled = 0;
    currentIndex[0] = Transition::CardConflicting[firstenabled];
    index_t currenttransition;

    // depth first search
    // quit when scc with enabled transitions is found.
    while (true)
    {
        currenttransition = dfsStack[stackpointer];
        if ((currentIndex[stackpointer]) > 0)
        {
            // current transition has another successor
            index_t newtransition = mustBeIncluded[stackpointer][--(currentIndex[stackpointer])];
            if (visited[newtransition] == stamp)
            {
                // transition already seen
                // update lowlink of currenttransition
                if (onTarjanStack[newtransition] == stamp && dfs[newtransition] < lowlink[currenttransition])
                {
                    lowlink[currenttransition] = dfs[newtransition];
                }
            }
            else
            {
                // transition not yet seen
                dfs[newtransition] = lowlink[newtransition] = nextDfs++;
                visited[newtransition] = onTarjanStack[newtransition] = stamp;
                dfsStack[++stackpointer] = newtransition;
                TarjanStack[++tarjanstackpointer] = newtransition;
                if (Transition::Enabled[newtransition])
                {
                    // must include conflicting transitions
                    mustBeIncluded[stackpointer] = Transition::Conflicting[newtransition];
                    currentIndex[stackpointer] = Transition::CardConflicting[newtransition];
                    dfsLastEnabled = nextDfs - 1;
                }
                else
                {
                    index_t scapegoat = Net::Arc[TR][PRE][newtransition][0];
                    // must include pretransitions of scapegoat
                    mustBeIncluded[stackpointer] = Net::Arc[PL][PRE][scapegoat];
                    currentIndex[stackpointer] = Net::CardArcs[PL][PRE][scapegoat];
                }
            }
        }
        else
        {
            // current transition does not have another successor

            // check for closed scc
            if (dfs[currenttransition] == lowlink[currenttransition])
            {
                // scc closed
                // check whether scc contains enabled transitions
                if (dfsLastEnabled >= dfs[currenttransition])
                {
                    // build firelist from current scc,
                    // pop all other scc for resetting data structures

                    index_t CardStubborn = 0;
                    for (index_t i = tarjanstackpointer; TarjanStack[i] != currenttransition;)
                    {
                        if (Transition::Enabled[TarjanStack[i--]])
                        {
                            ++CardStubborn;
                        }
                    }
                    if (Transition::Enabled[currenttransition])
                    {
                        ++CardStubborn;
                    }
                    assert(CardStubborn > 0);
                    assert(CardStubborn <= Transition::CardEnabled);
                    * result = new index_t [CardStubborn];
                    index_t resultindex = 0;
                    while (currenttransition != TarjanStack[tarjanstackpointer])
                    {
                        index_t poppedTransition = TarjanStack[tarjanstackpointer--];
                        if (Transition::Enabled[poppedTransition])
                        {
                            (*result)[resultindex++] = poppedTransition;
                        }
                    }
                    if (Transition::Enabled[currenttransition])
                    {
                        (*result)[resultindex++] = currenttransition;
                    }
                    assert(resultindex == CardStubborn);
                    return(CardStubborn);
                }
                else
                {
                    // no enabled transitions
                    // pop current scc from tarjanstack and continue
                    while (currenttransition != TarjanStack[tarjanstackpointer--])
                    {
                    }
                    assert(stackpointer > 0);
                    --stackpointer;
                    if (lowlink[currenttransition] < lowlink[dfsStack[stackpointer]])
                    {
                        lowlink[dfsStack[stackpointer]] = lowlink[currenttransition];
                    }

                }
            }
            else
            {
                // scc not closed
                assert(stackpointer > 0);
                --stackpointer;
                if (lowlink[currenttransition] < lowlink[dfsStack[stackpointer]])
                {
                    lowlink[dfsStack[stackpointer]] = lowlink[currenttransition];
                }
            }
        }
    }
}
