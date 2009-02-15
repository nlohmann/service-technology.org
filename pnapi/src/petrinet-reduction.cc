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
 *          Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: \$Author$
 *
 * \since   2006-03-16
 *
 * \date    \$Date$
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 *          This file has been refactured according to 
 *          "Peter H. Starke - Analyse von Petri-Netz-Modellen" ([STA90]).
 *          Each reduction rule now refers to the appropriate rule in
 *          chapter 9 of [STA90], except those functions, without an
 *          appropriate rule in [STA90]. Due to this refactoring some 
 *          functions have been replaced.
 *          
 *          In detail:
 *          - reduce_identical_places() is now covered by reduce_rule_3p()
 *          - reduce_identical_transitions() is now covered by reduce_rule_3p()
 *          - reduce_series_places() is now covered by reduce_rule_9()
 *          - reduce_series_transitions() is now covered by reduce_rule_6()
 *          - reduce_self_loop_places() is now covered by reduce_rule_7()
 *          - reduce_self_loop_transitions() is now covered by reduce_rule_8()
 *          - reduce_equal_places() is now covered by reduce_rule_4()
 * 
 *          According to [STA90] these rules preserve boundedness and liveness.
 * 
 * \version \$Revision$
 *
 * \ingroup petrinet
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cassert>
#include <iostream>
#include <list>
#include <set>
#include <utility>

#include "petrinet.h"
#include "util.h"		// helper functions (toString, setUnion)
//#include "debug.h"		// (trace)

using std::cerr;
using std::endl;
using std::list;
using std::pair;
using std::set;

using namespace pnapi;





/******************************************************************************
 * Preserved functions to remove unnecessary nodes
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



/******************************************************************************
 * Functions to structurally simplify the Petri net model according to [STA90]
 *****************************************************************************/

/*!
 * \brief Elimination of parallel places:
 * 
 * If there exist two parallel places p1 and p2 (precondition 1)
 * and p1 has less than or as many tokens as p2 (precondition 2),
 * then p2 can be removed.
 * If precondition 2 is not fulfilled, p1 will be removed.
 * 
 * A place is removed by merging its history with this one of 
 * place parallel to it.
 * 
 * \return  Number of removed places.
 * 
 * \note  This implementation will not affect isolated places. 
 * 
 */
int PetriNet::reduce_rule_3p()
{
  set<Place*> obsoletePlaces, seenPlaces;
  map<Place*, Place*> replaceRelation;
  
  // iterate internal places
  for (set<Place*>::iterator p1 = internalPlaces_.begin(); 
       p1 != internalPlaces_.end(); ++p1)
  { 
    /*
     * Since parallel places form a equivalence class, each place once
     * identified as parallel to an other place can be ignored.
     */
    if(seenPlaces.find(*p1)!=seenPlaces.end())
      continue;
    
    // STEP 1: Get parallel places.
    set<Place*> parallelPlaces;
    
    // get a transition of the preset
    Node* preTransition = *((*p1)->getPreset().begin());
    if(preTransition != 0)
    {
      for (set<Node*>::iterator p2 = preTransition->getPostset().begin(); 
           p2 != preTransition->getPostset().end(); ++p2)
      {
        if ((*p1)->isParallel(*(*p2))) // precondition 1
        {
          parallelPlaces.insert(static_cast<Place*>(*p2));
        }
      }
    }
    
    if((*p1)->getPreset().empty())
    {
      // get a transition of the postset
      Node* postTransition = *((*p1)->getPostset().begin());
      if(postTransition != 0)
      {
        for (set<Node*>::iterator p2 = postTransition->getPreset().begin(); 
             p2 != postTransition->getPreset().end(); ++p2)
          if ((*p1)->isParallel(*(*p2))) // precondition 1
          {
            parallelPlaces.insert(static_cast<Place*>(*p2));
          }
      }
    }
    
    /*
     * STEP 2: Get minimal marked place and mark all other 
     * places as obsolete (precondition 2)
     */
    
    Place* minPlace = *p1;
    unsigned int minMark = minPlace->getTokenCount(); 
    for(set<Place*>::iterator p=parallelPlaces.begin(); 
        p!=parallelPlaces.end(); ++p)
    {
      if ((*p)->getTokenCount() < minMark)
      {
        minMark = (*p)->getTokenCount();
        minPlace = *p;
      }
      seenPlaces.insert(*p); // mark places as seen
    }
    parallelPlaces.insert(*p1);
    for(set<Place*>::iterator p=parallelPlaces.begin(); 
        p!=parallelPlaces.end(); ++p)
    {
      if((*p) != minPlace)
      {
        obsoletePlaces.insert(*p);
        replaceRelation[*p]=minPlace;
      }
    }
  }
  
  // STEP 3: remove obsolete places
  int erg=0;
  for (set<Place*>::iterator p1 = obsoletePlaces.begin();
       p1 != obsoletePlaces.end(); ++p1)
  {
    Place* p2 = replaceRelation[*p1];
    p2->mergeNameHistory(**p1);
    deletePlace(**p1);
    ++erg;
  }
  
  return erg;
}


