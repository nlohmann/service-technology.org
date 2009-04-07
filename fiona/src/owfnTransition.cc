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
 * \file    owfnTransition.cc
 *
 * \brief   functions for Petri net transitions
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include "mynew.h"
#include "owfnTransition.h"
#include "owfn.h"
#include "main.h"
#include "debug.h"
#include "state.h"
#include "options.h"
#include "AnnotatedGraph.h"
#include "GraphFormula.h"

#include <cstdlib>

//! \brief constructor
AdjacentPlace::AdjacentPlace(owfnPlace* place, unsigned int multiplicity) :
    place_(place), multiplicity_(multiplicity) {
}

//! \brief returns the owfn place of AdjacentPlace
//! \return owfnPlace
owfnPlace* AdjacentPlace::getOwfnPlace() const {
    return place_;
}


//! \brief returns the Multiplicity of AdjacentPlace
//! \return multiplicity
unsigned int AdjacentPlace::getMultiplicity() const {
    return multiplicity_;
}


//! \brief constructor
owfnTransition::owfnTransition(const std::string& name) :
    Node(name), labelForMatching(GraphFormulaLiteral::TAU), isEnabled_(false),
            isQuasiEnabled_(false), quasiEnabledNr(0), enabledNr(0),
            isConnectedToOtherPort(false), isConnectedToMyPort(false) {
}


//! \brief deconstructor
owfnTransition::~owfnTransition() {
}


//! \brief Adds a owfnPlace to the pre-set of this owfnTransition.
//! \param owfnPlace owfnPlace to be added to this owfnTransition's
//!        pre-set.
//! \param multiplicity Multiplicity of the arc from the given owfnPlace
//!        to this owfnTransition.
void owfnTransition::addPrePlace(owfnPlace* owfnPlace, unsigned int multiplicity) {
    PrePlaces.push_back(AdjacentPlace(owfnPlace, multiplicity));
}


//! \brief returns whether this transition is enabled
//! \return true if so
bool owfnTransition::isEnabled() const {
    return isEnabled_;
}

//! \brief Set enabledness of this owfnTransition.
//! \param isEnabled New value of enabledness.
void owfnTransition::setEnabled(bool isEnabled) {
    isEnabled_ = isEnabled;
}


//! \brief returns whether this transition is quasi enabled
//! \return true if so
bool owfnTransition::isQuasiEnabled() const {
    return isQuasiEnabled_;
}


//! \brief Set quasi enabledness of this owfnTransition.
//! \param isEnabled New value of quasi enabledness.
void owfnTransition::setQuasiEnabled(bool isQuasiEnabled) {
    isQuasiEnabled_ = isQuasiEnabled;
}


//! \brief Returns true if this transition is normal, false otherwise.
bool owfnTransition::isNormal() const {

    //trace(TRACE_5, "owfnTransition::isNormal() : start\n");

    int countInterfacePlaces = 0;

    for (AdjacentPlaces_t::size_type i = 0; i != IncrPlaces.size() && countInterfacePlaces < 2; i++) {
        AdjacentPlace incrPlace = IncrPlaces[i];
        owfnPlace* incrOwfnPlace = incrPlace.getOwfnPlace();

        if ( incrOwfnPlace->getType() != INTERNAL ) countInterfacePlaces++;
    }

    for (AdjacentPlaces_t::size_type i = 0; i != DecrPlaces.size() && countInterfacePlaces < 2; i++) {
        AdjacentPlace decrPlace = DecrPlaces[i];
        owfnPlace* decrOwfnPlace = decrPlace.getOwfnPlace();

        if ( decrOwfnPlace->getType() != INTERNAL ) countInterfacePlaces++;
    }

    //trace(TRACE_5, "owfnTransition::isNormal() : start\n");
    return (countInterfacePlaces < 2);
}


