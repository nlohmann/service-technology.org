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

class owfnTransition: public Node
{
    private:
        std::string labelForMatching;

        /** number of internal pre-places marked with appropriate tokens */
        unsigned int quasiEnabledNr;

        /** number of input pre-places marked with appropriate tokens */
        unsigned int enabledNr;

        /** Places to be checked for enabledness */
        owfnPlace* *PrePlaces;

        /** Multiplicity to be checked */
        unsigned int *Pre;

        /** Places that are incremented by transition */
        owfnPlace **IncrPlaces;

        /** Amount of increment */
        unsigned int *Incr;

        /** Places that are decremented by transition */
        owfnPlace **DecrPlaces;

        /** amount of decrement */
        unsigned int *Decr;

        /**
         * list of transitions where enabledness must be checked again after
         * firing this transition
         */
        owfnTransition **ImproveEnabling;

        /**
         * list of transitions where disabledness must be checked again after
         * firing this transition
         */
        owfnTransition **ImproveDisabling;

        void excludeTransitionFromEnabledList(oWFN *);
        void excludeTransitionFromQuasiEnabledList(oWFN *);

        /**  change of hash value by firing t; */
        int hash_change;
        void set_hashchange();

        /**
         * dfsnum of last state where some fired transition disables this one
         */
        unsigned int lastdisabled;

        /** dfsnum of last state where this tr. was fired */
        unsigned int lastfired;

#ifdef STUBBORN
        /** an insufficiently marked pre-place, if this disabled */
        owfnPlace *scapegoat;

        /** conflicting transitions, for use as mustbeincluded */
        owfnTransition **conflicting;
#endif

    public:
        owfnTransition(const std::string&);
        ~owfnTransition();
        bool quasiEnabled;
        bool enabled;
        std::set<unsigned int> messageSet;

        /**
         * double linking in list of enabled transitions. in the sequel,
         * lists are NIL-terminated
         */
        owfnTransition *NextEnabled;
        owfnTransition *PrevEnabled;

        /**
         * double linking in list of quasi enabled transitions in the sequel,
         * lists are NIL-terminated
         */
        owfnTransition *NextQuasiEnabled;
        owfnTransition *PrevQuasiEnabled;

        /** Set above arrays, list, enabled... */
        void initialize(oWFN *);

        /**
         * replace current marking by successor marking, force enabling test
         * where necessary
         */
        void fire(oWFN *);

        /**
         * fire transition backwards to replace original state, force enabling
         * tests where necessary
         */
        void backfire(oWFN *PN);

        /** test if tr is enabled. If necessary, rearrange list; */
        void check_enabled(oWFN *);

        void setLabelForMatching(const std::string& label);
        std::string getLabelForMatching() const;
        bool hasNonTauLabelForMatching() const;

#undef new
        /**
         * Provides user defined operator new. Needed to trace all new
         * operations on this class.
         */
        NEW_OPERATOR(owfnTransition)
#define new NEW_NEW

#ifdef STUBBORN

        /** elements of stubborn set are organized as linked list */
        owfnTransition *NextStubborn;

        /** elements of stubborn set are marked. */
        bool instubborn;

        /** If this is in stubborn set, so must be the ones in array */
        owfnTransition **mustbeincluded;

        /** stubborn sets are calculated through scc detection */
        unsigned int dfs;

        /** Tarjan's lowlink */
        unsigned int min;

        /** used to mark visited transitions */
        unsigned int stamp;

        /** currently processed index in mustbeincluded */
        unsigned int mbiindex;

        /** stack organized as list */
        owfnTransition *nextontarjanstack;

        /** stack organized as list */
        owfnTransition *nextoncallstack;
#endif
};

#endif /*OWFNTRANSITION_H_*/
