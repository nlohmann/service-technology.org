/*!
 * \file    automaton.cc
 *
 * \author  Stephan Mennicke <stephan.mennicke@uni-rostock.de>
 */

#include <cassert>
#include <cmath>
#include <iostream>

#include "petrinet.h"
#include "util.h"
#include "automaton.h"

using std::cerr;
using std::cout;
using std::ostream;
using std::list;

namespace pnapi
{


  /*!
   * \brief   Standard constructor for service automaton
   *
   * This constructor takes a net n and gets the initial
   * marking from the net itself.
   *
   * \param   PetriNet n
   *
   * \note    Before building an automaton one has to normalize the by
   *          calling the public method normalize(). See precondition
   *          initialize().
   */
  ServiceAutomaton::ServiceAutomaton(PetriNet &n) :
    AbstractAutomaton<StateB>(n), hashTable_(PNAPI_SA_HASHSIZE),
    initialmarking_(n, true)
  {
    net_.normalize();
    for (std::set<Transition *>::const_iterator t =
        net_.getTransitions().begin(); t != net_.getTransitions().end(); t++)
    {
      edgeTypes_[*t] = (**t).getType();
    }
    edgeLabels_ = net_.normalize(true);
    initialmarking_ = *new Marking(net_);

    StateB *i = new StateB(initialmarking_);
    states_.push_back(i);

    dfs(*i);
  }


  /*!
   * \brief   Standard destructor for Service Automaton
   */
  ServiceAutomaton::~ServiceAutomaton()
  {
  }


  /*!
   * \brief
   */
  void ServiceAutomaton::addState(StateB &s)
  {
    states_.push_back(&s);
  }


  /*!
   * \brief   Depth-First-Search creating Automaton from Petri Net
   */
  void ServiceAutomaton::dfs(StateB &i)
  {
    hashTable_[i.getHashValue()].insert(&i);
    Marking m = i.getMarking();

    // final state
    if (net_.finalCondition().isSatisfied(m))
    {
      i.final();
      return;
    }

    for (set<Transition *>::const_iterator t = net_.getTransitions().begin();
        t != net_.getTransitions().end(); t++)
    {
      bool doubled = false;

      if (!m.activates(**t))
        continue;

      StateB &j = *new StateB(m.successor(**t));
      if (i == j)
        continue;

      // collision detection
      for (set<StateB *>::const_iterator s =
          hashTable_[j.getHashValue()].begin();
          s != hashTable_[j.getHashValue()].end(); s++)
      {
        if (**s == j)
        {
          doubled = true;
          createEdge(i, **s, edgeLabels_[*t], edgeTypes_[*t]);
          break;
        }
      }

      if (doubled)
        continue;

      addState(j);
      createEdge(i, j, edgeLabels_[*t], edgeTypes_[*t]);

      dfs(j);
    }
  }


  /*!
   * \brief   standard constructor of Automaton class
   */
  Automaton::Automaton() :
    AbstractAutomaton<State>()
  {
  }


}
