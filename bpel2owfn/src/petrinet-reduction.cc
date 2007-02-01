/*****************************************************************************\
 * Copyright 2006 Niels Lohmann                                              *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    petrinet-reduction.cc
 *
 * \brief   Petri Net API: structural reduction
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *
 * \since   2006-03-16
 *
 * \date    \$Date: 2007/02/01 14:14:20 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.55 $
 *
 * \ingroup petrinet
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <list>
#include <cassert>
#include <set>
#include <utility>

#include "petrinet.h"
#include "debug.h"		// debugging help
#include "helpers.h"
#include "options.h"

using std::pair;
using std::cerr;
using std::endl;





/******************************************************************************
 * Global variables
 *****************************************************************************/

//set<unsigned int> visited;    // used for transitive reduction
//set<unsigned int> visited2;   // used for transitive reduction





/******************************************************************************
 * Functions to structurally simplify the Petri net model
 *****************************************************************************/

/*!
 * Remove status places that are not read by any transition. These places are
 * usually the result of reduction rules applied before. The places are not
 * "read" by any transition, thus can be removed if the are not covered by a
 * final marking.
 *
 * \note This rule is BPEL2oWFN specific.
 *
 * \return number of removed places
 *
 * \pre \f$p\f$ is an internal place: \f$ p \in P \f$
 * \pre \f$p\f$'s postset is empty: \f$ p^\bullet = \emptyset \f$
 * \pre \f$p\f$ is not covered by a final marking: \f$ m(p) = 0 \f$ for all \f$ m \in \Omega \f$
 *
 * \post \f$p\f$ is removed: \f$ P' = P \; \backslash \; \{ p \} \f$
 */
unsigned int PetriNet::reduce_unused_status_places()
{
  list<Place *> unused_status_places;
  unsigned int result = 0;

  // find unused status places
  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
    if ( postset(*p).empty() )
      if ( !( (*p)->isFinal ) )
  	unused_status_places.push_back(*p);

  // remove unused places
  for (list<Place*>::iterator p = unused_status_places.begin(); p != unused_status_places.end(); p++)
    if (P.find(*p) != P.end())
    {
      removePlace(*p);
      result++;
    }

  return result;
}





/*!
 * Remove transitions with empty pre or postset. These transitions occur mostly
 * atfer removing unused status places.
 *
 * \note This rule is BPEL2oWFN specific.
 *
 * \return number of removed transitions
 *
 * \pre \f$t\f$ is a transition of the net: \f$ t \in T \f$
 * \pre \f$t\f$'s preset or postset empty: \f$ {}^\bullet t = \emptyset \f$ or \f$ t^\bullet = \emptyset \f$
 *
 * \post \f$t\f$ is removed: \f$ T' = T \; \backslash \; \{ t \} \f$
 */
unsigned int PetriNet::reduce_suspicious_transitions()
{
  list<Transition*> suspiciousTransitions;
  unsigned int result = 0;

  // find suspicious transitions
  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
    if (postset(*t).empty() || preset(*t).empty())
      suspiciousTransitions.push_back(*t);

  // remove suspicious transitions
  for (list<Transition*>::iterator t = suspiciousTransitions.begin(); t != suspiciousTransitions.end(); t++)
    if (T.find(*t) != T.end())
    {
      removeTransition(*t);
      result++;
    }

  return result;
}





/*!
 * Remove structural dead nodes.
 *
 * \todo Re-organize the storing and removing of nodes.
 */
