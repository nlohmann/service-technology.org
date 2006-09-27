/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\*****************************************************************************/

/*!
 * \file petrinet-reduction.cc
 *
 * \brief Reduction rules for Petri nets (implementation)
 *
 * This file implements the classes and functions defined in petrinet.h.
 *
 *  - PetriNet::removeInterface()
 *  - PetriNet::removeVariables()
 *  - PetriNet::removeUnusedStatusPlaces()
 *  - PetriNet::removeDeadNodes()
 *  - PetriNet::elminiationOfIdenticalPlaces()
 *  - PetriNet::elminiationOfIdenticalTransitions()
 *  - PetriNet::fusionOfSeriesPlaces()
 *  - PetriNet::fusionOfSeriesTransitions()
 *  - PetriNet::communicationInPostSet(Place *p)
 *  - PetriNet::collapseSequences()
 *  - PetriNet::simplify()
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created: 2006-03-16
 *          - last changed: \$Date: 2006/09/27 13:40:20 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.32 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <list>
#include <assert.h>
#include <set>

#include "petrinet.h"
#include "debug.h"		// debugging help
#include "helpers.h"
#include "options.h"





/******************************************************************************
 * External variables
 *****************************************************************************/

extern set<string> ASTE_variables; // needed for PetriNet::removeVariables()





/******************************************************************************
 * Functions to remove several aspects of the Petri net model
 *****************************************************************************/

/*!
 * Collect all input and output places and remove (i.e. detach) them. Formally
 * the inner of the generated open workflow is builded.
 */
void PetriNet::removeInterface()
{
  trace(TRACE_DEBUG, "[PN]\tRemoving interface places.\n");

  list<Place*> killList;

  for (set<Place*>::iterator p = P_in.begin(); p != P_in.end(); p++)
    killList.push_back(*p);

  for (set<Place*>::iterator p = P_out.begin(); p != P_out.end(); p++)
    killList.push_back(*p);

  for (list<Place*>::iterator it = killList.begin(); it != killList.end(); it++)
    removePlace(*it);

  hasNoInterface = true;
}





/*!
 * Remove the places modelling variables.
 */
void PetriNet::removeVariables()
{
  for (set<string>::iterator variable = ASTE_variables.begin();
      variable != ASTE_variables.end(); variable++)
  {
    removePlace( findPlace("variable." + *variable) );
  }

  removePlace( findPlace("1.internal.clock") );
}





/******************************************************************************
 * Functions to structurally simplify the Petri net model
 *****************************************************************************/

/*!
 * Remove status places that are not read by any transition.
 */
void PetriNet::removeUnusedStatusPlaces()
{
  list<Place*> unusedPlaces;

  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
  {
    if (postset(*p).empty() && !((*p)->historyContains("1.internal.final")))
      unusedPlaces.push_back(*p);
  }

  // remove unused places
  for (list<Place*>::iterator p = unusedPlaces.begin(); p != unusedPlaces.end(); p++)
    if (P.find(*p) != P.end())
      removePlace(*p);
}





/*!
 * Remove structural dead nodes.
 */
void PetriNet::removeDeadNodes()
{
  trace(TRACE_DEBUG, "[PN]\tRemoving structurally dead nodes...\n");
  
  bool done = false;

  while (!done)
  {
    done = true;
  
    list<Place*> deadPlaces;
    list<Transition*> deadTransitions;
    list<Place*> tempPlaces;

    // find unmarked places with empty preset
    for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
    {
      if (preset(*p).empty() && !(*p)->marked)
      {
	deadPlaces.push_back(*p);
	tempPlaces.push_back(*p);
	trace(TRACE_VERY_DEBUG, "[PN]\tPlace p" + toString((*p)->id) + " is structurally dead.\n");
	done = false;
      }
    }

    while (!tempPlaces.empty())
    {
      // p is a dead place
      Place* p = tempPlaces.back();
      assert(p != NULL);

      tempPlaces.pop_back();
      set<Node*> ps = postset(p);

      // transitions in the postset of a dead place are dead
      for (set<Node*>::iterator t = ps.begin(); t != ps.end(); t++)
      {
      	deadTransitions.push_back( (Transition*)(*t) );
	trace(TRACE_VERY_DEBUG, "[PN]\tTransition t" + toString((*t)->id) + " is structurally dead\n");
	done = false;
      }
    }


    // remove dead places and transitions
    for (list<Place*>::iterator p = deadPlaces.begin(); p != deadPlaces.end(); p++)
      if (P.find(*p) != P.end())
	removePlace(*p);

    for (list<Transition*>::iterator t = deadTransitions.begin(); t != deadTransitions.end(); t++)
      if (T. find(*t) != T.end())
	removeTransition(*t);

    

    // remove isolated communication places

    list<Place*> uselessCommunicationPlaces;

    for (set<Place*>::iterator p = P_in.begin(); p != P_in.end(); p++)
      if (postset(*p).empty())
	uselessCommunicationPlaces.push_back(*p);

    for (list<Place*>::iterator p = uselessCommunicationPlaces.begin(); p != uselessCommunicationPlaces.end(); p++)
      if (P_in.find(*p) != P_in.end())
	P_in.erase(*p);

    uselessCommunicationPlaces.clear();

    for (set<Place*>::iterator p = P_out.begin(); p != P_out.end(); p++)
      if (preset(*p).empty())
	uselessCommunicationPlaces.push_back(*p);

    for (list<Place*>::iterator p = uselessCommunicationPlaces.begin(); p != uselessCommunicationPlaces.end(); p++)
      if (P_out.find(*p) != P_out.end())
	P_out.erase(*p);
  }
}





