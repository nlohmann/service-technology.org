#include "automaton.h"
#include "state.h"

#include <cmath>
#include <map>

using std::map;

namespace pnapi
{


State::State(Marking &m) :
  m_(m), hashValue_(NULL)
{
}

State::State(const State &s) :
  m_(s.m_), index_(s.index_), successors_(s.successors_), reasons_(s.reasons_)
{
  hashValue_ = new unsigned int(*s.hashValue_);
}

State::~State()
{
}

unsigned int State::maxIndex_ = 0;

Marking & State::getMarking() const
{
  return m_;
}

void State::setMarking(Marking &m)
{
  m_ = m;
}

void State::setIndex()
{
  index_ = maxIndex_++;
}

unsigned int State::getIndex() const
{
  return index_;
}

unsigned int State::size() const
{
  return m_.size();
}

void State::addSuccessor(State &s)
{
  successors_.push_back(&s);
}

const list<State *> & State::getSuccessors() const
{
  return successors_;
}

void State::addReason(const string &r)
{
  reasons_.push_back(r);
}

const list<string> & State::getReason() const
{
  return reasons_;
}

unsigned int State::getHashValue(map<Place *, unsigned int> &pt)
{
  if (pt.empty())
    return 0;

  if (hashValue_ != NULL)
  {
    unsigned int hash = 1;
    for (map<Place *, unsigned int>::const_iterator p = pt.begin();
        p != pt.end(); ++p)
      hash *= pow((*p).second, m_[(*p).first]);

    hashValue_ = new unsigned int(hash);
  }

  return *hashValue_;
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
  return m_ == j.m_;
}

}
