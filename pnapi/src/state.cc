#include "automaton.h"
#include "state.h"

#include <cmath>
#include <map>

using std::map;

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

unsigned int State::getHashValue(map<Place *, unsigned int> &pt)
{
  if (pt.empty())
    return 0;

  if (hashValue != NULL)
  {
    unsigned int hash = 1;
    for (map<Place *, unsigned int>::const_iterator p = pt.begin();
        p != pt.end(); ++p)
      hash *= pow((*p).second, m[(*p).first]);

    hashValue = new unsigned int(hash);
  }

  return *hashValue;
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
