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


#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "Diagnosis.h"
#include "verbose.h"
#include "util.h"

using std::map;
using std::set;
using std::string;
using std::vector;

/*!
  \param[in,out] file  the output stream to write the dot representation to
  \bug synchronous events are ignored when evaluating waitstates
 */
void Diagnosis::output_diagnosedot(std::ostream& file) {
    file << "digraph G {\n"
         << " node [fontname=\"Helvetica\" fontsize=10]\n"
         << " edge [fontname=\"Helvetica\" fontsize=10]\n";

    // draw the nodes
    FOREACH(it, StoredKnowledge::hashTree) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            if (StoredKnowledge::seen.find(it->second[i]) != StoredKnowledge::seen.end()) {
                file << "\"" << it->second[i] << "\" [label=<" << it->second[i] << "<BR/>";

                bool blacklisted = false;

                // collect possible send events for the waitstates
                PossibleSendEvents p = PossibleSendEvents(true, 1);
                for (unsigned int j = 0; j < it->second[i]->sizeDeadlockMarkings; ++j) {
                    p &= *InnerMarking::inner_markings[it->second[i]->inner[j]]->possibleSendEvents;
                }

                set<InnerMarking_ID> hiddenStates;
                for (unsigned int j = 0; j < it->second[i]->sizeAllMarkings; ++j) {
                    bool inner_waitstate = (j < it->second[i]->sizeDeadlockMarkings);
                    bool inner_final = InnerMarking::inner_markings[it->second[i]->inner[j]]->is_final;
                    bool inner_dead = InnerMarking::inner_markings[it->second[i]->inner[j]]->is_bad;
                    bool interface_empty = it->second[i]->interface[j]->unmarked();
                    bool interface_sane = it->second[i]->interface[j]->sane();
                    bool interface_pendingOutput = it->second[i]->interface[j]->pendingOutput();

                    file << "m" << static_cast<size_t>(it->second[i]->inner[j]) << " ";
                    file << *(it->second[i]->interface[j]);

                    string reason;

                    if (inner_dead) {
                        reason += " (dl)";
                        message("node %p is blacklisted: m%u is internal deadlock",
                                it->second[i], static_cast<size_t>(it->second[i]->inner[j]));
                    }
                    if (not interface_sane) {
                        reason += " (mb)";
                        for (Label_ID l = Label::first_receive; l <= Label::last_send; ++l) {
                            if (it->second[i]->interface[j]->get(l) > InterfaceMarking::message_bound) {
                                message("node %p is blacklisted: message bound violation on channel %s",
                                        it->second[i], Label::id2name[l].c_str());
                            }
                        }
                    }

                    if (not reason.empty()) {
                        blacklisted = true;
                        file << " <FONT COLOR=\"RED\">" << reason << "</FONT>";
                    } else {
                        if (inner_final and interface_empty) {
                            file << " <FONT COLOR=\"GREEN\">(f)</FONT>";
                        } else {
                            if (inner_waitstate and not interface_pendingOutput) {
                                // check who can resolve this waitstate
                                vector<Label_ID> resolvers, disallowedResolvers;
                                for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
                                    if (InnerMarking::receivers[l].find(it->second[i]->inner[j]) != InnerMarking::receivers[l].end()) {
                                        resolvers.push_back(l);
                                    }
                                }

                                for (unsigned int l = 0; l < resolvers.size(); ++l) {
                                    char* a = p.decode();
                                    if (a[resolvers[l] - Label::first_send] == 0) {
                                        disallowedResolvers.push_back(resolvers[l]);
                                    }
                                }
                                if (disallowedResolvers.size() == resolvers.size()) {
                                    blacklisted = true;
                                    file << " <FONT COLOR=\"RED\">(uw)</FONT>";
                                    message("node %p is blacklisted: m%u cannot be safely resolved",
                                            it->second[i], static_cast<size_t>(it->second[i]->inner[j]));
                                    hiddenStates.insert(it->second[i]->inner[j]);
                                } else {
                                    file << " <FONT COLOR=\"ORANGE\">(w)</FONT>";
                                }
                            } else {
                                file << " (t)";
                            }
                        }
                    }

                    file << "<BR/>";
                }

                file << ">";

                if (blacklisted) {
                    file << " color=red";
                }
                file << "]\n";

                if (not hiddenStates.empty()) {
                    FOREACH(it1, hiddenStates) {
                        FOREACH(it2, hiddenStates) {
                            if (*it1 != *it2) {
                                message("you need to fix m%u", lastCommonPredecessor(*it1, *it2));
                            }
                        }
                    }
                }

                // draw the edges
                for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
                    if (it->second[i]->successors[l-1] != NULL and
                            (StoredKnowledge::seen.find(it->second[i]->successors[l-1]) != StoredKnowledge::seen.end())) {
                        file << "\"" << it->second[i] << "\" -> \""
                             << it->second[i]->successors[l-1]
                             << "\" [label=\"" << PREFIX(l)
                             << Label::id2name[l] << "\"]\n";
                    }
                }
            }
        }
    }

    file << "}\n";
}

InnerMarking_ID Diagnosis::lastCommonPredecessor(InnerMarking_ID m1, InnerMarking_ID m2) {
    static map<InnerMarking_ID, set<InnerMarking_ID> > reachable;

    // first call: initialize reachability information
    if (reachable.empty()) {
        // for each marking: add yourself and your direct successors
        for (InnerMarking_ID m = 0; m < InnerMarking::stats.markings; ++m) {
            reachable[m].insert(m);
            InnerMarking* current = InnerMarking::inner_markings[m];
            for (uint8_t succ = 0; succ < current->out_degree; ++succ) {
                reachable[m].insert(current->successors[succ]);
            }
        }

        // until fixed point is reached: collect successors' sucessors
        bool done = false;
        do {
            done = true;
            for (InnerMarking_ID m = 0; m < InnerMarking::stats.markings; ++m) {
                InnerMarking* current = InnerMarking::inner_markings[m];
                for (uint8_t succ = 0; succ < current->out_degree; ++succ) {
                    FOREACH(it, reachable[current->successors[succ]]) {
                        if (reachable[m].insert(*it).second) {
                            done = false;
                        }
                    }
                }
            }
        } while (not done);
    }

    InnerMarking_ID last_common = 0;
    for (InnerMarking_ID m = 0; m < InnerMarking::stats.markings; ++m) {
        bool m1_reach = reachable[m].find(m1) != reachable[m].end();
        bool m2_reach = reachable[m].find(m2) != reachable[m].end();
        if (m1_reach and m2_reach and m > last_common) {
            last_common = m;
        }
    }

    return last_common;
}