/*!
 * \brief Elimination of parallel transitions:
 * 
 * If there exist two parallel transitions t1 and t2 (precondition 1)
 * one of them can be removed.
 *  
 * A transition is removed by merging its history with this one of 
 * transition parallel to it.
 * 
 * \return  Number of removed transitions. 
 * 
 */
int PetriNet::reduce_rule_3t()
{
  set<Transition*> obsoleteTransitions, seenTransitions;
  map<Transition*, Transition*> replaceRelation;
  
  // iterate transitions
  for (set<Transition*>::iterator t1 = transitions_.begin(); 
       t1 != transitions_.end(); ++t1)
  { 
    /*
     * Since parallel transitions form a equivalence class, 
     * each transition once identified as parallel to 
     * an other transition can be ignored.
     */
    if(seenTransitions.find(*t1)!=seenTransitions.end())
      continue;
    
    // STEP 1: Get parallel transitions.
    
    // get a place of the preset
    Node* prePlace = *((*t1)->getPreset().begin());
    if(prePlace != 0)
    {
      for (set<Node*>::iterator t2 = prePlace->getPostset().begin(); 
           t2 != prePlace->getPostset().end(); ++t2)
      {
        if ((*t1)->isParallel(*(*t2))) // precondition 1
        {
          seenTransitions.insert(static_cast<Transition*>(*t2)); // mark transition as seen
          obsoleteTransitions.insert(static_cast<Transition*>(*t2));
          replaceRelation[static_cast<Transition*>(*t2)]=*t1;
        }
      }
    }
    
    if((*t1)->getPreset().empty())
    {
      // get a place of the postset
      Node* postPlace = *((*t1)->getPostset().begin());
      if(postPlace != 0)
      {
        for (set<Node*>::iterator t2 = postPlace->getPreset().begin(); 
             t2 != postPlace->getPreset().end(); ++t2)
          if ((*t1)->isParallel(*(*t2))) // precondition 1
          {
            seenTransitions.insert(static_cast<Transition*>(*t2)); // mark transition as seen
            obsoleteTransitions.insert(static_cast<Transition*>(*t2));
            replaceRelation[static_cast<Transition*>(*t2)]=*t1;
          }
      }
    }
  }
  
  // STEP 2: remove obsolete places
  int erg=0;
  for (set<Transition*>::iterator t1 = obsoleteTransitions.begin();
       t1 != obsoleteTransitions.end(); ++t1)
  {
    Transition* t2 = replaceRelation[*t1];
    t2->mergeNameHistory(**t1);
    deleteTransition(**t1);
    ++erg;
  }
  
  return erg;
  
}


/*!
 * \brief Check whether transitions t1 and t2 are connected to all
 *        places in the same way, except the given places p1 and p2.
 */
bool PetriNet::reduce_isEqual(Transition* t1, Transition* t2, Place* p1, Place* p2)
{
  // check for equal presets and postsets
  set<Node*> places, pre1, pre2;
  places.insert(p1);
  places.insert(p2);
  pre1 = util::setDifference(t1->getPreset(),places);
  pre2 = util::setDifference(t2->getPreset(),places);
  
  if( (pre1 != pre2) ||
      (t1->getPostset() != t2->getPostset()) )
    return false;
  
  // check for arc weights
  // iterate the preset
  for (set<Node*>::iterator p = t1->getPreset().begin();
       p != t1->getPreset().end(); ++p)
  {
    if (*p == p1)
      continue;
    if(findArc(**p,*t1)->getWeight() != findArc(**p,*t2)->getWeight())
      return false;
  }
  
  // iterate the postset
  for (set<Node*>::iterator p = t1->getPostset().begin();
         p != t1->getPostset().end(); ++p)
  {
    if(findArc(*t1,**p)->getWeight() != findArc(*t2,**p)->getWeight())
      return false;
  }
  
  return true;
}


/*!
 * \brief Elimination of equal places:
 * 
 * If there exist two distinct places p1 and p2 (precondition 1)
 * with singleton postset t1 and t2 respectively (precondition 2)
 * connected an arc weight of 1 respectively (precondition 3)
 * and both transitions are connected with all places different
 * from p1 and p2 in the same way (precondition 4)
 * and the preset of p1 and p2 respectively is not empty (precondition 5),
 * then the following changes can be applied:
 * 1.: For each transition t in the preset of p2 add an arc
 *     of the same weight to p1. If such an arc already exist,
 *     increase its weight by the weight of the arc from t to p2.
 * 2.: Increase the amount of tokens stored at p1 
 *     by the amount of tokens stored at p2.
 * 3.: Remove p2 and t2.  
 *  
 * The history of p2 will be stored in p1 and the history of
 * t2 will be stored in t1.
 * 
 * \return  Number of removed places (which equals the number 
 *          of removed transitions). 
 * 
 */
