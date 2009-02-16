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
    hashTable_(HASHSIZE), initialmarking_(n), AbstractAutomaton<StateB>(n)
  {
    f_ = SA;
    initialmarking_ = *new Marking(n, true);

    StateB *i = new StateB(initialmarking_);
    states_.insert(i);

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
    states_.insert(&s);
  }


  /*!
   * \brief   Depth-First-Search creating Automaton from Petri Net
   */
  void ServiceAutomaton::dfs(StateB &i)
  {
    cout << "DEBUG: " << i.getName();
    cout << " with hash value " << i.getHashValue() << "\n";

    hashTable_[i.getHashValue()].insert(&i);
    Marking m = i.getMarking();

    // final state
    if (net_.finalCondition().isSatisfied(m))
    {
      cout << "DEBUG: " << i.getName() << " is a final state.\n";
      return;
    }

    set<Transition *> Trans = net_.getTransitions();
    for (set<Transition *>::const_iterator t = Trans.begin(); t != Trans.end(); t++)
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
          createEdge(i, **s, "bla");
          break;
        }
      }

      if (doubled)
        continue;

      addState(j);
      createEdge(i, j, "bla");
      dfs(j);
    }
  }


}
