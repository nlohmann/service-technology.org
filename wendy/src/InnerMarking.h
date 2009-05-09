/*****************************************************************************\
 Wendy -- Calculating Operating Guidelines

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

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
#include "pnapi.h"
#include "types.h"


/*!
 \brief inner marking
 
 An inner marking only stores three bits about its nature (is_final,
 is_deadlock, and is_waitstate -- see determineType()) and its successors.
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

        /// an array of the inner markings
        static InnerMarking **inner_markings;

        /// a mapping from labels to inner markings that might receive this message
        static std::map<Label_ID, std::set<InnerMarking_ID> > receivers;

        /// the open net that created these inner markings
        static pnapi::PetriNet *net;

    private: /* static attributes */

        /// the number of deadlocks
        static unsigned int stats_deadlocks;

        /// the number markings that will reach a deadlock
        static unsigned int stats_inevitable_deadlocks;

        /// the number of final markings
        static unsigned int stats_final_markings;

    public: /* member functions */

        /// constructor
        InnerMarking(const std::vector<Label_ID> &, const std::vector<InnerMarking_ID> &, bool);

        /// destructor
        ~InnerMarking();

    private: /* member functions */

        /// determine the type of this marking
        void determineType();

    public: /* member attributes */

        /// whether this marking is final
        unsigned is_final : 1;

        /// whether this marking needs an external event to proceed
        unsigned is_waitstate : 1;

        /// whether this marking is an internal deadlock
        unsigned is_deadlock : 1;

        /// the number of successor markings (size of successors and labels)
        uint8_t out_degree;

        /// the successor marking ids
        InnerMarking_ID *successors;

        /// the successor label ids
        Label_ID *labels;
};

#endif