#ifdef STUBBORN
//! \brief DESCRIPTON
//! \return DESCRIPTON
bool owfnTransition::prePlaceIsScapegoatForDisabledness(oWFN* petrinet, AdjacentPlace prePlace) const {
    //cout << "owfnTransition::prePlaceIsScapegoatForDisabledness multiplicity = " + intToString(prePlace.getMultiplicity()) + "\n";
    //cout << "owfnTransition::prePlaceIsScapegoatForDisabledness petrinet->CurrentMarking = " + intToString(petrinet->CurrentMarking[petrinet->getPlaceIndex(prePlace.getOwfnPlace())]) + "\n";
    return (petrinet->CurrentMarking[petrinet->getPlaceIndex(prePlace.getOwfnPlace())] <
            prePlace.getMultiplicity());
}
#endif

//! \brief sets the hashchange
void owfnTransition::set_hashchange() {
    hash_change = 0;
    for (AdjacentPlaces_t::size_type i = 0; i != IncrPlaces.size(); ++i) {
        hash_change += IncrPlaces[i].getMultiplicity()
                *IncrPlaces[i].getOwfnPlace()->hash_factor;
    }
    for (AdjacentPlaces_t::size_type i = 0; i != DecrPlaces.size(); ++i) {
        hash_change -= DecrPlaces[i].getMultiplicity()
                *DecrPlaces[i].getOwfnPlace()->hash_factor;
    }
    hash_change %= HASHSIZE;
}


//! \brief sets initializes various values of the transition
void owfnTransition::initialize() {
    //trace(TRACE_5, "owfnTransition::initialize() start\n");

    // Create list of Pre-Places for enabling test
    for (unsigned int i = 0; i < getArrivingArcsCount(); ++i) {
        PrePlaces.push_back(AdjacentPlace(getArrivingArc(i)->pl, getArrivingArc(i)->Multiplicity));
    }

    // Create list of places where transition increments marking
    //cout << "    found " << getLeavingArcsCount() << " leaving arcs for transition " << getName() << "(" << this << ")" << endl;
    for (unsigned int i = 0; i < getLeavingArcsCount(); i++) {
        //Is oWFNPlace a loop place?
        unsigned int j = 0;
        for (j = 0; j < getArrivingArcsCount(); j++) {
            if ((getLeavingArc(i)->Destination) == (getArrivingArc(j)->Source))
                break;
        }

        if (j < getArrivingArcsCount()) {
            //yes, loop place
            if (getLeavingArc(i)->Multiplicity > getArrivingArc(j)->Multiplicity) {
                // indeed, transition increments place
                IncrPlaces.push_back(AdjacentPlace(getLeavingArc(i)->pl, getLeavingArc(i)->Multiplicity -getArrivingArc(j)->Multiplicity));
            }
        } else {
            // no loop place
            IncrPlaces.push_back(AdjacentPlace(getLeavingArc(i)->pl, getLeavingArc(i)->Multiplicity));
        }
    }

    // Create list of places where transition decrements marking
    //cout << "    found " << getArrivingArcsCount() << " arriving arcs for transition " << getName() << "(" << this << ")" << endl;
    for (unsigned int i = 0; i < getArrivingArcsCount(); i++) {
        //Is oWFNPlace a loop place?
        unsigned int j = 0;
        for (j = 0; j < getLeavingArcsCount(); j++) {
            if ((getArrivingArc(i)->Source) == (getLeavingArc(j)->Destination))
                break;
        }

        if (j < getLeavingArcsCount()) {
            //yes, loop place
            if (getArrivingArc(i)->Multiplicity > getLeavingArc(j)->Multiplicity) {
                // indeed, transition decrements place
                DecrPlaces.push_back(AdjacentPlace(getArrivingArc(i)->pl, getArrivingArc(i)->Multiplicity -getLeavingArc(j)->Multiplicity));
            }
        } else {
            // no loop place
            DecrPlaces.push_back(AdjacentPlace(getArrivingArc(i)->pl, getArrivingArc(i)->Multiplicity));
        }
    }

    // Create list of transitions where enabledness can change
    // if this transition fires.
    //cout << "    found " << IncrPlaces.size() << " IncrPlaces for transition " << getName() << "(" << this << ")" << endl;
    for (AdjacentPlaces_t::size_type i = 0; i != IncrPlaces.size(); ++i) {
        owfnPlace* incrOwfnPlace = IncrPlaces[i].getOwfnPlace();
        for (unsigned int j = 0; j < incrOwfnPlace->getLeavingArcsCount(); j++) {
            owfnTransition* transition = incrOwfnPlace->getLeavingArc(j)->tr;
            //cout << "    checking transition " << transition->getName() << "(" << transition << ")" << " for transition " << getName() << "(" << this << ")" << endl;
            if (!transition->isEnabled()) {
                // not yet in list
                ImproveEnabling.push_back( transition );
                //cout << "    pushing transition " << transition->getName() << "(" << transition << ")" << " for transition " << getName() << "(" << this << ")" << endl;
            }
        }
    }

    // Create list of transitions where enabledness can change
    // if this transition fires.
    //cout << "    found " << DecrPlaces.size() << " DecrPlaces for transition " << getName() << "(" << this << ")" << endl;
    for (AdjacentPlaces_t::size_type i = 0; i != DecrPlaces.size(); ++i) {
        owfnPlace* decrOwfnPlace = DecrPlaces[i].getOwfnPlace();
        for (unsigned int j = 0; j < decrOwfnPlace->getLeavingArcsCount(); j++) {
            owfnTransition* transition = decrOwfnPlace->getLeavingArc(j)->tr;
            //cout << "    checking transition " << transition->getName() << "(" << transition << ")" << " for transition " << getName() << "(" << this << ")" << endl;
            if (!transition->isEnabled()) {
                // not yet in list
                ImproveDisabling.push_back( transition );
                //cout << "    pushing transition " << transition->getName() << "(" << transition << ")" << " for transition " << getName() << "(" << this << ")" << endl;
            }
        }
    }

    set_hashchange();
#ifdef STUBBORN
    stamp = 0;
    NextStubborn = 0;
    instubborn = false;

    // create list of conflicting transitions
    for (unsigned int i = 0; i != PrePlaces.size(); ++i) {
        AdjacentPlace prePlace = PrePlaces[i];
        owfnPlace* preOwfnPlace = prePlace.getOwfnPlace();
        for (unsigned int j = 0; j < preOwfnPlace->getLeavingArcsCount(); j++) {
            if (preOwfnPlace->getLeavingArc(j)->tr != this) {
                if (!preOwfnPlace->getLeavingArc(j)->tr ->isEnabled()) {
                    conflicting.push_back(preOwfnPlace->getLeavingArc(j)->tr);
                }
            }
        }
    }

    mustbeincluded = conflicting;
#endif

    //trace(TRACE_5, "owfnTransition::initialize() end\n");
}


