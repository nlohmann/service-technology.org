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


#ifndef _INNERMARKING_H
#define _INNERMARKING_H

#include <vector>
#include <map>
#include <set>
#include "types.h"
#include "config.h"
#include "PossibleSendEvents.h"
#include "InterfaceMarking.h"
#include <pnapi/pnapi.h>


/*!
 \brief inner marking

 An inner marking only stores three bits about its nature (is_final,
 is_bad, and is_waitstate -- see determineType()) and its successors.
 The successors are stored in two C-style arrays: one consisting of the
 identifiers of the successors and one consisting of the interface labels of
 the respective transitions.

 The reachability graph of the inner of the net is created by LoLA and then
 parsed. The parser creates objects of this class and stores pointers to them
 in the mapping markingMap together with an identifier (the depth-first
 search number given by LoLA). After all markings are parsed, the function
 initialize() is called to copy the markingMap into a C-style array.
*/
class InnerMarking {
    public: /* static functions */

        /// copy markings from temporary storage to array
        static void initialize();

    public: /* static attributes */

        /// a temporary storage used during parsing of the reachability graph
        static std::map<InnerMarking_ID, InnerMarking*> markingMap;

        /// a temporary storage used to detect internal livelocks
        static std::map<InnerMarking_ID, bool> finalMarkingReachableMap;

        /// an array of the inner markings
        static InnerMarking **inner_markings;

        /// a mapping from labels to inner markings that might receive this message
        static std::map<Label_ID, std::set<InnerMarking_ID> > receivers;

        /// a mapping from labels to inner markings that can synchronize with this label
        static std::map<Label_ID, std::set<InnerMarking_ID> > synchs;

        /// the open net that created these inner markings
        static pnapi::PetriNet *net;

    private: /* static attributes */

        /// the number of total markings
        static unsigned int stats_markings;

        /// the number of internal bad states
        static unsigned int stats_bad_states;

        /// the number of markings that will reach a deadlock
        static unsigned int stats_inevitable_deadlocks;

        /// the number of final markings
        static unsigned int stats_final_markings;

    public: /* member functions */

        /// constructor
        InnerMarking(const InnerMarking_ID&, const std::vector<Label_ID>&, const std::vector<InnerMarking_ID>&, bool);

        /// destructor
        ~InnerMarking();

        /// whether this marking is a waitstate resolveable by the given label
        bool waitstate(const Label_ID&) const;

        /// checks if all input messages will be consumed later on
        bool sentMessagesConsumed(const InterfaceMarking&) const;

        /// determines which receiving transitions are potentially reachable from this marking
        void calcReachableSendingEvents();

    private: /* member functions */

        /// determine the type of this marking
        void determineType(const InnerMarking_ID&);

    public: /* member attributes */

        /// whether this marking is final
        unsigned is_final : 1;

        /// whether this marking needs an external event to proceed
        unsigned is_waitstate : 1;

        /// whether this marking is an internal deadlock or livelock
        unsigned is_bad : 1;

        /// the number of successor markings (size of successors and labels)
        uint8_t out_degree;

        /// the successor marking ids
        InnerMarking_ID *successors;

        /// the successor label ids
        Label_ID *labels;

        /// receiving transitions that are potentially reachable
        PossibleSendEvents *possibleSendEvents;
};

#endif
