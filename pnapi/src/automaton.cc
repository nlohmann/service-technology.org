#include "config.h"
#include <cassert>

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <set>
#include <iostream>
#include <cstdlib>
using std::cerr;
using std::cout;
using std::endl;
using std::flush;

#include "automaton.h"
#include "component.h"
#include "myio.h"
#include "marking.h"
#include "petrinet.h"
#include "state.h"
#include "util.h"
#include "Output.h"

namespace pnapi
{


/*!
 * The result of the constructor is an emty automaton.
 */
Automaton::Automaton() :
  edgeLabels_(NULL), edgeTypes_(NULL), net_(NULL), hashTable_(NULL), weights_(NULL), counter_(0)
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
Automaton::Automaton(PetriNet &net) :
  counter_(0)
  {
  // initializing private variables
  net_ = new PetriNet(net);
  edgeLabels_ = new std::map<Transition *, std::string>();
  edgeTypes_ = new std::map<Transition *, Type>();
  weights_ = new std::map<const Place *, unsigned int>();
  hashTable_ = new std::vector<std::set<State *> >(HASH_SIZE);

  // normalizing the copied net and retrieving the edge labels
  (*edgeLabels_) = net_->normalize();

  // preparing input and output labels
  for (std::set<Place *>::iterator p = net_->getInterfacePlaces().begin(); 
  p != net_->getInterfacePlaces().end(); ++p)
  {
    if ((*p)->getType() == Node::INPUT)
    {
      addInput((*p)->getName());
    }
    else
    {
      addOutput((*p)->getName());
    }
  }

  // giving each transition its type
  for (std::set<Transition *>::iterator t = net_->getTransitions().begin();
  t != net_->getTransitions().end(); ++t)
    switch((**t).getType())
    {
    case Node::INTERNAL:
      (*edgeTypes_)[*t] = ((*t)->isSynchronized()) ? SYNCHRONOUS : TAU;
      break;
    case Node::INPUT:
      (*edgeTypes_)[*t] = INPUT;
      break;
    case Node::OUTPUT:
      (*edgeTypes_)[*t] = OUTPUT;
      break;
    default:
      break;
    }

  // deleting all interface places
  net_->makeInnerStructure();

  // making a marking's hash value more random
  srand(time(NULL));

  //unsigned int size = net_->getPlaces().size();
  for (std::set<Place *>::iterator p = net_->getPlaces().begin();
  p != net_->getPlaces().end(); ++p)
  {
    (*weights_)[*p] = rand() % HASH_SIZE;
  }

  // creating initial state
  State & start = createState(*new Marking(*net_));
  start.initial();

  // beginning to find follow-up states
  dfs(start);

  delete edgeLabels_;
  edgeLabels_ = NULL;
  delete edgeTypes_;
  edgeTypes_ = NULL;
  delete hashTable_;
  hashTable_ = NULL;
  delete weights_;
  weights_ = NULL;
  }


/*!
 * The standard copy constructor copies the everything except for
 * properties which marked as optional in the according header file.
 */
Automaton::Automaton(const Automaton &a) :
  input_(a.input_), output_(a.output_),
  labels_(a.labels_), counter_(a.counter_),
  edgeLabels_(NULL), edgeTypes_(NULL),
  weights_(NULL), hashTable_(NULL)
  {
  map<const Place*, const Place*> placeMap;

  if (a.net_ == NULL)
  {
    net_ = NULL;
  }
  else
  {
    net_ = new PetriNet(*a.net_);
    for(set<Place*>::iterator p = a.net_->getPlaces().begin();
    p != a.net_->getPlaces().end(); ++p)
    {
      placeMap[*p] = net_->findPlace((*p)->getName());
    }
  }

  // mapping from states to their copy
  map<State*, State*> stateMap;

  // copy states
  for(unsigned int i = 0; i < a.states_.size(); ++i)
  {
    State * s = State::copy(*(a.states_[i]), net_, &placeMap);
    states_.push_back(s);
    stateMap[a.states_[i]] = s;
  }

  // copy edges
  for(unsigned int i = 0; i < a.edges_.size(); ++i)
  {
    Edge * e = a.edges_[i];
    createEdge(*(stateMap[&(e->source())]), *(stateMap[&(e->destination())]),
        e->label(), e->type());
  }
  }

/*!
 * \brief "=" operator
 */
Automaton & Automaton::operator=(const Automaton & a)
{
  assert(this != &a);

  this->~Automaton();
  return *new (this) Automaton(a);
}

/*!
 * The standard destructor deletes the optional objects created
 * while the transformation PetriNet => Automaton
 */
Automaton::~Automaton()
{
  delete net_;

  for(unsigned int i = 0; i < edges_.size(); ++i)
    delete edges_[i];

  // no need to call deleteState() since states_ is already to be deleted
  for(unsigned int i = 0; i < states_.size(); ++i)
    delete states_[i];
}


/*!
 * A state will be added to the set of states. The name given can be
 * empty, so a standard name will be set.
 *
 * \return    State &s .. the newly created state
 */
State & Automaton::createState()
{
  State *s = new State(&counter_);
  assert(s != NULL);
  states_.push_back(s);
  return *s;
}


/*!
 * A state will be added to the set of states with a given name.
 * If there exists a state with the given name, this state will
 * be returned.
 *
 * \param     const unsigned int name.
 * \return    State &s .. the newly created state.
 */
State & Automaton::createState(const unsigned int name)
{
  for (unsigned int i = 0; i < states_.size(); i++)
    if (states_[i]->name() == name)
      return *states_[i];
  State *s = new State(name);
  assert(s != NULL);
  states_.push_back(s);
  return *s;
}


/*!
 * A state will be added to the set of states. This state is based on
 * a given marking, which is needed to calculate the state's hash value.
 *
 * \param     Marking &m
 * \param     const std::string name
 *
 * \return    State &s .. the newly created state
 */
State & Automaton::createState(Marking &m)
{
  State *s = new State(m, weights_, counter_);
  assert(s != NULL);
  states_.push_back(s);
  return *s;
}


/*!
 * \brief     Finding a state by name.
 */
State * Automaton::findState(const unsigned int name) const
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
    const std::string& label, Type type)
{
  Edge *e = new Edge(s1, s2, label, type);
  edges_.push_back(e);
  switch(type)
  {
  case INPUT: addInput(label); break;
  case OUTPUT: addOutput(label); break;
  case SYNCHRONOUS: labels_.insert(label); break;
  default: break;
  }

  return *e;
}


