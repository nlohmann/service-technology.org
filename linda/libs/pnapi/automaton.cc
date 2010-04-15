/*!
 * \file  automaton.cc
 */

#include "config.h"

#include "automaton.h"
#include "marking.h"
#include "port.h"
#include "util.h"

#include <sstream>
#include <ctime>

using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::map;
using std::set;
using std::string;

namespace pnapi
{


/*!
 * \brief standard constructor
 * 
 * The result of the constructor is an emty automaton.
 */
Automaton::Automaton() :
  edgeLabels_(NULL), edgeTypes_(NULL), weights_(NULL), net_(NULL), hashTable_(NULL), stateCounter_(0)
{
  /* do nothing */
}


/*!
 * \brief constructor
 * 
 * Transformation: PetriNet => Automaton.
 * First, the Petri net is normalized to avoid collisions.
 * Second, it is necessary to save the transition's type (see state.h class Edge).
 * Third, the marking is gained from the net to create the initial state.
 * At last the DEPTH-FIRST-SEARCH is started and results in a fully
 * built automaton from a Petri Net.
 */
Automaton::Automaton(PetriNet & net) :
  stateCounter_(0)
{
  // initializing private variables
  net_ = new PetriNet(net);
  edgeTypes_ = new std::map<Transition *, Edge::Type>();
  weights_ = new std::map<const Place *, unsigned int>();
  hashTable_ = new std::vector<std::set<State *> >(HASH_SIZE);
  
  // normalizing the copied net and retrieving the edge labels
  edgeLabels_ = new std::map<Transition *, std::string>(net_->normalize());
  
  // preparing labels
  set<Label *> tmpLabelSet = net_->getInterface().getInputLabels();
  PNAPI_FOREACH(l, tmpLabelSet)
  {
    input_.insert((*l)->getName());
  }
  
  tmpLabelSet = net_->getInterface().getOutputLabels();
  PNAPI_FOREACH(l, tmpLabelSet)
  {
    output_.insert((*l)->getName());
  }
  
  tmpLabelSet = net_->getInterface().getSynchronousLabels();
  PNAPI_FOREACH(l, tmpLabelSet)
  {
    synchronous_.insert((*l)->getName());
  }
  
  // giving each transition its type
  PNAPI_FOREACH(t, net_->getTransitions())
  {
    switch((*t)->getType())
    {
    case Transition::INTERNAL:
      (*edgeTypes_)[*t] = ((*t)->isSynchronized()) ? Edge::SYNCHRONOUS : Edge::TAU;
      break;
    case Transition::INPUT:
      (*edgeTypes_)[*t] = Edge::INPUT;
      break;
    case Transition::OUTPUT:
      (*edgeTypes_)[*t] = Edge::OUTPUT;
      break;
    default: /* do nothing */ break;  // \todo: report error?
    }
  }
  
  // making a marking's hash value more random
  srand(time(NULL));
  
  // setting place weights
  PNAPI_FOREACH(p, net_->getPlaces())
  {
    (*weights_)[*p] = rand() % HASH_SIZE;
  }
  
  // creating initial state
  State & start = createState(*new Marking(*net_));
  start.setInitial();
  
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
 * \brief copy constructor
 * 
 * The standard copy constructor copies the everything except for
 * properties marked as optional in the according header file.
 */
Automaton::Automaton(const Automaton & a) :
  input_(a.input_), output_(a.output_),
  synchronous_(a.synchronous_),
  edgeLabels_(NULL), edgeTypes_(NULL),
  weights_(NULL), hashTable_(NULL),
  stateCounter_(a.stateCounter_), net_(NULL)
{
  map<const Place *, const Place *> placeMap;

  if (a.net_ != NULL)
  {
    net_ = new PetriNet(*a.net_);
    PNAPI_FOREACH(p, a.net_->getPlaces())
    {
      placeMap[*p] = net_->findPlace((*p)->getName());
    }
  }

  // mapping from states to their copy
  map<State *, State *> stateMap;

  // copy states
  for(unsigned int i = 0; i < a.states_.size(); ++i)
  {
    State * s = new State(*(a.states_[i]), net_, &placeMap);
    states_.push_back(s);
    stateMap[a.states_[i]] = s;
  }

  // copy edges
  for(unsigned int i = 0; i < a.edges_.size(); ++i)
  {
    Edge * e = a.edges_[i];
    createEdge(*(stateMap[&(e->getSource())]), *(stateMap[&(e->getDestination())]),
                e->getLabel(), e->getType());
  }
}

/*!
 * \brief assignment operator
 */
Automaton & Automaton::operator=(const Automaton & a)
{
  if (&a == this)
    return *this;
  
  this->~Automaton();
  return *new (this) Automaton(a);
}

/*!
 * \brief destructor
 * 
 * The standard destructor deletes the optional objects created
 * while the transformation PetriNet => Automaton
 */
Automaton::~Automaton()
{
  delete net_;

  for(unsigned int i = 0; i < edges_.size(); ++i)
  {
    delete (edges_[i]);
  }

  // no need to call deleteState() since states_ is already to be deleted
  for(unsigned int i = 0; i < states_.size(); ++i)
  {
    delete (states_[i]);
  }
}


/*!
 * \brief creating a state
 * 
 * A state will be added to the set of states. The name given can be
 * empty, so a standard name will be set.
 *
 * \return the newly created state
 */
State & Automaton::createState()
{
  State * s = new State(&stateCounter_);
  assert(s != NULL);
  states_.push_back(s);
  return *s;
}


/*!
 * \brief creating a state with a given name
 * 
 * A state will be added to the set of states with a given name.
 * If there exists a state with the given name, this state will
 * be returned.
 *
 * \param  name the name of the state
 * \return the newly created state
 */
State & Automaton::createState(const unsigned int name)
{
  for (unsigned int i = 0; i < states_.size(); ++i)
  {
    if(states_[i]->getName() == name)
    {
      return *states_[i];
    }
  }
  State * s = new State(name);
  assert(s != NULL);
  states_.push_back(s);
  return *s;
}


/*!
 * \brief creating a state from a given marking
 * 
 * A state will be added to the set of states. This state is based on
 * a given marking, which is needed to calculate the state's hash value.
 *
 * \param m the given marking
 *
 * \return the newly created state
 */
State & Automaton::createState(Marking & m)
{
  State * s = new State(m, weights_, stateCounter_);
  assert(s != NULL);
  states_.push_back(s);
  return *s;
}


/*!
 * \brief Finding a state by name.
 */
State * Automaton::findState(const unsigned int name) const
{
  for (unsigned int i = 0; i < states_.size(); ++i)
  {
    if (states_[i]->getName() == name)
      return states_[i];
  }
  return NULL;
}


/*!
 * \brief creating an edge from state 1 to state 2
 */
Edge & Automaton::createEdge(State & s1, State & s2)
{
  Edge * e = new Edge(s1, s2);
  edges_.push_back(e);
  return *e;
}


/*!
 * \brief creating an edge with label and type
 */
Edge & Automaton::createEdge(State & s1, State & s2,
              const std::string & label, Edge::Type type)
{
  Edge * e = new Edge(s1, s2, label, type);
  edges_.push_back(e);
  switch(type)
  {
  case Edge::INPUT: addInputLabel(label); break;
  case Edge::OUTPUT: addOutputLabel(label); break;
  case Edge::SYNCHRONOUS: synchronous_.insert(label); break;
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
PetriNet Automaton::toStateMachine() const
{
  PetriNet result; // resulting net
  std::map<State *,Place *> state2place; // places by states

  // caches for faster place search
  map<string, Place *> interfacePlaces;

  Condition final;
  final = false; // final places

  /* no comment */

  if (states_.empty())
    return result;

  // copy interface
  Interface & tmpInterface = result.getInterface();
  PNAPI_FOREACH(l, input_)
  {
    tmpInterface.addInputLabel(*l);
  }
  PNAPI_FOREACH(l, output_)
  {
    tmpInterface.addOutputLabel(*l);
  }
  PNAPI_FOREACH(l, synchronous_)
  {
    tmpInterface.addSynchronousLabel(*l);
  }

  // generate places from states
  for(unsigned int i = 0; i < states_.size(); ++i)
  {
    std::stringstream ss;
    std::string id;
    ss << states_[i]->getName();
    ss >> id;
    Place * p = &(result.createPlace("p"+id));
    state2place[states_[i]] = p;
    if (states_[i]->isInitial())
    {
      p->setTokenCount(1);
    }

    /*
     * if the state is final then the according place
     * has to be in the final marking.
     */
    if(states_[i]->isFinal())
    {
      final = (final.getFormula() || ((*p) == 1));
    }
  }

  // generate transitions from edges
  for(unsigned int i = 0; i < edges_.size(); ++i)
  {
    Transition * t = &(result.createTransition(""));
    
    if(edges_[i]->getType() != Edge::TAU)
    {
      t->addLabel(*(result.getInterface().findLabel(edges_[i]->getLabel())));
    }

    Place * p = state2place[&(edges_[i]->getSource())];
    result.createArc(*p,*t);

    p = state2place[&(edges_[i]->getDestination())];
    result.createArc(*t,*p);
  }

  // generate final condition
  result.getFinalCondition() = final.getFormula();
  result.getFinalCondition().allOtherPlacesEmpty(result); // actually unneccassary

  return result;
}


/*!
 * \brief returning a set of states with no preset
 * 
 * The initial states are those states which have no preset.
 * In basic service automata there is only one state which
 * is called the initial state.
 *
 * \return set of initial states
 */
std::set<State *> Automaton::getInitialStates() const
{
  std::set<State *> result;
  for(unsigned int i = 0; i < states_.size(); ++i)
  {
    if (states_[i]->isInitial())
      result.insert(states_[i]);
  }

  return result;
}


/*!
 * \brief returning a set of states with no postset
 * 
 * The final states are those which are flagged as final. You can trigger
 * this flag by calling State::final() on a state object.
 *
 * \return set of final states
 */
std::set<State *> Automaton::getFinalStates() const
{
  std::set<State *> result;
  for(unsigned int i = 0; i < states_.size(); ++i)
  {
    if (states_[i]->isFinal())
      result.insert(states_[i]);
  }

  return result;
}


/*!
 * \brief returning a set of input labels (after PetriNet => Automaton)
 */
const std::set<std::string> & Automaton::getInputLabels() const
{
  return input_;
}


/*!
 * \brief adds an input label
 */
void Automaton::addInputLabel(const std::string & label)
{
  input_.insert(label);
}


/*!
 * \brief returning a set of output labels (after PetriNet => Automaton)
 */
const std::set<std::string> & Automaton::getOutputLabels() const
{
  return output_;
}


/*!
 * \brief adds an output label
 */
void Automaton::addOutputLabel(const std::string & label)
{
  output_.insert(label);
}


/*!
 * \brief adds a synchronous label
 */
void Automaton::addSynchronousLabel(const std::string & label)
{
  synchronous_.insert(label);
}

/*!
 * \brief sets the synchronous labels of the automaton
 */
void Automaton::setSynchronousLabels(const std::set<std::string> & labels)
{
  synchronous_ = labels;
}


/*!
 * \brief returning the set of synchronous labels
 */
const std::set<std::string> & Automaton::getSynchronousLabels() const
{
  return synchronous_;
}

/*!
 * \brief depth-first-search in the unknown automaton
 * 
 * Special depth first search method for service automaton creation
 * from Petri net. It's a recursive method which takes a State (named
 * start) and then tries to find its successors.
 *
 * \param start first node
 */
void Automaton::dfs(State & start)
{
  //cerr << "dfs on node " << start.name() << "..." << endl;
  (*hashTable_)[start.getHashValue()].insert(&start);

  // assuming that each state has a marking
  Marking * m = start.getMarking();
  assert(m != NULL);

  // final state
  if(net_->getFinalCondition().isSatisfied(*m))
  {
    start.setFinal();
  }

  // iterate over all transitions to check if they can fire
  PNAPI_FOREACH(t, net_->getTransitions())
  {
    if (!(m->activates(**t)))
      continue;

    //cerr << "transition " << (*t)->getName() << " is activated..." << endl;
    State & succ = createState(m->getSuccessor(**t));

    //cerr << "created node " << j.name() << endl;
    if (start == succ)
    {
      deleteState(succ);
      createEdge(start, start, (*edgeLabels_)[*t], (*edgeTypes_)[*t]);
      continue;
    }

    // collision detection
    bool doubled = false;

    PNAPI_FOREACH(s, (*hashTable_)[succ.getHashValue()])
    {
      if (**s == succ)
      {
        doubled = true;
        createEdge(start, **s, (*edgeLabels_)[*t], (*edgeTypes_)[*t]);
        break;
      }
    }

    if (doubled)
    {
      deleteState(succ);
      continue;
    }

    createEdge(start, succ, (*edgeLabels_)[*t], (*edgeTypes_)[*t]);

    dfs(succ);
  }
}


/*!
 * \brief deleting a state from the automaton
 */
void Automaton::deleteState(State & s)
{
  if (s.getPreset().empty() && s.getPostset().empty())
  {
    if (states_[states_.size()-1] != &s)
    {
      for (unsigned int i = 0; i < states_.size()-2; i++)
      {
        if (states_[i] == &s)
        {
          states_[i] = states_[states_.size()-1];
          break;
        }
      }
    }
    states_.pop_back();

    delete (&s);
  }
}


} /* namespace pnapi */
