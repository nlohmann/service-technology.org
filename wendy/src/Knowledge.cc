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


#include <cassert>
#include "config.h"
#include "Knowledge.h"
#include "Label.h"

using std::map;
using std::vector;


/***************
 * CONSTRUCTOR *
 ***************/

Knowledge::Knowledge(InnerMarking_ID m) : is_sane(1), size(1) {
    // add this marking to the bubble and the todo queue
    bubble[m].push_back(new InterfaceMarking());
    std::queue<FullMarking> todo;
    todo.push(FullMarking(m));

    // calculate the closure
    closure(todo);
}


/*!
 \note no action in this constructor can introduce a duplicate
*/
Knowledge::Knowledge(const Knowledge* const parent, Label_ID label) : is_sane(1), size(0) {
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
                            if (InnerMarking::inner_markings[m->successors[j]]->is_deadlock) {
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

 \todo do I need to copy the queue item to "current"?

 \todo sort bubble

 \todo comment me -- it was not entirely clear that markings added to todo
       are not automatically added to the bubble
*/
inline void Knowledge::closure(std::queue<FullMarking> &todo) {
    // to collect markings that were/are already considered
    set<FullMarking> considered;

    while(not todo.empty()) {
        FullMarking current(todo.front());
        todo.pop();

        // if this marking was already taken out of the todo queue, skip it this time
        if (not considered.insert(current).second) {
            continue;
        }

        // process successors of the current marking
        InnerMarking *m = InnerMarking::inner_markings[current.inner];
        for (uint8_t i = 0; i < m->out_degree; ++i) {

            // a synchronization is impossible without the environment -- skip
            if (SYNC(m->labels[i])) {
                continue;
            }

            // in any case, create a successor candidate -- it will be valid for transient transitions anyway
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

            // check if successor is a deadlock
            if (InnerMarking::inner_markings[m->successors[i]]->is_deadlock) {
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
