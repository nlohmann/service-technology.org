#include "automaton.h"
#include "component.h"
#include "marking.h"
#include "petrinet.h"
#include "state.h"

#include <iostream>
#include "io.h"


namespace pnapi
{


  /*!
   */
  Automaton::Automaton()
  {
    /* do nothing */
  }


  /*!
   * Transformation: PetriNet => Automaton.
   * First, the Petri net is normalized to avoid collisions.
   * Second, it is necessary to save the transition's type (see state.h class
   * Edge). Third, the marking is gained from the net to create the initial
   * state. At last the DEPTH-FIRST-SEARCH is started and results in a fully
   * built automaton from a Petri Net.
   */
  Automaton::Automaton(PetriNet &net)
  {
    net_ = new PetriNet(net);
    edgeLabels_ = new std::map<Transition *, std::string>();
    edgeTypes_ = new std::map<Transition *, Node::Type>();
    hashTable_ = new std::vector<std::set<State *> >(HASH_SIZE);

    (*edgeLabels_) = net_->normalize();
    for (std::set<Transition *>::iterator t = net_->getTransitions().begin();
        t != net_->getTransitions().end(); t++)
      (*edgeTypes_)[*t] = (**t).getType();
    net_->makeInnerStructure();

    State &start = createState(*new Marking(*net_));
    dfs(start);
  }


  /*!
   * The standard copy constructor copies the everything except for
   * properties which marked as optional in the according header file.
   */
  Automaton::Automaton(const Automaton &a) :
    states_(a.states_), edges_(a.edges_)
  {
    if (net_ == NULL)
      net_ = NULL;
    else
      net_ = new PetriNet(*net_);
    edgeTypes_ = NULL;
  }


  /*!
   */
  Automaton::~Automaton()
  {
    if (net_ != NULL)
      delete net_;
    if (edgeLabels_ != NULL)
      delete edgeLabels_;
    if (edgeTypes_ != NULL)
      delete edgeTypes_;
    if (hashTable_ != NULL)
      delete hashTable_;
  }


  /*!
   */
  State & Automaton::createState(const std::string name)
  {
    State *s = new State(name);
    states_.push_back(s);
    return *s;
  }


  /*!
   */
  State & Automaton::createState(Marking &m, const std::string name)
  {
    State *s = new State(m, name);
    states_.push_back(s);
    return *s;
  }


  /*!
   */
  State * Automaton::findState(const std::string name) const
  {
    for (unsigned int i = 0; i < states_.size(); i++)
      if (states_[i]->name() == name)
        return states_[i];

    return NULL;
  }


  /*!
   */
  Edge & Automaton::createEdge(State &s1, State &s2)
  {
    Edge *e = new Edge(s1, s2);
    edges_.push_back(e);
    return *e;
  }


  /*!
   */
  Edge & Automaton::createEdge(State &s1, State &s2,
    const std::string label)
  {
    Edge *e = new Edge(s1, s2, label);
    edges_.push_back(e);
    return *e;
  }


  /*!
   */
  Edge & Automaton::createEdge(State &s1, State &s2,
    const std::string label, Node::Type type)
  {
    Edge *e = new Edge(s1, s2, label, type);
    edges_.push_back(e);
    return *e;
  }


  /*!
   * \brief Transforms the automaton to a state machine petri net.
   *
   * States become places, edges become transitions, initial states
   * will be initially marked and final states will be connected
   * disjunctive in the final condition.
   *
   */
  PetriNet & Automaton::stateMachine() const
  {
    PetriNet *result_ = new PetriNet(); // resulting net
    std::map<State*,Place*> state2place_; // places by states

    Condition final_;
    final_ = false; // final places

    /* no comment */

    if (states_.empty())
      return *result_;

    // mark first state initially (see assumtion above)
    state2place_[states_[0]] = &(result_->createPlace("",Node::INTERNAL,1));
    if (states_[0]->isFinal())
      final_ = final_.formula() || (*(state2place_[states_[0]])) == 1;

    // generate places from states
    for(unsigned int i=1; i < states_.size(); ++i)
    {
      Place* p = &(result_->createPlace());
      state2place_[states_[i]] = p;

      /*
       * if the state is final then the according place
       * has to be in the final marking.
       */
      if(states_[i]->isFinal())
        final_ = final_.formula() || (*(state2place_[states_[i]])) == 1;
    }

    // generate transitions from edges
    for(unsigned int i=0; i < edges_.size(); ++i)
    {
      Transition* t = &(result_->createTransition());

      Place* p = state2place_[&(edges_[i]->source())];
      result_->createArc(*p,*t);

      p = state2place_[&(edges_[i]->destination())];
      result_->createArc(*t,*p);
    }

    // generate final condition;
    /// \todo addd all other places empty
    result_->finalCondition() = final_.formula();

    return *result_;
  }