int PetriNet::reduce_rule_4()
{
  /* 
   * STEP 1:  get places with singleton postset which is connected
   *          with arc weight 1
   */
  set<Place*> relevantPlaces;
  
  // iterate internal places
  for (set<Place*>::iterator p = internalPlaces_.begin(); 
       p != internalPlaces_.end(); ++p)
  {
    if(( (*p)->getPostset().size()==1) && // precondition 2
       ( (*((*p)->getPostsetArcs().begin()))->getWeight()==1 ) ) // precondition 3
    {
      relevantPlaces.insert(*p);
    }
  }
  
  // STEP 2: check those places for equality
  set<pair<Place*,Place*> > equalPlaces;
  
  for (set<Place*>::iterator p1 = relevantPlaces.begin();
       p1 != relevantPlaces.end(); ++p1)
  {
    for (set<Place*>::iterator p2 = (++p1)--; // precondition 1
           p2 != relevantPlaces.end(); ++p2)
    {
      Transition* t1 = static_cast<Transition*>(*((*p1)->getPostset().begin()));
      Transition* t2 = static_cast<Transition*>(*((*p2)->getPostset().begin()));
      if(reduce_isEqual(t1,t2,*p1,*p2))
      {
        equalPlaces.insert(pair<Place*,Place*>(*p1,*p2));
      }
    }
  }
  
  // STEP 3: actual reduction
  int count=0;
  
  for(set<pair<Place*,Place*> >::iterator equals = equalPlaces.begin();
      equals != equalPlaces.end(); ++equals)
  {
    // STEP 3.1: set arcs
    Place* p1 = (*equals).first;
    Place* p2 = (*equals).second;
    
    for(set<Arc*>::iterator a = p2->getPresetArcs().begin();
        a != p2->getPresetArcs().end(); ++a)
    {
      int weight = (*a)->getWeight();
      Arc* a2 = findArc((*a)->getSourceNode(),*p1);
      if(a2!=0)
      {
        weight += a2->getWeight();
        deleteArc(*a2);
      }
      createArc((*a)->getSourceNode(),*p1,weight);
    }
    
    // STEP 3.2: set marking
    p1->mark(p1->getTokenCount()+p2->getTokenCount());
    
    // save history
    p1->mergeNameHistory(*p2);
    (*(p1->getPostset().begin()))->mergeNameHistory(*(*(p1->getPostset().begin())));
    
    // STEP 3.3: remove place and transition
    deleteTransition(*(static_cast<Transition*>(*(p2->getPostset().begin()))));
    deletePlace(*p2);
    ++count;
  }
  
  return count;
}

/*!
 * \brief Checks, wether the postset of a set of nodes is empty.
 */
bool PetriNet::reduce_emptyPostset(const set<Node*> & nodes)
{
  for(set<Node*>::iterator n = nodes.begin();
      n != nodes.end(); ++n)
  {
    if(!((*n)->getPostset().empty()))
      return false;
  }
  return true;
}

/*!
 * \brief Check if the preset of a set stores only one item.
 */
bool PetriNet::reduce_singletonPreset(const set<Node*> & nodes)
{
  for(set<Node*>::iterator n = nodes.begin();
      n != nodes.end(); ++n)
  {
    if((*n)->getPreset().size() != 1)
      return false;
  }
  return true;
}

/*!
 * \brief Fusion of transitions:
 * 
 * If there exist a place p with a not empty preset t1 to tk (precondition 1),
 * a not empty postset t1' to tn' (precondition 2)
 * which are distinct (precondition 3)
 * and the postset's postset is not empty (precondition 4)
 * and the postsets preset is {p} (precondition 5)
 * and the arc weight from p to each transition of its postset is v (precondition 6)
 * 
 * and
 * 
 * case a: n == 1 (i.e. there exists only a singleton postset of p) (precondition 7a)
 *         and the weight of all ingoing arcs of p are multiples of v (precondition 8a)
 * 
 * or
 * 
 * case b: n > 1 (precondition 7b),
 *         p stores less than v tokens (precondition 8b)
 *         and the weight of each ingoing arc of p is v (precondition 9b)
 * 
 * then the following changes can be applied:
 * 
 * 1.: If p stores at least v tokens, fire its posttransition until
 *     p stores less than v tokens.
 * 2.: For each transition ti (1 <= i <= k) 
 *     and each transition tj' (1 <= j <= n)
 *     add a new transition tij so that firing of transition tij 
 *     replaces fireing of transition ti followed by fireing of tj'.
 * 3.: Remove nodes p, t1 to tk and t1' to tn'.   
 * 
 * Tij will store the history of ti, p and tj'.
 * 
 * \return  Number of removed places. 
 * 
 */
