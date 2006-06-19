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
 *  - PetriNet::simplify()
 *  - PetriNet::removeIsolatedNodes()
 *  - PetriNet::removeVariables()
 *
 * \author
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *
 * \date
 *          - created: 2006-03-16
 *          - last changed: \$Date: 2006/06/19 08:55:11 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.18 $
 */





#include <list>
#include "petrinet.h"
#include "debug.h"		// debugging help
#include "symbol-table.h"
#include "helpers.h"
#include "options.h"



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
  extern SymbolTable symTab;
  
  for (list<string>::iterator variable = symTab.variables.begin();
      variable != symTab.variables.end(); variable++)
  {
    removePlace( findPlace("variable." + *variable) );
  }

  removePlace( findPlace("1.internal.clock") );
}





/******************************************************************************
 * Functions to structurally simplify the Petri net model
 *****************************************************************************/


void PetriNet::removeUnusedStatusPlaces()
{
  list<Place*> unusedPlaces;

  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
  {
    if (postset(*p).empty() && (*p)->history[0] != "1.internal.final")
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
	trace(TRACE_VERY_DEBUG, "[PN]\tPlace p" + intToString((*p)->id) + " is structurally dead.\n");
	done = false;
      }
    }

    while (!tempPlaces.empty())
    {
      // p is a dead place
      Place* p = tempPlaces.back();
      tempPlaces.pop_back();
      set<Node*> ps = postset(p);

      // transitions in the postset of a dead place are dead
      for (set<Node*>::iterator t = ps.begin(); t != ps.end(); t++)
      {
      	deadTransitions.push_back( (Transition*)(*t) );
	trace(TRACE_VERY_DEBUG, "[PN]\tTransition t" + intToString((*t)->id) + " is structurally dead\n");
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

  trace(TRACE_VERY_DEBUG, "[PN]\tSearching for places with same preset and postset...\n");

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
  
  trace(TRACE_VERY_DEBUG, "[PN]\tFound " + intToString(placePairs.size()) + " places with same preset and postset...\n");

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

  trace(TRACE_VERY_DEBUG, "[PN]\tSearching for transitions with same preset and postset...\n");

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
  
  trace(TRACE_VERY_DEBUG, "[PN]\tFound " + intToString(transitionPairs.size()) + " transitions with same preset and postset...\n");

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
 * can be merged and the transition can be removed.
 */
void PetriNet::fusionOfSeriesPlaces()
{
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
	(postset(prePlace).size() == 1) )		 // precondition 3
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
    Place* uselessPlace = findPlace(*label);
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












/*-----------------------------------------------------------------------------
 * old structural reduction rules
 *---------------------------------------------------------------------------*/

/*!
 * Returns true if there is a communicating transition in the postset of the
 * given place p.
 *
 * \param  p a place to check
 * \return true, if a communicating transition was found
 */
bool PetriNet::communicationInPostSet(Place *p)
{
  set<Node*> pp = postset(p);
  for (set<Node*>::iterator t = pp.begin();
      t != pp.end();
      t++)
  {
    if (((Transition*)(*t))->type != INTERNAL)
      return true;
  }
  
  return false;
}


/*!
 * Collapse simple sequences.
 *
 * A simple sequence is a transition with
 *  - singleton preset
 *  - singleton postset
 *  - no communicating transition following
 *  - preset != postset
 */
void PetriNet::collapseSequences()
{
  trace(TRACE_VERY_DEBUG, "[PN]\tCollapsing simple sequences\n");

  // a pair to store places to be merged
  vector<string> sequenceTransitions;
  vector<pair<string, string> >placeMerge;

  // find transitions with singelton preset and postset
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
  {
    if (
	(preset(*t).size() == 1) &&
	(postset(*t).size() == 1) &&
	!communicationInPostSet((Place*)*(postset(*t).begin())) &&
	(*(postset(*t).begin()) != (*preset(*t).begin()))
       )
    {
      string id1 = *((*(preset(*t).begin()))->history.begin());
      string id2 = *((*(postset(*t).begin()))->history.begin());
      placeMerge.push_back(pair < string, string >(id1, id2));
      sequenceTransitions.push_back(*((*t)->history.begin()));
    }
  }

  // merge preset and postset
  for (unsigned int i = 0; i < placeMerge.size(); i++)
    mergePlaces(placeMerge[i].first, placeMerge[i].second);

  // remove "sequence"-transtions
  for (unsigned int i = 0; i < sequenceTransitions.size(); i++)
  {
    Transition *sequenceTransition = findTransition(sequenceTransitions[i]);
    if (sequenceTransition != NULL)
      removeTransition(sequenceTransition);
  }  
}



/*!
 * Merges twin transitions.
 */
void PetriNet::mergeTwinTransitions()
{
  // a pair to store transitions to be merged
  vector<pair<string, string> > transitionPairs;

  trace(TRACE_VERY_DEBUG, "[PN]\tSearching for transitions with same preset and postset...\n");
  // find transitions with same preset and postset
  for (set<Transition *>::iterator t1 = T.begin(); t1 != T.end(); t1++)
    for (set<Transition *>::iterator t2 = t1; t2 != T.end(); t2++)
      if (*t1 != *t2)
	if ((preset(*t1) == preset(*t2)) && (postset(*t1) == postset(*t2)))
	  transitionPairs.push_back(pair<string, string>(*((*t1)->history.begin()), *((*t2)->history.begin())));

  trace(TRACE_VERY_DEBUG, "[PN]\tFound " + intToString(transitionPairs.size()) + " transitions with same preset and postset...\n");

  // merge the found transitions
  for (unsigned int i = 0; i < transitionPairs.size(); i++)
  {
    Transition *t1 = findTransition(transitionPairs[i].first);
    Transition *t2 = findTransition(transitionPairs[i].second);

    if ((t1 != NULL) && (t2 != NULL) && (t1 != t2))
      mergeTransitions(t1, t2);
  }
}

/*---------------------------------------------------------------------------*/










/*!
 * Calls some simple structural reduction rules for Petri nets:
 *
 * - Structural dead nodes are removed.
 *
 * - If two transitions t1 and t2 have the same preset and postset, one of them
 *   can safely be removed.
 *
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
  int passes = 1;
  while (!done)
  {
    removeDeadNodes();

    removeUnusedStatusPlaces();

    elminiationOfIdenticalPlaces();		// RB1
    elminiationOfIdenticalTransitions();	// RB2
    fusionOfSeriesPlaces();			// RA1
    fusionOfSeriesTransitions();		// RA2

/*    mergeTwinTransitions();
    collapseSequences();*/

    trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification pass " + intToString(passes++) + ": " + information() + "\n");

    done = (old == information());
    old = information();
  }

  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: " + information() + "\n");
}
