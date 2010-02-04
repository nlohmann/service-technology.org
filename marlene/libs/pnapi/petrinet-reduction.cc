/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

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
 *          last changes of: \$Author: cas $
 *
 * \since   2006-03-16
 *
 * \date    \$Date: 2010-01-22 00:23:39 +0100 (Fri, 22 Jan 2010) $
 *
 * \note    This file is part of the tool GNU BPEL2oWFN and was created during
 *          the project Tools4BPEL at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 *          This file is divided in 3 sections:
 *          Section 1 contains rules to remove unnecessary nodes, i.e.
 *          unused status places, transitions without preset or postset,
 *          dead nodes or initial marked places in choreographies.
 *          Section 2 contains rules from  
 *          "Peter H. Starke - Analyse von Petri-Netz-Modellen" ([Sta90]).
 *          These rules preserve lifeness and boundedness,
 *          but no k-boundedness.
 *          Section 3 contains rules from "Thomas Pillat - 
 *          Gegenüberstellung struktureller Reduktionstechniken
 *          für Petrinetze" ([Pil08])". These rules preserve lifeness and
 *          k-boundedness.         
 * 
 * \version \$Revision: 5258 $
 *
 * \ingroup petrinet
 * 
 * \todo    Check, if rules still can be applied by precondition 
 *          "place is empty in each final marking" and add new places
 *          to the set of implied empty places (and also to the final condition).
 */



/******************************************************************************
 * Headers
 *****************************************************************************/

#include "config.h"
#include <cassert>

#include <iostream>
#include <list>
#include <set>
#include <utility>

#include "petrinet.h"
#include "util.h"		// helper functions (toString, setUnion, foreach)
//#include "debug.h"		// (trace)

using std::cerr;
using std::cout;
using std::flush;
using std::endl;
using std::list;
using std::pair;
using std::set;

using namespace pnapi;


#define __REDUCE_CHECK_FINAL(x) (reducablePlaces_->count(x) > 0)


/******************************************************************************
 * Functions to remove unnecessary nodes
 *****************************************************************************/

/*!
 * Remove status places that are not read by any transition. These places are
 * usually the result of reduction rules applied before. The places are not
 * "read" by any transition, thus can be removed if the are not concerned by a
 * final condition.
 *
 * \note This rule is BPEL2oWFN specific.
 *
 * \return number of removed places
 *
 * \pre p is an internal place
 * \pre p's postset is empty
 * \pre p is not initially marked
 * \pre p is not concerned by a final condition
 *
 * \post p is removed
 * 
 */
unsigned int PetriNet::reduce_unused_status_places()
{
  //trace(TRACE_DEBUG, "[PN]\tReducing unused places...\n");
  set<Place *> unused_status_places;
  unsigned int result = 0;

  // find unused status places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
    if ( ((*p)->getPostset().empty()) && // precondition 2
        __REDUCE_CHECK_FINAL(*p) && // precondition 4
        ((*p)->getTokenCount() == 0 ) ) // precondition 3
      unused_status_places.insert(*p);

  // remove unused places
  PNAPI_FOREACH(set<Place*>, unused_status_places, p)
  {
    deletePlace(**p);
    ++result;    
  }

  /*
  if (result!=0)
  {
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
  } 
  //*/

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
 * \pre t is a transition of the net
 * \pre t's preset or postset empty
 * \pre t is not synchronized
 *
 * \post t is removed
 */
unsigned int PetriNet::reduce_suspicious_transitions()
{
  // trace(TRACE_DEBUG, "[PN]\tReducing suspicious transitions...\n");
  set<Transition*> suspiciousTransitions;
  unsigned int result = 0;

  // find suspicious transitions
  PNAPI_FOREACH(set<Transition*>, transitions_, t)
  {
    if ((*t)->isSynchronized())
      continue;
    if ( (*t)->getPostset().empty() || 
        (*t)->getPreset().empty() )
      suspiciousTransitions.insert(*t);
  }

  // remove suspicious transitions
  PNAPI_FOREACH(set<Transition*>, suspiciousTransitions, t)
  {
    deleteTransition(**t);
    ++result;
  }

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
  //*/

  return result;
}





/*!
 * Remove structural dead nodes.
 * 
 * If there exists a place p with empty preset (precondition 1)
 * and for each transition t in its postset applies,
 * that the arc weight from p to t is higher than 
 * the amount of tokens stored in p (precondition 2)
 * and p is not concerned by a final condition (precondition 3)
 * and t is not synchronized (precondition 4)
 * then this place and its postset can be removed. 
 *
 * \post  p and its postset are removed 
 * 
 * \return  number of removed nodes
 * 
 */
unsigned int PetriNet::reduce_dead_nodes()
{
  // trace(TRACE_DEBUG, "[PN]\tRemoving structurally dead nodes...\n");
  unsigned int result = 0;
  bool done = false;

  while (!done)
  {
    done = true;

    set<Place*> deadPlaces;
    set<Transition*> deadTransitions;

    // find insufficiently marked places with empty preset
    PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
    {
      if ( ((*p)->getPreset().empty()) && // precondition 1 
          __REDUCE_CHECK_FINAL(*p) ) // precondition 3
      {
        // check the postset
        bool arcs=true;		

        PNAPI_FOREACH(set<Arc*>, (*p)->getPostsetArcs(), a)
        {
          if( ((*a)->getWeight() <= (*p)->getTokenCount()) || // precondition 2
              ((*a)->getTransition().isSynchronized()) ) // precondition 4
          {
            arcs=false; // there exists a transition that can fire or is synchronized
          }
        }
        if(arcs) 
        {
          deadPlaces.insert(*p); // p is a dead place

          // transitions in the postset of a dead place are dead
          PNAPI_FOREACH(set<Node*>, (*p)->getPostset(), t)
          {
            deadTransitions.insert(static_cast<Transition*>(*t));
            // trace(TRACE_VERY_DEBUG, "[PN]\tTransition t" + toString((*t)->id) + " is structurally dead\n");
          }

          // trace(TRACE_VERY_DEBUG, "[PN]\tPlace p" + toString((*p)->id) + " is structurally dead.\n");
          done = false; // repeat search for dead nodes once more
        }
      }
    }

    // remove dead places and transitions
    PNAPI_FOREACH(set<Place*>, deadPlaces, p)
    {
      deletePlace(**p);
      ++result;
    }   

    PNAPI_FOREACH(set<Transition*>, deadTransitions, t)
    {
      deleteTransition(**t);
      ++result;
    }
  }

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " nodes.\n");
  //*/

  return result;
}




/*!
 * \brief remove unneeded initially marked places in choreographies
 * 
 * \return  Number of reduced places.
 *
 * \note  This function is useful in BPEL2oWFN but need to be fixed.
 *        Hence this rule is temporally removed.
 * 
 * \todo rewrite me, comment me and add synchronisation test!
 */
unsigned int PetriNet::reduce_remove_initially_marked_places_in_choreographies()
{
  /*

  // trace(TRACE_DEBUG, "[PN]\tApplying rule \"Elimination of unnecessary initial places\"...\n");

  set<Place*> redundant_places;

  // iterate the places
  for (set<Place *>::iterator p = internalPlaces_.begin();
       p != internalPlaces_.end(); ++p)
  {
    // find initial places with empty preset and singleton postset
    if ( (*p)->getTokenCount() == 1  &&
         (*p)->getPreset().empty() &&
         (*p)->getPostset().size() == 1 )
    {
      //Transition *post_transition = static_cast<Transition *> (*((*place)->getPostset().begin()));

      // FIXME:
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

    }
  }


  // remove the redundant places
  unsigned int result = 0;

  for (set<Place*>::const_iterator place = redundant_places.begin();
       place != redundant_places.end();
       place++)
  {
    deletePlace(**place);
    ++result;
  }

  //*/

  /*
  if (!redundant_places.empty())
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(redundant_places.size()) + " places.\n");
  //*/

  // return result;

  return 0;
}



/******************************************************************************
 * Functions to structurally simplify the Petri net model according to [Sta90]
 *****************************************************************************/

/*!
 * \brief Elimination of parallel places:
 * 
 * If there exist two parallel places p1 and p2 (precondition 1)
 * and both p1 and p2 are internal places (precondition 4)
 * and p1 has less than or as many tokens as p2 (precondition 2),
 * and p2 is not concerned by a final condition (precondition 3),
 * then p2 can be removed.
 * If precondition 2 is not fulfilled and p1 is not 
 * concerned by a final condition (precondition 3), 
 * p1 will be removed.
 * 
 * A place is removed by merging its history with this one of 
 * place parallel to it.
 * 
 * \post  this rule preserves lifeness and boundedness according to [Sta90]
 * 
 * \return  number of removed places
 * 
 * \note  This implementation will not affect isolated places. 
 * 
 */