int PetriNet::reduce_rule_5()
{
  // search for places fullfilling the preconditions
  set<Place*> obsoletePlaces;
  map<Node*,unsigned int> v_;
  
  // transitions must not be in more than one reduction at once
  map<Node*,bool> seenTransitions;
  for(set<Transition*>::iterator t = transitions_.begin();
      t != transitions_.end(); ++t)
    seenTransitions[*t]=false;
  
  // iterate internal places
  for (set<Place*>::iterator p = internalPlaces_.begin(); 
       p != internalPlaces_.end(); ++p)
  {
    set<Node*> preset = (*p)->getPreset();
    set<Arc*> preArcs = (*p)->getPresetArcs();
    set<Node*> postset = (*p)->getPostset();
    set<Arc*> postArcs = (*p)->getPostsetArcs();
    
    // check for seen transitions
    {
      bool seen = false;
      for(set<Node*>::iterator t = preset.begin();
          t != preset.end(); ++t)
        seen = (seen || seenTransitions[*t]);
      for(set<Node*>::iterator t = postset.begin();
          t != postset.end(); ++t)
        seen = (seen || seenTransitions[*t]);
      if(seen)
        continue;
    }
    
    if((preset.empty()) || // precondition 1
       (postset.empty()) || // precondition 2
       (!((util::setIntersection(preset,postset)).empty())) || // precondition 3
       (reduce_emptyPostset(postset)) || // precondition 4
       (!(reduce_singletonPreset(postset))) ) // precondition 5
      continue;
    
    unsigned int v = (*(postArcs.begin()))->getWeight();
    bool precond_6 = true;
    
    for(set<Arc*>::iterator a = postArcs.begin();
        a != postArcs.end(); ++a)
    {
      if((*a)->getWeight() != v)
      {
        precond_6 = false;
        break;
      }
    }
    
    if(!precond_6) // precondition 6
      continue;
    
    if( postset.size() == 1 ) // case a, precondition 7a
    {
      bool precond_8a = true;
      for(set<Arc*>::iterator a = preArcs.begin();
          a!=preArcs.end(); ++a)
      {
        if((*a)->getWeight() % v != 0)
        {  
          precond_8a = false;
          break;
        }
      }
      if(precond_8a) // precondition 8a
      {
        obsoletePlaces.insert(*p);
        v_[*p]=v;
        for(set<Node*>::iterator t = preset.begin();
            t != preset.end(); ++t)
          seenTransitions[*t]=true;
        for(set<Node*>::iterator t = postset.begin();
            t != postset.end(); ++t)
          seenTransitions[*t]=true;
      }
    }
    else // case b, precondition 7b
    {
      if((*p)->getTokenCount() >= v) // precondition 8b
        continue;
      
      bool precond_9b = true;
      for(set<Arc*>::iterator a = preArcs.begin();
          a!=preArcs.end(); ++a)
      {
        if((*a)->getWeight() != v)
        {
          precond_9b = false;
          break;
        }
      }
      if(precond_9b) // precondition 9b
      {
        obsoletePlaces.insert(*p);
        v_[*p]=v;
        for(set<Node*>::iterator t = preset.begin();
            t != preset.end(); ++t)
          seenTransitions[*t]=true;
        for(set<Node*>::iterator t = postset.begin();
            t != postset.end(); ++t)
          seenTransitions[*t]=true;
      }
    }
  }
  
  // apply reduction
  int ret = 0;
  
  for(set<Place*>::iterator p = obsoletePlaces.begin();
      p!=obsoletePlaces.end(); ++p)
  {
    // STEP 1
    while((*p)->getTokenCount() > v_[*p]) // if there are too much tokens stored in p
    {
      // fire the only transition in p's postset
      Transition* t = static_cast<Transition*>(*((*p)->getPostset().begin()));
      
      (*p)->mark((*p)->getTokenCount() - v_[*p]); // consume tokens
      
      for(set<Arc*>::iterator a = t->getPostsetArcs().begin();
          a != t->getPostsetArcs().end(); ++a)
      {
        Place* postP = static_cast<Place*>(&((*a)->getTargetNode())); 
        postP->mark(postP->getTokenCount() + (*a)->getWeight()); // produce tokens
      }
    }
    
    // STEP 2
    for(set<Node*>::iterator t1 = (*p)->getPreset().begin();
        t1 != (*p)->getPreset().end(); ++t1)
      for(set<Node*>::iterator t2 = (*p)->getPostset().begin();
          t2 != (*p)->getPostset().end(); ++t2)
      {
        // save the history
        Transition* t12 = &createTransition();
        t12->mergeNameHistory(**t1);
        t12->mergeNameHistory(**p);
        t12->mergeNameHistory(**t2);
        
        // copy incoming arcs
        for(set<Arc*>::iterator a = (*t1)->getPresetArcs().begin();
            a != (*t1)->getPresetArcs().end(); ++a)
          createArc((*a)->getSourceNode(),*t12,(*a)->getWeight());
        
        // arrange outgoing arcs
        set<Node*> newPostset;
        map<Node*,unsigned int> weights;
        
        for(set<Arc*>::iterator a = (*t1)->getPostsetArcs().begin();
            a != (*t1)->getPostsetArcs().end(); ++a)
        {
          if(&((*a)->getTargetNode()) == (*p))
              continue;
          newPostset.insert(&((*a)->getTargetNode()));
          weights[&((*a)->getTargetNode())]+=(*a)->getWeight();
        }
        
        for(set<Arc*>::iterator a = (*t2)->getPostsetArcs().begin();
            a != (*t2)->getPostsetArcs().end(); ++a)
        {
          newPostset.insert(&((*a)->getTargetNode()));
          weights[&((*a)->getTargetNode())] += ((findArc((**t1),(**p))->getWeight()) / v_[*p]) * (*a)->getWeight();
        }
        
        for(set<Node*>::iterator np = newPostset.begin();
            np != newPostset.end(); ++np)
          createArc(*t12,(**np),weights[*np]);
      }
    
    // STEP 3:
    set<Node*> preset = (*p)->getPreset();
    set<Node*> postset = (*p)->getPostset();
    deletePlace(*(*p));
    for(set<Node*>::iterator t = preset.begin();
        t != preset.end(); ++t)
      deleteTransition(*static_cast<Transition*>(*t));
    for(set<Node*>::iterator t = postset.begin();
        t != postset.end(); ++t)
      deleteTransition(*static_cast<Transition*>(*t));
    
    ++ret;
  }
  
  return ret; 
}

