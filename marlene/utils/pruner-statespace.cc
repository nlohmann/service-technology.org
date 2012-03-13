#include <iostream>
#include <algorithm>
#include <macros.h>
#include "pruner-statespace.h"

std::map<unsigned int, shared_ptr<State> > State::stateSpace;

std::map<std::string, std::string> State::abstractedIn;
std::map<std::string, std::string> State::abstractedOut;
std::map<std::string, std::string> State::abstractedSync;

std::map<std::string, std::string> State::remainingIn;
std::map<std::string, std::string> State::remainingOut;
std::map<std::string, std::string> State::remainingSync;

unsigned int State::messageBound = 1;

State::State(long number, ListOfPairs::List_ptr marking,
             ListOfPairs::List_ptr transitions, bool isFinal, unsigned int lowlink,
             List<unsigned int>::List_ptr sccmember) {
    this->number = number;
    this->marking = marking;
    this->transitions = transitions;
    this->isFinal = isFinal;
    this->finalReachable = isFinal;
    this->finalEnforceable = isFinal;
    this->lowlink = lowlink;
    this->index = number;
    this->lastMessagesReset = 0;
    this->sccmember = sccmember;
}

State::~State() {
    this->marking = ListOfPairs::List_ptr();
    this->transitions = ListOfPairs::List_ptr();
    this->sccmember = List<unsigned int>::List_ptr();
}

void State::output() {

    using namespace std;

    std::map<unsigned int, bool> seen;

    stateSpace[0]->output(seen);
}

void State::output(std::map<unsigned int, bool>& seen) {

    using namespace std;

    if (not seen[number]) {
        seen[number] = true;

        //shared_ptr<State> state(stateSpace[stateNo]);

        // first: output all successors
        if (transitions.get() != 0) {
            for (ListOfPairs::List_ptr transition = transitions;
                    transition.get() != 0;
                    stateSpace[transition->getValue()->getSecond()]->output(seen), transition =
                        transition->getNext())
                ;

        }

        if (isFinal) status("State %d is a final state.", index);
        cout << "STATE " << index << " Lowlink: " << lowlink;
        if (sccmember.get() != 0) {
            cout << " SCC:";
            for (List<unsigned int>::List_ptr next = sccmember;
                    next.get() != 0; cout << " " << next->getValue(), next =
                        next->getNext())
                ;
        }
        if (marking.get() != 0) {
            bool first = true;
            for (ListOfPairs::List_ptr marking = this->marking;
                    marking.get() != 0;
                    cout << (first ? "" : ",") << endl
                    << marking->getValue()->getFirst() << " : "
                    << marking->getValue()->getSecond(), marking =
                        marking->getNext(), first = false)
                ;

        }
        cout << endl;
        // if (isFinal) cout << "this is a final state!" << endl;
        if (transitions.get() != 0) {
            for (ListOfPairs::List_ptr transition = transitions;
                    transition.get() != 0;
                    cout
                    << endl
                    << transition->getValue()->getFirst()
                    << " -> "
                    << stateSpace[transition->getValue()->getSecond()]->index, transition =
                        transition->getNext())
                ;

        }
        cout << endl;
        cout << endl;
    }
}

bool State::checkFinalReachable() {

    std::map<unsigned int, bool> seen;
    unsigned int count = 0;
    unsigned int old_count = 0;

    for(std::map<unsigned int, shared_ptr<State> >::const_iterator state = stateSpace.begin(); state != stateSpace.end(); ++state) {
        state->second->finalReachable = state->second->isFinal;
        state->second->finalEnforceable = state->second->isFinal;
        state->second->lastMessagesReset = 0;
        state->second->messages.clear();
    }
    bool result = false;
    do {
        old_count = count;
        count = 0;
        result = stateSpace[0]->checkFinalReachable(seen, count);
        seen.clear();
    } while (count > old_count);
    status("There are %d state(s) for which a final state is reachable.",
           count);
    return result;
}

