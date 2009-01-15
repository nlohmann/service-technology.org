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
 *          last changes of: \$Author: gierds $
 *
 * \since   2006-03-16
 *
 * \date    \$Date: 2008-06-19 11:42:07 $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.4 $
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
#include "util.h"		// helper functions (toString, setUnion)
//#include "debug.h"		// (trace)

using std::list;
using std::pair;
using std::cerr;
using std::endl;
using namespace pnapi;





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
  //trace(TRACE_DEBUG, "[PN]\tReducing unused places...\n");
  list<Place *> unused_status_places;
  unsigned int result = 0;
  
  // find unused status places
  for (set<Place*>::iterator p = places_.begin(); p != places_.end(); p++)
    if ( (*p)->getPostset().empty() )
      //FIXME: if ( !( (*p)->isFinal ) )
        if ( (*p)->getTokenCount() == 0 )
          unused_status_places.push_back(*p);
  
  // remove unused places
  for (list<Place*>::iterator p = unused_status_places.begin(); p != unused_status_places.end(); p++)
  {
    if (places_.find(*p) != places_.end())
    {
      deletePlace(**p);
      result++;
    }
  }
      if (result!=0)
      {
        //trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
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
  // trace(TRACE_DEBUG, "[PN]\tReducing suspicious transitions...\n");
  list<Transition*> suspiciousTransitions;
  unsigned int result = 0;
  
  // find suspicious transitions
  for (set<Transition*>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
    if ((*t)->getPostset().empty() || (*t)->getPreset().empty())
      suspiciousTransitions.push_back(*t);
  
  // remove suspicious transitions
  for (list<Transition*>::iterator t = suspiciousTransitions.begin(); t != suspiciousTransitions.end(); t++)
    if (transitions_.find(*t) != transitions_.end())
    {
      deleteTransition(**t);
      result++;
    }
      
      //if (result!=0)
        // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
  return result;
}





/*!
 * Remove structural dead nodes.
 *
 * \todo Re-organize the storing and removing of nodes.
 */
void PetriNet::reduce_dead_nodes()
{
  // trace(TRACE_DEBUG, "[PN]\tRemoving structurally dead nodes...\n");
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
    for (set<Place*>::iterator p = places_.begin(); p != places_.end(); p++)
    {
      //FIXME: if ((*p)->getPreset().empty() && !((*p)->isFinal) && !((*p)->historyContains("1.internal.initial")))
      {
        arcs=true;		
        for(set<Node*>::iterator t = (*p)->getPostset().begin(); t != (*p)->getPostset().end(); t++)
        {
          if(findArc(**p,**t)->getWeight() <= (*p)->getTokenCount())	
          {
            arcs=false;
          }
        }
        if(arcs)
        {
          deadPlaces.push_back(*p);
          tempPlaces.push_back(*p);
          // trace(TRACE_VERY_DEBUG, "[PN]\tPlace p" + toString((*p)->id) + " is structurally dead.\n");
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
      for (set<Node*>::iterator t = p->getPostset().begin(); t != p->getPostset().end(); t++)
      {
      	deadTransitions.push_back( static_cast<Transition*>(*t) );
        // trace(TRACE_VERY_DEBUG, "[PN]\tTransition t" + toString((*t)->id) + " is structurally dead\n");
        done = false;
      }
    }
    
    
    // remove dead places and transitions
    for (list<Place*>::iterator p = deadPlaces.begin(); p != deadPlaces.end(); p++)
    {
      if (places_.find(*p) != places_.end())
      {
        deletePlace(**p);
        result++;
      }
    }   
     
    for (list<Transition*>::iterator t = deadTransitions.begin(); t != deadTransitions.end(); t++)
    {
      if (transitions_. find(*t) != transitions_.end())
      {
        deleteTransition(**t);
        result++;
      }
    }
            
     /*       
            // remove isolated communication places
            list<Place*> uselessInputPlaces;
    
    for (set<Place*>::iterator p = inputPlaces_.begin(); p != inputPlaces_.end(); p++)
      if ((*p)->getPostset().empty())
        uselessInputPlaces.push_back(*p);
    
    for (list<Place*>::iterator p = uselessInputPlaces.begin(); p != uselessInputPlaces.end(); p++)
      if (inputPlaces_.find(*p) != inputPlaces_.end())
      {
        inputPlaces_.erase(*p);
        result++;
      }
        
        list<Place*> uselessOutputPlaces;
    
    for (set<Place*>::iterator p = outputPlaces_.begin(); p != outputPlaces_.end(); p++)
      if ((*p)->getPreset().empty())
        uselessOutputPlaces.push_back(*p);
    
    for (list<Place*>::iterator p = uselessOutputPlaces.begin(); p != uselessOutputPlaces.end(); p++)
      if (outputPlaces_.find(*p) != outputPlaces_.end())
      {
        outputPlaces_.erase(*p);
        result++;
      }
      */
  }
//    if (result!=0)
      // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " nodes.\n");
}





/*!
 * \brief Elimination of identical places (RB1):
 *
 * If there exist two distinct (precondition 1) places 
 * with identical preset (precondition 2) 
 * and postset (precondition 3) 
 * and the weights of all incoming and outgoing arcs 
 * have the value 1 (precondition 4), 
 * and neither of them is initially marked (precondition 5), 
 * then they can be "merged". 
 * I.e. one of these places can be removed (including all connected arcs)
 * and its history will be attached to the other's one.
 * (See mergeParallelPlaces)
 * 
 * This reduction preserves liveness and boundedness (Pillat2008, def. 3.3). 
 * 
 * \todo 
 *       - Overwork the preconditions and postconditions. 
 *       - Re-organize the storing and removing of nodes.
 * 
 */
void PetriNet::reduce_identical_places()
{
  int result=0;
  set<pair<string, string> > placePairs;
  
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RB1 (elimination of identical places)...\n");
  
  // iterate the places
  for (set<Place*>::iterator p1 = places_.begin(); p1 != places_.end(); p1++)
  {
    if (!(sameweights(*p1))) // precondition 4
      continue;
    
    // iterate the preset
    for (set<Node*>:: iterator preTransition = (*p1)->getPreset().begin(); preTransition != (*p1)->getPreset().end(); preTransition++)
    {
      for (set<Node*>::iterator p2 = (*preTransition)->getPostset().begin(); p2 != (*preTransition)->getPostset().end(); p2++)
        if ((*p1 != *p2) &&		// precondition 1
            ((*p1)->getPreset() == (*p2)->getPreset()) &&	// precondition 2
            ((*p1)->getPostset() == (*p2)->getPostset()) && // precondition 3
            (sameweights(*p2)) && // precondition 4
            (findArc(**preTransition, **p1)->getWeight() == 1) && // precondition 4 
            (findArc(**preTransition, **p2)->getWeight() == 1) && // precondition 4
            ((*p1)->getTokenCount() == 0) && // precondition 5
            (static_cast<Place*>(*p2)->getTokenCount() == 0) ) // precondition 5
        {
          string id1 = ((*p1)->getName());
          string id2 = ((*p2)->getName());
          placePairs.insert(pair<string, string>(id1, id2));
        }
    }
    
    if((*p1)->getPreset().empty()){
      // iterate the postset
      for (set<Node*>:: iterator postTransition = (*p1)->getPostset().begin(); postTransition != (*p1)->getPostset().end(); postTransition++)
      {
        for (set<Node*>::iterator p2 = (*postTransition)->getPreset().begin(); p2 != (*postTransition)->getPreset().end(); p2++)
          if ((*p1 != *p2) &&     // precondition 1
              ((*p1)->getPreset() == (*p2)->getPreset()) && // precondition 2
              ((*p1)->getPostset() == (*p2)->getPostset()) && // precondition 3
              (sameweights(*p2)) && // precondition 4
              (findArc((**p1), (**postTransition))->getWeight() == 1) && // precondition 4 
              (findArc((**p2), (**postTransition))->getWeight() == 1) ) // precondition 4
          {
            string id1 = ((*p1)->getName());
            string id2 = ((*p2)->getName());
            placePairs.insert(pair<string, string>(id1, id2));
          }
      }
    }
  }
    
    // trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(placePairs.size()) + " places with same preset and postset...\n");
    
    // "merge" the found places
    for (set<pair<string, string> >::iterator labels = placePairs.begin();
         labels != placePairs.end(); labels++)
    {
      Place* p1 = findPlace(labels->first);
      Place* p2 = findPlace(labels->second);
      p1->merge(*p2, false);
      result++;
    }
    //if (result!=0)
      // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
}





/*!
 * \brief Elimination of identical transitions (RB2):
 *
 * If there exist two distinct (precondition 1) transitions 
 * with identical preset (precondition 2) 
 * and postset (precondition 3) 
 * and none of those two is connected to any arc with a weight 
 * other than 1 (precondition 4),
 * then they can be "merged".
 * I.e. one place will be removed and its history will be attached
 * to the other ones.
 * 
 * This reduction preserves liveness and boundedness (Pillat2008, def. 5.4).
 * 
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
 * 
 */
void PetriNet::reduce_identical_transitions()
{
  int result=0;
  set<pair<string, string> > transitionPairs;
  
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RB2 (elimination of identical transitions)...\n");
  
  // iterate the transitions
  for (set<Transition*>::iterator t1 = transitions_.begin(); t1 != transitions_.end(); t1++)
  {
    if (!(sameweights(*t1))) // precondition 4
      continue;
    
    // iterate the preset
    for (set<Node*>:: iterator prePlace = (*t1)->getPreset().begin(); prePlace != (*t1)->getPreset().end(); prePlace++)
    {
      for (set<Node*>::iterator t2 = (*prePlace)->getPostset().begin(); t2 != (*prePlace)->getPostset().end(); t2++)
      {
        if ((*t1 != *t2) &&		// precondition 1
            ((*t1)->getPreset() == (*t2)->getPreset()) &&	// precondition 2
            ((*t1)->getPostset() == (*t2)->getPostset()) && // precondition 3
            (sameweights(*t2)) && // precondition 4
            (findArc(**prePlace,**t1)->getWeight() == 1) && // precondition 4
            (findArc(**prePlace,**t2)->getWeight() == 1) ) // precondition 4
        {
          string id1 = ((*t1)->getName());
          string id2 = ((*t2)->getName());
          transitionPairs.insert(pair<string, string>(id1, id2));
        }
      }
    }
    
    if ((*t1)->getPreset().empty())
    {
      // iterate the postset
      for (set<Node*>:: iterator postPlace = (*t1)->getPostset().begin(); postPlace != (*t1)->getPostset().end(); postPlace++)
      {
        for (set<Node*>::iterator t2 = (*postPlace)->getPreset().begin(); t2 != (*postPlace)->getPreset().end(); t2++)
        {
          if ((*t1 != *t2) &&     // precondition 1
              ((*t1)->getPreset() == (*t2)->getPreset()) && // precondition 2
              ((*t1)->getPostset() == (*t2)->getPostset()) && // precondition 3
              (sameweights(*t2)) && // precondition 4
              (findArc(**t1,**postPlace)->getWeight() == 1) && // precondition 4
              (findArc(**t2,**postPlace)->getWeight() == 1) ) // precondition 4
          {
            string id1 = ((*t1)->getName());
            string id2 = ((*t2)->getName());
            transitionPairs.insert(pair<string, string>(id1, id2));
          }
        }
      }
    }
  }
    
    // trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(transitionPairs.size()) + " transitions with same preset and postset...\n");
    
    // merge the found transitions
    for (set<pair<string, string> >::iterator labels = transitionPairs.begin();
         labels != transitionPairs.end(); labels++)
    {
      Transition* t1 = findTransition(labels->first);
      Transition* t2 = findTransition(labels->second);
      
      t1->merge(*t2, false);
      result++;
    }
    //if (result!=0)
      // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
}





/*!
 * \brief Fusion of series places (RA1):
 *
 * If there exists a transition with singleton preset and 
 * postset (precondition 1) that are distinct (precondition 2),
 * where the preset's preset is not empty (precondition 3)
 * and the place in the preset has no other outgoing arcs (precondition 4)
 * and alle arcs connected with this transition have a weight of 1 (precondition 5)
 * and the postplace ist not initially marked (precondition 6),
 * then the preset and postset can be merged an the transition can be removed.
 * Furthermore none of these places may be communicating (precondition 7).
 * 
 * This reduction preserves liveness and boundedness (Pillat2008, def. 4.6).
 * 
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
 * 
*/
void PetriNet::reduce_series_places()
{
  int result=0;
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RA1 (fusion of series places)...\n");
  
  set<string> uselessTransitions;
  set<pair<string, string> > placePairs;
  
  
  // iterate the transtions
  for (set<Transition*>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
  {
    Place* prePlace = static_cast<Place*>(*((*t)->getPreset().begin()));
    Place* postPlace = static_cast<Place*>(*((*t)->getPostset().begin()));
    
    if (((*t)->getPreset().size() == 1) && // precondition 1
        ((*t)->getPostset().size() == 1) && // precondition 1
        (prePlace != postPlace) &&			 // precondition 2
        (!(prePlace->getPreset().empty())) &&     // precondition 3
        ((prePlace)->getPostset().size() == 1) &&		 // precondition 4
        (findArc(*prePlace, **t)->getWeight() == 1) && // precondition 5
        (findArc(**t, *postPlace)->getWeight() == 1) && // precondition 5
        (postPlace->getTokenCount() == 0 ) && // precondition 6
        (prePlace->getType() == Node::INTERNAL) &&	// precondition 7
        (postPlace->getType() == Node::INTERNAL) ) // precondition 7
        
    {
      string id1 = ((*((*t)->getPreset().begin()))->getName());
      string id2 = ((*((*t)->getPostset().begin()))->getName());
      placePairs.insert(pair<string, string>(id1, id2));
      uselessTransitions.insert(((*t)->getName()));
    }
  }
  
  // remove useless transtions
  for (set<string>::iterator label = uselessTransitions.begin();
       label != uselessTransitions.end(); label++)
  {
    Transition* uselessTransition = findTransition(*label);
    if (uselessTransition != NULL)
      deleteTransition(*uselessTransition);
  }
  
  // merge place pairs
  for (set<pair<string, string> >::iterator placePair = placePairs.begin();
       placePair != placePairs.end(); placePair++)
  {
    findPlace(placePair->first)->merge(*findPlace(placePair->second));
    result++;
  }
  //if (result!=0)
    // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
}





/*!
 * \brief Fusion of series transition (RA2):
 *
 * If there exists a place with singleton preset and postset (precondition 1),
 * which is not final or marked (precondition 2),
 * and if the transition in its postset has no other incoming arcs (precondition 3),
 * and if the preset's postset and the postset's postset are disjunct (precondition 4),
 * then the preset and the postset can be merged and the place can be removed.
 * 
 * This reduction preserves liveness and boundedness (Pillat2008, def. 4.3). 
 * 
 * If both the pretransition and the posttransition are connected with an
 * interface place and keepNormal is true, the reduction will be prevented (precondition 5)
 *
 * 
 * \param   keepNormal determines wether reduction of a normalized net should
 *          preserve normalization or not. If true, interface transitions
 *          (i.e. transitions connected with interface places) will only be
 *          merged with internal transitions (i.e. transitions connected
 *          with internal places only). 
 * 
 * \todo
 *       - Overwork the preconditions and postconditions.
 *       - Re-organize the storing and removing of nodes.
 *       ???? What about initially marked and final places ????
 *
 */
void PetriNet::reduce_series_transitions(bool keepNormal) {
    // trace(TRACE_DEBUG, "[PN]\tApplying rule RA2 (fusion of series transitions)...\n");
    int result=0;

    set<string> uselessPlaces;
    set<pair<string, string> > transitionPairs;

    // iterate the places
    for (set<Place*>::iterator p = places_.begin(); p != places_.end(); p++) {
        if (((*p)->getPostset().size() == 1) && //precondition 1
            ((*p)->getPreset().size() == 1) && //precondition 1
            //FIXME: (!(*p)->isFinal) && // precondition 2
            (!(*p)->getTokenCount() > 0) ) // precondition 2
        {
            Transition* t1 = static_cast<Transition*>(*((*p)->getPreset().begin()));
            Transition* t2 = static_cast<Transition*>(*((*p)->getPostset().begin()));

            if (((t2)->getPreset().size() == 1) && // precondition 3
		(util::setIntersection(t1->getPostset(),t2->getPostset()).empty() ) && // precondition 4
		(!((t1->getType() != Node::INTERNAL) && 
		   (t2->getType() != Node::INTERNAL) && 
                  (keepNormal))) ) //precondition 5
            {
                string id1 = (t1->getName());
                string id2 = (t2->getName());
                transitionPairs.insert(pair<string, string>(id1, id2));
                uselessPlaces.insert(((*p)->getName()));
            }
        }
    }

    // remove useless places
    for (set<string>::iterator label = uselessPlaces.begin(); label
                    != uselessPlaces.end(); label++) {
        Place *uselessPlace = findPlace(*label);
        deletePlace(*uselessPlace);
    }

    // merge transition pairs
    for (set<pair<string, string> >::iterator transitionPair =
                    transitionPairs.begin(); transitionPair
                    != transitionPairs.end(); transitionPair++) {
        Transition* t1 = findTransition(transitionPair->first);
        Transition* t2 = findTransition(transitionPair->second);
        t1->merge(*t2);
        result++;
    }
    //if (result!=0)
    // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
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
 * Extension by arc weights:
 * If there exists a place p with singleton preset t (precondition 1),
 * where the preset is identical to the postset (precondition 2),
 * and the arc weights of (p->t) and (t->p) is equal (precondition 3),
 * and p stores initially at least as many tokens as given by the arc weights (precondition 4),
 * and p is not final (precondition 5),
 * then this place can be removed.
 * 
 */
unsigned int PetriNet::reduce_self_loop_places()
{
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RC1 (Elimination of self-loop places)...\n");
  list<Place *> self_loop_places;
  unsigned int result = 0;
  
  // find places fulfilling the preconditions
  for (set<Place *>::iterator p = places_.begin(); p != places_.end(); p++)
    if ( ((*p)->getPreset().size() == 1) &&       // precondition 1
         ((*p)->getPreset() == (*p)->getPostset()) &&  // precodnition 2
         (sameweights(*p)) &&                // precondition 3
         ((*p)->getTokenCount() >= findArc(**p,**((*p)->getPreset().begin()))->getWeight()) )//&& // precondition 4
         //FIXME: (!(*p)->isFinal) ) // precondition 5
	 self_loop_places.push_back(*p);
  
  // remove useless places
  for (list<Place *>::iterator p = self_loop_places.begin(); p != self_loop_places.end(); p++)
    if (places_.find(*p) != places_.end())
    {
      deletePlace(**p);
      result++;
    }
      
      //if (result!=0)
        // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
  
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
 * Extension by arc weights:
 * If there exists a transition t with singleton preset p (precondition 1),
 * where the preset is identical to the postset (precondition 2),
 * and the arc weights both of (p->t) and of (t->p) is equal to 1 (precondition 3),
 * then this transition can be removed.
 * 
 */
unsigned int PetriNet::reduce_self_loop_transitions()
{
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RC1 (Elimination of self-loop transitions)...\n");
  list<Transition *> self_loop_transitions;
  unsigned int result = 0;
  
  // find transitions fulfilling the preconditions
  for (set<Transition *>::iterator t = transitions_.begin(); t != transitions_.end(); t++)
    if ( ((*t)->getPreset().size() == 1) &&        // precondition 1
         ((*t)->getPreset() == (*t)->getPostset()) &&   // precondition 2
         (findArc(**t,**((*t)->getPreset().begin()))->getWeight() == 1) &&  // precondition 3
         (findArc(**((*t)->getPreset().begin()),**t)->getWeight() == 1) )  // precondition 3
        self_loop_transitions.push_back(*t);
  
  // remove useless transitions
  for (list<Transition *>::iterator t = self_loop_transitions.begin(); t != self_loop_transitions.end(); t++)
    if (transitions_.find(*t) != transitions_.end())
    {
      deleteTransition(**t);
      result++;
    }
      
      //if (result!=0)
        // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
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
  
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RD1 (elimination of equal places)...\n");
  int result=0;
  set<pair<string, string> > placePairs;
  set<pair<string, string> > delPairs;
  bool safe = true;
  
  set<Place*> candidates = places_;
  // since we need two places for this rule, we need sufficent candidates
  while ( candidates.size() > 1 )
  {
    Place * p1 = *(candidates.begin());
    // erase candidate from set, so it cannot compared with itself and set gets smaller
    candidates.erase(p1);
    
    if ((p1)->getPostset().size() !=1) //precondition 2
      continue;
    
    if (findArc(*p1,**(p1)->getPostset().begin())->getWeight()!=1) //precondition 3
      continue;
    
    Node* t1 = *(p1)->getPostset().begin();
    
    for (set<Place*>::iterator p2 = candidates.begin(); p2 != candidates.end(); p2++)
    {
      if ((*p2)->getType() != Node::INTERNAL) //precondition 0
        continue;
      
      if(p1 == *p2) // precondition 1
        continue;
      
      if ((*p2)->getPostset().size() !=1) //precondition 2
        continue;
      
      if (findArc(**p2,**(*p2)->getPostset().begin())->getWeight() !=1) //precondition 3
        continue;
      
      Node* t2 = *(*p2)->getPostset().begin();
      
      if (t1 == t2) //precondition 4
        continue;
      // FIXME
      //set<Node*> postSetT1 = postset (t1);
      //set<Node*> postSetT2 = postset (t2);
      
      //if (postSetT1 != postSetT2) //precondition 5
      //  continue;
      
      /* FIXME
      set<Node*> preSetT1 = (t1)->preset;
      set<Node*> preSetT2 = (t2)->preset;
      
      preSetT1.erase(p1);
      preSetT2.erase(*p2);
      
      if (preSetT1 != preSetT2) //precondition 5
        continue;
      */
      
      string id1 = ((p1)->getName());
      string id2 = ((*p2)->getName());
	    
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
   for (set<Place*>::iterator p1 = places_.begin(); p1 != places_.end(); p1++)
   {
     if ((*p1)->getPostset().size() !=1) //precondition 2
       continue;
     
     if (findArc(**p1,**(*p1)->getPostset().begin())->getWeight()!=1) //precondition 3
       continue;
     
     Node* t1= *(*p1)->getPostset().begin();
     
     for (set<Place*>::iterator p2 = places_.begin(); p2 != places_.end(); p2++)
     {
       if ((*p2)->type != INTERNAL) //precondition 0
         continue;
       
       if(*p1 == *p2) // precondition 1
         continue;
       
       if ((*p2)->getPostset().size() !=1) //precondition 2
         continue;
       
       if (findArc(**p2,**(*p2)->getPostset().begin())->getWeight() !=1) //precondition 3
         continue;
       
       Node* t2 = *(*p2)->getPostset().begin();
       
       if (t1 == t2) //precondition 4
         continue;
       
       set<Node*> postSetT1 = getPostset() (t1);
       set<Node*> postSetT2 = getPostset() (t2);
       
       if (postSetT1 != postSetT2) //precondition 5
         continue;
       
       set<Node*> preSetT1 = (t1)->preset;
       set<Node*> preSetT2 = (t2)->preset;
       
       preSetT1.erase(*p1);
       preSetT2.erase(*p2);
       
       if (preSetT1 != preSetT2) //precondition 5
         continue;
       
       string id1 = ((*p1)->getName());
       string id2 = ((*p2)->getName());
       
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
    // trace(TRACE_DEBUG, "[PN]\tFound something to reduce with RD1!\n");
    Place* p1 = findPlace(labels->first);
    Place* p2 = findPlace(labels->second);
    
    string trans_id = ((*(p2->getPostset().begin()))->getName());
    
    unsigned int arcadd = 0;
    
    for (set<Node*>::iterator n = p2->getPreset().begin(); n != p2->getPreset().end(); n++)
    {
      arcadd=0;
      
      // test if there has already been an arc
      if(p1->getPreset().find(*n) != p1->getPreset().end())
      {
        arcadd = arcadd + findArc(**n, *p1)->getWeight();          
        for (set<Arc*>::iterator f = arcs_.begin(); f != arcs_.end(); f++)
          if ((&(*f)->getSourceNode() == *n) || (&(*f)->getTargetNode() == p1))
            ;//FIXME:removeArc(*f);
      }
      
      //FIXME: newArc(*n, p1, STANDARD, (findArc(**n,*p2)->getWeight() + arcadd));
      
    }
    
    //FIXME: p1->tokens = p1->getTokenCount() + p2->getTokenCount();
    
    deletePlace(*p2);
    deleteTransition(*findTransition(trans_id)); 
    result++;   
  }
  //if (result!=0)
    // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
}





/*!
 * \brief remove unneeded initially marked places in choreographies
 *
 * \todo comment me!
 */
void PetriNet::reduce_remove_initially_marked_places_in_choreographies()
{
  // trace(TRACE_DEBUG, "[PN]\tApplying rule \"Elimination of unnecessary initial places\"...\n");
  
  set<Place*> redundant_places;
  
  // traverse the places
  for (set<Place *>::const_iterator place = places_.begin();
       place != places_.end();
       place++)
  {
    // find initial places with empty preset and singleton postset
    if ( (*place)->getTokenCount() == 1  &&
         (*place)->getPreset().empty() &&
         (*place)->getPostset().size() == 1 )
    {
      //Transition *post_transition = static_cast<Transition *> (*((*place)->getPostset().begin()));
      
      /* FIXME:
      // if the transition in the postset is executed exactly once...
      if ( post_transition->max_occurrences == 1 )
      {
        for (set<Node *>::const_iterator pre_place = post_transition->getPreset().begin();
             pre_place != post_transition->getPreset().end();
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
      */
    }
  }
  
  
  // remove the redundant places
  for (set<Place*>::const_iterator place = redundant_places.begin();
       place != redundant_places.end();
       place++)
  {
    deletePlace(**place);
  }
  
  //if (!redundant_places.empty())
    // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(redundant_places.size()) + " places.\n");
}


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
 * The implementations of RA1, RA2, RB1 and RB2 accord with Thomas Pillat's 
 * "Gegenüberstellung struktureller Reduktionstechniken für Petrinetze",
 * written 2008-03-31 (Pillat2008).
 *
 * The rules are applied until a fixed point (i.e. the number of places,
 * transitions and arcs does not change) is reached.
 *
 * \return the number of passes until a fixed point was reached
 *
 * \param   keepNormal determines wether reduction of a normalized net should
 *          preserve normalization or not. If true, interface transitions
 *          (i.e. transitions connected with interface places) will only be
 *          merged with internal transitions (i.e. transitions connected
 *          with internal places only). 
 * 
 * \note    reduce_dead_nodes, reduce_unused_status_places and 
 *          reduce_suspicious_transitions temporary won't be invoked 
 *          since they are no murta rules.
 */
unsigned int PetriNet::reduce(unsigned int reduction_level, bool keepNormal)
{
  // trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: " + information() + "\n");
  // trace(TRACE_INFORMATION, "Simplifying Petri net...\n");
  
  //string old = information();
  bool done = false;
  unsigned int passes = 1;
  
  while (!done)
  {
    if (reduction_level >= 1)
    {
      reduce_dead_nodes();
    }
    
    /* no murata rule
    if (reduction_level >= 2)
    {
#ifdef USING_BPEL2OWFN
      reduce_unused_status_places();
      reduce_suspicious_transitions();
#endif
    }
    //*/
    
    if (reduction_level >= 3)
    {
      reduce_identical_places();	// RB1
      reduce_identical_transitions();	// RB2
    }
    
    if (reduction_level >= 4)
    {
      reduce_series_places();		// RA1
      reduce_series_transitions(keepNormal);	// RA2
    }
    
    if (reduction_level == 5)
    {
      reduce_self_loop_places();	// RC1
      reduce_self_loop_transitions();	// RC2
      // no murata rule
      // reduce_remove_initially_marked_places_in_choreographies();
    }
    
    /* no murata rule
    if (reduction_level == 6)
    {
      reduce_equal_places();		// RD1
    }
    //*/
    
    
    // trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification pass " + toString(passes++) + ": " + information() + "\n");
    
    //done = (old == information());
    //old = information();
  }
  
  
  // trace(TRACE_INFORMATION, "Simplifying complete.\n");
  // trace(TRACE_DEBUG, "[PN]\tPetri net size after simplification: " + information() + "\n");
  
  return passes;
}


/****************************************************
 * Backup
 ****************************************************/


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
/*
void PetriNet::reduce_identical_places()
{
  int result=0;
  set<pair<string, string> > placePairs;
  
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RB1 (elimination of identical places)...\n");
  
  // iterate the places
  for (set<Place*>::iterator p1 = places_.begin(); p1 != places_.end(); p1++)
  {
    if (((*p1)->getPreset().empty()) || ((*p1)->getPostset().empty()) || !(sameweights(*p1)))
      continue;
    
    for (set<Node*>:: iterator preTransition = (*p1)->getPreset().begin(); preTransition != (*p1)->getPreset().end(); preTransition++)
    {
      for (set<Node*>::iterator p2 = (*preTransition)->getPostset().begin(); p2 != (*preTransition)->getPostset().end(); p2++)
        if ((*p1 != *p2) &&     // precondition 1
            ((*p1)->getPreset() == (*p2)->getPreset()) && // precondition 2
            ((*p1)->getPostset() == (*p2)->getPostset()) && // precondition 3
            (sameweights(*p2)) && // precondition 4
            (findArc(*(*preTransition), (**p1))->getWeight() == findArc(*(*p2), *(*((*p1)->getPostset().begin())))->getWeight()) ) // precondition 4
        {
          string id1 = ((*p1)->getName());
          string id2 = ((*p2)->getName());
          placePairs.insert(pair<string, string>(id1, id2));
        }
    }
  }
    
    // trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(placePairs.size()) + " places with same preset and postset...\n");
    
    // merge the found places
    for (set<pair<string, string> >::iterator labels = placePairs.begin();
         labels != placePairs.end(); labels++)
    {
      Place* p1 = findPlace(labels->first);
      Place* p2 = findPlace(labels->second);
      mergePlaces(p1, p2);
      result++;
    }
    //if (result!=0)
      // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
}
//*/

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
/*
void PetriNet::reduce_identical_transitions()
{
  int result=0;
  set<pair<string, string> > transitionPairs;
  
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RB2 (elimination of identical transitions)...\n");
  
  // iterate the transitions
  for (set<Transition*>::iterator t1 = transitions_.begin(); t1 != transitions_.end(); t1++)
  {
    if (!(sameweights(*t1)))
      continue;
    
    for (set<Node*>:: iterator prePlace = (*t1)->getPreset().begin(); prePlace != (*t1)->getPreset().end(); prePlace++)
    {
      for (set<Node*>::iterator t2 = (*prePlace)->getPostset().begin(); t2 != (*prePlace)->getPostset().end(); t2++)
        if ((*t1 != *t2) &&     // precondition 1
            ((*t1)->getPreset() == (*t2)->getPreset()) && // precondition 2
            ((*t1)->getPostset() == (*t2)->getPostset()) && // precondition 3
            (sameweights(*t2)) && // precondition 4
            (findArc(*(*((*t1)->getPreset().begin())),*(*t1))->getWeight() == findArc(*(*t2), *(*((*t1)->getPostset().begin())))->getWeight())) // precondition 4
        {
          string id1 = ((*t1)->getName());
          string id2 = ((*t2)->getName());
          transitionPairs.insert(pair<string, string>(id1, id2));
        }
    }
  }
    
    // trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(transitionPairs.size()) + " transitions with same preset and postset...\n");
    
    // merge the found transitions
    for (set<pair<string, string> >::iterator labels = transitionPairs.begin();
         labels != transitionPairs.end(); labels++)
    {
      Transition* t1 = findTransition(labels->first);
      Transition* t2 = findTransition(labels->second);
      
      mergeParallelTransitions(t1, t2);
      result++;
    }
    //if (result!=0)
      // trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
}
//*/