/*!
 * \brief Fusion of transitions
 * 
 * If there exists a place p with only one pretransition t (precondition 1)
 * and p is the only postplace of t (precondition 2)
 * and the postset of p is not empty (precondition 3)
 * and t is not in the postset of p (precondition 4)
 * and the postset of the preset of t contains only t (precondition 5)
 * and all arcs to p or from p have the weight v (precondition 6)
 * and the amount of tokens stored in p is less than v (precondition 7)
 * then the following reduction can be applied:
 * 
 * 1.:  For each transition ti' from the postset of p add a new 
 *      transition ti which will be connected with the presets 
 *      of t and ti' and the postset of ti' appropriate.
 * 2.:  Remove p and its pre- and postset.
 * 
 * Ti will store the history of t, p and ti'.
 * 
 * \return  Number of removed places.
 *   
 */
int PetriNet::reduce_rule_6()
{
  // search for places fullfilling the preconditions
  set<Place*> obsoletePlaces;
  
  // transitions must not be in more than one reduction at once
  map<Node*,bool> seenTransitions;
  for(set<Transition*>::iterator t = transitions_.begin();
      t != transitions_.end(); ++t)
    seenTransitions[*t]=false;
  
  // iterate internal places
  for (set<Place*>::iterator p = internalPlaces_.begin(); 
       p != internalPlaces_.end(); ++p)
  {
    
    if ((*p)->getPreset().size() != 1) // precondition 1
      continue;
    
    Transition* t = static_cast<Transition*>(*((*p)->getPreset().begin()));
    
    // check for seen transitions
    {
      bool seen = seenTransitions[t];
      
      for(set<Node*>::iterator n = (*p)->getPostset().begin();
          n != (*p)->getPostset().end(); ++n)
        seen = (seen || seenTransitions[*n]);
      
      if(seen)
        continue;
    }
    
    if( (t->getPostset().size() != 1) || // precondition 2
        ((*p)->getPostset().empty()) || // precondition 3
        ((*p)->getPostset().find(t) != (*p)->getPostset().end()) ) // precondition 4
      continue;
    
    bool precond5 = false;
    
    for(set<Node*>::iterator n = t->getPreset().begin(); 
          n != t->getPreset().end(); ++n)
    {
      if((*n)->getPostset().size() != 1)
      {
        precond5 = true;
        break;
      }
    }
    
    if(precond5) // precondition 5
      continue;
  
    unsigned int v = (*((*p)->getPresetArcs().begin()))->getWeight();
    bool precond6 = false;
    
    for(set<Arc*>::iterator a = (*p)->getPostsetArcs().begin();
          a != (*p)->getPostsetArcs().end(); ++a)
    {
      if((*a)->getWeight() != v)
      {
        precond6 = true;
        break;
      }
    }
    
    if( (precond6) ||  // precondition 6
        ((*p)->getTokenCount() >= v) ) // precondition 7
      continue;
    
    // all preconditions fullfilled
    obsoletePlaces.insert(*p); // mark p as obsolete
    seenTransitions[t] = true; // mark t as seen
    for(set<Node*>::iterator n = (*p)->getPostset().begin();
          n != (*p)->getPostset().end(); ++n)
      seenTransitions[*n] = true; // mark postset as seen
  }
  
  // apply reduction
  int ret = 0;
  
  for(set<Place*>::iterator p = obsoletePlaces.begin();
        p != obsoletePlaces.end(); ++p)
  {
    // STEP 1:
    Transition* t = static_cast<Transition*>(*((*p)->getPreset().begin()));
    
    for(set<Node*>::iterator t__ = (*p)->getPostset().begin();
          t__ != (*p)->getPostset().end(); ++t__)
    {
      Transition* nt = &createTransition(); // get new transition
      nt->mergeNameHistory(*t); // save history
      nt->mergeNameHistory(**p); // save history
      nt->mergeNameHistory(**t__); // save history
      
      // arrange preset
      set<Node*> preset;
      map<Node*,unsigned int> weights;
      
      for(set<Arc*>::iterator a = t->getPresetArcs().begin();
            a != t->getPresetArcs().end(); ++a)
      {
        preset.insert(&((*a)->getSourceNode()));
        weights[&((*a)->getSourceNode())] += (*a)->getWeight();
      }
      
      for(set<Arc*>::iterator a = (*t__)->getPresetArcs().begin();
            a != (*t__)->getPresetArcs().end(); ++a)
      {
        preset.insert(&((*a)->getSourceNode()));
        weights[&((*a)->getSourceNode())] += (*a)->getWeight();
      }
      
      for(set<Node*>::iterator n = preset.begin();
            n != preset.end(); ++n)
      {
        createArc((**n),*nt,weights[*n]);
      }
      
      // copy postset
      for(set<Arc*>::iterator a = (*t__)->getPostsetArcs().begin();
            a != (*t__)->getPostsetArcs().end(); ++a)
        createArc(*nt,(*a)->getTargetNode(),(*a)->getWeight());
    }
    
    // STEP 2:
    deleteTransition(*t);
    
    for(set<Node*>::iterator n = (*p)->getPostset().begin();
          n != (*p)->getPostset().end(); ++n)
      deleteTransition(*static_cast<Transition*>(*n));
    
    deletePlace(**p);
    ++ret;
  }
  
  return ret;
}

