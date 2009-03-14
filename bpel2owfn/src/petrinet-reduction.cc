/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

/*!
 * \file    petrinet-reduction.cc
 *
 * \brief   Petri Net API: structural reduction
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *
 * \since   2006-03-16
 *
 * \date    \$Date: 2007/07/13 12:50:48 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.81 $
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
#include "helpers.h"		// helper functions (toString, setUnion)
#include "debug.h"		// (trace)

#ifdef HAVE_LIBLPSOLVE55
#include "lpsolve/lp_lib.h"
#endif

using std::list;
using std::pair;
using std::cerr;
using std::endl;
using namespace PNapi;





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
  trace(TRACE_DEBUG, "[PN]\tReducing unused places...\n");
  list<Place *> unused_status_places;
  unsigned int result = 0;
  
  // find unused status places
  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
    if ( (*p)->postset.empty() )
      if ( !( (*p)->isFinal ) )
        if ( (*p)->tokens == 0 )
          unused_status_places.push_back(*p);
  
  // remove unused places
  for (list<Place*>::iterator p = unused_status_places.begin(); p != unused_status_places.end(); p++)
    if (P.find(*p) != P.end())
    {
      removePlace(*p);
      result++;
    }
      if (result!=0)
        trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
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
  trace(TRACE_DEBUG, "[PN]\tReducing suspicious transitions...\n");
  list<Transition*> suspiciousTransitions;
  unsigned int result = 0;
  
  // find suspicious transitions
  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
    if ((*t)->postset.empty() || (*t)->preset.empty())
      suspiciousTransitions.push_back(*t);
  
  // remove suspicious transitions
  for (list<Transition*>::iterator t = suspiciousTransitions.begin(); t != suspiciousTransitions.end(); t++)
    if (T.find(*t) != T.end())
    {
      removeTransition(*t);
      result++;
    }
      
      if (result!=0)
        trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
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
  int result = 0;
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
      if ((*p)->preset.empty() && !((*p)->isFinal) && !((*p)->historyContains("1.internal.initial")))
      {
        arcs=true;		
        for(set<Node*>::iterator t = (*p)->postset.begin(); t != (*p)->postset.end(); t++)
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
      
      // transitions in the postset of a dead place are dead
      for (set<Node*>::iterator t = p->postset.begin(); t != p->postset.end(); t++)
      {
      	deadTransitions.push_back( static_cast<Transition*>(*t) );
        trace(TRACE_VERY_DEBUG, "[PN]\tTransition t" + toString((*t)->id) + " is structurally dead\n");
        done = false;
      }
    }
    
    
    // remove dead places and transitions
    for (list<Place*>::iterator p = deadPlaces.begin(); p != deadPlaces.end(); p++)
      if (P.find(*p) != P.end())
      {
        removePlace(*p);
        result++;
      }
        
        for (list<Transition*>::iterator t = deadTransitions.begin(); t != deadTransitions.end(); t++)
          if (T. find(*t) != T.end())
          {
            removeTransition(*t);
            result++;
          }
            
            
            // remove isolated communication places
            list<Place*> uselessInputPlaces;
    
    for (set<Place*>::iterator p = P_in.begin(); p != P_in.end(); p++)
      if ((*p)->postset.empty())
        uselessInputPlaces.push_back(*p);
    
    for (list<Place*>::iterator p = uselessInputPlaces.begin(); p != uselessInputPlaces.end(); p++)
      if (P_in.find(*p) != P_in.end())
      {
        P_in.erase(*p);
        result++;
      }
        
        list<Place*> uselessOutputPlaces;
    
    for (set<Place*>::iterator p = P_out.begin(); p != P_out.end(); p++)
      if ((*p)->preset.empty())
        uselessOutputPlaces.push_back(*p);
    
    for (list<Place*>::iterator p = uselessOutputPlaces.begin(); p != uselessOutputPlaces.end(); p++)
      if (P_out.find(*p) != P_out.end())
      {
        P_out.erase(*p);
        result++;
      }
  }
    if (result!=0)
      trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " nodes.\n");
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
void PetriNet::reduce_identical_places()
{
  int result=0;
  set<pair<string, string> > placePairs;
  
  trace(TRACE_DEBUG, "[PN]\tApplying rule RB1 (elimination of identical places)...\n");
  
  // iterate the places
  for (set<Place*>::iterator p1 = P.begin(); p1 != P.end(); p1++)
  {
    if (((*p1)->preset.empty()) || ((*p1)->postset.empty()) || !(sameweights(*p1)))
      continue;
    
    for (set<Node*>:: iterator preTransition = (*p1)->preset.begin(); preTransition != (*p1)->preset.end(); preTransition++)
    {
      for (set<Node*>::iterator p2 = (*preTransition)->postset.begin(); p2 != (*preTransition)->postset.end(); p2++)
        if ((*p1 != *p2) &&		// precondition 1
            ((*p1)->preset == (*p2)->preset) &&	// precondition 2
            ((*p1)->postset == (*p2)->postset) && // precondition 3
            (sameweights(*p2)) && // precondition 4
            (arc_weight((*preTransition), (*p1)) == arc_weight((*p2), (*((*p1)->postset.begin())))) ) // precondition 4
        {
          string id1 = ((*p1)->nodeFullName());
          string id2 = ((*p2)->nodeFullName());
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
      result++;
    }
    if (result!=0)
      trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
}





/*!
 * \brief Elimination of identical transitions (RB2):
 *
 * If there exist two distinct (precondition 1) transitions with identical
 * preset (precondition 2) and postset (precondition 3) and none of those two
 * is connected to any arc with a weight other than 1 (precondition 4),
 * then they can be merged.
 *
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
 */
