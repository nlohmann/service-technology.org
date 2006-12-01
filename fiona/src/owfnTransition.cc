// *****************************************************************************\
// * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
// *                                                                           *
// * This file is part of Fiona.                                               *
// *                                                                           *
// * Fiona is free software; you can redistribute it and/or modify it          *
// * under the terms of the GNU General Public License as published by the     *
// * Free Software Foundation; either version 2 of the License, or (at your    *
// * option) any later version.                                                *
// *                                                                           *
// * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
// * more details.                                                             *
// *                                                                           *
// * You should have received a copy of the GNU General Public License along   *
// * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
// * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
// *****************************************************************************/

/*!
 * \file    owfnTransition.cc
 *
 * \brief   functions for Petri net transitions
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "owfnTransition.h"
#include "owfn.h"
#include "main.h"
#include "debug.h"
#include "state.h"
#include "options.h"

owfnTransition::owfnTransition(char * name) : Node(name), quasiEnabledNr(0), 
	enabledNr(0), NextEnabled(NULL), PrevEnabled(NULL), 
	NextQuasiEnabled(NULL), PrevQuasiEnabled(NULL), 
	quasiEnabled(false), enabled(false) {
		
	NrOfArriving = 0;
	NrOfLeaving = 0;
}

owfnTransition::~owfnTransition() {
	delete[] ImproveEnabling;
	delete[] ImproveDisabling;
	delete[] IncrPlaces;
	delete[] Incr;
	delete[] DecrPlaces;
	delete[] Decr;
	delete[] PrePlaces;
	delete[] Pre;

//	if (ImproveEnabling) {
//		delete ImproveEnabling;
//	}			
//	if (ImproveDisabling) {
//		delete ImproveDisabling;
//	}			
}


void owfnTransition::set_hashchange(oWFN * PN) {
	unsigned int i;

	hash_change = 0;
	for(i = 0; IncrPlaces[i] != NULL; i++) {
		hash_change += Incr[i] * IncrPlaces[i]->hash_factor;
	}
	for(i = 0; DecrPlaces[i] != NULL; i++) {
		hash_change -= Decr[i] * DecrPlaces[i]->hash_factor;
	}
	hash_change %= HASHSIZE;
}

void owfnTransition::initialize(oWFN * PN) {

  unsigned int i,j,k;
  // Create list of Pre-Places for enabling test
  PrePlaces = new owfnPlace*  [NrOfArriving + 1];
  Pre = new unsigned int [NrOfArriving + 1];
  for(i = 0; i < NrOfArriving;i++)
    {
      PrePlaces[i] = ArrivingArcs[i]->pl;
      Pre[i] = ArrivingArcs[i]->Multiplicity;
    }
  PrePlaces[NrOfArriving] = NULL;
  // Create list of places where transition increments marking
  IncrPlaces = new owfnPlace* [NrOfLeaving + 1];
  Incr = new unsigned int [NrOfLeaving + 1];
  
  k=0;
  for(i = 0; i < NrOfLeaving;i++)
    {
      //Is oWFNPlace a loop place?
      for(j=0;j<NrOfArriving;j++)
	{
	  if((LeavingArcs[i]->Destination) == (ArrivingArcs[j]->Source))
	    {
	      break;
	    }
	}
      if(j<NrOfArriving)
	{
	  //yes, loop place
	  if(LeavingArcs[i]->Multiplicity > ArrivingArcs[j]->Multiplicity)
	    {
	      // indeed, transition increments place
	      IncrPlaces[k] = LeavingArcs[i]->pl;
	      Incr[k] = LeavingArcs[i]->Multiplicity - ArrivingArcs[j]->Multiplicity;
	      k++;
	    } 
	}
      else
	{
	  // no loop place
	  IncrPlaces[k] = LeavingArcs[i]->pl;
	  Incr[k] = LeavingArcs[i]->Multiplicity;
	  k++;
	}
    }
  IncrPlaces[k] = NULL;
  Incr[k] = 0;
  // Create list of places where transition decrements marking
  DecrPlaces = new owfnPlace* [NrOfArriving + 1];
  Decr = new unsigned int [NrOfArriving + 1];
  k=0;
  for(i = 0; i < NrOfArriving;i++)
    {
      //Is oWFNPlace a loop place?
      for(j=0;j<NrOfLeaving;j++)
	{
	  if((ArrivingArcs[i]->Source) == (LeavingArcs[j]->Destination))
	    {
	      break;
	    }
	}
      if(j<NrOfLeaving)
	{
	  //yes, loop place
	  if(ArrivingArcs[i]->Multiplicity > LeavingArcs[j]->Multiplicity)
	    {
	      // indeed, transition decrements place
	      DecrPlaces[k] = ArrivingArcs[i]->pl;
	      Decr[k] = ArrivingArcs[i]->Multiplicity - LeavingArcs[j]->Multiplicity;
	      k++;
	    } 
	}
      else
	{
	  // no loop place
	  DecrPlaces[k] = ArrivingArcs[i]->pl;
	  Decr[k] = ArrivingArcs[i]->Multiplicity;
	  k++;
	}
    }
  DecrPlaces[k] = NULL;
  Decr[k] = 0;
  // Create list of transitions where enabledness can change
  // if this transition fires. For collecting these transitions, we
  // abuse the Enabled linked list
  PN->startOfEnabledList = (owfnTransition *) 0;
  PN->transNrEnabled = 0;
  for(i=0;IncrPlaces[i] != NULL;i++)
    {
      for(j=0;j<IncrPlaces[i]->NrOfLeaving;j++)
	{
	  if(!(IncrPlaces[i]->LeavingArcs)[j]->tr->enabled)
	    {
	      // not yet in list
	      (IncrPlaces[i]->LeavingArcs)[j]->tr->NextEnabled = PN->startOfEnabledList;
	      PN->startOfEnabledList = (IncrPlaces[i]->LeavingArcs)[j]->tr;
	      PN->transNrEnabled++;
	      (IncrPlaces[i]->LeavingArcs)[j]->tr->enabled = true;
	    }
	}
    }

  ImproveEnabling = new owfnTransition * [PN->transNrEnabled + 1];
  for(i=0; PN->startOfEnabledList; PN->startOfEnabledList = PN->startOfEnabledList -> NextEnabled,i++)
    {
      PN->startOfEnabledList->enabled = false;
      ImproveEnabling[i] = PN->startOfEnabledList;
    }
  ImproveEnabling[i] = (owfnTransition *) 0;
  // Create list of transitions where enabledness can change
  // if this transition fires. For collecting these transitions, we
  // abuse the Enabled linked list
  PN->startOfEnabledList = (owfnTransition *) 0;
  PN->transNrEnabled = 0;
  
  for(i=0;DecrPlaces[i] != NULL;i++)
    {
      for(j=0;j < DecrPlaces[i]->NrOfLeaving;j++)
	{
	  if(!(DecrPlaces[i]->LeavingArcs)[j]->tr->enabled)
	    {
	      // not yet in list
	      (DecrPlaces[i]->LeavingArcs)[j]->tr->NextEnabled = PN->startOfEnabledList;
	      PN->startOfEnabledList = (DecrPlaces[i]->LeavingArcs)[j]->tr;
	      PN->transNrEnabled++;
	      (DecrPlaces[i]->LeavingArcs)[j]->tr->enabled = true;
	    }
	}
    }
  	
  	ImproveDisabling = new owfnTransition * [PN->transNrEnabled + 1];
  	
  	for(i=0; PN->startOfEnabledList; PN->startOfEnabledList = PN->startOfEnabledList -> NextEnabled,i++) {
		PN->startOfEnabledList->enabled = false;
		ImproveDisabling[i] = PN->startOfEnabledList;
    }
    
    
  ImproveDisabling[i] = (owfnTransition *) 0;
  PN->transNrQuasiEnabled = 0;
 
  set_hashchange(PN);
#ifdef STUBBORN
	stamp =0;
	NextStubborn = 0;
	instubborn = false;
	
	// create list of conflicting transitions
	PN -> startOfEnabledList = (owfnTransition *) 0;
	PN->transNrEnabled = 0;
	for(i=0;PrePlaces[i];i++)
	{
		for(j=0;j<PrePlaces[i]->NrOfLeaving;j++)
		{
			if(PrePlaces[i]->LeavingArcs[j]->tr != this)
			{
				if(!(PrePlaces[i]->LeavingArcs)[j]->tr -> enabled)
				{
					(PrePlaces[i]->LeavingArcs)[j]->tr->NextEnabled = PN->startOfEnabledList;
					PN->startOfEnabledList = (PrePlaces[i]->LeavingArcs)[j]->tr;
					PN->transNrEnabled ++;
					(PrePlaces[i]->LeavingArcs)[j]->tr->enabled = true;
				}
			}
		}
	}
	conflicting = new owfnTransition * [PN->transNrEnabled + 1];
	for(i=0;PN->startOfEnabledList;PN->startOfEnabledList = PN->startOfEnabledList ->NextEnabled,i++)
	{
		PN->startOfEnabledList ->enabled = false;
		conflicting[i] = PN->startOfEnabledList;
	}
	conflicting[i] = (owfnTransition *) 0;
	mustbeincluded = conflicting;
#endif
  PN->transNrEnabled = 0;
}


void owfnTransition::fire(oWFN * PN) {
	owfnPlace* * p;
	owfnTransition ** t;
	unsigned int * i;
	unsigned int a,b;

	trace(TRACE_5, "owfnTransition::fire(oWFN * PN) : start\n");
			
	for(p = IncrPlaces,i = Incr; *p != NULL; p++,i++) {
		PN->CurrentMarking[(* p)->index] += * i;
#ifdef CHECKCAPACITY
		if(PN->CurrentMarking[(* p)->index] > (* p)->capacity) {
			cerr << "capacity of place " << (* p)->name << " exceeded!" << endl;
			_exit(4);
		}
#endif
	}
	for(p = DecrPlaces, i = Decr; * p != NULL; p++,i++) {
		if (PN->CurrentMarking[(* p)->index] < *i) {
			PN->printmarking();
      		cerr << "marking of place " << (* p)->name << " is: " << PN->CurrentMarking[(* p)->index] << " but transition " << this->name << " consumes: " << *i << endl;
			cerr << "number of states calculated so far: " << State::card << endl;
			_exit(4);
		} else {
			PN->CurrentMarking[(* p)->index] -= * i;
		}
    }


	PN->placeHashValue += hash_change;
	PN->placeHashValue %= HASHSIZE;
	for(t = ImproveEnabling; *t; t++) {
		if(!((*t) -> enabled) || !((*t) -> quasiEnabled)) {
			(*t)->check_enabled(PN);
		}
	}
	for(t = ImproveDisabling; *t; t++) {
		if((*t)->enabled || (*t)->quasiEnabled) {
			(*t)->check_enabled(PN);
		}
	}


    // update value of formula after having fired t
//    for(p = DecrPlaces; *p != NULL; p++) {
//		unsigned int j;
//        for(j=0; j < (* p) -> cardprop; j++) {
//            if ((* p)->proposition != NULL) {
//	            (* p)->proposition[j] -> update(PN->CurrentMarking[(* p)->index]);
//			}
//        }
//    }
//    
//    for(p = IncrPlaces; * p != NULL; p++) {
//		unsigned int j;
//        for(j=0; j < (* p) -> cardprop; j++) {
//            if ((* p)->proposition != NULL) {
//	            (* p)->proposition[j] -> update(PN->CurrentMarking[(* p)->index]);
//            }
//        }
//    }
    
   	trace(TRACE_5, "owfnTransition::fire(oWFN * PN) : end\n");
}

//void owfnTransition::backfire(oWFN * PN)
//{
//  PN->placeHashValue -= hash_change;
//  PN->placeHashValue %= HASHSIZE;
// 
//}

void owfnTransition::backfire(oWFN * PN)
{
  owfnPlace* * p;
  owfnTransition ** t;
  unsigned int * i;

/*  for(p = IncrPlaces,i = Incr; *p < UINT_MAX; p++,i++)
    {
      PN->CurrentMarking[* p] -= * i;
    }
  for(p = DecrPlaces,i = Decr; * p < UINT_MAX; p++,i++)
    {
      PN->CurrentMarking[* p] += * i;
    }
  PN->placeHashValue -= hash_change;
  PN->placeHashValue %= HASHSIZE;
*/ 
  for(t = ImproveEnabling;*t;t++)
    {
      if((*t) -> enabled)
	{
	  (*t)->check_enabled(PN);
	}
    }
  for(t = ImproveDisabling;*t;t++)
    {
      if(!((*t)->enabled))
	{
	  (*t)->check_enabled(PN);
	}
    }


    // update value of formula after having fired t