/*!
 * \brief Elimination of identical places (RB1):
 *
 * If there exist two distinct (precondition 1) places with identical preset
 * (precondition 2) and postset (precondition 3), then they can be merged.
 */
void PetriNet::elminiationOfIdenticalPlaces()
{
  set<pair<string, string> > placePairs;

  trace(TRACE_DEBUG, "[PN]\tApplying rule RB1 (elimination of identical places)...\n");

  // iterate the places
  for (set<Place*>::iterator p1 = P.begin(); p1 != P.end(); p1++)
  {
    set<Node*> preSet  = preset(*p1);
    set<Node*> postSet = postset(*p1);

    if ((preSet.empty()) || (postSet.empty()))
      continue;

    for (set<Node*>:: iterator preTransition = preSet.begin(); preTransition != preSet.end(); preTransition++)
    {
      set<Node*> pPostSet = postset(*preTransition);
      for (set<Node*>::iterator p2 = pPostSet.begin(); p2 != pPostSet.end(); p2++)
	if ((*p1 != *p2) &&		// precondition 1
	    (preSet == preset(*p2)) &&	// precondition 2
	    (postSet == postset(*p2)))	// precondition 3
	{
	  string id1 = *((*p1)->history.begin());
	  string id2 = *((*p2)->history.begin());
	  placePairs.insert(pair<string, string>(id1, id2));
	}
    }
  }
  
  trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(placePairs.size()) + " places with same preset and postset...\n");

  // merge the found transitions
  for (set<pair<string, string> >::iterator labels = placePairs.begin();
      labels != placePairs.end(); labels++)
  {
    Place* p1 = findPlace(labels->first);
    Place* p2 = findPlace(labels->second);
    mergePlaces(p1, p2);
  }
}





/*!
 * \brief Elimination of identical transitions (RB2):
 *
 * If there exist two distinct (precondition 1) transitions with identical
 * preset (precondition 2) and postset (precondition 3), then they can be merged.
 */
void PetriNet::elminiationOfIdenticalTransitions()
{
  set<pair<string, string> > transitionPairs;

  trace(TRACE_DEBUG, "[PN]\tApplying rule RB2 (elimination of identical transitions)...\n");

  // iterate the transitions
  for (set<Transition*>::iterator t1 = T.begin(); t1 != T.end(); t1++)
  {
    set<Node*> preSet  = preset(*t1);
    set<Node*> postSet = postset(*t1);

    for (set<Node*>:: iterator prePlace = preSet.begin(); prePlace != preSet.end(); prePlace++)
    {
      set<Node*> pPostSet = postset(*prePlace);
      for (set<Node*>::iterator t2 = pPostSet.begin(); t2 != pPostSet.end(); t2++)
	if ((*t1 != *t2) &&		// precondition 1
	    (preSet == preset(*t2)) &&	// precondition 2
	    (postSet == postset(*t2)))	// precondition 3
	{
	  string id1 = *((*t1)->history.begin());
	  string id2 = *((*t2)->history.begin());
	  transitionPairs.insert(pair<string, string>(id1, id2));
	}
    }
  }
  
  trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(transitionPairs.size()) + " transitions with same preset and postset...\n");

  // merge the found transitions
  for (set<pair<string, string> >::iterator labels = transitionPairs.begin();
      labels != transitionPairs.end(); labels++)
  {
    Transition* t1 = findTransition(labels->first);
    Transition* t2 = findTransition(labels->second);

    mergeTransitions(t1, t2);
  }
}





/*!
 * \brief Fusion of series places (RA1):
 *
 * If there exists a transition with singleton preset and postset
 * (precondition 1) that are distinct (precondition 2) and where the place in
 * the preset has no other outgoing arcs (precondition 3), then the places
 * can be merged and the transition can be removed. Furthermore, none of the
 * places may be communicating (precondition 4).
 */
