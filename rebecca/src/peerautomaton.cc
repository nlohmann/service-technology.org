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


PeerAutomaton::PeerAutomaton(PeerAutomatonType type) :
  equivalenceClasses_(NULL), type_(type)
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
    input_ = new set<string>();
    output_ = new set<string>();
    synchronous_ = new set<string>();
    break;
  default:
    abort(11, "unknown peer automaton type");
  }
}


PeerAutomaton::~PeerAutomaton()
{
  switch (type_)
  {
  case CHOREOGRAPHY:
    delete states_;
    delete initialState_;
    delete finalStates_;
    delete edges_;
    break;
  case PROJECTION:
    delete stateSets_;
    delete initialStateSet_;
    delete finalStateSets_;
    delete pEdges_;
    break;
  default:
    abort(11, "unknown peer automaton type");
  }
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

  for (set<Edge *>::iterator ee = edges_->begin(); ee != edges_->end(); ee++)
    if (e->source == (*ee)->source && e->destination == (*ee)->destination &&
        e->label == (*ee)->label && e->type == (*ee)->type)
      return e;
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

  for (set<PEdge *>::iterator ee = pEdges_->begin(); ee != pEdges_->end(); ee++)
    if (e->source == (*ee)->source && e->destination == (*ee)->destination &&
        e->label == (*ee)->label && e->type == (*ee)->type)
      return e;
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


bool PeerAutomaton::isChoreography() const
{
  set<string> inChannels, outChannels;
  bool isChor = false;
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
    isChor = true;
  else
  {
    for (set<string>::iterator in = inChannels.begin(); in != inChannels.end();
        in++)
      status("'%s' has no appropriate output event", in->c_str());
    for (set<string>::iterator out = outChannels.begin(); out
        != outChannels.end(); out++)
      status("'%s' has no appropriate input event", out->c_str());
  }

  // TODO: conversation check is missing

  return isChor;
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
 * This method finds a state qa with q -a-> qa
 */
const int PeerAutomaton::findState(int q, int a) const
{
  set<Edge *> qE = edgesFrom(q);
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); e++)
    if ((*e)->label == events_[a])
      return (*e)->destination;
  return -1;
}


/*
 * See PeerAutomaton::findState(int q, int a) const
 */
const int PeerAutomaton::findState(int q, const string & a) const
{
  set<Edge *> qE = edgesFrom(q);
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); e++)
    if ((*e)->label == a)
      return (*e)->destination;
  return -1;
}


/*
 * This method finds a state qab with q -a-> qa -b-> qab
 */
