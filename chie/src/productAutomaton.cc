#include <config.h>

#include "productAutomaton.h"

#include <queue>
#include <stack>
#include <sstream>

#include "Output.h"


/*!
 * \brief hash function for channel states
 */
unsigned int channelStateHash(ChannelState & s)
{
  unsigned int result = 0;
  for(unsigned int i = 0; i < s.size(); ++i)
  {
    result += (1 << i) * s[i];
  }
  return result;
}


/// hash map of all channel states
HashMap<ChannelState, channelStateHash> ProductAutomatonState::channelStates_;

/// pointer to initial state
ProductAutomatonState * ProductAutomatonState::initialState_(NULL);

/// pointer to specification automaton
ServiceAutomaton * ProductAutomatonState::specificationAutomaton_(NULL);

/// pointer to test case automaton
ServiceAutomaton * ProductAutomatonState::testCaseAutomaton_(NULL);


/*!
 * \brief initial state constructor
 */
ProductAutomatonState::ProductAutomatonState(ServiceAutomaton & specification, ServiceAutomaton & testCase):
    specificationState_(specification.initialState), testCaseState_(testCase.initialState),
    channelState_(new ChannelState(ServiceAutomaton::labels.size() - 1, 0)),
    dfs(0), lowlink(0), onTarjanStack(false)
{
  assert(initialState_ == NULL);
  initialState_ = this;
  specificationAutomaton_ = &specification;
  testCaseAutomaton_ = &testCase;
  channelStates_.insert(*channelState_);
}

/*!
 * \brief successor generating constructor
 */
ProductAutomatonState::ProductAutomatonState(ProductAutomatonState & pre, unsigned int nextState,
                                             bool isSpecificationStep, ChannelState * nextChannelState):
    specificationState_(isSpecificationStep ? nextState : pre.specificationState_),
    testCaseState_(isSpecificationStep? pre.testCaseState_ : nextState),
    channelState_(nextChannelState), dfs(0), lowlink(0), onTarjanStack(false)
{
}


/*!
 * \brief destructor
 */
ProductAutomatonState::~ProductAutomatonState()
{
  if(this == initialState_)
  {
    // clear data
    channelStates_.clearDelete();
    initialState_ = NULL;
    specificationAutomaton_ = testCaseAutomaton_ = NULL;
  }
}

/*!
 * \brief hash function
 */
unsigned int ProductAutomatonState::hash(ProductAutomatonState & a)
{
  return ((a.specificationState_ << 16) + a.testCaseState_ + channelStateHash(*a.channelState_));
}

/*!
 * \brief compare pointers
 */
bool ProductAutomatonState::isLessThan(ProductAutomatonState * l, ProductAutomatonState * r)
{
  return ((*l) < (*r));
}

/*!
 * \brief comparison operator
 */
bool ProductAutomatonState::operator<(const ProductAutomatonState & other) const
{
  if(specificationState_ != other.specificationState_)
  {
    return (specificationState_ < other.specificationState_);
  }
  if(testCaseState_ != other.testCaseState_)
  {
    return (testCaseState_ < other.testCaseState_);
  }
  return ((*channelState_) < (*other.channelState_));
}

/*!
 * \brief comparison operator
 */
bool ProductAutomatonState::operator==(ProductAutomatonState & other)
{
  return ((specificationState_ == other.specificationState_) &&
          (testCaseState_ == other.testCaseState_) &&
          (channelState_ == other.channelState_));
}

/*!
 * \brief successor creation method
 */
