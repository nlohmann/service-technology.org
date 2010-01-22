#include "choreography.h"
#include "verbose.h"
#include <climits>
#include <map>

using std::map;
using std::set;
using std::string;
using std::vector;

#define DEBUG
#ifdef DEBUG
#include <iostream>
using std::cerr;
using std::endl;
#endif

/*!
 * \brief Standard constructor
 */
Choreography::Choreography() :
  initialState_(INT_MIN)
{
}

/*!
 * \brief Standard destructor
 */
Choreography::~Choreography()
{
  for (set<Edge *>::iterator e = edges_.begin(); e != edges_.end(); ++e)
    delete (*e);
  edges_.clear();
  for (int i = 0; i < (int) collaboration_.size(); ++i)
    delete collaboration_[i];
  collaboration_.clear();
}

/*!
 * \brief Create a state with a fresh name
 */
int Choreography::createState()
{
  static int max = -1;
  if (max >= 0)
  {
    states_.insert(states_.end(), ++max);
    return max;
  }
  for (set<int>::iterator i = states_.begin(); i != states_.end(); ++i)
    max = max < *i ? *i : max;
  states_.insert(states_.end(), ++max);
  return max;
}

/*!
 * \brief Add a state to the set of states
 *
 * \param q
 */
void Choreography::pushState(int q)
{
  states_.insert(states_.end(), q);
}

/*!
 * \brief Delete a state from the set of states
 *
 * The method also handles edges from and to the
 * deleted state.
 *
 * \param q
 */
void Choreography::deleteState(int q)
{
  states_.erase(q);
  if (isFinal(q))
  {
    finalStates_.erase(q);
  }
  if (q == initialState_)
  {
    initialState_ = INT_MIN;
  }
  for (set<Edge *>::iterator e = edges_.begin(); e != edges_.end(); ++e)
    if ((*e)->destination == q || (*e)->source == q)
      deleteEdge(*e);
}

/*!
 * \brief Set the initial state to the given value
 *
 * \param q
 */
void Choreography::setInitialState(int q)
{
  initialState_ = q;
}

/*!
 * \brief Add the given state to the set of final states
 *
 * \param q
 */
void Choreography::pushFinalState(int q)
{
  finalStates_.insert(q);
}

/*!
 * \brief Create a new edge
 *
 * \param source
 * \param label
 * \param destination
 * \param type
 */
Edge * Choreography::createEdge(int source, const string & label,
    int destination, EventType type)
{
  Edge *e = new Edge();

  e->source = source;
  e->label = label;
  e->destination = destination;
  e->type = type;

  for (set<Edge *>::iterator ee = edges_.begin(); ee != edges_.end(); ++ee)
    if (e->source == (*ee)->source && e->label == (*ee)->label
        && e->destination == (*ee)->destination && e->type == (*ee)->type)
      return *ee;
  edges_.insert(edges_.end(), e);

  return e;
}

/*!
 * \brief Delete an edge
 *
 * \param e
 */
#include <iostream>
void Choreography::deleteEdge(Edge * e)
{
  edges_.erase(e);
  delete e;
}

/*!
 * \brief Add the given peer to the collaboration
 *
 * \param p
 */
void Choreography::pushPeer(Peer * p)
{
  collaboration_.push_back(p);
}

/*!
 * \brief Add the given event to the events vector
 *
 * \param e
 */
void Choreography::pushEvent(const string & e)
{
  events_.push_back(e);
}

/*!
 * \brief Return the set of states
 */
const set<int> & Choreography::states() const
{
  return states_;
}

/*!
 * \brief Return the initial state
 */
int Choreography::initialState() const
{
  return initialState_;
}

/*!
 * \brief This method finds a state qa with q -a-> qa
 */
int Choreography::findState(int q, int a) const
{
  set<Edge *> qE = edgesFrom(q);
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); ++e)
    if ((*e)->label == events_[a])
      return (*e)->destination;
  return -1;
}

/*!
 * \brief See PeerAutomaton::findState(int q, int a) const
 */
int Choreography::findState(int q, const string & a) const
{
  set<Edge *> qE = edgesFrom(q);
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); ++e)
    if ((*e)->label == a)
      return (*e)->destination;
  return -1;
}

/*!
 * \brief This method finds a state qab with q -a-> qa -b-> qab
 */
