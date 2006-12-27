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
 * \file    owfnTransition.h
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

#ifndef OWFNTRANSITION_H_
#define OWFNTRANSITION_H_

#include "mynew.h"
#include "petriNetNode.h"
#include <set>

class oWFN;
class owfnPlace;

class owfnTransition : public Node {
    private:
        std::string labelForMatching;
	public:
		owfnTransition(char *);
		~owfnTransition();
	
		bool quasiEnabled;
		bool enabled;

		std::set<unsigned int> messageSet;
		
		unsigned int quasiEnabledNr;	//!< number of internal pre-places marked with appropriate tokens
		unsigned int enabledNr;			//!< number of input pre-places marked with appropriate tokens
		
		owfnTransition * NextEnabled;
		owfnTransition * PrevEnabled; 	// double linking in list of enabled transitions
  										// in the sequel, lists are NIL-terminated
		owfnTransition * NextQuasiEnabled;
		owfnTransition * PrevQuasiEnabled; 	// double linking in list of quasi enabled transitions
  											// in the sequel, lists are NIL-terminated								

		owfnPlace* * PrePlaces; // Places to be checked for enabledness
		unsigned int * Pre; // Multiplicity to be checked
		owfnPlace* * IncrPlaces; // Places that are incremented by transition
		unsigned int * Incr; // Amount of increment
		owfnPlace* * DecrPlaces; // Places that are decremented by transition
		unsigned int * Decr; // amount of decrement
		
		owfnTransition ** ImproveEnabling; 	// list of transitions where enabledness
  											//must be checked again after firing this transition
  		owfnTransition ** ImproveDisabling; // list of transitions where disabledness
  											// must be checked again after firing this transition
  		void initialize(oWFN *); // Set above arrays, list, enabled...
  		void fire(oWFN *); // replace current marking by successor marking, force
  								// enabling test where necessary
  		void backfire(oWFN * PN); // fire transition backwards to replace original state,
  										// force enabling tests where necessary
  										
  		void excludeTransitionFromEnabledList(oWFN * );
  		void excludeTransitionFromQuasiEnabledList(oWFN * );
  		
  		void check_enabled(oWFN *); // test if tr is enabled. If necessary, rearrange list
		int hash_change; // change of hash value by firing t;
		void set_hashchange(oWFN *);
		unsigned int lastdisabled; 	// dfsnum of last state where
							  		// some fired transition disables this one
		unsigned int lastfired; 	// dfsnum of last state where this tr. was fired

// *** Definitions for stubborn set calculations
#ifdef STUBBORN
		owfnTransition * NextStubborn;    	// elements of stubborn set are organized as linked list
		bool instubborn;		// ... and marked
		owfnPlace * scapegoat;		// an insufficiently marked pre-place, if this disabled
		owfnTransition ** mustbeincluded;	// If this is in stubborn set, so must be the ones in array
		owfnTransition ** conflicting;	// conflicting transitions, for use as mustbeincluded
		unsigned int dfs;		// stubborn sets are calculated through scc detection
		unsigned int min;		// = Tarjan's lowlink
		unsigned int stamp;		// used to mark visited transitions
		unsigned int mbiindex;		// currently processed index in mustbeincluded
		owfnTransition * nextontarjanstack; // stack organized as list
		owfnTransition * nextoncallstack;	// stack organized as list 
#endif

        void setLabelForMatching(const std::string& label);
        std::string getLabelForMatching() const;
        bool hasNonTauLabelForMatching() const;

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(owfnTransition)
#define new NEW_NEW
};

#endif /*OWFNTRANSITION_H_*/
