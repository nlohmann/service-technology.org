#include "peerautomaton.h"
#include "verbose.h"

#include <climits>
#include <set>
#include <sstream>

#include <iostream>


using std::endl;
using std::map;
using std::ostream;
using std::set;
using std::string;
using std::stringstream;
using std::vector;


PeerAutomaton::PeerAutomaton()
{
}


PeerAutomaton::~PeerAutomaton()
{
  for (set<PEdge *>::iterator e = edges_.begin(); e != edges_.end(); ++e)
    delete (*e);
  edges_.clear();
}


void PeerAutomaton::pushState(set<int> q)
{
  states_.insert(states_.end(), q);
}


PEdge * PeerAutomaton::createEdge(set<int> source, string label,
    set<int> destination, EventType type)
{
  PEdge *e = new PEdge();
  e->source = source;
  e->label = label;
  e->destination = destination;
  e->type = type;

  for (set<PEdge *>::iterator ee = edges_.begin(); ee != edges_.end(); ee++)
    if (e->source == (*ee)->source && e->destination == (*ee)->destination &&
        e->label == (*ee)->label && e->type == (*ee)->type)
      return e;
  edges_.insert(e);

  return e;
}


void PeerAutomaton::setInitialState(set<int> q0)
{
  initialState_ = q0;
}


void PeerAutomaton::pushFinalState(set<int> qf)
{
  finalStates_.insert(qf);
}


bool PeerAutomaton::isState(int q) const
{
  for (set<set<int> >::iterator qq = states_.begin();
      qq != states_.end(); qq++)
    if (qq->count(q) > 0)
      return true;
  return false;
}


/*
 * \brief This method finds a set of states which represents a new
 *        state in a projection that contains the state sq.
 */
const set<int> PeerAutomaton::findState(int sq) const
{
  for (set<set<int> >::iterator q = states_.begin(); q != states_.end(); q++)
    if (q->count(sq))
      return *q;
  return set<int>();
}


bool PeerAutomaton::isFinal(set<int> q) const
{
  if (finalStates_.count(q) > 0)
    return true;
  return false;
}


const set<set<int> > & PeerAutomaton::states() const
{
  return states_;
}


const set<PEdge *> PeerAutomaton::edgesFrom(set<int> source) const
{
  static map<set<int>, set<PEdge *> > cache;
  if (cache.size() == 0)
    cache[set<int>()] = edges_;
  if (cache[set<int>()] != edges_)
  {
    cache.clear();
    cache[set<int>()] = edges_;
  }
  if (cache.count(source) > 0)
    return cache[source];

  set<PEdge *> result;
  for (set<PEdge *>::iterator e = edges_.begin(); e != edges_.end(); e++)
    if ((*e)->source == source)
      result.insert(*e);
  cache[source] = result;
  return result;
}


bool PeerAutomaton::haveInput() const
{
  return !input_.empty();
}


bool PeerAutomaton::haveOutput() const
{
  return !output_.empty();
}


bool PeerAutomaton::haveSynchronous() const
{
  return !synchronous_.empty();
}


void PeerAutomaton::pushInput(const string & in)
{
  input_.insert(in);
}


const set<string> & PeerAutomaton::input() const
{
  return input_;
}


void PeerAutomaton::pushOutput(const string & out)
{
  output_.insert(out);
}


const set<string> & PeerAutomaton::output() const
{
  return output_;
}


void PeerAutomaton::pushSynchronous(const string & syn)
{
  synchronous_.insert(syn);
}


const set<string> & PeerAutomaton::synchronous() const
{
  return synchronous_;
}
