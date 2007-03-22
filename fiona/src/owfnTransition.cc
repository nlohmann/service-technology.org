/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    owfnTransition.cc
 *
 * \brief   functions for Petri net transitions
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitšt zu Berlin. See
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
#include "OGFromFile.h"

AdjacentPlace::AdjacentPlace(owfnPlace* place, unsigned int multiplicity) :
    place_(place),
    multiplicity_(multiplicity)
{
}

owfnPlace* AdjacentPlace::getOwfnPlace() const
{
    return place_;
}

unsigned int AdjacentPlace::getMultiplicity() const
{
    return multiplicity_;
}

owfnTransition::owfnTransition(const std::string& name) :
    Node(name),
    labelForMatching(CommGraphFormulaAssignment::TAU),
    isEnabled_(false),
    isQuasiEnabled_(false),
    quasiEnabledNr(0),
    enabledNr(0),
#ifdef STUBBORN
    conflicting(NULL),
#endif
    NextEnabled(NULL),
    PrevEnabled(NULL),
    NextQuasiEnabled(NULL),
    PrevQuasiEnabled(NULL)
{
}

owfnTransition::~owfnTransition() {
#ifdef STUBBORN
	delete [] conflicting;
#endif
}

bool owfnTransition::isEnabled() const
{
    return isEnabled_;
}

void owfnTransition::setEnabled(bool isEnabled)
{
    isEnabled_ = isEnabled;
}

bool owfnTransition::isQuasiEnabled() const
{
    return isQuasiEnabled_;
}

void owfnTransition::setQuasiEnabled(bool isQuasiEnabled)
{
    isQuasiEnabled_ = isQuasiEnabled;
}

#ifdef STUBBORN
bool owfnTransition::prePlaceIsScapegoatForDisabledness(AdjacentPlace prePlace)
    const
{
    return (PN->CurrentMarking[prePlace.getOwfnPlace()->index] <
            prePlace.getMultiplicity());
}
#endif

void owfnTransition::set_hashchange() {
	hash_change = 0;
	for(AdjacentPlaces_t::size_type i = 0; i != IncrPlaces.size(); ++i) {
		hash_change += IncrPlaces[i].getMultiplicity() *
		               IncrPlaces[i].getOwfnPlace()->hash_factor;
	}
	for(AdjacentPlaces_t::size_type i = 0; i != DecrPlaces.size(); ++i) {
		hash_change -= DecrPlaces[i].getMultiplicity() *
		               DecrPlaces[i].getOwfnPlace()->hash_factor;
	}
	hash_change %= HASHSIZE;
}

void owfnTransition::initialize(oWFN * PN) {

  unsigned int i,j,k;
  // Create list of Pre-Places for enabling test
  for (i = 0; i < NrOfArriving; ++i)
  {
      PrePlaces.push_back(
          AdjacentPlace(ArrivingArcs[i]->pl, ArrivingArcs[i]->Multiplicity)
      );
  }
  // Create list of places where transition increments marking
  
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
	      IncrPlaces.push_back(
	          AdjacentPlace(
	              LeavingArcs[i]->pl,
	              LeavingArcs[i]->Multiplicity - ArrivingArcs[j]->Multiplicity
	          )
	      );
	    } 
	}
      else
	{
	  // no loop place
	  IncrPlaces.push_back(
	      AdjacentPlace(LeavingArcs[i]->pl, LeavingArcs[i]->Multiplicity)
	  );
	}
    }
  // Create list of places where transition decrements marking
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
	      DecrPlaces.push_back(
	          AdjacentPlace(
	              ArrivingArcs[i]->pl,
	              ArrivingArcs[i]->Multiplicity - LeavingArcs[j]->Multiplicity
	          )
	      );
	    } 
	}
      else
	{
	  // no loop place
	  DecrPlaces.push_back(
	      AdjacentPlace(ArrivingArcs[i]->pl, ArrivingArcs[i]->Multiplicity)
	  );
	}
    }
    // Create list of transitions where enabledness can change
    // if this transition fires.
    for (AdjacentPlaces_t::size_type i = 0; i != IncrPlaces.size(); ++i)
    {
        owfnPlace* incrOwfnPlace = IncrPlaces[i].getOwfnPlace();
        for (j = 0; j < incrOwfnPlace->NrOfLeaving;j++)
        {
            if (!(incrOwfnPlace->LeavingArcs)[j]->tr->isEnabled())
            {
                // not yet in list
                ImproveEnabling.push_back((incrOwfnPlace->LeavingArcs)[j]->tr);
            }
        }
    }

    // Create list of transitions where enabledness can change
    // if this transition fires.
    for (AdjacentPlaces_t::size_type i = 0; i != DecrPlaces.size(); ++i)
    {
        owfnPlace* decrOwfnPlace = DecrPlaces[i].getOwfnPlace();
        for(j=0;j < decrOwfnPlace->NrOfLeaving;j++)
        {
            if (!(decrOwfnPlace->LeavingArcs)[j]->tr->isEnabled())
            {
                // not yet in list
                ImproveDisabling.push_back((decrOwfnPlace->LeavingArcs)[j]->tr);
            }
        }
    }

  PN->transNrQuasiEnabled = 0;
 
  set_hashchange();
