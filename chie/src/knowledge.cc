#include "knowledge.h"
#include <algorithm>
#include <sstream>
#include <stack>
#include <queue>


// set of stored knowledges
HashMap<Knowledge, Knowledge::hash> Knowledge::storedKnowledge_;

// pointer to related product automaton
ProductAutomaton* Knowledge::productAutomaton_ = NULL;

/*!
 * \brief hash function
 */
unsigned int Knowledge::hash(Knowledge& k) {
    unsigned int result = 0;

    for (unsigned int i = 0; i < k.bubble_.size(); ++i) {
        result += ProductAutomatonState::hash(*k.bubble_[i]);
    }

    return result;
}

/*!
 * \brief deletes all knowledge bubbles
 */
void Knowledge::deleteAll() {
    storedKnowledge_.clearDelete();
    productAutomaton_ = NULL;
}


/*!
 * \brief constructor
 */
Knowledge::Knowledge(ProductAutomaton& automaton) {
    productAutomaton_ = &automaton;
    bubble_.push_back(automaton.initialState_);
}

/*!
 * \brief copy constructor
 */
Knowledge::Knowledge(std::set<ProductAutomatonState*>& bubble) {
    FOREACH(state, bubble) {
        bubble_.push_back(*state);
    }
}


/*!
 * \brief comparison operator
 */
bool Knowledge::operator==(Knowledge& other) {
    return (bubble_ == other.bubble_);
}



/*!
 * \brief compute closure
 */
void Knowledge::closure() {
    // set to remove doubles
    std::set<ProductAutomatonState*> tmpBubble;

    // copy bubble
    for (unsigned int i = 0; i < bubble_.size(); ++i) {
        tmpBubble.insert(bubble_[i]);
    }

    // using actual bubble as stack
    while (bubble_.size() > 0) {
        ProductAutomatonState* state = bubble_.back();  // get next state
        bubble_.pop_back(); // remove from stack

        // itereate over successors
        FOREACH(successors, state->specificationSuccessors_) {
            for (unsigned int i = 0; i < successors->second.size(); ++i) {
                if (tmpBubble.count(successors->second[i]) == 0) { // unknown state
                    tmpBubble.insert(successors->second[i]); // insert to new bubble
                    bubble_.push_back(successors->second[i]); // schedule for processing
                }
            }
        }
    }

    // write back
    FOREACH(state, tmpBubble) {
        bubble_.push_back(*state);
    }

    // sort bubble
    sort(bubble_.begin(), bubble_.end(), ProductAutomatonState::isLessThan);
}


/*!
 * \brief compute successor bubble
 */
std::map<unsigned int, Knowledge*> Knowledge::genereateSuccessors() {
    // generate result
    std::map<unsigned int, Knowledge*> result;

    // temporary knowledge bubbles
    std::set<ProductAutomatonState*> successors[ServiceAutomaton::labels.size()];
    for (unsigned int i = 0; i < bubble_.size(); ++i) {
        FOREACH(succ, bubble_[i]->testCaseSuccessors_) {
            for (unsigned int j = 0; j < succ->second.size(); ++j) {
                successors[succ->first].insert(succ->second[j]);
            }
        }
    }

    // generate successors
    for (unsigned int i = 0; i < ServiceAutomaton::labels.size(); ++i) {
        if (successors[i].size() > 0) {
            Knowledge* s = new Knowledge(successors[i]);
            s->closure();
            result[i] = s;
        }
    }

    // return result
    return result;
}

/*
 * clean cache in weak reachability check
 */
inline void cleanWeakReceivabilityCache(std::map<Knowledge*, unsigned short*>& cache) {
    FOREACH(item, cache) {
        delete(item->second);
    }
}

/*!
 * \brief check weak receivability
 */
