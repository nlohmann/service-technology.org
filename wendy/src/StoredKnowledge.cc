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
#include <iostream>
#include <string>
#include <vector>
#include "StoredKnowledge.h"
#include "Cover.h"
#include "verbose.h"
#include "cmdline.h"

using std::map;
using std::set;
using std::string;
using std::vector;

extern gengetopt_args_info args_info;
extern string invocation;


/******************
 * STATIC MEMBERS *
 ******************/

std::map<hash_t, std::vector<StoredKnowledge*> > StoredKnowledge::hashTree;
StoredKnowledge* StoredKnowledge::root = NULL;
StoredKnowledge* StoredKnowledge::empty = reinterpret_cast<StoredKnowledge*>(1);
std::set<StoredKnowledge*> StoredKnowledge::deletedNodes;
std::set<StoredKnowledge*> StoredKnowledge::seen;
std::vector<StoredKnowledge *> StoredKnowledge::tarjanStack;
unsigned int StoredKnowledge::bookmarkTSCC = 0;
bool StoredKnowledge::emptyNodeReachable = false;
StoredKnowledge::_stats StoredKnowledge::stats;
std::map<const StoredKnowledge*, std::pair<unsigned int, unsigned int> > StoredKnowledge::tarjanMapping;


#define MINIMUM(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAXIMUM(X, Y) ((X) > (Y) ? (X) : (Y))


/********************
 * STATIC FUNCTIONS *
 ********************/

/*!
 \note maxBucketSize must be initialized to 1
 */
StoredKnowledge::_stats::_stats()
        : hashCollisions(0), storedEdges(0), builtInsaneNodes(0),
          maxBucketSize(1), storedKnowledges(0), maxSCCSize(0),
          numberOfNonTrivialSCCs(0), numberOfTrivialSCCs(0) {}


/*!
 \param[in] K   a knowledge bubble (explicitly stored)
 \param[in] SK  a knowledge bubble (compactly stored)
 \param[in] l   a label
 */
void StoredKnowledge::processSuccessor(const Knowledge* K,
                                       StoredKnowledge* const SK,
                                       const Label_ID& l) {
    // create a new knowledge for the given label
    Knowledge* K_new = new Knowledge(K, l);

    // do not store and process the empty node explicitly
    if (K_new->size == 0) {
        if (K_new->is_sane) {
            SK->addSuccessor(l, empty);
        }
        delete K_new;
        return;
    }

    // only process knowledges within the message bounds
    if (K_new->is_sane or args_info.diagnose_given) {
        // create a compact version of the knowledge bubble
        StoredKnowledge* SK_new = new StoredKnowledge(K_new);

        // add it to the knowledge tree
        StoredKnowledge* SK_store = SK_new->store();

        // store an edge from the parent to this node
        SK->addSuccessor(l, SK_store);

        // evaluate the storage result
        if (SK_store == SK_new) {
            if (K_new->is_sane) {
                // the node was new and sane, so check its successors
                processNode(K_new, SK_store);
            }
        } else {
            // we did not find new knowledge
            delete SK_new;
        }

        // the successors of the new knowledge have been calculated, so adjust lowlink value of SK
        SK->adjustLowlinkValue(SK_store, SK_store == SK_new);
    } else {
        // the node was not sane -- count it
        ++stats.builtInsaneNodes;
    }

    delete K_new;
}


/*!
 \param[in] K   a knowledge bubble (explicitly stored)
 \param[in] SK  a knowledge bubble (compactly stored)

 \note possible optimization: don't create a copy for the last label but use
       the object itself
 */
void StoredKnowledge::processNode(const Knowledge* K, StoredKnowledge* const SK) {
    static unsigned int calls = 0;

    // statistics output
    if (args_info.reportFrequency_arg and ++calls % args_info.reportFrequency_arg == 0) {
        message("%8d knowledges, %8d edges", stats.storedKnowledges, stats.storedEdges);
    }

    // traverse the labels of the interface and process K's successors
    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {

        // reduction rule: send leads to insane node
        if (not args_info.ignoreUnreceivedMessages_flag and SENDING(l) and not K->considerSendingEvent(l)) {
            continue;
        }

        // reduction rule: sequentialize receiving events
        // if current receiving event is not to be considered, continue
        if (args_info.seqReceivingEvents_flag and RECEIVING(l) and not K->considerReceivingEvent(l)) {
            continue;
        }

        // reduction rule: receive before send
        if (args_info.receivingBeforeSending_flag and not RECEIVING(l) and K->receivingHelps()) {
            break;
        }

        // reduction rule: only consider waitstates
        if (args_info.waitstatesOnly_flag and not RECEIVING(l) and not K->resolvableWaitstate(l)) {
            continue;
        }

        // recursion
        processSuccessor(K, SK, l);

        // reduction rule: quit, once all waitstates are resolved
        if (args_info.quitAsSoonAsPossible_flag and SK->sat(true)) {
            break;
        }

        // reduction rule: stop considering another sending event, if the
        // latest sending event considered succeeded
        /// \todo what about synchronous events?
        if (args_info.succeedingSendingEvent_flag and SENDING(l) and SK->sat(true)) {
            if ((args_info.correctness_arg != correctness_arg_livelock) or SK->is_final_reachable) {
                l = Label::first_sync;
            }
        }
    }

    SK->evaluateKnowledge();
}


