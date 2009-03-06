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
    const T & findState(const string name) const;
    /// creating an edge
    void createEdge(T &source, T &destination, const string label);


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
   * \brief
   */
  template <class T>
  AbstractAutomaton<T>::AbstractAutomaton(PetriNet &net) :
    net_(net)
  {
  }


  /*!
   * \brief
   */
  template <class T>
  AbstractAutomaton<T>::~AbstractAutomaton()
  {
  }


  /*!
   * \brief
   */
  template <class T>
  void AbstractAutomaton<T>::createState(const string name)
  {
    states_.push_back(new State(name));
  }


  /*!
   * \brief
   */
  template <class T>
  const T & AbstractAutomaton<T>::findState(const string name) const
  {
    for (unsigned int i = 0; i < states_.size(); i++)
      if (states_[i]->getName() == name)
        return *states_[i];
  }


  /*!
   * \brief
   */
  template <class T>
  void AbstractAutomaton<T>::createEdge(T &source, T &destination, const string label)
  {
    edges_.push_back(new Edge<T>(source, destination, label));
  }


  /*!
   * \brief
   */
  template <class T>
  void AbstractAutomaton<T>::output_sa(std::ostream &os) const
  {
    deque<string> interface;
    for (int i = 0; i < edges_.size(); i++)
    {
      std::cout << edges_[i]->getLabel() << "\n";
      if (edges_[i]->getLabel().c_str()[0] == '!')
      {
        interface.push_front(edges_[i]->getLabel());
        std::cout << i << "!\n";
      }
      else if (edges_[i]->getLabel().c_str()[0] == '?')
      {
        std::cout << i << "?\n";
        interface.push_back(edges_[i]->getLabel());
      }
    }

    os << "INTERFACE\n";
    os << "  " << "INPUT ";
    if (!interface.empty() && interface[0].c_str()[0] == '!')
    {
      os << interface[0];
      interface.pop_front();
      while (interface[0].c_str()[0] == '!')
      {
        os << ", " << interface[0];
        interface.pop_front();
      }
    }
    os << ";\n";

    os << "  " << "OUTPUT ";
    if (!interface.empty())
    {
      os << interface[0];
      interface.pop_front();
      while (!interface.empty())
      {
        os << ", " << interface[0];
        interface.pop_front();
      }
    }
    os << ";\n\n";

    os << "NODES\n";

    os << ";\n\n";

    os << "INITIALNODE\n";

    os << ";\n\n";

    os << "FINALNODES\n";

    os << ";\n\n";

    os << "TRANSITIONS\n";

    os << ";\n";
  }


  /*!
   * \brief
   */
  template <class T>
  void AbstractAutomaton<T>::output_stg(std::ostream &os) const
  {
    /// implement it!
  }


  /// Automaton class which can read .sa files
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
    /// constructor with Petri net - initial marking taken from place marks
    ServiceAutomaton(PetriNet &n);
    /// standard destructor
    virtual ~ServiceAutomaton();

  private:
    /// attributes & methods used for creating an automaton from Petri net
    static const int PNAPI_SA_HASHSIZE = 65535;

    std::vector<std::set<StateB *> > hashTable_;

    Marking initialmarking_;

    std::map<Transition *, string> edgeLabels_;

    /// adds a StateB to the automaton
    void addState(StateB &s);

    /// Depth-First-Search
    void dfs(StateB &i);
  };


}

#endif /* SERVICEAUTOMATON_H */