bool Knowledge::checkWeakReceivability() {
    // compute own closure and store
    closure();
    storedKnowledge_.insert(*this);

    // set of knowledges without successors
    std::vector<Knowledge*> noSuccessorStates;

    // 2do stack for DFS
    std::stack<Knowledge*> toDoStack;
    toDoStack.push(this);

    // perform DFS over knowledges
    while (toDoStack.size() > 0) {
        // next knowledge
        Knowledge* current = toDoStack.top();
        toDoStack.pop();

        // get successors
        std::map<unsigned int, Knowledge*> successors = current->genereateSuccessors();
        if (successors.size() == 0) { // knowledge without successor found
            noSuccessorStates.push_back(current);
            continue;
        }

        FOREACH(succ, successors) {
            Knowledge* inserted = storedKnowledge_.insert(*succ->second);
            if (inserted == succ->second) {
                // new knowledge - schedule for DFS
                toDoStack.push(inserted);
            } else {
                // already seen knowledge; delete double
                delete(succ->second);
            }

            // insert predecessor
            inserted->predecessors_[succ->first].push_back(current);
        }
    }

    // cache of message status in each knowledge bubble
    std::map<Knowledge*, unsigned short*> messageCache;
    enum {
        MC_NONE = 0,
        MC_NEED_TO_REMOVE = 1,
        MC_CAN_REMOVE = 2,
        MC_NEED_TO_AND_CAN_REMOVE = 3,
        MC_TRIED_TO_REMOVE = 4,
        MC_NEED_TO_AND_TRIED_TO_REMOVE = 5
    };
    int length = ServiceAutomaton::lastReceiveID + 1 - ServiceAutomaton::firstReceiveID;

    // iterate over states without successors
    for (unsigned int i = 0; i < noSuccessorStates.size(); ++i) {
        // current knowledge
        Knowledge* current = noSuccessorStates[i];
        if (messageCache[current] == NULL) {
            messageCache[current] = new unsigned short[length];
            for (unsigned int jj = 0; jj < length; ++jj) {
                messageCache[current][jj] = MC_NONE;
            }
        }

        // collect leftover messages
        for (unsigned int j = 0; j < current->bubble_.size(); ++j) { // for all states in this bubble
            for (unsigned int k = ServiceAutomaton::firstReceiveID; k <= ServiceAutomaton::lastReceiveID; ++k) { // for all messages sent by the test case
                if ((*current->bubble_[j]->channelState_)[k - 1] > 0) { // if there is such a message left
                    // we need to remove it somewhere
                    messageCache[current][k - ServiceAutomaton::firstReceiveID] |= MC_NEED_TO_REMOVE;
                } else {
                    // there is a path to remove this message if it is left over somewhere
                    messageCache[current][k - ServiceAutomaton::firstReceiveID] |= MC_CAN_REMOVE;
                }
                messageCache[current][k - ServiceAutomaton::firstReceiveID] |= MC_TRIED_TO_REMOVE; // we tried to remove this message in this bubble
            }
        }

        // for each predecessor: we need to remove all messages that could not be removed here
        std::queue<Knowledge*> bfsQueue;
        bfsQueue.push(current);
        while (!bfsQueue.empty()) { // while queue not empty
            // get next node
            current = bfsQueue.front();
            bfsQueue.pop();

            // set of predecessors to be scheduled
            std::set<Knowledge*> nextToSchedule;

            for (unsigned int j = ServiceAutomaton::firstReceiveID; j <= ServiceAutomaton::lastReceiveID; ++j) { // for all messages sent by the test case
                // if message need to be removed but was not yet
                if ((messageCache[current][j - ServiceAutomaton::firstReceiveID] & MC_NEED_TO_AND_TRIED_TO_REMOVE) == MC_NEED_TO_REMOVE) {
                    // try to remove in this bubble
                    for (unsigned int k = 0; k < current->bubble_.size(); ++k) {
                        if ((*current->bubble_[k]->channelState_)[j - 1] == 0) { // found a state with removed message
                            messageCache[current][j - ServiceAutomaton::firstReceiveID] |= MC_CAN_REMOVE;
                            break;
                        }
                    }

                    // mark as tried
                    messageCache[current][j - ServiceAutomaton::firstReceiveID] |= MC_TRIED_TO_REMOVE;
                }

                // if message need to be removed but could not
                if ((messageCache[current][j - ServiceAutomaton::firstReceiveID] & MC_NEED_TO_AND_CAN_REMOVE) == MC_NEED_TO_REMOVE) {
                    // need to delegate to predecessors
                    FOREACH(predecessors, current->predecessors_) {
                        if (predecessors->first == j) {
                            // clean cache
                            cleanWeakReceivabilityCache(messageCache);

                            // get a bad node
                            productAutomaton_->badState_ = predecessors->second[0]->bubble_[0];

                            // generate error message
                            std::stringstream ss;
                            ss << "message '" << ServiceAutomaton::labels[j] << "' sent by test case automaton in state '"
                               << productAutomaton_->badState_->testCaseState_ << "' is not weak receivable\n";
                            productAutomaton_->errorMessage = ss.str();

                            // got to this node by sending not removable message i.e. this message is not weak receivable
                            return false;
                        }

                        for (unsigned int k = 0; k < predecessors->second.size(); ++k) {
                            // get predecessor
                            Knowledge* predecessor = predecessors->second[k];
                            // create cache
                            if (messageCache[predecessor] == NULL) {
                                messageCache[predecessor] = new unsigned short[length];
                                for (unsigned int jj = 0; jj < length; ++jj) {
                                    messageCache[predecessor][jj] = MC_NONE;
                                }
                            }

                            // mark message as need to remove
                            messageCache[predecessor][j - ServiceAutomaton::firstReceiveID] |= MC_NEED_TO_REMOVE;

                            // if not tried already
                            if ((messageCache[predecessor][j - ServiceAutomaton::firstReceiveID] & MC_TRIED_TO_REMOVE) == MC_NONE) {
                                // schedule in BFS
                                nextToSchedule.insert(predecessor);
                            }
                        }
                    }
                }
            }

            // actual scheduling
            FOREACH(predecessor, nextToSchedule) {
                bfsQueue.push(*predecessor);
            }
        }
    }

    // clean cache
    cleanWeakReceivabilityCache(messageCache);

    // could remove all leftovers so all messages are weak receivable
    return true;
}
