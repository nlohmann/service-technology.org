/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2008  Dirk Fahland <dirk.fahland@service-technolog.org>,

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    petrinet-workflow.cc
 *
 * \brief   Petri Net API: workflow extension
 *
 * \author  Dirk Fahland <fahland@informatik.hu-berlin,de>
 *
 * \since   2008-06-06
 *
 * \date    \$Date: 2007/06/28 07:38:17 $
 *
 * \note    This file is part of the tool UML2oWFN.
 *
 * \version \$Revision: 1.222 $
 *
 * \ingroup petrinet
 */

/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <iostream>
#include <utility>

#include <stack>

#include "petrinet-workflow.h"
#include "helpers.h"		// helper functions (setUnion, setDifference, max, toString)
#include "pnapi.h"
#include "debug.h"      // (trace)

using std::stack;
using std::set;
using std::cerr;
using std::endl;
using std::string;


/******************************************************************************
 * Member methods on properties of extended workflow nets
 *****************************************************************************/

set< Node* > ExtendedWorkflowNet::isFreeChoice() const {

	set< Node* > resultSet;

	for (set<Transition *>::iterator it = T.begin(); it != T.end(); it++)
	{
		set<Transition *>::iterator it2 = it; it2++;	// get next transition
		for (; it2 != T.end(); it2++)
		{
			Transition* t = static_cast<Transition*>(*it);
			Transition* t2 = static_cast<Transition*>(*it2);

			bool empty = true;
			for (set<Node *>::iterator pre = t->preset.begin(); pre != t->preset.end(); pre++) {
				if (t2->preset.find(*pre) != t2->preset.end()) {
					empty = false;	// intersection is non-empty
				} else if (!empty) {
					//cerr << "transitions " << t->nodeFullName() << " and " << t2->nodeFullName() << " are not free-choice" << endl;
					// pre sets of t1 and t2 are not equal
					//return false;
					resultSet.insert(t);
					resultSet.insert(t2);
				}
			}
		}
	}
	return resultSet;
}