int Choreography::findState(int q, int a, int b) const
{
  int qa = findState(q, a);
  if (qa == -1)
    return -1;
  set<Edge *> qaE = edgesFrom(qa);
  for (set<Edge *>::iterator e = qaE.begin(); e != qaE.end(); ++e)
    if ((*e)->label == events_[b])
      return (*e)->destination;
  return -1;
}

/*!
 * \brief Return the set of edges
 */
const set<Edge *> & Choreography::edges() const
{
  return edges_;
}

/*!
 * \brief Return the set of edges where q is the source state
 *
 * \param q
 */
const set<Edge *> Choreography::edgesFrom(int q) const
{
  // handling cache structure
  static map<int, set<Edge *> > cache;
  if (cache.size() == 0)
    cache[-1] = edges_;
  if (cache[-1] != edges_)
  {
    cache.clear();
    cache[-1] = edges_;
  }
  if (cache.count(q))
    return cache[q];

  // computing edges from q
  set<Edge *> result;
  for (set<Edge *>::iterator e = edges_.begin(); e != edges_.end(); ++e)
    if ((*e)->source == q)
      result.insert(*e);

  cache[q] = result;
  return result;
}

/*!
 * \brief Return the set of edges where one q' in q is the source state
 */
const set<Edge *> Choreography::edgesFrom(set<int> q) const
{
  // handling cache structure
  static map<set<int> , set<Edge *> > cache;
  if (cache.size() == 0)
    cache[set<int> ()] = edges_;
  if (cache[set<int> ()] != edges_)
  {
    cache.clear();
    cache[set<int> ()] = edges_;
  }
  if (cache.count(q))
    return cache[q];

  set<Edge *> result;
  for (set<Edge *>::iterator e = edges_.begin(); e != edges_.end(); ++e)
    for (set<int>::iterator qprime = q.begin(); qprime != q.end(); ++qprime)
      if ((*e)->source == *qprime)
        result.insert(*e);

  cache[q] = result;
  return result;
}

/*!
 * \brief Return the set of edges where q is the destination state
 *
 * \param q
 */
const set<Edge *> Choreography::edgesTo(int q) const
{
  // handling cache structure
  static map<int, set<Edge *> > cache;
  if (cache.size() == 0)
    cache[-1] = edges_;
  if (cache[-1] != edges_)
  {
    cache.clear();
    cache[-1] = edges_;
  }
  if (cache.count(q))
    return cache[q];

  // computing edges to q
  set<Edge *> result;
  for (set<Edge *>::iterator e = edges_.begin(); e != edges_.end(); ++e)
    if ((*e)->destination == q)
      result.insert(*e);

  cache[q] = result;
  return result;
}

/*!
 * \brief Return the collaboration
 */
const std::vector<Peer *> & Choreography::collaboration() const
{
  return collaboration_;
}

/*!
 * \brief Return the events
 */
const std::vector<std::string> & Choreography::events() const
{
  return events_;
}

/*!
 * \brief Checks whether \f$q_f \in\f$ finalStates_
 *
 * \param q
 */
bool Choreography::isFinal(int q) const
{
  return finalStates_.count(q);
}

/*!
 * \brief Checks whether one state in q is a final state
 */
bool Choreography::isFinal(const set<int> & qf) const
{
  for (set<int>::iterator q = qf.begin(); q != qf.end(); ++q)
    if (isFinal(*q))
      return true;
  return false;
}

/*!
 * \brief Return a 2-dimensional array to quickly check whether two events
 *        are distant.
 */
bool ** Choreography::distantEvents() const
{
  bool ** result = new bool *[events_.size()];
  for (int i = 0; i < (int) events_.size(); ++i)
  {
    result[i] = new bool[events_.size()];
    result[i][i] = false;
  }
  for (int i = 0; i < (int) events_.size(); ++i)
    for (int j = i + 1; j < (int) events_.size(); ++j)
      result[i][j] = result[j][i] = distant(events_[i], events_[j]);

  return result;
}

/*!
 * \brief Return true if the given events are distant
 *
 * Two message events \f$a,b \in\f$ events_ are distant iff
 * there exists no peer \f$[I,O] \in\f$ collaboration_ such
 * that \f$\{a,b\}\subseteq(I\cup O)\f$.
 *
 * \param a
 * \param b
 */
