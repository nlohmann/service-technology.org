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


#include "config.h"
#include <cassert>

#include <set>
#include <string>
#include <iostream>
#include "Knowledge.h"
#include "Label.h"
#include "cmdline.h"

using std::map;
using std::vector;
using std::set;
using std::string;

extern gengetopt_args_info args_info;

/***************
 * CONSTRUCTOR *
 ***************/

Knowledge::Knowledge(InnerMarking_ID m) : is_sane(1), size(1) {

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
}


/*!
 \note no action in this constructor can introduce a duplicate
*/
Knowledge::Knowledge(const Knowledge* const parent, const Label_ID &label) : is_sane(1), size(0) {
    // tau does not make sense here
    assert(not SILENT(label));

    // CASE 1: we receive -- decrement interface markings
    if (RECEIVING(label)) {
        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent->bubble.begin(); pos != parent->bubble.end(); ++pos) {
            for (size_t i = 0; i < pos->second.size(); ++i) {
                // copy an interface marking from the parent and decrement it
                bool result = true;
                InterfaceMarking *interface = new InterfaceMarking(*(pos->second[i]), label, false, result);

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

        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent->bubble.begin(); pos != parent->bubble.end(); ++pos) {
            // check if this label makes the current inner marking possibly transient
            bool receiver = (InnerMarking::receivers[label].find(pos->first) != InnerMarking::receivers[label].end());

            for (size_t i = 0; i < pos->second.size(); ++i) {
                // copy an interface marking from the parent and increment it
                bool result = true;
                InterfaceMarking *interface = new InterfaceMarking(*(pos->second[i]), label, true, result);

                // analyze the result of the copying
                if (result) {
                    // store this interface marking
                    bubble[pos->first].push_back(interface);
                    ++size;

                    // success -- possibly, this marking became transient
                    if (receiver) {
                        todo.push(FullMarking(pos->first, *interface));
                    }
                } else {
                    // increment failed -- message bound violation
                    delete interface;
                    is_sane = 0;
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

        for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = parent->bubble.begin(); pos != parent->bubble.end(); ++pos) {
            // check if this label makes the current inner marking possibly transient
            if ( (InnerMarking::synchs[label].find(pos->first) != InnerMarking::synchs[label].end()) ) {

                for (size_t i = 0; i < pos->second.size(); ++i) {
                    // copy the interface marking (won't change during synchronization)
                    InterfaceMarking *interface = new InterfaceMarking(*(pos->second[i]));

                    InnerMarking *m = InnerMarking::inner_markings[pos->first];
                    for (uint8_t j = 0; j < m->out_degree; ++j) {
                        if (m->labels[j] == label) {
                            // check the marking reached by synchronization
                            if (InnerMarking::inner_markings[m->successors[j]]->is_bad) {
                                delete interface;
                                is_sane = 0;
                                return;
                            }

                            // the marking is OK, so add it to the bubble
                            /// \bug this might introduce duplicates
                            todo.push(FullMarking(m->successors[j], *interface));
                            bubble[m->successors[j]].push_back(interface);
                            ++size;
                        }
                    }
                }
            }
        }

        // calculate the closure
        closure(todo);
    }
}


Knowledge::~Knowledge() {
    // delete the stored interface markings
    for (map<InnerMarking_ID, vector<InterfaceMarking*> >::iterator pos = bubble.begin(); pos != bubble.end(); ++pos) {
        for (size_t i = 0; i < pos->second.size(); ++i) {
            delete pos->second[i];
        }
    }
}


/********************
 * MEMBER FUNCTIONS *
 ********************/


/*!
 \param todo  a queue of markings to process (will be altered by this function)

 \post queue is empty

 \note no action in this constructor can introduce a duplicate

 \todo sort bubble
*/
inline void Knowledge::closure(std::queue<FullMarking> &todo) {
    // to collect markings that were/are already considered
    set<FullMarking> considered;

    // process the queue
    while(not todo.empty()) {
        FullMarking current(todo.front());
        todo.pop();

        // if this marking was already taken out of the todo queue, skip it this time
        if (not considered.insert(current).second) {
            continue;
        }

        // process successors of the current marking
        InnerMarking *m = InnerMarking::inner_markings[current.inner];

        // check, if each sent message contained on the interface of this marking will ever be consumed
        if (args_info.smartSendingEvent_flag and not m->sentMessagesConsumed(current.interface)) {
            is_sane = 0;
            return;
        }

        for (uint8_t i = 0; i < m->out_degree; ++i) {

            // a synchronization is impossible without the environment -- skip
            if (SYNC(m->labels[i])) {
                continue;
            }

            // in any case, create a successor candidate -- it will be valid
            // for transient transitions anyway
            FullMarking candidate(m->successors[i], current.interface);

            // we receive -> the net sends
            if (RECEIVING(m->labels[i])) {
                // message bound violation?
                if (not candidate.interface.inc(m->labels[i])) {
                    is_sane = 0;
                    return;
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
                return;
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
                InterfaceMarking *copy = new InterfaceMarking(candidate.interface);
                bubble[candidate.inner].push_back(copy);
                ++size;
                todo.push(candidate);
            }
        }
    }

    /* sort bubble! */
}


/*!
 \return whether the knowledge contains a waitstate that can be resolved by
         label l (synchronous or send)
*/
bool Knowledge::resolvableWaitstate(const Label_ID &l) const {
    assert (not RECEIVING(l));

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
 \param  consideredReceivingEvents remember only those receiving event which are essential to resolve each and every waitstate
 \brief  before traversing through each and every receiving event, we first check
         which receiving events are essentially needed to resolve every waitstate of the current bubble
*/
void Knowledge::sequentializeReceivingEvents(map<Label_ID, bool> & consideredReceivingEvents) const {

    // count the number that a receiving event is activated
    map<Label_ID, unsigned int> occuranceOfReceivingEvent;

    // remember to consider this state again; actually we only need to take a look at its interface
    map<vector<InterfaceMarking*>, bool> visitStateAgain;

    // traverse the inner markings
    for (map<InnerMarking_ID, vector<InterfaceMarking*> >::const_iterator pos = bubble.begin(); pos != bubble.end(); ++pos) {

        // only consider non-final waitstates
        if (InnerMarking::inner_markings[pos->first]->is_waitstate) {

            // traverse the interface markings
            for (size_t i = 0; i < pos->second.size(); ++i) {

                // check if waitstate marks an output place
                Label_ID marked = 0;
                Label_ID consideredReceivingEvent = 0;

                for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                    if (pos->second[i]->marked(l)) {
                        // remember that current receiving event is activated in a waitstate
                        occuranceOfReceivingEvent[l] += 1;
                        // remember this event in case the current waitstate activates only one receiving event
                        consideredReceivingEvent = l;
                        marked++;
                    }
                }
                // check if waitstate activates only a single receiving event
                if (marked == 1) {

                    // this receiving event has to be considered
                    consideredReceivingEvents[consideredReceivingEvent] = true;

                }
                // remember to visit this state again; we only store the interface here, we don't need more information later on
                visitStateAgain[pos->second] = true;
            }
        }
    }

    // now traverse through all states that we remembered to consider again
    for (map<vector<InterfaceMarking*>, bool>::const_iterator currenState = visitStateAgain.begin();
                                                              currenState != visitStateAgain.end(); currenState++) {

        // remember the receiving event that will resolve this waitstate
        Label_ID consideredReceivingEvent = 0;

        // we need to remember if the value stored in consideredReceivingEvent is a real one
        bool realEvent = false;

        // current state is resolved, so we consider the next one
        bool considerNewState = false;

        // traverse the interface markings
        for (size_t i = 0; i < currenState->first.size(); ++i) {

            // check if a receiving event is activated that we have remembered already
            for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
                // receiving event will resolve this waitstate
                if (currenState->first[i]->marked(l)) {

                    // it will be considered, so continue with the next waitstate
                    if (consideredReceivingEvents[l]) {

                        considerNewState = true;
                        break;
                    }
                    // if currently considered activated receiving event is not the currently considered one
                    if (!realEvent || l > consideredReceivingEvent) {
                        // check if the currently considered activated receiving event is activated by more
                        // waitstates than the currently considered one
                        if (!realEvent || occuranceOfReceivingEvent[l] > occuranceOfReceivingEvent[consideredReceivingEvent]) {
                            // yes, so we will (temporarily) consider the current receiving event to be essential to
                            // resolve the waitstate
                            consideredReceivingEvent = l;

                            // the value stored in consideredReceivingEvent is a real event
                            realEvent = true;
                        }
                    }
                } // end if, receiving event is activated
            } // end for, traverse through receiving interface
            if (considerNewState) {
                break;
            }
        } // end for, traverse the interface markings

        // consider the temporal receiving event for real
        if (realEvent) {
            consideredReceivingEvents[consideredReceivingEvent] = true;
        }
    } // end for, traverse through states
}
