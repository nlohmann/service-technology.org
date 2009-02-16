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
#include <typeinfo>

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

bool ExtendedWorkflowNet::isConnected() const {
  set<Node*> reachableNodes;
  stack<Node*> searchStack;

  searchStack.push(*(P.begin()));
  while (!searchStack.empty()) {
    Node *n = searchStack.top(); searchStack.pop();
    reachableNodes.insert(n);

    for (set<Node *>::iterator n2 = n->preset.begin(); n2 != n->preset.end(); n2++) {
      // already seen in this search
      if (reachableNodes.find(*n2) != reachableNodes.end())
        continue;
      searchStack.push(*n2);
    }
    for (set<Node *>::iterator n2 = n->postset.begin(); n2 != n->postset.end(); n2++) {
      // already seen in this search
      if (reachableNodes.find(*n2) != reachableNodes.end())
        continue;
      searchStack.push(*n2);
    }
  }

  int totalNodeNum = P.size() + P_in.size() + P_out.size() + T.size();
  return (totalNodeNum == reachableNodes.size());
}

bool ExtendedWorkflowNet::isStructurallyCorrect() const {
  if (process_endNodes.size() == 0 && process_stopNodes.size() == 0)
    return false;
  if (process_startNodes.size() == 0)
    return false;

  return true;
}

/*!
 * \result  size of the input interface
 */
int ExtendedWorkflowNet::inputSize() const {
  return P_in.size();
}

/*!
 * \result  size of the output interface
 */
