/*!
 * \file    serviceautomaton.cc
 *
 * \author  Stephan Mennicke <stephan.mennicke@uni-rostock.de>
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include "util.h"
//#include "marking.h"
#include "automaton.h"

using std::cerr;
using std::cout;

namespace pnapi
{



/*!
 * \brief   Standard constructor for service automaton
 *
 * This constructor takes a net n and gets the initial
 * marking from the net itself.
 *
 * \param   PetriNet n
 *
 * \note    Before building an automaton one has to normalize the by
 *          calling the public method normalize(). See precondition
 *          initialize().
 */
Automaton::Automaton(PetriNet &n) :
  hashTable_(HASHSIZE), net_(n), initialmarking_(n), f_(SA)
{
  fillPrimes();
  initialize();
  State::initialize();

  initialmarking_ = *new Marking(n, true);

  State *i = new State(initialmarking_);

  dfs(*i);

  first_ = i->getIndex();
}




/*!
 * \brief   Standard destructor
 */
Automaton::~Automaton()
{
}




/*!
 * \brief   Initialization of the service automaton
 *
 * This method sets the reasons for firing a transition t.
 *
 * \pre     PetriNet net_ is normal.
 */
void Automaton::initialize()
{
  set<Place *> Pin = net_.getInputPlaces();
  set<Place *> Pout = net_.getOutputPlaces();

  set<Transition *> done;

  for (set<Place *>::const_iterator ip = Pin.begin(); ip != Pin.end(); ip++)
  {
    for (set<Node *>::const_iterator it = (*ip)->getPostset().begin(); it
        != (*ip)->getPostset().end(); it++)
    {
      Transition *t = static_cast<Transition *> (*it);
      reasons_[t] = "?" + (*ip)->getName();
      in_.push_back((*ip)->getName());
      done.insert(t);
    }
  }

  for (set<Place *>::const_iterator op = Pout.begin(); op != Pout.end(); op++)
  {
    for (set<Node *>::const_iterator ot = (*op)->getPreset().begin(); ot
        != (*op)->getPreset().end(); ot++)
    {
      Transition *t = static_cast<Transition *>(*ot);
      reasons_[t] = "!" + (*op)->getName();
      out_.push_back((*op)->getName());
      done.insert(t);
    }
  }

  set<Transition *> notdone = util::setDifference(net_.getTransitions(), done);
  for (set<Transition *>::const_iterator t = notdone.begin(); t != notdone.end(); t++)
    reasons_[*t] = "tau";
}




void Automaton::fillPrimes()
{
  unsigned int p = 2;
  for (set<Place *>::const_iterator i = net_.getPlaces().begin();
      i != net_.getPlaces().end(); i++)
  {
    primes_[*i] = p;
    while (!isPrime(++p))
      ;
  }
}

bool Automaton::isPrime(unsigned int &p) const
{
  if (p % 2 == 0)
    return false;

  unsigned int _p = static_cast<unsigned int>(sqrt(p));
  for (unsigned int d = 3; d <= _p; d++)
    if (p % d == 0)
      return false;

  return true;
}

void Automaton::dfs(State &i)
{
  i.setIndex();
  cout << "DEBUG: new index " << i.getIndex();
  cout << " with hash value " << i.getHashValue(primes_) << "\n";

  // collision detection
  for (set<State *>::const_iterator s =
      hashTable_[i.getHashValue(primes_)].begin(); s !=
      hashTable_[i.getHashValue(primes_)].end(); s++)
  {
    cout << "DEBUG: Collision detected (" << i.getHashValue(primes_) << ")!\n";
    if (**s == i)
      return;
  }

  hashTable_[i.getHashValue(primes_)].insert(&i);
  Marking m = i.getMarking();

  // final state
  if (net_.finalCondition().checkFinalMarking(m))
  {
    cout << "DEBUG: " << i.getIndex() << " is pushed back to the final list.\n";
    finals_.push_back(i.getIndex());
    return;
  }

  set<Transition *> T = net_.getTransitions();
  for (set<Transition *>::const_iterator t = T.begin(); t != T.end(); t++)
  {
    if (!m.activates(**t))
      continue;

    State &j = *new State(m.successor(**t));
    if (i == j)
      continue;

    i.addSuccessor(j);
    i.addReason(reasons_[*t]);
    dfs(j);
  }
}

void Automaton::setFormat(Format f)
{
  f_ = f;
}

void Automaton::output_sa(ostream &os) const
{
  os << "INTERFACE\n";
  os << "  " << "INPUT ";
  list<string> lin = in_;
  bool written = false;
  while (!lin.empty())
  {
    if (!written)
    {
      os << lin.front();
      lin.pop_front();
      written = true;
      continue;
    }
    os << ", " << lin.front();
    lin.pop_front();
  }
  os << ";\n";
  os << "  " << "OUTPUT ";
  list<string> lout = out_;
  written = false;
  while (!lout.empty())
  {
    if (!written)
    {
      os << lout.front();
      lout.pop_front();
      written = true;
      continue;
    }
    os << ", " << lout.front();
    lout.pop_front();
  }
  os << ";\n\n";

  os << "NODES\n";
  list<State *> nodelist;
  int i = 0;
  for (unsigned int n = 0; n < hashTable_.size(); n++)
    for (set<State *>::iterator s = hashTable_[n].begin(); s
        != hashTable_[n].end(); s++)
    {
      if (i == 0)
        os << "  " << (*s)->getIndex();
      else
      {
        if (!i % 16)
          os << ",\n  " << (*s)->getIndex();
        else
          os << ", " << (*s)->getIndex();
      }
      nodelist.push_back(*s);
      i++;
    }
  os << ";\n\n";

  os << "INITIALNODE\n";
  os << "  " << first_ << ";\n\n";

  os << "FINALNODES\n";
  list<unsigned int> finals = finals_;
  i = 0;
  while (!finals.empty())
  {
    if (i == 0)
    {
      os << "  " << finals.front();
    }
    else
    {
      if (i % 16 == 0)
        os << ",\n  " << finals.front();
      else
        os << ", " << finals.front();
    }
    i++;
    finals.pop_front();
  }
  os << ";\n\n";

  os << "TRANSITIONS\n";
  written = false;
  while (!nodelist.empty())
  {
    State *c = nodelist.front();
    nodelist.pop_front();
    list<State *> succ = c->getSuccessors();
    list<string *> reas = c->getReasons();
    while (!succ.empty())
    {
      if (!written)
      {
        written = true;
        os << "  " << c->getIndex() << " -> " << succ.front()->getIndex()
            << " : " << *reas.front();
        succ.pop_front();
        reas.pop_front();
        continue;
      }
      os << ",\n  " << c->getIndex() << " -> " << succ.front()->getIndex()
          << " : " << *reas.front();
      succ.pop_front();
      reas.pop_front();
    }
  }
  os << ";\n";
}

void Automaton::output_stg(ostream &os) const
{
  /// will follow as soon as I get an example file
  /// fiona generates petrinet-stgs but no
  /// automata stg
}

ostream &operator <<(ostream &os, const Automaton &sa)
{
  switch (sa.f_)
  {
  case SA:    sa.output_sa(os); break;
  case STG:   sa.output_stg(os); break;
  default:    assert(false); break;
  }

  return os;
}

}