#ifdef STUBBORN
	stamp =0;
	NextStubborn = 0;
	instubborn = false;
	
	// create list of conflicting transitions
	PN -> startOfEnabledList = (owfnTransition *) 0;
	PN->transNrEnabled = 0;
	for (i = 0; i != PrePlaces.size(); ++i)
	{
		AdjacentPlace prePlace = PrePlaces[i];
		owfnPlace* preOwfnPlace = prePlace.getOwfnPlace();
		for(j = 0; j < preOwfnPlace->NrOfLeaving; j++)
		{
			if(preOwfnPlace->LeavingArcs[j]->tr != this)
			{
				if(!(preOwfnPlace->LeavingArcs)[j]->tr ->isEnabled())
				{
					(preOwfnPlace->LeavingArcs)[j]->tr->NextEnabled = PN->startOfEnabledList;
					PN->startOfEnabledList = (preOwfnPlace->LeavingArcs)[j]->tr;
					PN->transNrEnabled ++;
					(preOwfnPlace->LeavingArcs)[j]->tr->setEnabled(true);
				}
			}
		}
	}
	conflicting = new owfnTransition * [PN->transNrEnabled + 1];
	for(i=0;PN->startOfEnabledList;PN->startOfEnabledList = PN->startOfEnabledList ->NextEnabled,i++)
	{
		PN->startOfEnabledList ->setEnabled(false);
		conflicting[i] = PN->startOfEnabledList;
	}
	conflicting[i] = (owfnTransition *) 0;
	mustbeincluded = conflicting;
#endif
  PN->transNrEnabled = 0;
}


