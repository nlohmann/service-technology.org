#include <stack>
#include <vector>
#include "automata.h"
#include "util.h"
#include "verbose.h"

#include <iostream>
using std::cerr;
using std::endl;

std::set<std::string> ServiceAutomaton::getSendingEvents()
{
  std::set<std::string> result;

  FOREACH(event, isSendingEvent)
  {
    if(event->second)
    {
      result.insert(event->first);
    }
  }
  return result;
}


unsigned int getChannelStateID(ProductAutomaton & a, ChannelState & s)
{
  static unsigned int nextID = 1;

  if(a.channelStateCache.count(s) > 0)
  {
    return a.channelStateCache[s];
  }

  a.channelStates[nextID] = s;
  a.channelStateCache[s] = nextID;

  return nextID++;
}

unsigned int getStateID(ProductAutomaton & a, InternalState & s, unsigned int channelStateID)
{
  static unsigned int nextID = 2;

  if((a.stateCache.count(s) > 0) &&
     (a.stateCache[s].count(channelStateID) > 0)) // state already known
  {
    return a.stateCache[s][channelStateID];
  }

  a.stateCache[s][channelStateID] = nextID;
  a.states[nextID] = ProductState(s, channelStateID);

  return nextID++;
}

ProductState::ProductState(InternalState & is, unsigned int iid) :
    internalState(is), interfaceStateID(iid), dfs(0), lowlink(0)
{
}

// standard constructor
ProductState::ProductState()
{
}

// copy constructor
ProductState::ProductState(const ProductState & cc) :
    internalState(cc.internalState), interfaceStateID(cc.interfaceStateID),
    dfs(cc.dfs), lowlink(cc.lowlink)
{
}

/*!
 * \note ID counters start at 1
 */
