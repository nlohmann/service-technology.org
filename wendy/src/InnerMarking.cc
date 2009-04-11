#include "InnerMarking.h"
#include "cmdline.h"
#include "config.h"

/// the command line parameters
extern gengetopt_args_info args_info;


/******************
 * STATIC MEMBERS *
 ******************/

std::map<InnerMarking_ID, InnerMarking*> InnerMarking::markingMap;
pnapi::PetriNet *InnerMarking::net = new pnapi::PetriNet();
InnerMarking **InnerMarking::inner_markings = NULL;
InnerMarking_ID InnerMarking::inner_marking_count = 0;
unsigned int InnerMarking::edges_count = 0;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::receivers;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::sync;

unsigned int InnerMarking::stats_deadlocks = 0;
unsigned int InnerMarking::stats_inevitable_deadlocks = 0;
unsigned int InnerMarking::stats_final_markings = 0;


/******************
 * STATIC METHODS *
 ******************/

void InnerMarking::initialize() {
    inner_marking_count = markingMap.size();
    inner_markings = new InnerMarking*[inner_marking_count];

    // copy data from mapping (used during parsing) to a C array
    for (InnerMarking_ID i = 0; i < inner_marking_count; ++i) {
        inner_markings[i] = markingMap[i];
        
        // register markings that may become activated by sending a message to them
        for (unsigned int j = 0; j < inner_markings[i]->out_degree; ++j) {
            if (SENDING(inner_markings[i]->labels[j])) {
                receivers[i].insert(inner_markings[i]->labels[j]);
            }
            
            if (SYNC(inner_markings[i]->labels[j])) {
                sync[i].insert(inner_markings[i]->labels[j]);
            }
        }
    }

    markingMap.clear();

    if (args_info.verbose_given) {
        fprintf(stderr, "%s: found %d final markings, %d deadlocks, and %d inevitable deadlocks\n",
            PACKAGE, stats_final_markings, stats_deadlocks, stats_inevitable_deadlocks);
        fprintf(stderr, "%s: stored %d inner markings and %d edges",
            PACKAGE, inner_marking_count, edges_count);
    }    
}


/***************
 * CONSTRUCTOR *
 ***************/
 
InnerMarking::InnerMarking(const std::vector<Label_ID> &_labels,
                           const std::vector<InnerMarking_ID> &_successors,
                           bool _is_final) :
                           is_final(_is_final), is_waitstate(0), is_deadlock(0) {

    assert(_labels.size() == _successors.size());
    assert (_successors.size() < UCHAR_MAX);

    out_degree = _successors.size();
    edges_count += out_degree;

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
 well as the fact whether this marking is a final marking.

 \note except is_final, all types are initialized with 0, so it is sufficent
       to only set values to 1
 \todo check the switched off assertion
 */
inline void InnerMarking::determineType() {
    bool is_tau = false;
    bool is_receive = false;
    bool is_send = false;
    bool is_sync = false;
    
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

//        assert(markingMap[successors[i]]); 
// I had to switch off this assertion and add "markingMap[successors[i]] != NULL" to the if below.
        if (!args_info.noDeadlockDetection_given &&
            markingMap[successors[i]] != NULL &&
            deadlock_inevitable &&
            !markingMap[successors[i]]->is_deadlock) {
            deadlock_inevitable = false;
        }
        
        if (labels[i] == 0) {
            is_tau = true;
        } else {
            if (RECEIVING(labels[i])) {
                is_send = true;
            } else {
                if (SENDING(labels[i])) {
                    is_receive = true;
                } else {
                    is_sync = true;
                }
            }
        }
    }

    // deadlock cannot be avoided any more -- treat this marking as deadlock
    if (!args_info.noDeadlockDetection_given && !is_final && deadlock_inevitable) {
        is_deadlock = 1;
        ++stats_inevitable_deadlocks;
    }
    
    // draw some conclusions
    if (!is_final && !is_tau && !is_send) {
        is_waitstate = 1;
    }
}