bool State::checkFinalReachable(std::map<unsigned int, bool>& seen , unsigned int & count) {

    //shared_ptr<State> state = stateSpace[stateNo];

    bool result = isFinal || this->finalReachable;
    seen[number] = true;
    if (transitions.get() != 0) {
        for (ListOfPairs::List_ptr transition = transitions;
                transition.get() != 0; transition = transition->getNext()) {

            if (not seen[transition->getValue()->getSecond()]) {
                bool resultstep = stateSpace[transition->getValue()->getSecond()]->checkFinalReachable(seen, count);
                result = result || resultstep;
            } else {
                result = result || stateSpace[transition->getValue()->getSecond()]->finalReachable;
            }
        }
        if (result) {
            ++count;
        }
    }
    this->finalReachable = result;
    return result;
}

void State::tauFolding() {

    std::map< unsigned int, bool> seen;
    // brauchen wir das überhaupt?
    // std::map< unsigned int, bool> statesFolded;

    stateSpace[0]->tauFolding(seen);
    calculateSCC();
}

void State::tauFolding(std::map<unsigned int, bool>& seen) {

    seen[this->number] = true;
    std::map<unsigned int, bool> dfsSeen;

    // status("Tau folding for state %d", this->number);

    ListOfPairs::List_ptr retValue = followTau(dfsSeen);
    this->transitions = retValue;
    for (ListOfPairs::List_ptr transition = transitions;
            transition.get() != 0; transition = transition->getNext()) {
        shared_ptr<State> successor =
            stateSpace[transition->getValue()->getSecond()];
        if (not seen[successor->number]) {
            successor->tauFolding(seen);
        }
    }
}

ListOfPairs::List_ptr State::followTau(std::map<unsigned int, bool>& dfsSeen) {

    dfsSeen[this->number] = true;
    ListOfPairs::List_ptr result;
    for (ListOfPairs::List_ptr transition = transitions;
            transition.get() != 0; transition = transition->getNext()) {
        shared_ptr<State> successor =
            stateSpace[transition->getValue()->getSecond()];
        ListOfPairs::List_ptr retValue;
        if(transitionsIsTau(transition->getValue()->getFirst())) {
            // status("Follow tau transition from %d to %d", this->number, successor->number);
            if (successor->isFinal) {
                //status("It'a a final state (%d)", successor->number);
                retValue = ListOfPairs::List_ptr(new ListOfPairs(transition->getValue()));
            }
            else if (successor->transitions.get() == 0) {
                // status("It'a a deadlock (%d)", successor->number);
                retValue = ListOfPairs::List_ptr(new ListOfPairs(transition->getValue()));
            } else {
                retValue = successor->followTau(dfsSeen);
                // a livelock has no outgoing non-tau-steps
                if (retValue.get() == 0 && successor->sccmember->length() > 1) {
                    // status("It'a a livelock (%d)", successor->number);
                    retValue = ListOfPairs::List_ptr(new ListOfPairs(transition->getValue()));
                }
            }
        }
        else {
            // status("Found non-tau transition from %d to %d", this->number, successor->number);
            retValue = ListOfPairs::List_ptr(new ListOfPairs(transition->getValue()));
        }
        if (result.get() == 0) {
            result = retValue;
        } else {
            result->push_back(retValue);
        }
    }
    return result;
}

unsigned int noSCC;

void State::calculateSCC() {
    unsigned int index = 0;
    unsigned int lowlink = 0;

    noSCC = 0;

    std::map<unsigned int, bool> seen;
    std::list<unsigned int> stack;
    stateSpace[0]->calculateSCC(index, lowlink, seen, stack);
    status("Found %d SCCs", noSCC);
}


