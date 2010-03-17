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


#include <set>
#include "Label.h"
#include "InnerMarking.h"
#include "Cover.h"
#include "cmdline.h"
#include "verbose.h"
#include "util.h"

using std::set;
using std::string;
using pnapi::Node;
using pnapi::Place;
using pnapi::Transition;

extern gengetopt_args_info args_info;


/******************
 * STATIC MEMBERS *
 ******************/

Label_ID Label::first_receive = 1;  //sic! (0 is the id for tau)
Label_ID Label::last_receive = 0;
Label_ID Label::first_send = 0;
Label_ID Label::last_send = 0;
Label_ID Label::first_sync = 0;
Label_ID Label::last_sync = 0;
Label_ID Label::send_events = 0;
Label_ID Label::receive_events = 0;
Label_ID Label::sync_events = 0;
Label_ID Label::events = 0;

std::map<Label_ID, string> Label::id2name;
std::map<string, Label_ID> Label::name2id;


/******************
 * STATIC METHODS *
 ******************/

/*!
 This function traverses the Petri net nodes and extracts the labels of the
 communication events (asynchronous send and receive events as well as
 synchronous events). Each event then gets a unique identifier.
 */
void Label::initialize() {
    // ASYNCHRONOUS RECEIVE EVENTS (?-step for us)
    const set<pnapi::Label*> outputLabels(InnerMarking::net->getInterface().getOutputLabels());
    for (set<pnapi::Label*>::const_iterator l = outputLabels.begin(); l != outputLabels.end(); ++l, ++receive_events) {
        id2name[++events] = (**l).getName();
        if (args_info.cover_given) {
            Cover::labelCache[(**l).getName()] = events;
        }

        const set<Transition*> preset((**l).getTransitions());
        FOREACH(t, preset) {
            name2id[(**t).getName()] = events;
        }
    }

    last_receive = events;


    // ASYNCHRONOUS RECEIVE SEND (!-step for us)
    first_send = events+1;

    const set<pnapi::Label*> inputLabels(InnerMarking::net->getInterface().getInputLabels());

    for (set<pnapi::Label*>::const_iterator l = inputLabels.begin(); l != inputLabels.end(); ++l, ++send_events) {
        id2name[++events] = (**l).getName();
        if (args_info.cover_given) {
            Cover::labelCache[(**l).getName()] = events;
        }

        const set<Transition*> postset((**l).getTransitions());
        FOREACH(t, postset) {
            name2id[(**t).getName()] = events;
        }
    }

    last_send = events;


    // SYNCHRONOUS EVENTS (#-step for us)
    first_sync = events+1;

    // collect the labels
    std::map<string, Label_ID> sync_labels;
    const set<pnapi::Label*> sync_label_names(InnerMarking::net->getInterface().getSynchronousLabels());
    FOREACH(l, sync_label_names) {
        sync_labels[(**l).getName()] = ++events;
        id2name[events] = (**l).getName();
        
        const set<Transition*> trans((**l).getTransitions());
        FOREACH(t, trans) {
          name2id[(**t).getName()] = events;
        }
    }

    last_sync = events;
    sync_events = last_sync - send_events - receive_events;
}


void Label::output_results(Results& r) {
    r.add("statistics.events", events);
    r.add("statistics.events_send", send_events);
    r.add("statistics.events_receive", receive_events);
    r.add("statistics.events_synchronous", sync_events);
}