  /*!
   * The initial states are those states which have no preset.
   * In basic service automata there is only one state which
   * is called the initial state.
   *
   * \return    std::set<State *> result - set of initial states
   */
  const std::set<State *> Automaton::initialStates() const
  {
    std::set<State *> result;
    result.clear();
    for (unsigned int i = 0; i < states_.size(); i++)
      if (states_[i]->preset().empty())
        result.insert(states_[i]);

    return result;
  }


  /*!
   * The final states are those which are flagged as final. You can trigger
   * this flag by calling State::final() on a state object.
   *
   * \return    std::set<State *> result - set of final states
   */
  const std::set<State *> Automaton::finalStates() const
  {
    std::set<State *> result;
    result.clear();
    for (unsigned int i = 0; i < states_.size(); i++)
      if (states_[i]->isFinal())
        result.insert(states_[i]);

    return result;
  }


  /*!
   */
  const std::set<std::string> Automaton::input() const
  {
    std::set<std::string> result;
    result.clear();
    for (std::map<Transition *, Node::Type>::const_iterator
        p = (*edgeTypes_).begin(); p != (*edgeTypes_).end(); p++)
      if (p->second == Node::INPUT)
        result.insert((*edgeLabels_)[p->first]);

    return result;
  }


  /*!
   */
  const std::set<std::string> Automaton::output() const
  {
    std::set<std::string> result;
    result.clear();
    for (std::map<Transition *, Node::Type>::const_iterator
        p = (*edgeTypes_).begin(); p != (*edgeTypes_).end(); p++)
      if (p->second == Node::OUTPUT)
        result.insert((*edgeLabels_)[p->first]);

    return result;
  }


  /*!
   * Special depth first search method for service automaton creation
   * from Petri net. It's a recursive method which takes a State (named
   * start) and then tries to find its successors.
   *
   * \param     State &start
   */
  void Automaton::dfs(State &start)
  {
    (*hashTable_)[start.hashValue()].insert(&start);
    // assuming that each state has a marking
    Marking m = *start.marking();

    // final state
    if (net_->finalCondition().isSatisfied(m))
    {
      start.final();
      return;
    }

    bool doubled;
    // iterate over all transitions to check if they can fire
    for (std::set<Transition *>::const_iterator
        t = net_->getTransitions().begin(); t != net_->getTransitions().end();
        t++)
    {
      if (!m.activates(**t))
        continue;

      State &j = createState(m.successor(**t));
      if (start == j)
      {
        deleteState(&j);
        continue;
      }

      // collision detection
      doubled = false;
      for (std::set<State *>::const_iterator s =
          (*hashTable_)[j.hashValue()].begin();
          s != (*hashTable_)[j.hashValue()].end(); s++)
      {
        if (**s == j)
        {
          doubled = true;
          createEdge(start, **s, (*edgeLabels_)[*t], (*edgeTypes_)[*t]);
          break;
        }
      }

      if (doubled)
      {
        deleteState(&j);
        continue;
      }
      createEdge(start, j, (*edgeLabels_)[*t], (*edgeTypes_)[*t]);

      dfs(j);
    }
  }


  /*!
   *
   */
  void Automaton::deleteState(State *s)
  {
    if (s->preset().empty() && s->postset().empty())
    {
      if (states_[states_.size()-1] == s)
        states_.pop_back();
      else
        for (unsigned int i = 0; i < states_.size()-2; i++)
          if (states_[i] == s)
          {
            states_[i] = states_[states_.size()-1];
            states_.pop_back();
          }
    }
  }


} /* END OF NAMESPACE pnapi */

