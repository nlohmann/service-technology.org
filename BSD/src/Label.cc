/*****************************************************************************\
 Locretia -- generating logs...

 Copyright (c) 2012 Simon Heiden

 Locretia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Locretia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Locretia.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <set>
#include "Label.h"
#include "openNet.h"
#include "cmdline.h"
#include "verbose.h"
#include "util.h"

using std::set;
using std::string;
using pnapi::Node;
using pnapi::Place;
using pnapi::Transition;


/******************
 * STATIC MEMBERS *
 ******************/

Label_ID Label::first_receive = 2;  //sic! (0 is the id for tau, 1 is the id for bound_broken-transition)
Label_ID Label::last_receive = 0;
Label_ID Label::first_send = 0;
Label_ID Label::last_send = 0;
Label_ID Label::first_sync = 0;
Label_ID Label::last_sync = 0;
Label_ID Label::send_events = 0;
Label_ID Label::receive_events = 0;
Label_ID Label::sync_events = 0;
Label_ID Label::events = 1; // (sic!)
std::string Label::visible_transitions = "";

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

	// set label for internal transitions (tau)
	id2name[0] = "\\tau";

    // ASYNCHRONOUS RECEIVE EVENTS (?-step for us)
    const set<pnapi::Label*> outputLabels(openNet::net->getInterface().getOutputLabels());
    for (set<pnapi::Label*>::const_iterator l = outputLabels.begin(); l != outputLabels.end(); ++l, ++receive_events) {
        id2name[++events] = (**l).getName();

        const set<Transition*> preset((**l).getTransitions());
        FOREACH(t, preset) {
        	//status("label: %s, transition: %s", (**l).getName().c_str(), (**t).getName().c_str());
            visible_transitions += (visible_transitions.empty() ? "" : " OR ") + (**t).getName();
            name2id[(**t).getName()] = events;
        }
    }

    last_receive = events;


    // ASYNCHRONOUS RECEIVE SEND (!-step for us)
    first_send = events + 1;

    const set<pnapi::Label*> inputLabels(openNet::net->getInterface().getInputLabels());

    for (set<pnapi::Label*>::const_iterator l = inputLabels.begin(); l != inputLabels.end(); ++l, ++send_events) {
        id2name[++events] = (**l).getName();

        const set<Transition*> postset((**l).getTransitions());
        FOREACH(t, postset) {
        	//status("label: %s, transition: %s", (**l).getName().c_str(), (**t).getName().c_str());
            visible_transitions += (visible_transitions.empty() ? "" : " OR ") + (**t).getName();
            name2id[(**t).getName()] = events;
        }
    }

    last_send = events;


    // SYNCHRONOUS EVENTS (#-step for us)
    first_sync = events + 1;

    // collect the labels
    std::map<string, Label_ID> sync_labels;
    const set<pnapi::Label*> sync_label_names(openNet::net->getInterface().getSynchronousLabels());

    for (set<pnapi::Label*>::const_iterator l = sync_label_names.begin(); l != sync_label_names.end(); ++l, ++sync_events) {
    	if ((**l).getName().compare("bound_broken") != 0) {
    		sync_labels[(**l).getName()] = ++events;
    		id2name[events] = (**l).getName();

    		const set<Transition*> trans((**l).getTransitions());
    		FOREACH(t, trans) {
    			visible_transitions += (visible_transitions.empty() ? "" : " OR ") + (**t).getName();
    			name2id[(**t).getName()] = events;
    		}
    	} else {
    		sync_labels[(**l).getName()] = 1;
    		id2name[1] = (**l).getName();

    		const set<Transition*> trans((**l).getTransitions());
    		FOREACH(t, trans) {
    			visible_transitions += (visible_transitions.empty() ? "" : " OR ") + (**t).getName();
    			name2id[(**t).getName()] = 1;
    		}
    	}
    }

    last_sync = events;
}