/*!
 create the predecessor relation of all knowledges contained in the given set
 and then evaluate each member of the given set of knowledges and propagate
 the property of being insane accordingly

 \pre  knowledgeSet contains all nodes (knowledges) of the current SCC
 \post knowledgeSet is empty
*/
void StoredKnowledge::analyzeSCCOfKnowledges(std::set<StoredKnowledge*>& knowledgeSet) {
    // a temporary data structure to store the predecessor relation
    std::map<StoredKnowledge*, std::set<StoredKnowledge*> > tempPredecessors;

    // remember if from at least one node of the current SCC a final node is reachable
    bool is_final_reachable = false;

    // if it is not a TSCC, we have to evaluate each member of the SCC
    // first, we generate the predecessor relation between the members
    for (std::set<StoredKnowledge*>::const_iterator iScc = knowledgeSet.begin(); iScc != knowledgeSet.end(); ++iScc) {
        // for each successor which is part of the current SCC, register the predecessor
        for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
            if ((**iScc).successors[l-1] != NULL and (**iScc).successors[l-1] != empty and
                knowledgeSet.find((**iScc).successors[l-1]) != knowledgeSet.end()) {

                tempPredecessors[(**iScc).successors[l-1]].insert(*iScc);
            }
            // check if there exists a successor (within or without the current SCC) from which a
            // final node is reachable
            if ((**iScc).successors[l-1] != NULL and (**iScc).successors[l-1] != empty and
                (**iScc).successors[l-1]->is_final_reachable) {
                is_final_reachable = true;
            }
        }
    }

    // propagate that at least from one node of the current SCC a final node is reachable
    if (is_final_reachable) {
        for (std::set<StoredKnowledge*>::const_iterator iScc = knowledgeSet.begin(); iScc != knowledgeSet.end(); ++iScc) {
            (**iScc).is_final_reachable = true;
        }
    }

    // evaluate each member
    while (not knowledgeSet.empty()) {
        StoredKnowledge* currentKnowledge = *knowledgeSet.begin();
        knowledgeSet.erase(knowledgeSet.begin());

        if (currentKnowledge->is_sane and not currentKnowledge->sat()) {
            currentKnowledge->is_sane = 0;

            // tell each predecessor of current knowledge that this knowledge switched its status to insane
            /// \todo looks as if a set union would do the job here
            knowledgeSet.insert(tempPredecessors[currentKnowledge].begin(), tempPredecessors[currentKnowledge].end());

        } else if (args_info.correctness_arg == correctness_arg_livelock) {
            currentKnowledge->is_final_reachable = 1;
        }
    }
}


/*!
  move all transient markings to the end of the array and adjust size of the
  markings array

  \post all deadlock markings can be found between index 0 and sizeDeadlockMarkings
 */
