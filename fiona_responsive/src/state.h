/*****************************************************************************
 * Copyright 2005, 2006, 2007 Karsten Wolf, Peter Massuthe, Daniela Weinberg *
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
 * \file    state.h
 *
 * \brief   functions for Petri net states
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef STATE_H
#define STATE_H

/// Possible types of states
enum stateType {
    TRANS,          ///< there is at least one enabled transition
    DEADLOCK,       ///< no transition is enabled (may be internal or external DL)
    FINALSTATE,     ///< state fulfills finalmarking or finalcondition (may be transient, too)
    I_DEADLOCK,     ///< internal deadlock (used by exactType())
    E_DEADLOCK      ///< external deadlock (used by exactType())
};

class binDecision;
//class owfnTransition;

// Include owfn.h after declaration of stateType to avoid cyclic dependencies!
#include "owfn.h"

// State objects are nodes in the state graph.
class State {
    public:
        /// constructor
        State();
        
        /// destructor
        ~State();

        /// number of states
        static unsigned int state_count;
        
        /// number of states
        static unsigned int state_count_stored_in_binDec;

        /// number of transitions to be fired
        unsigned int cardFireList;

        /// Transitions to be fired 
        /// note: in case of -R (state reduction in nodes) this list contains
        /// transitions, that are stubborn!!!
        owfnTransition ** firelist;

        /// Transitions that are quasi enabled
        owfnTransition ** quasiFirelist;

        /// Nr of last already fired element of firelist
        unsigned int current;

        /// last branch in binary decision tree that represents marking
        binDecision * my_entry;

        /// hashValue of that state (necessary for binDecision)
        unsigned int placeHashValue;

        /// successor states in graph
        State ** succ;

        /// state responsible for first generation
        State * parent;

        /// type of state (Deadlock, Final, Transient)
        stateType type;
        
        /// returns exact type of state (Final, iDL, eDL, TR)
        stateType exactType();

        /// Same as decodeShowOnly(), but initializes all transition of the given 'PN'.
        void decode(oWFN* PN);

        /// Decodes State in bintree and writes the corresponding marking into
        /// the CurrentMarking of the given 'PN'.
        void decodeShowOnly(oWFN* PN);

        /// Returns true iff this state has an enabled tau labeled transition.
        bool hasLeavingTauTransitionForMatching() const;
    
        /// is transient in autonomous setting
        bool isNotAutonomouslyTransient() const;

        /// TARJAN's values
        /// proposition: if s is start state of an SCC, then s.dfs == s.lowlink
        /// 			 and we consider s to be the representative of the SCC
        unsigned int dfs, lowlink; 

        /// true,  iff this state is a representative of a TERMINAL SCC
        /// false, iff 
        ///			(a) this state is not a representative at all, and/or
        ///		    (b) this state is a representative of an SCC
        bool repTSCC;	
        
        State * nexttar, * prevtar;
		unsigned int tarlevel;
        
#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(State)
#define new NEW_NEW
    };
    
#endif