/*!
 * \brief Remove loop places.
 * 
 * If there exists a place p with identical preset and postset (precondition 1)
 * and for each transition in the preset of p applies that the
 * arc weight from p to t equals the arc weight from t to p (precondition 2)
 * and this arc weight is less than the amount of tokens stored in p (precondition 3)
 * than this place can be removed as well as each transition 
 * becoming isolated by this reduction.
 * 
 * \return  Number of removed places.
 * 
 * \todo: How to handle the history of removed places and transitions?
 *   
 */
int PetriNet::reduce_rule_7()
{
  // search for places fullfilling the preconditions
  set<Place*> obsoletePlaces;
  
  // iterate internal places
  for (set<Place*>::iterator p = internalPlaces_.begin(); 
       p != internalPlaces_.end(); ++p)
  {
    if(!((*p)->getPreset() == (*p)->getPostset())) // precondition 1
      continue;
    
    unsigned int m = (*p)->getTokenCount();
    bool precond = false;
    
    for(set<Arc*>::iterator a1 = (*p)->getPresetArcs().begin();
          a1 != (*p)->getPresetArcs().end(); ++a1)
    {
      Arc* a2 = findArc((**p),(*a1)->getSourceNode());
      if( ((*a1)->getWeight() != a2->getWeight()) || // precondition 2
          (a2->getWeight() != m) ) // precondition 3
      {
        precond = true;
        break;
      }
    }
    
    if(precond)
      continue;
    
    // p fullfilles the preconditions
    obsoletePlaces.insert(*p);
  }
  
  // apply reduction
  int ret = 0;
  
  set<Transition*> obsoleteTransitions;
  
  for(set<Place*>::iterator p = obsoletePlaces.begin();
        p != obsoletePlaces.end(); ++p)
  {
    // check for isolated transitions
    for(set<Node*>::iterator t = (*p)->getPreset().begin();
          t != (*p)->getPreset().end(); ++t)
      if ( ((*t)->getPreset().size() == 1) &&
           ((*t)->getPostset().size() == 1) )
        obsoleteTransitions.insert(static_cast<Transition*>(*t));
    
    // remove place
    deletePlace(**p);
    ++ret;
  }
  
  // clean transitions
  for(set<Transition*>::iterator t = obsoleteTransitions.begin();
        t != obsoleteTransitions.end(); ++t)
    deleteTransition(**t);
  
  return ret;
}

/*!
 * \brief Elimination of loops.
 * 
 * If there exists a transition t with identical preset and postset (precondition 1)
 * and for each place of the preset of t applies that the arc weight
 * from p to t equals the arc weight from t to p (precondition 2)
 * and there exists an transition t0 different from t (precondition 3)
 * and for eacht place p in the preset of t applies that
 * p is in the preset of t0 (precondition 4)
 * and the arc weight from p to t0 is greater than or equal to
 * the arc weight from p to t (precondition 5)
 * than t can be removed.
 * 
 * \return  Number of removed places.
 * 
 * \todo: How to handle the history of removed transitions?
 *  
 */
