/*!

\file Transition.cc
\author Karsten
\status new
\brief Useful routines for transition specific information

All data that describe attributes of transitions can be found here. General information for a transition
in its role as a node, ar contained in Node.*
*/

#include "Node.h"	
#include "Place.h"	
#include "Transition.h"	
#include "Marking.h"	
#include <cstdlib>

// Dimensions.h has a type index_type that has at least max(nr of places,nr of transitions) elements
#include "Dimensions.h"
#include "FairnessAssumptions.h"
#include "BitSetC.h"

/*!

\brief clean up transitions for valgrind

*/

void deleteTransitions()
{
	free(Transition::Fairness);
	free(Transition::Enabled);
	free(Transition::DeltaHash);
	for(int direction = PRE; direction <= POST; direction++)
	{
		for(index_type i = 0;i<Node::Card[TR];i++)
		{
			free(Transition::MultDeltaT[direction][i]);
			free(Transition::DeltaT[direction][i]);
		}
		free(Transition::CardDeltaT[direction]);
		free(Transition::DeltaT[direction]);
		free(Transition::MultDeltaT[direction]);
	}
	free(Transition::CardConflicting);
	free(Transition::CardBackConflicting);
	for(index_type i = 0; i< Node::Card[TR];i++)
	{
		free(Transition::Conflicting[i]);
		free(Transition::BackConflicting[i]);
	}
	free(Transition::Conflicting);
	free(Transition::BackConflicting);
	free(Transition::PositionScapegoat);
}


/// Check transition for activation
/// 1. scan through pre-places for testing eanbledness
/// 2. if enabled ->disabled, insert in Disabled list of scapegoat
/// 3. if disabled->enabled, Remove from Disabled list of scapegoat
/// 4. if disabled->disabled, perhaps move to other scapegoat
void checkEnabled(index_type t)
{
	// scan through all pre-places
	for(index_type i = 0;i < Node::CardArcs[TR][PRE][t]; i++)
	{
		if(Marking::Current[Node::Arc[TR][PRE][t][i]] < Node::Mult[TR][PRE][t][i])
		{
			// transition is disabled, Node::Arc[TR][PRE][t][i] is new scapegpat
			index_type scapegoat = Node::Arc[TR][PRE][t][i];
			if(Transition::Enabled[t])
			{
				// enabled --> disabled: insert to scapegoat's disabled list
				Transition::Enabled[t] = false;
				--Transition::CardEnabled;
				Place::Disabled[scapegoat][Transition::PositionScapegoat[t]=Place::CardDisabled[scapegoat]++] = t;
	
				// swap scapegoat to front of transition's PRE list
				if(i>0)
				{
					mult_type tmp = Node::Mult[TR][PRE][t][i];
					Node::Arc[TR][PRE][t][i] = Node::Arc[TR][PRE][t][0];
					Node::Mult[TR][PRE][t][i] = Node::Mult[TR][PRE][t][0];
					Node::Arc[TR][PRE][t][0] = scapegoat;
					Node::Mult[TR][PRE][t][0] = tmp;
				}
			}
			else
			{
				//disabled --> disabled: perhaps scapegoat has changed
				if(i>0)
				{
					//indeed, scapegoat has changed.
					//remove from old scapegoat's Disabled list
					index_type old_scapegoat = Node::Arc[TR][PRE][t][0];
					if(Transition::PositionScapegoat[t] != --Place::CardDisabled[old_scapegoat])
					{
						// transition not last in scapegoat's disabed list--> swap with last
						index_type other_t = Place::Disabled[old_scapegoat][Place::CardDisabled[old_scapegoat]];
						Transition::PositionScapegoat[other_t] = Transition::PositionScapegoat[t];
						Place::Disabled[old_scapegoat][Transition::PositionScapegoat[t]] = other_t;
					}
					// insert to new scapegoat's disabled list
					Place::Disabled[scapegoat][Transition::PositionScapegoat[t]=Place::CardDisabled[scapegoat]++] = t;
					// swap scapegoat to front of transition's PRE list
					if(i>0)
					{
						mult_type tmp = Node::Mult[TR][PRE][t][i];
						Node::Arc[TR][PRE][t][i] = Node::Arc[TR][PRE][t][0];
						Node::Mult[TR][PRE][t][i] = Node::Mult[TR][PRE][t][0];
						Node::Arc[TR][PRE][t][0] = scapegoat;
						Node::Mult[TR][PRE][t][0] = tmp;
					}

				}
				
			}
		}
	} 
	//transition enabled
	if(!Transition::Enabled[t])
	{
		// disabled-->enabled: remove from scapegoat's disabled list
		Transition::Enabled[t]=true;
		++Transition::CardEnabled;
		index_type old_scapegoat = Node::Arc[TR][PRE][t][0];
		if(Transition::PositionScapegoat[t] != --Place::CardDisabled[old_scapegoat])
		{
			// transition not last in scapegoat's disabed list--> swap with last
			index_type other_t = Place::Disabled[old_scapegoat][Place::CardDisabled[old_scapegoat]];
			Transition::PositionScapegoat[other_t] = Transition::PositionScapegoat[t];
			Place::Disabled[old_scapegoat][Transition::PositionScapegoat[t]] = other_t;
		}
	}
}