void State::calculateSCC(unsigned int& index,
                         unsigned int& lowlink, std::map<unsigned int, bool>& seen
                         , std::list<unsigned int>& stack) {
    // shared_ptr<State> state = stateSpace[stateNo];

    seen[number] = true;

    this->index = index;
    this->lowlink = index;
    ++index;

    stack.push_front(this->index);

    for (ListOfPairs::List_ptr transition = transitions;
            transition.get() != 0; transition = transition->getNext()) {
        shared_ptr<State> successor =
            stateSpace[transition->getValue()->getSecond()];
        if (not seen[successor->number]) {
            successor->calculateSCC(index, lowlink, seen, stack);
            this->lowlink =
                (this->lowlink < successor->lowlink) ?
                this->lowlink : successor->lowlink;
        } else if (find(stack.begin(), stack.end(), successor->index)
                   != stack.end()) {
            this->lowlink =
                (this->lowlink < successor->index) ?
                this->lowlink : successor->index;
        }
    }

    if (this->lowlink == this->index) {
        List<unsigned int>::List_ptr newSCC;
        unsigned int w = 0;
        unsigned int count = 0;
        do {
            w = stack.front();
            stack.pop_front();
            if (newSCC.get() == 0) {
                newSCC = List<unsigned int>::List_ptr(
                             new List<unsigned int>(w));
            } else {
                newSCC->push_back(w);
            }
            ++count;
        } while (w != this->index);
        if (count > 1) ++noSCC;
        this->sccmember = newSCC;

    } else {
        this->sccmember = List<unsigned int>::List_ptr();
    }

}

void State::prune() {

    unsigned int count = 0;
    unsigned int old_count = 0;
    unsigned int sum = 0;

    checkFinalReachable();
    calculateSCC();

    do {
        std::map<unsigned int, bool> seen;
        std::map<unsigned int, bool> active;
        old_count = count;
        stateSpace[0]->prune(seen, active, count);
        status("removed %d transitions in iteration \n\n\n\n", count - old_count);
//        sum += count;
        checkFinalReachable();
        calculateSCC();
        seen.clear();
    } while (count > old_count);
    status("removed %d transitions in total", count);
}