//    for(p = DecrPlaces,i = Decr; * p != NULL; p++,i++)
//    {
//	unsigned int j;
//        for(j=0; j < (* p) -> cardprop;j++)
//	{
//            (* p)->proposition[j] -> update(PN->CurrentMarking[(* p)->index]);
//        }
//    }
//    for(p = IncrPlaces,i = Incr; * p != NULL; p++,i++)
//    {
//	unsigned int j;
//        for(j=0; j < (* p) -> cardprop;j++)
//	{
//	  (* p)->proposition[j] -> update(PN->CurrentMarking[(* p)->index]);
//        }
//    }
}

void owfnTransition::excludeTransitionFromEnabledList(oWFN * PN) {
	// exclude transition from list of enabled transitions
	if(NextEnabled) {
		NextEnabled -> PrevEnabled = PrevEnabled;
	}
	if(PrevEnabled) {
		PrevEnabled -> NextEnabled = NextEnabled;
	} else {
		PN->startOfEnabledList = NextEnabled;
	}
}

void owfnTransition::excludeTransitionFromQuasiEnabledList(oWFN * PN) {

	// exclude transition from list of quasi enabled transitions
	if(NextQuasiEnabled) {
		NextQuasiEnabled -> PrevQuasiEnabled = PrevQuasiEnabled;
	}
	if(PrevQuasiEnabled) {
		PrevQuasiEnabled -> NextQuasiEnabled = NextQuasiEnabled;
	} else {
		PN->startOfQuasiEnabledList = NextQuasiEnabled;
	}
}

