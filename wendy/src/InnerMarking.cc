#include "InnerMarking.h"
#include "Label.h"
#include "cmdline.h"

/// the command line parameters
extern gengetopt_args_info args_info;


/******************
 * STATIC MEMBERS *
 ******************/

std::map<InnerMarking_ID, InnerMarking*> InnerMarking::markingMap;
pnapi::PetriNet *InnerMarking::net = new pnapi::PetriNet();
InnerMarking **InnerMarking::inner_markings = NULL;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::receivers;

unsigned int InnerMarking::stats_deadlocks = 0;
unsigned int InnerMarking::stats_inevitable_deadlocks = 0;
unsigned int InnerMarking::stats_final_markings = 0;


/******************
 * STATIC METHODS *
 ******************/

/*!
 Copy the markings from the mapping markingMap to C-style arrays.
 Additionally, a mapping is filled to quickly determine whether a marking can
 become transient if a message with a given label was sent to the net.
 
 \todo replace the mapping receivers by a two-dimensional C-style array
 */
void InnerMarking::initialize() {
    unsigned int inner_marking_count = markingMap.size();
    inner_markings = new InnerMarking*[inner_marking_count];

    // copy data from mapping (used during parsing) to a C array
    for (InnerMarking_ID i = 0; i < inner_marking_count; ++i) {
        inner_markings[i] = markingMap[i];
        
        // register markings that may become activated by sending a message to them
        for (unsigned int j = 0; j < inner_markings[i]->out_degree; ++j) {
            if (SENDING(inner_markings[i]->labels[j])) {
                receivers[inner_markings[i]->labels[j]].insert(i);
            }
        }
    }

    markingMap.clear();

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: found %d final markings, %d deadlocks, and %d inevitable deadlocks\n",
            PACKAGE, stats_final_markings, stats_deadlocks, stats_inevitable_deadlocks);
        fprintf(stderr, "%s: stored %d inner markings",
            PACKAGE, inner_marking_count);
    }    
}


/***************
 * CONSTRUCTOR *
 ***************/
 
InnerMarking::InnerMarking(const std::vector<Label_ID> &_labels,
                           const std::vector<InnerMarking_ID> &_successors,
                           bool _is_final) :
                           is_final(_is_final), is_waitstate(0), is_deadlock(0)
{
    assert(_labels.size() == _successors.size());
    assert (_successors.size() < UCHAR_MAX);

    out_degree = _successors.size();

    labels = new Label_ID[out_degree];
    std::copy(_labels.begin(), _labels.end(), labels);

    successors = new InnerMarking_ID[out_degree];
    std::copy(_successors.begin(), _successors.end(), successors);

    // knowing all successors, we can determine the type of the marking
    determineType();
}

InnerMarking::~InnerMarking() {
    delete[] labels;
    delete[] successors;
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
 The type is determined by checking the labels of the leaving transitions as
 well as the fact whether this marking is a final marking. For the further
 processing, it is sufficient to distinguish four types:
 
 - the marking is a deadlock (is_deadlock) -- then a knowledge containing
   this inner marking can immediately be considered insane
 - the marking is a final marking (is_final) -- this is needed to distinguish
   deadlocks from final markings
 - the marking is a waitstate (is_waitstate) -- a waitstate is a marking of
   the inner net that can only be left by firing a transition that is
   connected to an input place
   
 This function also implements the detection of inevitable deadlocks. A
 marking is an inevitable deadlock, if it is a deadlock or all is successor
 markings are inevitable deadlocks. The detection exploits the way LoLA
 returns the reachability graph: when a marking is returned, we know that
 the only successors we have not considered yet (i.e. those markings where
 successors[i] == NULL) are also predessessors of this marking and cannot be
 a reason for this marking to be a deadlock. Hence, this marking is an
 inevitable deadlock if we cannot find a non-deadlocking successor.

 \note except is_final, all types are initialized with 0, so it is sufficent
       to only set values to 1
 */
inline void InnerMarking::determineType() {
    bool is_transient = false;
    
    // deadlock: no successor markings and not final
    if (out_degree == 0 && is_final != 1) {
        ++stats_deadlocks;
        is_deadlock = 1;
    }
    
    if (is_final) {
        ++stats_final_markings;
    }
    
    // variable to detect whether this marking has only deadlocking successors
    bool deadlock_inevitable = true;
    for (unsigned int i = 0; i < out_degree; ++i) {
        // if a single successor is not a deadlock, everything is OK
        if (!args_info.noDeadlockDetection_given &&
            markingMap[successors[i]] != NULL &&
            deadlock_inevitable &&
            !markingMap[successors[i]]->is_deadlock) {
            deadlock_inevitable = false;
        }
        
        // a tau or sending (sic!) transition makes this marking transient
        if (SILENT(labels[i]) || RECEIVING(labels[i])) {
            is_transient = true;
        }
    }

    // deadlock cannot be avoided any more -- treat this marking as deadlock
    if (!args_info.noDeadlockDetection_given && !is_final && deadlock_inevitable) {
        is_deadlock = 1;
        ++stats_inevitable_deadlocks;
    }
    
    // draw some conclusions
    if (!is_final && !is_transient) {
        is_waitstate = 1;
    }
}
