#ifndef _INNERMARKING_H
#define _INNERMARKING_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include "config.h"
#include "Label.h"
#include "pnapi.h"

using std::string;

typedef uint32_t InnerMarking_ID;

/*!
 \brief inner marking
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

        /// the number of inner markings (used as length for inner_markings)
        static InnerMarking_ID inner_marking_count;

        /// a mapping from labels to inner markings that might receive this message
        static std::map<Label_ID, std::set<InnerMarking_ID> > receivers;

        /// a mapping from labels to inner markings that can synchronize
        static std::map<Label_ID, std::set<InnerMarking_ID> > sync;

        /// the number of edges in the reachability graph (fired transitions)
        static unsigned int edges_count;
        
        /// the open net that created these inner markings
        static pnapi::PetriNet *net;
        
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

        /// determine the type of this marking
        void determineType();
            
    public: /* member attributes */
    
        /// whether this marking has a silent tau successor
        unsigned is_tau : 1;
        
        /// whether this marking is final
        unsigned is_final : 1;
        
        /// whether this marking can asynchronously receive
        unsigned is_receive : 1;

        /// whether this marking can asynchronously send
        unsigned is_send : 1;
        
        /// whether this marking can synchronously communicate
        unsigned is_sync : 1;
        
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
