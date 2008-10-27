/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    state.h
 *
 * \brief   functions for Petri net states
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
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

        /// transitions to be fired 
        /// note: in case of -R (state reduction in nodes) this list contains
        /// transitions, that are stubborn!!!
        owfnTransition ** firelist;

        /// transitions that are quasi enabled
        owfnTransition ** quasiFirelist;

        /// nr of last already fired element of firelist
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
        bool hasEnabledTransitionWithTauLabelForMatching() const;
    
        /// is transient in autonomous setting
        bool isNotAutonomouslyTransient() const;

        /// TARJAN's values, needed to calculate the (T)SCCs
        /// proposition: if s is start state of an (T)SCC, then s.dfs == s.lowlink
        /// 			 and we consider s to be the representative of the (T)SCC

        /// depth first search number
        unsigned int dfs;
        
        /// the ID of the SCC
        unsigned int lowlink; 

        /// true,  iff this state is a representative of a TERMINAL SCC
        /// false, iff 
        ///			(a) this state is not a representative at all, and/or
        ///		    (b) this state is a representative of an SCC
        bool repTSCC;	
        
        /// previous and next state in the depth first search graph
        /// it may lead to a state outside of the current SCC!
        State * nexttar, * prevtar;
        
		unsigned int tarlevel;
        
#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(State)
#define new NEW_NEW
    };
    
#endif
