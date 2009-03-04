// -*- C++ -*-

#ifndef PNAPI_AUTOMATON_H
#define PNAPI_AUTOMATON_H

#include <iostream>
#include <cassert>
#include <list>
#include <set>
#include <vector>

#include "marking.h"
#include "petrinet.h"
#include "state.h"

namespace pnapi
{


  /// output format
  enum Format { SA, STG };


  // forward declarations
  class State;
  class Place;
  class Transition;
  class Marking;
  template <class T> class AbstractAutomaton;


  /// Real automata - anything is possible but creating one from a net
  typedef AbstractAutomaton<State> Automaton;
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
    /// creating an edge
    void createEdge(T &source, T &destination, const string label);

    /// friend operation <<
    template <class R> friend std::ostream &
    operator <<(std::ostream &os, const AbstractAutomaton<R> &sa);

    /// setting the format of the automaton output
    void setFormat(Format f);

  protected:
    set<T *> states_;
    set<Edge<T> *> edges_;

    /// described PetriNet
    PetriNet &net_;

    /// temporary output flag
    Format f_;

    /// Service automata output format (ig like)
    void output_sa(std::ostream &os) const;
    /// stg output format
    void output_stg(std::ostream &os) const;


  };


  /// operator << for abstract automata
  template <typename T>
  std::ostream & operator <<(std::ostream &os, const AbstractAutomaton<T> &sa)
  {
    switch (sa.f_)
    {
    case SA:    sa.output_sa(os); break;
    case STG:   sa.output_stg(os); break;
    default:    assert(false); break;
    }

    return os;
  }


  /*!
   * \brief   standard constructor
   */
  template <class T>
  AbstractAutomaton<T>::AbstractAutomaton() :
    net_(*new pnapi::PetriNet())
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
    states_.insert(new State(name));
  }


  /*!
   * \brief
   */
  template <class T>
  void AbstractAutomaton<T>::createEdge(T &source, T &destination, const string label)
  {
    std::cout << "DEBUG: Created edge from " << source.getName();
    std::cout << " to " << destination.getName() << "\n";
    edges_.insert(new Edge<T>(source, destination, label));
  }


  /*!
   * \brief
   */
  template <class T>
  void AbstractAutomaton<T>::setFormat(Format f)
  {
    f_ = f;
  }


  /*!
   * \brief
   *
   * \bug     Too many output on interface.
   */
  template <class T>
  void AbstractAutomaton<T>::output_sa(std::ostream &os) const
  {
    os << "INTERFACE\n";
    os << "  " << "INPUT ";
/*    bool begin = true;
    for (set<Edge<T> *>::const_iterator e = edges_.begin();
        e != edges_.end(); e++)
    {
      if (begin)
      {
        string currLabel = (*e)->getLabel();
        if (currLabel.c_str()[0] != '!')
          continue;
        os << currLabel.substr(1, currLabel.length());
        begin = false;
        continue;
      }

      string currLabel = (*e)->getLabel();
      if (currLabel.c_str()[0] != '!')
        continue;
      os << ", " << currLabel.substr(1, currLabel.length());
    }*/
    os << ";\n";

    os << "  " << "OUTPUT ";
/*      begin = true;
    for (set<Edge<T> *>::const_iterator e = edges_.begin();
        e != edges_.end(); e++)
    {
      if (begin)
      {
        string currLabel = (*e)->getLabel();
        if (currLabel.c_str()[0] != '?')
          continue;
        os << currLabel.substr(1, currLabel.length());
        begin = false;
        continue;
      }

      string currLabel = (*e)->getLabel();
      if (currLabel.c_str()[0] != '?')
        continue;
      os << ", " << currLabel.substr(1, currLabel.length());
    }*/
    os << ";\n\n";

    os << "NODES\n";

    os << ";\n\n";

    os << "INITIALNODE\n";
/*      T *stat = NULL;
    for (set<T *>::const_iterator s = states_.begin(); s != states_.end();
        s++)
    {
      if ((*s)->getPreset().empty())
      {
        stat = *s;
        break;
      }
    }
    os << "  " << stat->getName() << ";\n\n";*/

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


  /// Service Automaton
  class ServiceAutomaton : public AbstractAutomaton<StateB>
  {
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

    /// adds a StateB to the automaton
    void addState(StateB &s);

    /// Depth-First-Search
    void dfs(StateB &i);
  };


}

#endif /* SERVICEAUTOMATON_H */