/// fire a transition and update enabledness of all transitions
void fire(index_type t)
{
	// 1. Update current marking
	for(index_type i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
	{
		Marking::Current[Transition::DeltaT[PRE][t][i]] -=  Transition::MultDeltaT[PRE][t][i];
	}
	for(index_type i = 0; i < Transition::CardDeltaT[POST][t]; i++)
	{
		Marking::Current[Transition::DeltaT[POST][t][i]] +=  Transition::MultDeltaT[POST][t][i];
	}
	// 2. check conflicting enabled transitions for enabledness
	
	for(index_type i = 0; i < Transition::CardConflicting[t];i++)
	{
		index_type tt;
		if(Transition::Enabled[tt = Transition::Conflicting[t][i]])
		checkEnabled(tt);
	}	
	// 3. check those transitions where the scapegoat received additional tokens
	for(index_type i = 0; i < Transition::CardDeltaT[POST][t]; i++)
	{
		for(index_type j=0; j < Place::CardDisabled[Transition::DeltaT[POST][t][i]]; /* tricky increment handling */)
		{
			index_type tt;
			checkEnabled(Place::Disabled[tt=Transition::DeltaT[POST][t][i]][j]);
			if(!Transition::Enabled[tt])
			{
				j++;
			}
			else
			{
				// in this case, checkEnabled decremente CardDisabled and swaps 
				// another transition to position j
			}
		}
	}
	// 4. update hash value
	Marking::HashCurrent += Transition::DeltaHash[t];
	Marking::HashCurrent %= SIZEOF_MARKINGTABLE;
}

/// fire a transition in reverse direction (for backtracking) and update enabledness of all transitions
void backfire(index_type t)
{
	// 1. Update current marking
	for(index_type i = 0; i < Transition::CardDeltaT[PRE][t]; i++)
	{
		Marking::Current[Transition::DeltaT[PRE][t][i]] +=  Transition::MultDeltaT[PRE][t][i];
	}
	for(index_type i = 0; i < Transition::CardDeltaT[POST][t]; i++)
	{
		Marking::Current[Transition::DeltaT[POST][t][i]] -=  Transition::MultDeltaT[POST][t][i];
	}
	// 2. check backward conflicting enabled transitions for enabledness
	
	for(index_type i = 0; i < Transition::CardBackConflicting[t];i++)
	{
		index_type tt;
		if(Transition::Enabled[tt = Transition::BackConflicting[t][i]])
		checkEnabled(tt);
	}	
	// 3. check those transitions where the scapegoat received additional tokens
	for(index_type i = 0; i < Transition::CardDeltaT[PRE][t];i++)
	{
		for(index_type j=0; j < Place::CardDisabled[Transition::DeltaT[PRE][t][i]]; /* tricky increment handling */)
		{
			index_type tt;
			checkEnabled(Place::Disabled[tt=Transition::DeltaT[PRE][t][i]][j]);
			if(!Transition::Enabled[tt])
			{
				j++;
			}
			else
			{
				// in this case, checkEnabled decremente CardDisabled and swaps 
				// another transition to position j
			}
		}
	}
	// 4. update hash value
	Marking::HashCurrent -= Transition::DeltaHash[t];
	Marking::HashCurrent %= SIZEOF_MARKINGTABLE;
}


tFairnessAssumption * Transition::Fairness = NULL;
bool * Transition::Enabled = NULL;
index_type Transition::CardEnabled = 0;
int * Transition::DeltaHash = NULL;
index_type * Transition::CardDeltaT[2] = {NULL};
index_type ** Transition::DeltaT[2] = {NULL};
mult_type ** Transition::MultDeltaT[2] = {NULL};
index_type * Transition::CardConflicting = NULL;
index_type ** Transition::Conflicting = NULL;
index_type * Transition::CardBackConflicting = NULL;
index_type ** Transition::BackConflicting = NULL;
index_type * Transition::PositionScapegoat = NULL;