std::vector<ProductAutomatonTransition *> ProductAutomatonState::generateSuccessors()
{
  // create resulting vector
  std::vector<ProductAutomatonTransition *> result;

  // specification steps
  FOREACH(successors, specificationAutomaton_->stateSpace[specificationState_])
  {
    unsigned int event = successors->first;
    ChannelState * nextChannelState = channelState_; // next channel state for TAU events
    if((event >= ServiceAutomaton::firstReceiveID) && (event <= ServiceAutomaton::lastReceiveID)) // receiving
    {
      if((*channelState_)[event - 1] == 0) // message not present
      {
        // can not receive
        continue;
      }

      // calculate next channel state
      nextChannelState = new ChannelState(*channelState_); // copy recent channel state
      --(*nextChannelState)[event - 1]; // consume message
      ChannelState * inserted = channelStates_.insert(*nextChannelState); // insert in hash map
      if(inserted != nextChannelState) // if state already existed
      {
        // cleanup
        delete nextChannelState;
        nextChannelState = inserted;
      }
    }
    if((event >= ServiceAutomaton::firstSendID) && (event <= ServiceAutomaton::lastSendID)) // sending
    {
      // calculate next channel state
      nextChannelState = new ChannelState(*channelState_); // copy recent channel state
      ++(*nextChannelState)[event - 1]; // produce message
      ChannelState * inserted = channelStates_.insert(*nextChannelState); // insert in hash map
      if(inserted != nextChannelState) // if state already existed
      {
        // cleanup
        delete nextChannelState;
        nextChannelState = inserted;
      }
    }

    // generate successors
    for(unsigned int i = 0; i < successors->second.size(); ++i)
    {
      result.push_back(new ProductAutomatonTransition(event, true, new ProductAutomatonState(*this, successors->second[i], true, nextChannelState)));
    }
  }

  // test case steps
  FOREACH(successors, testCaseAutomaton_->stateSpace[testCaseState_])
  {
    unsigned int event = successors->first;
    ChannelState * nextChannelState = channelState_; // next channel state for TAU events
    if((event >= ServiceAutomaton::firstSendID) && (event <= ServiceAutomaton::lastSendID)) // receiving
    {
      if((*channelState_)[event - 1] == 0) // message not present
      {
        // can not receive
        continue;
      }

      // calculate next channel state
      nextChannelState = new ChannelState(*channelState_); // copy recent channel state
      --(*nextChannelState)[event - 1]; // consume message
      ChannelState * inserted = channelStates_.insert(*nextChannelState); // insert in hash map
      if(inserted != nextChannelState) // if state already existed
      {
        // cleanup
        delete nextChannelState;
        nextChannelState = inserted;
      }
    }
    if((event >= ServiceAutomaton::firstReceiveID) && (event <= ServiceAutomaton::lastReceiveID)) // sending
    {
      // calculate next channel state
      nextChannelState = new ChannelState(*channelState_); // copy recent channel state
      ++(*nextChannelState)[event - 1]; // produce message
      ChannelState * inserted = channelStates_.insert(*nextChannelState); // insert in hash map
      if(inserted != nextChannelState) // if state already existed
      {
        // cleanup
        delete nextChannelState;
        nextChannelState = inserted;
      }
    }

    // generate successors
    for(unsigned int i = 0; i < successors->second.size(); ++i)
    {
      result.push_back(new ProductAutomatonTransition(event, false, new ProductAutomatonState(*this, successors->second[i], false, nextChannelState)));
    }
  }

  return result;
}



/*!
 * \brief contructor
 */
ProductAutomatonTransition::ProductAutomatonTransition(unsigned int e, bool s, ProductAutomatonState * r):
    event(e), specificationStep(s), resultingState(r)
{
}



/*!
 * \brief constructor
 */
