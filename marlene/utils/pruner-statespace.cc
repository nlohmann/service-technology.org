#include <iostream>
#include <algorithm>
#include <macros.h>
#include "pruner-statespace.h"

std::map<unsigned int, shared_ptr<State> > State::stateSpace;

std::map<std::string, std::string> State::abstractedIn;
std::map<std::string, std::string> State::abstractedOut;
std::map<std::string, std::string> State::abstractedSync;

std::map<std::string, std::string> State::remainingIn;
std::map<std::string, std::string> State::remainingOut;
std::map<std::string, std::string> State::remainingSync;

State::State(long number, ListOfPairs::List_ptr marking,
    ListOfPairs::List_ptr transitions, bool isFinal, unsigned int lowlink,
    List<unsigned int>::List_ptr sccmember)
{
  this->number = number;
  this->marking = marking;
  this->transitions = transitions;
  this->isFinal = isFinal;
  this->finalReachable = isFinal;
  this->lowlink = lowlink;
  this->index = number;
  this->sccmember = sccmember;
}

State::~State()
{
  this->marking = ListOfPairs::List_ptr();
  this->transitions = ListOfPairs::List_ptr();
  this->sccmember = List<unsigned int>::List_ptr();
}

void
State::output()
{

  using namespace std;

  std::map<unsigned int, bool> seen;

  output(0, seen);
}

void
State::output(unsigned int stateNo, std::map<unsigned int, bool>& seen)
{

  using namespace std;

  if (not seen[stateNo])
    {
      seen[stateNo] = true;

      shared_ptr<State> state(stateSpace[stateNo]);

      // first: output all successors
      if (state->transitions.get() != 0)
        {
          for (ListOfPairs::List_ptr transition = state->transitions;
              transition.get() != 0;
              output(transition->getValue()->getSecond(), seen), transition =
                  transition->getNext())
            ;

        }

      cout << "STATE " << state->index << " Lowlink: " << state->lowlink;
      if (state->sccmember.get() != 0)
        {
          cout << " SCC:";
          for (List<unsigned int>::List_ptr next = state->sccmember;
              next.get() != 0;
              cout << " " << next->getValue(), next = next->getNext())
            ;
        }
      if (state->marking.get() != 0)
        {
          bool first = true;
          for (ListOfPairs::List_ptr marking = state->marking;
              marking.get() != 0;
              cout << (first ? "" : ",") << endl
                  << marking->getValue()->getFirst() << " : "
                  << marking->getValue()->getSecond(), marking =
                  marking->getNext(), first = false)
            ;

        }
      cout << endl;
      if (state->transitions.get() != 0)
        {
          for (ListOfPairs::List_ptr transition = state->transitions;
              transition.get() != 0;
              cout << endl << transition->getValue()->getFirst() << " -> "
                  << stateSpace[transition->getValue()->getSecond()]->index, transition =
                  transition->getNext())
            ;

        }
      cout << endl;
      cout << endl;
    }
}

bool
State::checkFinalReachable()
{

  std::map<unsigned int, bool> seen;
  unsigned int count = 0;
  unsigned int old_count = 0;

  bool result = false;
  do
    {
      old_count = count;
      count = 0;
      result = checkFinalReachable(0, seen, count);
      status("There are %d state(s) for which a final state is reachable",
          count);
      seen.clear();
    }
  while (count > old_count);
  return result;
}

bool
State::checkFinalReachable(unsigned int stateNo,
    std::map<unsigned int, bool>& seen , unsigned int & count)
{

  shared_ptr<State> state = stateSpace[stateNo];

  bool result = state->isFinal || state->finalReachable;
  seen[stateNo] = true;
  if (state->transitions.get() != 0)
    {
      for (ListOfPairs::List_ptr transition = state->transitions;
          transition.get() != 0; transition = transition->getNext())
        {

          if (not seen[transition->getValue()->getSecond()])
            {
              bool resultstep = checkFinalReachable(
                  transition->getValue()->getSecond(), seen, count);
              result = result || resultstep;
            }
          result =
              result
                  || stateSpace[transition->getValue()->getSecond()]->finalReachable;
        }
      if (result)
        {
          ++count;
        }
    }
  state->finalReachable = result;
  return result;
}

void
State::calculateSCC()
{
  unsigned int index = 0;
  unsigned int lowlink = 0;

  std::map<unsigned int, bool> seen;
  std::list<unsigned int> stack;
  calculateSCC(0, index, lowlink, seen, stack);
}

void
State::calculateSCC(unsigned int stateNo, unsigned int& index,
    unsigned int& lowlink, std::map<unsigned int, bool>& seen
    , std::list<unsigned int>& stack)
{
  shared_ptr<State> state = stateSpace[stateNo];

  seen[stateNo] = true;

  state->index = index;
  state->lowlink = index;
  ++index;

  stack.push_front(state->index);

  for (ListOfPairs::List_ptr transition = state->transitions;
      transition.get() != 0; transition = transition->getNext())
    {
      shared_ptr<State> successor =
          stateSpace[transition->getValue()->getSecond()];
      if (not seen[successor->number])
        {
          calculateSCC(successor->number, index, lowlink, seen, stack);
          state->lowlink =
              (state->lowlink < successor->lowlink) ?
                  state->lowlink : successor->lowlink;
        }
      else if (find(stack.begin(), stack.end(), successor->number)
          != stack.end())
        {
          state->lowlink =
              (state->lowlink < successor->index) ?
                  state->lowlink : successor->index;
        }
    }

  if (state->lowlink == state->index)
    {
      List<unsigned int>::List_ptr newSCC;
      unsigned int w = 0;
      do
        {
          w = stack.front();
          stack.pop_front();
          if (newSCC.get() == 0)
            {
              newSCC = List<unsigned int>::List_ptr(new List<unsigned int>(w));
            }
          else
            {
              newSCC->push_back(w);
            }
        }
      while (w != state->index);
      state->sccmember = newSCC;

    }

}

void
State::prune()
{

  std::map<unsigned int, bool> seen;
  unsigned int count = 0;
  unsigned int old_count = 0;

  do
    {
      old_count = count;
      count = 0;
      prune(0, seen, count);
      status("Removed %d transitions.", count);
      seen.clear();
    }
  while (count > old_count);
}

void
State::prune(unsigned int stateNo, std::map<unsigned int, bool>& seen
    , unsigned int & count)
{

  shared_ptr<State> state = stateSpace[stateNo];

  ListOfPairs::List_ptr newTransitions;

  seen[stateNo] = true;
  if (state->transitions.get() != 0)
    {
      for (ListOfPairs::List_ptr transition = state->transitions;
          transition.get() != 0; transition = transition->getNext())
        {
          shared_ptr<State> successor =
              stateSpace[transition->getValue()->getSecond()];
          std::string label = transition->getValue()->getFirst();
          if (state->finalReachable && (not successor->finalReachable)
              && (abstractedSync[transition->getValue()->getFirst()] != ""))
            {
              status("pruning from state %d to %d (cause label is %s)",
                  stateNo, successor->number,
                  abstractedSync[transition->getValue()->getFirst()].c_str());
              ++count;
            }
          else
            {
              if (newTransitions.get() == 0) {
                  newTransitions = ListOfPairs::List_ptr(new ListOfPairs(transition->getValue()));
              }
              else
                {
                  newTransitions->push_back(transition->getValue());
                }
              if (not seen[successor->number])

                {
                  prune(successor->number, seen, count);
                }
            }
        }
    }
  state->transitions = newTransitions;
}
