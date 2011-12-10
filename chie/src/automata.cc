#include <stack>
#include <sstream>
#include <vector>
#include "automata.h"
#include "util.h"
#include "verbose.h"

// message for verbose output
extern std::string globalErrorMessage; // defined in main.cc


/*!
 * \brief get the ID of a channel state
 */
unsigned int getChannelStateID(ProductAutomaton & a, ChannelState & s)
{
  if(a.channelStateCache.count(s) > 0) // if this state is already known
  {
    // return its ID
    return a.channelStateCache[s];
  }

  // otherwise write it to the cache
  a.channelStates[a.nextChannelStateID] = s;
  a.channelStateCache[s] = a.nextChannelStateID;

  // and return its ID
  return a.nextChannelStateID++;
}


/*!
 * \brief get the ID of a product automaton state
 *
 * \note Product automaton state has been split in internal state and channel state
 *       in order to avoid creating the actual state object when calculating successors.
 */
unsigned int getStateID(ProductAutomaton & a, InternalState & s, unsigned int channelStateID)
{
  if((a.stateCache.count(s) > 0) &&
     (a.stateCache[s].count(channelStateID) > 0)) // state already known
  {
    return a.stateCache[s][channelStateID];
  }

  // otherwise write state to the cache
  a.stateCache[s][channelStateID] = a.nextStateID;
  a.states[a.nextStateID] = ProductState(s, channelStateID);

  // and return its ID
  return a.nextStateID++;
}


/*!
 * \brief constructor
 */
ProductState::ProductState(InternalState & is, unsigned int iid) :
    internalState(is), interfaceStateID(iid), dfs(0), lowlink(0)
{
}

/*!
 * \brief standard constructor
 */
ProductState::ProductState()
{
}

/*!
 * \brief copy constructor
 */
ProductState::ProductState(const ProductState & cc) :
    internalState(cc.internalState), interfaceStateID(cc.interfaceStateID),
    dfs(cc.dfs), lowlink(cc.lowlink)
{
}


/*!
 * \brief create the product automaton of two service automata
 *
 * \param specification service automaton of the specification
 * \param testCase service automaton of the test case
 * \param result product automaton object the result will be written to
 *
 * \note ID counters start at 1
 */
bool createProductAutomaton(ServiceAutomaton & specification, ServiceAutomaton & testCase, ProductAutomaton & result)
{
  // create event mapping
  result.events.push_back("TAU");
  FOREACH(event, specification.isSendingEvent)
  {
    result.events2IDs[event->first] = result.events.size();
    result.events.push_back(event->first);
  }

  // DFS stacks
  std::stack<unsigned int> toDoStack; // scheduled states
  std::stack<unsigned int> dfsStack; // actual DFS stack
  std::vector<unsigned int> tarjanStack; // "stack" to compute SCC

  // state computing variables
  ChannelState channelState; // currently channel state
  unsigned int channelStateID = getChannelStateID(result, channelState); // ... and its ID
  InternalState internalState(specification.initialState, testCase.initialState); // internal state
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
    if(specification.noSuccessorStates.count(currentState->internalState.first) > 0) // if service automaton has no successor
    {
      result.noSpecificationSuccessorStates.insert(currentStateID); // add this state to the list
    }
    if(testCase.noSuccessorStates.count(currentState->internalState.second) > 0) // if test case automaton has no successor
    {
      result.noTestCaseSuccessorStates.insert(currentStateID); // add this state to the list
    }


    bool successorFound = false; // check whether this state has a successor

    // successors when first automaton does a step
    std::map<std::string, std::set<unsigned int> > successors = specification.stateSpace[currentState->internalState.first];
    FOREACH(succ, successors)
    {
      FOREACH(succ_, succ->second)
      {
        // divide current state in internal state and channel state
        channelState = result.channelStates[channelStateID = currentState->interfaceStateID];
        internalState = currentState->internalState;

        if(specification.isSendingEvent.count(succ->first) == 0) // internal step
        {
          internalState.first = *succ_;
          transitionType = INTERNAL;
        }
        else
        {
          if(specification.isSendingEvent[succ->first]) // sending event
          {
            internalState.first = *succ_;
            channelState.insert(result.events2IDs[succ->first]);
            transitionType = SENDING;
          }
          else // receive event
          {
            // check, whether message is on the channel
            if(channelState.count(result.events2IDs[succ->first]) > 0)
            {
              internalState.first = *succ_;
              channelState.erase(channelState.find(result.events2IDs[succ->first])); // remove only one message
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
    successors = testCase.stateSpace[currentState->internalState.second];
    FOREACH(succ, successors)
    {
      FOREACH(succ_, succ->second)
      {
        // divide current state in internal state and channel state
        channelState = result.channelStates[channelStateID = currentState->interfaceStateID];
        internalState = currentState->internalState;

        if(testCase.isSendingEvent.count(succ->first) == 0) // internal step
        {
          internalState.second = *succ_;
          transitionType = INTERNAL;
        }
        else
        {
          if(testCase.isSendingEvent[succ->first]) // sending event
          {
            internalState.second = *succ_;
            channelState.insert(result.events2IDs[succ->first]);
            transitionType = SENDING;
          }
          else // receive event
          {
            // check, whether message is on the channel
            if(channelState.count(result.events2IDs[succ->first]) > 0)
            {
              internalState.second = *succ_;
              channelState.erase(channelState.find(result.events2IDs[succ->first])); // remove only one message
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
      // avoid counting twice
      bool cond1 = (specification.finalStates.count(currentState->internalState.first) == 0); // specification state is no final state
      bool cond2 = (testCase.finalStates.count(currentState->internalState.second) == 0); // test case state is no final state

      // no successor found, so both the specification automaton state and the test case automaton state must be final states
      if(cond1 || cond2)
      {
        result.badNode = currentStateID; // mark this state as bad node in product automaton

        // generate error message
        std::stringstream ss;
        ss << "node " << currentStateID << " with specification state " << (currentState->internalState.first)
           << " and test case state " << (currentState->internalState.second) << " has no successors but ";
        if(cond1)
        {
          ss << "specification automaton";
        }
        if(cond1 && cond2)
        {
          ss << " and ";
        }
        if(cond2)
        {
          ss << "test case automaton";
        }
        ss << ((cond1 && cond2) ? " are" : " is") << " not in a final state";
        globalErrorMessage = ss.str();

        // return result
        return false;
      }
    }
  }

  // finished building product automaton; no error occurred
  return true;
}
