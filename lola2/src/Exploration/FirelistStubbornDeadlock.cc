/*!
\file Firelist.cc
\author Karsten
\status new

\brief Class for firelist generation. Default is firelist consisting of all enabled transitions.
*/

#include <config.h>
#include <cstdlib>
#include <cstdio>
#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Net/Transition.h>
#include <Exploration/FirelistStubbornDeadlock.h>

/// \todo move initialization to initializer list
FirelistStubbornDeadlock::FirelistStubbornDeadlock()
{
    dfsStack = new index_t[Net::Card[TR]];
    dfs = new index_t[Net::Card[TR]];
    lowlink = new index_t[Net::Card[TR]];
    currentIndex = new index_t[Net::Card[TR]];
    TarjanStack = new index_t[Net::Card[TR]];
    mustBeIncluded = new index_t *[Net::Card[TR]];
    visited = new uint32_t[Net::Card[TR] ]();
    onTarjanStack = new uint32_t[Net::Card[TR] ]();
    stamp = 0;
}

FirelistStubbornDeadlock::~FirelistStubbornDeadlock()
{
    delete[] dfsStack;
    delete[] dfs;
    delete[] lowlink;
    delete[] currentIndex;
    delete[] TarjanStack;
    delete[] mustBeIncluded;
    delete[] visited;
    delete[] onTarjanStack;
}

void FirelistStubbornDeadlock::newStamp()
{
    // 0xFFFFFFFF = max uint32_t
    if (UNLIKELY(++stamp == 0xFFFFFFFF))
    {
        // This happens rarely and only in long runs. Thus it is
        // hard to be tested
        // LCOV_EXCL_START
        for (index_t i = 0; i < Net::Card[TR]; ++i)
        {
            visited[i] = 0;
            onTarjanStack[i] = 0;
        }
        stamp = 1;
        // LCOV_EXCL_STOP
    }
}

index_t FirelistStubbornDeadlock::getFirelist(NetState &ns, index_t **result)
{
    index_t nextDfs = 1;
    index_t stackpointer = 0;
    index_t tarjanstackpointer = 0;
    index_t dfsLastEnabled;
    newStamp();

    // 1. find enabled transition and initialize stacks with it

    // This branch is here only for the case that exploration continues
    // after having found a deadlock. In current LoLA, it cannot happen
    // since check property will raise its flag before firelist is
    // requested
    // LCOV_EXCL_START
    if (UNLIKELY(ns.CardEnabled == 0))
    {
        assert(false);
        * result = new index_t[1];
        return 0;
    }
    // LCOV_EXCL_STOP
    int firstenabled;
    for (firstenabled = 0; !ns.Enabled[firstenabled]; ++firstenabled);
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
                if (ns.Enabled[newtransition])
                {
                    // must include conflicting transitions
                    mustBeIncluded[stackpointer] = Transition::Conflicting[newtransition];
                    currentIndex[stackpointer] = Transition::CardConflicting[newtransition];
                    dfsLastEnabled = nextDfs - 1;
                }
                else
                {
                    index_t scapegoat = ns.Arc[TR][PRE][newtransition][0];
                    // must include pretransitions of scapegoat
                    mustBeIncluded[stackpointer] = ns.Arc[PL][PRE][scapegoat];
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
                        if (ns.Enabled[TarjanStack[i--]])
                        {
                            ++CardStubborn;
                        }
                    }
                    if (ns.Enabled[currenttransition])
                    {
                        ++CardStubborn;
                    }
                    assert(CardStubborn > 0);
                    assert(CardStubborn <= ns.CardEnabled);
                    * result = new index_t [CardStubborn];
                    index_t resultindex = CardStubborn;
                    while (currenttransition != TarjanStack[tarjanstackpointer])
                    {
                        index_t poppedTransition = TarjanStack[tarjanstackpointer--];
                        if (ns.Enabled[poppedTransition])
                        {
                            (*result)[--resultindex] = poppedTransition;
                        }
                    }
                    if (ns.Enabled[currenttransition])
                    {
                        (*result)[--resultindex] = currenttransition;
                    }
                    assert(resultindex == 0);
                    return (CardStubborn);
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

Firelist *FirelistStubbornDeadlock::createNewFireList(SimpleProperty *property)
{
    return new FirelistStubbornDeadlock();
}