/*!
 * \brief Transforms the automaton to a state machine Petri net.
 *
 * States become places, edges become transitions, initial states
 * will be initially marked and final states will be connected
 * disjunctive in the final condition.
 */
PetriNet Automaton::stateMachine() const
{
  PetriNet result; // resulting net
  std::map<State*,Place*> state2place; // places by states

  // caches for faster place search
  map<string, Place*> interfacePlaces;

  Condition final;
  final = false; // final places

  /* no comment */

  if (states_.empty())
    return result;

  for (std::set<std::string>::iterator i = input_.begin(); i != input_.end(); ++i)
  {
    interfacePlaces[*i] = &(result.createPlace(*i, Node::INPUT));
  }
  for (std::set<std::string>::iterator o = output_.begin(); o != output_.end(); ++o)
  {
    interfacePlaces[*o] = &(result.createPlace(*o, Node::OUTPUT));
  }

  // generate places from states
  for(unsigned int i=0; i < states_.size(); ++i)
  {
    std::stringstream ss;
    std::string id;
    ss << states_[i]->name();
    ss >> id;
    Place *p = &(result.createPlace("p"+id));
    state2place[states_[i]] = p;
    if (states_[i]->isInitial())
      p->mark();

    /*
     * if the state is final then the according place
     * has to be in the final marking.
     */
    if(states_[i]->isFinal())
      final = final.formula() || (*p) == 1;
  }

  // map synchlabel : T -> {string}
  std::map<Transition *, std::set<std::string> > synchlabel;
  // generate transitions from edges
  for(unsigned int i=0; i < edges_.size(); ++i)
  {
    set<string> syncLabel;
    if(edges_[i]->type() == Automaton::SYNCHRONOUS)
      syncLabel.insert(edges_[i]->label());

    Transition* t = &(result.createTransition("", syncLabel));

    switch (edges_[i]->type())
    {
    case Automaton::INPUT:
      result.createArc(*interfacePlaces[edges_[i]->label()], *t);
      break;
    case Automaton::OUTPUT:
      result.createArc(*t, *interfacePlaces[edges_[i]->label()]);
      break;
    default:
      break;
    }

    Place* p = state2place[&(edges_[i]->source())];
    result.createArc(*p,*t);

    p = state2place[&(edges_[i]->destination())];
    result.createArc(*t,*p);
  }

  // generate final condition
  result.finalCondition() = final.formula() && formula::ALL_OTHER_PLACES_EMPTY;

  // copy synchronous interface (how did this ever work before???)
  result.setSynchronousLabels(labels_);

  return result;
}


