#include "automaton.h"
#include "state.h"

namespace pnapi
{


State::State(Marking &marking) :
  m(marking), hashValue(NULL)
{
}

State::~State()
{
}

unsigned int State::maxIndex = 0;

Marking & State::getMarking() const
{
  return m;
}

void State::setMarking(Marking &mm)
{
  m = mm;
}

void State::setIndex()
{
  index = maxIndex++;
}

unsigned int State::getIndex() const
{
  return index;
}

unsigned int State::size() const
{
  return m.size();
}

void State::addSuccessor(State &s)
{
  successors.push_back(&s);
}

list<State *> State::getSuccessors() const
{
  return successors;
}

void State::addReason(const string &r)
{
  reason.push_back(r);
}

list<string> State::getReason() const
{
  return reason;
}

void State::setHashValue(unsigned int hv)
{
  hashValue = new unsigned int(hv);
}

unsigned int State::getHashValue() const
{
  return *hashValue;
}

bool State::isHashSet() const
{
  return hashValue != NULL;
}




/*!
 * \brief   Checks if two states are equal
 *
 * Two states i and j are equal iff their markings
 * they represent are equal.
 *
 * \param   State j
 *
 * \return  TRUE iff the condition is fulfilled.
 */
bool State::operator ==(const State &j) const
{
  return m == j.m;
}

}
