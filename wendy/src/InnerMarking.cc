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


#include <climits>
#include <cassert>
#include "config.h"
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
 
 \todo replace the mapping receivers by a two-dimensional C-style array or do
       this check in the constructor
 */
void InnerMarking::initialize() {
    unsigned int inner_marking_count = markingMap.size();
    inner_markings = new InnerMarking*[inner_marking_count];

    // copy data from mapping (used during parsing) to a C array
    for (InnerMarking_ID i = 0; i < inner_marking_count; ++i) {
        inner_markings[i] = markingMap[i];

        // register markings that may become activated by sending a message to them
        for (uint8_t j = 0; j < inner_markings[i]->out_degree; ++j) {
            if (SENDING(inner_markings[i]->labels[j])) {
                receivers[inner_markings[i]->labels[j]].insert(i);
            }
        }
    }

    markingMap.clear();

    if (stats_final_markings == 0) {
        fprintf(stderr, "%s: warning: no final marking found\n", PACKAGE);
    }

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
 
 \bug the whole deadlock detection has to be overworked (see bug7) -- it
      seems as if it is possible to loose/gain strategies there
 */
inline void InnerMarking::determineType() {
    bool is_transient = false;

    // deadlock: no successor markings and not final
    if (out_degree == 0 and not is_final) {
        ++stats_deadlocks;
        is_deadlock = 1;
    }

    if (is_final) {
        ++stats_final_markings;
    }

    // variable to detect whether this marking has only deadlocking successors
    bool deadlock_inevitable = true;
    for (uint8_t i = 0; i < out_degree; ++i) {
        // if a single successor is not a deadlock, everything is OK
        if (not args_info.noDeadlockDetection_given and
            markingMap[successors[i]] != NULL and
            deadlock_inevitable and
            not markingMap[successors[i]]->is_deadlock) {
            deadlock_inevitable = false;
        }

        // set deadlock_inevitable to false in case there is a communicating
        // successor
        if (SENDING(labels[i]) or RECEIVING(labels[i])) {
            deadlock_inevitable = false;
        }

        // a tau or sending (sic!) transition makes this marking transient
        if (SILENT(labels[i]) or RECEIVING(labels[i])) {
            is_transient = true;
        }
    }

    // deadlock cannot be avoided any more -- treat this marking as deadlock
    if (not args_info.noDeadlockDetection_given and
        not is_final and
        not is_deadlock and
        deadlock_inevitable) {
        is_deadlock = 1;
        ++stats_inevitable_deadlocks;
    }

    // draw some last conclusions
    if (not (is_transient or is_deadlock)) {
        is_waitstate = 1;
    }
}
