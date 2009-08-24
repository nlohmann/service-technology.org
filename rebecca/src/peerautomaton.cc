#include "peerautomaton.h"
#include "verbose.h"

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


PeerAutomaton::PeerAutomaton(PeerAutomatonType type) :
  type_(type)
{
  switch (type_)
  {
  case CHOREOGRAPHY:
    states_ = new set<int>();
    initialState_ = new int();
    finalStates_ = new set<int>();
    edges_ = new set<Edge *>();
    break;
  case PROJECTION:
    stateSets_ = new set<set<int> >();
    initialStateSet_ = new set<int>();
    finalStateSets_ = new set<set<int> >();
    pEdges_ = new set<PEdge *>();
    break;
  default:
    abort(11, "unknown peer automaton type");
  }
}


PeerAutomaton::~PeerAutomaton()
{
}


void PeerAutomaton::pushState(int q)
{
  states_->insert(q);
}


void PeerAutomaton::pushState(set<int> q)
{
  stateSets_->insert(stateSets_->end(), q);
}


int PeerAutomaton::createState()
{
  static int max = -1;
  if (max >= 0)
  {
    states_->insert(++max);
    return max;
  }
  for (set<int>::iterator i = states_->begin(); i != states_->end(); i++)
    max = max < *i ? *i : max;
  states_->insert(++max);
  return max;
}


Edge * PeerAutomaton::createTransition(int source, string label,
    int destination, EventType type)
{
  Edge *e = new Edge();
  e->source = source;
  e->label = label;
  e->destination = destination;
  e->type = type;

  edges_->insert(e);

  return e;
}

PEdge * PeerAutomaton::createTransition(set<int> source, string label,
    set<int> destination, EventType type)
{
  PEdge *e = new PEdge();
  e->source = source;
  e->label = label;
  e->destination = destination;
  e->type = type;

  pEdges_->insert(e);

  return e;
}


void PeerAutomaton::setInitialState(int q0)
{
  *initialState_ = q0;
}


void PeerAutomaton::setInitialState(set<int> q0)
{
  *initialStateSet_ = q0;
}


void PeerAutomaton::pushFinalState(int qf)
{
  finalStates_->insert(qf);
}


void PeerAutomaton::pushFinalState(set<int> qf)
{
  finalStateSets_->insert(qf);
}


void PeerAutomaton::pushPeer(const Peer * p)
{
  collaboration_.push_back(p);
}


void PeerAutomaton::pushEvent(const string & event)
{
  for (int i = 0; i < (int) events_.size(); i++)
    if (events_[i] == event)
      return;
  events_.push_back(event);
}


bool PeerAutomaton::isSane() const
{
  set<string> inChannels, outChannels;
  for (int i = 0; i < (int) collaboration_.size(); i++)
  {
    for (set<string>::iterator in = collaboration_[i]->input().begin(); in
        != collaboration_[i]->input().end(); in++)
      if (outChannels.count(*in) > 0)
        outChannels.erase(*in);
      else
        inChannels.insert(*in);
    for (set<string>::iterator out = collaboration_[i]->output().begin(); out
        != collaboration_[i]->output().end(); out++)
      if (inChannels.count(*out) > 0)
        inChannels.erase(*out);
      else
        outChannels.insert(*out);
  }

  if (inChannels.empty() && outChannels.empty())
    return true;
  else
  {
    for (set<string>::iterator in = inChannels.begin(); in != inChannels.end();
        in++)
      status("'%s' has no appropriate output event", in->c_str());
    for (set<string>::iterator out = outChannels.begin(); out
        != outChannels.end(); out++)
      status("'%s' has no appropriate input event", out->c_str());
  }

  return false;
}


bool PeerAutomaton::isState(int q) const
{
  switch (type_)
  {
  case CHOREOGRAPHY:
    return (states_->count(q) > 0);
  case PROJECTION:
    for (set<set<int> >::iterator qq = stateSets_->begin();
        qq != stateSets_->end(); qq++)
      if (qq->count(q) > 0)
        return true;
    return false;
  default:
    abort(11, "unknown peer automaton type");
  }

  return false;
}


