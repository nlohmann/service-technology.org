/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2007, 2008, 2009  Dirk Fahland and Martin Znamirowski

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
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

#include <config.h>
#include <iostream>
#include <utility>
#include <typeinfo>

#include <stack>

#include "petrinet-workflow.h"
#include "helpers.h"		// helper functions (setUnion, setDifference, max, toString)
#include "debug.h"      // (trace)

using std::stack;
using std::set;
using std::cerr;
using std::endl;
using std::string;

namespace pnapi {

/******************************************************************************
 * Member methods on properties of extended workflow nets
 *****************************************************************************/

set< Node* > ExtendedWorkflowNet::isFreeChoice() const {

	set< Node* > resultSet;

	for (set<Transition *>::iterator it = getTransitions().begin(); it != getTransitions().end(); it++)
	{
		set<Transition *>::iterator it2 = it; it2++;	// get next transition
		for (; it2 != getTransitions().end(); it2++)
		{
			Transition* t = static_cast<Transition*>(*it);
			Transition* t2 = static_cast<Transition*>(*it2);

			bool empty = true;
			for (set<Node *>::iterator pre = t->getPreset().begin(); pre != t->getPreset().end(); pre++) {
				if (t2->getPreset().find(*pre) != t2->getPreset().end()) {
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

	//std::cout << "search alpha" << std::flush << endl;
	Place* p = findPlace("alpha");
	//std::cout << "done" << std::flush << endl;

	stack< Node* > searchStack;
	if (p == NULL) return false;
	else {
	  //std::cout << "push" << std::flush << endl;
	  searchStack.push(p);
	}
	//std::cout << "done" << endl;

	//std::cout << "fromAlpha" << endl;
	set< Node* > fromAlpha;
	while (!searchStack.empty()) {
		Node *n = searchStack.top(); searchStack.pop();
		fromAlpha.insert(n);

		for (set<Node *>::const_iterator n2 = n->getPostset().begin(); n2 != n->getPostset().end(); n2++) {
			// already seen in this search
			if (fromAlpha.find(*n2) != fromAlpha.end())
				continue;
			searchStack.push(*n2);
		}
	}
	//std::cout << "done" << endl;

	//std::cout << "find omega" << endl;
	for (set<Place *>::const_iterator n = getPlaces().begin(); n != getPlaces().end(); n++) {
		string name = (*n)->getName();
		if (name.find("omega", 0) != string::npos) {
			searchStack.push(*n);
		}
	}
	//std::cout << "find done" << endl;

	//std::cout << "fromAlphaToOmega" << endl;
	set< Node* > fromAlphaToOmega;
	while (!searchStack.empty()) {
		Node *n = searchStack.top(); searchStack.pop();
		fromAlphaToOmega.insert(n);

		for (set<Node *>::const_iterator n2 = n->getPreset().begin(); n2 != n->getPreset().end(); n2++) {
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
	//std::cout << "done" << endl;

	//std::cout << "find in fromAlphaToOmega: T" << endl;
	for (set<Transition *>::const_iterator n = getTransitions().begin(); n != getTransitions().end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: transition " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}

	//std::cout << "find in fromAlphaToOmega: P" << endl;
	for (set<Place *>::const_iterator n = getPlaces().begin(); n != getPlaces().end(); n++) {
		if (fromAlphaToOmega.find(*n) == fromAlphaToOmega.end()) {
			//cerr << "wf-struct: place " << (*n)->nodeFullName() << " is not on a path from alpha to omega" << endl;
			return *n;
		}
	}

	//std::cout << "done" << endl;

	return NULL;
}


// #include <sstream>  // (std::ostringstream, std::istringstream)

bool ExtendedWorkflowNet::isConnected() const {

  set<Node*> reachableNodes;
  stack<Node*> searchStack;

#ifdef DEBUG
  trace(TRACE_DEBUG, "---> init\n");
#endif
  searchStack.push(*(getInternalPlaces().begin()));
#ifdef DEBUG
  trace(TRACE_DEBUG, "---> 1\n");
#endif
  while (!searchStack.empty()) {
    Node *n = searchStack.top(); searchStack.pop();
    reachableNodes.insert(n);

#ifdef DEBUG
    //std::ostringstream buffer;
    //buffer << "---> " << n << " pre\n";
    //trace(TRACE_DEBUG, buffer.str());
#endif

    for (set<Node *>::iterator n2 = n->getPreset().begin(); n2 != n->getPreset().end(); n2++) {
      // already seen in this search
      if (reachableNodes.find(*n2) != reachableNodes.end())
        continue;
      searchStack.push(*n2);
    }

#ifdef DEBUG
    //std::ostringstream buffer2;
    //buffer2 << "---> " << n << " post\n";
    //trace(TRACE_DEBUG, buffer2.str());
#endif
    for (set<Node *>::iterator n2 = n->getPostset().begin(); n2 != n->getPostset().end(); n2++) {
      // already seen in this search
      if (reachableNodes.find(*n2) != reachableNodes.end())
        continue;
      searchStack.push(*n2);
    }
  }

#ifdef DEBUG
  trace(TRACE_DEBUG, "---> done\n");
#endif

  int totalNodeNum = getPlaces().size() + getTransitions().size();
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
  return getInputPlaces().size();
}

/*!
 * \result  size of the output interface
 */
int ExtendedWorkflowNet::outputSize() const {
  return getOutputPlaces().size();
}

/*!
 * \brief anonymize the roles of all nodes of the net
 */
void ExtendedWorkflowNet::anonymizeNodes() {
  int currentId = 1;

  for (set<Place *>::iterator p = getPlaces().begin(); p != getPlaces().end(); p++)
    (*p)->setName("p"+toString(++currentId));

  for (set<Transition *>::iterator t = getTransitions().begin(); t != getTransitions().end(); t++)
    (*t)->setName("t"+toString(++currentId));
}

/*!
 * \brief rename places of the net to data interface scheme
 */
void ExtendedWorkflowNet::renamePlacesToInterfaces() {

  // count names of places to generate unique IDs
  map<string, int>       sameIDcounter;

  for (std::set<Place *>::iterator p = getPlaces().begin(); p != getPlaces().end(); p++) {
    if ((*p)->getName().find("Data_") == 0 || (*p)->getName().find("Control_") == 0 ) {

      int idBegin = (*p)->getName().find_last_of("_");
      string name = (*p)->getName().substr(0,idBegin);

      // get most recent ID for this name from the net
      int id = 0;
      for (map<string, int>::iterator nameIDs = sameIDcounter.begin(); nameIDs != sameIDcounter.end(); nameIDs++ ) {
        if (name.compare(nameIDs->first) == 0) {
          id = nameIDs->second;
          break;
        }
      }

      id++; // increase

      // remember new id count and append to name
      sameIDcounter[name] = id;
      name = name + "_" + toString(id);

      (*p)->setName(name);
    }
  }
}

/*!
 *
 */
void ExtendedWorkflowNet::toLoLAIdent() {
  for (set<Place *>::iterator p = getPlaces().begin(); p != getPlaces().end(); p++) {
    (*p)->setName(toLoLAident((*p)->getName()));
  }

  for (set<Transition *>::iterator t = getTransitions().begin(); t != getTransitions().end(); t++) {
    (*t)->setName(toLoLAident((*t)->getName()));
  }

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

int ExtendedWorkflowNet::reduce_series_places()
{
  int result=0;
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA1(k) (fusion of series places preserving k-boundedness)...\n");

  set<Transition *> uselessTransitions;
  set<pair<string, string> > placePairs;
  ////<Dirk.F> preserve (un)safeness of nets, 1 line
  set<Place *> touched;

  // iterate the transitions
  for (set<Transition*>::iterator t = getTransitions().begin(); t != getTransitions().end(); t++)
  {
    Place* prePlace = static_cast<Place*>(*((*t)->getPreset().begin()));
    Place* postPlace = static_cast<Place*>(*((*t)->getPostset().begin()));

    if (((*t)->getPreset().size() == 1) && ((*t)->getPostset().size() == 1) && // precondition 1
        (prePlace != postPlace) &&       // precondition 2
        ((prePlace)->getPostset().size() == 1) &&     // precondition 3
        (prePlace->getType() == Node::INTERNAL) &&      // precondition 4
        (postPlace->getType() == Node::INTERNAL) &&
        (findArc(*prePlace, **t)->getWeight() == 1 && findArc(**t, *postPlace)->getWeight() == 1)) // precondition 5
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
      for (set<Node*>::iterator t2 = prePlace->getPostset().begin(); t2 != prePlace->getPostset().end(); t2++) {
        if (postPlace->getPostset().find(*t2) != postPlace->getPostset().end()) {
          disjoint = false; break;
        }
      }
      if (!disjoint)
        continue;

      disjoint = true;
      for (set<Node*>::iterator t2 = prePlace->getPreset().begin(); t2 != prePlace->getPreset().end(); t2++) {
        if (postPlace->getPreset().find(*t2) != postPlace->getPreset().end()) {
          disjoint = false; break;
        }
      }
      if (!disjoint)
        continue;
      ////<Dirk.F> end, preserve (un)safeness of nets

      placePairs.insert(pair<string, string>(prePlace->getName(), postPlace->getName()));
      uselessTransitions.insert(*t);
      ////<Dirk.F> start, preserve (un)safeness of nets, 2 lines
      touched.insert(prePlace);
      touched.insert(postPlace);
      ////<Dirk.F> end, preserve (un)safeness of nets
    }
  }

  // remove useless transtions
  for (set<Transition *>::iterator t = uselessTransitions.begin();
       t != uselessTransitions.end(); t++)
  {
    trace(TRACE_DEBUG, "trying to remove "+(*t)->getName());
    deleteTransition(**t);
    trace(TRACE_DEBUG, " done\n");
  }

  // merge the found places
  for (set<pair<string, string> >::iterator labels = placePairs.begin();
       labels != placePairs.end(); labels++)
  {
    Place* p1 = findPlace(labels->first);
    Place* p2 = findPlace(labels->second);
    if (p1 == NULL || p2 == NULL) continue;   // place no longer present (previous merge)
    mergePlaces(p1, p2);
    result++;
  }
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");

  return result;
}

/*!
 * \brief   Returns true only if the given node's incoming and outgoing arcs
 *          all have the same weight.
 *
 * \param   n  node to be examined
 *
 * \pre     n != NULL
 */
bool ExtendedWorkflowNet::sameweights(Node *n) const {
  assert(n != NULL);
  bool first = true;
  unsigned int w = 0;

  for (set<Arc*>::iterator f = getArcs().begin(); f != getArcs().end(); f++) {
    if ((&(*f)->getSourceNode() == n) || (&(*f)->getTargetNode() == n) )
      if (first) {
        first=false;
        w = (*f)->getWeight();
      } else {
        if ( (*f)->getWeight() != w)
          return false;
      }
  }
  return true;
}



/*!
 * \brief Elimination of identical places (RB1):
 *
 * If there exist two distinct (precondition 1) places with identical preset
 * (precondition 2) and postset (precondition 3) and the weights of all incoming
 * and outgoing arcs have the same value (precondition 4), then they can be merged.
 *
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
 */
int ExtendedWorkflowNet::reduce_identical_places()
{
  int result=0;
  set<pair<string, string> > placePairs;

  trace(TRACE_DEBUG, "[PN]\tApplying rule RB1 (elimination of identical places)...\n");

  // iterate the places
  for (set<Place*>::iterator p1 = getInternalPlaces().begin(); p1 != getInternalPlaces().end(); p1++)
  {
    if (((*p1)->getPreset().empty()) || ((*p1)->getPostset().empty()) || !(sameweights(*p1)))
      continue;

    for (set<Node*>:: iterator preTransition = (*p1)->getPreset().begin(); preTransition != (*p1)->getPreset().end(); preTransition++)
    {
      for (set<Node*>::iterator p2 = (*preTransition)->getPostset().begin(); p2 != (*preTransition)->getPostset().end(); p2++)
        if ((*p1 != *p2) &&   // precondition 1
            ((*p1)->getPreset() == (*p2)->getPreset()) && // precondition 2
            ((*p1)->getPostset() == (*p2)->getPostset()) && // precondition 3
            (sameweights(*p2)) && // precondition 4
            (findArc(**preTransition, **p1)->getWeight() == findArc(**p2, (**((*p1)->getPostset().begin())))->getWeight()) ) // precondition 4
        {
          string id1 = ((*p1)->getName());
          string id2 = ((*p2)->getName());
          placePairs.insert(pair<string, string>(id1, id2));
        }
    }
  }

  trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(placePairs.size()) + " places with same preset and postset...\n");

  // merge the found places
  for (set<pair<string, string> >::iterator labels = placePairs.begin();
       labels != placePairs.end(); labels++)
  {
    Place* p1 = findPlace(labels->first);
    Place* p2 = findPlace(labels->second);
    if (p1 == NULL || p2 == NULL) continue;   // place no longer present (previous merge)

    mergePlaces(p1, p2);
    result++;
  }
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");

  return result;
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

int ExtendedWorkflowNet::reduce_series_transitions()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA2(k) (fusion of series transitions preserving k-boundedness)...\n");
  int result=0;

  set<Place *> uselessPlaces;
  set<pair<string, string> > transitionPairs;
  ////<Dirk.F> preserve (un)safeness of nets, 1 line
  set<Transition *> touched;

  // iterate the places
  for (set<Place*>::iterator p = getInternalPlaces().begin(); p != getInternalPlaces().end(); p++)
  {
    if (((*p)->getPostset().size() == 1) && ((*p)->getPreset().size() == 1)) // precondition 1
    {
      Transition* t1 = static_cast<Transition*>(*((*p)->getPreset().begin()));
      Transition* t2 = static_cast<Transition*>(*((*p)->getPostset().begin()));

      ////<Dirk.F> start, preserve (un)safeness of nets, 24 lines
      if (touched.find(t1) != touched.end())
          continue;
      if (touched.find(t2) != touched.end())
          continue;

      // check whether t1 and t2 have disjoint post-sets and disjoint pre-sets
      bool disjoint = true;
      for (set<Node*>::iterator p2 = t1->getPostset().begin(); p2 != t1->getPostset().end(); p2++) {
        if (t2->getPostset().find(*p2) != t2->getPostset().end()) {
          disjoint = false; break;
        }
      }
      if (!disjoint)
        continue;

      disjoint = true;
      for (set<Node*>::iterator p2 = t1->getPreset().begin(); p2 != t1->getPreset().end(); p2++) {
        if (t2->getPreset().find(*p2) != t2->getPreset().end()) {
          disjoint = false; break;
        }
      }
      if (!disjoint)
        continue;
      ////<Dirk.F> end, preserve (un)safeness of nets

      if (((t2)->getPreset().size() == 1) && // precondition 2
          (findArc(*t1, **p)->getWeight() == findArc(**p, *t2)->getWeight())) // precondition 5
      {
        transitionPairs.insert(pair<string, string>(t1->getName(), t2->getName()));
        uselessPlaces.insert(*p);
        ////<Dirk.F> start, preserve (un)safeness of nets, 2 lines
        touched.insert(t1);
        touched.insert(t2);
        ////<Dirk.F> end, preserve (un)safeness of nets
      }
    }
  }


  // remove useless places
  for (set<Place*>::iterator p = uselessPlaces.begin();
       p != uselessPlaces.end(); p++)
  {
    deletePlace(**p);
  }


  // merge transition pairs
  for (set<pair<string, string> >::iterator transitionPair = transitionPairs.begin();
       transitionPair != transitionPairs.end(); transitionPair++)
  {
    Transition* t1 = findTransition(transitionPair->first);
    Transition* t2 = findTransition(transitionPair->second);
    if (t1 == NULL || t2 == NULL) continue;   // transition no longer present (previous merge)
    mergeTransitions(t1, t2);
    result++;
  }
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");

  return result;
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
  //trace(TRACE_DEBUG, "[PN]\tWorkflow size before simplification: " + information() + "\n");
  trace(TRACE_INFORMATION, "Simplifying workflow...\n");

  bool done = false;
  unsigned int passes = 1;

  int result;

  while (!done)
  {
    result = 0;

    if (reduction_level >= 1)
    {
      //reduce_dead_nodes();
      result += reduce_series_places();   // RA1
    }
//    if (reduction_level >= 2)
//    {
//      reduce_unused_status_places();
//      reduce_suspicious_transitions();
//    }
    if (reduction_level >= 3)
    {
      result += reduce_identical_places();  // RB1
      //reduce_identical_transitions();   // RB2
    }
//    if (reduction_level >= 4)
//    {
//      reduce_series_places();   // RA1
//      reduce_series_transitions();  // RA2
//    }
//
//    if (reduction_level == 5)
//    {
//      reduce_self_loop_places();  // RC1
//      reduce_self_loop_transitions(); // RC2
//      reduce_remove_initially_marked_places_in_choreographies();
//    }
//
//    if (reduction_level == 6)
//    {
//      reduce_equal_places();    // RD1
//    }

    //trace(TRACE_DEBUG, "[PN]\tWorkflow size after simplification pass " + toString(passes++) + ": " + information() + "\n");

    done = (result == 0);
  }


  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  //trace(TRACE_DEBUG, "[PN]\tWorkflow size after simplification: " + information() + "\n");

  return passes;
}


/*!
 * \brief   merge the given two places of the net, extends the standard
 *          Petri net merge method by taking care of the sets of places
 *          of which the places to be merged may be part of
 */
Place* ExtendedWorkflowNet::mergePlaces(pnapi::Place * & p1, pnapi::Place * & p2) {

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

  // final places
  bool isOmega = false;
  if (omegaPlaces.find(p1) != omegaPlaces.end()) {
    omegaPlaces.erase(p1);
    isOmega = true;
  }
  if (omegaPlaces.find(p2) != omegaPlaces.end()) {
    omegaPlaces.erase(p2);
    isOmega = true;
  }

  p1->merge(*p2, false, false);
  Place* result = p1;

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
  if (isOmega)
    omegaPlaces.insert(result);

  return result;
}

/*!
 * \brief   remove the given place from the net and update all sets of places
 *          of the workflow net
 */
void ExtendedWorkflowNet::deletePlace(pnapi::Place & place) {
  Place *p = &place;
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
  if (omegaPlaces.find(p) != omegaPlaces.end())
    omegaPlaces.erase(p);

  PetriNet::deletePlace(place);
}


/*!
 * \brief   merge the given two transitions of the net, extends the standard
 *          Petri net merge method by taking care of the sets of transitions
 *          of which the transitions may be part of
 */
Transition* ExtendedWorkflowNet::mergeTransitions(pnapi::Transition * & t1, pnapi::Transition * & t2) {

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

  t1->merge(*t2, false);
  Transition* result = t1;

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
void ExtendedWorkflowNet::deleteTransition(Transition & t) {
  process_inputPinSets.erase(&t);
  process_outputPinSets.erase(&t);

  PetriNet::deleteTransition(t);
}

/*!
 * \brief remove places representing unconnected pins from the net
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
      if (p->getPreset().empty() || p->getPostset().empty())
        to_remove.insert(p);
    }

    // finally remove all gathered nodes
    for (set< Place * >::iterator place = to_remove.begin(); place != to_remove.end(); place ++) {

        if (!(*place)->getPostset().empty()) {
          // we are going to delete a place,
          // its post-transitions may become unbounded
          maybeUnbounded.insert((*place)->getPostset().begin(), (*place)->getPostset().end());
        }

        //cerr << "Removing Place: " << (*place)->nodeFullName() << "\n";
        deletePlace(**place);
        // remove place from BOM process structure as well
        process_internalPlaces.erase(*place);
        pinPlaces.erase(*place);
        changed = true;
    }

    // remove transitions that become unbounded because of removing
    // an unconnected pin
    for (set< Node * >::iterator t = maybeUnbounded.begin(); t != maybeUnbounded.end(); t++) {
        if ((*t)->getPreset().empty())
          deleteTransition(*static_cast<Transition*>(*t));
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
  Place &pAlpha = createPlace("alpha");
  pAlpha.mark();

  set<Place*> removeNodes;
  unsigned int setNum = 1;

  // create an activating transition for each input pinset of the process
  for (set<Transition *>::iterator inPS = process_inputPinSets.begin(); inPS != process_inputPinSets.end(); inPS++)
  {
    // the transition of the input pinset
    Transition* pinset_transition = static_cast<Transition *>(*inPS);
    set<Node *> inputPins = pinset_transition->getPreset();  // and the input pins

    if (inputPins.size() == 0)  // if the pinset has no transition (process without data input)
    {
      createArc(pAlpha, *pinset_transition);  // add new arc to the pinset transition
      continue;
    }

    createArc(pAlpha, *pinset_transition);

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
      deletePlace(**p);
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

  // make all final places (as found in the final marking) non-final
  omegaPlaces.clear();

  // preparation for termination semantics #1 ("standard")
  if (termination == TERM_UML_STANDARD) { // in case of an OR-join: add unique omega-place
    Place &omega = createPlace("omega");
    omegaPlaces.insert(&omega);
  }

  // preparation for termination semantics #4 ("OR-join")
  // In case we do an OR-join, the places that related input pinsets to
  // output pinsets, and stopNodes to output pinsets are no longer useful.
  if (termination == TERM_ORJOIN) {
    for (set<Place*>::iterator p = process_inputCriterion_used.begin(); p != process_inputCriterion_used.end(); p++)
      places_to_remove.insert(*p);
    for (set<Place*>::iterator p = places_to_remove.begin(); p != places_to_remove.end(); p++)
      deletePlace(**p);
    places_to_remove.clear();
  }

  // we strip the termination part of the current net removing redundant structure
  // and extend the remaining net with places and transitions according to the
  // chosen termination semantics


  // first modify the post-places of each output pinset transition
  for (set<Transition *>::iterator outPS = process_outputPinSets.begin(); outPS != process_outputPinSets.end(); outPS++)
  {
    // remove the outer data-flow output places
    set<Node *> outputPins = (*outPS)->getPostset(); // and the output pins
    for (set<Node *>::iterator it = outputPins.begin(); it != outputPins.end(); it++)
    {
      Place* place = static_cast<Place*>(*it);
      places_to_remove.insert(place);
    }

    // now extend the output pinset transition according to the termination
    // semantics

    if (termination == TERM_WORKFLOW) {
      // semantics #2 (workflow net)

      // keep pinsets: create omega post-place of the pinset-transition,
      // and add a live-locking loop if necessary
      Place& outputPlace = createPlace((*outPS)->getName() + "_omega");
      createArc((**outPS), outputPlace);
      // make it a final place
      omegaPlaces.insert(&outputPlace);  // is one of the new final places

      if (liveLocks) {  // introduce live-lock at omega-place
        Transition& tLoop = createTransition(outputPlace.getName() + ".loop");
        createArc(outputPlace, tLoop);
        createArc(tLoop, outputPlace);
      }

    } else if (termination == TERM_IGNORE_DATA) {
      // semanics #3 ("no data-flow dependent termination")

      // do not keep pinsets: remove pinset transition
      transitions_to_remove.insert(*outPS);
      // and add a token consuming transition to each pre-place
      set<Node *> inputPins = (*outPS)->getPreset();
      for (set<Node *>::iterator it = inputPins.begin(); it != inputPins.end(); it++)
      {
        Place* place = static_cast<Place*>(*it);

        string tConsume_str = place->getName()+"_consume";
        // check whether transition already exists (overlapping pinsets)
        if (!findTransition(tConsume_str)) {
          Transition& tConsume = createTransition(tConsume_str);
          createArc(*place, tConsume);
          // adding live-locks is not necessary
        }
      }

    } else if (termination == TERM_UML_STANDARD) {
      // semantics #1 ("standard")

      // add an arc producing a token on omega
      Place* omega = *(omegaPlaces.begin());
      createArc(**outPS, *omega);

    } else if (termination == TERM_ORJOIN) {

      // see if the pinset has any data pins at all
      if ((*outPS)->getPreset().empty()) {
        // empty output pinset, this output transition is isolated, remove it
        Transition* t = static_cast<Transition*>(*outPS);
        // DO NOT ADD t FOR REMOVAL HERE, REMOVAL IS PERFORMED
        // AT THE END OF THIS METHOD
        //transitions_to_remove.insert(t);
      } else {
        // keep pinsets: create omega post-place of the pinset-transition,
        // marking this place indicates that the process terminated via this
        // (data) output pinset
        Place& outputPlace = createPlace((*outPS)->getName() + "_omega");
        createArc((**outPS), outputPlace);
        // make it a final place
        omegaPlaces.insert(&outputPlace);
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
        Transition* t = static_cast<Transition*>(*(p->getPostset().begin()));
        transitions_to_remove.insert(t);

        // try to create a workflow net: each output pinset has its own
        // omega transition and its own omega-place, add each end node
        // to the preset of each output pinset-omega transition
        for (set<Transition *>::iterator outPS = process_outputPinSets.begin();
             outPS != process_outputPinSets.end(); outPS++)
        {
          createArc(*p, (**outPS));
        }

      } else if (termination == TERM_ORJOIN) {
        // each endNode is a final place, marking this place means that
        // the process terminated via this endNode
        omegaPlaces.insert(p);

        // remove post-transition ".eat" of end place p
        Transition* t = static_cast<Transition*>(*(p->getPostset().begin()));
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
        Transition* t = static_cast<Transition*>(*(p->getPostset().begin()));
        transitions_to_remove.insert(t);
      }

      if (stopNodes) {
        // distinguish stop nodes from end-nodes

        if (termination == TERM_ORJOIN) {
          // each stopNode is a final place, marking this place means that
          // the process terminated via this stopNode
          omegaPlaces.insert(p);

          // remove post-transition ".eat" of stop place p
          Transition* t = static_cast<Transition*>(*(p->getPostset().begin()));
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
            createArc(*p, (**outPS));
          }
        }

        else if (termination == TERM_ORJOIN) {
          // each endNode is a final place, marking this place means that
          // the process terminated via this endNode
          omegaPlaces.insert(p);

          // remove post-transition ".eat" of end place p
          Transition* t = static_cast<Transition*>(*(p->getPostset().begin()));
          transitions_to_remove.insert(t);
        }
      }
    }

    // finishing touches after control- and data-flow are synchronized
    // a workflow net may have only one omega place, create it here
    if (termination == TERM_WORKFLOW) {
      // termination semenatics #2 ("workflow net")
      Place& processOmega = createPlace("omega");
      for (set<Place *>::iterator omega = omegaPlaces.begin();
                    omega != omegaPlaces.end(); omega++)
      {
        Transition& tOmega = createTransition((*omega)->getName() + ".finish");
        createArc(**omega, tOmega);
        createArc(tOmega, processOmega);
        //(*omega)->isFinal = false;  // local omega is no longer a final place
      }
      // is the only final place in the process
      omegaPlaces.insert(&processOmega);
    }
  } // end: connect end/stop nodes with pinsets

  // finally clean up the net from the nodes which we remembered for removal
  // remove the old interface output places
  for (set<Place*>::iterator p = places_to_remove.begin(); p != places_to_remove.end(); p++) {
      deletePlace(**p);
  }

  // remove transitions, e.g. "endNode.eat"
  for (set<Transition*>::iterator t = transitions_to_remove.begin(); t != transitions_to_remove.end(); t++) {
    deleteTransition(**t);
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
    if (t->getPreset().empty() && t->getPostset().empty()) {
      transitions_to_remove.insert(t);
    }
  }

  // remove all isolated transitions
  for (set<Transition*>::iterator t = transitions_to_remove.begin(); t != transitions_to_remove.end(); t++) {
      deleteTransition(**t);
  }
}

/*!
 * \brief construct a formula the describes the legal final states of the
 *        process considering the different termination semantics due to stop
 *        nodes and end nodes, the construction of the formula is based on the
 *        fields of this object that distinguish internal nodes, stop nodes, and
 *        end nodes
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

    // skip final places (= omega place)
    if (omegaPlaces.find(p) != omegaPlaces.end())
      continue;
    // all non-final places must have no token in the terminal state
    processNodesLit_zero.insert(new PetriNetLiteral(p, COMPARE_EQUAL, 0));
  }

  if (stopNodes) {
    return NULL;
  } else {
    FormulaState* omegaF = new FormulaState(LOG_OR);

    for (set<Place *>::const_iterator it = getPlaces().begin(); it != getPlaces().end(); ++it)
    {
      // create a literal "> 0" for all final places (= omega places)
      Place* p = static_cast<Place*>(*it);
      assert(p != NULL);

      // check if p is a final place
      if (omegaPlaces.find(p) == omegaPlaces.end())
        continue;

      // pOmega > 0
      PetriNetLiteral *lit = new PetriNetLiteral(p, COMPARE_GREATER, 0);
      omegaF->subFormulas.insert(lit);
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

    if (p->getPreset().size() <= 1) {
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
  for (set<Transition*>::iterator t = getTransitions().begin(); t != getTransitions().end(); t++) {
    set<Node*> reach;
    forwardReachableNodes(*t, &reach);
    if (reach.find(*t) != reach.end()) {
      // transition *t lies on a circle of the net
      for (set<Node *>::iterator p = (*t)->getPostset().begin(); p != (*t)->getPostset().end(); p++) {
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
void ExtendedWorkflowNet::forwardReachableNodes(const pnapi::Node *n, set<pnapi::Node*>* reachable) const {
  assert(reachable != NULL);

  list<const Node *> stack;
  stack.push_front(n);
  // depth-first search on all nodes of the net that are reachable from node *n
  while (!stack.empty()) {
    const Node *current = stack.front(); stack.pop_front();
    // follow the post-set relation
    for (set<Node*>::const_iterator post = current->getPostset().begin(); post != current->getPostset().end(); post++) {
      if (reachable->find(*post) == reachable->end()) {
        stack.push_front(*post);
        reachable->insert(*post);
      }
    }
  }
}

/*!
 * \brief   compute the set of all nodes that are backward reachable from node *n
 *          (by depth-first search), add resulting nodes to the
 *          set of nodes *reachable
 */
void ExtendedWorkflowNet::backwardReachableNodes(const pnapi::Node *n, set<pnapi::Node*>* reachable) const {
  assert(reachable != NULL);

  list<const Node *> stack;
  stack.push_front(n);
  // depth-first search on all nodes of the net that are reachable from node *n
  // via predecessor relation
  while (!stack.empty()) {
    const Node *current = stack.front(); stack.pop_front();
    // follow the pre-set relation
    for (set<Node*>::const_iterator pre = current->getPreset().begin(); pre != current->getPreset().end(); pre++) {
      if (reachable->find(*pre) == reachable->end()) {
        stack.push_front(*pre);
        reachable->insert(*pre);
      }
    }
  }
}

/*!
 * \brief   extend a multi-terminal workflow net to a single-terminal
 *          workflow net by structurally computing dead-path eliminiation
 *
 */
bool ExtendedWorkflowNet::complete_to_WFnet_DPE() {

  // will terminate the net via an AND-join on all omegaPlaces
  Transition& omegaANDJoin = createTransition("finalANDjoin");

  // To make this AND-join sound, we have to guarantee that if a final
  // marking is reached, all omegaPlaces get a token. To this end, we implement
  // dead-path-elimination that produces a token on an "omega"-place which
  // produces a "new" token on an omegaPlace whenever the net decides for a
  // branch that would not lead to marking this omegaPlace.

  // for each omega place
  for (set<Place*>::const_iterator omega = omegaPlaces.begin(); omega != omegaPlaces.end(); omega++) {
    // find all transitive predecessors
    set<Node *> backwards;
    backwardReachableNodes(*omega, &backwards);

    for (set<Node*>::const_iterator n = backwards.begin(); n != backwards.end(); n++) {
      if (typeid(**n) == typeid(Place)) {
        // for each *greyPlace that precedes place *omega, find all post-transitions
        // that do not precede *omega
        Place *greyPlace = static_cast<Place*>(*n);
        for (set<Node*>::iterator blackTransition = greyPlace->getPostset().begin(); blackTransition != greyPlace->getPostset().end(); blackTransition++) {
          if (backwards.find(*blackTransition) != backwards.end())
            continue;

          // the *blackTransition is not a predecessor of place *omega, if it
          // fires, then *omega will not get a token, therefore add an arc
          // to guarantee that *omega always gets marked
          createArc(**blackTransition, (**omega));
        }
      }
    }

    // add an arc to the final AND join
    createArc(**omega, omegaANDJoin);
  }
  omegaPlaces.clear();  // all former final places will no longer be final

  // the new final place is this new omega place
  Place& omega = createPlace("omega");
  createArc(omegaANDJoin, omega);
  omegaPlaces.insert(&omega);

  return true;
}

/******************************************************************************
 * Methods for modifying the Petri net with respect to roles.
 *****************************************************************************/
/*
// removes places, transitions and adjusts the interface according to the role set and mode
void ExtendedWorkflowNet::cutByRoles() {

    // dont cut anything if the net would become empty
    if(globals::keepRoles.empty() && globals::exclusiveRoles.empty() && globals::keepRolesExact.empty()) {
        return;
    }

    list<Transition*> deleteTransitions;
    bool remove;
    bool foundExcluded;

    // first find all transitions that do not fullfil the role requirements
    for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++) {

        remove = true;
        foundExcluded = false;

        if (!globals::keepRoles.empty() || !globals::exclusiveRoles.empty()) {
          if (!(*t)->roles.empty()) {
              for (set<string>::iterator tRole = (*t)->roles.begin(); tRole != (*t)->roles.end(); tRole ++) {
                  if (globals::keepRoles.find(*tRole) != globals::keepRoles.end()) {
                      remove = false;
                      break;
                  } else if (globals::exclusiveRoles.find((*tRole)) != globals::exclusiveRoles.end()) {
                      foundExcluded = true;
                  }
              }
              if (!globals::exclusiveRoles.empty()) {
                  if (!foundExcluded) {
                      remove = false;
                  }
              }
          }
        } else {
          if (globals::keepRolesExact.find((*t)->roles) != globals::keepRolesExact.end())
            remove = false;
        }

        if (remove) {
            bool hasAlpha = false;
            for (set<Node*>::const_iterator p = (*t)->preset.begin(); p != (*t)->preset.end(); p++) {
              if ((*p)->historyContains("process.alpha")) {
                hasAlpha = true;
                break;
              }
            }
            if (!hasAlpha)
              deleteTransitions.push_back(*t);
        }
    }

    // now delete all those transitions
    for (list<Transition*>::iterator t = deleteTransitions.begin(); t != deleteTransitions.end(); t++) {
        deleteTransition(*t);
    }

    // all places that were internal to a deletet role are now isolated and can be found ...
    list<Place*> removeIsolatedPlaces;
    for (set<Place*>::iterator p = P.begin(); p != P.end(); p++) {
        if ((*p)->postset.empty() && (*p)->preset.empty()) {
            removeIsolatedPlaces.push_back(*p);
        }
    }
    for (set<Place*>::iterator p = P_in.begin(); p != P_in.end(); p++) {
        if ((*p)->postset.empty() && (*p)->preset.empty()) {
            removeIsolatedPlaces.push_back(*p);
        }
    }
    for (set<Place*>::iterator p = P_out.begin(); p != P_out.end(); p++) {
        if ((*p)->postset.empty() && (*p)->preset.empty()) {
            removeIsolatedPlaces.push_back(*p);
        }
    }

    // ... and delete as well
    for (list<Place*>::iterator p = removeIsolatedPlaces.begin(); p != removeIsolatedPlaces.end(); p++) {
        if (*p)
        deletePlace(*p);
    }

    // sets for all places that now have to be made interface places
    list<Place*> turnToInput;
    list<Place*> turnToOutput;

    // find those places
    for (set<Place*>::iterator p = P.begin(); p != P.end(); p++) {
        if ((*p)->preset.empty() && !(*p)->marked()) {
            turnToInput.push_back(*p);
        } else if ((*p)->postset.empty()) {
            turnToOutput.push_back(*p);
        }
    }

    // create the new input interface
    for (list<Place*>::iterator p = turnToInput.begin(); p != turnToInput.end(); p++) {
        (*p)->type = IN;
        P.erase(*p);
        P_in.insert(*p);
        for (set<Node*>::iterator t = (*p)->postset.begin(); t != (*p)->postset.end(); t++) {
            (*t)->type = IN;
        }
    }

    // create the new output interface
    for (list<Place*>::iterator p = turnToOutput.begin(); p != turnToOutput.end(); p++) {
        (*p)->type = OUT;
        P.erase(*p);
        P_out.insert(*p);
        for (set<Node*>::iterator t = (*p)->preset.begin(); t != (*p)->preset.end(); t++) {
            (*t)->type = OUT;
        }
    }
}
*/

/*!
 * \brief Converts a given identifier into a LoLA/oWFN compatible identifier
 *      warning: conversion may project two different strings onto the same
 *      compatible string
 *
 * \param name  identifier to convert
 *
 * \pre name != NULL
 */
string toLoLAident(string name)
{
  bool nonUpperCaseFound = false;
  string result = "";

  // Display the list
  string::iterator it;
  for( it = name.begin(); it != name.end(); it++ )
  {
    switch (*it) {
      case ',':
      case ';':
      case ':':
      case '(':
      case ')':
      case '{':
      case '}':
      case ' ':
        result = result + "_";
        nonUpperCaseFound = true;
        break;
      default:
        if (!isupper(*it))
          nonUpperCaseFound = true;
        result = result + *it;
        break;
    }
   }
   if (!nonUpperCaseFound)
   {
      int i;
    for( i=0,it = result.begin(); it != result.end(); it++,i++ )
      result.replace(i, 1,1, tolower(*it));
   }
   return result;
}

} // namespace pnapi