int ExtendedWorkflowNet::outputSize() const {
  return P_out.size();
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
    trace(TRACE_DEBUG, "trying to remove "+(*label));
    Transition* uselessTransition = findTransition(*label);
    if (uselessTransition != NULL) {
      removeTransition(uselessTransition);
      trace(TRACE_DEBUG, " done");
    }
    trace(TRACE_DEBUG, "\n");
  }

  // merge place pairs
  for (set<pair<string, string> >::iterator placePair = placePairs.begin();
       placePair != placePairs.end(); placePair++)
  {
    mergePlaces(placePair->first, placePair->second);
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

/*!
 * \brief   merge the given two places of the net, extends the standard
 *          Petri net merge method by taking care of the sets of places
 *          of which the places to be merged may be part of
 */
Place* ExtendedWorkflowNet::mergePlaces(Place * & p1, Place * & p2) {

  if (p1 == p2 && p1 != NULL)
    return p1;

  // update all local fields of the workflow net

  // pins
  bool isPin = false;
  if (pinPlaces.find(p1) != pinPlaces.end()) {
    pinPlaces.erase(p1);
    isPin = true;
  }
  if (pinPlaces.find(p2) != pinPlaces.end()) {
    pinPlaces.erase(p2);
    isPin = true;
  }

  // start nodes
  bool isStartNode = false;
  if (process_startNodes.find(p1) != process_startNodes.end()) {
    process_startNodes.erase(p1);
    isStartNode = true;
  }
  if (process_startNodes.find(p2) != process_startNodes.end()) {
    process_startNodes.erase(p2);
    isStartNode = true;
  }

  // end nodes
  bool isEndNode = false;
  if (process_endNodes.find(p1) != process_endNodes.end()) {
    process_endNodes.erase(p1);
    isEndNode = true;
  }
  if (process_endNodes.find(p2) != process_endNodes.end()) {
    process_endNodes.erase(p2);
    isEndNode = true;
  }

  // stop nodes
  bool isStopNode = false;
  if (process_stopNodes.find(p1) != process_stopNodes.end()) {
    process_stopNodes.erase(p1);
    isStopNode = true;
  }
  if (process_stopNodes.find(p2) != process_stopNodes.end()) {
    process_stopNodes.erase(p2);
    isStopNode = true;
  }

  // internal places of the net
  bool isInternal = false;
  if (process_internalPlaces.find(p1) != process_internalPlaces.end()) {
    process_internalPlaces.erase(p1);
    isInternal = true;
  }
  if (process_internalPlaces.find(p2) != process_internalPlaces.end()) {
    process_internalPlaces.erase(p2);
    isInternal = true;
  }

  // places denoting the use of a pinset
  bool isInputCriterionUsed = false;
  if (process_inputCriterion_used.find(p1) != process_inputCriterion_used.end()) {
    process_inputCriterion_used.erase(p1);
    isInputCriterionUsed = true;
  }
  if (process_inputCriterion_used.find(p2) != process_inputCriterion_used.end()) {
    process_inputCriterion_used.erase(p2);
    isInputCriterionUsed = true;
  }

  //cerr << "merging " << p1->id << " and " << p2->id << endl;

  Place* result = PetriNet::mergePlaces(p1,p2);

  if (isPin)
    pinPlaces.insert(result);
  if (isInternal && !(isEndNode || isStopNode))
    process_internalPlaces.insert(result);
  if (isStartNode)
    process_startNodes.insert(result);
  if (isEndNode)
    process_endNodes.insert(result);
  if (isStopNode)
    process_stopNodes.insert(result);
  if (isInputCriterionUsed)
    process_inputCriterion_used.insert(result);

  return result;
}

Place* ExtendedWorkflowNet::mergePlaces(string role1, string role2) {
  PetriNet::mergePlaces(role1,role2);
}

/*!
 * \brief   remove the given place from the net and update all sets of places
 *          of the workflow net
 */
void ExtendedWorkflowNet::removePlace(Place* p) {
  if (pinPlaces.find(p) != pinPlaces.end())
    pinPlaces.erase(p);
  if (process_startNodes.find(p) != process_startNodes.end())
    process_startNodes.erase(p);
  if (process_endNodes.find(p) != process_endNodes.end())
    process_endNodes.erase(p);
  if (process_stopNodes.find(p) != process_stopNodes.end())
    process_stopNodes.erase(p);
  if (process_internalPlaces.find(p) != process_internalPlaces.end())
    process_internalPlaces.erase(p);
  if (process_inputCriterion_used.find(p) != process_inputCriterion_used.end())
    process_inputCriterion_used.erase(p);

  PetriNet::removePlace(p);
}


/*!
 * \brief   merge the given two transitions of the net, extends the standard
 *          Petri net merge method by taking care of the sets of transitions
 *          of which the transitions may be part of
 */
Transition* ExtendedWorkflowNet::mergeTransitions(Transition * & t1, Transition * & t2) {

  if (t1 == t2)
    return t1;

  // update all local fields of the workflow net

  // input pinsets
  bool isInputPinset = false;
  if (process_inputPinSets.find(t1) != process_inputPinSets.end()) {
    process_inputPinSets.erase(t1);
    isInputPinset = true;
  }
  if (process_inputPinSets.find(t2) != process_inputPinSets.end()) {
    process_inputPinSets.erase(t2);
    isInputPinset = true;
  }

  // output pinsets
  bool isOutputPinset = false;
  if (process_outputPinSets.find(t1) != process_outputPinSets.end()) {
    process_outputPinSets.erase(t1);
    isOutputPinset = true;
  }
  if (process_outputPinSets.find(t2) != process_outputPinSets.end()) {
    process_outputPinSets.erase(t2);
    isOutputPinset = true;
  }

  //cerr << "merging " << t1->id << " and " << t2->id << endl;

  Transition* result = PetriNet::mergeTransitions(t1,t2);

  if (isInputPinset)
    process_inputPinSets.insert(result);
  if (isOutputPinset)
    process_outputPinSets.insert(result);

  return result;
}


/*!
 * \brief   remove the given place from the net and update all sets of places
 *          of the workflow net
 */
void ExtendedWorkflowNet::removeTransition(Transition* t) {
  process_inputPinSets.erase(t);
  process_outputPinSets.erase(t);

  PetriNet::removeTransition(t);
}

/*!
 * \brief remove places representing unconnected pins from the net
 *
 * \param PN  the net to be modified
 */
void ExtendedWorkflowNet::removeUnconnectedPins() {

  bool changed;

  do {
    changed = false;

    set<Place*> to_remove;      // list of places to be removed
    set<Node*>  maybeUnbounded; // list of possibly unbounded transitions

    for (set<Place *>::iterator it = pinPlaces.begin(); it != pinPlaces.end(); it++) {
      Place* p = static_cast<Place*>(*it);

      // if the place has an empty pre-set or post-set, remove it
      if (preset(p).empty() || postset(p).empty())
        to_remove.insert(p);
    }

    // finally remove all gathered nodes
    for (set< Place * >::iterator place = to_remove.begin(); place != to_remove.end(); place ++) {

        if (!(*place)->postset.empty()) {
          // we are going to delete a place,
          // its post-transitions may become unbounded
          maybeUnbounded.insert((*place)->postset.begin(), (*place)->postset.end());
        }

        //cerr << "Removing Place: " << (*place)->nodeFullName() << "\n";
        removePlace(*place);
        // remove place from BOM process structure as well
        process_internalPlaces.erase(*place);
        pinPlaces.erase(*place);
        changed = true;
    }

    // remove transitions that become unbounded because of removing
    // an unconnected pin
    for (set< Node * >::iterator t = maybeUnbounded.begin(); t != maybeUnbounded.end(); t++) {
        if ((*t)->preset.empty())
          removeTransition(static_cast<Transition*>(*t));
    }
    // keep removing until no more unconnected pins are removed
  } while (changed);
}

/*!
 * \brief makes all input places internal and add
 *      an initially marked 'alpha' place to the net
 */
void ExtendedWorkflowNet::soundness_initialPlaces() {

  // create new initially marked alpha-place
  Place* pAlpha = newPlace("alpha", INTERNAL);
  pAlpha->mark();

  set<Place*> removeNodes;
  set<Node*> noMoreInput;
  unsigned int setNum = 1;

  // create an activating transition for each input pinset of the process
  for (set<Transition *>::iterator inPS = process_inputPinSets.begin(); inPS != process_inputPinSets.end(); inPS++)
  {
    // the transition of the input pinset
    Transition* pinset_transition = static_cast<Transition *>(*inPS);
    set<Node *> inputPins = preset(pinset_transition);  // and the input pins

    if (inputPins.size() == 0)  // if the pinset has no transition (process without data input)
    {
      newArc(pAlpha, pinset_transition);  // add new arc to the pinset transition
      continue;
    }

    newArc(pAlpha, pinset_transition);
    noMoreInput.insert(pinset_transition);

    for (set<Node *>::iterator it = inputPins.begin(); it != inputPins.end(); it++)
    {
      Place* place = static_cast<Place*>(*it);
          // choose the 'old' input place for removal
        removeNodes.insert(place);
    }
    setNum++;
  }

  // remove all chosen places
  for (set<Place*>::iterator p = removeNodes.begin(); p != removeNodes.end(); p++) {
    //cerr << "removing place " << (*p)->nodeFullName() << endl;
      removePlace(*p);
  }
  // make all post-transitions that had an input-place internal transitions
  for (set<Node*>::iterator t = noMoreInput.begin(); t != noMoreInput.end(); t++) {
    //cerr << "making transition " << (*t)->nodeFullName() << " internal" << endl;
      (*t)->type = INTERNAL;
  }
}


/*!
 * \brief implement the termination semantics for the net by adding and removing
 *        places and transitions, some places will be marked as final, together
 *        with information from the BomProcess, this information can be used
 *        to construct verification properties to verify the correctness of the
 *        net
 *
 * we provide four kinds of termination semantics
 * #1: the UML2 termination semantics; the process control-flow
 *     terminates via an implicit OR-join, the data-flow terminates via an
 *     XOR-join
 *      [parameter: termination = TERM_UML_STANDARD (standard) ]
 *
 *     this semantics can be suppelemented with live-locking loops on
 *     the terminal places of the net
 *      [parameter: livelocks]
 *
 * #2: the workflow net semantics, all control- and data-flow terminates
 *     on a single "omega" places, we thereby assume that data-flow terminates
 *     via an XOR-join while all control-flow terminates via an AND-join
 *      [parameter: termination = TERM_WORKFLOW ]
 *
 *     this semantics can be suppelemented with live-locking loops on
 *     the terminal places of the net
 *      [parameter: livelocks]
 *
 * #3: the control-flow OR-join termination semantics which removes data-flow
 *     from the termination semantics and let's all control-flow terminate via
 *     an implicit OR-join
 *      [parameter: termination = TERM_IGNORE_DATA ]
 *
 * #4: all control and data-flow terminates with an implicit OR-join
 *      [parameter: termination = TERM_ORJOIN ]
 *
 * \param liveLocks  introduce live-locks at final state places
 * \param stopNodes  distinguish stop nodes from end nodes
 * \param termination  switch, which termination semantics should be used
 */
void ExtendedWorkflowNet::soundness_terminalPlaces(bool liveLocks, bool stopNodes, terminationSemantics_t termination)
{
  set<Place*>      places_to_remove;
  set<Transition*> transitions_to_remove;
  set<Node*>       noMoreOutput;
  set<Place*>      omegaPlaces;      // set of new omega places

  // make all final places (as found in the final marking) non-final
  set<Place *>  finalPlaces = getFinalPlaces();
  for (set<Place *>::iterator place = finalPlaces.begin(); place != finalPlaces.end(); place ++)
  {
    (*place)->isFinal = false;
      // possible extension: introduce omega-place as unqiue final place
  }

  // preparation for termination semantics #1 ("standard")
  if (termination == TERM_UML_STANDARD) { // in case of an OR-join: add unique omega-place
    Place *omega = newPlace("omega");
    omega->isFinal = true,
    omegaPlaces.insert(omega);
  }

  // preparation for termination semantics #4 ("OR-join")
  // In case we do an OR-join, the places that related input pinsets to
  // output pinsets, and stopNodes to output pinsets are no longer useful.
  if (termination == TERM_ORJOIN) {
    for (set<Place*>::iterator p = process_inputCriterion_used.begin(); p != process_inputCriterion_used.end(); p++)
      places_to_remove.insert(*p);
    for (set<Place*>::iterator p = places_to_remove.begin(); p != places_to_remove.end(); p++)
      removePlace(*p);
    places_to_remove.clear();
  }

  // we strip the termination part of the current net removing redundant structure
  // and extend the remaining net with places and transitions according to the
  // chosen termination semantics


  // first modify the post-places of each output pinset transition
  for (set<Transition *>::iterator outPS = process_outputPinSets.begin(); outPS != process_outputPinSets.end(); outPS++)
  {
    // remove the outer data-flow output places
    set<Node *> outputPins = (*outPS)->postset; // and the output pins
    for (set<Node *>::iterator it = outputPins.begin(); it != outputPins.end(); it++)
    {
      Place* place = static_cast<Place*>(*it);
      places_to_remove.insert(place);
    }
    // the output pinset transition is not an output transition anymore
    noMoreOutput.insert((*outPS));

    // now extend the output pinset transition according to the termination
    // semantics

    if (termination == TERM_WORKFLOW) {
      // semantics #2 (workflow net)

      // keep pinsets: create omega post-place of the pinset-transition,
      // and add a live-locking loop if necessary
      Place* outputPlace = newPlace((*outPS)->nodeFullName() + "_omega");
      newArc((*outPS), outputPlace);
      outputPlace->isFinal = true;      // make it a final place

      omegaPlaces.insert(outputPlace);  // is one of the new final places

      if (liveLocks) {  // introduce live-lock at omega-place
        Transition* tLoop = newTransition(outputPlace->nodeFullName() + ".loop");
        newArc(outputPlace, tLoop, STANDARD, 1);
        newArc(tLoop, outputPlace, STANDARD, 1);
      }

    } else if (termination == TERM_IGNORE_DATA) {
      // semanics #3 ("no data-flow dependent termination")

      // do not keep pinsets: remove pinset transition
      transitions_to_remove.insert(*outPS);
      // and add a token consuming transition to each pre-place
      set<Node *> inputPins = (*outPS)->preset;
      for (set<Node *>::iterator it = inputPins.begin(); it != inputPins.end(); it++)
      {
        Place* place = static_cast<Place*>(*it);

        string tConsume_str = place->nodeFullName()+"_consume";
        // check whether transition already exists (overlapping pinsets)
        if (!findTransition(tConsume_str)) {
          Transition* tConsume = newTransition(tConsume_str);
          newArc(place, tConsume);
          // adding live-locks is not necessary
        }
      }

    } else if (termination == TERM_UML_STANDARD) {
      // semantics #1 ("standard")

      // add an arc producing a token on omega
      Place* omega = *omegaPlaces.begin();
      newArc(*outPS, omega, STANDARD, 1);

    } else if (termination == TERM_ORJOIN) {

      // see if the pinset has any data pins at all
      if ((*outPS)->preset.empty()) {
        // empty output pinset, this output transition is isolated, remove it
        Transition* t = static_cast<Transition*>(*outPS);
        // DO NOT ADD t FOR REMOVAL HERE, REMOVAL IS PERFORMED
        // AT THE END OF THIS METHOD
        //transitions_to_remove.insert(t);
      } else {
        // keep pinsets: create omega post-place of the pinset-transition,
        // marking this place indicates that the process terminated via this
        // (data) output pinset
        Place* outputPlace = newPlace((*outPS)->nodeFullName() + "_omega");
        newArc((*outPS), outputPlace);
        outputPlace->isFinal = true;      // make it a final place
      }
    }
  }

  // now connect the end- and stop- nodes to the output pinset transitions to
  // synchronize control-flow with data-flow upon termination
  if (termination != TERM_IGNORE_DATA)
  {
    // termination sematnics #1, #2 and #4

    // implement the BOM process termination semantics in petri nets:
    // when checking soundness, transition which consume from
    // end-nodes must be made looping, this is necessary to make sure
    // that the soundness-formula for correct termination on end nodes
    //   AG EF (endNode > 0 AND p1 = 0 AND ... p17 = 0)
    // remains true once an endNode carries a token
    // (consuming from endNode must not violate the formula)


#ifdef DEBUG
    /*
        cerr << "iterating over all endNodes:";
        for (set<Place *>::iterator it = process_endNodes.begin(); it != process_endNodes.end(); it++)
          cerr << " " << (*it)->id;
        cerr << endl;
     */
#endif

    // include all end nodes in the process termination semantics
    for (set<Place *>::iterator it = process_endNodes.begin(); it != process_endNodes.end(); it++) {
      Place* p = static_cast<Place*>(*it);

      if (termination == TERM_WORKFLOW) {
        // termination semantics #2 ("workflow net")

        // remove post-transition ".eat" of end place p
        Transition* t = static_cast<Transition*>(*(postset(p).begin()));
        transitions_to_remove.insert(t);

        // try to create a workflow net: each output pinset has its own
        // omega transition and its own omega-place, add each end node
        // to the preset of each output pinset-omega transition
        for (set<Transition *>::iterator outPS = process_outputPinSets.begin();
             outPS != process_outputPinSets.end(); outPS++)
        {
          newArc(p, (*outPS));
        }

      } else if (termination == TERM_ORJOIN) {
        // each endNode is a final place, marking this place means that
        // the process terminated via this endNode
        p->isFinal = true;

        // remove post-transition ".eat" of end place p
        Transition* t = static_cast<Transition*>(*(postset(p).begin()));
        transitions_to_remove.insert(t);
      }

    }

    // include all stop nodes in the process termination semantics
    // warning code duplication in case we treat stop-nodes as end-nodes
    for (set<Place *>::iterator it = process_stopNodes.begin(); it != process_stopNodes.end(); it++) {
      Place* p = static_cast<Place*>(*it);

      if (termination == TERM_WORKFLOW) {
        // termination semantics #2 ("workflow net")

        // remove post-transition ".eat" of end place p
        Transition* t = static_cast<Transition*>(*(postset(p).begin()));
        transitions_to_remove.insert(t);
      }

      if (stopNodes) {
        // distinguish stop nodes from end-nodes

        if (termination == TERM_ORJOIN) {
          // each stopNode is a final place, marking this place means that
          // the process terminated via this stopNode
          p->isFinal = true;

          // remove post-transition ".eat" of stop place p
          Transition* t = static_cast<Transition*>(*(postset(p).begin()));
          transitions_to_remove.insert(t);
        }

      } else {
        // treat stop nodes like end nodes
        if (termination == TERM_WORKFLOW) {
          // termination semantics #2 ("workflow net")

          // try to create a workflow net: each output pinset has its own
          // omega transition and its own omega-place, add each end node
          // to the preset of each output pinset-omega transition
          for (set<Transition *>::iterator outPS = process_outputPinSets.begin();
               outPS != process_outputPinSets.end(); outPS++)
          {
            newArc(p, (*outPS));
          }
        }

        else if (termination == TERM_ORJOIN) {
          // each endNode is a final place, marking this place means that
          // the process terminated via this endNode
          p->isFinal = true;

          // remove post-transition ".eat" of end place p
          Transition* t = static_cast<Transition*>(*(postset(p).begin()));
          transitions_to_remove.insert(t);
        }
      }
    }

    // finishing touches after control- and data-flow are synchronized
    // a workflow net may have only one omega place, create it here
    if (termination == TERM_WORKFLOW) {
      // termination semenatics #2 ("workflow net")
      Place* processOmega = newPlace("omega");
      for (set<Place *>::iterator omega = omegaPlaces.begin();
                    omega != omegaPlaces.end(); omega++)
      {
        Transition* tOmega = newTransition((*omega)->nodeFullName() + ".finish");
        newArc(*omega, tOmega);
        newArc(tOmega, processOmega);
        (*omega)->isFinal = false;  // local omega is no longer a final place
      }
      processOmega->isFinal = true; // is the only final place in the process
    }
  } // end: connect end/stop nodes with pinsets

  // finally clean up the net from the nodes which we remembered for removal
  // remove the old interface output places
  for (set<Place*>::iterator p = places_to_remove.begin(); p != places_to_remove.end(); p++) {
      removePlace(*p);
  }

  // remove transitions, e.g. "endNode.eat"
  for (set<Transition*>::iterator t = transitions_to_remove.begin(); t != transitions_to_remove.end(); t++) {
      removeTransition(*t);
  }

  // make all pre-transitions of former output places internal
  for (set<Node*>::iterator t = noMoreOutput.begin(); t != noMoreOutput.end(); t++) {
      (*t)->type = INTERNAL;
  }

  // clean up the net, this code must be executed here until the
  // new Petri Net API is available
  if (termination == TERM_ORJOIN) {
    deleteIsolatedPinsetTransitions();
  }
}

/*!
 * \brief a work-around method to delete isolated pinset transitions
 *        this code could actually be executed within soundness_terminalPlaces,
 *        but if executed there, it provokes untraceable errors when deleting
 *        further transitions or freeing nets
 */
void ExtendedWorkflowNet::deleteIsolatedPinsetTransitions() {
  set<Transition*> transitions_to_remove;

  // check all output pinset transitions
  for (set<Transition *>::iterator outPS = process_outputPinSets.begin(); outPS != process_outputPinSets.end(); outPS++) {
    Transition* t = static_cast<Transition*>(*outPS);

    // see if the transition is isolated
    if (t->preset.empty() && t->postset.empty()) {
      transitions_to_remove.insert(t);
    }
  }

  // remove all isolated transitions
  for (set<Transition*>::iterator t = transitions_to_remove.begin(); t != transitions_to_remove.end(); t++) {
      removeTransition(*t);
  }
}

/*!
 * \brief construct a formula the describes the legal final states of the
 *        process considering the different termination semantics due to stop
 *        nodes and end nodes, the construction of the formula is based on the
 *        fields of this object that distinguish internal nodes, stop nodes, and
 *        end nodes
 *
 * \param PN  a net that formalizes the process of this object
 */
FormulaState* ExtendedWorkflowNet::createFinalStatePredicate() const
{
  FormulaState* mainF = new FormulaState(LOG_OR);

  // create a literal p = 0 for each internal place p of the process
  set<PetriNetLiteral *>  processNodesLit_zero;
  for (set<Place *>::iterator it = process_internalPlaces.begin(); it != process_internalPlaces.end(); it++) {
    Place* p = static_cast<Place*>(*it);
    processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_EQUAL, 0));
  }

  // create all stop node formulae, iterate over all stop nodes
  //
  // stop node formula:   pStop > 0
  for (set<Place *>::iterator it = process_stopNodes.begin(); it != process_stopNodes.end(); it++) {
    Place* p = static_cast<Place*>(*it);
    PetriNetLiteral* pnLit = new PetriNetLiteral(p, COMPARE_GREATER, 0);

    // build disjunction: pStop > 0 OR (other final state formulas)
    mainF->subFormulas.insert(pnLit);
  }

  // create all end node formulae, iterate over all end nodes
  //
  // end node formula:   pEnd > 0 AND p1 = 0 AND ... AND p17 = 0 (pXY internal node only)
  for (set<Place *>::iterator it = process_endNodes.begin(); it != process_endNodes.end(); it++) {
    Place* p = static_cast<Place*>(*it);
    PetriNetLiteral* pnLit = new PetriNetLiteral(p, COMPARE_GREATER, 0);
    FormulaState* subF = new FormulaState(LOG_AND);

    subF->subFormulas.insert(pnLit);     // endNode > 0 AND

    if (processNodesLit_zero.size() > 0) // all other internal places = 0
      subF->subFormulas.insert(processNodesLit_zero.begin(),
                               processNodesLit_zero.end());

    // build disjunction: endNodeFormula OR (other final state formulas)
    mainF->subFormulas.insert(subF);
  }
  return mainF;
}