/*
 * This method finds a set of states which represents a new
 * state in a projection that contains the state sq.
 */
const set<int> PeerAutomaton::findStateSet(int sq) const
{
  for (set<set<int> >::iterator q = stateSets_->begin(); q != stateSets_->end(); q++)
    if (q->count(sq) > 0)
      return *q;
  return set<int>();
}


/*
 * This method finds a state qab with q -a-> qa -b-> qab
 */
const int PeerAutomaton::findState(int q, int a, int b) const
{
  set<Edge *> qE = edges(q);
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); e++)
    if ((*e)->label == events_[a])
    {
      set<Edge *> qaE = edges((*e)->destination);
      for (set<Edge *>::iterator ee = qaE.begin(); ee != qaE.end(); ee++)
        if ((*ee)->label == events_[b])
          return (*ee)->destination;
    }
  return -1;
}


bool PeerAutomaton::distant(string a, string b) const
{
  for (int i = 0; i < (int) collaboration_.size(); i++)
    if (collaboration_[i]->output().count(a) > 0
        || collaboration_[i]->input().count(a) > 0)
      if (collaboration_[i]->output().count(b) > 0
          || collaboration_[i]->input().count(b) > 0)
        return false;
  return true;
}


int PeerAutomaton::initialState() const
{
  return *initialState_;
}


bool PeerAutomaton::isFinal(int q) const
{
  if (finalStates_->count(q) > 0)
    return true;
  return false;
}


bool PeerAutomaton::isFinal(set<int> q) const
{
  switch (type_)
  {
  case CHOREOGRAPHY:
    {
      for (set<int>::iterator qf = q.begin(); qf != q.end(); qf++)
        if (isFinal(*qf))
          return true;
      return false;
    }
  case PROJECTION:
    {
      if (finalStateSets_->count(q) > 0)
          return true;
      return false;
    }
  default:
    abort(11, "unknown peer automaton type");
  }
}


bool ** PeerAutomaton::distantMessages() const
{
  bool ** result = new bool * [events_.size()];
  for (int i = 0; i < (int) events_.size(); i++)
  {
    result[i] = new bool[events_.size()];
    result[i][i] = false;
  }
  for (int i = 0; i < (int) events_.size(); i++)
    for (int j = i+1; j < (int) events_.size(); j++)
      result[i][j] = result[j][i] = distant(events_[i], events_[j]);

  return result;
}


set<int> & PeerAutomaton::states() const
{
  return *states_;
}


set<set<int> > & PeerAutomaton::stateSets() const
{
  return *stateSets_;
}


const set<Edge *> PeerAutomaton::edges() const
{
  return *edges_;
}


const set<Edge *> PeerAutomaton::edges(int source) const
{
  static map<int, set<Edge *> > cache;
  if (cache.size() == 0)
    cache[-1] = *edges_;
  if (cache[-1] != *edges_)
  {
    cache.clear();
    cache[-1] = *edges_;
  }
  if (cache.count(source) > 0)
    return cache[source];

  set<Edge *> result;
  for (set<Edge *>::iterator e = edges_->begin(); e != edges_->end(); e++)
    if ((*e)->source == source)
      result.insert(*e);

  cache[source] = result;
  return result;
}


const set<Edge *> PeerAutomaton::edges(set<int> sources) const
{
  set<Edge *> result;
  for (set<int>::iterator q = sources.begin(); q != sources.end(); q++)
  {
    set<Edge *> E = edges(*q);
    for (set<Edge *>::iterator e = E.begin(); e != E.end(); e++)
      result.insert(*e);
  }
  return result;
}


const set<PEdge *> PeerAutomaton::pEdges(set<int> source) const
{
  static map<set<int>, set<PEdge *> > cache;
  if (cache.size() == 0)
    cache[set<int>()] = *pEdges_;
  if (cache[set<int>()] != *pEdges_)
  {
    cache.clear();
    cache[set<int>()] = *pEdges_;
  }
  if (cache.count(source) > 0)
    return cache[source];

  set<PEdge *> result;
  for (set<PEdge *>::iterator e = pEdges_->begin(); e != pEdges_->end(); e++)
    if ((*e)->source == source)
      result.insert(*e);
  cache[source] = result;
  return result;
}


