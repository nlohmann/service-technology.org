#include <config.h> // assertions
#include "conformanceCheck.h"
#include "util.h"
#include <queue>
#include <stack>
#include <sstream>
#include "cmdline.h"
#include "verbose.h"

#include <iostream>
using std::cerr;
using std::endl;

// message for verbose output
extern std::string globalErrorMessage; // defined in main.cc

// used to determine whether to write dot output
extern gengetopt_args_info args_info; // defined in main.cc

/*!
 * \brief whether two service automata are conformance partners
 */
bool isConformancePartner(ServiceAutomaton & specification, ServiceAutomaton & testCase)
{
  // initialize resulting automaton
  ProductAutomaton productAutomaton;
  productAutomaton.nextChannelStateID = 1;
  productAutomaton.nextStateID = 2;

  // build automaton and execute tests
  bool result = (createProductAutomaton(specification, testCase, productAutomaton) && // build product automaton and check states without successors
                 checkStrongReceivability(specification, testCase, productAutomaton) && // check strong receivability from service to test case
                 checkWeakReceivability(specification, testCase, productAutomaton)); // check weak receivability from test case to specification

  if((!result) && (args_info.dot_given))
  {
    // TODO: write dot output here
  }

  // return result
  return result;
}


/*!
 * \brief whether messages from the first automaton are strong receivable in the second automaton
 */
bool checkStrongReceivability(ServiceAutomaton & specification, ServiceAutomaton & testCase, ProductAutomaton & productAutomaton)
{
  // get sending events
  std::set<unsigned int> sendingEvents;

  FOREACH(event, specification.isSendingEvent)
  {
    if(event->second)
    {
      sendingEvents.insert(productAutomaton.events2IDs[event->first]);
    }
  }

  // stack for DFS
  std::stack<unsigned int> dfsStack;

  // get states of product automaton holding states without successors in the service automaton
  std::set<unsigned int> whiteNodes = productAutomaton.noSpecificationSuccessorStates;

  // perform DFS on SCCs in these states
  while(whiteNodes.size() > 0) // while set of these states (= unvisited states) is not empty
  {
    // put first state on DFS stack
    dfsStack.push(*whiteNodes.begin());

    while(dfsStack.size() > 0) // while stack not empty
    {
      // get current state and remove it from DFS stack
      unsigned int currentStateID = dfsStack.top();
      ProductState & currentState = productAutomaton.states[currentStateID];
      dfsStack.pop();

      // check whether this state has already been processed
      if(whiteNodes.count(currentStateID) == 0)
      {
        continue;
      }

      // get SCC and remove all nodes from the white nodes
      std::set<unsigned int> & sccNodes = productAutomaton.SCCs[currentState.lowlink];
      FOREACH(node, sccNodes)
      {
        whiteNodes.erase(*node);
      }

      // get interface
      ChannelState & channelState = productAutomaton.channelStates[currentState.interfaceStateID];

      // check for sending events
      bool sendingEventFound = false;
      FOREACH(event, sendingEvents)
      {
        if(channelState.count(*event) > 0)
        {
          sendingEventFound = true;
          break;
        }
      }

      if(sendingEventFound) // need to clean up interface
      {
        std::set<unsigned int> successorNodes;
        bool processedSuccessorFound = false;

        // check for successors
        FOREACH(sccNode, sccNodes) // iterate over all states of this SCC
        {
          FOREACH(succ, productAutomaton.transitions[*sccNode]) // iterate over all successor states
          {
            FOREACH(succ_, succ->second)
            {
              unsigned int succSCC = productAutomaton.states[succ_->second].lowlink;
              if(succSCC != currentState.lowlink) // successor not in current SCC
              {
                // get first Node of successor SCC
                unsigned int successorNode = *productAutomaton.SCCs[succSCC].begin();
                if(whiteNodes.count(successorNode) > 0) // node not seen yet
                {
                  // need to visit this node
                  successorNodes.insert(successorNode);
                }
                else
                {
                  // successor already seen, i.e. we will reach a "good" state from there
                  processedSuccessorFound = true;
                  break;
                }
              }
            }
          }
          if(processedSuccessorFound)
          {
            break; // skip further nodes of this SCC
          }
        }

        if(!processedSuccessorFound)
        {
          if(successorNodes.size() == 0) // if we can not leave this SCC
          {
            // we found a sending event of the service that is not strong receivable
            productAutomaton.badNode = currentStateID; // mark current state as bad node

            // generate error message
            std::stringstream ss;
            ss << "message '";
            FOREACH(event, sendingEvents)
            {
              if(channelState.count(*event) > 0)
              {
                ss << (productAutomaton.events[*event]);
                break;
              }
            }
            ss << "' sent by the specification can not be removed from state " << currentStateID
               << " and is therefore not strong receivable";
            globalErrorMessage = ss.str();

            // return result
            return false;
          }
          // we need to check our successors
          FOREACH(succ, successorNodes)
          {
            dfsStack.push(*succ);
          }
        }
        // else: we found a succeeding SCC from which a state with a "good" interface is reachable
      }
      // else: everything is fine here
    }
  }

  // all states seen, all sending events could be removed from the interface
  return true;
}