//! \brief fires the transition in a given owfn and changes its state and marking accordingly
//! \param petrinet the owfn to be fired in
void owfnTransition::fire(oWFN * petrinet) {

    //trace(TRACE_5, "owfnTransition::fire(oWFN * petrinet) : start\n");

    //cerr << "transition has " << intToString(IncrPlaces.size()) << " IncrPlaces\n" << endl;
    for (AdjacentPlaces_t::size_type i = 0; i != IncrPlaces.size(); ++i) {
        AdjacentPlace incrPlace = IncrPlaces[i];
        owfnPlace* incrOwfnPlace = incrPlace.getOwfnPlace();

        petrinet->CurrentMarking[petrinet->getPlaceIndex(incrOwfnPlace)] += incrPlace.getMultiplicity();
#ifdef CHECKCAPACITY
        if(petrinet->CurrentMarking[petrinet->getPlaceIndex(incrOwfnPlace)] > incrOwfnPlace->capacity) {
            TRACE(TRACE_2, "capacity " + intToString(incrOwfnPlace->capacity) + " of place " + incrOwfnPlace->name + " exceeded!\n") ;
            throw CapacityException(incrOwfnPlace->name);
            //_exit(4);
        }
#endif
    }

    //cerr << "transition has " << intToString(DecrPlaces.size()) << " DecrPlaces\n" << endl;
    for (AdjacentPlaces_t::size_type i = 0; i != DecrPlaces.size(); ++i) {
        AdjacentPlace decrPlace = DecrPlaces[i];
        owfnPlace* decrOwfnPlace = decrPlace.getOwfnPlace();

        if (petrinet->CurrentMarking[petrinet->getPlaceIndex(decrOwfnPlace)]
                < decrPlace.getMultiplicity()) {
            petrinet->printCurrentMarking();
            cerr << "marking of place "<< decrOwfnPlace->name<< " is: "
                    << petrinet->CurrentMarking[petrinet->getPlaceIndex(decrOwfnPlace)]
                    << " but transition "<< this->name<< " consumes: "
                    << decrPlace.getMultiplicity() << endl;
            cerr << "number of states calculated so far: "<< State::state_count
                    << endl;
            setExitCode(EC_CAPACITY_EXCEEDED);
        } else {
            petrinet->CurrentMarking[petrinet->getPlaceIndex(decrOwfnPlace)] -= decrPlace.getMultiplicity();
        }
    }

    petrinet->placeHashValue += hash_change;
    petrinet->placeHashValue %= HASHSIZE;
    for (ImproveDisEnabling_t::size_type i = 0; i != ImproveEnabling.size(); ++i) {
        owfnTransition* transition = ImproveEnabling[i];
        if (!(transition->isEnabled()) || !(transition->isQuasiEnabled())) {
            transition->check_enabled(petrinet);
        }
    }
    for (ImproveDisEnabling_t::size_type i = 0; i != ImproveDisabling.size(); ++i) {
        owfnTransition* transition = ImproveDisabling[i];
        if (transition->isEnabled() || transition->isQuasiEnabled()) {
            transition->check_enabled(petrinet);
        }
    }

    // update value of formula after having fired t
    //    for(p = DecrPlaces; *p != NULL; p++) {
    //		unsigned int j;
    //        for(j=0; j < (* p) -> cardprop; j++) {
    //            if ((* p)->proposition != NULL) {
    //	            (* p)->proposition[j] -> update(petrinet->CurrentMarking[(* p)->index]);
    //			}
    //        }
    //    }
    //
    //    for(p = IncrPlaces; * p != NULL; p++) {
    //		unsigned int j;
    //        for(j=0; j < (* p) -> cardprop; j++) {
    //            if ((* p)->proposition != NULL) {
    //	            (* p)->proposition[j] -> update(petrinet->CurrentMarking[(* p)->index]);
    //            }
    //        }
    //    }

    //trace(TRACE_5, "owfnTransition::fire(oWFN * petrinet) : end\n");
}