const vector<string> & PeerAutomaton::events() const
{
  return events_;
}


const vector<const Peer *> & PeerAutomaton::collaboration() const
{
  return collaboration_;
}


#include "io.h"
set<int> & PeerAutomaton::closure(set<int> & S, int peer)
{
  const Peer * p = collaboration_[peer];
  for (set<int>::iterator q = S.begin(); q != S.end(); q++)
  {
    set<Edge *> E = edges(*q);
    for (set<Edge *>::iterator eq = E.begin(); eq != E.end(); eq++)
      if (S.count((*eq)->destination) == 0 && p->input().count((*eq)->label)
          == 0 && p->output().count((*eq)->label) == 0)
      {
        S.insert((*eq)->destination);
        S = closure(S, peer);
      }
  }
  return S;
}


bool PeerAutomaton::haveInput() const
{
  static bool *result = NULL;
  if (result != NULL)
    return *result;
  switch (type_)
  {
  case CHOREOGRAPHY:
    break;
  case PROJECTION:
    {
      for (set<PEdge *>::iterator e = pEdges_->begin(); e != pEdges_->end(); e++)
        if ((*e)->type == RCV)
        {
          result = new bool(true);
          return true;
        }
      break;
    }
  default:
    break;
  }
  result = new bool(false);
  return false;
}


bool PeerAutomaton::haveOutput() const
{
  static bool *result = NULL;
  if (result != NULL)
    return *result;
  switch (type_)
  {
  case CHOREOGRAPHY:
    break;
  case PROJECTION:
    {
      for (set<PEdge *>::iterator e = pEdges_->begin(); e != pEdges_->end(); e++)
        if ((*e)->type == RCV)
        {
          result = new bool(true);
          return true;
        }
      break;
    }
  default:
    break;
  }
  result = new bool(false);
  return false;
}


bool PeerAutomaton::haveSynchronous() const
{
  static bool *result = NULL;
  if (result != NULL)
    return *result;
  switch (type_)
  {
  case CHOREOGRAPHY:
    break;
  case PROJECTION:
    {
      for (set<PEdge *>::iterator e = pEdges_->begin(); e != pEdges_->end(); e++)
        if ((*e)->type == SYN)
        {
          result = new bool(true);
          return true;
        }
      break;
    }
  default:
    break;
  }
  result = new bool(false);
  return false;
}


/*!
 * Pay attention! The following methods do not check whether a and b
 * are distant messages.
 */
bool PeerAutomaton::enables(int state, int a, int b) const
{
  set<Edge *> qE = edges(state);
  bool qabExists = false;
  for(set<Edge *>::iterator e = qE.begin(); e != qE.end(); e++)
  {
    if ((*e)->label == events_[b])
      return false;

    if ((*e)->label == events_[a])
    {
      set<Edge *> qaE = edges((*e)->destination);
      for (set<Edge *>::iterator ea = qaE.begin(); ea != qaE.end(); ea++)
        if ((*ea)->label == events_[b])
          qabExists = true;
    }
  }
  return qabExists;
}


bool PeerAutomaton::disables(int state, int a, int b) const
{
  set<Edge *> qE = edges(state);
  bool qaExists, qbExists;
  qaExists = qbExists = false;
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); e++)
  {
    if ((*e)->label == events_[a])
    {
      qaExists = true;
      set<Edge *> qaE = edges((*e)->destination);
      for (set<Edge *>::iterator ea = qaE.begin(); ea != qaE.end(); ea++)
        if ((*ea)->label == events_[b])
          return false;
    }
    if ((*e)->label == events_[b])
      qbExists = true;
  }
  return qaExists && qbExists;
}


bool PeerAutomaton::equivalent(int qa, int qb) const
{
  return false;
}


void PeerAutomaton::deleteTransition(Edge * e)
{
  edges_->erase(e);
  delete e;
}


void PeerAutomaton::deleteState(int q)
{
  states_->erase(q);
  set<Edge *> E = *edges_;
  for (set<Edge *>::iterator e = E.begin(); e != E.end(); e++)
    if ((*e)->destination == q)
      deleteTransition(*e);
}
