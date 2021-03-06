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
 * \file    owfnTransition.h
 *
 * \brief   functions for Petri net transitions
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef OWFNTRANSITION_H_
#define OWFNTRANSITION_H_

#include "mynew.h"
#include "PetriNetNode.h"
#include <set>
#include <vector>

class oWFN;
class owfnPlace;


/**
 * A place that is adjecent to a owfnTransition. An owfnTransition can have
 * multiple AdjacentPlaces. Each contains a pointer to its corresponding
 * owfnPlace and stores the multiplicity of the arc between that place and the
 * owning owfnTransition.
 */
class AdjacentPlace {
    private:
        /// Points to corresponding owfnPlace.
        owfnPlace* place_;

        /// The multiplicity of the arc between the corresponding owfnPlace and
        /// the owfnTransition that owns this PrePlace.
        unsigned int multiplicity_;
    public:
        /// Creates a AdjacentPlace from the given place and multiplicity.
        AdjacentPlace(owfnPlace* place, unsigned int multiplicity);

        /// Returns the owfnPlace that belongs to this AdjacentPlace.
        owfnPlace* getOwfnPlace() const;

        /// Returns the multiplicity of the arc between the corresponding
        /// owfnPlace and the owning owfnTransition.
        unsigned int getMultiplicity() const;
};


class owfnTransition : public Node {
    private:
        std::string labelForMatching;

        bool isEnabled_;

        bool isQuasiEnabled_;

        /// number of internal pre-places marked with appropriate tokens
        unsigned int quasiEnabledNr;

        /// number of input pre-places marked with appropriate tokens
        unsigned int enabledNr;

        /// Type of the containers holding all adjacent places of this
        /// transition.
        typedef std::vector<AdjacentPlace> AdjacentPlaces_t;

        /// Places with their multiplicities to be checked for enabledness.
        AdjacentPlaces_t PrePlaces;

        /// Places that are incremented by transition; together with the
        /// corresponding amounts of increment.
        AdjacentPlaces_t IncrPlaces;

        /// Places that are decremented by transition; together with the
        /// corresponding amounts of decrement.
        AdjacentPlaces_t DecrPlaces;

        /// Type for ImproveEnabling and ImproveDisabling.
        typedef std::vector<owfnTransition*> ImproveDisEnabling_t;

        /// list of transitions where enabledness must be checked again after
        /// firing this transition
        ImproveDisEnabling_t ImproveEnabling;

        /// list of transitions where disabledness must be checked again after
        /// firing this transition
        ImproveDisEnabling_t ImproveDisabling;

        /// change of hash value by firing t;
        int hash_change;
        void set_hashchange();

        /// dfsnum of last state where some fired transition disables this one
        unsigned int lastdisabled;

        /// dfsnum of last state where this tr. was fired
        unsigned int lastfired;

#ifdef STUBBORN
        ///
        bool prePlaceIsScapegoatForDisabledness(oWFN* petrinet, AdjacentPlace prePlace) const;

        /// an insufficiently marked pre-place, if this disabled
        owfnPlace *scapegoat;

        /// conflicting transitions, for use as mustbeincluded
        std::vector<owfnTransition*> conflicting;
#endif

    public:
        /// constructor
        owfnTransition(const std::string& name);

        /// constructor
        owfnTransition(const std::string& name, const unsigned int c); 

        /// destructor
        ~owfnTransition();

        /// Adds a owfnPlace to the pre-set of this owfnTransition.
        void addPrePlace(owfnPlace* owfnPlace, unsigned int multiplicity);

        /// Returns whether this owfnTransition is enabled or not.
        bool isEnabled() const;

        /// Set enabledness of this owfnTransition.
        void setEnabled(bool isEnabled);

        /// Returns wether this owfnTransition in quasi enabled.
        bool isQuasiEnabled() const;

        /// Sets quasi enabledness of this owfnTransition.
        void setQuasiEnabled(bool isQuasiEnabled);

        /// Returns whether this is normal (connected to max. 1 input/output place) or not
        bool isNormal() const;

        std::set<unsigned int> messageSet;

        /// Set above arrays, list, enabled...
        void initialize();

        /// replace current marking by successor marking, force enabling test
        /// where necessary
        void fire(oWFN *);

        /// fire transition backwards to replace original state, force enabling
        /// tests where necessary
        void backfire(oWFN *PN);

        /// test if tr is enabled. If necessary, rearrange list;
        void check_enabled(oWFN *);

        /// sets a label for the matching
        void setLabelForMatching(const std::string& label);
        
        /// returns a label for the matching
        std::string getLabelForMatching() const;
        
        /// returns true if there is no tau label for matching
        bool hasNonTauLabelForMatching() const;

        /// cost we have to pay for firing this transition
        /// TODO implement this private and with getter/setter
        unsigned int cost;

#undef new
        /// Provides user defined operator new. Needed to trace all new
        /// operations on this class.
        NEW_OPERATOR(owfnTransition)
#define new NEW_NEW

#ifdef STUBBORN

        /// elements of stubborn set are organized as linked list
        owfnTransition *NextStubborn;

        /// elements of stubborn set are marked. 
        bool instubborn;

        /// If this is in stubborn set, so must be the ones in array
        std::vector<owfnTransition*> mustbeincluded;

        /// stubborn sets are calculated through scc detection 
        unsigned int dfs;

        /// Tarjan's lowlink
        unsigned int min;

        /// used to mark visited transitions
        unsigned int stamp;

        /// currently processed index in mustbeincluded
        unsigned int mbiindex;

        /// stack organized as list
        owfnTransition *nextontarjanstack;

        /// stack organized as list
        owfnTransition *nextoncallstack;
    
        /// true iff transition is connected to input place of other port
        bool isConnectedToOtherPort;
    
        /// true iff transition is connected to input place of my port
        bool isConnectedToMyPort;
#endif
};

#endif /*OWFNTRANSITION_H_*/
