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

#pragma once

#include <pnapi/pnapi.h>
#include <vector>
#include <map>
#include <set>
#include "Results.h"
#include "types.h"


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
        friend class Diagnosis;

    public: /* static functions */
        /// copy markings from temporary storage to array
        static void initialize();

        static void changeView(const int c);

        static void deleteCounterPlaces();

        /// print the knowledges as OG
        static void create_log(std::ostream& file, std::string & filename,
        										   const int trace_count ,
				  	  	  	  	  	  	  	  	   const int trace_min_length,
				  	  	  	  	  	  	  	  	   const int trace_max_length);

        /// destroy all objects of this class
        static void finalize();

        /// print results
        static void output_results(Results&);

        /// traverse reachability graph
        static void traverse(const InnerMarking_ID& markingID);

        /// add a (more or less) random interface to the net
        static void addInterface(const int count);

        /// add the final condition to the (special) tpn
        static void addFinalCondition();

    public: /* static attributes */
        /// a temporary storage used during parsing of the reachability graph
        static std::map<InnerMarking_ID, InnerMarking*> markingMap;

        /// a temporary storage used to detect internal livelocks
        static std::map<InnerMarking_ID, bool> finalMarkingReachableMap;

        /// an array of the inner markings
        static InnerMarking** inner_markings;

        /// a mapping from labels to inner markings that might receive this message
        static std::map<Label_ID, std::set<InnerMarking_ID> > receivers;

        /// a mapping from labels to inner markings that might send this message
        static std::map<Label_ID, std::set<InnerMarking_ID> > senders;

        /// a mapping from labels to inner markings that can synchronize with this label
        static std::map<Label_ID, std::set<InnerMarking_ID> > synchs;

        /// the open net that created these inner markings
        static pnapi::PetriNet* net;

        /// remember if the reachability graph of the inner is acyclic
        static bool is_acyclic;

        static std::set<pnapi::Place *> counterPlaces;

    private: /* static attributes */
        /// struct combining the statistics on the class InnerMarking
        static struct _stats {
            /// the number of total markings
            unsigned int markings;

            /// the number of internal bad states
            unsigned int bad_states;

            /// the number of markings that will reach a deadlock
            unsigned int inevitable_deadlocks;

            /// the number of final markings
            unsigned int final_markings;
        } stats;

    private: /* static functions */
            /// creates the header for the output file
            static void fileHeader(std::ostream&, std::string& filename);

            /// creates the footer for the output file
            static void fileFooter(std::ostream&);

            /// creates a trace and writes it to the output file
            static void create_trace(std::ostream& file, const int trace_number, const int trace_maxlength);

    public: /* member functions */
        /// constructor
        InnerMarking(const InnerMarking_ID&, const std::vector<Label_ID>&,
                     const std::vector<InnerMarking_ID>&, const bool&);

        /// destructor
        ~InnerMarking();

    private: /* member functions */
        /// determine the type of this marking
        inline void determineType(const InnerMarking_ID&);

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
        InnerMarking_ID* successors;

        /// the successor label ids
        Label_ID* labels;
};

#define TRACE_CLASSIFIER "Trace Name"
#define TRACE_KEY_NAME "concept:name"
#define TRACE_KEY_LENGTH "length"
#define EVENT_CLASSIFIER "Event Name"
#define EVENT_KEY_LABEL "concept:name"
#define EVENT_KEY_NUMBER "number"

#define PORT_NAME "port"