void StoredKnowledge::rearrangeKnowledgeBubble() {
    // check the stored markings and remove all transient states
    unsigned int j = 0;

    while (j < sizeDeadlockMarkings) {
        assert(interface[j]);

        // find out whether marking is transient
        bool transient = false;

        // case 1: a final marking that is not a waitstate
        if (InnerMarking::inner_markings[inner[j]]->is_final and interface[j]->unmarked()) {
            // remember that this knowledge contains a final marking
            is_final = is_final_reachable = 1;

            // only if the final marking is not a waitstate, we're done
            if (not InnerMarking::inner_markings[inner[j]]->is_waitstate) {
                transient = true;
            }
        }

        // case 2: a resolved waitstate
        if (InnerMarking::inner_markings[inner[j]]->is_waitstate) {
            // check if DL is resolved by interface marking
            for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
                if (interface[j]->marked(l) and
                    InnerMarking::receivers[l].find(inner[j]) != InnerMarking::receivers[l].end()) {
                    transient = true;
                }
            }
        } else {
            // case 3: a transient marking
            transient = true;
        }

        // "hide" transient markings behind the end of the array
        if (transient) {
            InnerMarking_ID temp_inner = inner[j];
            InterfaceMarking* temp_interface = interface[j];

            inner[j] = inner[ sizeDeadlockMarkings-1 ];
            interface[j] = interface[ sizeDeadlockMarkings-1 ];

            inner[ sizeDeadlockMarkings-1 ] = temp_inner;
            interface[ sizeDeadlockMarkings-1 ] = temp_interface;

            --sizeDeadlockMarkings;
        } else {
            ++j;
        }
    }
}


/******************************************
 * CONSTRUCTOR, DESTRUCTOR, AND FINALIZER *
 ******************************************/

/*!
 converts a Knowledge object into a StoredKnowledge object

 \param[in] K  the knowledge to copy from
*/
StoredKnowledge::StoredKnowledge(const Knowledge* K)
        : is_final(0), is_final_reachable(0), is_sane(K->is_sane),
          is_on_tarjan_stack(1), sizeDeadlockMarkings(K->size),
          sizeAllMarkings(K->size), inner(NULL), interface(NULL),
          successors(NULL) {
    assert(sizeAllMarkings > 0);

    // reserve the necessary memory for the successors (fixed)
    successors = new StoredKnowledge*[Label::events];
    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
        // initialization is necessary to detect absent edges
        successors[l-1] = NULL;
    }

    // reserve the necessary memory for the internal and interface markings
    inner = new InnerMarking_ID[sizeAllMarkings];
    interface = new InterfaceMarking*[sizeAllMarkings];

    // finally copy data structure to C-style arrays
    size_t count = 0;

    // traverse the bubble and copy the markings into the C arrays
    for (std::map<InnerMarking_ID, std::vector<InterfaceMarking*> >::const_iterator pos = K->bubble.begin(); pos != K->bubble.end(); ++pos) {
        for (size_t i = 0; i < pos->second.size(); ++i, ++count) {
            // copy the inner marking and the interface marking
            inner[count] = pos->first;
            interface[count] = new InterfaceMarking(*(pos->second[i]));
        }
    }

    // we must not forget a marking
    assert(sizeAllMarkings == count);

    // check this knowledge for covering nodes
    if (args_info.cover_given) {
        Cover::checkKnowledge(this, K->bubble);
    }

    // move waitstates to front of bubble
    rearrangeKnowledgeBubble();
}


StoredKnowledge::~StoredKnowledge() {
    // delete the interface markings
    for (unsigned int i = 0; i < sizeAllMarkings; ++i) {
        delete interface[i];
    }
    delete[] interface;

    delete[] inner;
    delete[] successors;

    if (args_info.cover_given) {
        Cover::removeKnowledge(this);
    }

    tarjanMapping.erase(this);
}


void StoredKnowledge::finalize() {
    unsigned int count = 0;

    for (map<hash_t, vector<StoredKnowledge*> >::iterator it = hashTree.begin(); it != hashTree.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i, ++count) {
            delete it->second[i];
        }
    }

    status("StoredKnowledge: deleted %d objects", count);
}


/********************
 * MEMBER FUNCTIONS *
 ********************/

/*!
 adjust lowlink value of stored knowledge object according to Tarjan algorithm

 \param SK           new knowledge whose successors have been calculated completely
 \param newKnowledge SK has been newly created in calling function process()
*/
void StoredKnowledge::adjustLowlinkValue(const StoredKnowledge* const SK, const bool newKnowledge) const {
    // successor node is not new
    if (not newKnowledge) {
        if (SK->is_on_tarjan_stack) {
            // but it is still on the stack, compare lowlink and dfs value
            tarjanMapping[this].second = MINIMUM(tarjanMapping[this].second, tarjanMapping[SK].first);
        }
    } else {
        // successor node is new, compare lowlink values
        tarjanMapping[this].second = MINIMUM(tarjanMapping[this].second, tarjanMapping[SK].second);
    }
}