void PetriNet::fusionOfSeriesPlaces()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA1 (fusion of series places)...\n");

  set<string> uselessTransitions;
  set<pair<string, string> > placePairs;


  // iterate the transtions
  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
  {
    set<Node*> postSet = postset(*t);
    set<Node*> preSet  = preset (*t);
    Place* prePlace = (Place*) *(preSet.begin());
    Place* postPlace = (Place*) *(postSet.begin());

    if ((preSet.size() == 1) &&	(postSet.size() == 1) && // precondition 1
	(prePlace != postPlace) &&			 // precondition 2
	(postset(prePlace).size() == 1) &&		 // precondition 3
	(prePlace->type == INTERNAL) &&			 // precondition 4
	(postPlace->type == INTERNAL))
   {
      string id1 = *((*(preSet.begin()))->history.begin());
      string id2 = *((*(postSet.begin()))->history.begin());
      placePairs.insert(pair<string, string>(id1, id2));
      uselessTransitions.insert(*((*t)->history.begin()));
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
    mergePlaces(placePair->first, placePair->second);
  }
}





/*!
 * \brief Fusion of series transition (RA2):
 *
 * If there exists a place with singleton preset and postset (precondition 1)
 * and if the transition in its postset has no other incoming arcs
 * (precondition 2), then the preset and the postset can be merged and the
 * place can be removed.
 */
void PetriNet::fusionOfSeriesTransitions()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA2 (fusion of series transitions)...\n");

  set<string> uselessPlaces;
  set<pair<string, string> > transitionPairs;


  // iterate the places
  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((postset(*p).size() == 1) && (preset(*p).size() == 1)) // precondition 1
    {
      Transition* t1 = (Transition*)(*(preset(*p).begin()));
      Transition* t2 = (Transition*)(*(postset(*p).begin()));

      if (preset(t2).size() == 1) // precondition 2
      {
	string id1 = *(t1->history.begin());
	string id2 = *(t2->history.begin());
	transitionPairs.insert(pair<string, string>(id1, id2));
	uselessPlaces.insert(*((*p)->history.begin()));
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
  }
}





/*!
 * Calls some simple structural reduction rules for Petri nets:
 *
 * - Structural dead nodes are removed.
 *
 * - If two transitions t1 and t2 have the same preset and postset, one of them
 *   can safely be removed.
 *
 * - Elimination of identical places (RB1)
 * - Elimination of identical transitions (RB2)
 * - Fusion of series places (RA1)
 * - Fusion of series transitions (RA2)
 * 
 * \todo
 *       -(nlohmann) improve performance
 *       -(nlohmann) implement more reduction rules
 *
 */
void PetriNet::simplify()
{
  trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: " + information() + "\n");
  trace(TRACE_INFORMATION, "Simplifying Petri net...\n");

  string old = information();
  bool done = false;
  bool treduced = false;
  int passes = 1;
  while (!done)
  {
    removeDeadNodes();
    removeUnusedStatusPlaces();

    elminiationOfIdenticalPlaces();		// RB1
    elminiationOfIdenticalTransitions();	// RB2
    fusionOfSeriesPlaces();			// RA1
    fusionOfSeriesTransitions();		// RA2

//    collapseSequences();

    trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification pass " + toString(passes++) + ": " + information() + "\n");

    done = (old == information());
    old = information();

    if (parameters[P_TRED] && !treduced)
    {
      treduced = true;
      transitiveReduction();
      done = false;
    }
  }

  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: " + information() + "\n");
}







/* another set union function */
set<unsigned int> setUnion(set<unsigned int> a, set<unsigned int> b)
{
  set<unsigned int> result;
  insert_iterator<set<unsigned int, less<unsigned int> > > res_ins(result, result.begin());
  set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);
  
  return result;
}

set<unsigned int> setDifference(set<unsigned int> a, set<unsigned int> b)
{
  set<unsigned int> resultSet (a);
  if (! (a.empty() || b.empty()))
    for (set<unsigned int>::iterator iter = b.begin(); iter != b.end(); iter++)
      resultSet.erase(*iter);

  return resultSet;
}

set<Node *> setIntersection(set<Node *> a, set<Node *> b)
{
  set<Node *> result;
  insert_iterator<set<Node *, less<Node *> > > res_ins(result, result.begin());
  set_intersection(a.begin(), a.end(), b.begin(), b.end(), res_ins);
  
  return result;
}





set<unsigned int> visited2;


