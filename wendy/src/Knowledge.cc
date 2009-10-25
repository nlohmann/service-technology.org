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


#include <config.h>
#include <set>
#include "Knowledge.h"
#include "cmdline.h"

using std::map;
using std::vector;
using std::set;

extern gengetopt_args_info args_info;


/***************
 * CONSTRUCTOR *
 ***************/

Knowledge::Knowledge(InnerMarking_ID m)
        : is_sane(1), size(1),
          posSendEvents(NULL), posSendEventsDecoded(NULL) {
    // add this marking to the bubble and the todo queue
    bubble[m].push_back(new InterfaceMarking());
    std::queue<FullMarking> todo;
    todo.push(FullMarking(m));

    // check if initial marking is already bad
    if (InnerMarking::inner_markings[m]->is_bad) {
        is_sane = 0;
        return;
    }

    // calculate the closure
    closure(todo);

    initialize();
}


/*!
 \note no action in this constructor can introduce a duplicate
*/
Knowledge::Knowledge(Knowledge const& parent, const Label_ID& label)
        : is_sane(1), size(0), posSendEvents(NULL), posSendEventsDecoded(NULL) {
    // tau does not make sense here
    assert(not SILENT(label));

    // CASE 1: we receive -- decrement interface markings
    if (RECEIVING(label)) {
        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent.bubble.begin(); pos != parent.bubble.end(); ++pos) {
            for (size_t i = 0; i < pos->second.size(); ++i) {
                // copy an interface marking from the parent and decrement it
                bool result = true;
                InterfaceMarking* interface = new InterfaceMarking(*(pos->second[i]), label, false, result);

                // analyze the result of the copying
                if (result) {
                    // store this interface marking
                    bubble[pos->first].push_back(interface);
                    ++size;
                } else {
                    // decrement failed -- remove this (unreachable) interface marking
                    delete interface;
                }
            }
        }
    }

    // CASE 2: we send -- increment interface markings and calculate closure
    if (SENDING(label)) {
        std::queue<FullMarking> todo;

        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent.bubble.begin(); pos != parent.bubble.end(); ++pos) {
            // check if this label makes the current inner marking possibly transient
            bool receiver = (InnerMarking::receivers[label].find(pos->first) != InnerMarking::receivers[label].end());

            for (size_t i = 0; i < pos->second.size(); ++i) {
                // copy an interface marking from the parent and increment it
                bool result = true;
                InterfaceMarking* interface = new InterfaceMarking(*(pos->second[i]), label, true, result);
                is_sane = is_sane and result;

                // analyze the result of the copying
                if (result or args_info.diagnose_given) {
                    // store this interface marking
                    bubble[pos->first].push_back(interface);
                    ++size;

                    // success -- possibly, this marking became transient
                    if (receiver and result) {
                        todo.push(FullMarking(pos->first, *interface));
                    }
                } else {
                    // increment failed -- message bound violation
                    delete interface;
                    return;
                }
            }
        }

        // calculate the closure
        closure(todo);
    }

    // CASE 3: synchronization
    if (SYNC(label)) {
        std::queue<FullMarking> todo;

        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent.bubble.begin(); pos != parent.bubble.end(); ++pos) {
            // check if this label makes the current inner marking possibly transient
            if ( (InnerMarking::synchs[label].find(pos->first) != InnerMarking::synchs[label].end()) ) {

                for (size_t i = 0; i < pos->second.size(); ++i) {
                    // copy the interface marking (won't change during synchronization)
                    InterfaceMarking* interface = new InterfaceMarking(*(pos->second[i]));

                    InnerMarking* m = InnerMarking::inner_markings[pos->first];
                    for (uint8_t j = 0; j < m->out_degree; ++j) {
                        if (m->labels[j] == label) {
                            // check the marking reached by synchronization
                            if (InnerMarking::inner_markings[m->successors[j]]->is_bad) {
                                is_sane = 0;
                                if (not args_info.diagnose_given) {
                                    delete interface;
                                    return;
                                }
                            }
                            // check if we found a new marking (THIS IS UGLY, but necessary in case the asynchronous interface is empty!)
                            bool found = false;
                            for (std::vector<InterfaceMarking*>::iterator it = bubble[m->successors[j]].begin(); it != bubble[m->successors[j]].end(); ++it) {
                                if (*it == interface) {
                                    found = true;
                                    break;
                                }
                            }
                            if (not found) {
                                // the marking is OK and new, so add it to the bubble
                                todo.push(FullMarking(m->successors[j], *interface));
                                bubble[m->successors[j]].push_back(interface);
                                ++size;
                            }
                        }
                    }
                }
            }
        }

        // calculate the closure
        closure(todo);
    }

    initialize();
}


Knowledge::~Knowledge() {
    // delete the stored interface markings
    for (map<InnerMarking_ID, vector<InterfaceMarking*> >::iterator pos = bubble.begin(); pos != bubble.end(); ++pos) {
        for (size_t i = 0; i < pos->second.size(); ++i) {
            delete pos->second[i];
        }
    }

    if (not args_info.ignoreUnreceivedMessages_flag) {
        delete posSendEvents;
    }
}