/*!
 * \brief whether messages from the second automaton are weak receivable in the first automaton
 */
bool checkWeakReceivability(ServiceAutomaton & specification, ServiceAutomaton & testCase, ProductAutomaton & productAutomaton)
{
  // initialisation
  std::set<unsigned int> whiteNodes = productAutomaton.noTestCaseSuccessorStates;
  std::queue<unsigned int> bfsQueue;
  //std::stack<unsigned int> dfsStack;
  searchCache mustRemove, // node must remove these messages for weak receivability or pass them to parents
              needToRemove, // "2do-list" for DFS
              canRemove, // positive results for DFS
              triedToRemove; // already performed DFS
  std::set<unsigned int> sendingEvents; // sending events
  FOREACH(event, testCase.isSendingEvent)
  {
    if(event->second)
    {
      sendingEvents.insert(productAutomaton.events2IDs[event->first]);
    }
  }

  // store visited nodes for each root node
  std::map<unsigned int, std::set<unsigned int> > blackNodes; // map: root -> visited nodes

  // perform BFS backwards
  while(whiteNodes.size() > 0)
  {
    // start BFS on next node
    unsigned int rootNode = *whiteNodes.begin();
    whiteNodes.erase(rootNode);
    bfsQueue.push(rootNode);

    // check own interface
    ChannelState & channelState = productAutomaton.channelStates[productAutomaton.states[rootNode].interfaceStateID];
    FOREACH(event, sendingEvents)
    {
      if(channelState.count(*event) > 0)
      {
        mustRemove[rootNode].insert(*event); // sending events on the interface must be removed
      }
    }

    while(bfsQueue.size() > 0) // actual BFS
    {
      // get next state
      unsigned int currentStateID = bfsQueue.front();
      bfsQueue.pop();

      // mark state as visited
      blackNodes[rootNode].insert(currentStateID);

      // collect "mustRemove"s
      FOREACH(message, mustRemove[currentStateID])
      {
        needToRemove[currentStateID].insert(*message);
      }


      // BEGIN DFS
      std::set<unsigned int> dfsSeenNodes;
      std::stack<unsigned int> dfs2Do, dfsTrace;
      dfs2Do.push(currentStateID);
      std::map<unsigned int, std::set<unsigned int> > successors, dfsLeftovers; // successor and leftover cache

      while(dfs2Do.size() > 0)
      {
        unsigned int dfsCurrentStateID = dfs2Do.top();

        if(dfsSeenNodes.count(dfsCurrentStateID) == 0) // node not seen, yet
        {
          dfsSeenNodes.insert(dfsCurrentStateID); // mark node as seen
          dfsTrace.push(dfsCurrentStateID); // do one DFS step

          if(needToRemove[dfsCurrentStateID].size() == triedToRemove[dfsCurrentStateID].size()) // if everything is already done for this node
          {
            continue; // do not search twice
          }

          ChannelState & channelState = productAutomaton.channelStates[productAutomaton.states[dfsCurrentStateID].interfaceStateID]; // current interface
          FOREACH(message, needToRemove[dfsCurrentStateID])
          {
            if(triedToRemove[dfsCurrentStateID].count(*message) == 0) // not tried to remove this message, yet
            {
              triedToRemove[dfsCurrentStateID].insert(*message); // now we will try
              if(channelState.count(*message) > 0)
              {
                // message not removed from channel; need to delegate to child nodes
                dfsLeftovers[dfsCurrentStateID].insert(*message);
              }
              else
              {
                // message cleared, whohoo
                canRemove[dfsCurrentStateID].insert(*message);
              }
            }
          }

          if(dfsLeftovers[dfsCurrentStateID].size() == 0) // nothing to delegate to children
          {
            continue; // we are finished for this node
          }

          unsigned int currentTestCaseState = productAutomaton.states[dfsCurrentStateID].internalState.second; // only specification is allowed to do operations
          FOREACH(transition, productAutomaton.transitions[dfsCurrentStateID]) // iterate over all successors
          {
            FOREACH(trans, transition->second)
            {
              switch(trans->first) // switch transition type
              {
              case INTERNAL:
                if(productAutomaton.states[trans->second].internalState.second == currentTestCaseState)
                {
                  // either specification did a step or we are TAU-looping to this state again and will abort next iteration
                  successors[dfsCurrentStateID].insert(trans->second);
                }
                break;
              case SENDING:
                if(!testCase.isSendingEvent[transition->first])
                {
                  // a sending step, but test case will only receive this message so specification did a step
                  successors[dfsCurrentStateID].insert(trans->second);
                }
                break;
              case RECEIVING:
                if(testCase.isSendingEvent[transition->first])
                {
                  // a receiving step, but test case will only send this message so specification did a step
                  successors[dfsCurrentStateID].insert(trans->second);
                }
                break;
              default:
                assert(false); // should not happen
              }
            }
          }

          // process all specification successors
          FOREACH(successor, successors[dfsCurrentStateID])
          {
            // schedule leftovers for next DFS iteration
            FOREACH(message, dfsLeftovers[dfsCurrentStateID])
            {
              needToRemove[*successor].insert(*message);
              dfs2Do.push(*successor);
            }
          }
        }
        else // node already seen
        {
          dfs2Do.pop();
          if(dfsTrace.top() == dfsCurrentStateID) // backtracking
          {
            dfsTrace.pop();
            FOREACH(successor, successors[dfsCurrentStateID])
            {
              // collect results
              FOREACH(message, dfsLeftovers[dfsCurrentStateID])
              {
                if(canRemove[*successor].count(*message) > 0) // if successor could remove this message
                {
                  // than we also can
                  canRemove[dfsCurrentStateID].insert(*message);
                }
              }
            }
          }
          // else reaching this node by a non-tree-arc; ignore
        }
      }
      // END DFS


      // check "mustRemove"s
      std::set<unsigned int> leftovers;
      FOREACH(message, mustRemove[currentStateID])
      {
        if(canRemove[currentStateID].count(*message) == 0) // if message could not be removed
        {
          leftovers.insert(*message); // some parent needs to remove this message
        }
      }

      if(leftovers.size() == 0)
      {
        // we are done for this node
        continue;
      }

      // whether we can delegate cleanup to predecessors
      bool predecessorFound = false;

      // get predecessors
      FOREACH(predecessor, productAutomaton.predecessors[currentStateID])
      {
        if(blackNodes[rootNode].count(*predecessor) > 0) // node already visited
        {
          // skip this predecessor
          continue;
        }

        // find successor "arc" leading to current state
        FOREACH(transition, productAutomaton.transitions[*predecessor])
        {
          FOREACH(trans, transition->second)
          {
            if(trans->second == currentStateID) // successor arc found
            {
              if((trans->first == SENDING) && // got to current state by sending a message
                  (leftovers.count(productAutomaton.events2IDs[transition->first]) > 0)) // and this message cannot be removed from here
              {
                // must not use this arc for backtracking
                break;
              }

              // otherwise parent must remove our leftovers
              FOREACH(message, leftovers)
              {
                mustRemove[*predecessor].insert(*message);
              }

              // schedule parent for BFS
              bfsQueue.push(*predecessor);
              predecessorFound = true; // we found at least one predecessor now

              break; // skip further iteration over successors of this parent
            }
          }
        }
      }

      if(!predecessorFound) // if no predecessor could be found to remove our leftovers
      {
        // then each message left over is not weak receivable
        productAutomaton.badNode = currentStateID; // mark current node as bad node

        // generate error message
        std::stringstream ss;
        ss << " for message '" << (productAutomaton.events[*leftovers.begin()]) << "' sent by the test case and present in state "
           << currentStateID << " no appropriate prefix path could be found, therefore this message is not weak receivable";
        globalErrorMessage = ss.str();

        // return result
        return false;
      }
    }
  }

  // all messages could be removed somehow; all messages are weak receivable
  return true;
}