void owfnTransition::fire(oWFN * PN) {

	trace(TRACE_5, "owfnTransition::fire(oWFN * PN) : start\n");
			
	for (AdjacentPlaces_t::size_type i = 0; i != IncrPlaces.size(); ++i) {
		AdjacentPlace incrPlace = IncrPlaces[i];
		owfnPlace* incrOwfnPlace = incrPlace.getOwfnPlace();
		PN->CurrentMarking[incrOwfnPlace->index] += incrPlace.getMultiplicity();
#ifdef CHECKCAPACITY
		if(PN->CurrentMarking[incrOwfnPlace->index] > incrOwfnPlace->capacity) {
			cerr << "capacity of place " << incrOwfnPlace->name << " exceeded!" << endl;
			_exit(4);
		}
#endif
	}
	for (AdjacentPlaces_t::size_type i = 0; i != DecrPlaces.size(); ++i) {
		AdjacentPlace decrPlace = DecrPlaces[i];
        owfnPlace* decrOwfnPlace = decrPlace.getOwfnPlace();
		if (PN->CurrentMarking[decrOwfnPlace->index] < decrPlace.getMultiplicity()) {
			PN->printCurrentMarking();
      		cerr << "marking of place " << decrOwfnPlace->name << " is: " << PN->CurrentMarking[decrOwfnPlace->index] << " but transition " << this->name << " consumes: " << decrPlace.getMultiplicity() << endl;
			cerr << "number of states calculated so far: " << State::card << endl;
			_exit(4);
		} else {
			PN->CurrentMarking[decrOwfnPlace->index] -= decrPlace.getMultiplicity();
		}
    }


	PN->placeHashValue += hash_change;
	PN->placeHashValue %= HASHSIZE;
	for (ImproveDisEnabling_t::size_type i = 0; i != ImproveEnabling.size();
	     ++i) {
		owfnTransition* transition = ImproveEnabling[i];
		if (!(transition->isEnabled()) || !(transition->isQuasiEnabled())) {
			transition->check_enabled(PN);
		}
	}
	for(ImproveDisEnabling_t::size_type i = 0; i != ImproveDisabling.size();
		++i) {
		owfnTransition* transition = ImproveDisabling[i];
		if(transition->isEnabled() || transition->isQuasiEnabled()) {
			transition->check_enabled(PN);
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

/*
  owfnPlace* * p;
  unsigned int * i;
  for(p = IncrPlaces,i = Incr; *p < UINT_MAX; p++,i++)
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

    for (ImproveDisEnabling_t::size_type i = 0; i != ImproveEnabling.size();
         ++i)
    {
        owfnTransition* transition = ImproveEnabling[i];
        if (transition->isEnabled())
        {
            transition->check_enabled(PN);
        }
    }

    for (ImproveDisEnabling_t::size_type i = 0; i != ImproveDisabling.size();
         ++i)
    {
        owfnTransition* transition = ImproveDisabling[i];
        if (!transition->isEnabled())
        {
            transition->check_enabled(PN);
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
   	trace(TRACE_5, "owfnTransition::check_enabled(oWFN * PN) : start\n");

	messageSet.clear();

	enabledNr = quasiEnabledNr = 0;		// suppose no pre-place has appropriate marking at all
#ifdef STUBBORN
	mustbeincluded = conflicting;
#endif
	for (AdjacentPlaces_t::size_type iPrePlace = 0; iPrePlace != PrePlaces.size();
	     ++iPrePlace) {
		AdjacentPlace prePlace = PrePlaces[iPrePlace];
		owfnPlace* preOwfnPlace = prePlace.getOwfnPlace();
		if (prePlaceIsScapegoatForDisabledness(prePlace)) {
#ifdef STUBBORN
			scapegoat = preOwfnPlace;
			mustbeincluded = preOwfnPlace->PreTransitions;
#endif
			if (preOwfnPlace->type == INPUT) {
				messageSet.insert(preOwfnPlace->index);
				quasiEnabledNr++;	// remember that we have found an input pre-place with no appropriate marking
			}
		} else {
			enabledNr++;	// remember that we have found a pre-place with appropriate marking
		}
	}
	
	if(enabledNr == NrOfArriving) {		// there are as many pre-places appropriatly marked as there are incoming arcs
		if (!isEnabled() || PN->startOfEnabledList == 0) {		// transition was not enabled before
			// include transition into list of enabled transitions
			NextEnabled = PN->startOfEnabledList;
			if(PN->startOfEnabledList) {
				NextEnabled -> PrevEnabled = this;
			}
			PN->startOfEnabledList = this;
			PrevEnabled = (owfnTransition *) 0;
			setEnabled(true);
			PN->transNrEnabled++;
		}
		if (isQuasiEnabled()) {		// transition was quasi enabled before
			setQuasiEnabled(false);
			PN->transNrQuasiEnabled--;
			excludeTransitionFromQuasiEnabledList(PN);		// delete transition from list of quasi enabled transtions
		}
	} else if ((enabledNr + quasiEnabledNr) == NrOfArriving) {
		// transition is quasi enabled
		if (!isQuasiEnabled()) {  // transition was not quasi enabled before
			// include transition into list of quasi enabled transitions
			NextQuasiEnabled = PN->startOfQuasiEnabledList;
			if(PN->startOfQuasiEnabledList) {
				NextQuasiEnabled -> PrevQuasiEnabled = this;
			}
			PN->startOfQuasiEnabledList = this;
			PrevQuasiEnabled = (owfnTransition *) 0;
			setQuasiEnabled(true);
			PN->transNrQuasiEnabled++;
		} 
		if (isEnabled()) {		// transition was enabled before
			setEnabled(false);
			PN->transNrEnabled--;
			excludeTransitionFromEnabledList(PN);		// delete transition from list of enabled transtions
			
		}
	} else {	// transition is not enabled at all
	    if (isEnabled()) {		// transition was enabled before
			setEnabled(false);
			PN->transNrEnabled--;
			excludeTransitionFromEnabledList(PN);		// delete transition from list of enabled transtions

		}
		if (isQuasiEnabled()) {  // transition was quasi enabled before
			setQuasiEnabled(false);
			PN->transNrQuasiEnabled--;
			excludeTransitionFromQuasiEnabledList(PN);	// delete transition from list of quasi enabled transtions
		}
	}
	// cout << "current marking: " << PN->getCurrentMarkingAsString() << endl;
	// cout << "transition " << name << " is quasiEnabled: " << isQuasiEnabled() << " and enabled: " << enabled << endl;

   	trace(TRACE_5, "owfnTransition::check_enabled(oWFN * PN) : end\n");
}

void owfnTransition::setLabelForMatching(const std::string& label)
{
    labelForMatching = label;
}

std::string owfnTransition::getLabelForMatching() const
{
    return labelForMatching;
}

bool owfnTransition::hasNonTauLabelForMatching() const
{
    return getLabelForMatching() != CommGraphFormulaAssignment::TAU;
}