/*!
 * \brief construct a formula the describes the legal final states of the
 *        process, the construction of the formula is based on the declaration
 *        of Petri net places as "final places"
 *
 * \param PN  a net that formalizes a workflow
 * \param stopNodes  switch whether final places shall be treated as stopNodes
 *                   which clear the tokens on all internal places, this
 *                   functionality is currently not implemented and returns NULL
 */
FormulaState* ExtendedWorkflowNet::createOmegaPredicate(bool stopNodes) const
{
  FormulaState* mainF;

  // create a literal p = 0 for each internal place p of the process
  set<Place*> PN_P = getInternalPlaces();
  set<PetriNetLiteral *>  processNodesLit_zero;
  for (set<Place *>::iterator it = PN_P.begin(); it != PN_P.end(); it++) {
    Place* p = static_cast<Place*>(*it);

    if (p->isFinal) // skip final places (= omega place)
      continue;
    // all non-final places must have no token in the terminal state
    processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_EQUAL, 0));
  }

  if (stopNodes) {
    return NULL;
  } else {
    FormulaState* omegaF = new FormulaState(LOG_OR);

    // create a literal "> 0" for all final places (= omega places)
    set<Place *>  finalPlaces = getFinalPlaces();
    for (set<Place*>::iterator it = finalPlaces.begin(); it != finalPlaces.end(); it++)
    {
      Place* p = static_cast<Place*>(*it);
      // pOmega > 0
      omegaF->subFormulas.insert(new PetriNetLiteral(p, COMPARE_GREATER, 0));
    }

    // and build the conjunction
    // ( omega1 > 0 OR ... OR omegaN > 0) AND p1 = 0 AND ... AND pM = 0
    mainF = new FormulaState(LOG_AND);
    if (omegaF->size() > 0)
      mainF->subFormulas.insert(omegaF);
    mainF->subFormulas.insert(processNodesLit_zero.begin(), processNodesLit_zero.end());
  }

  return mainF;
}

