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
      edgeLabels_(NULL), edgeTypes_(NULL), net_(NULL), hashTable_(NULL), counter_(0)
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
    for (std::set<Place *>::iterator p = net_->getInterfacePlaces().begin(); p != net_->getInterfacePlaces().end(); p++)
    {
      if ((*p)->getType() == Node::INPUT)
        addInput((*p)->getName());
      else
        addOutput((*p)->getName());
    }
    // giving each transition its type
    for (std::set<Transition *>::iterator t = net_->getTransitions().begin();
        t != net_->getTransitions().end(); t++)
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
        p != net_->getPlaces().end(); p++)
    {
      (*weights_)[*p] = rand() % HASH_SIZE;
    }

    // creating initial state
    State &start = createState(*new Marking(*net_));
    start.initial();
    // beginning to find follow-up states
    dfs(start);
  }


  /*!
   * The standard copy constructor copies the everything except for
   * properties which marked as optional in the according header file.
   */
  Automaton::Automaton(const Automaton &a) :
    states_(a.states_), edges_(a.edges_), edgeLabels_(NULL), hashTable_(NULL), counter_(a.counter_+1)
  {
    if (a.net_ == NULL)
      net_ = NULL;
    else
      net_ = new PetriNet(*a.net_);
    edgeTypes_ = NULL;
  }


  /*!
   * The standard destructor deletes the optional objects created
   * while the transformation PetriNet => Automaton
   */
  Automaton::~Automaton()
  {
    if (net_ != NULL)
    {
      delete net_;
    }
    if (edgeLabels_ != NULL)
    {
      delete edgeLabels_;
    }
    if (edgeTypes_ != NULL)
    {
      delete edgeTypes_;
    }
    if (hashTable_ != NULL)
    {
      delete hashTable_;
    }
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
    State *s = new State(m, weights_, &counter_);
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
    const std::string label, Type type)
  {
    Edge *e = new Edge(s1, s2, label, type);
    edges_.push_back(e);
    if (type == INPUT)
      addInput(label);
    if (type == OUTPUT)
      addOutput(label);
    if (type == SYNCHRONOUS)
      labels_.insert(label);
    return *e;
  }


  /*!
   * \brief Transforms the automaton to a state machine Petri net.
   *
   * States become places, edges become transitions, initial states
   * will be initially marked and final states will be connected
   * disjunctive in the final condition.
   *
   * \todo clean me!
   */
  PetriNet & Automaton::stateMachine() const
  {
    PetriNet *result = new PetriNet(); // resulting net
    std::map<State*,Place*> state2place; // places by states

    Condition final;
    final = false; // final places

    /* no comment */

    if (states_.empty())
      return *result;

    std::set<std::string> in = input();
    std::set<std::string> out = output();
    for (std::set<std::string>::iterator i = in.begin(); i != in.end(); i++)
    {
      result->createPlace(*i, Node::INPUT);
    }
    for (std::set<std::string>::iterator o = out.begin(); o != out.end(); o++)
    {
      result->createPlace(*o, Node::OUTPUT);
    }

    // generate places from states
    for(unsigned int i=0; i < states_.size(); ++i)
    {
      std::stringstream s;
      std::string id;
      s << states_[i]->name();
      s >> id;
      Place *p = &(result->createPlace("p"+id));
      state2place[states_[i]] = p;
      if (states_[i]->isInitial())
        p->mark();

      /*
       * if the state is final then the according place
       * has to be in the final marking.
       */
      if(states_[i]->isFinal())
        final = final.formula() || (*(state2place[states_[i]])) == 1;
    }

    // map synchlabel : T -> {string}
    std::map<Transition *, std::set<std::string> > synchlabel;
    // generate transitions from edges
    for(unsigned int i=0; i < edges_.size(); ++i)
    {
      Transition* t = &(result->createTransition());
      switch (edges_[i]->type())
      {
      case Automaton::INPUT:
        result->createArc(*result->findPlace(edges_[i]->label()), *t);
        break;
      case Automaton::OUTPUT:
        result->createArc(*t, *result->findPlace(edges_[i]->label()));
        break;
      case Automaton::SYNCHRONOUS:
      {
        synchlabel[t].insert(edges_[i]->label());
        break;
      }
      default:
        break;
      }

      Place* p = state2place[&(edges_[i]->source())];
      result->createArc(*p,*t);

      p = state2place[&(edges_[i]->destination())];
      result->createArc(*t,*p);
    }

    std::set<Transition *> transitions = result->getTransitions();
    for (std::set<Transition *>::iterator t = transitions.begin(); t != transitions.end(); t++)
    {
      // FIXME: the easy way seems not to be working
      // FIXME: THIS IS THE EASY WAY: (*t)->setSynchronizeLabels(synchlabel[*t]);

      // making a copy of all transitions and give them the labels
      Transition &tt = result->createTransition("", synchlabel[*t]);
      for (std::set<Arc *>::iterator f = (*t)->getPresetArcs().begin(); f != (*t)->getPresetArcs().end(); f++)
        result->createArc((*f)->getPlace(), tt, (*f)->getWeight());
      for (std::set<Arc *>::iterator f = (*t)->getPostsetArcs().begin(); f != (*t)->getPostsetArcs().end(); f++)
        result->createArc(tt, (*f)->getPlace(), (*f)->getWeight());

      result->deleteTransition(**t);
    }

    // generate final condition
    result->finalCondition() = final.formula() && formula::ALL_OTHER_PLACES_EMPTY;

    // copy synchronous interface (how did this ever work before???)
    result->setSynchronousLabels(labels_);
    
    return *result;
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
    result.clear(); ///TODO: <- ???
    for (unsigned int i = 0; i < states_.size(); i++)
      if (states_[i]->isInitial())
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
   * \param     State &start
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

    bool doubled;
    // iterate over all transitions to check if they can fire
    for (std::set<Transition *>::const_iterator
        t = net_->getTransitions().begin(); t != net_->getTransitions().end();
        t++)
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


  /*!
   * \brief     creates a STG file of the graph
   * \param     edgeLabels a reference to a vector of strings containing the old
   *            label names from this graph
   * \param     out a reference to the output class handleing the temporary file
   */
  void Automaton::printToSTG(std::vector<std::string> & edgeLabels, util::Output & out) const
  {
    // create and fill stringstream for buffering graph information
    std::ostringstream STGStringStream; // used as buffer for graph information
    
    STGStringStream << ".state graph" << "\n";
    
    // copy edges
    for(unsigned int i = 0; i < edges_.size(); ++i)
    {
      Edge * edge = edges_[i];
      
      // build label vector:
      // each label is mapped to his position in edgeLabes
      int foundPosition = -1;
      for(int j = 0; j < (int)edgeLabels.size(); ++j)
      {
        if (edge->label() == edgeLabels[j])
        {
            foundPosition = j;
            break;
        }
      }
      if (foundPosition == -1)
      {
          foundPosition = (int)edgeLabels.size();
          edgeLabels.push_back(edge->label());
      }
      
      // print current transition to stream
      STGStringStream  << "p" << edge->source().name() << " t" << foundPosition 
                       << " p" << edge->destination().name() << "\n";
    }
    
    // mark final states
    for(unsigned int i = 0; i < states_.size(); ++i)
    {
      // TODO: possibly buggy because for every final node is "FINAL" added?
      if (states_[i]->isFinal())
      {
        // each label is mapped to his position in edgeLabes
        std::string currentLabel = "FINAL";
        currentLabel += states_[i]->name();
        int foundPosition = (int)edgeLabels.size();
        edgeLabels.push_back(currentLabel);
        STGStringStream << "p" << states_[i]->name() << " t" << foundPosition << " p00" << "\n";
      }
    }
    
    STGStringStream << ".marking {p" << (*initialStates().begin())->name() << "}" << "\n";
    STGStringStream << ".end" << std::flush;


    // create STG file, print header, transition information and then
    // add buffered graph information
    out.stream() << ".model Labeled_Transition_System" << "\n";
    out.stream() << ".dummy";
    for (int i = 0; i < (int)edgeLabels.size(); i++)
    {
      out.stream() << " t" << i;
    }
    std::string STGGraphString = STGStringStream.str();
    out.stream() << "\n" << STGGraphString << std::endl;
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
