/*!
 * 	\class owfnTransition
    \brief implementation class for the transitions of an owfn net

	\author Daniela Weinberg
	\par History
		- 2006-01-16 creation

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

	if (ImproveEnabling) {
		delete ImproveEnabling;
	}
	if (ImproveDisabling) {
		delete ImproveDisabling;
	}
	if (IncrPlaces) {
		delete IncrPlaces;
	}
	if (Incr) {
		delete Incr;
	}
	if (DecrPlaces) {
		delete DecrPlaces;
	}
	if (Decr) {
		delete Decr;
	}
	
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
	for(i = 0; IncrPlaces[i] < PN->getPlaceCnt(); i++) {
		hash_change += Incr[i] * PN->Places[IncrPlaces[i]]->hash_factor;
	}
	for(i = 0; DecrPlaces[i] < PN->getPlaceCnt(); i++) {
		hash_change -= Decr[i] * PN->Places[DecrPlaces[i]]->hash_factor;
	}
	hash_change %= HASHSIZE;
}

void owfnTransition::initialize(oWFN * PN) {

  unsigned int i,j,k;

  // Create list of Pre-Places for enabling test
  PrePlaces = new unsigned int  [NrOfArriving + 1];
  Pre = new unsigned int [NrOfArriving + 1];
  for(i = 0; i < NrOfArriving;i++)
    {
      PrePlaces[i] = ArrivingArcs[i]->pl->index;
      Pre[i] = ArrivingArcs[i]->Multiplicity;
    }
  PrePlaces[NrOfArriving] = UINT_MAX;
  // Create list of places where transition increments marking
  IncrPlaces = new unsigned int [NrOfLeaving + 1];
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
	      IncrPlaces[k] = LeavingArcs[i]->pl->index;
	      Incr[k] = LeavingArcs[i]->Multiplicity - ArrivingArcs[j]->Multiplicity;
	      k++;
	    } 
	}
      else
	{
	  // no loop place
	  IncrPlaces[k] = LeavingArcs[i]->pl->index;
	  Incr[k] = LeavingArcs[i]->Multiplicity;
	  k++;
	}
    }
  IncrPlaces[k] = UINT_MAX;
  Incr[k] = 0;
  // Create list of places where transition decrements marking
  DecrPlaces = new unsigned int [NrOfArriving + 1];
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
	      DecrPlaces[k] = ArrivingArcs[i]->pl->index;
	      Decr[k] = ArrivingArcs[i]->Multiplicity - LeavingArcs[j]->Multiplicity;
	      k++;
	    } 
	}
      else
	{
	  // no loop place
	  DecrPlaces[k] = ArrivingArcs[i]->pl->index;
	  Decr[k] = ArrivingArcs[i]->Multiplicity;
	  k++;
	}
    }
  DecrPlaces[k] = UINT_MAX;
  Decr[k] = 0;
  // Create list of transitions where enabledness can change
  // if this transition fires. For collecting these transitions, we
  // abuse the Enabled linked list
  PN->startOfEnabledList = (owfnTransition *) 0;
  PN->transNrEnabled = 0;
//  PN->startOfQuasiEnabledList = (owfnTransition *) 0;
//  PN->transNrQuasiEnabled = 0;
  for(i=0;IncrPlaces[i] < PN->getPlaceCnt();i++)
    {
      for(j=0;j<PN->Places[IncrPlaces[i]]->NrOfLeaving;j++)
	{
	  if(!(PN->Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->enabled)
	    {
	      // not yet in list
	      (PN->Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->NextEnabled = PN->startOfEnabledList;
	      PN->startOfEnabledList = (PN->Places[IncrPlaces[i]]->LeavingArcs)[j]->tr;
	      PN->transNrEnabled++;
	      (PN->Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->enabled = true;
	    }
//	    if(!(PN->Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->quasiEnabled)
//	    {
//	      // not yet in list
//	      (PN->Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->NextQuasiEnabled = PN->startOfQuasiEnabledList;
//	      PN->startOfQuasiEnabledList = (PN->Places[IncrPlaces[i]]->LeavingArcs)[j]->tr;
//	      PN->transNrQuasiEnabled++;
//	      (PN->Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->quasiEnabled = true;
//	    }
	}
    }

  ImproveEnabling = new owfnTransition * [PN->transNrEnabled + 1];
  for(i=0; PN->startOfEnabledList; PN->startOfEnabledList = PN->startOfEnabledList -> NextEnabled,i++)
    {
      PN->startOfEnabledList->enabled = false;
      ImproveEnabling[i] = PN->startOfEnabledList;
    }
//  for(i; PN->startOfQuasiEnabledList; PN->startOfQuasiEnabledList = PN->startOfQuasiEnabledList -> NextQuasiEnabled,i++)
//    {
//      PN->startOfQuasiEnabledList->quasiEnabled = false;
//      ImproveEnabling[i] = PN->startOfQuasiEnabledList;
//    }
  ImproveEnabling[i] = (owfnTransition *) 0;
  // Create list of transitions where enabledness can change
  // if this transition fires. For collecting these transitions, we
  // abuse the Enabled linked list
  PN->startOfEnabledList = (owfnTransition *) 0;
  PN->transNrEnabled = 0;
//  PN->startOfQuasiEnabledList = (owfnTransition *) 0;
//  PN->transNrQuasiEnabled = 0;
  
  for(i=0;DecrPlaces[i] < PN->getPlaceCnt();i++)
    {
      for(j=0;j < PN->Places[DecrPlaces[i]]->NrOfLeaving;j++)
	{
	  if(!(PN->Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->enabled)
	    {
	      // not yet in list
	      (PN->Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->NextEnabled = PN->startOfEnabledList;
	      PN->startOfEnabledList = (PN->Places[DecrPlaces[i]]->LeavingArcs)[j]->tr;
	      PN->transNrEnabled++;
	      (PN->Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->enabled = true;
	    }
//	     if(!(PN->Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->quasiEnabled)
//	    {
//	      // not yet in list
//	      (PN->Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->NextQuasiEnabled = PN->startOfQuasiEnabledList;
//	      PN->startOfQuasiEnabledList = (PN->Places[DecrPlaces[i]]->LeavingArcs)[j]->tr;
//	      PN->transNrQuasiEnabled++;
//	      (PN->Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->quasiEnabled = true;
//	    }
	}
    }
  	
  	ImproveDisabling = new owfnTransition * [PN->transNrEnabled + 1];
  	
  	for(i=0; PN->startOfEnabledList; PN->startOfEnabledList = PN->startOfEnabledList -> NextEnabled,i++) {
		PN->startOfEnabledList->enabled = false;
		ImproveDisabling[i] = PN->startOfEnabledList;
    }
    
//    for(i; PN->startOfQuasiEnabledList; PN->startOfQuasiEnabledList = PN->startOfQuasiEnabledList -> NextQuasiEnabled,i++) {
//		PN->startOfQuasiEnabledList->quasiEnabled = false;
//		ImproveDisabling[i] = PN->startOfQuasiEnabledList;
//    }
    
  ImproveDisabling[i] = (owfnTransition *) 0;
  PN->transNrEnabled = 0;
  PN->transNrQuasiEnabled = 0;
 
  set_hashchange(PN);
}


void owfnTransition::fire(oWFN * PN) {
  unsigned int * p;
  owfnTransition ** t;
  unsigned int * i;
  unsigned int a,b;

	trace(TRACE_5, "owfnTransition::fire(oWFN * PN) : start\n");
			
  for(p = IncrPlaces,i = Incr; *p < UINT_MAX; p++,i++)
    {
      PN->CurrentMarking[* p] += * i;
#ifdef CHECKCAPACITY
	if(PN->CurrentMarking[*p] > PN->Places[*p]->capacity) 
	{
		cerr << "capacity of place " << PN->Places[*p]->name << " exceeded!" << endl;
		_exit(4);
	}
#endif
    }
  for(p = DecrPlaces, i = Decr; * p < UINT_MAX; p++,i++)
    {
      if (PN->CurrentMarking[*p] < *i) {
      		PN->printmarking();
      		cerr << "marking of place " << PN->Places[*p]->name << " is: " << PN->CurrentMarking[*p] << " but transition " << this->name << " consumes: " << *i << endl;
			cerr << "number of states calculated so far: " << State::card << endl;
		_exit(4);
      } else {
			PN->CurrentMarking[*p] -= * i;
      } 
    }

  
  PN->placeHashValue += hash_change;
  PN->placeHashValue %= HASHSIZE;
  for(t = ImproveEnabling;*t;t++)
    {
      if(!((*t) -> enabled) || !((*t) -> quasiEnabled))
	{
	  (*t)->check_enabled(PN);
	}
    }
  for(t = ImproveDisabling;*t;t++)
    {
      if((*t)->enabled || (*t)->quasiEnabled)
	{
	  (*t)->check_enabled(PN);
	}
    }

    // update value of formula after having fired t

    for(p = DecrPlaces; * p < UINT_MAX; p++) {
		unsigned int j;
        for(j=0; j < PN->Places[*p] -> cardprop; j++) {
            if (PN->Places[*p]->proposition != NULL) {
	            PN->Places[*p]->proposition[j] -> update(PN->CurrentMarking[*p]);
			}
        }
    }
    
    for(p = IncrPlaces; * p < UINT_MAX; p++) {
		unsigned int j;
        for(j=0; j < PN->Places[*p] -> cardprop; j++) {
            if (PN->Places[*p]->proposition != NULL) {
	            PN->Places[*p]->proposition[j] -> update(PN->CurrentMarking[*p]);
            }
        }
    }
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
  unsigned int * p;
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

    for(p = DecrPlaces,i = Decr; * p < UINT_MAX; p++,i++)
    {
	unsigned int j;
        for(j=0; j < PN->Places[*p] -> cardprop;j++)
	{
            PN->Places[*p]->proposition[j] -> update(PN->CurrentMarking[*p]);
        }
    }
    for(p = IncrPlaces,i = Incr; * p < UINT_MAX; p++,i++)
    {
	unsigned int j;
        for(j=0; j < PN->Places[*p] -> cardprop;j++)
	{
            PN->Places[*p]->proposition[j] -> update(PN->CurrentMarking[*p]);
        }
    }
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
	unsigned int * p;
	unsigned int *i;
		
	messageSet.clear();
	
	enabledNr = quasiEnabledNr = 0;		// suppose no pre-place has appropriate marking at all

	for(p = PrePlaces , i = Pre ; *p < UINT_MAX; p++ , i++) {
		if(PN->CurrentMarking[*p] < *i) {
	    	if (PN->Places[*p]->type == INPUT) {
	    		messageSet.insert(*p);
	    		quasiEnabledNr++;	// remember that we have found an input pre-place with no appropriate marking
	    	}
		} else if(PN->CurrentMarking[*p] >= *i) {
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
	      	if (quasiEnabled) {		// transition was quasi enabled before
	      		quasiEnabled = false;
	      		PN->transNrQuasiEnabled--;
	      		excludeTransitionFromQuasiEnabledList(PN);		// delete transition from list of quasi enabled transtions
	      	}
		}
		if (quasiEnabled) {		// transition was quasi enabled before
      		quasiEnabled = false;
      		PN->transNrQuasiEnabled--;
      		excludeTransitionFromQuasiEnabledList(PN);		// delete transition from list of quasi enabled transtions
      	}
    } else if ((enabledNr + quasiEnabledNr) == NrOfArriving) {	// transition is quasi enabled
		if (!quasiEnabled) {	// transition was not quasi enabled before
			// include transition into list of quasi enabled transitions
			NextQuasiEnabled = PN->startOfQuasiEnabledList;
			if(PN->startOfQuasiEnabledList) {
				NextQuasiEnabled -> PrevQuasiEnabled = this;
			}
	      	PN->startOfQuasiEnabledList = this;
	      	PrevQuasiEnabled = (owfnTransition *) 0;
	      	quasiEnabled = true;
	      	PN->transNrQuasiEnabled++;
	      	if (enabled) {		// transition was enabled before
	      		enabled = false;
	      		PN->transNrEnabled--;
	      		excludeTransitionFromEnabledList(PN);		// delete transition from list of enabled transtions
	      	}
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
  	
  //	cout << "current marking: " << PN->printCurrentMarkingForDot() << endl;
  //	cout << "transition " << name << " is quasiEnabled: " << quasiEnabled << " and enabled: " << enabled << endl;
}