int PetriNet::reduce_rule_8()
{
  // search for transitions fullfilling the preconditions
  set<Transition*> obsoleteTransitions;
  
  // don't reduce "backup"-transitions
  map<Node*,bool> seenTransitions;
  
  for (set<Transition*>::iterator t = transitions_.begin(); 
         t != transitions_.end(); ++t)
  {
    seenTransitions[*t] = false;
  }
  
  // iterate transitions
  for (set<Transition*>::iterator t = transitions_.begin(); 
       t != transitions_.end(); ++t)
  {
    // check if t is a "backup"-transition
    if(seenTransitions[*t])
      continue;
    
    if(!((*t)->getPreset() == (*t)->getPostset())) // precondition 1
      continue;
    
    bool precond2 = false;
    
    for(set<Arc*>::iterator a1 = (*t)->getPresetArcs().begin();
          a1 != (*t)->getPresetArcs().end(); ++a1)
    {
      Arc* a2 = findArc((**t),(*a1)->getSourceNode());
      if( ((*a1)->getWeight() != a2->getWeight()) ) // precondition 2
      {
        precond2 = true;
        break;
      }
    }
    
    if(precond2)
      continue;
    
    for (set<Node*>::iterator t0 = (*((*t)->getPreset().begin()))->getPostset().begin(); 
           t0 != (*((*t)->getPreset().begin()))->getPostset().end(); ++t0)
    {
      if((*t)==(*t0)) // precondition 3
        continue;
      
      bool precond45 = false;
      
      for (set<Arc*>::iterator a1 = (*t)->getPresetArcs().begin();
            a1 != (*t)->getPresetArcs().end(); ++a1)
      {
        Arc* a2 = findArc((*a1)->getSourceNode(),(**t0));
        if ( (a2 == 0) || // precondition 4 
             (a2->getWeight() < (*a1)->getWeight()) ) //precondition 5
        {
          precond45 = true;
          break;
        }
      }
      
      if (precond45)
      {
        continue;
      }
      else
      {
        // transition t fullfilles the preconditions
        obsoleteTransitions.insert(*t);
        seenTransitions[*t0] = true; // mark t0 as "backup"-transition
        break; 
      }
    }
  }
  
  // apply reduction
  
  int ret = 0;
  
  for(set<Transition*>::iterator t = obsoleteTransitions.begin();
        t != obsoleteTransitions.end(); ++t)
  {
    deleteTransition(**t);
    ++ret;
  }
  
  return ret;
}

/*!
 * \brief Fusion of Places.
 * 
 * If there exists a transition t with singleton preplace p (precondition 1)
 * and the arc weight from p to t is 1 (precondition 2)
 * and t is the only posttransition of of p (precondition 2a)
 * and the preset of p is not empty (precondition 3)
 * and the postset of t is not empty (precondition 4)
 * and p is not in the postset of t (precondition 5)
 * then the following changes can be applied:
 * 
 * 1.:  Fire t until p stores 0 tokens.
 * 2.:  For each transition t' of the preset of p add a new transition t''
 *      which will be connected with the preset of t' in the same way as t',
 *      and with the postset of t' and t so that fireing of t''
 *      equals fireing of t' once followed by fireing of t until p stores
 *      0 tokens again.
 * 3.:  Remove t, p and the preset of p.
 * 
 * T'' will store the history of t', p and t.
 * 
 * \return  Number of removed places.
 * 
 * \note  Precondition 2a is not from [STA90].
 * 
 */