/*!
 * The initial states are those states which have no preset.
 * In basic service automata there is only one state which
 * is called the initial state.
 *
 * \return    result  set of initial states
 */
const std::set<State *> Automaton::initialStates() const
{
  std::set<State *> result;
  for (unsigned int i = 0; i < states_.size(); i++)
    if (states_[i]->isInitial())
      result.insert(states_[i]);

  return result;
}


/*!
 * The final states are those which are flagged as final. You can trigger
 * this flag by calling State::final() on a state object.
 *
 * \return    result - set of final states
 */
const std::set<State *> Automaton::finalStates() const
{
  std::set<State *> result;
  for (unsigned int i = 0; i < states_.size(); i++)
    if (states_[i]->isFinal())
      result.insert(states_[i]);

  return result;
}


/*!
 */
 std::set<std::string> Automaton::input() const
 {
   return input_;
 }


 /*!
  *
  */
 void Automaton::addInput(std::string label)
 {
   input_.insert(label);
 }


 /*!
  */
 std::set<std::string> Automaton::output() const
 {
   return output_;
 }


 /*!
  *
  */
 void Automaton::addOutput(std::string label)
 {
   output_.insert(label);
 }


 /*!
  * Special depth first search method for service automaton creation
  * from Petri net. It's a recursive method which takes a State (named
  * start) and then tries to find its successors.
  *
  * \param     start
  */
 void Automaton::dfs(State &start)
 {
   //cerr << "dfs on node " << start.name() << "..." << endl;
   (*hashTable_)[start.hashValue()].insert(&start);

   // assuming that each state has a marking
   Marking m = *start.marking();

   // final state
   if (net_->finalCondition().isSatisfied(m))
     start.final();

   // iterate over all transitions to check if they can fire
   for (std::set<Transition *>::const_iterator t = net_->getTransitions().begin(); 
   t != net_->getTransitions().end(); ++t)
   {
     if (!m.activates(**t))
       continue;

     //cerr << "transition " << (*t)->getName() << " is activated..." << endl;
     State &j = createState(m.successor(**t));

     //cerr << "created node " << j.name() << endl;
     if (start == j)
     {
       deleteState(&j);
       createEdge(start, start, (*edgeLabels_)[*t], (*edgeTypes_)[*t]);
       continue;
     }

     // collision detection
     bool doubled = false;

     for (std::set<State *>::const_iterator s = (*hashTable_)[j.hashValue()].begin();
     s != (*hashTable_)[j.hashValue()].end(); ++s)
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
     {
       states_.pop_back();
     }
     else
     {
       for (unsigned int i = 0; i < states_.size()-2; i++)
         if (states_[i] == s)
         {
           states_[i] = states_[states_.size()-1];
           states_.pop_back();
         }
     }

     delete s;
   }
 }

 /*!
  * This method sets the synchronous labels of the automaton.
  */
 void Automaton::setSynchronousLabels(const std::set<std::string> & labels)
 {
   labels_ = labels;
 }


 /*!
  * Returning the set of synchronous labels.
  */
 std::set<std::string> Automaton::getSynchronousLabels() const
 {
   return labels_;
 }


} /* END OF NAMESPACE pnapi */