void PetriNet::reduce_identical_transitions()
{
  int result=0;
  set<pair<string, string> > transitionPairs;
  
  trace(TRACE_DEBUG, "[PN]\tApplying rule RB2 (elimination of identical transitions)...\n");
  
  // iterate the transitions
  for (set<Transition*>::iterator t1 = T.begin(); t1 != T.end(); t1++)
  {
    if (!(sameweights(*t1)))
      continue;
    
    for (set<Node*>:: iterator prePlace = (*t1)->preset.begin(); prePlace != (*t1)->preset.end(); prePlace++)
    {
      for (set<Node*>::iterator t2 = (*prePlace)->postset.begin(); t2 != (*prePlace)->postset.end(); t2++)
        if ((*t1 != *t2) &&		// precondition 1
            ((*t1)->preset == (*t2)->preset) &&	// precondition 2
            ((*t1)->postset == (*t2)->postset) && // precondition 3
            (sameweights(*t2)) && // precondition 4
            (arc_weight((*((*t1)->preset.begin())),(*t1)) == arc_weight((*t2), (*((*t1)->postset.begin()))))) // precondition 4
        {
          string id1 = ((*t1)->nodeFullName());
          string id2 = ((*t2)->nodeFullName());
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
      
      mergeParallelTransitions(t1, t2);
      result++;
    }
    if (result!=0)
      trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
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
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
*/
void PetriNet::reduce_series_places()
{
  int result=0;
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA1 (fusion of series places)...\n");
  
  set<string> uselessTransitions;
  set<pair<string, string> > placePairs;
  
  
  // iterate the transtions
  for (set<Transition*>::iterator t = T.begin(); t != T.end(); t++)
  {
    Place* prePlace = static_cast<Place*>(*((*t)->preset.begin()));
    Place* postPlace = static_cast<Place*>(*((*t)->postset.begin()));
    
    if (((*t)->preset.size() == 1) && ((*t)->postset.size() == 1) && // precondition 1
        (prePlace != postPlace) &&			 // precondition 2
        ((prePlace)->postset.size() == 1) &&		 // precondition 3
        (prePlace->type == INTERNAL) &&			 // precondition 4
        (postPlace->type == INTERNAL) &&
        (arc_weight(prePlace, *t) == 1 && arc_weight(*t, postPlace) == 1)) // precondition 5
    {
      string id1 = ((*((*t)->preset.begin()))->nodeFullName());
      string id2 = ((*((*t)->postset.begin()))->nodeFullName());
      placePairs.insert(pair<string, string>(id1, id2));
      uselessTransitions.insert(((*t)->nodeFullName()));
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
 * the same weight. (precondition 3).
 *
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
 */
void PetriNet::reduce_series_transitions()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RA2 (fusion of series transitions)...\n");
  int result=0;
  
  set<string> uselessPlaces;
  set<pair<string, string> > transitionPairs;
  
  
  // iterate the places
  for (set<Place*>::iterator p = P.begin(); p != P.end(); p++)
  {
    if (((*p)->postset.size() == 1) && ((*p)->preset.size() == 1)) // precondition 1
    {
      Transition* t1 = static_cast<Transition*>(*((*p)->preset.begin()));
      Transition* t2 = static_cast<Transition*>(*((*p)->postset.begin()));
      
      if (((t2)->preset.size() == 1) && // precondition 2
          (arc_weight(t1, *p) == arc_weight(*p, t2))) // precondition 5
      {
        string id1 = (t1->nodeFullName());
        string id2 = (t2->nodeFullName());
        transitionPairs.insert(pair<string, string>(id1, id2));
        uselessPlaces.insert(((*p)->nodeFullName()));
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
 */
unsigned int PetriNet::reduce_self_loop_places()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RC1 (Elimination of self-loop places)...\n");
  list<Place *> self_loop_places;
  unsigned int result = 0;
  
  // find places fulfilling the preconditions
  for (set<Place *>::iterator p = P.begin(); p != P.end(); p++)
    if ((*p)->tokens > 0)
      if ((*p)->postset.size() == 1 && (*p)->preset.size() == 1)
        if ((*p)->preset == (*p)->postset)
          self_loop_places.push_back(*p);
  
  // remove useless places
  for (list<Place *>::iterator p = self_loop_places.begin(); p != self_loop_places.end(); p++)
    if (P.find(*p) != P.end())
    {
      removePlace(*p);
      result++;
    }
      
      if (result!=0)
        trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
  
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
 */
unsigned int PetriNet::reduce_self_loop_transitions()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule RC1 (Elimination of self-loop transitions)...\n");
  list<Transition *> self_loop_transitions;
  unsigned int result = 0;
  
  // find transitions fulfilling the preconditions
  for (set<Transition *>::iterator t = T.begin(); t != T.end(); t++)
    if ((*t)->postset.size() == 1 && (*t)->preset.size() == 1)
      if ((*t)->preset == (*t)->postset)
        self_loop_transitions.push_back(*t);
  
  // remove useless transitions
  for (list<Transition *>::iterator t = self_loop_transitions.begin(); t != self_loop_transitions.end(); t++)
    if (T.find(*t) != T.end())
    {
      removeTransition(*t);
      result++;
    }
      
      if (result!=0)
        trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
  return result;
}





/*!
 * \brief Elimination of equal places (RD1):
 *
 * If there exist two distinct (precondition 1) places with only one (precondition 2) 
 * outgoing arc with a weight of 1 (precondition 3) each leading to two distinct 
 * (precondition 4) Transitions t1 and t2, which have identical presets and postsets excluding
 * p1 and p2 (precondition 5) then p2 and t2 can be removed by directing all arcs that 
 * once led to p2 to p1.
 *
 * Added precondition 0, which ensures that the place which is going to be removed is not an input
 * place.
 *
 */
void PetriNet::reduce_equal_places()
{
  
  trace(TRACE_DEBUG, "[PN]\tApplying rule RD1 (elimination of equal places)...\n");
  int result=0;
  set<pair<string, string> > placePairs;
  set<pair<string, string> > delPairs;
  bool safe = true;
  
  set<Place*> candidates = P;
  // since we need two places for this rule, we need sufficent candidates
  while ( candidates.size() > 1 )
  {
    Place * p1 = *(candidates.begin());
    // erase candidate from set, so it cannot compared with itself and set gets smaller
    candidates.erase(p1);
    
    if ((p1)->postset.size() !=1) //precondition 2
      continue;
    
    if (arc_weight(p1,*(p1)->postset.begin())!=1) //precondition 3
      continue;
    
    Node* t1 = *(p1)->postset.begin();
    
    for (set<Place*>::iterator p2 = candidates.begin(); p2 != candidates.end(); p2++)
    {
      if ((*p2)->type != INTERNAL) //precondition 0
        continue;
      
      if(p1 == *p2) // precondition 1
        continue;
      
      if ((*p2)->postset.size() !=1) //precondition 2
        continue;
      
      if (arc_weight(*p2,*(*p2)->postset.begin()) !=1) //precondition 3
        continue;
      
      Node* t2 = *(*p2)->postset.begin();
      
      if (t1 == t2) //precondition 4
        continue;
      set<Node*> postSetT1 = postset (t1);
      set<Node*> postSetT2 = postset (t2);
      
      if (postSetT1 != postSetT2) //precondition 5
        continue;
      
      set<Node*> preSetT1 = (t1)->preset;
      set<Node*> preSetT2 = (t2)->preset;
      
      preSetT1.erase(p1);
      preSetT2.erase(*p2);
      
      if (preSetT1 != preSetT2) //precondition 5
        continue;
      
      string id1 = ((p1)->nodeFullName());
      string id2 = ((*p2)->nodeFullName());
	    
      for (set<pair<string, string> >::iterator labels = placePairs.begin();
           labels != placePairs.end(); labels++)
      {
        if ( (labels->first==id1) || (labels->second==id1) || (labels->first==id2) || (labels->second==id2))
          safe = false;
      }
	    
      if (placePairs.find(pair<string, string>(id2, id1)) == placePairs.end() && safe)
      {
        placePairs.insert(pair<string, string>(id1, id2));
      }
    }
  }
  
  /*
   // Testing all preconditions
   for (set<Place*>::iterator p1 = P.begin(); p1 != P.end(); p1++)
   {
     if ((*p1)->postset.size() !=1) //precondition 2
       continue;
     
     if (arc_weight(*p1,*(*p1)->postset.begin())!=1) //precondition 3
       continue;
     
     Node* t1= *(*p1)->postset.begin();
     
     for (set<Place*>::iterator p2 = P.begin(); p2 != P.end(); p2++)
     {
       if ((*p2)->type != INTERNAL) //precondition 0
         continue;
       
       if(*p1 == *p2) // precondition 1
         continue;
       
       if ((*p2)->postset.size() !=1) //precondition 2
         continue;
       
       if (arc_weight(*p2,*(*p2)->postset.begin()) !=1) //precondition 3
         continue;
       
       Node* t2 = *(*p2)->postset.begin();
       
       if (t1 == t2) //precondition 4
         continue;
       
       set<Node*> postSetT1 = postset (t1);
       set<Node*> postSetT2 = postset (t2);
       
       if (postSetT1 != postSetT2) //precondition 5
         continue;
       
       set<Node*> preSetT1 = (t1)->preset;
       set<Node*> preSetT2 = (t2)->preset;
       
       preSetT1.erase(*p1);
       preSetT2.erase(*p2);
       
       if (preSetT1 != preSetT2) //precondition 5
         continue;
       
       string id1 = ((*p1)->nodeFullName());
       string id2 = ((*p2)->nodeFullName());
       
       for (set<pair<string, string> >::iterator labels = placePairs.begin();
            labels != placePairs.end(); labels++)
       {
         if ( (labels->first==id1) || (labels->second==id1) || (labels->first==id2) || (labels->second==id2))
           safe = false;
       }
       
       if (placePairs.find(pair<string, string>(id2, id1)) == placePairs.end() && safe)
       {
         placePairs.insert(pair<string, string>(id1, id2));
       }
     }
   }
   */
  
  
  for (set<pair<string, string> >::iterator labels = placePairs.begin();
       labels != placePairs.end(); labels++)
  {
    trace(TRACE_DEBUG, "[PN]\tFound something to reduce with RD1!\n");
    Place* p1 = findPlace(labels->first);
    Place* p2 = findPlace(labels->second);
    
    string trans_id = ((*(p2->postset.begin()))->nodeFullName());
    
    unsigned int arcadd = 0;
    
    for (set<Node*>::iterator n = p2->preset.begin(); n != p2->preset.end(); n++)
    {
      arcadd=0;
      
      // test if there has already been an arc
      if(p1->preset.find(*n) != p1->preset.end())
      {
        arcadd = arcadd + arc_weight(*n, p1);          
        for (set<Arc*>::iterator f = F.begin(); f != F.end(); f++)
          if (((*f)->source == *n) || ((*f)->target == p1))
            removeArc(*f);
      }
      
      newArc(*n, p1, STANDARD, (arc_weight(*n,p2) + arcadd));
      
    }
    
    p1->tokens = p1->tokens + p2->tokens;
    
    removePlace(p2);
    removeTransition(findTransition(trans_id)); 
    result++;   
  }
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
}





/*!
 * \brief remove unneeded initially marked places in choreographies
 *
 * \todo comment me!
 */
void PetriNet::reduce_remove_initially_marked_places_in_choreographies()
{
  trace(TRACE_DEBUG, "[PN]\tApplying rule \"Elimination of unnecessary initial places\"...\n");
  
  set<Place*> redundant_places;
  
  // traverse the places
  for (set<Place *>::const_iterator place = P.begin();
       place != P.end();
       place++)
  {
    // find initial places with empty preset and singleton postset
    if ( (*place)->tokens == 1  &&
         (*place)->preset.empty() &&
         (*place)->postset.size() == 1 )
    {
      Transition *post_transition = static_cast<Transition *> (*((*place)->postset.begin()));
      
      // if the transition in the postset is executed exactly once...
      if ( post_transition->max_occurrences == 1 )
      {
        for (set<Node *>::const_iterator pre_place = post_transition->preset.begin();
             pre_place != post_transition->preset.end();
             pre_place++)
        {
          // ... and has a former communication place in its preset
          if (static_cast<Place *>(*pre_place)->wasExternal != "")
          {
            // the marked place is not necessary
            redundant_places.insert(*place);
            break;
          }
        }
      }
    }
  }
  
  
  // remove the redundant places
  for (set<Place*>::const_iterator place = redundant_places.begin();
       place != redundant_places.end();
       place++)
  {
    removePlace(*place);
  }
  
  if (!redundant_places.empty())
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(redundant_places.size()) + " places.\n");
}


/***** BEGIN CODE by Thomas Pillat *****/
/*!
 * Abstraction rule as defined in [ES01]. This rule removes a place and merges the post- and preset 
 * of this place if the place satisfies certain conditions. The
 * rule preserves boundedness, state based LTL-X or action based LTL properties
 * which only consider transitions of the reduced net or places of the reduced net, respectively.
 * Furthemore, if the original net is live, the reduced net will also be live if
 * the original net is safe. The conversion holds not. Thus, deadlock won't be preserved.
 *
 * \note As the place in \f$ t \f$'s preset/postset has ingoing and outgoing
 *       arcs, it is an internal place. Thus, \f$ t \f$ does not communicate
 *       and its removal does not affect controllability or the set of
 *       communicating partners.
 *
 *
 * \pre \f$ p \f$ is a place of the net: \f$ p \in P \f$
 * \pre \f$ t \f$ is a transition in the postset of \f$ p \f$ but not in the preset of \f$ p \f$: \f$ p \in p^{\bullet}\setminus {^{\bullet}p} \f$
 * \pre \f$ t \f$ is the only transition in the postset of \f$ p \f$: \f$ p^\bullet = \{t\} \f$
 * \pre the preset of \f$ p \f$ must not be empty: \f$ {{^{bullet}}p}\neq\emptyset \f$
 * \pre the postset of the preset of \f$ p \f$ is again \f$ p \f$: \f$ ({^{\bullet}p}^{\bullet})=\{p\} \f$
 * \pre \f$ p \f$ must not be marked initially: \f$ M_0(p)=0 \f$
 * \pre weights of all arcs form \f$ p \f$ to adjacent Transitions must be 1: \f$ \forall (p,t),(t,p)\in F\,:\,W((p,t))=W((t,p))=1  \f$
 * 
 * \post \f$ p \f$ is removed: \f$ P' = P \; \setminus \; \{p\} \f$
 * \post \f$ t \f$ is removed and if the union of the preset of a transition \f$ u \in {^{\bullet}p} \f$ and of the preset of \f$ t \f$ is not empty, \f$ u \f$ is also removed: \f$T' = T \setminus (\{t\}\cup\{u \in {^{\bullet}p}\mid {^{\bullet}u}\cap{^{\bullet}u}\neq\emptyset\}) \f$
 * \post 
 */
void PetriNet::reduce_esparzas_abstraction_rule() {
    trace(TRACE_DEBUG,
                    "[PN]\t Applying rule RE1 (Esparzas and Schroeters abstraction rule)...\n");
    // p is a place satisfying the abstraction rule
    set<Node *> none_p_preset_transitions;
    set<Node *> none_p_places;
    unsigned int p_result = 0;
    unsigned int t_result = 0;
    unsigned int old_P_size = 0;
    bool isPlaceToRemove;
    Place *placeToRemove;

    while (old_P_size != P.size() ) {

        isPlaceToRemove = false;
        old_P_size = P.size();
        // traverse the places
        for (set<Place *>::const_iterator place = P.begin(); place != P.end(); place++) {

            if (none_p_places.begin() != none_p_places.end()
                            && none_p_places.find( *place)
                                            != none_p_places.end() ) {
                // this place is no candidate for a place satisfying the abstraction rule or was yet
                // analyzed. Hence,...
                continue; //... with the next place
            }

            set<Node *> p_postset = (*place)->postset;
            set<Node *> p_preset = (*place)->preset;

            if ( !p_preset.empty() // precondition 4 is satisfied
            && !p_postset.empty() // this is necessary to satisfy condition 5
            && (*place)->tokens == 0 // precondition 6 is satisfied                                      
                            && p_postset.size() == 1 // precondition 3 is satisfied
                            && setIntersection( p_postset, p_preset ).empty() // precondition 2 is satisfied 
            && sameweights( *place) // precondition 7 is satisfied
            && is_presets_postset_singleton( *place) ) { // precondition 5 is satisfied
                // all conditions are satisfied, thus the loop must be left to remove the place
                placeToRemove = *place;
                isPlaceToRemove = true;
                break;

            } else { // one of the preconditions is not satisfied 
                // ...hence all input transitions of this place cannot be the only input transtions of another place
                none_p_preset_transitions.insert( (p_preset.begin()),
                                (p_preset.end()));
                // ...and the output places of their input transitions and so on
                // cannot be places satisfying precondition 5
                set<Node *> none_p_places_Tmp;
                set<Node *> outputPlaces;
                none_p_places_Tmp.insert( *place);
                none_p_places.insert( *place);

                while ( !none_p_places_Tmp.empty() ) {

                    for (set<Node *>::const_iterator none_p =
                                    none_p_places_Tmp.begin(); none_p
                                    != none_p_places_Tmp.end(); none_p++) {
                        set<Node *> presetTmp = (*none_p)->preset;

                        for (set<Node *>::const_iterator t = presetTmp.begin(); t
                                        != presetTmp.end(); t++) {

                            if (none_p_preset_transitions.find( *t)
                                            != none_p_preset_transitions.end() ) {
                                outputPlaces.insert((*t)->postset.begin(), (*t)->postset.end() );
                            }
                        }
                    }

                    none_p_places_Tmp.erase( *place);
                    if ( !outputPlaces.empty() ) {
                        none_p_places_Tmp = setDifference(outputPlaces,
                                        none_p_places);
                        none_p_places.insert(outputPlaces.begin(),
                                        outputPlaces.end() );
                    }
                }
            }
        }

        if (isPlaceToRemove) {
            set<Transition *> placeToRemovePreset;
            set<Node *> preSet = placeToRemove->preset;

            for (set<Node *>::const_iterator pre_t = preSet.begin(); pre_t
                            != preSet.end(); pre_t++) {
                placeToRemovePreset.insert( static_cast<Transition *>(*pre_t) );
            }

            set<Transition *> placeToRemovePostset;
            set<Node *> postSet = placeToRemove->postset;

            for (set<Node *>::const_iterator post_t = postSet.begin(); post_t
                            != postSet.end(); post_t++) {
                placeToRemovePostset.insert( static_cast<Transition *>(*post_t) );
            }

            set<Transition *> transitions_to_remove;
            // Removes each transitions from the specified preset if and only if the unfication of
            // the preset and the postset of the according transition is not empty.
            for (set<Transition *>::const_iterator t_pre =
                            placeToRemovePreset.begin(); t_pre
                            != placeToRemovePreset.end(); t_pre++) {

                for (set<Transition *>::const_iterator t_post =
                                placeToRemovePostset.begin(); t_post
                                != placeToRemovePostset.end(); t_post++) {

                    if ( !(setIntersection( (*t_pre)->preset, (*t_post)->preset ).empty() )) {
                        transitions_to_remove.insert( *t_pre);
                    }
                }
            }

            for (set<Transition *>::const_iterator t =
                            transitions_to_remove.begin(); t
                            != transitions_to_remove.end(); t++) {

                placeToRemovePreset.erase( *t);
                removeTransition( *t);
                t_result++;
            }

            // detach the place from the petri net and remove it finally
            removePlace(placeToRemove);
            // compute the cartesian product and determine the pre and post set of the new places
            agglomerateTransitions(placeToRemovePreset, placeToRemovePostset);
            p_result++;
            t_result++;
        }
        none_p_places.clear();
    }

    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(t_result)
                    + " transitions.\n");
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(p_result) + " places.\n");
}

/*!
 * Pre agglomeration rule as defined in [ES01]. This rule removes a place and 
 * merges the post- and preset of this place if the place satisfies certain 
 * conditions. The rule preserves boundedness, state based LTL-X or action based
 * LTL properties which only consider transitions of the reduced net or places 
 * of the reduced net.
 * Furthemore, if the original net is live, the reduced net will also be live in
 *  the case the the original net is safe. The conversion holds not. Thus, 
 * deadlock won't be preserved.
 *
 * \note As the place in \f$ t \f$'s preset/postset has ingoing and outgoing
 *       arcs, it is an internal place. Thus, \f$ t \f$ does not communicate
 *       and its removal does not affect controllability or the set of
 *       communicating partners.
 *
 *
 * \pre \f$ p \f$ is a place of the net: \f$ p \in P \f$
 * \pre \f$ u \f$ is a transition in the preset of \f$ p \f$ but not in the postset of \f$ p \f$: \f$ p \in {^{\bullet}p}\setminus p^{\bullet}\f$
 * \pre \f$ u \f$ is the only transition in the preset of \f$ p \f$: \f$ {^\bullet}p = \{u\} \f$
 * \pre the preset of \f$ p \f$ must not be empty: \f$ {{^{bullet}}p}\neq\emptyset \f$
 * \pre the preset of the postset of \f$ p \f$ again is \f$ p \f$: \f$ {}^{\bullet}(p^{\bullet})=\{p\} \f$
 * \pre \f$ p \f$ must not be marked initially: \f$ M_0(p)=0 \f$
 * \pre weights of all arcs form \f$ p \f$ to adjacent Transitions must be 1: \f$ \forall (p,t),(t,p)\in F\,:\,W((p,t))=W((t,p))=1  \f$
 * 
 * \post \f$ p \f$ is removed: \f$ P' = P \; \setminus \; \{p\} \f$
 * \post \f$ t \f$ is removed and if the union of the preset of a transition \f$ u \in {^{\bullet}p} \f$ and of the preset of \f$ t \f$ is not empty, \f$ u \f$ is also removed: \f$T' = T \setminus (\{t\}\cup\{u \in {^{\bullet}p}\mid {^{\bullet}u}\cap{^{\bullet}u}\neq\emptyset\}) \f$
 * \post 
 */
void PetriNet::reduce_pre_agglomeration() {
    trace(TRACE_DEBUG,
                    "[PN]\t Applying rule RE2 (Esparzas and Schroeters pre agglomeration)...\n");
    // p is a place satisfying the abstraction rule
    set<Node *> none_p_preset_transitions;
    set<Node *> none_p_places;
    unsigned int p_result = 0;
    unsigned int t_result = 0;
    unsigned int old_P_size = 0;
    bool isPlaceToRemove;
    Place *placeToRemove;

    while (old_P_size != P.size() ) {

        isPlaceToRemove = false;
        old_P_size = P.size();
        // traverse the places
        for (set<Place *>::const_iterator place = P.begin(); place != P.end(); place++) {

            if (none_p_places.begin() != none_p_places.end()
                            && none_p_places.find( *place)
                                            != none_p_places.end() ) {
                // this place is no candidate for a place satisfying the abstraction rule or was yet
                // analyzed. Hence,...
                continue; //... with the next place
            }

            set<Node *> p_postset = (*place)->postset;
            set<Node *> p_preset = (*place)->preset;

            if ( !p_preset.empty() // precondition 4 is satisfied
            && !p_postset.empty() // this is necessary to satisfy condition 5
            && (*place)->tokens == 0 // precondition 6 is satisfied                                      
                            && p_preset.size() == 1 // precondition 3 is satisfied
                            && setIntersection( p_postset, p_preset ).empty() // precondition 2 is satisfied 
            && sameweights( *place) // precondition 7 is satisfied
            && is_presets_postset_singleton( *place) ) { // precondition 5 is satisfied
                // all conditions are satisfied, thus the loop must be left to remove the place
                placeToRemove = *place;
                isPlaceToRemove = true;
                break;

            } else { // one of the preconditions is not satisfied 
                // ...hence all input transitions of this place cannot be the only input transtions of another place
                none_p_preset_transitions.insert( (p_preset.begin()),
                                (p_preset.end()));
                // ...and the output places of their input transitions and so on
                // cannot be places satisfying precondition 5
                set<Node *> none_p_places_Tmp;
                set<Node *> outputPlaces;
                none_p_places_Tmp.insert( *place);
                none_p_places.insert( *place);

                while ( !none_p_places_Tmp.empty() ) {

                    for (set<Node *>::const_iterator none_p =
                                    none_p_places_Tmp.begin(); none_p
                                    != none_p_places_Tmp.end(); none_p++) {
                        set<Node *> presetTmp = (*none_p)->preset;

                        for (set<Node *>::const_iterator t = presetTmp.begin(); t
                                        != presetTmp.end(); t++) {

                            if (none_p_preset_transitions.find( *t)
                                            != none_p_preset_transitions.end() ) {
                                outputPlaces.insert((*t)->postset.begin(), (*t)->postset.end() );
                            }
                        }
                    }

                    none_p_places_Tmp.erase( *place);
                    if ( !outputPlaces.empty() ) {
                        none_p_places_Tmp = setDifference(outputPlaces,
                                        none_p_places);
                        none_p_places.insert(outputPlaces.begin(),
                                        outputPlaces.end() );
                    }
                }
            }
        }

        if (isPlaceToRemove) {
            set<Transition *> placeToRemovePreset;
            set<Node *> preSet = placeToRemove->preset;

            for (set<Node *>::const_iterator pre_t = preSet.begin(); pre_t
                            != preSet.end(); pre_t++) {
                placeToRemovePreset.insert( static_cast<Transition *>(*pre_t) );
            }

            set<Transition *> placeToRemovePostset;
            set<Node *> postSet = placeToRemove->postset;

            for (set<Node *>::const_iterator post_t = postSet.begin(); post_t
                            != postSet.end(); post_t++) {
                placeToRemovePostset.insert( static_cast<Transition *>(*post_t) );
            }

            set<Transition *> transitions_to_remove;
            // Removes each transitions from the specified postset if and only if the unfication of
            // the preset and the postset of the according transition is not empty.
            for (set<Transition *>::const_iterator t_pre =
                            placeToRemovePreset.begin(); t_pre
                            != placeToRemovePreset.end(); t_pre++) {

                for (set<Transition *>::const_iterator t_post =
                                placeToRemovePostset.begin(); t_post
                                != placeToRemovePostset.end(); t_post++) {

                    if ( !(setIntersection( (*t_pre)->preset, (*t_post)->preset ).empty() )) {
                        transitions_to_remove.insert( *t_post);
                    }
                }
            }

            for (set<Transition *>::const_iterator t =
                            transitions_to_remove.begin(); t
                            != transitions_to_remove.end(); t++) {

                placeToRemovePostset.erase( *t);
                removeTransition( *t);
                t_result++;
            }

            // detach the place from the petri net and remove it finally
            removePlace(placeToRemove);
            // compute the cartesian product and determine the pre and post set of the new places
            agglomerateTransitions(placeToRemovePreset, placeToRemovePostset);

            p_result++;
            t_result++;
        }

        none_p_places.clear();
    }

    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(t_result)
                    + " transitions.\n");
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(p_result) + " places.\n");
}

/*!
 * Post agglomeration rule as defined in [ES01]. This rule removes a place and merges the post- and preset 
 * of this place if the place satisfies certain conditions. The
 * rule preserves boundedness, state based LTL-X or action based LTL properties
 * which only consider transitions of the reduced net or places of the reduced net.
 * Furthemore, if the original net is live, the reduced net will also be live in the case the
 * the original net is safe. The conversion holds not. Thus, deadlock won't be preserved.
 *
 * \note As the place in \f$ t \f$'s preset/postset has ingoing and outgoing
 *       arcs, it is an internal place. Thus, \f$ t \f$ does not communicate
 *       and its removal does not affect controllability or the set of
 *       communicating partners.
 *
 *
 * \pre \f$ p \f$ is a place of the net: \f$ p \in P \f$
 * \pre the preset of \f$ p \f$ must not be empty: \f$ {{^{bullet}}p}\neq\emptyset \f$
 * \pre the postset of \f$ p \f$ must not be empty: \f$ p^{bullet}\neq\emptyset \f$
 * \pre the postset of the preset of \f$ p \f$ is again \f$ p \f$: \f$ ({^{\bullet}p}^{\bullet})=\{p\} \f$
 * \pre \f$ p \f$ must not be marked initially: \f$ M_0(p)=0 \f$
 * \pre weights of all arcs form \f$ p \f$ to adjacent Transitions must be 1: \f$ \forall (p,t),(t,p)\in F\,:\,W((p,t))=W((t,p))=1  \f$
 * 
 * \post \f$ p \f$ is removed: \f$ P' = P \; \setminus \; \{p\} \f$
 * \post \f$ t \f$ is removed and if the union of the preset of a transition \f$ u \in {^{\bullet}p} \f$ and of the preset of \f$ t \f$ is not empty, \f$ u \f$ is also removed: \f$T' = T \setminus (\{t\}\cup\{u \in {^{\bullet}p}\mid {^{\bullet}u}\cap{^{\bullet}u}\neq\emptyset\}) \f$
 * \post 
 */
void PetriNet::reduce_post_agglomeration() {
    trace(TRACE_DEBUG,
                    "[PN]\t Applying rule RE3 (Esparzas and Schroeters post agglomeration)...\n");
    // p is a place satisfying the abstraction rule
    unsigned int p_result = 0;
    unsigned int t_result = 0;
    unsigned int old_P_size = 0;
    bool isPlaceToRemove;
    Place *placeToRemove;

    while (old_P_size != P.size() ) {

        isPlaceToRemove = false;
        old_P_size = P.size();
        // traverse the places
        for (set<Place *>::const_iterator place = P.begin(); place != P.end(); place++) {

            set<Node *> p_postset = (*place)->postset;
            set<Node *> p_preset = (*place)->preset;

            if ( !p_preset.empty() // precondition 4 is satisfied
            && !p_postset.empty() // this is necessary to satisfy condition 5
            && (*place)->tokens == 0 // precondition 6 is satisfied                                      
                            && setIntersection( p_postset, p_preset ).empty() // precondition 2 is satisfied 
            && sameweights( *place) // precondition 7 is satisfied
            && is_postsets_preset_singleton( *place) ) { // precondition 5 is satisfied
                // all conditions are satisfied, thus the loop must be left to remove the place
                placeToRemove = *place;
                isPlaceToRemove = true;
                break;

            }

            if (isPlaceToRemove) {
                set<Transition *> placeToRemovePreset;
                set<Node *> preSet = placeToRemove->preset;

                for (set<Node *>::const_iterator pre_t = preSet.begin(); pre_t
                                != preSet.end(); pre_t++) {
                    placeToRemovePreset.insert( static_cast<Transition *>(*pre_t) );
                }

                set<Transition *> placeToRemovePostset;
                set<Node *> postSet = placeToRemove->postset;

                for (set<Node *>::const_iterator post_t = postSet.begin(); post_t
                                != postSet.end(); post_t++) {
                    placeToRemovePostset.insert( static_cast<Transition *>(*post_t) );
                }

                // detach the place from the petri net and remove it finally
                removePlace(placeToRemove);
                // compute the cartesian product and determine the pre and post set of the new places
                agglomerateTransitions(placeToRemovePreset,
                                placeToRemovePostset);

                p_result++;
            }
        }
    }

    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(t_result)
                    + " transitions.\n");
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(p_result) + " places.\n");
}

/*!
 * \brief Checks whether the presets postset of the specified place only contains the specified place itself.
 * 
 * \param p a place
 * 
 * returns true if the \f$ ({^{\bullet}p}^{\bullet})=\{p\} \f$
 */
bool PetriNet::is_presets_postset_singleton(Place *p) const {

    set<Node *> p_preset = p->preset;

    for (set<Node *>::const_iterator t = p_preset.begin(); t != p_preset.end(); t++) {
        // the postset of every transition in the preset of p must contain only one element (p itself)
        if ((*t)->postset.size() != 1)
            return false;
    }

    return true;
}

/*!
 * \brief Checks whether the postsets preset of the specified place only contains the specified place itself.
 * 
 * \param p a place
 * 
 * returns true if the \f$ ({^{\bullet}p}^{\bullet})=\{p\} \f$
 */
bool PetriNet::is_postsets_preset_singleton(Place *p) const {

    set<Node *> p_postset = p->postset;

    for (set<Node *>::const_iterator t = p_postset.begin(); t != p_postset.end(); t++) {
        // the preset of every transition in the postset of p must contain only one element (p itself)
        if ((*t)->preset.size() != 1)
            return false;
    }

    return true;
}

/*!
 * Implicite place rule as defined in [ES01]. This rule removes an implicit place. The
 * rule preserves boundedness, liveness, state based LTL-X or action based LTL properties
 * which only consider transitions of the reduced net or places of the reduced net.
 *
 * \note As the place in \f$ t \f$'s preset/postset has ingoing and outgoing
 *       arcs, it is an internal place. Thus, \f$ t \f$ does not communicate
 *       and its removal does not affect controllability or the set of
 *       communicating partners.
 *
 *
 * \pre  the place \f$ p $\f is implicit if the following linear program holds a solution
 *          - minimize \f$ {\vec Y}^T \cdot M_0 + \mu $\f such that
 *          - \f$ {\vec Y} \ge \mathbf{0} $\f
 *          - \f$ {\vec Y}^T \cdot \mathbf{N} \le {\vec l_p}$\f
 *          - \f$ Y(p) = 0 $\f
 *          - \f$ \forall t_i \in p^{\bullet}\,:\, \sum_{p' \in {^{\bullet}t_i}} Y(p') \cdot F(p',t_i) + \mu \ge F(p,t_i) $\f 
 *          - \f$ {\vec Y}^T \cdot M_0 + \mu \le M_0(p) $\f
 * 
 * \post \f$ p \f$ is removed: \f$ P' = P \; \setminus \; \{p\} \f$
 */

#ifdef HAVE_LIBLPSOLVE55

void PetriNet::reduce_implicit_places() {
    int result=0;
    trace(TRACE_DEBUG,
                    "[PN]\tApplying rule ES01 (removal of implicit places)...\n");

    int Ncol, j, i, ret = 0, t_number = T.size(), p_number = P.size();
    unsigned int prior_p_size = 0;

    while (prior_p_size != P.size() ) {

        map< string, int> pName2ConstraintIndex;

        i = 0;

        prior_p_size = P.size();
        int **incidenceMatrix = getIncidenceMatrix();
        p_number = P.size();

        for (set<Place*>::const_iterator p_candidate = P.begin(); p_candidate
                        != P.end(); p_candidate++) {

            lprec *lp;
            int *colno= NULL;
            REAL *row = NULL;

            /* We will build the model row by row
             So we start with creating a model with 0 rows and P.size() + 1 columns */
            Ncol = P.size() + 1; /* we have as many variables as places and the variable \f$ \mu \f$ additionally */

            lp = make_lp(0, Ncol);
            if (lp == NULL)
                ret = 1; /* couldn't construct a new model... */

            if (ret == 0) {

                j = 1;
                /* let us name our variables. Not required, but can be useful for debugging */
                for (set<Place *>::const_iterator p = P.begin(); p != P.end(); p++) {
                    char pName[4];
                    strncpy(pName, ((*p)->nodeFullName()).c_str(), sizeof(pName )
                                    - 1);
                    set_col_name(lp, j, pName);
                    pName2ConstraintIndex[ (*p)->nodeFullName() ] = j++;
                }

                set_col_name(lp, Ncol, "mu");

                // all variables must be integers, it is an integer linear program
                for (int x = 1; x <= Ncol; x++) {
                    set_int(lp, x, TRUE);
                }

                /* create space large enough for one row */
                colno = (int *) malloc(Ncol * sizeof(*colno));
                row = (REAL *) malloc(Ncol * sizeof(*row));

                if ((colno == NULL) || (row == NULL))
                    ret = 2;
            }

            /* add all the constraints */

            if (ret == 0) {

                set_add_rowmode(lp, TRUE); /* makes building the model faster if it is done rows by row */

                /* construct \f$ {\vec Y} \ge \mathbf{0} \f$ */
                j = 0;
                for (int y = 0; y <= Ncol; y++) {

                    if (i == y) {
                        // \f$ Y(p) = 0 $\f
                        colno[j] = y + 1;
                        row[j++] = 1;

                        /* add the row to lpsolve */
                        if ( !add_constraintex(lp, j, row, colno, EQ, 0) ) {
                            ret = 3;
                            break;
                        }

                    } else {
                        colno[j] = y + 1;
                        row[j++] = 1;

                        /* add the row to lpsolve */
                        if ( !add_constraintex(lp, j, row, colno, GE, 0) ) {
                            ret = 3;
                            break;
                        }

                    }

                    j = 0;
                }

            }

            /* build up the following inequation system
             * \f$ {\vec Y}^T \cdot \mathbf{N} \le {\vec l_p}$\f
             */
            if (ret == 0) {

                for (int x = 0; x < t_number; x++) {

                    for (int y = 0; y < p_number; y++) {

                        colno[y] = y + 1;
                        row[y] = incidenceMatrix[y][x];
                    }

                    if ( !add_constraintex(lp, p_number, row, colno, LE,
                                    incidenceMatrix[i][x]) )
                        ret = 3;

                }
            }

            set<Place *>::const_iterator p = P.begin();

            /* build up the following inequation system
             * \f$ \forall t_i \in p^{\bullet}\,:\, \sum_{p' \in {^{\bullet}t_i}} Y(p') \cdot F(p',t_i) + \mu \ge F(p,t_i) $\f 
             */
            if (ret == 0) {
                set<Node *> postset = (*p)->postset;

                for (set<Node *>::const_iterator t = postset.begin(); t
                                != postset.end(); t++) {

                    j = 0;
                    set<Node *> preset = (*t)->preset;

                    for (set<Node *>::const_iterator pPar = preset.begin(); pPar
                                    != preset.end(); pPar++) {

                        row[j] = arc_weight( *pPar, *t);
                        map< string, int>::iterator
                                        iter =
                                                        pName2ConstraintIndex.find((*pPar)->nodeFullName() );

                        if (iter != pName2ConstraintIndex.end() ) {
                            colno[j++] = iter->second;
                        }
                    }

                    colno[j] = Ncol;
                    row[j++] = 1;

                    if ( !add_constraintex(lp, j, row, colno, GE, arc_weight(
                                    *p, *t) ) )
                        ret = 3;

                }

            }

            // \f$ {\vec Y}^T \cdot M_0 + \mu \le M_0(p) $\f
            if (ret == 0) {

                j = 0;
                for (set<Place *>::const_iterator place = P.begin(); place
                                != P.end(); place++) {

                    row[j] = (*place)->tokens;
                    map< string, int>::iterator iter =
                                    pName2ConstraintIndex.find((*place)->nodeFullName() );

                    if (iter != pName2ConstraintIndex.end() ) {
                        colno[j++] = iter->second;
                    }
                }

                colno[j] = Ncol;
                row[j++] = 1;

                if ( !add_constraintex(lp, j, row, colno, LE, (*p)->tokens) )
                    ret = 3;

            }

            if (ret == 0) {

                set_add_rowmode(lp, FALSE); /* rowmode should be turned off again when done building the model */

                j = 0;
                // setup the objective function: \f$ {\vec Y}^T \cdot M_0 + \mu $\f
                for (set<Place *>::const_iterator place = P.begin(); place
                                != P.end(); place++) {

                    row[j] = (*place)->tokens;
                    map< string, int>::iterator iter =
                                    pName2ConstraintIndex.find((*place)->nodeFullName() );

                    if (iter != pName2ConstraintIndex.end() ) {
                        colno[j++] = iter->second;
                    }
                }

                colno[j] = Ncol;
                row[j++] = 1;

                /* set the objective in lpsolve */
                if (!set_obj_fnex(lp, j, row, colno) )
                    ret = 4;
            }

            if (ret == 0) {
                /* set the object direction to minimize */
                set_minim(lp);

                /* just out of curioucity, now show the model in lp format on screen */
                /* this only works if this is a console application. If not, use write_lp and a filename */
                // write_LP(lp, stdout);
                // write_lp(lp, "model.lp");

                /* I only want to see important messages on screen while solving */
                set_verbose(lp, IMPORTANT);

                /* Now let lpsolve calculate a solution */
                ret = solve(lp);

                if (ret == OPTIMAL && !(*p_candidate)->isFinal && !((*p_candidate)->tokens
                                >0)) {
                    ret = 0;
                    removePlace( *p_candidate);
                    result++;
                    break;

                } else {
                    ret = 5;
                }
            }

            if (ret == 0) {
                /* a solution is calculated, now lets get some results */

                /* objective value */
                //              printf("Objective value: %f\n", get_objective(lp));

                /* variable values */
                get_variables(lp, row);
                //              for(j = 0; j < Ncol; j++)
                //                printf("%s: %f\n", get_col_name(lp, j + 1), row[j]);

                /* we are done now */
            }

            /* free allocated memory */
            if (row != NULL)
                free(row);
            if (colno != NULL)
                free(colno);

            if (lp != NULL) {
                /* clean up such that all used memory by lpsolve is freed */
                delete_lp(lp);
            }

            i++;
            ret = 0;
        }

    }

    if (result!=0)
        trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result)
                        + " places.\n");
}