/* depth-first search returning the set of reachable nodes */
set<unsigned int> dfs(unsigned int i, map<unsigned int, set<unsigned int> > &Adj)
{
  set<unsigned int> result;
  result.insert(i);
  visited2.insert(i);

  for (set<unsigned int>::iterator it = Adj[i].begin(); it != Adj[i].end(); it++)
  {
    if (visited2.find(*it) == visited2.end())
      result = setUnion(result, dfs(*it, Adj));
  }

  return result;
}


/* creates accessibility list from adjacency list */
map<unsigned int, set<unsigned int> > toAcc(map<unsigned int, set<unsigned int> > &Adj, set<unsigned int> &nodes)
{
  map<unsigned int, set<unsigned int> > result;

  for (set<unsigned int>::iterator it = nodes.begin(); it != nodes.end(); it++)
  {
    result[*it] = dfs(*it, Adj);
    result[*it].erase(*it);
    visited2.clear();
  }

  return result;
}


/* outputs a mapping */
void mapOutput(map<unsigned int, set<unsigned int> > &mapping, set<unsigned int> &nodes)
{
  for (set<unsigned int>::iterator it = nodes.begin(); it != nodes.end(); it++)
  {
    cerr << *it << "\t";

    for (set<unsigned int>::iterator it2 = mapping[*it].begin(); it2 != mapping[*it].end(); it2++)
      cerr << *it2 << " ";

    cerr << endl;
  }
}

set<unsigned int> visited;








void prune_acc(unsigned int i, map<unsigned int, set<unsigned int> > &Acc, map<unsigned int, set<unsigned int> > &Adj)
{
  for (set<unsigned int>::iterator it = Acc[i].begin(); it != Acc[i].end(); it++)
  {
    if (Acc[*it].empty())
      visited.insert(*it);
    else
      prune_acc(*it, Acc, Adj);
  }

  for (set<unsigned int>::iterator it = Acc[i].begin(); it != Acc[i].end(); it++)
    for (set<unsigned int>::iterator it2 = Adj[*it].begin(); it2 != Adj[*it].end(); it2++)
      if (Acc[i].find(*it2) != Acc[i].end())
	Adj[i].erase(*it2);

  visited.insert(i);
}






/* wrapper functions for the transitive reduction */
void PetriNet::transitiveReduction()
{
  trace(TRACE_DEBUG, "[PN]\tApplying transitive reduction...\n");
  
  set<unsigned int> nodes;
  map<unsigned int, set<unsigned int> > Adj;

  // generate the list of nodes
  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
    nodes.insert((*t)->id);

  // generate the adjacency lists
  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
  {
    set<Node*> pre = preset(*p);
    set<Node*> post = postset(*p);

    if (pre.size() > 0 && post.size() > 0)
      for (set<Node*>::iterator t1 = pre.begin(); t1 != pre.end(); t1++)
	for (set<Node*>::iterator t2 = post.begin(); t2 != post.end(); t2++)
	  Adj[(*t1)->id].insert((*t2)->id);
  }

  // generate the accessibility list
  map<unsigned int, set<unsigned int> > Acc = toAcc(Adj, nodes);

  // the accessibility is base for the transitive reduction
  map<unsigned int, set<unsigned int> > Adj_reduced = Acc;

  // iterate the nodes and reduce
  for (set<unsigned int>::iterator it = nodes.begin(); it != nodes.end(); it++)
  {
    if (visited.find(*it) == visited.end())
      prune_acc(*it, Acc, Adj_reduced);
  }

  visited.clear();

  set<Place*> transitivePlaces;

  for (set<unsigned int>::iterator it = nodes.begin(); it != nodes.end(); it++)
  {
    set<unsigned int> difference = setDifference(Adj[*it], Adj_reduced[*it]);
    for (set<unsigned int>::iterator it2 = difference.begin(); it2 != difference.end(); it2++)
      transitivePlaces.insert(findPlace(*it, *it2));
  }


  // remove transitive places
  for (set<Place*>::iterator p = transitivePlaces.begin(); p != transitivePlaces.end(); p++)
    removePlace(*p);

  cerr << "removed " << transitivePlaces.size() << " transitive places" << endl;
}





Place *PetriNet::findPlace(unsigned id1, unsigned id2)
{
  Transition *t1 = NULL;
  Transition *t2 = NULL;

  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
  {
    if ( (*t)->id == id1 )
      t1 = *t;
    if ( (*t)->id == id2 )
      t2 = *t;
  }

  assert(t1 != NULL);
  assert(t2 != NULL);
  assert(t1 != t2);

  set<Node*> temp = setIntersection(postset(t1), preset(t2));

  if (temp.size() > 1)
    cerr << "WARNING" << endl;

  Place *result = (Place*)(*(temp.begin()));

  assert(result != NULL);

  return result;
}