/********************
 * MEMBER FUNCTIONS *
 ********************/

/*!
 initialize current knowledge's member attributes appropriately
*/
void Knowledge::initialize() {
    // reduction rule: sequentialize receiving events
    if (args_info.seqReceivingEvents_flag) {
        // calculate those receiving events that are essential to resolve each and every waitstate
        sequentializeReceivingEvents();
    }

    // reduction rule: smart sending event
    // create array of those sending events that are possible in _all_ markings of the current bubble
    if (not args_info.ignoreUnreceivedMessages_flag) {
        // initially, every sending event is possible
        posSendEvents = new PossibleSendEvents(true, 1);

        // traverse each marking of the current bubble
        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = bubble.begin(); pos != bubble.end(); ++pos) {
            // use boolean AND to detect which sending event is possible in each and every marking of the current bubble
            *posSendEvents &= *(InnerMarking::inner_markings[pos->first]->possibleSendEvents);
        }

        // create array to be used when traversing all events
        posSendEventsDecoded = posSendEvents->decode();
    }
}


/*!
 \param todo  a queue of markings to process (will be altered by this function)

 \post queue is empty

 \note no action in this constructor can introduce a duplicate
 \note In case the --diagnose flag is given, the insane marking is added to
       the knowledge before the function is left.
*/
void Knowledge::closure(std::queue<FullMarking>& todo) {
    // process the queue
    while (not todo.empty()) {
        // process successors of the current marking
        InnerMarking* m = InnerMarking::inner_markings[todo.front().inner];

        // check, if each sent message contained on the interface of this marking will ever be consumed
        if (not args_info.ignoreUnreceivedMessages_flag and not m->sentMessagesConsumed(todo.front().interface)) {
            is_sane = 0;
            if (not args_info.diagnose_given) {
                return;
            }
        }

        for (uint8_t i = 0; i < m->out_degree; ++i) {
            // a synchronization is impossible without the environment -- skip
            if (SYNC(m->labels[i])) {
                continue;
            }

            // in any case, create a successor candidate -- it will be valid
            // for transient transitions anyway
            FullMarking candidate(m->successors[i], todo.front().interface);

            // we receive -> the net sends
            if (RECEIVING(m->labels[i])) {
                // message bound violation?
                if (not candidate.interface.inc(m->labels[i])) {
                    is_sane = 0;
                    if (not args_info.diagnose_given) {
                        return;
                    }
                }
            }

            // we send -> the net receives
            if (SENDING(m->labels[i])) {
                if (not candidate.interface.dec(m->labels[i])) {
                    // this marking is not reachable
                    continue;
                }
            }

            // check if successor is a deadlock or livelock
            if (InnerMarking::inner_markings[m->successors[i]]->is_bad) {
                is_sane = 0;
                if (not args_info.diagnose_given) {
                    return;
                }
            }

            // if we found a valid successor candidate, check if it is already stored
            bool candidateFound = false;
            for (size_t j = 0; j < bubble[candidate.inner].size(); ++j) {
                if (*(bubble[candidate.inner][j]) == candidate.interface) {
                    candidateFound = true;
                    break;
                }
            }
            if (not candidateFound) {
                InterfaceMarking* copy = new InterfaceMarking(candidate.interface);
                bubble[candidate.inner].push_back(copy);
                ++size;
                todo.push(candidate);

                // sort bubble using STL algorithms
                if (bubble[candidate.inner].size() > 1) {
                    InterfaceMarking::sort(bubble[candidate.inner]);
                }

                if (not is_sane) {
                    return;
                }
            }
        }
        todo.pop();
    }
}


/*!
 \return whether the knowledge contains a waitstate that can be resolved by
         label l (synchronous or send)
*/
bool Knowledge::resolvableWaitstate(const Label_ID& l) const {
    assert(not RECEIVING(l));

    for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = bubble.begin(); pos != bubble.end(); ++pos) {
        if (InnerMarking::inner_markings[pos->first]->waitstate(l)) {
            return true;
        }
    }
    return false;
}


/*!
 \return whether each waitstate in the knowledge marks and output place, i.e.
         receiving these messages is sufficient and no sending is required
*/
bool Knowledge::receivingHelps() const {
    // traverse the inner markings
    for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = bubble.begin(); pos != bubble.end(); ++pos) {

        // only consider non-final waitstates
        if (InnerMarking::inner_markings[pos->first]->is_waitstate) {

            // traverse the interface markings
            for (size_t i = 0; i < pos->second.size(); ++i) {

                // check if waitstate is resolved by interface marking
                bool resolved = false;
                for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
                    if (pos->second[i]->marked(l) and
                        InnerMarking::receivers[l].find(pos->first) != InnerMarking::receivers[l].end()) {
                        resolved = true;
                        break;
                    }
                }
                if (resolved) {
                    continue;
                }

                // check if waitstate marks an output place
                bool marked = false;
                for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                    if (pos->second[i]->marked(l)) {
                        marked = true;
                    }
                }
                if (not marked) {
                    return false;
                }
            }
        }
    }

    return true;
}


