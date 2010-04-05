/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef COVER_H_
#define COVER_H_

/************
 * Includes *
 ************/

#include <pnapi/pnapi.h>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <config.h>
#include "StoredKnowledge.h"
#include "types.h"

/*!
 * \brief cover class
 *
 * The cover class holds all variables and methods to calculate
 * the global contraint of an operating guideline to cover
 * certain places or transitions.
 */
class Cover {
    public: /* data structures */
        /// struct to collect all nodes to be covered
        struct CoverData {
            /// internal places
            std::vector<unsigned int> inP;
            /// interface places
            std::vector<unsigned int> comP;
            /// internal transitions
            std::vector<unsigned int> inT;
            /// interface transitions
            std::vector<unsigned int> comT;
        };

    private: /* static attributes */

        /// array of internal places to cover
        static std::string* internalPlaces;

        /// number of internal places to cover
        static unsigned int internalPlaceCount;

        /// array of internal transitions to cover
        static std::string* internalTransitions;

        /// number of internal transitions to cover
        static unsigned int internalTransitionCount;

        /// array of interface places to cover
        static std::string* interfacePlaces;

        /// number of interface places to cover
        static unsigned int interfacePlaceCount;

        /// array of interface transitions to cover
        static std::string* interfaceTransitions;

        /// number of interface transitions to cover
        static unsigned int interfaceTransitionCount;

        /// array of internal place pointer for a faster occur check
        static pnapi::Place** internalPlaceCache;

        /// mapping  as array of interface place number to appropriate label
        static Label_ID* interfacePlaceLabels;

        /// mapping of inner marking to CoverData
        static std::map<InnerMarking_ID, CoverData> inner2CD;

        /// mapping of StoredKnowledge to CoverData
        static std::map<StoredKnowledge*, CoverData> knowledge2CD;

        /// vectors of knowledges, that cover nodes
        static std::vector<StoredKnowledge*>* coveringInternalPlaces;
        static std::vector<StoredKnowledge*>* coveringInterfacePlaces;
        static std::vector<StoredKnowledge*>* coveringInternalTransitions;
        static std::vector<StoredKnowledge*>* coveringInterfaceTransitions;

    public: /* static methods */

        /// fill the set of places and transitions to cover
        static void initialize(const std::vector<std::string>&, const std::vector<std::string>&);
        static void initialize(const std::vector<pnapi::Place*>&, const std::vector<pnapi::Label*>&,
                               const std::vector<pnapi::Transition*>&, const std::vector<pnapi::Transition*>&);

        /// cover all nodes of the given net
        static void coverAll();

        /// checks for a given inner marking, which nodes are covered
        static void checkInnerMarking(InnerMarking_ID, std::map<const pnapi::Place*, unsigned int>&, const std::set<std::string>&);

        /// deletes cache
        static void clear();

        /// checks for a given knowledge, which nodes are covered
        static void checkKnowledge(StoredKnowledge*, const std::map<InnerMarking_ID, std::vector<InterfaceMarking*> > &);

        /// removes knowledges when deleted
        static void removeKnowledge(StoredKnowledge*);

        /// calculates the global contraint
        static void calculate(const std::set<StoredKnowledge*>&);

        /// writes the contraint to an output stream
        static void write(std::ostream&);

    public: /* static attributs */

        /// label cache; filled in Label::initialize()
        static std::map<std::string, Label_ID> labelCache;

        /// whether the constraint is satisfiable
        static bool satisfiable;

        /// nodes to cover
        static unsigned int nodeCount;

        /// synchronous labels
        static std::vector<std::string> synchronousLabels;
};

#endif /*COVER_H_*/