//! \fn void owfnTransition::check_enabled(oWFN * PN)
//! \param PN owfn this transition is part of
//! \brief check whether this transition is (quasi) enabled at the current marking
//! quasi-enabled means, that this transition activates a sending event (this transition is a receiving transition)
void owfnTransition::check_enabled(oWFN * PN) {
	owfnPlace* * p;
	unsigned int *i;

	messageSet.clear();

	enabledNr = quasiEnabledNr = 0;		// suppose no pre-place has appropriate marking at all
#ifdef STUBBORN
	mustbeincluded = conflicting;
#endif
	for(p = PrePlaces , i = Pre ; *p != NULL; p++ , i++) {
		if(PN->CurrentMarking[(* p)->index] < *i) {
#ifdef STUBBORN
			scapegoat = *p;
			mustbeincluded = (*p)->PreTransitions;
#endif
			if ((* p)->type == INPUT) {
				messageSet.insert((* p)->index);
				quasiEnabledNr++;	// remember that we have found an input pre-place with no appropriate marking
			}
		} else if(PN->CurrentMarking[(* p)->index] >= *i) {
			enabledNr++;	// remember that we have found a pre-place with appropriate marking
		}
	}
	
	if(enabledNr == NrOfArriving) {		// there are as many pre-places appropriatly marked as there are incoming arcs
		if (!enabled || PN->startOfEnabledList == 0) {		// transition was not enabled before
			// include transition into list of enabled transitions
			NextEnabled = PN->startOfEnabledList;
			if(PN->startOfEnabledList) {
				NextEnabled -> PrevEnabled = this;
			}
			PN->startOfEnabledList = this;
			PrevEnabled = (owfnTransition *) 0;
			enabled = true;
			PN->transNrEnabled++;
		}
		if (quasiEnabled) {		// transition was quasi enabled before
			quasiEnabled = false;
			PN->transNrQuasiEnabled--;
			excludeTransitionFromQuasiEnabledList(PN);		// delete transition from list of quasi enabled transtions
		}
	} else if ((enabledNr + quasiEnabledNr) == NrOfArriving) {
		// transition is quasi enabled
		if (!quasiEnabled) {          // transition was not quasi enabled before
			// include transition into list of quasi enabled transitions
			NextQuasiEnabled = PN->startOfQuasiEnabledList;
			if(PN->startOfQuasiEnabledList) {
				NextQuasiEnabled -> PrevQuasiEnabled = this;
			}
			PN->startOfQuasiEnabledList = this;
			PrevQuasiEnabled = (owfnTransition *) 0;
			quasiEnabled = true;
			PN->transNrQuasiEnabled++;
		} 
		if (enabled) {		// transition was enabled before
			enabled = false;
			PN->transNrEnabled--;
			excludeTransitionFromEnabledList(PN);		// delete transition from list of enabled transtions
			
		}
	} else {	// transition is not enabled at all
	    if (enabled) {		// transition was enabled before
			enabled = false;
			PN->transNrEnabled--;
			excludeTransitionFromEnabledList(PN);		// delete transition from list of enabled transtions

		}
		if (quasiEnabled) {  // transition was quasi enabled before
			quasiEnabled = false;
			PN->transNrQuasiEnabled--;
			excludeTransitionFromQuasiEnabledList(PN);	// delete transition from list of quasi enabled transtions
		}
	}
	// cout << "current marking: " << PN->printCurrentMarkingForDot() << endl;
	// cout << "transition " << name << " is quasiEnabled: " << quasiEnabled << " and enabled: " << enabled << endl;
}