bool Choreography::distant(const string & a, const string & b) const
{
  for (int i = 0; i < (int) collaboration_.size(); ++i)
    if (collaboration_[i]->output().count(a)
        || collaboration_[i]->input().count(a))
      if (collaboration_[i]->output().count(b)
          || collaboration_[i]->input().count(b))
      {
        return false;
      }
  return true;
}

/*!
 * \brief Checks whether a enables b in q
 *
 * Let \f$a,b\in\f$ events_ be distant messages. \f$a\f$ enables
 * \f$b\f$ in \f$q\in\f$ states_, if there exist states \f$q_a, q_{ab}\in\f$
 * states_ with \f$q \rightarrow^a q_a \rightarrow^b q_{ab}\f$,
 * but there exists no state \f$q_b\in\f$ states_ with \f$q\rightarrow^b q_b\f$.
 *
 * \param state
 * \param a
 * \param b
 *
 * \note Pay attention! This method does not check whether a and b
 *       are distant messages.
 */
bool Choreography::enables(int state, int a, int b) const
{
  if (events_[a].substr(1, events_[a].size()) == events_[b].substr(1,
      events_[b].size()))
    return false;
  set<Edge *> qE = edgesFrom(state);
  bool qabExists = false;
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); ++e)
  {
    if ((*e)->label == events_[b])
      return false;

    if ((*e)->label == events_[a])
    {
      set<Edge *> qaE = edgesFrom((*e)->destination);
      for (set<Edge *>::iterator ea = qaE.begin(); ea != qaE.end(); ++ea)
        if ((*ea)->label == events_[b])
          qabExists = true;
    }
  }
  return qabExists;
}

/*!
 * \brief Checks whether a disables b in state q
 *
 * \note Pay attention! This method does not check whether a and b
 *       are distant messages.
 */
bool Choreography::disables(int state, int a, int b) const
{
  set<Edge *> qE = edgesFrom(state);
  bool qaExists, qbExists;
  qaExists = qbExists = false;
  for (set<Edge *>::iterator e = qE.begin(); e != qE.end(); ++e)
  {
    if ((*e)->label == events_[a])
    {
      qaExists = true;
      set<Edge *> qaE = edgesFrom((*e)->destination);
      for (set<Edge *>::iterator ea = qaE.begin(); ea != qaE.end(); ++ea)
        if ((*ea)->label == events_[b])
          return false;
    }
    if ((*e)->label == events_[b])
      qbExists = true;
  }
  return qaExists && qbExists;
}

/*!
 * \brief Checks whether qa and qb are equivalent
 */
bool Choreography::equivalent(int qa, int qb) const
{
  /// Basis:
  if (qa == qb)
    return true;

  if ((finalStates_.count(qa) && !finalStates_.count(qb))
      || (finalStates_.count(qb) && !finalStates_.count(qa)))
    return false;

  /// Induction:
  bool result = true;
  set<string> seen;
  set<Edge *> Eqa = edgesFrom(qa);
  for (set<Edge *>::iterator e = Eqa.begin(); e != Eqa.end(); ++e)
  {
    int qax = (*e)->destination;
    int qbx = findState(qb, (*e)->label);
    seen.insert((*e)->label);
    if (qbx == -1)
      return false;
    result = result && equivalent(qax, qbx);
  }
  set<Edge *> Eqb = edgesFrom(qb);
  for (set<Edge *>::iterator e = Eqb.begin(); e != Eqb.end(); ++e)
  {
    if (!seen.count((*e)->label))
      return false;
  }
  return result;
}

set<int> & Choreography::closure(set<int> & S, int peer) const
{
  const Peer * p = collaboration_[peer];
  for (set<int>::iterator q = S.begin(); q != S.end(); ++q)
  {
    set<Edge *> E = edgesFrom(*q);
    for (set<Edge *>::iterator eq = E.begin(); eq != E.end(); ++eq)
    {
      if (!S.count((*eq)->destination))
      {
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
        {
          if (!p->output().count((*eq)->label) && !p->input().count(
              (*eq)->label))
          {
            S.insert((*eq)->destination);
            S = closure(S, peer);
          }
          break;
        }
        default:
        {
          S.insert((*eq)->destination);
          S = closure(S, peer);
          break;
        }
        }
      }
    }
  }
  return S;
}