ProductAutomaton::ProductAutomaton(ServiceAutomaton & specification, ServiceAutomaton & testCase):
    specificationAutomaton_(specification), testCaseAutomaton_(testCase), initialState_(new ProductAutomatonState(specification, testCase)),
    badState_(NULL)
{
  // insert initial state to state set
  states_.insert(*initialState_);

  // variables for DFS
  std::stack<ProductAutomatonState *> toDoStack, dfsStack, tarjanStack;
  toDoStack.push(initialState_);
  unsigned int nextID = 1;
  ProductAutomatonState * currentState = NULL;

  // perform DFS
  while(toDoStack.size() > 0)
  {
    // get next state
    currentState = toDoStack.top();

    if(currentState->dfs > 0) // state already seen
    {
      if(currentState == dfsStack.top()) // successors currently seen; i.e. now backtracking
      {
        if(currentState->lowlink == currentState->dfs) // SCC found
        {
          // collecting SCC members; use lowlink as SCC ID
          while(tarjanStack.top() != currentState)
          {
            SCCs_[currentState->lowlink].push_back(tarjanStack.top());
            tarjanStack.top()->lowlink = currentState->lowlink; // lowlink for these nodes not needed anymore; recycle as SCC ID
            // remove SCC from Tarjan stack
            tarjanStack.top()->onTarjanStack = false;
            tarjanStack.pop();
          }
          SCCs_[currentState->lowlink].push_back(currentState);
          // remove last SCC node from Tarjan stack
          currentState->onTarjanStack = false;
          tarjanStack.pop();

          // "no successor" stuff
          if(specificationAutomaton_.noSuccessorStates.count(currentState->specificationState_) > 0) // if specification automaton has no successor
          {
            noSpecificationSuccessors_.push_back(currentState->lowlink); // add this SCC to the list
          }
        }

        dfsStack.pop(); // backtracking
        if(dfsStack.size() > 0) // if DFS is not finished yet (i.e. if this was a recursive step)
        {
          // set parent's lowlink
          dfsStack.top()->lowlink = (dfsStack.top()->lowlink > currentState->lowlink) ? dfsStack.top()->lowlink : currentState->lowlink;
        }
      }
      else // this state was reached by a non-tree-arc
      {
        if(currentState->onTarjanStack) // if this state is on tarjan stack
        {
          // set parent's lowlink
          dfsStack.top()->lowlink = (dfsStack.top()->lowlink > currentState->dfs) ? dfsStack.top()->lowlink : currentState->dfs;
        }
      }

      toDoStack.pop(); // remove state from processing stack
      continue; // skip further steps for this state
    }// else: white node

    // actual processing of this state
    currentState->dfs = currentState->lowlink = nextID++;
    dfsStack.push(currentState); // one step deeper
    tarjanStack.push(currentState);
    currentState->onTarjanStack = true;

    // get successors
    std::vector<ProductAutomatonTransition *> successors = currentState->generateSuccessors();

    if(successors.size() == 0) // if there is no successor for this state
    {
      // avoid counting twice
      bool cond1 = (specificationAutomaton_.finalStates.count(currentState->specificationState_) == 0); // specification state is no final state
      bool cond2 = (testCaseAutomaton_.finalStates.count(currentState->testCaseState_) == 0); // test case state is no final state

      // no successor found, so both the specification automaton state and the test case automaton state must be final states
      if(cond1 || cond2)
      {
        badState_ = currentState; // mark this state as bad node in product automaton

        // generate error message
        std::stringstream ss;
        ss << "node " << (currentState->dfs) << " with specification state " << (currentState->specificationState_)
           << " and test case state " << (currentState->testCaseState_) << " has no successors but ";
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
        errorMessage = ss.str();

        // abort DFS
        break;
      }
    }

    for(unsigned int i = 0; i < successors.size(); ++i)
    {
      // insert to state set
      ProductAutomatonState * inserted = states_.insert(*successors[i]->resultingState);

      if(inserted != successors[i]->resultingState) // duplicate found
      {
        delete (successors[i]->resultingState);
      }
      if(successors[i]->specificationStep)
      {
        currentState->specificationSuccessors_[successors[i]->event].push_back(inserted);
      }
      else
      {
        currentState->testCaseSuccessors_[successors[i]->event].push_back(inserted);
      }
      delete (successors[i]); // free memory

      // add to 2do-Stack
      toDoStack.push(inserted);
    }
  }
}

/*!
 * \brief destructor
 */
ProductAutomaton::~ProductAutomaton()
{
  states_.clearDelete();
}

/*!
 * \brief whether messages from specification automaton to test case automaton are strong receivable
 */