void PetriNet::reduce_dead_nodes()
{
  trace(TRACE_DEBUG, "[PN]\tRemoving structurally dead nodes...\n");

  bool done = false;
  bool arcs = true;

  while (!done)
  {
    done = true;

    list<Place*> deadPlaces;
    list<Transition*> deadTransitions;
    list<Place*> tempPlaces;

    // find insufficiently marked places with empty preset
    for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
    {
      if (preset(*p).empty()) //&& (*p)->tokens == 0)
      {
			arcs=true;			
			for(set<Node*>::iterator t = postset(*p).begin(); t != postset(*p).end(); t++)
			{
				if(arc_weight(*p,*t) <= (*p)->tokens)	
				{
					arcs=false;
				}
			}
			if(arcs)
			{
				deadPlaces.push_back(*p);
				tempPlaces.push_back(*p);
				trace(TRACE_VERY_DEBUG, "[PN]\tPlace p" + toString((*p)->id) + " is structurally dead.\n");
				done = false;
			}
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
      	deadTransitions.push_back( static_cast<Transition*>(*t) );
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
    list<Place*> uselessInputPlaces;

    for (set<Place*>::iterator p = P_in.begin(); p != P_in.end(); p++)
      if (postset(*p).empty())
	uselessInputPlaces.push_back(*p);

    for (list<Place*>::iterator p = uselessInputPlaces.begin(); p != uselessInputPlaces.end(); p++)
      if (P_in.find(*p) != P_in.end())
	P_in.erase(*p);

    list<Place*> uselessOutputPlaces;

    for (set<Place*>::iterator p = P_out.begin(); p != P_out.end(); p++)
      if (preset(*p).empty())
	uselessOutputPlaces.push_back(*p);

    for (list<Place*>::iterator p = uselessOutputPlaces.begin(); p != uselessOutputPlaces.end(); p++)
      if (P_out.find(*p) != P_out.end())
	P_out.erase(*p);
  }
}





/*!
 * \brief Elimination of identical places (RB1):
 *
 * If there exist two distinct (precondition 1) places with identical preset
 * (precondition 2) and postset (precondition 3) and the weights of all incoming
 * and outgoing arcs have the same value (precondition 4), then they can be merged.
 *
 * \image html rb1.png
 *
 * \todo Overwork the preconditions and postconditions.
 * \todo Re-organize the storing and removing of nodes.
 */
void PetriNet::reduce_identical_places()
{
  set<pair<string, string> > placePairs;

  trace(TRACE_DEBUG, "[PN]\tApplying rule RB1 (elimination of identical places)...\n");

  // iterate the places
  for (set<Place*>::iterator p1 = P.begin(); p1 != P.end(); p1++)
  {
    set<Node*> preSet  = preset(*p1);
    set<Node*> postSet = postset(*p1);

    if ((preSet.empty()) || (postSet.empty()) || !(sameweights(*p1)))
      continue;

    for (set<Node*>:: iterator preTransition = preSet.begin(); preTransition != preSet.end(); preTransition++)
    {
      set<Node*> pPostSet = postset(*preTransition);
      for (set<Node*>::iterator p2 = pPostSet.begin(); p2 != pPostSet.end(); p2++)
	 if ((*p1 != *p2) &&		// precondition 1
	    (preSet == preset(*p2)) &&	// precondition 2
	    (postSet == postset(*p2)) && // precondition 3
	    (sameweights(*p2)) && // precondition 4
	    (arc_weight((*preTransition), (*p1)) == arc_weight((*p2), (*(postSet.begin())))) ) // precondition 4
	{
	  string id1 = *((*p1)->history.begin());
	  string id2 = *((*p2)->history.begin());
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
    mergePlaces(p1, p2);
  }
}





/*!
 * \brief Elimination of identical transitions (RB2):
 *
 * If there exist two distinct (precondition 1) transitions with identical
 * preset (precondition 2) and postset (precondition 3) and none of those two
 * is connected to any arc with a weight other than 1 (precondition 4),
 * then they can be merged.
 *
 * \image html ra2.png
 *
 * \todo Overwork the preconditions and postconditions.
 * \todo Re-organize the storing and removing of nodes.
 */
void PetriNet::reduce_identical_transitions()
{
  set<pair<string, string> > transitionPairs;

  trace(TRACE_DEBUG, "[PN]\tApplying rule RB2 (elimination of identical transitions)...\n");

  // iterate the transitions
  for (set<Transition*>::iterator t1 = T.begin(); t1 != T.end(); t1++)
  {
    set<Node*> preSet  = preset(*t1);
    set<Node*> postSet = postset(*t1);

    if (!(sameweights(*t1)))
      continue;

    for (set<Node*>:: iterator prePlace = preSet.begin(); prePlace != preSet.end(); prePlace++)
    {
      set<Node*> pPostSet = postset(*prePlace);
      for (set<Node*>::iterator t2 = pPostSet.begin(); t2 != pPostSet.end(); t2++)
	if ((*t1 != *t2) &&		// precondition 1
	    (preSet == preset(*t2)) &&	// precondition 2
	    (postSet == postset(*t2)) && // precondition 3
	    (sameweights(*t2)) && // precondition 4
	    (arc_weight((*(preSet.begin())),(*t1)) == arc_weight((*t2), (*(postSet.begin()))))) // precondition 4
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
 * places may be communicating (precondition 4) and the included arcs must have
 * a weight of 1 (precondition 5).
 *
 * \image html ra1.png
 *
 * \todo Overwork the preconditions and postconditions.
 * \todo Re-organize the storing and removing of nodes.
*/
void PetriNet::reduce_series_places()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA1 (fusion of series places)...\n");

  set<string> uselessTransitions;
  set<pair<string, string> > placePairs;


  // iterate the transtions
  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
  {
    set<Node*> postSet = postset(*t);
    set<Node*> preSet  = preset (*t);
    Place* prePlace = static_cast<Place*>(*(preSet.begin()));
    Place* postPlace = static_cast<Place*>(*(postSet.begin()));

    if ((preSet.size() == 1) &&	(postSet.size() == 1) && // precondition 1
	(prePlace != postPlace) &&			 // precondition 2
	(postset(prePlace).size() == 1) &&		 // precondition 3
	(prePlace->type == INTERNAL) &&			 // precondition 4
	(postPlace->type == INTERNAL) &&
	(arc_weight(prePlace, *t) == 1 && arc_weight(*t, postPlace) == 1)) // precondition 5
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
 * place can be removed. Furthermore the in and outgoing arcs have to have
 * the same weight. (precondition 3).
 *
 * \image html ra2.png
 *
 * \todo Overwork the preconditions and postconditions.
 * \todo Re-organize the storing and removing of nodes.
 */
void PetriNet::reduce_series_transitions()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA2 (fusion of series transitions)...\n");

  set<string> uselessPlaces;
  set<pair<string, string> > transitionPairs;


  // iterate the places
  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
  {
    if ((postset(*p).size() == 1) && (preset(*p).size() == 1)) // precondition 1
    {
      Transition* t1 = static_cast<Transition*>(*(preset(*p).begin()));
      Transition* t2 = static_cast<Transition*>(*(postset(*p).begin()));

      if ((preset(t2).size() == 1) && // precondition 2
          (arc_weight(t1, *p) == arc_weight(*p, t2))) // precondition 5
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
 * Elimination of self-loop places (ESP) as depicted in [Murrata89]. The rule
 * preserves liveness, safeness and boundedness.
 *
 * \note As \f$ p \f$ has both ingoing and outgoing arcs, \f$ p \f$ is an
 *       internal place. Thus, removing this place preserves controllability
 *       and all communicating partners if \f$ p \f$ is not covered by a final
 *       marking.
 *
 * \return number of removed places
 *
 * \pre \f$ p \f$ is a place of the net: \f$ p \in P \f$
 * \pre \f$ p \f$ is initially marked: \f$ m_0(p) > 0 \f$
 * \pre \f$ p \f$ has one transition in its preset and one transition in its postset: \f$ |{}^\bullet p| = 1 \f$, \f$ |p^\bullet| = 1 \f$
 * \pre \f$ p \f$'s preset and postset are equal: \f$ p^\bullet = {}^\bullet p \f$
 *
 * \post \f$ p \f$ is removed: \f$ P' = P \; \backslash \; \{p\} \f$
 *
 * \todo remove debug output
 */
unsigned int PetriNet::reduce_self_loop_places()
{
  list<Place *> self_loop_places;
  unsigned int result = 0;

  // find places fulfilling the preconditions
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    if ((*p)->tokens > 0)
      if (postset(*p).size() == 1 && preset(*p).size() == 1)
	if (preset(*p) == postset(*p))
	  self_loop_places.push_back(*p);

  // remove useless places
  for (list<Place *>::iterator p = self_loop_places.begin(); p != self_loop_places.end(); p++)
    if (P.find(*p) != P.end())
    {
      removePlace(*p);
      result++;
    }

  if (result > 0)
    cerr << "removed " << result << " self-loop places" << endl;

  return result;
}





/*!
 * Elimination of self-loop transitions (EST) as depicted in [Murrata89]. The
 * rule preserves liveness, safeness and boundedness.
 *
 * \note As the place in \f$ t \f$'s preset/postset has ingoing and outgoing
 *       arcs, it is an internal place. Thus, \f$ t \f$ does not communicate
 *       and its removal does not affect controllability or the set of
 *       communicating partners.
 *
 * \return number of removed transitions
 *
 * \pre \f$ t \f$ is a transition of the net: \f$ t \in T \f$
 * \pre \f$ t \f$ has one place in its preset and one place in its postset: \f$ |{}^\bullet t| = 1 \f$, \f$ |t^\bullet| = 1 \f$
 * \pre \f$ t \f$'s preset and postset are equal: \f$ t^\bullet = {}^\bullet t \f$
 *
 * \post \f$ t \f$ is removed: \f$ T' = T \; \backslash \; \{t\} \f$
 *
 * \todo remove debug output
 */
unsigned int PetriNet::reduce_self_loop_transitions()
{
  list<Transition *> self_loop_transitions;
  unsigned int result = 0;

  // find transitions fulfilling the preconditions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    if (postset(*t).size() == 1 && preset(*t).size() == 1)
      if (preset(*t) == postset(*t))
	self_loop_transitions.push_back(*t);

  // remove useless transitions
  for (list<Transition *>::iterator t = self_loop_transitions.begin(); t != self_loop_transitions.end(); t++)
    if (T.find(*t) != T.end())
    {
      removeTransition(*t);
      result++;
    }

  if (result > 0)
    cerr << "removed " << result << " self-loop transitions" << endl;

  return result;
}


/*!
 * \brief Elimination of equal places (RD1):
 *
 * If there exist two distinct (precondition 1) places with only one (precondition 2) 
 * outgoing arc with a weight of 1 (precondistion 3) each leading to two distinct 
 * (precondition 4) Transitions t1 and t2, which have identical presets and postsets excluding
 * p1 and p2 (precondition 5) then p2 and t2 can be removed by directing all arcs that 
 * once led to p2 to p1.
 *
 */
void PetriNet::reduce_equal_places()
{

  trace(TRACE_DEBUG, "[PN]\tApplying rule ST4 (elimination of equal places)...\n");
  set<pair<string, string> > placePairs;

  // iterate the places
  for (set<Place*>::iterator p1 = P.begin(); p1 != P.end(); p1++)
  {
    set<Node*> preSet1  = preset(*p1);
    set<Node*> postSet1 = postset(*p1);

    if (postSet1.size() !=1)
      continue;

    if (arc_weight(*p1,*postSet1.begin())!=1)
      continue;
	 
	 Node* t1= *postSet1.begin();

    for (set<Place*>::iterator p2 = P.begin(); p2 != P.end(); p2++)
    {
      if(*p1 == *p2)
		  continue;
  
      set<Node*> preSet2  = preset(*p2);
      set<Node*> postSet2 = postset(*p2);

		 if (postSet2.size() !=1)
        continue;

      if (arc_weight(*p2,*postSet2.begin()) !=1)
        continue;

   	 Node* t2 = *postSet2.begin();

      if (t1 == t2)
        continue;

		 set<Node*> postSetT1 = postset (t1);
		 set<Node*> postSetT2 = postset (t2);

		 if (postSetT1 != postSet2)
			continue;

		 set<Node*> preSetT1 = preset (t1);
		 set<Node*> preSetT2 = preset (t2);

		 preSetT1.erase(*p1);
		 preSetT2.erase(*p2);

		 if (preSetT1 != preSetT2)
		   continue;

	    string id1 = *((*p1)->history.begin());
	    string id2 = *((*p2)->history.begin());
	    placePairs.insert(pair<string, string>(id1, id2));
	 }
  }


  // Do the reduction
  for (set<pair<string, string> >::iterator labels = placePairs.begin();
      labels != placePairs.end(); labels++)
  {
    Place* p1 = findPlace(labels->first);
    Place* p2 = findPlace(labels->second);

    set<Node*> preSet1  = preset(p1);
    set<Node*> preSet2  = preset(p2);
    set<Node*> postSet2  = postset(p2);
    string trans_id = *((*postSet2.begin())->history.begin());

	 unsigned int arcadd = 0;

    for (set<Node*>::iterator n = preSet2.begin(); n != preSet2.end(); n++)
	 {
		arcadd=0;
	   if(preSet1.find(*n) != preSet1.end())
		{
			arcadd = arcadd + arc_weight(*n, p1);
        for (set<Arc*>::iterator f = F.begin(); f != F.end(); f++)
          if (((*f)->source == *n) || ((*f)->target == p1))
				 removeArc(*f);
		}
		newArc(*n, p1, STANDARD, arcadd);
	 }
	 p1->tokens = p1->tokens + p2->tokens;
	
	 removePlace(p2);
	 removeTransition(findTransition(trans_id));    
  }
}



/*!
 * Removes unnecessary nodes of the generated Petri net:
 *
 *  - structural dead nodes
 *  - unused status places
 *  - suspicious transitions
 *
 * Applys some simple structural reduction rules for Petri nets:
 *
 *  - elimination of identical places (RB1)
 *  - elimination of identical transitions (RB2)
 *  - fusion of series places (RA1)
 *  - fusion of series transitions (RA2)
 *  - elimination of self-loop places (RC1)
 *  - elimination of self-loop transitions (RC2)
 *  - elimination of self-loop transitions (RD1)
 *
 * The rules are applied until a fixed point (i.e. the number of places,
 * transitions and arcs does not change) is reached.
 *
 * \return the number of passes until a fixed point was reached
 *
 * \todo improve performance
 * \todo implement more reduction rules
 * \todo pass a parameter to this function to choose the property of the model to
 *       be preserved
 * \todo adjust rules to allow for arc weights
 */
unsigned int PetriNet::reduce()
{
  trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: " + information() + "\n");
  trace(TRACE_INFORMATION, "Simplifying Petri net...\n");

  string old = information();
  bool done = false;
  int passes = 1;

  while (!done)
  {
    reduce_dead_nodes();
    reduce_unused_status_places();
    reduce_suspicious_transitions();
    reduce_identical_places();		// RB1
    reduce_identical_transitions();	// RB2
    reduce_series_places();		// RA1
    reduce_series_transitions();	// RA2
    reduce_self_loop_places();		// RC1
    reduce_self_loop_transitions();	// RC2
    reduce_equal_places();				//RD1
//    if (parameters[P_TRED])
//      reduce_transitive_places();

    trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification pass " + toString(passes++) + ": " + information() + "\n");

    done = (old == information());
    old = information();
  }

  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: " + information() + "\n");

  return passes;
}





/*!
 * Removes transitive places, i.e. places that are marked iff another subnet is
 * marked. Transitivity means that there exist at least two paths in the net
 * from p's preset to p's postset, and p itself is on the shorter path.
 * Therefore p is marked while the net fires that longer path, thus p is marked
 * iff the other path is marked. Thus, p can be removed, because the transition
 * t2 in its postset can only fire if the tokens from the longer reach its
 * preset.
 *
 * \note This is an experimental rule.
 *
 * \returns number of removed places
 *
 * \pre p is a place of the net
 * \pre p incoming and outcoming arcs have the same weights
 * \pre p is not marked initially
 * \pre p has exactly one transition (t1) in its preset and exactly one
 *      transition (t2) in its postset
 * \pre t1 and t2 are distinct transitions
 * \pre there exists a path from t1 to t2 such that p is not part of it
 *
 * \post p is removed
 *
 * \todo remove debug output
 * \todo overwork the preconditions and postconditions
 */
unsigned int PetriNet::reduce_transitive_places()
{
  list<Place *> transitive_places;
  unsigned int result = 0;

  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    if (sameweights(*p))
      if ((*p)->tokens == 0)
        if (postset(*p).size() == 1 && preset(*p).size() == 1)
	if (preset(*p) != postset(*p))
	{
	  Transition *t1 = static_cast<Transition *>(*(preset(*p).begin()));
	  Node *t2 = *(postset(*p).begin());

	  set<Node *> postset_nodes = postset(t1);
	  postset_nodes.erase(*p);
	  bool done = false;

	  do
	  {
	    // collect the nodes reachable from the current set of nodes
	    unsigned int old_size = postset_nodes.size();
	    set<Node *> new_nodes;

	    for (set<Node *>::iterator n = postset_nodes.begin(); n != postset_nodes.end(); n++)
	      new_nodes = setUnion(new_nodes, postset(*n));

	    postset_nodes = setUnion(postset_nodes, new_nodes);

	    // a fixed point is reached -- abort
	    if (postset_nodes.size() == old_size)
	      done = true;

	    // t2 is reached: p can be removed -- abort
	    if (postset_nodes.find(t2) != postset_nodes.end())
	    {
	      done = true;
	      transitive_places.push_back(*p);
	    }
	  } while (!done);
	}


  // remove transitive places
  for (list<Place *>::iterator p = transitive_places.begin(); p != transitive_places.end(); p++)
    if (P.find(*p) != P.end())
    {
      removePlace(*p);
      result++;
    }

  if (result > 0)
    cerr << "removed " << result << " transitive places" << endl;

  return result;
}






/******************************************************************************
 * TRANSITIVE REDUCTION (alpha state)
 *****************************************************************************/

/*
// depth-first search returning the set of reachable nodes
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





// creates accessibility list from adjacency list
map<unsigned int, set<unsigned int> > toAcc(map<unsigned int, set<unsigned int> > &Adj, set<unsigned int> &nodes)
{
  map<unsigned int, set<unsigned int> > result;

  for (set<unsigned int>::iterator it = nodes.begin(); it != nodes.end(); it++)
  {
    result[*it] = dfs(*it, Adj);
    result[*it].erase(*it);
    visited2.clear();
  }

  trace(TRACE_VERY_DEBUG, "[PN]\tDFS complete.\n");

  return result;
}





void prune_acc(unsigned int i, map<unsigned int, set<unsigned int> > &Acc, map<unsigned int, set<unsigned int> > &Adj)
{
  trace(TRACE_VERY_DEBUG, "[PN]\tCalling prune_acc for t" + toString(i) + "\n");

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





// wrapper functions for the transitive reduction
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

  int actualDeleted = 0;

  // remove transitive places
  for (set<Place*>::iterator p = transitivePlaces.begin(); p != transitivePlaces.end(); p++)
  {
    // only remove places that are not conflicting
    if ( postset(*p).size() == 1 )
    {
      actualDeleted++;
      removePlace(*p);
    }
  }

  trace(TRACE_DEBUG, "[PN]\tRemoved " + toString(actualDeleted) + " transitive places\n");
}
*/