/*!
 if current knowledge is representative of an SCC (of knowledges) then evaluate current SCC
*/
void StoredKnowledge::evaluateKnowledge() {
    assert(not tarjanStack.empty());

    // check, if the current knowledge is a representative of a SCC
    // if so, get all knowledges within the SCC
    if (tarjanMapping[this].first == tarjanMapping[this].second) {

        unsigned int numberOfSccElements = 0;

        // node which has just been popped from Tarjan stack
        StoredKnowledge* poppedSK;

        // set of knowledges
        // first used to store the whole SCC, then it is used as a set storing those
        // knowledges that have to be evaluated again
        std::set<StoredKnowledge*> knowledgeSet;

        // found a TSCC
        if (tarjanMapping[this].first > StoredKnowledge::bookmarkTSCC) {
            StoredKnowledge::bookmarkTSCC = tarjanMapping[this].first;
        }

        // get (T)SCC
        do {
            // get and delete last element from stack
            poppedSK = tarjanStack.back();
            tarjanStack.pop_back();

            // remember that we removed this knowledge from the Tarjan stack
            poppedSK->is_on_tarjan_stack = 0;

            // remember this knowledge to be part of the current (T)SCC
            knowledgeSet.insert(poppedSK);

            ++numberOfSccElements;
        } while (this != poppedSK);


        // check if (T)SCC is trivial
        if (numberOfSccElements == 1) {
            // check if every deadlock is resolved -> if not, this knowledge is definitely not sane
            // deadlock freedom or livelock freedom will be treated in sat()
            (**knowledgeSet.begin()).is_sane = (**knowledgeSet.begin()).sat();

            ++stats.numberOfTrivialSCCs;
        } else if (numberOfSccElements > 1) {
            analyzeSCCOfKnowledges(knowledgeSet);

            stats.maxSCCSize = MAXIMUM(stats.maxSCCSize, numberOfSccElements);
            ++stats.numberOfNonTrivialSCCs;
        }
    }
}


/*!
 \return a pointer to a knowledge stored in the hash tree -- it is either
         "this" if the knowledge was not found in the hash tree or a pointer
         to a previously stored knowledge. In the latter case, the calling
         function can detect the duplicate
 */
StoredKnowledge* StoredKnowledge::store() {
    // we do not want to store the empty node
    assert(sizeAllMarkings != 0);

    // get the element's hash value
    const hash_t myHash(hash());

    // check if we find a bucket with that hash value
    std::map<hash_t, std::vector<StoredKnowledge*> >::const_iterator el = hashTree.find(myHash);
    if (el != hashTree.end()) {
        // we found an element with the same hash -- is it a collision?
        for (size_t i = 0; i < el->second.size(); ++i) {
            assert(el->second[i]);

            // compare the sizes
            if (sizeAllMarkings == el->second[i]->sizeAllMarkings) {
                // if still true after the loop, we found previously stored knowledge
                bool found = true;

                // compare the inner and interface markings
                for (unsigned int j = 0; (j < sizeAllMarkings and found); ++j) {
                    if (inner[j] != el->second[i]->inner[j] or
                        *interface[j] != *el->second[i]->interface[j]) {
                        found = false;
                        break;
                    }
                }

                // check if we found a previously stored knowledge
                if (found) {
                    // we found a previously stored element with the same
                    // markings -> return a pointer to this element
                    return el->second[i];
                }
            }
        }

        // this object was not found in the bucket -- this is a collision
        ++stats.hashCollisions;

        // update maximal bucket size (we add 1 as we will store this object later)
        stats.maxBucketSize = MAXIMUM(stats.maxBucketSize, hashTree[myHash].size() + 1);
    }

    // we need to store this object
    hashTree[myHash].push_back(this);
    ++stats.storedKnowledges;

    // set Tarjan values (first == dfs; second == lowlink)
    tarjanMapping[this].first = tarjanMapping[this].second = stats.storedKnowledges;

    // put knowledge on the Tarjan stack
    tarjanStack.push_back(this);

    // we return a pointer to the this object since it was newly stored
    return this;
}


hash_t StoredKnowledge::hash() const {
    hash_t result(0);

    for (unsigned int i = 0; i < sizeAllMarkings; ++i) {
        result += ((1 << i) * (inner[i]) + interface[i]->hash());
    }

    return result;
}


void StoredKnowledge::addSuccessor(const Label_ID& label, StoredKnowledge* const knowledge) {
    // we will never store label 0 (tau) -- hence decrease the label
    successors[label-1] = knowledge;

    ++stats.storedEdges;
}