#endif
/***** END CODE by Thomas Pillat *****/



/*!
 * Removes unnecessary nodes of the generated Petri net:
 *  - structural dead nodes
 *  - unused status places
 *  - suspicious transitions
 *
 * Applys some simple structural reduction rules for Petri nets:
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
 * \todo
 *       - Improve performance of the rules.
 *       - Pass a parameter to this function to choose the property of the
 *         model to be preserved.
 */
unsigned int PetriNet::reduce(unsigned int reduction_level)
{
  trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: " + information() + "\n");
  trace(TRACE_INFORMATION, "Simplifying Petri net...\n");
  
  string old = information();
  bool done = false;
  unsigned int passes = 1;
  
  while (!done)
  {
    if (reduction_level >= 1)
    {
      reduce_dead_nodes();
    }
    
    if (reduction_level >= 2)
    {
#ifdef USING_BPEL2OWFN
      reduce_unused_status_places();
      reduce_suspicious_transitions();
#endif
    }
    
    if (reduction_level >= 3)
    {
      reduce_identical_places();    // RB1
      reduce_identical_transitions();   // RB2
    }
    
    if (reduction_level >= 4)
    {
      reduce_series_places();       // RA1
      reduce_series_transitions();  // RA2
    }
    
    if (reduction_level >= 5)
    {
      reduce_self_loop_places();    // RC1
      reduce_self_loop_transitions();   // RC2
      reduce_remove_initially_marked_places_in_choreographies();
    }
    
    if (reduction_level == 6)
    {
#ifdef USING_BPEL2OWFN
      reduce_equal_places();        // RD1
#endif
    }

/***** BEGIN CODE by Thomas Pillat *****/
    if (reduction_level >= 7)
    {
      /**
       * TODO:
       * These rules have to be checked, if there preserve controllablity!
       * Until then there are not usable.
       */
        
      //reduce_esparzas_abstraction_rule();       // RE1
      //reduce_pre_agglomeration();               // RE2
      //reduce_post_agglomeration();              // RE3
    }

#ifdef HAVE_LIBLPSOLVE55
    if ( reduction_level >= 8 )
    {
       reduce_implicit_places();                // RI
    }
#endif
/***** END CODE by Thomas Pillat *****/
    
    trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification pass " + toString(passes++) + ": " + information() + "\n");
    
    done = (old == information());
    old = information();
  }
  
  
  trace(TRACE_INFORMATION, "Simplifying complete.\n");
  trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: " + information() + "\n");
  
  return passes;
}