FormulaState* ExtendedWorkflowNet::createSafeStatePredicate (bool mustBeNonEmpty=false) const
{
  // net is safe iff all internal places do not have more than one token
  // the analysis checks for the violation of the safe state
  FormulaState* mainF;

  // create a literal p = 0 for each internal place p of the process
  set<Place*> PN_P = getInternalPlaces();
  set<PetriNetLiteral *>  processNodesLit_zero;
  for (set<Place *>::iterator it = PN_P.begin(); it != PN_P.end(); it++) {
    Place* p = static_cast<Place*>(*it);

    if (p->preset.size() <= 1) {
      // this place is only unsafe if its predecessor is unsafe
      // or if it is unsafely marked
      // TODO add check for initial marking
      continue;
    }
    // all internal places must have not more than one token
    processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_GREATER, 1));
  }

  // the post-places of all transitions that lay on a circle are also
  // candidates for getting more than one token
  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++) {
    set<Node*> reach;
    forwardReachableNodes(*t, &reach);
    if (reach.find(*t) != reach.end()) {
      // transition *t lies on a circle of the net
      for (set<Node *>::iterator p = (*t)->postset.begin(); p != (*t)->postset.end(); p++) {
        // all post-places *p of *t must have not more than one token
        processNodesLit_zero.insert(new PetriNetLiteral(static_cast<Place*>(*p), COMPARE_GREATER, 1));
      }
    }
  }

  // no place that could become unsafe
  if (processNodesLit_zero.empty()) {
    if (mustBeNonEmpty)
    {
      // create a literal p > 1 for an arbitrary place
      Place* p = (Place*)(*getInternalPlaces().begin());
      processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_GREATER, 1));
    }
    else
      return NULL;
  }
  assert(!processNodesLit_zero.empty());

  // disjunction over all literals
  FormulaState* safeF = new FormulaState(LOG_OR);
  safeF->subFormulas.insert(processNodesLit_zero.begin(), processNodesLit_zero.end());
  return safeF;
}


