/*!
 * \file    serviceautomaton.cc
 *
 * \author  Stephan Mennicke <stephan.mennicke@uni-rostock.de>
 */

#include <cmath>
#include <iostream>
#include "util.h"
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
 */
Automaton::Automaton(PetriNet &n) :
  hashTable(HASHSIZE), net(n), initialmarking(*new Marking(n))
{
  primes.resize(initialmarking.size());

  fillPrimes();
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
 * The method normalizes the given net and saves for each
 * transition the (interface-)reason to fire. If there is
 * no interface place from/to one transition, the reason
 * will be a 'tau'.
 */
void Automaton::initialize()
{
  net.normalize();

  set<Place *> Pin = net.getInputPlaces();
  set<Place *> Pout = net.getOutputPlaces();

  set<Transition *> done;

  for (set<Place *>::const_iterator ip = Pin.begin(); ip != Pin.end(); ip++)
    for (set<Node *>::const_iterator it = (*ip)->getPostset().begin(); it
        != (*ip)->getPostset().end(); it++)
    {
      Transition *t = static_cast<Transition *> (*it);
      reasons[t] = "?" + (*ip)->getName();
      in.push_back((*ip)->getName());
      done.insert(t);
    }
  for (set<Place *>::const_iterator op = Pout.begin(); op != Pout.end(); op++)
    for (set<Node *>::const_iterator ot = (*op)->getPreset().begin(); ot
        != (*op)->getPreset().end(); ot++)
    {
      Transition *t = static_cast<Transition *>(*ot);
      reasons[t] = "!" + (*op)->getName();
      out.push_back((*op)->getName());
      done.insert(t);
    }

  set<Transition *> notdone = util::setDifference(net.getTransitions(), done);
  for (set<Transition *>::const_iterator t = notdone.begin(); t != notdone.end(); t++)
    reasons[*t] = "tau";

  // FIXME: Niels doesn't want this method: just don't use the interface places
  //net.deleteInterfacePlaces();
}




/*!
 * \brief   creates the service automaton
 *
 * Sets an initial state and starts the depth first
 * search. Afterwards, the property first gets its
 * value.
 */
void Automaton::createAutomaton()
{
  State *i = new State(initialmarking);

  dfs(*i);

  first = i->getIndex();
}

void Automaton::fillPrimes()
{
  unsigned int i = 0;
  unsigned int p = 2;
  while (i < primes.size())
  {
    primes[i] = p;
    i++;
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
  unsigned int hash = hashValue(i);
  i.setIndex();

  // collision
  for (set<State *>::const_iterator s = hashTable[hash].begin(); s != hashTable[hash].end(); s++)
  {
    if (**s == i)
      return;
  }

  hashTable[hash].insert(&i);
  Marking m = i.getMarking();

  // final state
  if (net.checkFinalCondition(m))
  {
    finals.push_back(i.getIndex());
    return;
  }

  set<Transition *> T = net.getTransitions();
  for (set<Transition *>::const_iterator t = T.begin(); t != T.end(); t++)
  {
    if (!net.activates(m, **t))
      continue;

    State *j = new State(net.successorMarking(m, *t));
    if (i == *j)
      continue;

    i.addSuccessor(*j);
    i.addReason(reasons[*t]);
    dfs(*j);
  }
}

ostream &operator <<(ostream &os, const Automaton &sa)
{
  os << "INTERFACE\n";
    os << "  " << "INPUT ";
    list<string> lin = sa.in;
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
    list<string> lout = sa.out;
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
    for (unsigned int n = 0; n < sa.hashTable.size(); n++)
      for (set<State *>::iterator s = sa.hashTable[n].begin(); s != sa.hashTable[n].end(); s++)
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
    os << "  " << sa.first << ";\n\n";

  os << "FINALNODES\n";
    list<unsigned int> finals = sa.finals;
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
    list<string> reas = c->getReason();
    while (!succ.empty())
    {
      if (!written)
      {
        written = true;
        os << "  " << c->getIndex() << " -> " << succ.front()->getIndex()
          << " : " << reas.front();
        succ.pop_front();
        reas.pop_front();
        continue;
      }
      os << ",\n  " << c->getIndex() << " -> " << succ.front()->getIndex()
        << " : " << reas.front();
      succ.pop_front();
      reas.pop_front();
    }
  }
  os << ";\n";

  return os;
}

}
