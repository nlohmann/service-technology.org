
#include <Formula/CTL/EUFormula.h>
#include <Exploration/SearchStack.h>
#include <Net/Transition.h>

bool EUFormula::check(Store<void *> &s, NetState &ns, Firelist &firelist,
                      std::vector<int> *witness)
{

    //rep->status("init EU check in %x",ns.HashCurrent);

    void **pInitialPayload;
    if (!s.searchAndInsert(ns, &pInitialPayload, 0))
    {
        // all-zeros is starting state for all values
        *pInitialPayload = ::operator new(payloadsize);
    }
    void *payload = *pInitialPayload;

    CTLFormulaResult cachedResult = getCachedResult(payload);
    if (cachedResult & 2) // value known
    {
        return cachedResult & 1;    // return result
    }
    assert(cachedResult != IN_PROGRESS); // impossible for first state

    // psi -> E phi U psi
    if (psi->check(s, ns, firelist, witness))
    {
        setCachedResult(payload, KNOWN_TRUE);
        return true;
    }

    witness->clear();
    // !phi and !psi -> !E phi U psi
    if (!phi->check(s, ns, firelist, witness))
    {
        setCachedResult(payload, KNOWN_FALSE);
        return false;
    }

    //rep->status("starting EU check in %x",ns.HashCurrent);

    // dfs stack will contain all gray nodes
    SearchStack<DFSStackEntry> dfsStack;

    // tarjan stack will contain all _black_ nodes the SCC of which is not yet finished
    // see doc/Tarjan for an explanation on how and why this works
    SearchStack<void *> tarjanStack;

    statenumber_t currentDFSNumber =
        0; // starting with 1 to leave 0 for recognizing uninitialized values
    statenumber_t currentLowlink;

    // fetch firelist
    index_t *currentFirelist;
    index_t currentFirelistIndex = firelist.getFirelist(ns, &currentFirelist);

    // initialize dfs number, lowlink; mark state to be on tarjan stack
    setDFS(payload, ++currentDFSNumber);
    currentLowlink = currentDFSNumber;
    setCachedResult(payload, IN_PROGRESS);

    bool revertEnabledNeeded = false;

    while (true)
    {
        if (currentFirelistIndex--)
        {
            //rep->status("fire %d (%x %d) from %x",currentFirelist[currentFirelistIndex],currentFirelist,currentFirelistIndex,ns.HashCurrent);
            Transition::fire(ns, currentFirelist[currentFirelistIndex]);
            // don't update enabledness and atomics yet, since it'll maybe not be needed at all.

            void **pNewPayload;
            if (!s.searchAndInsert(ns, &pNewPayload, 0))
            {
                // all-zeros is starting state for all values
                *pNewPayload = ::operator new(payloadsize);
            }
            void *newpayload = *pNewPayload;

            //rep->status("pPayload=%x, payload=%x",pNewPayload,newpayload);
            CTLFormulaResult newCachedResult = getCachedResult(newpayload);
            if (newCachedResult == UNKNOWN)
            {

                // update enabledness and atomic propositions for current state (needed for further checking)
                Transition::updateEnabled(ns, currentFirelist[currentFirelistIndex]);
                updateAtomics(ns, currentFirelist[currentFirelistIndex]);

                witness->clear();
                if (psi->check(s, ns, firelist, witness))
                {
                    setCachedResult(newpayload, KNOWN_TRUE);
                    // we updated the enabledness, so it needs to be reverted
                    revertEnabledNeeded = true;
                    // break; set all nodes to true
                    break;
                }

                witness->clear();
                if (!phi->check(s, ns, firelist, witness))
                {
                    setCachedResult(newpayload, KNOWN_FALSE);
                    // continue;
                    Transition::backfire(ns, currentFirelist[currentFirelistIndex]);
                    //rep->status("backfire %d (%x %d) to %x",currentFirelist[currentFirelistIndex],currentFirelist,currentFirelistIndex,ns.HashCurrent);
                    Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
                    revertAtomics(ns, currentFirelist[currentFirelistIndex]);

                    continue;
                }

                // recursive descent
                new (dfsStack.push()) DFSStackEntry(currentFirelist, currentFirelistIndex, payload, currentLowlink);

                payload = newpayload;
                currentFirelistIndex = firelist.getFirelist(ns, &currentFirelist);

                setDFS(newpayload, ++currentDFSNumber);
                currentLowlink = currentDFSNumber;
                setCachedResult(newpayload, IN_PROGRESS);
                continue;
            }
            else if (newCachedResult == KNOWN_TRUE)
            {
                // break; set all nodes to true
                break;
            }
            else if (newCachedResult == IN_PROGRESS)
            {
                // update lowlink and continue
                statenumber_t newdfs = getDFS(newpayload);
                if (newdfs < currentLowlink)
                {
                    currentLowlink = newdfs;
                }
                Transition::backfire(ns, currentFirelist[currentFirelistIndex]);
                //rep->status("backfire %d (%x %d) to %x",currentFirelist[currentFirelistIndex],currentFirelist,currentFirelistIndex,ns.HashCurrent);
                // enabledness and atomics weren't updated, so no revert needed
                continue;
            }
            else     // KNOWN_FALSE
            {
                // continue;
                Transition::backfire(ns, currentFirelist[currentFirelistIndex]);
                //rep->status("backfire %d (%x %d) to %x",currentFirelist[currentFirelistIndex],currentFirelist,currentFirelistIndex,ns.HashCurrent);
                // enabledness and atomics weren't updated, so no revert needed
                continue;
            }
        }
        else     // if(currentFirelistIndex--)
        {
            // free memory for finished firelist
            delete[] currentFirelist;

            // check if SCC is finished
            statenumber_t dfs = getDFS(payload);
            if (dfs == currentLowlink)
            {
                // all elements on tarjan stack that have a higher dfs number then ours belong to the finished SCC -> formula false
                while (tarjanStack.StackPointer && getDFS(tarjanStack.top()) > dfs)
                {
                    setCachedResult(tarjanStack.top(), KNOWN_FALSE);
                    tarjanStack.pop();
                }
                setCachedResult(payload, KNOWN_FALSE);
            }
            else
            {
                // SCC not yet finished, push self onto tarjan stack
                assert(dfs > currentLowlink);
                *tarjanStack.push() = payload;
            }

            // check if there are any states to backtrack to
            if (dfsStack.StackPointer)
            {
                currentFirelist = dfsStack.top().fl;
                currentFirelistIndex = dfsStack.top().flIndex;
                payload = dfsStack.top().payload;
                if (currentLowlink > dfsStack.top().lowlink) // propagate lowlink to parent
                {
                    currentLowlink = dfsStack.top().lowlink;
                }
                dfsStack.pop();

                Transition::backfire(ns, currentFirelist[currentFirelistIndex]);
                //rep->status("backfire %d (%x %d) to %x",currentFirelist[currentFirelistIndex],currentFirelist,currentFirelistIndex,ns.HashCurrent);
                Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
                revertAtomics(ns, currentFirelist[currentFirelistIndex]);
            }
            else
            {
                assert(!tarjanStack.StackPointer); // tarjan stack empty
                assert(dfs == currentLowlink); // first node is always start of SCC
                assert(*pInitialPayload == payload); // returned to initial state

                witness->clear(); // no witness found
                return false;
            }
        }
    }
    // revert transition that brought us to the witness state
    Transition::backfire(ns, currentFirelist[currentFirelistIndex]);
    //rep->status("backfire %d (%x %d) to %x",currentFirelist[currentFirelistIndex],currentFirelist,currentFirelistIndex,ns.HashCurrent);
    if (revertEnabledNeeded)
    {
        Transition::revertEnabled(ns, currentFirelist[currentFirelistIndex]);
        revertAtomics(ns, currentFirelist[currentFirelistIndex]);
    }

    // add transition to witness path
    //rep->status("push_back %d to %lu",currentFirelist[currentFirelistIndex],witness->size());
    witness->push_back(currentFirelist[currentFirelistIndex]);

    // current state can reach witness state -> formula true
    setCachedResult(payload, KNOWN_TRUE);

    // all elements that are still on tarjan stack can reach this state -> formula true
    while (tarjanStack.StackPointer)
    {
        setCachedResult(tarjanStack.top(), KNOWN_TRUE);
        tarjanStack.pop();
    }

    // free memory for current firelist
    delete[] currentFirelist;

    // all elements that are still on dfs stack (and hence on tarjan stack)
    // can reach this state -> formula true
    // revert all the transitions to restore original NetState
    while (dfsStack.StackPointer)
    {
        setCachedResult(dfsStack.top().payload, KNOWN_TRUE);
        Transition::backfire(ns, dfsStack.top().fl[dfsStack.top().flIndex]);
        //rep->status("backfire %d (%x %d) to %x",dfsStack.top().fl[dfsStack.top().flIndex],dfsStack.top().fl,dfsStack.top().flIndex,ns.HashCurrent);
        Transition::revertEnabled(ns, dfsStack.top().fl[dfsStack.top().flIndex]);
        revertAtomics(ns, dfsStack.top().fl[dfsStack.top().flIndex]);

        witness->push_back(dfsStack.top().fl[dfsStack.top().flIndex]);

        // free memory for stacked firelist
        delete[] dfsStack.top().fl;

        dfsStack.pop();
    }

    // witness path found
    return true;
}