bool ProductAutomaton::checkStrongReceivability()
{
  if(badState_) // if already a bad state was found during automaton creation
  {
    // skip this test
    return false;
  }

  // iterate over all SCCs without successors in specification automaton
  for(unsigned int i = 0; i < noSpecificationSuccessors_.size(); ++i)
  {
    // check for TSCC
    bool outgoingTransitionFound = false;

    for(unsigned int j = 0; j < SCCs_[noSpecificationSuccessors_[i]].size(); ++j) // iterate over all states in this SCC
    {
      FOREACH(successors, SCCs_[noSpecificationSuccessors_[i]][j]->testCaseSuccessors_) // iterate over all successors of this state
      {
        for(unsigned int k = 0; k < successors->second.size(); ++k)
        {
          if(successors->second[k]->lowlink != noSpecificationSuccessors_[i])
          {
            // this arc leaves the SCC
            outgoingTransitionFound = true;
            break;
          }
        }

        if(outgoingTransitionFound)
        {
          break;
        }
      }

      if(outgoingTransitionFound)
      {
        break;
      }
    }

    if(outgoingTransitionFound)
    {
      continue;
    }

    // when reaching this line a TSCC has been found
    ChannelState * channelState = SCCs_[noSpecificationSuccessors_[i]][0]->channelState_; // get channel state of this SCC

    // check for leftovers
    unsigned int messageLeft = 0;
    for(unsigned int j = ServiceAutomaton::firstSendID; j <= ServiceAutomaton::lastSendID; ++j) // iterate over all sent messages
    {
      if((*channelState)[j-1] > 0)
      {
        messageLeft = j;
        break;
      }
    }

    if(messageLeft) // bad state found
    {
      // mark bad state
      badState_ = SCCs_[noSpecificationSuccessors_[i]][0];

      // generate error message
      std::stringstream ss;
      ss << "message '"
         << ServiceAutomaton::labels[messageLeft]
         << "' sent by the specification can not be removed from state " << (badState_->dfs)
         << " (" << badState_->specificationState_ << "|" << badState_->testCaseState_ << ")"
         << " and is therefore not strong receivable";
      errorMessage = ss.str();

      // return result
      return false;
    }
  }

  // all TSCCs are fine
  return true;
}



/*!
 * \brief write automaton do dot file with given filename
 */
