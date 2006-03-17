/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or(at your    *
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
 *          - last changed: \$Date: 2006/03/17 10:24:48 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.2 $
 */





#include "petrinet.h"





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

  list<Place *> killList;

  for (set<Place *>::iterator p = P_in.begin(); p != P_in.end(); p++)
    killList.push_back(*p);

  for (set<Place *>::iterator p = P_out.begin(); p != P_out.end(); p++)
    killList.push_back(*p);

  for (list<Place *>::iterator it = killList.begin(); it != killList.end(); it++)
    removePlace(*it);

  hasNoInterface = true;
}





/*!
 * Remove the places modelling variables.
 */
void PetriNet::removeVariables()
{
  extern SymbolManager symMan;
  
  for (list<string>::iterator variable = symMan.variables.begin();
      variable != symMan.variables.end(); variable++)
  {
    removePlace( findPlace("variable." + *variable) );
  }
}





/******************************************************************************
 * Functions to structurally simplify the Petri net model
 *****************************************************************************/


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





/*!
 * Collapse simple sequences.
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
    if (preset(*t).size() == 1 && postset(*t).size() == 1)
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
 * Implements some simple structural reduction rules for Petri nets:
 *
 * - Structural dead nodes are removed.
 * - If two transitions t1 and t2 have the same preset and postset, one of them
 *   can safely be removed.
 * - If a transition has a singleton preset and postset, the transition can be
 *   removed(sequence) and the preset and postset can be merged.
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


  removeDeadNodes();
  mergeTwinTransitions();
  collapseSequences();

  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: " + information() + "\n");
}