/*!
*  inner marking is really waitstate in the context of the current knowledge
*  \param inner pointer to the current inner marking
*  \param interface interface belonging to the inner marking within the current knowledge
*/
bool Knowledge::isWaitstateInCurrentKnowledge(const InnerMarking_ID & inner, const InterfaceMarking* interface) {

    // check if waitstate is resolved by interface marking
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
		if (interface->marked(l) and
			InnerMarking::receivers[l].find(inner) != InnerMarking::receivers[l].end()) {
			return false;
		}
    }

    // inner waitstate remains a waitstate with the current interface of the knowledge
    return true;
}


/*!
 before traversing through each and every receiving event, we first check
 which receiving events are essentially needed to resolve every waitstate
 of the current bubble
*/
void Knowledge::sequentializeReceivingEvents() {

    // count the number that a receiving event is activated
    uint8_t* occuranceOfReceivingEvent = new uint8_t[Label::receive_events + 1];

    for (uint8_t i = 0; i <= Label::receive_events; ++i) {
        occuranceOfReceivingEvent[i] = 0;
    }

    // remember to consider this state again; actually we only need to take a look at its interface
    map<InterfaceMarking*, bool> visitStateAgain;

    // traverse the inner markings
    for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = bubble.begin(); pos != bubble.end(); ++pos) {

        // only consider non-final waitstates
        if (InnerMarking::inner_markings[pos->first]->is_waitstate) {

            // traverse the interface markings
            for (size_t i = 0; i < pos->second.size(); ++i) {

                if (isWaitstateInCurrentKnowledge(pos->first, pos->second[i])) {
                    // check if waitstate marks an output place
                    Label_ID marked = 0;
                    Label_ID consideredReceivingEvent = 0;

                    for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                        if (pos->second[i]->marked(l)) {
                            // remember that current receiving event is activated in a waitstate
                            ++occuranceOfReceivingEvent[l];
                            // remember this event in case the current waitstate activates only one receiving event
                            consideredReceivingEvent = l;
                            ++marked;
                        }
                    }
                    // check if waitstate activates only a single receiving event
                    if (marked == 1) {
                        // this receiving event has to be considered
                        consideredReceivingEvents[consideredReceivingEvent] = true;
                    }

                    // remember to visit this state again; we only store the
                    // interface here, we don't need more information later on
                    visitStateAgain[pos->second[i]] = true;
                }
            }
        }
    }

    // now traverse through all states that we remembered to consider again
    for (map<InterfaceMarking*, bool>::const_iterator currenState = visitStateAgain.begin();
                                                      currenState != visitStateAgain.end(); ++currenState) {

        // remember the receiving event that will resolve this waitstate
        Label_ID consideredReceivingEvent = 0;

        // we need to remember if the value stored in consideredReceivingEvent is a real one
        bool realEvent = false;

        // check if a receiving event is activated that we have remembered already
        for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {

            // receiving event will resolve this waitstate
            if ((currenState->first)->marked(l)) {

                // it will be considered, so continue with the next waitstate
                if (consideredReceivingEvents.find(l) != consideredReceivingEvents.end()) {
                    consideredReceivingEvent = l;
                    break;
                }
                // if currently considered activated receiving event is not the currently considered one
                if (not realEvent or l > consideredReceivingEvent) {
                    // check if the currently considered activated receiving event is activated by more
                    // waitstates than the currently considered one

                    if (not realEvent or occuranceOfReceivingEvent[l] > occuranceOfReceivingEvent[consideredReceivingEvent]) {
                        // yes, so we will (temporarily) consider the current receiving event to be essential to
                        // resolve the waitstate
                        consideredReceivingEvent = l;

                        // the value stored in consideredReceivingEvent is a real event
                        realEvent = true;
                    }
                }
            } // end if, receiving event is activated
        } // end for, traverse through receiving interface

        // consider the temporal receiving event for real
        if (realEvent) {
            consideredReceivingEvents[consideredReceivingEvent] = true;
        }
    } // end for, traverse through states

    delete[] occuranceOfReceivingEvent;
}


/*!
 if map consideredReceivingEvents has been set, return whether to consider the current
 receiving event or not

 \pre reduction rule seguentialize receiving events is used

 \param label current receiving event
*/
bool Knowledge::considerReceivingEvent(const Label_ID& label) const {
    assert(args_info.seqReceivingEvents_flag);

    return (consideredReceivingEvents.find(label) != consideredReceivingEvents.end());
}


/*!
 sending event is reachable

 \pre reduction rule smart sending event is used

 \param label current sending event

 \note given label must be adjusted such that first send event has label 0
*/
bool Knowledge::considerSendingEvent(const Label_ID& label) const {
    assert(not args_info.ignoreUnreceivedMessages_flag);
    assert(posSendEventsDecoded);

    return (posSendEventsDecoded[label - Label::first_send] == 1);
}