/*!
 \return whether each deadlock in the knowledge is resolved

 \param checkOnTarjanStack in case of reduction rules "quit as soon as possible" and "succeeding sending event"
                           it has to be ensured that the successor node has been completely analyzed and thus
                           is off the Tarjan stack

 \pre the markings in the array (0 to sizeDeadlockMarkings-1) are deadlocks -- all transient
      states or unmarked final markings are removed from the marking array
*/
bool StoredKnowledge::sat(const bool checkOnTarjanStack) {
    // if we find a sending successor, this node is OK
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (successors[l-1] != NULL and successors[l-1] != empty and successors[l-1]->is_sane) {

            if (checkOnTarjanStack and successors[l-1]->is_on_tarjan_stack) {
                continue;
            }

            if (args_info.correctness_arg == correctness_arg_livelock and not successors[l-1]->is_final_reachable) {
                continue;
            }

            is_final_reachable = successors[l-1]->is_final_reachable;

            return true;
        }
    }

    // now each deadlock must have ?-successors
    for (unsigned int i = 0; i < sizeDeadlockMarkings; ++i) {
        bool resolved = false;

        // we found a deadlock -- check whether for at least one marked
        // output place exists a respective receiving edge
        for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {

            if (interface[i]->marked(l) and successors[l-1] != NULL and successors[l-1] != empty and
                successors[l-1]->is_sane) {

                if (checkOnTarjanStack and successors[l-1]->is_on_tarjan_stack) {
                    continue;
                }

                if (args_info.correctness_arg == correctness_arg_livelock and not successors[l-1]->is_final_reachable) {
                    continue;
                }

                is_final_reachable = successors[l-1]->is_final_reachable;

                resolved = true;
                break;
            }
        }

        // check if a synchronous action can resolve this deadlock
        for (Label_ID l = Label::first_sync; l <= Label::last_sync; ++l) {
            if (InnerMarking::synchs[l].find(inner[i]) != InnerMarking::synchs[l].end() and
                successors[l-1] != NULL and successors[l-1] != empty and successors[l-1]->is_sane) {

                if (checkOnTarjanStack and successors[l-1]->is_on_tarjan_stack) {
                    continue;
                }

                if (args_info.correctness_arg == correctness_arg_livelock and not successors[l-1]->is_final_reachable) {
                    continue;
                }

                is_final_reachable = successors[l-1]->is_final_reachable;

                resolved = true;
                break;
            }
        }

        // the deadlock is neither resolved nor a final marking
        if (not resolved and not(InnerMarking::inner_markings[inner[i]]->is_final and interface[i]->unmarked())) {
            return false;
        }
    }

    // if we reach this line, every deadlock was resolved
    return true;
}


/*!
 \post All nodes that are reachable from the initial node are added to the
       set "seen".
 */
void StoredKnowledge::traverse() {
    if (seen.insert(this).second) {
        for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
            if (successors[l-1] != NULL and successors[l-1] != empty and
                (successors[l-1]->is_sane or args_info.diagnose_given)) {
                successors[l-1]->traverse();
            }
        }
    }
}


/****************************************
 * OUTPUT FUNCTIONS (STATIC AND MEMBER) *
 ****************************************/

void StoredKnowledge::fileHeader(std::ostream &file) {
    file << "{\n  generator:    " << PACKAGE_STRING
        << " (" << CONFIG_BUILDSYSTEM ")"
        << "\n  invocation:   " << invocation << "\n  events:       "
        << static_cast<unsigned int>(Label::send_events) << " send, "
        << static_cast<unsigned int>(Label::receive_events) << " receive, "
        << static_cast<unsigned int>(Label::sync_events) << " synchronous"
        << "\n  statistics:   " << seen.size() << " nodes\n}\n\n";
}

/*!
  \param[in,out] file  the output stream to write the OG to

  \note Fiona identifies node numbers by integers. To avoid numbering of
        nodes, the pointers are casted to integers. Though ugly, it still is
        a valid numbering.
 */