/*!
 * \brief   compute the set of all nodes that are forward reachable from node *n
 *          (by depth-first search), add resulting nodes to the
 *          set of nodes *reachable
 */
void ExtendedWorkflowNet::forwardReachableNodes(Node *n, set<Node*>* reachable) const {
  assert(reachable != NULL);

  list<Node *> stack;
  stack.push_front(n);
  // depth-first search on all nodes of the net that are reachable from node *n
  while (!stack.empty()) {
    Node *current = stack.front(); stack.pop_front();
    // follow the post-set relation
    for (set<Node*>::const_iterator post = current->postset.begin(); post != current->postset.end(); post++) {
      if (reachable->find(*post) == reachable->end()) {
        stack.push_front(*post);
        reachable->insert(*post);
      }
    }
  }
}

/*!
 * \brief   compute the set of all nodes that are forward reachable from node *n
 *          (by depth-first search), add resulting nodes to the
 *          set of nodes *reachable
 */
void ExtendedWorkflowNet::backwardReachableNodes(Node *n, set<Node*>* reachable) const {
  assert(reachable != NULL);

  list<Node *> stack;
  stack.push_front(n);
  // depth-first search on all nodes of the net that are reachable from node *n
  // via predecessor relation
  while (!stack.empty()) {
    Node *current = stack.front(); stack.pop_front();
    // follow the pre-set relation
    for (set<Node*>::const_iterator pre = current->preset.begin(); pre != current->preset.end(); pre++) {
      if (reachable->find(*pre) == reachable->end()) {
        stack.push_front(*pre);
        reachable->insert(*pre);
      }
    }
  }
}