unsigned int PetriNet::reduce_rule_3p()
{
  set<Place*> obsoletePlaces, seenPlaces;
  map<Place*, Place*> replaceRelation;

  // iterate internal places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p1) // precondition 4 for p1
  { 
    /*
     * Since parallel places form an equivalence class, each place once
     * identified as parallel to an other place can be ignored.
     */
    if(seenPlaces.find(*p1)!=seenPlaces.end())
      continue;

    // STEP 1: Get parallel places.
    set<Place*> parallelPlaces;

    // get a transition of the preset
    Node* preTransition = *((*p1)->getPreset().begin());
    if(preTransition != NULL)
    {
      PNAPI_FOREACH(set<Node*>, preTransition->getPostset(), p2)
      {
        if ( ((*p2)->getType() == Node::INTERNAL) && // precondition 4 for p2
             ((*p1)->isParallel(*(*p2))) ) // precondition 1
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
        PNAPI_FOREACH(set<Node*>, postTransition->getPreset(), p2)
          if ( ((*p2)->getType() == Node::INTERNAL) && // precondition 4 for p2
               ((*p1)->isParallel(*(*p2))) ) // precondition 1
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
    
    PNAPI_FOREACH(set<Place*>, parallelPlaces, p)
    {
      if ((*p)->getTokenCount() < minMark)
      {
        minMark = (*p)->getTokenCount();
        minPlace = *p;
      }
      seenPlaces.insert(*p); // mark places as seen
    }
    parallelPlaces.insert(*p1);
    PNAPI_FOREACH(set<Place*>, parallelPlaces, p)
    {
      if( ((*p) != minPlace) &&
          __REDUCE_CHECK_FINAL(*p) ) // precondition 3
      {
        obsoletePlaces.insert(*p);
        replaceRelation[*p]=minPlace;
      }
    }
  }

  // STEP 3: remove obsolete places
  unsigned int result=0;
  PNAPI_FOREACH(set<Place*>, obsoletePlaces, p1)
  {
    Place* p2 = replaceRelation[*p1];
    p2->mergeNameHistory(**p1);
    deletePlace(**p1);
    ++result;
  }

  return result;
}


/*!
 * \brief Elimination of parallel transitions:
 * 
 * If there exist two parallel transitions t1 and t2 (precondition 1)
 * one of them can be removed.
 * Synchronized transitions will not be removed. (precondition 2)
 *  
 * A transition is removed by merging its history with this one of
 * the transition parallel to it.
 * 
 * \post  this rule preserves lifeness and boundedness according to [Sta90]
 * 
 * \return  number of removed transitions 
 * 
 * \note synchronisation test may be implemented properly.
 * 
 */
unsigned int PetriNet::reduce_rule_3t()
{
  set<Transition*> obsoleteTransitions, seenTransitions;
  map<Transition*, Transition*> replaceRelation;

  // iterate transitions
  PNAPI_FOREACH(set<Transition*>, transitions_, t1)
  { 
    /*
     * Since parallel transitions form an equivalence class, 
     * each transition once identified as parallel to 
     * an other transition can be ignored.
     */
    if(seenTransitions.find(*t1)!=seenTransitions.end())
      continue;

    // STEP 1: Get parallel transitions.

    // get a place of the preset
    Node* prePlace = *((*t1)->getPreset().begin());
    if(prePlace != NULL)
    {
      PNAPI_FOREACH(set<Node*>, prePlace->getPostset(), t2)
      {
        if ( ((*t1)->isParallel(*(*t2))) && // precondition 1
            (!(static_cast<Transition*>(*t2)->isSynchronized())) ) // precondition 2
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
      if(postPlace != NULL)
      {
        PNAPI_FOREACH(set<Node*>, postPlace->getPreset(), t2)
          if ((*t1)->isParallel(*(*t2))) // precondition 1
          {
            seenTransitions.insert(static_cast<Transition*>(*t2)); // mark transition as seen
            obsoleteTransitions.insert(static_cast<Transition*>(*t2));
            replaceRelation[static_cast<Transition*>(*t2)]=*t1;
          }
      }
    }
  }

  // STEP 2: remove obsolete transitions
  unsigned int result=0;
  PNAPI_FOREACH(set<Transition*>, obsoleteTransitions, t1)
  {
    Transition* t2 = replaceRelation[*t1];
    t2->mergeNameHistory(**t1);
    deleteTransition(**t1);
    ++result;
  }

  return result;
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
  PNAPI_FOREACH(set<Node*>, t1->getPreset(), p)
  {
    if (*p == p1)
      continue;
    if(findArc(**p,*t1)->getWeight() != findArc(**p,*t2)->getWeight())
      return false;
  }

  // iterate the postset
  PNAPI_FOREACH(set<Node*>, t1->getPostset(), p)
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
 * connected by an arc weight of 1 respectively (precondition 3)
 * and both transitions are connected with all places different
 * from p1 and p2 in the same way (precondition 4)
 * and the preset of p1 and p2 respectively is not empty (precondition 5),
 * and p2 is not concerned by a final condition (precondition 6),
 * and t2 is not synchronized (precondition 7),
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
 * \post  this rule preserves lifeness and boundedness according to [Sta90]
 * 
 * \return  number of removed places (which equals the number 
 *          of removed transitions) 
 * 
 */
unsigned int PetriNet::reduce_rule_4()
{
  /* 
   * STEP 1:  get places with singleton postset which is connected
   *          with arc weight 1
   */
  set<Place*> relevantPlaces;

  // iterate internal places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
  {
    if( ( (*p)->getPostset().size()==1) && // precondition 2
        ( (*((*p)->getPostsetArcs().begin()))->getWeight()==1 ) && // precondition 3
        ( !((*p)->getPreset().empty()) ) ) // precondition 5
    {
      relevantPlaces.insert(*p);
    }
  }

  // STEP 2: check those places for equality
  set<pair<Place*,Place*> > equalPlaces;
  set<Place*> seenPlaces; 

  PNAPI_FOREACH(set<Place*>, relevantPlaces, p1)
  {
    if(seenPlaces.find(*p1) != seenPlaces.end())
      continue;

    for (set<Place*>::iterator p2 = (++p1)--; // precondition 1
    p2 != relevantPlaces.end(); ++p2)
    {
      Transition* t1 = static_cast<Transition*>(*((*p1)->getPostset().begin()));
      Transition* t2 = static_cast<Transition*>(*((*p2)->getPostset().begin()));
      if( (t1 != t2) &&
          __REDUCE_CHECK_FINAL(*p2) && // precondition 6
          (reduce_isEqual(t1,t2,*p1,*p2)) && // precondition 4
          (!(t2->isSynchronized())) ) // precondition 7
      {
        equalPlaces.insert(pair<Place*,Place*>(*p1,*p2));
        seenPlaces.insert(*p2);
      }
    }
  }

  // STEP 3: actual reduction
  unsigned int result=0;

  for(set<pair<Place*,Place*> >::iterator equals = equalPlaces.begin();
       equals != equalPlaces.end(); ++equals)
  {
    // STEP 3.1: set arcs
    Place* p1 = (*equals).first;
    Place* p2 = (*equals).second;

    PNAPI_FOREACH(set<Arc*>, p2->getPresetArcs(), a)
    {
      int weight = (*a)->getWeight();
      Arc* a2 = findArc((*a)->getSourceNode(),*p1);
      if(a2 != NULL)
      {
        weight += a2->getWeight();
        deleteArc(*a2);
      }
      createArc((*a)->getSourceNode(),*p1,weight);
    }

    // STEP 3.2: set marking
    p1->mark(p1->getTokenCount() + p2->getTokenCount());

    // save history
    p1->mergeNameHistory(*p2);
    (*(p1->getPostset().begin()))->mergeNameHistory(*(*(p2->getPostset().begin())));

    // STEP 3.3: remove place and transition
    deleteTransition(*(static_cast<Transition*>(*(p2->getPostset().begin()))));
    deletePlace(*p2);

    ++result;
  }

  return result;
}

/*!
 * \brief Checks, wether the postset of a set of nodes is empty.
 */
bool PetriNet::reduce_emptyPostset(const std::set<Node*> & nodes)
{
  PNAPI_FOREACH(set<Node*>, nodes, n)
  {
    if(!((*n)->getPostset().empty()))
      return false;
  }
  return true;
}

/*!
 * \brief Check if the preset of a set stores only one item.
 */
bool PetriNet::reduce_singletonPreset(const std::set<Node*> & nodes)
{
  PNAPI_FOREACH(set<Node*>, nodes, n)
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
 * and not synchronized (precondition 3a)
 * and the postset's postset is not empty (precondition 4)
 * and the postsets preset is {p} (precondition 5)
 * and the arc weight from p to each transition of its postset is v (precondition 6)
 * and p is not concerned by a final condition (precondition f)
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
 * If both the preset and the postset of p contain interface transitions
 * (i.e. transitons connected with interfac places) and keepNormal is true, 
 * the reduction will be prevented (precondition 10).
 * 
 * \param   keepNormal determines wether reduction of a normalized net should
 *          preserve normalization or not. If true, the reduction will be
 *          prevented if both the preset and the postset of p contain
 *          interface transitions.
 * 
 * \post  this rule preserves lifeness and boundedness according to [Sta90]
 * 
 * \return  number of removed places
 * 
 */
unsigned int PetriNet::reduce_rule_5(bool keepNormal)
{
  // search for places fullfilling the preconditions
  set<Place*> obsoletePlaces;
  map<Node*,unsigned int> v_;

  // transitions must not be in more than one reduction at once
  map<Node*,bool> seenTransitions;
  
  PNAPI_FOREACH(set<Transition*>, transitions_, t)
    seenTransitions[*t]=false;

  // iterate internal places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
  {
    if(!(__REDUCE_CHECK_FINAL(*p)) ) // precondition f
      continue;

    set<Node*> preset = (*p)->getPreset();
    set<Arc*> preArcs = (*p)->getPresetArcs();
    set<Node*> postset = (*p)->getPostset();
    set<Arc*> postArcs = (*p)->getPostsetArcs();

    {
      // precondition 10
      if(keepNormal)
      {
        bool precond10a = false;
        bool precond10b = false;

        // check the preset
        PNAPI_FOREACH(set<Node*>, preset, n)
          if((*n)->getType() != Node::INTERNAL )
          {
            precond10a = true;
            break;
          }

        // check the postset
        PNAPI_FOREACH(set<Node*>, postset, n)
          if((*n)->getType() != Node::INTERNAL )
          {
            precond10b = true;
            break;
          }

        if(precond10a && precond10b)
          continue;
      }
    }

    // check for seen or synchronized (precondition 3a) transitions
    {
      bool seen = false;
      PNAPI_FOREACH(set<Node*>, preset, t)
        seen = (seen || seenTransitions[*t] || static_cast<Transition*>(*t)->isSynchronized());
      PNAPI_FOREACH(set<Node*>, postset, t)
        seen = (seen || seenTransitions[*t] || static_cast<Transition*>(*t)->isSynchronized());
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

    PNAPI_FOREACH(set<Arc*>, postArcs, a)
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
      PNAPI_FOREACH(set<Arc*>, preArcs, a)
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
        PNAPI_FOREACH(set<Node*>, preset, t)
          seenTransitions[*t]=true;
        PNAPI_FOREACH(set<Node*>, postset, t)
          seenTransitions[*t]=true;
      }
    }
    else // case b, precondition 7b
    {
      if((*p)->getTokenCount() >= v) // precondition 8b
        continue;

      bool precond_9b = true;
      PNAPI_FOREACH(set<Arc*>, preArcs, a)
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
        PNAPI_FOREACH(set<Node*>, preset, t)
          seenTransitions[*t]=true;
        PNAPI_FOREACH(set<Node*>, postset, t)
          seenTransitions[*t]=true;
      }
    }
  }

  // apply reduction
  unsigned int result = 0;

  PNAPI_FOREACH(set<Place*>, obsoletePlaces, p)
  {
    // STEP 1
    while((*p)->getTokenCount() > v_[*p]) // if there are too much tokens stored in p
    {
      // fire the only transition in p's postset
      Transition* t = static_cast<Transition*>(*((*p)->getPostset().begin()));

      (*p)->mark((*p)->getTokenCount() - v_[*p]); // consume tokens

      PNAPI_FOREACH(set<Arc*>, t->getPostsetArcs(), a)
      {
        Place* postP = static_cast<Place*>(&((*a)->getTargetNode())); 
        postP->mark(postP->getTokenCount() + (*a)->getWeight()); // produce tokens
      }
    }

    // STEP 2
    PNAPI_FOREACH(set<Node*>, (*p)->getPreset(), t1)
      PNAPI_FOREACH(set<Node*>, (*p)->getPostset(), t2)
      {
        // save the history
        Transition* t12 = &createTransition();
        t12->mergeNameHistory(**t1);
        t12->mergeNameHistory(**p);
        t12->mergeNameHistory(**t2);

        // copy incoming arcs
        PNAPI_FOREACH(set<Arc*>, (*t1)->getPresetArcs(), a)
          createArc((*a)->getSourceNode(),*t12,(*a)->getWeight());

        // arrange outgoing arcs
        set<Node*> newPostset;
        map<Node*,unsigned int> weights;

        PNAPI_FOREACH(set<Arc*>, (*t1)->getPostsetArcs(), a)
        {
          if(&((*a)->getTargetNode()) == (*p))
            continue;
          newPostset.insert(&((*a)->getTargetNode()));
          weights[&((*a)->getTargetNode())]+=(*a)->getWeight();
        }

        PNAPI_FOREACH(set<Arc*>, (*t2)->getPostsetArcs(), a)
        {
          newPostset.insert(&((*a)->getTargetNode()));
          weights[&((*a)->getTargetNode())] += ((findArc((**t1),(**p))->getWeight()) / v_[*p]) * (*a)->getWeight();
        }

        PNAPI_FOREACH(set<Node*>, newPostset, np)
          createArc(*t12,(**np),weights[*np]);
      }

    // STEP 3:
    set<Node*> preset = (*p)->getPreset();
    set<Node*> postset = (*p)->getPostset();
    deletePlace(*(*p));
    PNAPI_FOREACH(set<Node*>, preset, t)
      deleteTransition(*static_cast<Transition*>(*t));
    PNAPI_FOREACH(set<Node*>, postset, t)
      deleteTransition(*static_cast<Transition*>(*t));

    ++result;
  }

  return result; 
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
 * and p is not concerned by a final condition (precondition 8)
 * and the pre- and posttransitions of p are not synchronized (precondition 9)
 * then the following reduction can be applied:
 * 
 * 1.:  For each transition ti' from the postset of p add a new 
 *      transition ti which will be connected with the presets 
 *      of t and ti' and the postset of ti' appropriate.
 * 2.:  Remove p and its pre- and postset.
 * 
 * Ti will store the history of t, p and ti'.
 * 
 * \post  this rule preserves lifeness and boundedness according to [Sta90]
 * 
 * If both t is an interface transition and the postset of p contains such
 * (i.e. transitons connected with interfac places) and keepNormal is true, 
 * the reduction will be prevented (precondition 10).
 * 
 * \param   keepNormal determines wether reduction of a normalized net should
 *          preserve normalization or not. If true, the reduction will be
 *          prevented if both t is an interface transition and 
 *          the postset of p contains such.
 * 
 * \return  number of removed places
 *   
 */
unsigned int PetriNet::reduce_rule_6(bool keepNormal)
{ 
  // search for places fullfilling the preconditions
  set<Place*> obsoletePlaces;

  // transitions must not be in more than one reduction at once
  map<Node*,bool> seenTransitions;
  PNAPI_FOREACH(set<Transition*>, transitions_, t)
    seenTransitions[*t]=false;

  // iterate internal places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
  {
    if ( ((*p)->getPreset().size() != 1) || // precondition 1
        (!(__REDUCE_CHECK_FINAL(*p))) ) // precondition 8
      continue;

    Transition* t = static_cast<Transition*>(*((*p)->getPreset().begin()));

    {
      // precondition 10
      if(keepNormal)
      {
        bool precond10 = false;

        // check the postset
        PNAPI_FOREACH(set<Node*>, (*p)->getPostset(), n)
        {
          if((*n)->getType() != Node::INTERNAL )
          {
            precond10 = true;
            break;
          }
        }

        if( precond10 &&
            (t->getType() != Node::INTERNAL ) ) 
          continue;
      }
    }

    // check for seen or synchronized transitions (precondition 9)
    {
      bool seen = (seenTransitions[t] || t->isSynchronized());

      PNAPI_FOREACH(set<Node*>, (*p)->getPostset(), n)
        seen = (seen || seenTransitions[*n] || static_cast<Transition*>(*n)->isSynchronized());

      if(seen)
        continue;
    }

    if( (t->getPostset().size() != 1) || // precondition 2
        ((*p)->getPostset().empty()) || // precondition 3
        ((*p)->getPostset().find(t) != (*p)->getPostset().end()) ) // precondition 4
      continue;

    // precondition 5
    {
      bool precond5 = false;

      PNAPI_FOREACH(set<Node*>, t->getPreset(), n)
      {
        if((*n)->getPostset().size() != 1)
        {
          precond5 = true;
          break;
        }
      }

      if(precond5) 
        continue;
    }

    unsigned int v = (*((*p)->getPresetArcs().begin()))->getWeight();
    bool precond6 = false;

    PNAPI_FOREACH(set<Arc*>, (*p)->getPostsetArcs(), a)
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
    PNAPI_FOREACH(set<Node*>, (*p)->getPostset(), n)
      seenTransitions[*n] = true; // mark postset as seen
  }

  // apply reduction
  unsigned int result = 0;

  PNAPI_FOREACH(set<Place*>, obsoletePlaces, p)
  {
    // STEP 1:
    Transition* t = static_cast<Transition*>(*((*p)->getPreset().begin()));

    set<Node*> postset = (*p)->getPostset();

    PNAPI_FOREACH(set<Node*>, postset, t__)
    {
      Transition* nt = &createTransition(); // get new transition

      nt->mergeNameHistory(*t); // save history
      nt->mergeNameHistory(**p); // save history
      nt->mergeNameHistory(**t__); // save history

      // arrange preset
      set<Node*> preset;
      map<Node*,unsigned int> weights;

      PNAPI_FOREACH(set<Arc*>, t->getPresetArcs(), a)
      {
        preset.insert(&((*a)->getSourceNode()));
        weights[&((*a)->getSourceNode())] += (*a)->getWeight();
      }

      PNAPI_FOREACH(set<Arc*>, (*t__)->getPresetArcs(), a)
      {
        if(&((*a)->getSourceNode()) == *p)
          continue;
        preset.insert(&((*a)->getSourceNode()));
        weights[&((*a)->getSourceNode())] += (*a)->getWeight();
      }

      PNAPI_FOREACH(set<Node*>, preset, n)
      {
        createArc((**n),*nt,weights[*n]);
      }

      // copy postset
      PNAPI_FOREACH(set<Arc*>, (*t__)->getPostsetArcs(), a)
        createArc(*nt,(*a)->getTargetNode(),(*a)->getWeight());
    }


    // STEP 2:
    deleteTransition(*t);

    set<Node*> obsoleteTransitions = (*p)->getPostset();

    deletePlace(**p);

    PNAPI_FOREACH(set<Node*>, obsoleteTransitions, n)
    {
      deleteTransition(*static_cast<Transition*>(*n));
    }

    ++result;
  }

  return result;
}

/*!
 * \brief Remove loop places.
 * 
 * If there exists a place p with identical preset and postset (precondition 1)
 * and for each transition in the preset of p applies that the
 * arc weight from p to t equals the arc weight from t to p (precondition 2)
 * and this arc weight is less or equal than the amount of tokens stored in p (precondition 3)
 * and p is not concerned by a final condition (precondition 4)
 * than this place can be removed as well as each transition
 * becoming isolated by this reduction, except those ones 
 * that are synchronized (precondition 5).
 * 
 * \post  this rule preserves lifeness and boundedness according to [Sta90]
 * 
 * \return  number of removed places
 * 
 * \todo: How to handle the history of removed places and transitions?
 *   
 */
unsigned int PetriNet::reduce_rule_7()
{
  // search for places fullfilling the preconditions
  set<Place*> obsoletePlaces;

  // iterate internal places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
  {
    if(!( ((*p)->getPreset() == (*p)->getPostset()) && // precondition 1 
        (__REDUCE_CHECK_FINAL(*p)) )) // precondition 4 
      continue;

    unsigned int m = (*p)->getTokenCount();
    bool precond = false;

    PNAPI_FOREACH(set<Arc*>, (*p)->getPresetArcs(), a1)
    {
      Arc* a2 = findArc((**p),(*a1)->getSourceNode());
      if( ((*a1)->getWeight() != a2->getWeight()) || // precondition 2
          (a2->getWeight() > m) ) // precondition 3
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
  unsigned int result = 0;

  set<Transition*> obsoleteTransitions;

  PNAPI_FOREACH(set<Place*>, obsoletePlaces, p)
  {
    // check for isolated transitions
    PNAPI_FOREACH(set<Node*>, (*p)->getPreset(), t)
      if ( ((*t)->getPreset().size() == 1) &&
          ((*t)->getPostset().size() == 1) )
        obsoleteTransitions.insert(static_cast<Transition*>(*t));

    // remove place
    deletePlace(**p);
    ++result;
  }

  // clean transitions
  PNAPI_FOREACH(set<Transition*>, obsoleteTransitions, t)
  {
    if((*t)->isSynchronized()) // precondition 5
      continue;
    deleteTransition(**t);
  }

  return result;
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
 * ant t is not synchronized (precondition 6)
 * than t can be removed.
 * 
 * \post  this rule preserves lifeness and boundedness according to [Sta90]
 * 
 * \return  number of removed places
 * 
 * \todo: How to handle the history of removed transitions?
 *  
 */
unsigned int PetriNet::reduce_rule_8()
{
  // search for transitions fullfilling the preconditions
  set<Transition*> obsoleteTransitions;

  // don't reduce "backup"-transitions
  map<Node*,bool> seenTransitions;

  PNAPI_FOREACH(set<Transition*>, transitions_, t)
  {
    seenTransitions[*t] = false;
  }

  // iterate transitions
  PNAPI_FOREACH(set<Transition*>, transitions_, t)
  {
    if( (seenTransitions[*t]) || // check if t is a "backup"-transition
        ((*t)->isSynchronized()) ) // precondition 6
      continue;

    if(!((*t)->getPreset() == (*t)->getPostset())) // precondition 1
      continue;

    bool precond2 = false;

    PNAPI_FOREACH(set<Arc*>, (*t)->getPresetArcs(), a1)
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

    PNAPI_FOREACH(set<Node*>, (*((*t)->getPreset().begin()))->getPostset(), t0)
    {
      if((*t)==(*t0)) // precondition 3
        continue;

      bool precond45 = false;

      PNAPI_FOREACH(set<Arc*>, (*t)->getPresetArcs(), a1)
      {
        Arc* a2 = findArc((*a1)->getSourceNode(),(**t0));
        if ( (a2 == NULL) || // precondition 4 
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

  unsigned int result = 0;

  PNAPI_FOREACH(set<Transition*>, obsoleteTransitions, t)
  {
    deleteTransition(**t);
    ++result;
  }

  return result;
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
 * and p is not concerned by a final condition (precondition 6)
 * and neither t nor the preset of p is synchronized (precondition 7)
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
 * \post  this rule preserves lifeness and boundedness according to [Sta90]
 * 
 * If both the preset of p contains interface transitions and t is an
 * interface transition (i.e. transitons connected with interfac places) 
 * and keepNormal is true, the reduction will be prevented (precondition 8).
 * 
 * \param   keepNormal determines wether reduction of a normalized net should
 *          preserve normalization or not. If true, the reduction will be
 *          prevented if both the preset of p contains interface transitions 
 *          and t is an interface transition.
 * 
 * \return  number of removed places
 * 
 * \note  Precondition 2a is not from [Sta90].
 * 
 */
unsigned int PetriNet::reduce_rule_9(bool keepNormal)
{
  // search for places fullfilling the preconditions
  set<Place*> obsoletePlaces;

  // transitions must not be in more than one reduction at once
  map<Node*,bool> seenTransitions;

  PNAPI_FOREACH(set<Transition*>, transitions_, t)
    seenTransitions[*t]=false;

  // iterate internal places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
  { 
    // precondition 8
    if(keepNormal)
    {
      bool precond10 = false;

      // check the preset
      PNAPI_FOREACH(set<Node*>, (*p)->getPreset(), n)
      {
        if((*n)->getType() != Node::INTERNAL )
        {
          precond10 = true;
          break;
        }
      }

      if( precond10 && // there is an interface transition in the preset
          ((*p)->getPostset().begin() != (*p)->getPostset().end()) && // there is a transition in the postset
          ((*((*p)->getPostset().begin()))->getType() != Node::INTERNAL ) ) // this transition is also an interface transition
        continue;
    }
    
    {
      // check for seen or synchronized transitions (precondition 7)
      bool seen = ((*p)->getPostset().begin() == (*p)->getPostset().end()) ?
          false : ( seenTransitions[*((*p)->getPostset().begin())] || 
              static_cast<Transition*>(*((*p)->getPostset().begin()))->isSynchronized());
      PNAPI_FOREACH(set<Node*>, (*p)->getPreset(), t)
      {
        seen = seen || seenTransitions[*t] || static_cast<Transition*>(*t)->isSynchronized();
      }

      if(seen)
        continue;
    }

    if( ((*p)->getPostset().size() != 1) || // precondition 2a
        ((*p)->getPreset().empty()) || // precondition 3
        ((*((*p)->getPostsetArcs().begin()))->getWeight() != 1) || //precondition 2
        (!(__REDUCE_CHECK_FINAL(*p))) ) // precondition 6
      continue;

    Transition* t = static_cast<Transition*>(*((*p)->getPostset().begin()));

    if( (t->getPreset().size() != 1) || // precondition 1
        (t->getPostset().empty()) || // precondition 4
        (t->getPostset().find(*p) != t->getPostset().end()) ) // precondition 5
      continue;

    // p is fullfilling the preconditions

    obsoletePlaces.insert(*p);
    seenTransitions[t] = true; // mark t as seen
    PNAPI_FOREACH(set<Node*>, (*p)->getPreset(), ti)
      seenTransitions[*ti] = true; // mark preset of p as seen    
  }

  // apply reduction  
  unsigned int result = 0;

  PNAPI_FOREACH(set<Place*>, obsoletePlaces, p)
  {
    // STEP 1:
    Transition* t = static_cast<Transition*>(*((*p)->getPostset().begin()));
    unsigned int tokens = (*p)->getTokenCount();

    PNAPI_FOREACH(set<Arc*>, t->getPostsetArcs(), a)
    { 
      Place* pi = static_cast<Place*>(&((*a)->getTargetNode()));
      pi->mark(pi->getTokenCount() + ((*a)->getWeight() * tokens));
    }

    (*p)->mark(0);

    // STEP 2:
    set<Node*> preset = (*p)->getPreset();

    // iterate preset of p
    PNAPI_FOREACH(set<Node*>, preset, ti)
    {
      Transition* tj = &createTransition(); // create new Transition Tj

      // save history
      tj->mergeNameHistory(**ti);
      tj->mergeNameHistory(**p);
      tj->mergeNameHistory(*t);

      // copy preset
      PNAPI_FOREACH(set<Arc*>, (*ti)->getPresetArcs(), a)
      {
        createArc((*a)->getSourceNode(),*tj,(*a)->getWeight());
      }

      // arrange postset
      set<Node*> np;
      map<Node*,unsigned int> weights;

      PNAPI_FOREACH(set<Arc*>, (*ti)->getPostsetArcs(), a)
      {
        if(&((*a)->getTargetNode()) == *p)
          continue;

        np.insert(&((*a)->getTargetNode()));
        weights[&((*a)->getTargetNode())] += (*a)->getWeight();
      }

      unsigned int weight = findArc(**ti,**p)->getWeight();

      PNAPI_FOREACH(set<Arc*>, t->getPostsetArcs(), a)
      {
        np.insert(&((*a)->getTargetNode()));
        weights[&((*a)->getTargetNode())] += ( weight * ((*a)->getWeight()) );
      }

      PNAPI_FOREACH(set<Node*>, np, n)
      {
        createArc(*tj,**n,weights[*n]);
      }
    }

    // STEP 3:
    deletePlace(**p);

    PNAPI_FOREACH(set<Node*>, preset, ti)
    {
      deleteTransition(*static_cast<Transition*>(*ti));
    }

    deleteTransition(*t);

    ++result;
  }

  return result;
}


/******************************************************************************
 * Functions to structurally simplify the Petri net model according to [Pil08]
 *****************************************************************************/



/*!
 * \brief Elimination of identical places (RB1):
 *
 * If there exist two distinct places p1 and p2 (precondition 1) 
 * with identical preset (precondition 2) 
 * and postset (precondition 3) 
 * and the weights of all incoming and outgoing arcs 
 * have the value 1 (precondition 4), 
 * and neither of them is initially marked (precondition 5), 
 * and p1 is not concerned by a final condition (precondition 6),
 * then p1 can be removed and its history will 
 * be attached to the history of p2.
 * 
 * \post  this rule preserves lifeness and k-boundedness
 *        according to [Pil08], def. 3.3. 
 * 
 * \return  number of removed places
 * 
 */
unsigned int PetriNet::reduce_identical_places()
{
  // obsolete place and its "backup"-place 
  set<pair<Place*, Place*> > placePairs;

  // these places either already will be deleted or must not be deleted in this iteration
  map<Node*,bool> seenPlaces;  

  PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
    seenPlaces[*p] = false;


  // trace(TRACE_DEBUG, "[PN]\tApplying rule RB1 (elimination of identical places)...\n");

  // iterate the internal places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p1)
  {
    if( (seenPlaces[*p1]) ||
        (!(__REDUCE_CHECK_FINAL(*p1))) ) // precondition 6
    {
      continue;
    }

    {
      // precondition 4
      bool precond4 = false;

      // check preset
      PNAPI_FOREACH(set<Arc*>, (*p1)->getPresetArcs(), a)
      {
        if((*a)->getWeight() != 1)
        {
          precond4 = true;
          break;
        }
      }

      // check postset
      PNAPI_FOREACH(set<Arc*>, (*p1)->getPostsetArcs(), a)
      {
        if(((*a)->getWeight() != 1) || (precond4))
        {
          precond4 = true;
          break;
        }
      }

      if(precond4)
      {
        continue;
      }
    }


    // get a pretransition
    Node* preTransition = *((*p1)->getPreset().begin());

    // test for null-pointer
    if(preTransition != NULL)
    {
      // check its postset
      PNAPI_FOREACH(set<Node*>, preTransition->getPostset(), p2)
        if ( (!(seenPlaces[*p2])) &&
            (*p1 != *p2) &&   // precondition 1
            ((*p1)->getPreset() == (*p2)->getPreset()) && // precondition 2
            ((*p1)->getPostset() == (*p2)->getPostset()) && // precondition 3
            ((*p1)->getTokenCount() == 0) && // precondition 5
            (static_cast<Place*>(*p2)->getTokenCount() == 0) ) // precondition 5
        {

          // precondition 4
          bool precond4 = false;

          // check preset
          PNAPI_FOREACH(set<Arc*>, (*p2)->getPresetArcs(), a)
            if((*a)->getWeight() != 1)
            {
              precond4 = true;
              break;
            }

          // check postset
          PNAPI_FOREACH(set<Arc*>, (*p2)->getPostsetArcs(), a)
            if(((*a)->getWeight() != 1) || (precond4))
            {
              precond4 = true;
              break;
            }

          if(precond4)
            continue;      

          // places fullfill the preconditions
          placePairs.insert(pair<Place*, Place*>(*p1, static_cast<Place*>(*p2)));
          seenPlaces[*p1] = true; // mark p1 as seen
          seenPlaces[*p2] = true; // mark p2 as seen
          break; // just one reduction at one iteration
        }
    }
    else // if there was no pretransition
    {
      // get a posttransition
      Node* postTransition = *((*p1)->getPostset().begin());

      // check for null-pointer
      if(postTransition != NULL)
      {
        PNAPI_FOREACH(set<Node*>, postTransition->getPreset(), p2)
          if ( (!(seenPlaces[*p2])) &&
              (*p1 != *p2) &&     // precondition 1
              ((*p2)->getPreset().empty()) && // precondition 2
              ((*p1)->getPostset() == (*p2)->getPostset()) && // precondition 3
              ((*p1)->getTokenCount() == 0) && // precondition 5
              (static_cast<Place*>(*p2)->getTokenCount() == 0) ) // precondition 5
          {
            // precondition 4
            bool precond4 = false;

            // check postset
            PNAPI_FOREACH(set<Arc*>, (*p2)->getPostsetArcs(), a)
              if((*a)->getWeight() != 1)
              {
                precond4 = true;
                break;
              }

            if(precond4)
              continue;      

            // places fullfill the preconditions
            placePairs.insert(pair<Place*, Place*>(*p1, static_cast<Place*>(*p2)));
            seenPlaces[*p1] = true; // mark p1 as seen
            seenPlaces[*p2] = true; // mark p2 as seen
            break; // just one reduction at one iteration
          }
      }

    }
  }

  // trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(placePairs.size()) + " places with same preset and postset...\n");

  unsigned int result = 0;
  // "merge" the found places
  for(set<pair<Place*, Place*> >::iterator p = placePairs.begin();
       p != placePairs.end(); ++p)
  {
    // get places from pair
    Place* p1 = p->first;
    Place* p2 = p->second;

    // save the history
    p2->mergeNameHistory(*p1);

    // remove place
    deletePlace(*p1);

    ++result;
  }

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
  //*/

  return result;
}



/*!
 * \brief Elimination of identical transitions (RB2):
 *
 * If there exist two distinct transitions t1 and t2 (precondition 1)
 * with identical preset (precondition 2) 
 * and postset (precondition 3) 
 * and none of them is connected to any arc with a weight 
 * other than 1 (precondition 4),
 * and t1 is not synchronized (precondition 5),
 * then t1 can be removed and its history will be stored at t2.
 * 
 * \post  this rule preserves lifeness and k-boundedness
 *        according to [Pil08], def. 5.4.
 * 
 * \return  number of removed transitions
 * 
 */
unsigned int PetriNet::reduce_identical_transitions()
{
  // obsolete transitions and its "backup"-transition
  set<pair<Transition*, Transition*> > transitionPairs;
  map<Node*,bool> backupTransition; // must not be reduced

  PNAPI_FOREACH(set<Transition*>, transitions_, t)
    backupTransition[*t] = false;

  // trace(TRACE_DEBUG, "[PN]\tApplying rule RB2 (elimination of identical transitions)...\n");

  // iterate the transitions
  PNAPI_FOREACH(set<Transition*>, transitions_, t1)
  {
    if( (backupTransition[*t1]) || 
        ((*t1)->isSynchronized()) ) // precondition 5
      continue;

    {
      // precondition 4
      bool precond4 = false;

      // check preset
      PNAPI_FOREACH(set<Arc*>, (*t1)->getPresetArcs(), a)
        if((*a)->getWeight() != 1)
        {
          precond4 = true;
          break;
        }

      // check postset
      PNAPI_FOREACH(set<Arc*>, (*t1)->getPostsetArcs(), a)
        if(((*a)->getWeight() != 1) || (precond4))
        {
          precond4 = true;
          break;
        }

      if(precond4)
        continue;      
    }


    // get a preplace
    Node* prePlace = *((*t1)->getPreset().begin());

    // check for null-pointer
    if(prePlace != NULL)
    {
      PNAPI_FOREACH(set<Node*>, prePlace->getPostset(), t2)
      {
        if ( (*t1 != *t2) &&		// precondition 1
            ((*t1)->getPreset() == (*t2)->getPreset()) &&	// precondition 2
            ((*t1)->getPostset() == (*t2)->getPostset()) ) // precondition 3
        {

          // precondition 4
          bool precond4 = false;

          // check preset
          PNAPI_FOREACH(set<Arc*>, (*t2)->getPresetArcs(), a)
            if((*a)->getWeight() != 1)
            {
              precond4 = true;
              break;
            }

          // check postset
          PNAPI_FOREACH(set<Arc*>, (*t2)->getPostsetArcs(), a)
            if(((*a)->getWeight() != 1) || (precond4))
            {
              precond4 = true;
              break;
            }

          if(precond4)
            continue;

          // transitions fullfill the preconditions
          transitionPairs.insert(pair<Transition*, Transition*>(*t1, static_cast<Transition*>(*t2)));
          backupTransition[*t2] = true; // mark t2 as backup transition
        }
      }
    }
    else // if there exists no preplace
    {
      // get a postplace
      Node* postPlace = *((*t1)->getPostset().begin());

      // check for null-pointer
      if(postPlace != NULL)
      {
        PNAPI_FOREACH(set<Node*>, postPlace->getPreset(), t2)
        {
          if ( (*t1 != *t2) &&     // precondition 1
              ((*t2)->getPreset().empty()) && // precondition 2
              ((*t1)->getPostset() == (*t2)->getPostset()) ) // precondition 3
          {
            // precondition 4
            bool precond4 = false;

            // check postset
            PNAPI_FOREACH(set<Arc*>, (*t2)->getPostsetArcs(), a)
              if((*a)->getWeight() != 1)
              {
                precond4 = true;
                break;
              }

            if(precond4)
              continue;

            // transitions fullfill the preconditions
            transitionPairs.insert(pair<Transition*, Transition*>(*t1, static_cast<Transition*>(*t2)));
            backupTransition[*t2] = true; // mark t2 as backup transition
          }
        }
      }
    }
  }

  // trace(TRACE_VERY_DEBUG, "[PN]\tFound " + toString(transitionPairs.size()) + " transitions with same preset and postset...\n");

  unsigned int result = 0;

  // merge the found transitions
  for(set<pair<Transition*, Transition*> >::iterator p = transitionPairs.begin();
       p != transitionPairs.end(); ++p)
  {
    // get transitions
    Transition* t1 = p->first;
    Transition* t2 = p->second;

    // save history
    t2->mergeNameHistory(*t1);

    // remove t1
    deleteTransition(*t1);

    ++result;
  }

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
  //*/

  return result;
}





/*!
 * \brief Fusion of series places (RA1):
 *
 * If there exists a transition t with 
 * singleton preset p and postset q (precondition 1) 
 * that are distinct (precondition 2),
 * where the p's preset is not empty (precondition 3)
 * and p has no other outgoing arcs (precondition 4)
 * and alle arcs connected with this 
 * transition have a weight of 1 (precondition 5)
 * and q is not initially marked (precondition 6),
 * and q is not concerned by a final condition (precondition f)
 * and none of these places is communicating (precondition 7)
 * and t is not synchronized (precondition 8)
 * then the histories of t and q will be attached to p,
 * the postset of q will be connected with p in the same way it
 * was connected to q, and t and q will be removed.
 * 
 * \post  this rule preserves lifeness and k-boundedness
 *        according to [Pil08], def. 4.46.
 * 
 * \return  number of removed transitions
 * 
 */
unsigned int PetriNet::reduce_series_places()
{
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RA1 (fusion of series places)...\n");

  set<Transition*> uselessTransitions;

  // places must not be involved in more than one reduction at once
  map<Node*,bool> seenPlaces;

  PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
    seenPlaces[*p] = false;

  // iterate the transtions
  PNAPI_FOREACH(set<Transition*>, transitions_, t)
  {
    Place* prePlace = static_cast<Place*>(*((*t)->getPreset().begin()));
    Place* postPlace = static_cast<Place*>(*((*t)->getPostset().begin()));

    if ( (seenPlaces[prePlace]) ||
        (seenPlaces[postPlace]) || 
        ((*t)->isSynchronized()) || // precondition 8
        (!(__REDUCE_CHECK_FINAL(postPlace))) ) // precondition f
      continue;

    if ( ((*t)->getPreset().size() == 1) && // precondition 1
        ((*t)->getPostset().size() == 1) && // precondition 1
        (prePlace != postPlace) &&			 // precondition 2
        (!(prePlace->getPreset().empty())) &&     // precondition 3
        (prePlace->getPostset().size() == 1) &&		 // precondition 4
        ((*((*t)->getPresetArcs().begin()))->getWeight() == 1) && // precondition 5
        ((*((*t)->getPostsetArcs().begin()))->getWeight() == 1) && // precondition 5
        (postPlace->getTokenCount() == 0 ) && // precondition 6
        (prePlace->getType() == Node::INTERNAL) &&	// precondition 7
        (postPlace->getType() == Node::INTERNAL) ) // precondition 7
    {
      // transition fullfilles preconditions
      uselessTransitions.insert(*t);
      seenPlaces[prePlace] = true; // mark preplace as seen
      seenPlaces[postPlace] = true; // mark postplace as seen
    }
  }

  // apply reduction
  unsigned int result = 0;

  PNAPI_FOREACH(set<Transition*>, uselessTransitions, t)
  {
    // get preset and postset
    Node* prePlace = *((*t)->getPreset().begin());
    Node* postPlace = *((*t)->getPostset().begin());

    // save history
    prePlace->mergeNameHistory(**t);
    prePlace->mergeNameHistory(*postPlace);

    // set new postset
    PNAPI_FOREACH(set<Arc*>, postPlace->getPostsetArcs(), a)
    {
      createArc(*prePlace,(*a)->getTargetNode(),(*a)->getWeight());
    }

    // remove postplace and t
    deletePlace(*(static_cast<Place*>(postPlace)));
    deleteTransition(**t);

    ++result;
  }

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
  //*/

  return result;
}





/*!
 * \brief Fusion of series transition (RA2):
 *
 * If there exists a place p with singleton preset t1 and postset t2 (precondition 1),
 * which is not final or marked (precondition 2),
 * and if t2 has no other incoming arcs (precondition 3),
 * and if the postsets of t1 and t2 are distinct (precondition 4),
 * and the arc weight from t1 to p and from p to t2 is 1 (precondition 5),
 * and neither t1 nor t2 are synchronized (precondition 6),
 * then the following reduction can be applied:
 * 1.:  A new transition t will be created and connected with the preset of
 *      t1 and the postsets of t1 and t2 according to the appropriate
 *      arc weights of t1 or t2 to their postplaces.
 * 2.:  T1, p and t2 will be removed.
 * 
 * T stores the history of t1, p and t2. 
 * 
 * \post  this rule preserves lifeness and k-boundedness
 *        according to [Pil08], def. 4.31.
 * 
 * If both the pretransition and the posttransition are connected with an
 * interface place and keepNormal is true, the reduction will be prevented (precondition 6)
 * 
 * \param   keepNormal determines wether reduction of a normalized net should
 *          preserve normalization or not. If true, interface transitions
 *          (i.e. transitions connected with interface places) will only be
 *          merged with internal transitions (i.e. transitions connected
 *          with internal places only).
 * 
 * \return  number of removed places 
 * 
 */
unsigned int PetriNet::reduce_series_transitions(bool keepNormal) {
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RA2 (fusion of series transitions)...\n");

  set<Place*> uselessPlaces;

  // transitions must not be involved in more than one reduction at once
  map<Node*,bool> seenTransitions;

  PNAPI_FOREACH(set<Transition*>, transitions_, t)
    seenTransitions[*t] = false;

  // iterate the internal places
  PNAPI_FOREACH(set<Place*>, internalPlaces_, p) 
  {
    if ( ((*p)->getPostset().size() == 1) && //precondition 1
        ((*p)->getPreset().size() == 1) && //precondition 1
        __REDUCE_CHECK_FINAL(*p) && // precondition 2
        (!((*p)->getTokenCount() > 0)) ) // precondition 2
    {
      Transition* t1 = static_cast<Transition*>(*((*p)->getPreset().begin()));
      Transition* t2 = static_cast<Transition*>(*((*p)->getPostset().begin()));

      // check for seen transitions
      if( (seenTransitions[t1]) || 
          (seenTransitions[t2]) ||
          (t1->isSynchronized()) || // precondition 6
          (t2->isSynchronized()) ) // precondition 6
        continue;

      if ( (t2->getPreset().size() == 1) && // precondition 3
          (util::setIntersection(t1->getPostset(),t2->getPostset()).empty() ) && // precondition 4
          (!( (t1->getType() != Node::INTERNAL) && 
              (t2->getType() != Node::INTERNAL) && 
              (keepNormal)) ) && //precondition 4
              ((*((*p)->getPresetArcs().begin()))->getWeight() == 1) && // precondition 5
              ((*((*p)->getPostsetArcs().begin()))->getWeight() == 1) ) // precondition 5
      {
        uselessPlaces.insert(*p);
        seenTransitions[t1] = true;
        seenTransitions[t2] = true;
      }
    }
  }

  // apply reduction
  unsigned int result = 0;

  PNAPI_FOREACH(set<Place*>, uselessPlaces, p) 
  {
    // create new transition
    Transition* t = &createTransition();

    Transition* t1 = static_cast<Transition*>(*((*p)->getPreset().begin()));
    Transition* t2 = static_cast<Transition*>(*((*p)->getPostset().begin()));

    // save history
    t->mergeNameHistory(*t1);
    t->mergeNameHistory(**p);
    t->mergeNameHistory(*t2);

    // copy preset
    PNAPI_FOREACH(set<Arc*>, t1->getPresetArcs(), a)
      createArc((*a)->getSourceNode(),*t,(*a)->getWeight());

    // copy postset
    PNAPI_FOREACH(set<Arc*>, t1->getPostsetArcs(), a)
      createArc(*t,(*a)->getTargetNode(),(*a)->getWeight());

    PNAPI_FOREACH(set<Arc*>, t2->getPostsetArcs(), a)
      createArc(*t,(*a)->getTargetNode(),(*a)->getWeight());

    // delete place
    deletePlace(**p);

    // delete transitions
    deleteTransition(*t1);
    deleteTransition(*t2);

    ++result;
  }

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
  //*/

  return result;
}


/*!
 * Elimination of self-loop places (ESP) as depicted in [Murrata89]. 
 * The rule preserves liveness, safeness and boundedness.
 *
 * \note As p has both ingoing and outgoing arcs, p is an
 *       internal place. Thus, removing this place preserves controllability
 *       and all communicating partners if p is not covered by a final
 *       marking. 
 *
 * \return number of removed places
 * 
 * \pre p is a place of the net
 * \pre p is initially marked (with exactly one token)
 * \pre p has one transition in its preset and one transition in its postset
 * \pre p's preset and postset are equal
 *
 * \post p is removed
 * 
 * Extension by arc weights:
 * If there exists a place p with singleton preset t (precondition 1),
 * where the preset is identical to the postset (precondition 2),
 * and the arc weights of (p->t) and (t->p) is 1 (precondition 3),
 * and p stores initially 1 token (precondition 4),
 * and p is not concerned by a final condition (precondition 5),
 * then this place can be removed.
 * 
 * \todo: How to handle the history of removed transitions?
 * 
 */
unsigned int PetriNet::reduce_self_loop_places()
{
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RC1 (Elimination of self-loop places)...\n");
  set<Place *> self_loop_places;

  // find places fulfilling the preconditions
  PNAPI_FOREACH(set<Place *>, internalPlaces_, p)
    if ( ((*p)->getPreset().size() == 1) &&       // precondition 1
        ((*p)->getPreset() == (*p)->getPostset()) &&  // precodnition 2
        ((*((*p)->getPresetArcs().begin()))->getWeight() == 1) && // precondition 3
        ((*((*p)->getPostsetArcs().begin()))->getWeight() == 1) && // precondition 3
        ((*p)->getTokenCount() == 1 ) && // precondition 4
        __REDUCE_CHECK_FINAL(*p) ) // precondition 5
      self_loop_places.insert(*p);

  // remove useless places
  unsigned int result = 0;

  PNAPI_FOREACH(set<Place*>, self_loop_places, p)
  {
    deletePlace(**p);
    ++result;
  }

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
  //*/

  return result;
}





/*!
 * Elimination of self-loop transitions (EST) as depicted in [Murrata89].
 * The rule preserves liveness, safeness and boundedness.
 *
 * \note As the place in t's preset/postset has ingoing and outgoing
 *       arcs, it is an internal place. Thus, t does not communicate
 *       and its removal does not affect controllability or the set of
 *       communicating partners.
 *
 * \return number of removed transitions
 * 
 * \pre t is a transition of the net
 * \pre t has one place in its preset and one place in its postset
 * \pre t's preset and postset are equal
 *
 * \post t is removed
 * 
 * Extension by arc weights:
 * If there exists a transition t with singleton preset p (precondition 1),
 * where the preset is identical to the postset (precondition 2),
 * and the arc weights both of (p->t) and of (t->p) is equal to 1 (precondition 3),
 * and t is not synchronized (precondition 4),
 * then this transition can be removed.
 * 
 * \todo: How to handle the history of removed transitions?
 * 
 */
unsigned int PetriNet::reduce_self_loop_transitions()
{
  // trace(TRACE_DEBUG, "[PN]\tApplying rule RC1 (Elimination of self-loop transitions)...\n");
  set<Transition*> self_loop_transitions;

  // find transitions fulfilling the preconditions
  unsigned int result = 0;

  PNAPI_FOREACH(set<Transition*>, transitions_, t)
    if ( ((*t)->getPreset().size() == 1) &&        // precondition 1
        ((*t)->getPreset() == (*t)->getPostset()) &&   // precondition 2
        ((*((*t)->getPresetArcs().begin()))->getWeight() == 1) &&  // precondition 3
        ((*((*t)->getPostsetArcs().begin()))->getWeight() == 1) &&  // precondition 3
        (!((*t)->isSynchronized())) ) // precondition 4
      self_loop_transitions.insert(*t);

  // remove useless transitions
  PNAPI_FOREACH(set<Transition*>, self_loop_transitions, t)
  {
    deleteTransition(**t);
    ++result;
  }

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " transitions.\n");
  //*/

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
 * \note  postponed until references are available
 *
 */
unsigned int PetriNet::reduce_equal_places()
{
  /*

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

      // FIXME
      set<Node*> preSetT1 = (t1)->preset;
      set<Node*> preSetT2 = (t2)->preset;

      preSetT1.erase(p1);
      preSetT2.erase(*p2);

      if (preSetT1 != preSetT2) //precondition 5
        continue;


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

   */

  /*
  if (result!=0)
    trace(TRACE_DEBUG, "[PN]\t...removed " + toString(result) + " places.\n");
  //*/

  return 0;
}



#undef __REDUCE_CHECK_FINAL




/*!
 * Removes unnecessary nodes of the generated Petri net:
 *  - structural dead nodes
 *  - unused status places
 *  - suspicious transitions
 *
 * Applys some simple structural reduction rules for Petri nets
 * preserving boundedness and lifeness (but no k-boundedness):
 *  - elimination of parallel places (rule_3p)
 *  - elimination of parallel transitions (rule_3t)
 *  - elimination of equal places (rule_4)
 *  - fusion of series places (rule_9)
 *  - fusion of series transitions (rule_5 and rule_6)
 *  - elimination of self-loop places (rule_7)
 *  - elimination of self-loop transitions (rule_8)
 * 
 * Functions are named by the appropriate rule in [Sta90].
 * 
 * Fuctions preserving k-boundedness in addition according to 
 * [Pil08] and [Murate89] are named by their behavior.
 *
 * The rules are applied until a fixed point is reached.
 *
 * \return the number of passes until a fixed point was reached
 * 
 * \param   reduction_level determines which rules will be performed.
 *          The first 19 bits determine, which rule will be applied;
 *          each bit stands for a specific rule. In detail: 
 *           1st - unused status places
 *           2nd - suspicious transitions 
 *           3rd - dead nodes
 *           4th - initially marked places in choreographies
 *           5th - Starke rule 3 for places
 *           6th - Starke rule 3 for transitions
 *           7th - Starke rule 4
 *           8th - Starke rule 5
 *           9th - Starke rule 6
 *          10th - Starke rule 7
 *          11th - Starke rule 8
 *          12th - Starke rule 9
 *          13th - identical places
 *          14th - identical transitions
 *          15th - series places
 *          16th - series transitions
 *          17th - self-loop places
 *          18th - self-loop transitions
 *          19th - equal places        
 * 
 *          The 20th bit determines, wether a normal net should be
 *          kept normal. Enum ReductionLevel has been implemented,
 *          to simplify the usage of reduction.
 *          Additional keywords apply a whole set of rules. I.e.:
 *  
 *          LEVEL_1 - Dead nodes will be removed.
 *          LEVEL_2 - Unused statusplaces and suspicious transitions 
 *                    will be removed. (BPEL2oWFN only)
 *          LEVEL_3 - Identical nodes will be removed.
 *          LEVEL_4 - Series nodes will be removed.
 *          LEVEL_5 - Self-loop nodes will be removed as well as
 *                    initially marked places in choreographies.
 *          LEVEL_6 - Equal places will be removed.
 *          (A higher level of reduction implies all reduction
 *            rules from lower levels.)
 *         
 *          SET_UNNECCESSARY - Unneccessary nodes will be removed.
 *          SET_PILLAT       - Rules mentioned in [Pil08] and 
 *                             [Murate89] will be applied.
 *          SET_STARKE       - Rules mentioned in [Sta90] will be applied.
 * 
 */
unsigned int PetriNet::reduce(unsigned int reduction_level)
{
  // trace(TRACE_DEBUG, "[PN]\tPetri net size before simplification: " + information() + "\n");
  // trace(TRACE_INFORMATION, "Simplifying Petri net...\n");

  bool keepNormal = ((reduction_level & KEEP_NORMAL) == KEEP_NORMAL);

  unsigned int done = 1;
  unsigned int passes = 0;

  // places implied as empty can be reduced
  reducablePlaces_ = new set<const Place*>(finalCondition().formula().emptyPlaces());
  
  {
    set<const Place*> noReduce = finalCondition().concerningPlaces();
    PNAPI_FOREACH(set<Place*>, internalPlaces_, p)
    {
      // places not concerned by the final condition can be reduced
      if(noReduce.count(*p) == 0)
        reducablePlaces_->insert(*p);
    }
  }

  // apply reductions
  while (done > 0)
  {
    done = 0;
    ++passes;

#ifdef USING_BPEL2OWFN
    if ((reduction_level & UNUSED_STATUS_PLACES) == UNUSED_STATUS_PLACES)
    {
      done += reduce_unused_status_places();
    }
    if ((reduction_level & SUSPICIOS_TRANSITIONS) == SUSPICIOS_TRANSITIONS)
    {
      done += reduce_suspicious_transitions();
    }
#endif

    if ((reduction_level & DEAD_NODES) == DEAD_NODES)
    {
      done += reduce_dead_nodes();
    }

    if ((reduction_level & INITIALLY_MARKED_PLACES_IN_CHOREOGRAPHIES) == INITIALLY_MARKED_PLACES_IN_CHOREOGRAPHIES)
    {
      done += reduce_remove_initially_marked_places_in_choreographies();
    }

    if ((reduction_level & STARKE_RULE_3_PLACES) == STARKE_RULE_3_PLACES)
    {
      done += reduce_rule_3p();
    }

    if ((reduction_level & STARKE_RULE_3_TRANSITIONS) == STARKE_RULE_3_TRANSITIONS)
    {
      done += reduce_rule_3t();
    }

    if ((reduction_level & STARKE_RULE_4) == STARKE_RULE_4)
    {
      done += reduce_rule_4();
    }

    if ((reduction_level & STARKE_RULE_5) == STARKE_RULE_5)
    {
      done += reduce_rule_5(keepNormal);
    }

    if ((reduction_level & STARKE_RULE_6) == STARKE_RULE_6)
    {
      done += reduce_rule_6(keepNormal);
    }

    if ((reduction_level & STARKE_RULE_7) == STARKE_RULE_7)
    {
      done += reduce_rule_7();
    }

    if ((reduction_level & STARKE_RULE_8) == STARKE_RULE_8)
    {
      done += reduce_rule_8();
    }

    if ((reduction_level & STARKE_RULE_9) == STARKE_RULE_9)
    {
      done += reduce_rule_9(keepNormal);
    }

    if ((reduction_level & IDENTICAL_PLACES) == IDENTICAL_PLACES)
    {
      done += reduce_identical_places();  // RB1
    }

    if ((reduction_level & IDENTICAL_TRANSITIONS) == IDENTICAL_TRANSITIONS)
    {
      done += reduce_identical_transitions(); // RB2
    }

    if ((reduction_level & SERIES_PLACES) == SERIES_PLACES)
    {
      done += reduce_series_places();   // RA1
    }

    if ((reduction_level & SERIES_TRANSITIONS) == SERIES_TRANSITIONS)
    {
      done += reduce_series_transitions(keepNormal);  // RA2
    }

    if ((reduction_level & SELF_LOOP_PLACES) == SELF_LOOP_PLACES)
    {
      done += reduce_self_loop_places();  // RC1
    }

    if ((reduction_level & SELF_LOOP_TRANSITIONS) == SELF_LOOP_TRANSITIONS)
    {
      done += reduce_self_loop_transitions(); // RC2
    }

    if ((reduction_level & EQUAL_PLACES) == EQUAL_PLACES)
    {
      done += reduce_equal_places();    // RD1
    }

    if ((reduction_level & ONCE) == ONCE)
      break;
  }

  delete reducablePlaces_;
  reducablePlaces_ = NULL;
  
  return passes;
}