void StoredKnowledge::output_og(std::ostream& file) {
    fileHeader(file);
    file << "INTERFACE\n";

    if (Label::receive_events > 0) {
        file << "  INPUT\n    ";
        bool first = true;
        for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
            if (not first) {
                file << ", ";
            }
            first = false;
            file << Label::id2name[l];
        }
        file << ";\n";
    }

    if (Label::send_events > 0) {
        file << "  OUTPUT\n    ";
        bool first = true;
        for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
            if (not first) {
                file << ", ";
            }
            first = false;
            file << Label::id2name[l];
        }
        file << ";\n";
    }

    if (Label::sync_events > 0) {
        file << "  SYNCHRONOUS\n    ";
        bool first = true;
        for (Label_ID l = Label::first_sync; l <= Label::last_sync; ++l) {
            if (not first) {
                file << ", ";
            }
            first = false;
            file << Label::id2name[l];
        }
        file << ";\n";
    }

    file << "\nNODES\n";

    // the root
    root->print(file);

    // all other nodes
    for (set<StoredKnowledge*>::const_iterator it = seen.begin(); it != seen.end(); ++it) {
        if (*it != root) {
            (**it).print(file);
        }
    }

    // print empty node unless we print an automaton
    if (not args_info.sa_given and emptyNodeReachable) {
         // the empty node
        file << "  0";
        if (args_info.formula_arg == formula_arg_cnf) {
            file << " : true\n";
        } else {
            file << " :: T\n";
        }

        // empty node loops
        for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
            file << "    " << Label::id2name[l]  << " -> 0\n";
        }
    }
}


/*!
  \param[in,out] file  the output stream to write the OG to

  \note Fiona identifies node numbers by integers. To avoid numbering of
        nodes, the pointers are casted to integers. Though ugly, it still is
        a valid numbering.

  \deprecated This is the old OG file format -- it is only kept here for
              compatibility reasons and to test generated operating
              guidelines.
 */
void StoredKnowledge::output_ogold(std::ostream& file) {
    fileHeader(file);
    file << "INTERFACE\n";
    file << "  INPUT\n";
    bool first = true;
    for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
        if (not first) {
            file << ",\n";
        }
        first = false;
        file << "    " << Label::id2name[l];
    }
    file << ";\n";

    file << "  OUTPUT\n";
    first = true;
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (not first) {
            file << ",\n";
        }
        first = false;
        file << "    " << Label::id2name[l];
    }
    file << ";\n";

    file << "\nNODES\n";

    // the empty node
    file << "  0 : true";

    for (set<StoredKnowledge*>::const_iterator it = seen.begin(); it != seen.end(); ++it) {
        file << ",\n  " << reinterpret_cast<size_t>(*it) << " : " << (**it).formula();
    }
    file << ";\n\n";

    file << "INITIALNODE\n  " << reinterpret_cast<size_t>(root) << ";\n\n";

    file << "TRANSITIONS\n";
    first = true;
    for (set<StoredKnowledge*>::const_iterator it = seen.begin(); it != seen.end(); ++it) {
        for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
            if ((**it).successors[l-1] != NULL and
                (**it).successors[l-1] != empty and
                (seen.find((**it).successors[l-1]) != seen.end())) {

                if (first) {
                    first = false;
                } else {
                    file << ",\n";
                }
                file << "  " << reinterpret_cast<size_t>(*it) << " -> "
                    << reinterpret_cast<size_t>((**it).successors[l-1])
                    << " : " << PREFIX(l) << Label::id2name[l];
            } else {
                // edges to the empty node
                if ((**it).successors[l-1] == empty) {
                    if (first) {
                        first = false;
                    } else {
                        file << ",\n";
                    }
                    file << "  " << reinterpret_cast<size_t>(*it) << " -> 0"
                        << " : " << PREFIX(l) << Label::id2name[l];
                }
            }
        }
    }

    // empty node loops
    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
        if (first) {
            first = false;
        } else {
            file << ",\n";
        }
        file << "  0 -> 0 : " << PREFIX(l) << Label::id2name[l];
    }

    file << ";\n";
}


/*!
  \bug Final states should not have outgoing tau or sending events.
 */
void StoredKnowledge::print(std::ostream& file) const {
    file << "  " << reinterpret_cast<size_t>(this);

    if (args_info.sa_given) {
        if (this == root and is_final) {
            file << " : INITIAL, FINAL";
        } else {
            if (this == root) {
                file << " : INITIAL";
            }
            if (is_final) {
                file << " : FINAL";
            }
        }
    } else {
        switch (args_info.formula_arg) {
            case(formula_arg_cnf): {
                file << " : " << formula();
                break;
            }
            case(formula_arg_bits): {
                string form(bits());
                if (form != "") {
                    file << " :: " << form;
                }
                break;
            }
        }
    }

    file << "\n";

    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
        if (successors[l-1] != NULL and
            successors[l-1] != empty and
        (seen.find(successors[l-1]) != seen.end())) {
            file << "    " << Label::id2name[l] << " -> "
                << reinterpret_cast<size_t>(successors[l-1])
                << "\n";
        } else {
            if (successors[l-1] == empty and not args_info.sa_given) {
                emptyNodeReachable = true;
                file << "    " << Label::id2name[l] << " -> 0\n";
            }
        }
    }
}