/*!
 * \brief   extend a multi-terminal workflow net to a single-terminal
 *          workflow net
 *
 * \result  true iff the net could be completed to a single terminal workflow
 *          net while preserving soundness
 */
bool ExtendedWorkflowNet::complete_to_WFnet() {
  // breadth-first search to identify concurrent nodes

  list<Place *> omegaPlaces;

  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++) {
    if ((*p)->isFinal)
      omegaPlaces.push_back(*p);
  }

  map<Place *, set<Place*> > co;
  map<Place *, set<Place*> > confl;

  for (list<Place*>::iterator p = omegaPlaces.begin(); p != omegaPlaces.end(); p++) {
    set<Node *> past_p;
    list<Node *> stack;
    stack.push_front(*p);
    // backwards depth-first search to find all predecessors of place *p
    while (!stack.empty()) {
      Node *n = stack.front(); stack.pop_front();

      // search all predecessors of n
      for (set<Node*>::iterator pred = n->preset.begin(); pred != n->preset.end(); pred++) {
        if (past_p.find(*pred) == past_p.end()) {
          // put unvisited predecessor onto stack and add it to the visited nodes
          stack.push_front(*pred);
          past_p.insert(*pred);
        }
      }
    }
    // the set past_p contains all transitive predecessors of place *p

    // compare with all omega places
    for (list<Place*>::iterator p2 = omegaPlaces.begin(); p2 != omegaPlaces.end(); p2++) {
      if (*p == *p2)  // don't compare identical places
        continue;

      set<Node *> past_p2;
      stack.clear();
      stack.push_front(*p2);
      while (!stack.empty()) {
        Node *n = stack.front(); stack.pop_front();

        // search all predecessors of n
        for (set<Node*>::iterator pred = n->preset.begin(); pred != n->preset.end(); pred++) {
          if (past_p2.find(*pred) == past_p2.end()) {
            // we're just visiting predecessor *pred
            past_p2.insert(*pred);

            // check if *pred is also backwards reachable from place *p
            if (past_p.find(*pred) != past_p.end()) {
              // node *pred is a common predecessor of place *p and place *p2

              // if the predecessor is a transition, both places can be
              // marked concurrenctly
              if (typeid(**pred) == typeid(Transition)) {
                co[*p].insert(*p2);
                co[*p2].insert(*p);

              // if the predecessor is a place, both places can be
              // marked alternatively
              } else if (typeid(**pred) == typeid(Place)) {
                confl[*p].insert(*p2);
                confl[*p2].insert(*p);
              }
              // we found evidence on how *p and *p2 relate to each other
              // do not descend further
            } else {
              // no hit, search further before node *pred
              stack.push_front(*pred);
            }
          }
        } // finished searhcing all direct predecessors of node n
      } // finished searching all transitive predecessors of place *p2
    } // finished comparing the past of place *p to the past of all other places
  } // finished constructing the co and confl relation for all omega places

  // construct the set of all possible concurrent set of omega places
  set< set<Place*> > omegaCoSets;
  bool contradicting = false;
  for (list<Place*>::iterator p = omegaPlaces.begin(); p != omegaPlaces.end(); p++) {

    set<Place*> p_coset = co[*p];   // copy the co-set of p
    p_coset.insert(*p); // and add *p to its co-set as well

    if (p_coset.size() > 1) // only add the interesting coSets
      omegaCoSets.insert(p_coset);

/*
    for (set<Place*>::iterator p2 = co[*p].begin(); p2 != co[*p].end(); p2++) {
      cerr << "(" << (*p)->nodeFullName() << " || " << (*p2)->nodeFullName() << ") " << endl;
    }
*/

    // check if there are omega places which are concurrent AND conflicting
    // then the subsequent extension cannot create an AND-join transition to
    // consume tokens in case of concurrent marking of the places, but only for
    // the alternative case
    for (set<Place*>::iterator p2 = confl[*p].begin(); p2 != confl[*p].end(); p2++) {
//      cerr << "(" << (*p)->nodeFullName() << " # " << (*p2)->nodeFullName() << ") " << endl;
      if (co[*p].find(*p2) != co[*p].end())
        contradicting = true;
    }
//    cerr << endl;
  }

  // the constructed sets are an overapproximation, it may contain sets
  // that contain conflicting nodes, find these sets, and split them
  bool splitSets;
  do {
    splitSets = false;
    for (set< set<Place*> >::iterator coset = omegaCoSets.begin(); coset != omegaCoSets.end(); coset++ ) {
      // see if the current coset contains conflicting places
      for (set<Place*>::iterator p = (*coset).begin(); p != (*coset).end(); p++) {
        for (set<Place*>::iterator p2 = confl[*p].begin(); p2 != confl[*p].end(); p2++) {
          if ((*coset).find(*p2) != (*coset).end()) {
            // place *p2 is in conflict with place *p and both places are in
            // the current coset

/*
            cerr << "because of " << (*p)->nodeFullName() << " # " << (*p2)->nodeFullName() << endl;
            cerr << "splitting" << endl;
            cerr << "{";
            for (set<Place*>::iterator p3 = (*coset).begin(); p3 != (*coset).end(); p3++)
              cerr << (*p3)->nodeFullName() << ", " << endl << " ";
            cerr << "}" << endl << "to" << endl;
*/

            // create two copies of the coset
            set<Place*> coset_p = *coset;
            set<Place*> coset_p2 = *coset;
            // one not containing place *p, one not containing place *p2
            coset_p.erase(*p2);
            coset_p2.erase(*p);

            // remove the old coset and insert the new cosets
            omegaCoSets.erase(*coset);

            bool canBeMaximal = false;
            // check if the new cosets are maximal, i.e. describe reachable markings
            for (set<Place*>::iterator p3 = coset_p.begin(); p3 != coset_p.end(); p3++) {
              bool onePlaceMissing = false;
              for (set<Place*>::iterator p4 = co[*p3].begin(); p4 != co[*p3].end(); p4++) {
                // place *p4 is concurrently reachable to place *p3
                if (coset_p.find(*p4) == coset_p.end()) {
                  // place *p4 is not in the reduced coSet that contains place *p3
                  onePlaceMissing = true;
                  break;
                }
              }
              // for place *p3, all concurrent places are still in coset_p
              // so coset_p contains a maximal set of concurrent places
              if (!onePlaceMissing)
                canBeMaximal = true;
            }

            if (canBeMaximal) {
              omegaCoSets.insert(coset_p);

/*
              cerr << "{";
              for (set<Place*>::iterator p3 = coset_p.begin(); p3 != coset_p.end(); p3++)
                cerr << (*p3)->nodeFullName() << ", " << endl << " ";
              cerr << "}" << endl;
*/
            }

            canBeMaximal = false;
            // check if the new cosets are maximal, i.e. describe reachable markings
            for (set<Place*>::iterator p3 = coset_p2.begin(); p3 != coset_p2.end(); p3++) {
              bool onePlaceMissing = false;
              for (set<Place*>::iterator p4 = co[*p3].begin(); p4 != co[*p3].end(); p4++) {
                // place *p4 is concurrently reachable to place *p3
                if (coset_p2.find(*p4) == coset_p2.end()) {
                  // place *p4 is not in the reduced coSet that contains place *p3
                  onePlaceMissing = true;
                  break;
                }
              }
              // for place *p3, all concurrent places are still in coset_p2
              // so coset_p2 contains a maximal set of concurrent places
              if (!onePlaceMissing)
                canBeMaximal = true;
            }

            if (canBeMaximal) {
              omegaCoSets.insert(coset_p2);
/*
              cerr << "{";
              for (set<Place*>::iterator p3 = coset_p2.begin(); p3 != coset_p2.end(); p3++)
                cerr << (*p3)->nodeFullName() << ", " << endl<< " ";
              cerr << "}" << endl;
*/
            }

            splitSets = true;
            break;
          }
        }
        if (splitSets)  // leave for-loop to re-initialize the iterators
          break;
      }
      if (splitSets)  // leave for-loop to re-initialize the iterators
        break;
    }
  } while (splitSets);

  bool duplicateFound;
  do {
    duplicateFound = false;
    // for each coSet
    for (set< set<Place*> >::iterator coset = omegaCoSets.begin(); coset != omegaCoSets.end(); coset++ ) {
      // check if there is a second coSet s.t. both coSets are equal
      for (set< set<Place*> >::iterator coset2 = omegaCoSets.begin(); coset2 != omegaCoSets.end(); coset2++ ) {
        if (*coset == *coset2) continue;

        if ((*coset).size() == (*coset2).size()) {
          // we found a coet2 that is possibly equal to coset

          // see if the sets are different
          bool noDifference = true;
          for (set<Place*>::iterator p2 = (*coset2).begin(); p2 != (*coset2).end(); p2++) {
            if ( (*coset).find(*p2) == (*coset).end() ) {
              // place *p2 is not contained in the given coSet: they are not equal
              noDifference = false;
              break;
            }
          }

          if (noDifference) {
            duplicateFound = true;     // we found a duplicate set
            omegaCoSets.erase(*coset); // remove the duplicate set from the set of all coSets
            break;
          }
        }
      } // finished comparing to all existing coSets
    }
  } while (duplicateFound);

  // now construct the completion of the multi-terminal workflow net
  // add an AND-join transition for each coset of omega places
  int joinNum = 0;
  for (set< set<Place*> >::iterator coset = omegaCoSets.begin(); coset != omegaCoSets.end(); coset++ ) {
    Transition *t = newTransition("tANDjoin"+toString(joinNum));
    for (set<Place*>::iterator pre = (*coset).begin(); pre != (*coset).end(); pre++) {
      (*pre)->isFinal = false;
      newArc(*pre, t);    // add arc from omega place to the AND-join transition
    }
    Place *p = newPlace("pANDjoinOmega"+toString(joinNum));
    p->isFinal = true;
    newArc(t,p);  // add new omega place after AND-join transition
    joinNum++;
  }

  // add an XOR-join transition that that finishes all alternative completions
  // to a single final omega place
  Place *pOmega = newPlace("omega");

  joinNum = 0;
  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++ ) {
    if ((*p)->isFinal) {
      // create a transition consuming from the old omega node and producing
      // on the new omega node
      Transition *tFinal = newTransition("tXORjoin"+toString(joinNum));
      (*p)->isFinal = false;
      newArc(*p, tFinal);
      newArc(tFinal, pOmega);
      joinNum++;
    }
  }
  pOmega->isFinal = true;


  return !contradicting;
}