int PetriNet::reduce_rule_9()
{
  // search for places fullfilling the preconditions
  set<Place*> obsoletePlaces;
  
  // transitions must not be in more than one reduction at once
  map<Node*,bool> seenTransitions;
  for(set<Transition*>::iterator t = transitions_.begin();
      t != transitions_.end(); ++t)
    seenTransitions[*t]=false;
  
  // iterate internal places
  for (set<Place*>::iterator p = internalPlaces_.begin(); 
       p != internalPlaces_.end(); ++p)
  {
    { // check for seen Transitions
      bool seen = seenTransitions[*((*p)->getPostset().begin())];
      for(set<Node*>::iterator t = (*p)->getPreset().begin();
          t != (*p)->getPreset().end(); ++t)
      {
        seen = seen || seenTransitions[*t];
      }
      
      if(seen)
        continue;
    }
    
    if( ((*p)->getPostset().size() != 1) || // precondition 2a
        ((*p)->getPreset().empty()) || // precondition 3
        ((*((*p)->getPostsetArcs().begin()))->getWeight() != 1) ) //precondition 2
      continue;
    
    Transition* t = static_cast<Transition*>(*((*p)->getPostset().begin()));
    
    if( (t->getPreset().size() != 1) || // precondition 1
        (t->getPostset().empty()) || // precondition 4
        (t->getPostset().find(*p) != t->getPostset().end()) ) // precondition 5
      continue;
    
    // p is fullfilling the preconditions
    
    obsoletePlaces.insert(*p);
    seenTransitions[t] = true; // mark t as seen
    for(set<Node*>::iterator ti = (*p)->getPreset().begin();
          ti != (*p)->getPreset().end(); ++ti)
      seenTransitions[*ti] = true; // mark preset of p as seen
  }
  
  // apply reduction
  
  int ret = 0;
  
  for(set<Place*>::iterator p = obsoletePlaces.begin();
        p != obsoletePlaces.end(); ++p)
  {
    // STEP 1:
    Transition* t = static_cast<Transition*>(*((*p)->getPostset().begin()));
    unsigned int tokens = (*p)->getTokenCount();
    
    for(set<Arc*>::iterator a = t->getPostsetArcs().begin();
          a != t->getPostsetArcs().end(); ++a)
    {
      Place* pi = static_cast<Place*>(&((*a)->getTargetNode()));
      pi->mark(pi->getTokenCount() + ((*a)->getWeight() * tokens));
    }
    
    (*p)->mark(0);
    
    // STEP 2:
    // iterate preset of p
    for(set<Node*>::iterator ti = (*p)->getPreset().begin();
          ti != (*p)->getPreset().end(); ++ti)
    {
      Transition* tj = &createTransition(); // create new Transition Tj
      
      // save history
      tj->mergeNameHistory(**ti);
      tj->mergeNameHistory(**p);
      tj->mergeNameHistory(*t);
      
      // copy preset
      for(set<Arc*>::iterator a = (*ti)->getPresetArcs().begin();
            a != (*ti)->getPresetArcs().end(); ++a)
      {
        createArc((*a)->getSourceNode(),*tj,(*a)->getWeight());
      }
      
      // arrange postset
      set<Node*> np;
      map<Node*,unsigned int> weights;
      
      for(set<Arc*>::iterator a = (*ti)->getPostsetArcs().begin();
            a != (*ti)->getPostsetArcs().end(); ++a)
      {
        np.insert(&((*a)->getTargetNode()));
        weights[&((*a)->getTargetNode())] += (*a)->getWeight();
      }
      
      unsigned int weight = findArc(**ti,**p)->getWeight();
      
      for(set<Arc*>::iterator a = t->getPostsetArcs().begin();
            a != t->getPostsetArcs().end(); ++a)
      {
        np.insert(&((*a)->getTargetNode()));
        weights[&((*a)->getTargetNode())] += ( weight * ((*a)->getWeight()) );
      }
      
      for(set<Node*>::iterator n = np.begin();
            n != np.end(); ++n)
      {
        createArc(*tj,**n,weights[*n]);
      }
    }
    
    // STEP 3:
    for(set<Node*>::iterator ti = (*p)->getPreset().begin();
          ti != (*p)->getPreset().end(); ++ti)
      deleteTransition(*static_cast<Transition*>(*ti));
    deleteTransition(*t);
    deletePlace(**p);
    
    ++ret;
  }
  
  return ret;
}


/******************************************************************************
 * Functions to structurally simplify the Petri net model according to [PIL08]
 *****************************************************************************/



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
 * \note  This function is obsolete due to [STA90] rule #3.
 * 
 */
void PetriNet::reduce_identical_places()
{
  
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
 * \note  This function is obsolete due to [STA90] rule #3.
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
      deleteTransition(*t2);
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
 * \note  This function is obsolete due to [STA90] rule #9.
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
    Place & p2 = *findPlace(placePair->second);
    findPlace(placePair->first)->merge(p2);
    deletePlace(p2);
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
 * \note  This function is obsolete due to [STA90] rule #5 and #6.
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
	deleteTransition(*t2);
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
 * \note  This function is obsolete due to [STA90] rule #7.
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
 * \note  This function is obsolete due to [STA90] rule #8.
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
 * \note  This function is obsolete due to [STA90] rule #4.
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
 * Removes unnecessary nodes of the generated Petri net:
 *  - structural dead nodes
 *  - unused status places
 *  - suspicious transitions
 *
 * Applys some simple structural reduction rules for Petri nets:
 *  - elimination of parallel places (rule_3p)
 *  - elimination of parallel transitions (rule_3t)
 *  - elimination of equal places (rule_4)
 *  - fusion of series places (rule_9)
 *  - fusion of series transitions (rule_5 and rule_6)
 *  - elimination of self-loop places (rule_7)
 *  - elimination of self-loop transitions (rule_8)
 * 
 * Functions are named by the appropriate rule in [STA90].
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
    
    if (reduction_level >= 2)
    {
#ifdef USING_BPEL2OWFN
      reduce_unused_status_places();
      reduce_suspicious_transitions();
#endif
    }
    
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