void Choreography::unite(int qa, int qb)
{
  set<Edge *> qaE = edgesFrom(qa);
  set<Edge *> qbE = edgesFrom(qb);
  set<Edge *> rm;
  for (set<Edge *>::iterator e = qaE.begin(); e != qaE.end(); ++e)
    rm.insert(*e);
  for (set<Edge *>::iterator e = qbE.begin(); e != qbE.end(); ++e)
  {
    bool found = false;
    for (set<Edge *>::iterator ee = rm.begin(); ee != rm.end(); ++ee)
      if ((*e)->label == (*ee)->label && (*e)->label != "")
      {
        rm.erase(*ee);
        found = true;
      }
    if (!found)
      rm.insert(*e);
  }
  for (set<Edge *>::iterator e = rm.begin(); e != rm.end(); ++e)
  {
    std::cout << "unite: not equivalent ways" << std::endl;
    deleteEdge(*e);
  }
  // recursive call
  qaE = edgesFrom(qa);
  qbE = edgesFrom(qb);
  for (set<Edge *>::iterator ea = qaE.begin(); ea != qaE.end(); ++ea)
    for (set<Edge *>::iterator eb = qbE.begin(); eb != qbE.end(); ++eb)
      if ((*ea)->label == (*eb)->label)
      {
        unite((*ea)->destination, (*eb)->destination);
        break;
      }
  // merge qa and qb
  set<Edge *> qbEt = edgesTo(qb);
  for (set<Edge *>::iterator e = qbEt.begin(); e != qbEt.end(); ++e)
  {
    createEdge((*e)->source, (*e)->label, qa, (*e)->type);
  }
  deleteState(qb);
}

bool Choreography::isChoreography() const
{
  set<string> inChannels, outChannels;
  bool isChor = false;
  for (int i = 0; i < (int) collaboration_.size(); ++i)
  {
    for (set<string>::iterator in = collaboration_[i]->in().begin(); in
        != collaboration_[i]->in().end(); ++in)
      if (outChannels.count(*in) > 0)
        outChannels.erase(*in);
      else
        inChannels.insert(*in);
    for (set<string>::iterator out = collaboration_[i]->out().begin(); out
        != collaboration_[i]->out().end(); ++out)
      if (inChannels.count(*out) > 0)
        inChannels.erase(*out);
      else
        outChannels.insert(*out);
  }

  if (inChannels.empty() && outChannels.empty())
    isChor = true;
  else
  {
    for (set<string>::iterator in = inChannels.begin(); in != inChannels.end(); ++in)
      status("'%s' has no appropriate output event", in->c_str());
    for (set<string>::iterator out = outChannels.begin(); out
        != outChannels.end(); ++out)
      status("'%s' has no appropriate input event", out->c_str());
  }

  // TODO: conversation check is missing

  return isChor;
}

/*!
 * This method resolves deadlocks by deleting such states that do not
 * lead to a final state. This is not the general term of resolving
 * but it suffices the needs of the application of realizability.
 */
bool Choreography::resolveDeadlocks()
{
  bool changed = false;

/*  if (finalStates_.empty() && !states_.empty())
  {
    std::set<int> tempStates = states_;
    for (set<int>::iterator i = tempStates.begin(); i != tempStates.end(); ++i)
    {
      deleteState(*i);
    }
    changed = true;
  }
  else
  {
    if (states_.empty())
      return false*/;
    vector<int> toBeDeleted;
    if (defined(initialState_))
      bfs(initialState_, toBeDeleted);
    for (int i = 0; i < (int) toBeDeleted.size(); ++i)
    {
      deleteState(toBeDeleted[i]);
      changed = true;
    }
//  }

  return changed;
}

bool Choreography::bfs(int q, vector<int> &toBeDeleted)
{
#ifdef DEBUG
  cerr << "bfs: checking " << q << endl;
#endif
  if (isFinal(q))
  {
    return true;
  }
  set<Edge *> edges = edgesFrom(q);
  if (edges.empty())
  {
    toBeDeleted.push_back(q);
    return false;
  }
  bool result = false;
  for (set<Edge *>::iterator e = edges.begin(); e != edges.end(); ++e)
  {
    result = bfs((*e)->destination, toBeDeleted) || result;
  }
  if (!result)
  {
    toBeDeleted.push_back(q);
  }
  return result;
}

/*!
 * Checks whether the given state is not INT_MIN (undefined state).
 */
bool Choreography::defined(int q)
{
  return q != INT_MIN;
}