void createProductAutomaton(ServiceAutomaton & serviceAutomaton, ServiceAutomaton & testCaseAutomaton, ProductAutomaton & result)
{
  // DFS stacks
  std::stack<unsigned int> toDoStack; // scheduled states
  std::stack<unsigned int> dfsStack; // actual DFS stack
  std::vector<unsigned int> tarjanStack; // "stack" to compute SCC

  // state computing variables
  ChannelState channelState; // currently channel state
  unsigned int channelStateID = getChannelStateID(result, channelState); // ... and its ID
  InternalState internalState(serviceAutomaton.initialState, testCaseAutomaton.initialState); // internal state
  ProductState * currentState = NULL; // resulting product state
  unsigned int currentStateID = 0; // ... and its ID
  TransitionType transitionType;
  unsigned int stateID;

  // initialization
  result.states[1] = ProductState(internalState, channelStateID);
  result.stateCache[internalState][1] = 1;
  unsigned int maxDfs = 1;
  toDoStack.push(1);

  // perform DFS
  while(toDoStack.size() > 0)
  {
    // get next state to process
    currentState = &result.states[currentStateID = toDoStack.top()];

    /*
    cerr << "current state: " << currentStateID << ": [" << currentState->internalState.first << "|" << currentState->internalState.second << "|{";
    FOREACH(m, result.channelStates[currentState->interfaceStateID])
    {
      cerr << *m << ",";
    }
    cerr << "}]" << endl;
    //*/

    if(currentState->dfs > 0) // state already seen
    {
      if(toDoStack.top() == dfsStack.top()) // all successors of this State currently seen (i.e. we are now backtracking)
      {
        if(currentState->lowlink == currentState->dfs) // SCC found
        {
          // collect SCC members; use lowlink as SCC ID
          while(tarjanStack.back() != currentStateID)
          {
            result.SCCs[currentState->lowlink].insert(tarjanStack.back());
            tarjanStack.pop_back(); // remove SCC from Tarjan stack
          }
          result.SCCs[currentState->lowlink].insert(currentStateID);
          tarjanStack.pop_back(); // remove last SCC node from Tarjan stack
        }

        dfsStack.pop(); // backtracking
        if(dfsStack.size() > 0) // if DFS is not finished yet (i.e. if this was a recursive step)
        {
          // set parent's lowlink
          ProductState * parentState = &result.states[dfsStack.top()];
          parentState->lowlink = (parentState->lowlink < currentState->lowlink) ? parentState->lowlink : currentState->lowlink;
        }
      }
      else // this state was reached by a non-tree-arc
      {
        // check whether this state is on the Tarjan stack
        bool found = false;
        for(int i = 0; i < tarjanStack.size(); ++i)
        {
          if(tarjanStack[i] == currentStateID)
          {
            found = true;
            break;
          }
        }
        if(found) // this state is on the Tarjan stack
        {
          // set parent's lowlink
          ProductState * parentState = &result.states[dfsStack.top()];
          parentState->lowlink = (parentState->lowlink < currentState->dfs) ? parentState->lowlink : currentState->dfs;
        }
      }

      toDoStack.pop(); // remove state from processing stack
      continue; // skip further steps for this state
    } // else: "white" node

    // actual processing of this state
    currentState->dfs = currentState->lowlink = maxDfs++;
    dfsStack.push(currentStateID); // one step deeper
    tarjanStack.push_back(currentStateID);

    // "no successor" stuff
    if(serviceAutomaton.noSuccessorStates.count(currentState->internalState.first) > 0) // if service automaton has no successor
    {
      result.noServiceSuccessorStates.insert(currentStateID); // add this state to the list
    }
    if(testCaseAutomaton.noSuccessorStates.count(currentState->internalState.second) > 0) // if test case automaton has no successor
    {
      result.noTestCaseSuccessorStates.insert(currentStateID); // add this state to the list
    }


    bool successorFound = false; // check whether this state has a successor

    // successors when first automaton does a step
    std::map<std::string, std::set<unsigned int> > successors = serviceAutomaton.stateSpace[currentState->internalState.first];
    FOREACH(succ, successors)
    {
      FOREACH(succ_, succ->second)
      {
        // divide current state in internal state and channel state
        channelState = result.channelStates[channelStateID = currentState->interfaceStateID];
        internalState = currentState->internalState;

        if(serviceAutomaton.isSendingEvent.count(succ->first) == 0) // internal step
        {
          internalState.first = *succ_;
          transitionType = INTERNAL;
        }
        else
        {
          if(serviceAutomaton.isSendingEvent[succ->first]) // sending event
          {
            internalState.first = *succ_;
            channelState.insert(succ->first);
            transitionType = SENDING;
          }
          else // receive event
          {
            // check, whether message is on the channel
            if(channelState.count(succ->first) > 0)
            {
              internalState.first = *succ_;
              channelState.erase(channelState.find(succ->first)); // remove only one message
              transitionType = RECEIVING;
            }
            else
            {
              // can not perform this step
              continue;
            }
          }
        }

        // successor found
        successorFound = true;

        // get ID of new state
        stateID = getStateID(result, internalState, getChannelStateID(result, channelState));
        // store successor "arc"
        result.transitions[currentStateID][succ->first].insert(Transition(transitionType, stateID));
        // store predecessor "arc"
        result.predecessors[stateID].insert(currentStateID);
        // schedule this state
        toDoStack.push(stateID);
      }
    }

    // successors when second automaton does a step
    successors = testCaseAutomaton.stateSpace[currentState->internalState.second];
    FOREACH(succ, successors)
    {
      FOREACH(succ_, succ->second)
      {
        // divide current state in internal state and channel state
        channelState = result.channelStates[channelStateID = currentState->interfaceStateID];
        internalState = currentState->internalState;

        if(testCaseAutomaton.isSendingEvent.count(succ->first) == 0) // internal step
        {
          internalState.second = *succ_;
          transitionType = INTERNAL;
        }
        else
        {
          if(testCaseAutomaton.isSendingEvent[succ->first]) // sending event
          {
            internalState.second = *succ_;
            channelState.insert(succ->first);
            transitionType = SENDING;
          }
          else // receive event
          {
            // check, whether message is on the channel
            if(channelState.count(succ->first) > 0)
            {
              internalState.second = *succ_;
              channelState.erase(channelState.find(succ->first)); // remove only one message
              transitionType = RECEIVING;
            }
            else
            {
              // can not perform this step
              continue;
            }
          }
        }

        // successor found
        successorFound = true;

        // get ID of new state
        stateID = getStateID(result, internalState, getChannelStateID(result, channelState));
        // store successor "arc"
        result.transitions[currentStateID][succ->first].insert(Transition(transitionType, stateID));
        // store predecessor "arc"
        result.predecessors[stateID].insert(currentStateID);
        // schedule this state
        toDoStack.push(stateID);
      }
    }

    if(!successorFound)
    {
      // no successor found
      result.noSuccessorStates.insert(currentStateID);
    }
  }
}