void State::prune(std::map<unsigned int, bool>& seen
                  , std::map<unsigned int, bool>& active, unsigned int & count) {

    // shared_ptr<State> state = stateSpace[stateNo];

    std::map<std::string, bool> labelBlacklist;
    // for message bound violation, that cannot be avoided set this to true
    bool tauMBV = false;

    // now we have seen this state
    seen[number] = true;
    active[number] = true;
    // if it has transitions, process successors
    if (transitions.get() != 0) {

        for (ListOfPairs::List_ptr transition = transitions;
                transition.get() != 0; transition = transition->getNext()) {

            // expect to keep the transition
            //bool keepTransition = true;

            // this is the successor state
            shared_ptr<State> successor =
                stateSpace[transition->getValue()->getSecond()];
            // and this is the firing transition
            std::string transitionname = transition->getValue()->getFirst();

            // if final state is not even reachable, we do not need to follow it,
            // if under control of controller port
            if ((not successor->finalReachable)
                    && (abstractedSync[transitionname] != "")) {
                //keepTransition = false;
                labelBlacklist[abstractedSync[transitionname]] = true;
                //status("final unreachable by %s", abstractedSync[transitionname].c_str());
            } else { // otherwise the regular handling


                // if we have no seen the successor so far, process
                if (not seen[successor->number]) {

                    assert(successor->number != 0);
                    //Do the same for incoming messages ... we do not need states we never wanna reach



                    // first check, if we would violate mb (sending to much)
                    // copy message count, if not receiving or sync transition
                    if (remainingIn[transitionname] == ""
                            && remainingSync[transitionname] == "") {
                        successor->messages = this->messages;
                        successor->lastMessagesReset = this->lastMessagesReset;
                    } else {
                        successor->messages.clear();
                        successor->lastMessagesReset = successor->number;
                    }
                    // if sending transition, increase count
                    if (remainingOut[transitionname] != "") {
//                        status("checking for message bound of label %s: %d", remainingOut[transitionname].c_str(), successor->messages[remainingOut[transitionname]]);
                          successor->messages[remainingOut[transitionname]] += 1;
                    }
                    // check for messagebound

                    if (remainingOut[transitionname] == "" ||
                            successor->messages[remainingOut[transitionname]]  <= messageBound) {
                        // messeage bound is okay, so go on
                        successor->prune(seen, active, count);

                    } else {
                        // message bound is violate, so from successor final state
                        // is NOT reachable
                        // as this is an action of the engine we can decide to
                        // remove the transition :)
                        // keepTransition = false;
                        labelBlacklist[remainingOut[transitionname]] = true;
                        status("Blacklisting label %s because of seqmbv (%d)", remainingOut[transitionname].c_str(), successor->messages[remainingOut[transitionname]]);
                    }
                } else {
                    // check for message bound violations
                    // is it a circle?
                    if (successor->lastMessagesReset
                            == this->lastMessagesReset) {
                        if (active[successor->number]) {
                            status("found circle from %d to %d (%s) ... active : %d", this->index, successor->index, remainingOut[transitionname].c_str(), active[successor->number]);
                            // are all values greater of Equal
                            std::map<std::string, unsigned int> oldMessages = successor->messages;
                            std::map<std::string, unsigned int> newMessages = this->messages;

                            if (remainingOut[transitionname] != "") {
        //                        status("checking for message bound of label %s: %d", remainingOut[transitionname].c_str(), successor->messages[remainingOut[transitionname]]);
                                  newMessages[remainingOut[transitionname]] += 1;
                            }

                            bool allGEq = true;
                            bool oneGreater = false;
                            std::string mess;
                            for (std::map<std::string, unsigned int>::const_iterator message =
                                        newMessages.begin();
                                    message != newMessages.end();
                                    ++message) {
                                if (message->first != "") {
                                    status("message %s: %d vs %d", message->first.c_str(), message->second, oldMessages[message->first]);
                                    // if there is a difference, then its bad!
                                    if (oldMessages[message->first] > message->second) {
                                        allGEq = false;
                                        break;
                                    } else
                                    if (message->second > oldMessages[message->first]) {
                                        oneGreater =true;
                                        mess = message->first;
                                    }
                                }

                            }
                            if ((allGEq && oneGreater) || (remainingOut[transitionname] != "" && newMessages[remainingOut[transitionname]] > messageBound )) {
                                if (abstractedSync[transitionname] != "" || remainingOut[transitionname] != "") {
                                    labelBlacklist[abstractedSync[transitionname]] = true;
                                    status(
                                            "pruning from state %d to %d (cause label %s causes mbv on circle, because message %s is created)",
                                            this->index,
                                            successor->index,
                                            abstractedSync[transition->getValue()->getFirst()].c_str(),
                                            mess.c_str());
                                }
                                else {
                                    tauMBV=true;
                                    status("tauMBV");
                                }
                            }
                            allGEq = true;
                            oneGreater = false;
                            for (std::map<std::string, unsigned int>::const_iterator message =
                                        oldMessages.begin();
                                    message != oldMessages.end();
                                    ++message) {
                                if (message->first != "") {
                                    status("message %s: %d vs %d", message->first.c_str(), message->second, newMessages[message->first]);
                                    // if there is a difference, then its bad!
                                    if (newMessages[message->first] < message->second) {
                                        allGEq = false;
                                        break;
                                    } else
                                    if (message->second < newMessages[message->first]) {
                                        oneGreater =true;
                                        mess = message->first;
                                    }
                                }
                            }
                            if (allGEq && oneGreater) {
                                if (abstractedSync[transitionname] != "" || remainingOut[transitionname] != "") {
                                    labelBlacklist[abstractedSync[transitionname]] = true;
                                    status(
                                            "pruning from state %d to %d (cause label %s causes mbv2 on circle, because message %s is created)",
                                            this->number,
                                            successor->number,
                                            abstractedSync[transition->getValue()->getFirst()].c_str(),
                                            mess.c_str());
                                }
                                else {
                                    tauMBV=true;
                                    status("tauMBV");
                                }
                            }
                        } else if (this->lowlink == successor->lowlink){
                            /**/
                            status("found non-circle from %d to %d (%s) ... active : %d", this->index, successor->index, remainingOut[transitionname].c_str(), active[successor->number]);
                            // not a circle ... non-deterministic decision?
                            std::map<std::string, unsigned int> oldMessages = successor->messages;
                            std::map<std::string, unsigned int> newMessages = this->messages;

                            if (remainingOut[transitionname] != "") {
        //                        status("checking for message bound of label %s: %d", remainingOut[transitionname].c_str(), successor->messages[remainingOut[transitionname]]);
                                  newMessages[remainingOut[transitionname]] += 1;
                            }

                            bool allGEq = true;
                            bool oneGreater = false;
                            std::string mess;
                            for (std::map<std::string, unsigned int>::const_iterator message =
                                        newMessages.begin();
                                    message != newMessages.end();
                                    ++message) {
                                if (message->first != "") {
                                    status("message %s: %d vs %d", message->first.c_str(), message->second, oldMessages[message->first]);
                                    // if there is a difference, then its bad!
                                    if (oldMessages[message->first] > message->second) {
                                        allGEq = false;
                                        break;
                                    } else
                                    if (message->second > oldMessages[message->first]) {
                                        oneGreater =true;
                                        mess = message->first;
                                    }
                                }

                            }
                            if ((allGEq && oneGreater) || (remainingOut[transitionname] != "" && newMessages[remainingOut[transitionname]] > messageBound )) {
                                if (abstractedSync[transitionname] != "" || remainingOut[transitionname] != "") {
                                    labelBlacklist[abstractedSync[transitionname]] = true;
                                    status(
                                            "pruning from state %d to %d (cause label %s causes mbv on circle, because message %s is created)",
                                            this->index,
                                            successor->index,
                                            abstractedSync[transition->getValue()->getFirst()].c_str(),
                                            mess.c_str());
                                }
                                else {
                                    tauMBV=true;
                                    status("tauMBV");
                                }
                            }
                            allGEq = true;
                            oneGreater = false;
                            for (std::map<std::string, unsigned int>::const_iterator message =
                                        oldMessages.begin();
                                    message != oldMessages.end();
                                    ++message) {
                                if (message->first != "") {
                                    status("message %s: %d vs %d", message->first.c_str(), message->second, newMessages[message->first]);
                                    // if there is a difference, then its bad!
                                    if (newMessages[message->first] > message->second) {
                                        allGEq = false;
                                        break;
                                    } else
                                    if (message->second > newMessages[message->first]) {
                                        oneGreater =true;
                                        mess = message->first;
                                    }
                                }
                            }
                            if (allGEq && oneGreater) {
                                if (abstractedSync[transitionname] != "" || remainingOut[transitionname] != "") {
                                    labelBlacklist[abstractedSync[transitionname]] = true;
                                    status(
                                            "pruning from state %d to %d (cause label %s causes mbv2 on circle, because message %s is created)",
                                            this->number,
                                            successor->number,
                                            abstractedSync[transition->getValue()->getFirst()].c_str(),
                                            mess.c_str());
                                }
                                else {
                                    tauMBV=true;
                                    status("tauMBV");
                                }
                            }
                            /**/
                        }
                    }
                }

                // if we still keep the transition, now check if violate enforceability
                //evaluate transition

                /*
                if (keepTransition && abstractedSync[transitionname] != "") {
                    // if an SCC node, at least final must be reachable
                    if (this->lowlink == successor->lowlink && not successor->finalReachable) {
                        keepTransition = false;
                        labelBlacklist[abstractedSync[transitionname]] = true;
                    // status("Blacklisting label %s", abstractedSync[transitionname].c_str());
                    } else if (this->lowlink != successor->lowlink && not successor->finalEnforceable) {
                        keepTransition = false;
                        labelBlacklist[abstractedSync[transitionname]] = true;

                    }
                }
                */

                if (abstractedSync[transitionname] != "") {
                    if (not this->isSCCNode() && not successor->finalEnforceable) {
                        labelBlacklist[abstractedSync[transitionname]] = true;
                        //status("final not enforceable by %s", abstractedSync[transitionname].c_str());
                    }
                    if (this->isSCCNode() && this->lowlink == successor->lowlink && not successor->finalReachable) {
                        labelBlacklist[abstractedSync[transitionname]] = true;
                        status("final not reachable in SCC by %s", abstractedSync[transitionname].c_str());
                    }
                    if (this->isSCCNode() && this->lowlink != successor->lowlink && not successor->finalEnforceable) {
                        labelBlacklist[abstractedSync[transitionname]] = true;
                        status("final not reachable by leaving SCC by %s", abstractedSync[transitionname].c_str());
                    }
                }


            }
        }
    }
    // check newTransitions for violation of blacklist (all transitions with
    // the same label have to be removed
    ListOfPairs::List_ptr filteredNewTransitions;
    unsigned int blaCounter = 0;
    for (ListOfPairs::List_ptr transition = this->transitions; transition.get() != 0; transition = transition->getNext()) {
        std::string transitionname = transition->getValue()->getFirst();
        std::string syncName = abstractedSync[transitionname];
        std::string outName = remainingOut[transitionname];
        if ((syncName != "" && labelBlacklist[syncName]) || (outName != "" && labelBlacklist[outName])) {
            /* ignore this transition */
            //status("Ignoring transition %s because of blacklisted label(%s, %s)", transitionname.c_str(), syncName.c_str(), outName.c_str());
            ++count;
        } else {
            /* add it to the list of filtered transitions */
            if (filteredNewTransitions.get() == 0) {
                filteredNewTransitions = ListOfPairs::List_ptr(new ListOfPairs(transition->getValue()));
            }
            else {
                filteredNewTransitions->push_back(transition->getValue());
            }
            ++blaCounter;
        }

    }
    bool vfinalReachable = false || isFinal;
    bool vfinalEnforceable = false || isFinal;

    //expect to be true, may be changed by transitions, if atleast 1
    if (filteredNewTransitions.get() != 0) {
        vfinalEnforceable = true && not tauMBV;
    }

    for (ListOfPairs::List_ptr transition = filteredNewTransitions; transition.get() != 0; transition = transition->getNext()) {
        shared_ptr<State> successor = stateSpace[transition->getValue()->getSecond()];
        std::string transitionname = transition->getValue()->getFirst();
        // if not SCC node
        vfinalReachable = vfinalReachable || successor->finalReachable;
        if (not this->isSCCNode()) {
                if(remainingIn[transitionname] == "" && remainingSync[transitionname] == "") {
                    vfinalEnforceable = vfinalEnforceable && successor->finalEnforceable;
                } else {
                    vfinalEnforceable = vfinalEnforceable && successor->finalEnforceable;
                    // vfinalEnforceable = vfinalEnforceable && (successor->isSCCNode()?successor->finalReachable:successor->finalEnforceable);
                }
        } else {
            // transitions leaving the SCC must enforce final
            if (this->lowlink != successor->lowlink) {
                vfinalEnforceable = vfinalEnforceable && successor->finalEnforceable;
            } else {
                // else reachability is enough
                vfinalEnforceable = vfinalEnforceable && successor->finalReachable;
            }
        }

        // finalInAllSuccessors = finalInAllSuccessors && successor->finalReachable;
        // add only, if successor only leads to a final state

    }
    assert(not vfinalEnforceable || vfinalReachable);
    status("state %d has %d transitions, is final %d, final is reachable/enforceable %d/%d, belongs to an SCC %d", this->index, blaCounter, this->isFinal, vfinalReachable, vfinalEnforceable, this->isSCCNode());
    this->finalReachable = vfinalReachable || this->isFinal; // || finalEnforceable;
    this->finalEnforceable = vfinalEnforceable || this->isFinal;
    transitions = filteredNewTransitions;
    active[number] = false;

}

bool State::transitionsIsTau(std::string transition) {
    return (abstractedIn[transition] == "" && abstractedOut[transition] == "" && abstractedSync[transition] == "" && remainingIn[transition] == "" && remainingOut[transition] == "" && remainingSync[transition] == "");
}

bool State::isSCCNode() {
    if(this->lowlink != this->index) {
//        status("found SCC %d", this->number);
        return true;
    } else {
        if (this->sccmember.get() == 0) {
            return false;
        }
        assert(this->sccmember.get() != 0);
//        status("found maybe SCC %d", this->sccmember->length());
        return this->sccmember->length() > 1;
    }
}
