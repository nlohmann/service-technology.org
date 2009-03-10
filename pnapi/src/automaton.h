// -*- C++ -*-

#ifndef PNAPI_AUTOMATON_H
#define PNAPI_AUTOMATON_H

#include <iostream>
#include <cassert>
#include <deque>
#include <list>
#include <set>
#include <vector>
#include <ostream>

#include "state.h"
#include "petrinet.h"

using std::deque;
using std::ostream;
using std::vector;

namespace pnapi
{


  // forward declarations
  class PetriNet;
  class Marking;
  template <class T> class AbstractAutomaton;
  class Automaton;
  class ServiceAutomaton;


  /// Operating Guidelines
  typedef AbstractAutomaton<StateOG> OG;


  /// forward declaration of AbstractAutomaton
  template <class T>
  class AbstractAutomaton
  {
  public:
    /// standard constructor
    AbstractAutomaton();
    /// constructor used by ServiceAutomaton class
    AbstractAutomaton(PetriNet &net);
    /// standard destructor
    virtual ~AbstractAutomaton();

    /// creating a state
    void createState(const string name = "");
    /// finding a state
    const T * findState(const string name) const;
    /// creating an edge
    void createEdge(T &source, T &destination, const string label,
        pnapi::Node::Type type);

    /// returns the Petri net
    PetriNet getPetriNet() const { return net_; }

    /// Service automata output format (ig like) -- absolutely experimental!!
    void output_sa(std::ostream &os) const;

  protected:
    vector<T *> states_;
    vector<Edge<T> *> edges_;

    /// described PetriNet
    PetriNet net_;

    /// Service automata output format (ig like)
    //void output_sa(std::ostream &os) const;
    /// stg output format
    void output_stg(std::ostream &os) const;


  };


  /*!
   * \brief   standard constructor
   */
  template <class T>
  AbstractAutomaton<T>::AbstractAutomaton() :
    net_(*new PetriNet())
  {
  }


  /*!
   * \brief   Constructor which instantiates a Petri net
   *
   * This constructor is used by the construction Petri net => Service
   * Automaton.
   */
  template <class T>
  AbstractAutomaton<T>::AbstractAutomaton(PetriNet &net) :
    net_(net)
  {
  }


  /*!
   * \brief   Standard destructor
   */
  template <class T>
  AbstractAutomaton<T>::~AbstractAutomaton()
  {
  }


  /*!
   * \brief Creates a state
   */
  template <class T>
  void AbstractAutomaton<T>::createState(const string name)
  {
    states_.push_back(new State(name));
  }


  /*!
   * \brief   Finds a state in the set of states
   */
  template <class T>
  const T * AbstractAutomaton<T>::findState(const string name) const
  {
    for (unsigned int i = 0; i < states_.size(); i++)
      if (states_[i]->getName() == name)
        return states_[i];

    return NULL;
  }


  /*!
   * \brief   Creates an automaton's edge from one state to another with a label
   */
  template <class T>
  void AbstractAutomaton<T>::createEdge(T &source, T &destination,
      const string label, pnapi::Node::Type type)
  {
    edges_.push_back(new Edge<T>(source, destination, label, type));
  }


  /*!
   * \brief   Provides the SA output (IG like)
   */
  template <class T>
  void AbstractAutomaton<T>::output_sa(std::ostream &os) const
  {
    bool first;
    std::set<string> seen;

    os << "INTERFACE\n";
    os << "  " << "INPUT ";
    first = true;
    seen.clear();
    for (unsigned int i = 0; i < edges_.size(); i++)
    {
      if ((*edges_[i]).getType() == pnapi::Node::INPUT &&
          seen.count((*edges_[i]).getLabel()) == 0)
      {
        if (first)
        {
          first = false;
          os << "!" << (*edges_[i]).getLabel();
        }
        else
          os << ", !" << (*edges_[i]).getLabel();
      }
      seen.insert((*edges_[i]).getLabel());
    }
    os << ";\n";

    os << "  " << "OUTPUT ";
    first = true;
    seen.clear();
    for (unsigned int i = 0; i < edges_.size(); i++)
    {
      if ((*edges_[i]).getType() == pnapi::Node::OUTPUT &&
          seen.count((*edges_[i]).getLabel()) == 0)
      {
        if (first)
        {
          first = false;
          os << "?" << (*edges_[i]).getLabel();
        }
        else
          os << ", ?" << (*edges_[i]).getLabel();
      }
      seen.insert((*edges_[i]).getLabel());
    }
    os << ";\n\n";

    os << "NODES\n  ";
    first = true;
    for (unsigned int i = 0; i < states_.size(); i++)
      if (first)
      {
        first = false;
        os << (*states_[i]).getName();
      }
      else
        os << ", " << (*states_[i]).getName();
    os << ";\n\n";

    os << "INITIALNODE\n";
    os << "  " << (*states_[0]).getName();
    os << ";\n\n";

    os << "FINALNODES\n  ";
    first = true;
    for (unsigned int i = 0; i < states_.size(); i++)
    {
      if ((*states_[i]).isFinal())
      {
        if (first)
        {
          first = false;
          os << (*states_[i]).getName();
        }
        else
          os << ", " << (*states_[i]).getName();
      }
    }
    os << ";\n\n";

    os << "TRANSITIONS\n";
    for (unsigned int i = 0; i < edges_.size(); i++)
    {
      os << "  " << (*edges_[i]).getSource().getName() << " -> ";
      os << (*edges_[i]).getDestination().getName() << " : ";
      switch ((*edges_[i]).getType())
      {
      case pnapi::Node::INPUT:
        os << "!";
        break;
      case pnapi::Node::OUTPUT:
        os << "?";
      default: break;
      }
      os << (*edges_[i]).getLabel();
      if (i == edges_.size()-1)
        os << ";\n";
      else
        os << ",\n";
    }
  }


  /*!
   * \brief   Output method for STG format.
   */
  template <class T>
  void AbstractAutomaton<T>::output_stg(std::ostream &os) const
  {

  }


  /// Automaton class which will be able to read SA format
  class Automaton : public AbstractAutomaton<State>
  {
    friend ostream & operator<<(ostream &, const Automaton &);
  public:
    Automaton();
    virtual ~Automaton() {}
  };


  /// Service Automaton
  class ServiceAutomaton : public AbstractAutomaton<StateB>
  {
    //friend ostream & operator<<(ostream &, const ServiceAutomaton &);
  public:
    /// constructor with Petri net - initial marking taken from place tokens
    ServiceAutomaton(PetriNet &n);
    /// standard destructor
    virtual ~ServiceAutomaton();

    /// overloaded method createState() -- replaces the older method addState
    void createState(StateB &s);

  private:
    /// attributes & methods used for creating an automaton from Petri net
    static const int PNAPI_SA_HASHSIZE = 65535;
    /// hash table for recovering already seen states
    std::vector<std::set<StateB *> > hashTable_;
    /// edge labels which represent the interface of the automaton
    std::map<Transition *, string> edgeLabels_;
    /// each transitions has got a type according to the interface
    /// place connected to the transition
    std::map<Transition *, pnapi::Node::Type> edgeTypes_;

    /// Depth-First-Search
    void dfs(StateB &i);
  };


}

#endif /* SERVICEAUTOMATON_H */