//void owfnTransition::backfire(oWFN * PN)
//{
//  PN->placeHashValue -= hash_change;
//  PN->placeHashValue %= HASHSIZE;
//
//}


//! \brief fires the transition in a given owfn backward and changes its state and marking accordingly
//! \param PN the owfn to be fired in
void owfnTransition::backfire(oWFN * PN) {

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

    for (ImproveDisEnabling_t::size_type i = 0; i != ImproveEnabling.size(); ++i) {
        owfnTransition* transition = ImproveEnabling[i];
        if (transition->isEnabled()) {
            transition->check_enabled(PN);
        }
    }

    for (ImproveDisEnabling_t::size_type i = 0; i != ImproveDisabling.size(); ++i) {
        owfnTransition* transition = ImproveDisabling[i];
        if (!transition->isEnabled()) {
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


//! \brief check whether this transition is (quasi) enabled at the current marking
//!        quasi-enabled means, that this transition activates a sending event (this transition is a receiving transition)
//! \param PN owfn this transition is part of
//! \fn void owfnTransition::check_enabled(oWFN * PN)
void owfnTransition::check_enabled(oWFN * PN) {

    //trace(TRACE_5, "owfnTransition::check_enabled(oWFN * PN) : start\n");

    messageSet.clear();

    enabledNr = quasiEnabledNr = 0; // suppose no pre-place has appropriate marking at all
#ifdef STUBBORN
    mustbeincluded = conflicting;
#endif

    //cout << "BEFORE" << endl;
    //cout << "PrePlaces.size() = " + intToString(PrePlaces.size()) + "\n";
    for (AdjacentPlaces_t::size_type iPrePlace = 0;
         iPrePlace != PrePlaces.size(); ++iPrePlace) {

        AdjacentPlace prePlace = PrePlaces[iPrePlace];
        owfnPlace* preOwfnPlace = prePlace.getOwfnPlace();

        if (prePlaceIsScapegoatForDisabledness(PN, prePlace)) {
#ifdef STUBBORN
            scapegoat = preOwfnPlace;
            mustbeincluded = preOwfnPlace->PreTransitions;
#endif
            if (preOwfnPlace->type == INPUT) {
                messageSet.insert(PN->getPlaceIndex(preOwfnPlace));
                quasiEnabledNr++; // remember that we have found an input pre-place with no appropriate marking
            }
        } else {
            enabledNr++; // remember that we have found a pre-place with appropriate marking
        }
    }

    //cout << "current marking: " << PN->getCurrentMarkingAsString() << endl;
    //cout << "transition " << name << "(" << this << ")" << " is quasiEnabled: " << isQuasiEnabled() << " and enabled: " << isEnabled() << endl;
    //cout << "enabledNr = " + intToString(enabledNr) + " getArrivingArcsCount = " + intToString(getArrivingArcsCount()) + "\n";
    // [LUHME XV] Check auf Enabledness und Quasi-Enabledness überarbeiten: evtl. kann sich ein Check einsparen lassen.
    // [LUHME XV] Wenn enabled, dann auch quasi-enabled.
    // [LUHME XV] Evtl. zwei Versionen, weil OG keine Quasi-Enabledness braucht.

    if (enabledNr == getArrivingArcsCount()) { // there are as many pre-places appropriatly marked as there are incoming arcs
        if (!isEnabled()) { // transition was not enabled before
            // include transition into list of enabled transitions
            PN->enabledTransitions.add(this);
            setEnabled(true);
        }
        if (isQuasiEnabled()) { // transition was quasi enabled before
            setQuasiEnabled(false);
            PN->quasiEnabledTransitions.remove(this); // delete transition from list of quasi enabled transtions
        }
    } else if ((enabledNr + quasiEnabledNr) == getArrivingArcsCount()) {
        // transition is quasi enabled
        if (!isQuasiEnabled()) { // transition was not quasi enabled before
            // include transition into list of quasi enabled transitions
            PN->quasiEnabledTransitions.add(this);
            setQuasiEnabled(true);
        }
        if (isEnabled()) { // transition was enabled before
            setEnabled(false);
            PN->enabledTransitions.remove(this); // delete transition from list of enabled transtions

        }
    } else { // transition is not enabled at all
        if (isEnabled()) { // transition was enabled before
            setEnabled(false);
            PN->enabledTransitions.remove(this); // delete transition from list of enabled transtions

        }
        if (isQuasiEnabled()) { // transition was quasi enabled before
            setQuasiEnabled(false);
            PN->quasiEnabledTransitions.remove(this); // delete transition from list of quasi enabled transtions
        }
    }

    //cout << "AFTER" << endl;
    //cout << "current marking: " << PN->getCurrentMarkingAsString() << endl;
    //cout << "transition " << name << "(" << this << ")" << " is quasiEnabled: " << isQuasiEnabled() << " and enabled: " << isEnabled() << endl;

    //trace(TRACE_5, "owfnTransition::check_enabled(oWFN * PN) : end\n");
}


//! \brief sets the label for matching
//! \param label new label
void owfnTransition::setLabelForMatching(const std::string& label) {
    //trace(TRACE_5, "    set LabelForMatching for transition " + getName() + " to " + label + "\n");
    labelForMatching = label;
}


//! \brief returns the label for matching
//! \return label for matching
std::string owfnTransition::getLabelForMatching() const {
    return labelForMatching;
}


//! \brief returns true if there is no tau label for matching
//! \return true if there is no tau label for matching, else false
bool owfnTransition::hasNonTauLabelForMatching() const {
    return getLabelForMatching() != GraphFormulaLiteral::TAU;
}