/*!
 \return a string representation of the formula

 \note This function is also used for an operating guidelines output for
       Fiona.

 \todo Adjust comments and variable names to reflect the fact that we also
       treat synchronous communication.
 */
std::string StoredKnowledge::formula() const {
    set<string> sendDisjunction;
    set<set<string> > receiveDisjunctions;

    // collect outgoing !-edges
    for (Label_ID l = Label::first_send; l <= Label::last_send; ++l) {
        if (successors[l-1] != NULL and successors[l-1]->is_sane) {
            if (args_info.fionaFormat_flag) {
                sendDisjunction.insert(PREFIX(l) + Label::id2name[l]);
            } else {
                sendDisjunction.insert(Label::id2name[l]);
            }
        }
    }

    // collect outgoing ?-edges and #-edges for the deadlocks
    bool dl_found = false;
    for (unsigned int i = 0; i < sizeDeadlockMarkings; ++i) {
        dl_found = true;
        set<string> temp(sendDisjunction);

        // sending
        for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
            if (interface[i]->marked(l) and
                successors[l-1] != NULL and successors[l-1] != empty and
                successors[l-1]->is_sane) {

                if (args_info.fionaFormat_flag) {
                    temp.insert(PREFIX(l) + Label::id2name[l]);
                } else {
                    temp.insert(Label::id2name[l]);
                }
            }
        }

        // synchronous communication
        for (Label_ID l = Label::first_sync; l <= Label::last_sync; ++l) {
            if (InnerMarking::synchs[l].find(inner[i]) != InnerMarking::synchs[l].end() and
                successors[l-1] != NULL and successors[l-1] != empty and
                successors[l-1]->is_sane) {
                if (args_info.fionaFormat_flag) {
                    temp.insert(PREFIX(l) + Label::id2name[l]);
                } else {
                    temp.insert(Label::id2name[l]);
                }
            }
        }

        if (interface[i]->unmarked() and InnerMarking::inner_markings[inner[i]]->is_final) {
            temp.insert("final");
        }

        receiveDisjunctions.insert(temp);
    }

    if (not dl_found) {
        return "true";
    }

    // create the formula
    string formula;
    if (not receiveDisjunctions.empty()) {
        for (set<set<string> >::iterator it = receiveDisjunctions.begin(); it != receiveDisjunctions.end(); ++it) {
            if (it != receiveDisjunctions.begin()) {
                formula += " * ";
            }
            if (it->size() > 1) {
                formula += "(";
            }
            for (set<string>::iterator it2 = it->begin(); it2 != it->end(); ++it2) {
                if (it2 != it->begin()) {
                    formula += " + ";
                }
                formula += *it2;
            }
            if (it->size() > 1) {
                formula += ")";
            }
        }
    }

    // required for diagnose mode in which a non-final node might have no
    // successors
    if (formula.empty()) {
        formula = "false";
    }

    return formula;
}


/*!
 \return "T" if formula is true (no deadlocks), "S" if formula can only be
         satisfied by sending events; "F" if formula contains the "final"
         literal

 \pre  all markings are deadlocks
 \pre  the node is sane, i.e. "sane()" would return true

 \note If the net contains synchronous communication channels, the main
       method already aborts with an error as the 2-bit annotations are not
       defined in this case.
 */
std::string StoredKnowledge::bits() const {
    if (sizeDeadlockMarkings == 0) {
        // the knowledge has no deadlock
        return "T";
    }

    if (is_final) {
        // the formula contains final
        return "F";
    }

    // traverse the deadlocks
    for (unsigned int i = 0; i < sizeDeadlockMarkings; ++i) {
        bool resolved = false;

        // check whether receiving resolves this deadlock
        for (Label_ID l = Label::first_receive; l <= Label::last_receive; ++l) {
            if (interface[i]->marked(l) and
                successors[l-1] != NULL and successors[l-1] != empty and
                successors[l-1]->is_sane) {
                resolved = true;
                break;
            }
        }

        if (not resolved) {
            // the deadlock can not be resolved by receiving -> must send
            return "S";
        }
    }

    // no bit needed
    return "";
}


