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
 * \file    owfnPlace.h
 *
 * \brief   functions for Petri net places
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef OWFNPLACE_H_
#define OWFNPLACE_H_

#include "mynew.h"
#include "PetriNetNode.h"
#include "formula.h"
#include <vector>
#include <string>


class oWFN;
class formula;

enum placeType {INPUT, OUTPUT, INTERNAL}; //< type of place


class owfnPlace : public Node {
    private:
        /// pointer to underlying petri net (needed for hash value)
        oWFN * net;

        /// the port of this place
        std::string port;

    public:
        /// constructor
        owfnPlace(char *, placeType, oWFN *);

        /// constructor
        owfnPlace(const std::string&, placeType, oWFN *);

        /// copy constructor
        owfnPlace(owfnPlace *, oWFN*);

        /// basic deconstructor
        ~owfnPlace();

        /// type of place (input, output, internal)
        placeType type;

        /// returns the label fitting for the communication graph
        std::string getLabelForCommGraph() const;

        /// returns the label fitting for matching
        std::string getLabelForMatching() const;

        /// returns the underlying owfn
        oWFN* getUnderlyingOWFN() const;

        // returns cost of this place if its an interface place
        unsigned int getCost();

        /// initial marking of the place
        unsigned int initial_marking;

        /// hashfactor of the place
        unsigned int hash_factor;

        /// increment marking of place
        void operator +=(unsigned int);

        /// decrement marking of place
        void operator -=(unsigned int);

        /// test enabledness with respect to place
        bool operator >=(unsigned int);

        /// define a factor for hash value calculation
        void set_hash(unsigned int);

        /// we remove isolated places
        unsigned int references;

        /// maximum capacity
        unsigned int capacity;

        /// nr of bits required for storing its marking (= log capacity)
        int nrbits;

        /// first bit representing this place in bit vector
        int startbit;

        /// max occurrences of the node in its owfn
        int max_occurrence;

        /// returns the type of the node
        placeType getType() const;

        /// number of propositions in final condition that mention this place
        unsigned int cardprop;

        /// array of propositions in final condition that mention this place
        formula ** proposition;

        int propsize;

        /// void set_marking(unsigned int);   /// set initial 1marking of place;
        /// hash(m) = sum(p in P) p.hash_factor*CurrentMarking[p]


        /// *** Definitions for stubborn set calculations
#ifdef STUBBORN
        std::vector<owfnTransition*> PreTransitions; /// these transitions must be included in
        /// stubborn set if this place is scapegoat
        void initialize(); /// initialize PreTransitions
#endif

        /// set the port of the place
        void setPort(const std::string port);

        /// get the port of the place
        std::string getPort();

        /// Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(owfnPlace)
#define new NEW_NEW
};

#endif /*OWFNPLACE_H_*/