void ProductAutomaton::writeToDotFile(std::string & fileName, bool writePath)
{
  // variable(s) used later (do not allocate memory within loops)
  bool notFirst;

  // path to bad node
  std::map<ProductAutomatonState *, ProductAutomatonState *> path;

  if(writePath)
  {
    findShortestPath(path);
  }

  // create output file
  Output out(fileName, "dot file");

  // write head
  out.stream() << "digraph G {\n"
               << " node [fontname=\"Helvetica\" fontsize=10]\n"
               << " edge [fontname=\"Helvetica\" fontsize=10]\n"
               << " INIT [label=\"\" height=\"0.01\" style=\"invis\"]\n"
               << " INIT -> \"1\" [minlen=\"0.5\"]\n";

  // iterate over product automaton statespace
  states_.resetToFirst();
  while(ProductAutomatonState * state = states_.getNext()) // while pointer != NULL
  {
    // write current state
    out.stream() << " \"" << (state->dfs) << "\" [label=\""
                 << (state->dfs) << ": "
                 << (state->specificationState_) << " | "
                 << (state->testCaseState_) << " | {";

    // write channel state
    ChannelState & cs = (*state->channelState_);
    notFirst = false;
    for(unsigned int i = 0; i < cs.size(); ++i)
    {
      for(unsigned int j = 0; j < cs[i]; ++j)
      {
        if(notFirst)
        {
          out.stream() << ", ";
        }
        out.stream() << ServiceAutomaton::labels[i + 1];
        notFirst = true;
      }
    }
    out.stream() << "}\"";

    // mark path to "bad node" red
    if(path.count(state) > 0)
    {
      out.stream() << " color=red";
    }

    out.stream() << "]\n";

    // collect specification successors
    FOREACH(succ, state->specificationSuccessors_)
    {
      // build arc label
      std::string label = "&tau;";
      if((succ->first >= ServiceAutomaton::firstReceiveID) && (succ->first <= ServiceAutomaton::lastReceiveID))
      {
        label = "?" + ServiceAutomaton::labels[succ->first];
      }
      if((succ->first >= ServiceAutomaton::firstSendID) && (succ->first <= ServiceAutomaton::lastSendID))
      {
        label = "!" + ServiceAutomaton::labels[succ->first];
      }

      // write successors
      for(unsigned int i = 0; i < succ->second.size(); ++i)
      {
        out.stream() << "\"" << (state->dfs) << "\" -> \"" << ((succ->second)[i]->dfs)
                     << "\" [label=\"" << label << "\"";

        // mark path to "bad node" red
        if((path.count(state) > 0) && (path[state] == ((succ->second)[i])))
        {
          out.stream() << " color=red";
        }
        out.stream() << "]\n";
      }
    }

    // collect test case successors
    FOREACH(succ, state->testCaseSuccessors_)
    {
      // build arc label
      std::string label = "&tau;";
      if((succ->first >= ServiceAutomaton::firstReceiveID) && (succ->first <= ServiceAutomaton::lastReceiveID))
      {
        label = "!" + ServiceAutomaton::labels[succ->first];
      }
      if((succ->first >= ServiceAutomaton::firstSendID) && (succ->first <= ServiceAutomaton::lastSendID))
      {
        label = "?" + ServiceAutomaton::labels[succ->first];
      }

      // write successors
      for(unsigned int i = 0; i < succ->second.size(); ++i)
      {
        out.stream() << "\"" << (state->dfs) << "\" -> \"" << ((succ->second)[i]->dfs)
                     << "\" [label=\"" << label << "\"";

        // mark path to "bad node" red
        if((path.count(state) > 0) && (path[state] == ((succ->second)[i])))
        {
          out.stream() << " color=red";
        }
        out.stream() << "]\n";
      }
    }
  }

  // finish file
  out.stream() << "}";
}

/*!
 * \brief generate shortest path to the "bad" state
 */
void ProductAutomaton::findShortestPath(std::map<ProductAutomatonState *, ProductAutomatonState *> & path)
{
  // BFS queue
  std::queue<ProductAutomatonState *> bfs;
  // predecessor relation
  std::map<ProductAutomatonState *, ProductAutomatonState *> predecessors;
  // set of seen states
  std::set<ProductAutomatonState *> seen;

  // init queue
  bfs.push(initialState_);
  seen.insert(initialState_);

  // current state
  ProductAutomatonState * currentState;

  // perform BFS
  while(true)
  {
    // get next state
    currentState = bfs.front();
    bfs.pop();

    // breaking condition
    if(currentState == badState_)
    {
      // bad state found
      break;
    }

    // get specification successors
    FOREACH(succ, currentState->specificationSuccessors_)
    {
      for(unsigned int i = 0; i < succ->second.size(); ++i)
      {
        if(seen.count((succ->second)[i]) == 0)
        {
          seen.insert((succ->second)[i]);
          predecessors[(succ->second)[i]] = currentState;
          bfs.push((succ->second)[i]);
        }
      }
    }
    FOREACH(succ, currentState->testCaseSuccessors_)
    {
      for(unsigned int i = 0; i < succ->second.size(); ++i)
      {
        if(seen.count((succ->second)[i]) == 0)
        {
          seen.insert((succ->second)[i]);
          predecessors[(succ->second)[i]] = currentState;
          bfs.push((succ->second)[i]);
        }
      }
    }
  }

  // collect path
  path[currentState = badState_] = badState_; // self loop so the bad state is in the result as well

  while(currentState != initialState_)
  {
    path[predecessors[currentState]] = currentState;
    currentState = predecessors[currentState];
  }
}