Node* ExtendedWorkflowNet::isPathCovered() const {

	stack< Node* > searchStack;

	set< Node* > fromAlpha;
	Place* p = findPlace("alpha");
	if (p == NULL) return false;
	else searchStack.push(p);

	while (!searchStack.empty()) {
		Node *n = searchStack.top(); searchStack.pop();
		fromAlpha.insert(n);

		for (set<Node *>::iterator n2 = n->postset.begin(); n2 != n->postset.end(); n2++) {
			// already seen in this search
			if (fromAlpha.find(*n2) != fromAlpha.end())
				continue;
			searchStack.push(*n2);
		}
	}

	set< Node* > fromAlphaToOmega;
	for (set<Place *>::iterator n = P.begin(); n != P.end(); n++) {
		string name = (*n)->nodeFullName();
		if (name.find("omega", 0) != string::npos) {
			searchStack.push(*n);
		}
	}

	while (!searchStack.empty()) {
		Node *n = searchStack.top(); searchStack.pop();
		fromAlphaToOmega.insert(n);

		for (set<Node *>::iterator n2 = n->preset.begin(); n2 != n->preset.end(); n2++) {
			// already seen in this search
			if (fromAlphaToOmega.find(*n2) != fromAlphaToOmega.end())
				continue;
			// node is not reachable from alpha, but from omega
			if (fromAlpha.find(*n2) == fromAlpha.end()) {
				//cerr << "wf-struct: node " << (*n2)->nodeFullName() << " is not on a path from alpha to omega" << endl;
				return *n2;
			}
			searchStack.push(*n2);
		}
	}

	for (set<Transition *>::iterator n = T.begin(); n != T.end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: transition " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}

	for (set<Place *>::iterator n = P.begin(); n != P.end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: place " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}

	for (set<Place *>::iterator n = P_in.begin(); n != P_in.end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: input place " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}

	for (set<Place *>::iterator n = P_out.begin(); n != P_out.end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: output place " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}
	return NULL;
}

/**
 * \brief anonymize the roles of all nodes of the net by making the id of
 *        each node its first role
 */
void ExtendedWorkflowNet::anonymizeNodes() {
  int currentId = 1;

  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    (*p)->history.push_front("p"+toString((*p)->id));
  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    (*p)->history.push_front("p"+toString((*p)->id));
  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    (*p)->history.push_front("p"+toString((*p)->id));

  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    (*t)->history.push_front("t"+toString((*t)->id));
}

/******************************************************************************
 * reduction rules on workflow nets
 *****************************************************************************/

/*!
 * \brief Fusion of series places (RA1):
 *
 * If there exists a transition with singleton preset and postset
 * (precondition 1) that are distinct (precondition 2) and where the place in
 * the preset has no other outgoing arcs (precondition 3), then the places
 * can be merged and the transition can be removed. Furthermore, none of the
 * places may be communicating (precondition 4) and the included arcs must have
 * a weight of 1 (precondition 5). Additionally, if we are going to fuse
 * two places, these places must have disjoint pre-sets and post-sets.
 *
 * This reduction rule preserves the k-boundedness of the net.
 *
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
*/
void ExtendedWorkflowNet::reduce_series_places()
{
  int result=0;
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA1(k) (fusion of series places preserving k-boundedness)...\n");

  set<string> uselessTransitions;
  set<pair<string, string> > placePairs;
  ////<Dirk.F> preserve (un)safeness of nets, 1 line
  set<Place *> touched;

  // iterate the transtions
  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
  {
    Place* prePlace = static_cast<Place*>(*((*t)->preset.begin()));
    Place* postPlace = static_cast<Place*>(*((*t)->postset.begin()));

    if (((*t)->preset.size() == 1) && ((*t)->postset.size() == 1) && // precondition 1
        (prePlace != postPlace) &&       // precondition 2
        ((prePlace)->postset.size() == 1) &&     // precondition 3
        (prePlace->type == INTERNAL) &&      // precondition 4
        (postPlace->type == INTERNAL) &&
        (arc_weight(prePlace, *t) == 1 && arc_weight(*t, postPlace) == 1)) // precondition 5
    {
      ////<Dirk.F> start, preserve (un)safeness of nets, 25 lines
      if (touched.find(prePlace) != touched.end())
          continue;
      if (touched.find(postPlace) != touched.end())
          continue;

      // check whether prePlace and postPlace have disjoint post-sets and disjoint pre-sets
      // if not then their fusion either changes the number of produces tokens or the
      // number of consumed tokens
      bool disjoint = true;
      for (set<Node*>::iterator t2 = prePlace->postset.begin(); t2 != prePlace->postset.end(); t2++) {
        if (postPlace->postset.find(*t2) != postPlace->postset.end()) {
          disjoint = false; break;
        }
      }
      if (!disjoint)
        continue;

      disjoint = true;
      for (set<Node*>::iterator t2 = prePlace->preset.begin(); t2 != prePlace->preset.end(); t2++) {
        if (postPlace->preset.find(*t2) != postPlace->preset.end()) {
          disjoint = false; break;
        }
      }
      if (!disjoint)
        continue;
      ////<Dirk.F> end, preserve (un)safeness of nets

      string id1 = ((*((*t)->preset.begin()))->nodeFullName());
      string id2 = ((*((*t)->postset.begin()))->nodeFullName());
      placePairs.insert(pair<string, string>(id1, id2));
      uselessTransitions.insert(((*t)->nodeFullName()));
      ////<Dirk.F> start, preserve (un)safeness of nets, 2 lines
      touched.insert(prePlace);
      touched.insert(postPlace);
      ////<Dirk.F> end, preserve (un)safeness of nets
    }
  }

  // remove useless transtions
  for (set<string>::iterator label = uselessTransitions.begin();
       label != uselessTransitions.end(); label++)
  {
    Transition* uselessTransition = findTransition(*label);
    if (uselessTransition != NULL)
      removeTransition(uselessTransition);
  }

  // merge place pairs
  for (set<pair<string, string> >::iterator placePair = placePairs.begin();
       placePair != placePairs.end(); placePair++)
  {
    Place *pMerged = mergePlaces(placePair->first, placePair->second);
    result++;
  }
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
}

/*!
 * \brief Fusion of series transition (RA2):
 *
 * If there exists a place with singleton preset and postset (precondition 1)
 * and if the transition in its postset has no other incoming arcs
 * (precondition 2), then the preset and the postset can be merged and the
 * place can be removed. Furthermore the in and outgoing arcs have to have
 * the same weight. (precondition 3). Two transitions are merged only if
 * they have disjoint pre-sets and post-sets.
 *
 * This reduction rule preserves the k-boundedness of the net.
 *
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
 */
void ExtendedWorkflowNet::reduce_series_transitions()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA2(k) (fusion of series transitions preserving k-boundedness)...\n");
  int result=0;

  set<string> uselessPlaces;
  set<pair<string, string> > transitionPairs;
  ////<Dirk.F> preserve (un)safeness of nets, 1 line
  set<Transition *> touched;

  // iterate the places
  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
  {
    if (((*p)->postset.size() == 1) && ((*p)->preset.size() == 1)) // precondition 1
    {
      Transition* t1 = static_cast<Transition*>(*((*p)->preset.begin()));
      Transition* t2 = static_cast<Transition*>(*((*p)->postset.begin()));

      ////<Dirk.F> start, preserve (un)safeness of nets, 24 lines
      if (touched.find(t1) != touched.end())
          continue;
      if (touched.find(t2) != touched.end())
          continue;

      // check whether t1 and t2 have disjoint post-sets and disjoint pre-sets
      bool disjoint = true;
      for (set<Node*>::iterator p2 = t1->postset.begin(); p2 != t1->postset.end(); p2++) {
        if (t2->postset.find(*p2) != t2->postset.end()) {
          disjoint = false; break;
        }
      }
      if (!disjoint)
        continue;

      disjoint = true;
      for (set<Node*>::iterator p2 = t1->preset.begin(); p2 != t1->preset.end(); p2++) {
        if (t2->preset.find(*p2) != t2->preset.end()) {
          disjoint = false; break;
        }
      }
      if (!disjoint)
        continue;
      ////<Dirk.F> end, preserve (un)safeness of nets

      if (((t2)->preset.size() == 1) && // precondition 2
          (arc_weight(t1, *p) == arc_weight(*p, t2))) // precondition 5
      {
        string id1 = (t1->nodeFullName());
        string id2 = (t2->nodeFullName());
        transitionPairs.insert(pair<string, string>(id1, id2));
        uselessPlaces.insert(((*p)->nodeFullName()));
        ////<Dirk.F> start, preserve (un)safeness of nets, 2 lines
        touched.insert(t1);
        touched.insert(t2);
        ////<Dirk.F> end, preserve (un)safeness of nets
      }
    }
  }


  // remove useless places
  for (set<string>::iterator label = uselessPlaces.begin();
       label != uselessPlaces.end(); label++)
  {
    Place *uselessPlace = findPlace(*label);
    removePlace(uselessPlace);
  }


  // merge transition pairs
  for (set<pair<string, string> >::iterator transitionPair = transitionPairs.begin();
       transitionPair != transitionPairs.end(); transitionPair++)
  {
    Transition* t1 = findTransition(transitionPair->first);
    Transition* t2 = findTransition(transitionPair->second);
    mergeTransitions(t1, t2);
    result++;
  }
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
}

/*!
 * Removes unnecessary nodes of the generated Petri net:
 *  - structural dead nodes
 *  - unused status places
 *  - suspicious transitions
 *
 * Applys some simple structural reduction rules for Petri nets:
 *  - elimination of identical places (RB1)
 *  - fusion of series transitions (RA2)
 *
 * The rules are applied until a fixed point (i.e. the number of places,
 * transitions and arcs does not change) is reached.
 *
 * \return the number of passes until a fixed point was reached
 *
 * \todo
 *       - Improve performance of the rules.
 *       - Pass a parameter to this function to choose the property of the
 *         model to be preserved.
 */
unsigned int ExtendedWorkflowNet::reduce(unsigned int reduction_level)
{
  trace(TRACE_DEBUG, "[PN]\tWorkflow size before simplification: " + information() + "\n");
  trace(TRACE_INFORMATION, "Simplifying workflow...\n");

  string old = information();
  bool done = false;
  unsigned int passes = 1;

  while (!done)
  {
    if (reduction_level >= 1)
    {
      //reduce_dead_nodes();
      reduce_series_places();   // RA1
    }
    /*
    if (reduction_level >= 2)
    {
      reduce_unused_status_places();
      reduce_suspicious_transitions();
    }
    */
    if (reduction_level >= 3)
    {
      reduce_identical_places();  // RB1
      //reduce_identical_transitions(); // RB2
    }
    /*
    if (reduction_level >= 4)
    {
      reduce_series_places();   // RA1
      reduce_series_transitions();  // RA2
    }

    if (reduction_level == 5)
    {
      reduce_self_loop_places();  // RC1
      reduce_self_loop_transitions(); // RC2
      reduce_remove_initially_marked_places_in_choreographies();
    }

    if (reduction_level == 6)
    {
      reduce_equal_places();    // RD1
    }
    */

    trace(TRACE_DEBUG, "[PN]\tWorkflow size after simplification pass " + toString(passes++) + ": " + information() + "\n");

    done = (old == information());
    old = information();
  }


  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  trace(TRACE_DEBUG, "[PN]\tWorkflow size after simplification: " + information() + "\n");

  return passes;
}