/*!
  \param[in,out] file  the output stream to write the dot representation to

  \note  The empty node has the number 0 and is only drawn if the command line
         parameter "--showEmptyNode" was given. For each node and receiving
         label, we add an edge to the empty node if this edge is not present
         before.
*/
void StoredKnowledge::output_dot(std::ostream& file) {
    file << "digraph G {\n"
        << " node [fontname=\"Helvetica\" fontsize=10]\n"
        << " edge [fontname=\"Helvetica\" fontsize=10]\n";

    // draw the nodes
    for (map<hash_t, vector<StoredKnowledge*> >::iterator it = hashTree.begin(); it != hashTree.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            if ((it->second[i]->is_sane or args_info.diagnose_given) and
                (seen.find(it->second[i]) != seen.end())) {

                string formula;
                switch (args_info.formula_arg) {
                    case(formula_arg_cnf): formula = it->second[i]->formula(); break;
                    case(formula_arg_bits): formula = it->second[i]->bits(); break;
                }
                file << "\"" << it->second[i] << "\" [label=\"" << formula << "\\n";

                if (args_info.diagnose_given and not it->second[i]->is_sane) {
                    file << "is not sane\\n";
                }

                if (args_info.showWaitstates_flag) {
                    for (unsigned int j = 0; j < it->second[i]->sizeDeadlockMarkings; ++j) {
                        file << "m" << static_cast<size_t>(it->second[i]->inner[j]) << " ";
                        file << *(it->second[i]->interface[j]) << " (w)\\n";
                    }
                }

                if (args_info.showTransients_flag) {
                    for (unsigned int j = it->second[i]->sizeDeadlockMarkings; j < it->second[i]->sizeAllMarkings; ++j) {
                        file << "m" << static_cast<size_t>(it->second[i]->inner[j]) << " ";
                        file << *(it->second[i]->interface[j]) << " (t)\\n";
                    }
                }

                file << "\"]\n";

                // draw the edges
                for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
                    if (it->second[i]->successors[l-1] != NULL and
                        (seen.find(it->second[i]->successors[l-1]) != seen.end()) and
                    (args_info.showEmptyNode_flag or it->second[i]->successors[l-1] != empty)) {
                        file << "\"" << it->second[i] << "\" -> \""
                            << it->second[i]->successors[l-1]
                            << "\" [label=\"" << PREFIX(l)
                            << Label::id2name[l] << "\"]\n";
                    }

                    // draw edges to the empty node if requested
                    if (args_info.showEmptyNode_flag and
                        it->second[i]->successors[l-1] == empty) {
                        emptyNodeReachable = true;
                        file << "\"" << it->second[i] << "\" -> 0"
                            << " [label=\"" << PREFIX(l)
                            << Label::id2name[l] << "\"]\n";
                    }
                }
            }
        }
    }

    // draw the empty node if it is requested and reachable
    if (args_info.showEmptyNode_flag and emptyNodeReachable) {
        file << "0 [label=\"";
        if (args_info.formula_arg == formula_arg_cnf) {
            file << "true";
        } else {
            file << "T";
        }
        file << "\"]\n";

        for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
            file << "0 -> 0 [label=\"" << PREFIX(l) << Label::id2name[l] << "\"]\n";
        }
    }

    file << "}\n";
}


void StoredKnowledge::output_migration(std::ostream& o) {
    map<InnerMarking_ID, map<StoredKnowledge*, set<InterfaceMarking*> > > migrationInfo;

    for (std::set<StoredKnowledge*>::const_iterator it = seen.begin(); it != seen.end(); ++it) {
        // traverse the bubble
        for (unsigned int i = 0; i < (**it).sizeAllMarkings; ++i) {
            InnerMarking_ID inner = (**it).inner[i];
            InterfaceMarking* interface = (**it).interface[i];
            StoredKnowledge* knowledge = *it;

            migrationInfo[inner][knowledge].insert(interface);
        }
    }

    // print information on the interface
    o << "INTERFACE [";
    for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
        if (l > Label::first_receive) {
            o << ",";
        }
        o << Label::id2name[l];
    }
    o << "]\n\n";

    // iterate the inner markings
    for (map<InnerMarking_ID, map<StoredKnowledge*, set<InterfaceMarking*> > >::iterator it1 = migrationInfo.begin(); it1 != migrationInfo.end(); ++it1) {

        // iterate the interface markings
        for (map<StoredKnowledge*, set<InterfaceMarking*> >::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {

            // iterate the knowledge bubbles
            for (set<InterfaceMarking*>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3) {
                o << (size_t) it1->first << " " << (size_t) it2->first << " " << **it3 << "\n";
            }
        }
    }
}

