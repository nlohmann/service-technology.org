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
    cout << "Before normalization!\nOWFN-Output:\n";
    //cout << io::owfn << n;
    n.normalize(true);
    cout << "Net is normalized!\nOWFN-Output:\n";
    //cout << io::owfn << n;
    initialmarking_ = *new Marking(n);

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
          createEdge(i, **s, edgeLabels_[*t]);
          break;
        }
      }

      if (doubled)
        continue;

      addState(j);
      createEdge(i, j, edgeLabels_[*t]);
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