const int PeerAutomaton::findState(int q, int a, int b) const
{
  int qa = findState(q, a);
  if (qa == -1)
    return -1;
  set<Edge *> qaE = edgesFrom(qa);
  for (set<Edge *>::iterator e = qaE.begin(); e != qaE.end(); e++)
    if ((*e)->label == events_[b])
      return (*e)->destination;
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
  if (!initialState_)
    return INT_MIN;
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
  return false;
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


const set<Edge *> PeerAutomaton::edgesFrom(int source) const
{
  static map<int, set<Edge *> > cache;
  if (cache.size() == 0)
    cache[-1] = *edges_;
  if (cache[-1] != *edges_)
  {
    cache.clear();
    cache[-1] = *edges_;
  }
  if (cache.count(source))
    return cache[source];

  set<Edge *> result;
  for (set<Edge *>::iterator e = edges_->begin(); e != edges_->end(); e++)
    if ((*e)->source == source)
      result.insert(*e);

  cache[source] = result;
  return result;
}


const set<Edge *> PeerAutomaton::edgesTo(int destination) const
{
  static map<int, set<Edge *> > cache;
  if (cache.size() == 0)
    cache[-1] = *edges_;
  if (cache[-1] != *edges_)
  {
    cache.clear();
    cache[-1] = *edges_;
  }
  if (cache.count(destination))
    return cache[destination];

  set<Edge *> result;
  for (set<Edge *>::iterator e = edges_->begin(); e != edges_->end(); e++)
    if ((*e)->destination == destination)
      result.insert(*e);

  cache[destination] = result;
  return result;
}


const set<Edge *> PeerAutomaton::edges(set<int> sources) const
{
  set<Edge *> result;
  for (set<int>::iterator q = sources.begin(); q != sources.end(); q++)
  {
    set<Edge *> E = edgesFrom(*q);
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


set<int> & PeerAutomaton::closure(set<int> & S, int peer)
{
  const Peer * p = collaboration_[peer];
  for (set<int>::iterator q = S.begin(); q != S.end(); q++)
  {
    set<Edge *> E = edgesFrom(*q);
    for (set<Edge *>::iterator eq = E.begin(); eq != E.end(); eq++)
      if (!S.count((*eq)->destination))
        switch ((*eq)->type)
        {
        case RCV:
          {
            if (!p->input().count((*eq)->label))
            {
              S.insert((*eq)->destination);
              S = closure(S, peer);
            }
            break;
          }
        case SND:
          {
            if (!p->output().count((*eq)->label))
            {
              S.insert((*eq)->destination);
              S = closure(S, peer);
            }
            break;
          }
        case SYN:
        default:
          {
            S.insert((*eq)->destination);
            S = closure(S, peer);
            break;
          }
        }
  }
  return S;
}


bool PeerAutomaton::haveInput() const
{
  return !input_->empty();
}


bool PeerAutomaton::haveOutput() const
{
  return !output_->empty();
}


bool PeerAutomaton::haveSynchronous() const
{
  return !synchronous_->empty();
}


/*!
 * Pay attention! The following methods do not check whether a and b
 * are distant messages.
 */
bool PeerAutomaton::enables(int state, int a, int b) const
{
  set<Edge *> qE = edgesFrom(state);
  bool qabExists = false;
  for(set<Edge *>::iterator e = qE.begin(); e != qE.end(); e++)
  {
    if ((*e)->label == events_[b])
      return false;

    if ((*e)->label == events_[a])
    {
      set<Edge *> qaE = edgesFrom((*e)->destination);
      for (set<Edge *>::iterator ea = qaE.begin(); ea != qaE.end(); ea++)
        if ((*ea)->label == events_[b])
          qabExists = true;
    }
  }
  return qabExists;
}


bool PeerAutomaton::disables(int state, int a, int b) const
{
  set<Edge *> qE = edgesFrom(state);
  bool qaExists, qbExists;
  qaExists = qbExists = false;
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); e++)
  {
    if ((*e)->label == events_[a])
    {
      qaExists = true;
      set<Edge *> qaE = edgesFrom((*e)->destination);
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
  /// Basis:
  if (qa == qb)
    return true;

  if ((finalStates_->count(qa) && !finalStates_->count(qb))
      || (finalStates_->count(qb) && !finalStates_->count(qa)))
    return false;

  /// Induction:
  bool result = true;
  set<string> seen;
  set<Edge *> Eqa = edgesFrom(qa);
  for (set<Edge *>::iterator e = Eqa.begin(); e != Eqa.end(); e++)
  {
    int qax = (*e)->destination;
    int qbx = findState(qb, (*e)->label);
    seen.insert((*e)->label);
    if (qbx == -1)
      return false;
    result = result && equivalent(qax, qbx);
  }
  set<Edge *> Eqb = edgesFrom(qb);
  for (set<Edge *>::iterator e = Eqb.begin(); e != Eqb.end(); e++)
  {
    if (!seen.count((*e)->label))
      return false;
  }
  return result;
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
    if ((*e)->destination == q || (*e)->source == q)
      deleteTransition(*e);
  if (*initialState_ == q)
    initialState_ = NULL;
}


void PeerAutomaton::unite(int qa, int qb)
{
  set<Edge *> qaE = edgesFrom(qa);
  set<Edge *> qbE = edgesFrom(qb);
  set<Edge *> rm;
  for (set<Edge *>::iterator e = qaE.begin(); e != qaE.end(); e++)
    rm.insert(*e);
  for (set<Edge *>::iterator e = qbE.begin(); e != qbE.end(); e++)
  {
    bool found = false;
    for (set<Edge *>::iterator ee = rm.begin(); ee != rm.end(); ee++)
      if ((*e)->label == (*ee)->label)
      {
        rm.erase(ee);
        found = true;
      }
    if (!found)
      rm.insert(*e);
  }
  for (set<Edge *>::iterator e = rm.begin(); e != rm.end(); e++)
  {
    deleteTransition(*e);
  }
  // recursive call
  qaE = edgesFrom(qa);
  qbE = edgesFrom(qb);
  for (set<Edge *>::iterator ea = qaE.begin(); ea != qaE.end(); ea++)
    for (set<Edge *>::iterator eb = qbE.begin(); eb != qbE.end(); eb++)
      if ((*ea)->label == (*eb)->label)
      {
        unite((*ea)->destination, (*eb)->destination);
        break;
      }
  // merge qa and qb
  set<Edge *> qbEt = edgesTo(qb);
  for (set<Edge *>::iterator e = qbEt.begin(); e != qbEt.end(); e++)
  {
    createTransition((*e)->source, (*e)->label, qa, (*e)->type);
  }
  deleteState(qb);
}


void PeerAutomaton::pushInput(const string & in)
{
  input_->insert(in);
}


const set<string> & PeerAutomaton::input() const
{
  return *input_;
}


void PeerAutomaton::pushOutput(const string & out)
{
  output_->insert(out);
}


const set<string> & PeerAutomaton::output() const
{
  return *output_;
}


void PeerAutomaton::pushSynchronous(const string & syn)
{
  synchronous_->insert(syn);
}


const set<string> & PeerAutomaton::synchronous() const
{
  return *synchronous_;
}