/*!
 * \brief   extend a multi-terminal workflow net to a single-terminal
 *          workflow net by structurally computing dead-path eliminiation
 *
 */
bool ExtendedWorkflowNet::complete_to_WFnet_DPE() {

  list<Place *> omegaPlaces;

  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++) {
    if ((*p)->isFinal) {
      omegaPlaces.push_back(*p);
      (*p)->isFinal = false;
    }
  }

  // will terminate the net via an AND-join on all omegaPlaces
  Transition* omegaANDJoin = newTransition("finalANDjoin");

  // To make this AND-join sound, we have to guarantee that if a final
  // marking is reached, all omegaPlaces get a token. To this end, we implement
  // dead-path-elimination that produces a token on an "omega"-place which
  // produces a "new" token on an omegaPlace whenever the net decides for a
  // branch that would not lead to marking this omegaPlace.

  // for each omega place
  for (list<Place*>::iterator omega = omegaPlaces.begin(); omega != omegaPlaces.end(); omega++) {
    // find all transitive predecessors
    set<Node *> backwards;
    backwardReachableNodes(*omega, &backwards);

    for (set<Node*>::iterator n = backwards.begin(); n != backwards.end(); n++) {
      if (typeid(**n) == typeid(Place)) {
        // for each *greyPlace that precedes place *omega, find all post-transitions
        // that do not precede *omega
        Place *greyPlace = static_cast<Place*>(*n);
        for (set<Node*>::iterator blackTransition = greyPlace->postset.begin(); blackTransition != greyPlace->postset.end(); blackTransition++) {
          if (backwards.find(*blackTransition) != backwards.end())
            continue;

          // the *blackTransition is not a predecessor of place *omega, if it
          // fires, then *omega will not get a token, therefore add an arc
          // to guarantee that *omega always gets marked
          newArc(*blackTransition, *omega);
        }
      }
    }

    // add an arc to the final AND join
    newArc(*omega, omegaANDJoin);
  }

  Place* omega = newPlace("omega");
  newArc(omegaANDJoin, omega);
  omega->isFinal = true;

  return true;
}
