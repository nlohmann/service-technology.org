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

    bool result = isFinal || finalReachable;
    seen[number] = true;
    if (transitions.get() != 0) {
        for (ListOfPairs::List_ptr transition = transitions;
                transition.get() != 0; transition = transition->getNext()) {

            if (not seen[transition->getValue()->getSecond()]) {
                bool resultstep = stateSpace[transition->getValue()->getSecond()]->checkFinalReachable(seen, count);
                result = result || resultstep;
            }
            result =
                result
                || stateSpace[transition->getValue()->getSecond()]->finalReachable;
        }
        if (result) {
            ++count;
        }
    }
    finalReachable = result;
    return result;
}

void State::tauFolding() {

    std::map< unsigned int, bool> seen;
    // brauchen wir das überhaupt?
    std::map< unsigned int, bool> statesFolded;
}

ListOfPairs::List_ptr State::taufolding(std::map<unsigned int, bool>& seen, std::map<unsigned int, bool>& statesFolded) {

    // hier einfach ne Schleife, die alles erkennt?
}


void State::calculateSCC() {
    unsigned int index = 0;
    unsigned int lowlink = 0;

    std::map<unsigned int, bool> seen;
    std::list<unsigned int> stack;
    stateSpace[0]->calculateSCC(index, lowlink, seen, stack);
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
        do {
            w = stack.front();
            stack.pop_front();
            if (newSCC.get() == 0) {
                newSCC = List<unsigned int>::List_ptr(
                             new List<unsigned int>(w));
            } else {
                newSCC->push_back(w);
            }
        } while (w != this->index);
        this->sccmember = newSCC;

    } else {
        this->sccmember = List<unsigned int>::List_ptr();
    }

}

void State::prune() {

    unsigned int count = 0;
    unsigned int old_count = 0;
    unsigned int sum = 0;

    do {
        std::map<unsigned int, bool> seen;
        std::map<unsigned int, bool> active;
        old_count = count;
        stateSpace[0]->prune(seen, active, count);
        status("removed %d transitions in iteration", count - old_count);
//        sum += count;
        calculateSCC();
        seen.clear();
    } while (count > old_count);
    status("removed %d transitions in total", count);
}

void State::prune(std::map<unsigned int, bool>& seen
                  , std::map<unsigned int, bool>& active, unsigned int & count) {

    // shared_ptr<State> state = stateSpace[stateNo];

    ListOfPairs::List_ptr newTransitions;
    bool finalReachable = false || isFinal;
    bool finalEnforceable = false || isFinal;

    // now we have seen this state
    seen[number] = true;
    active[number] = true;
    // if it has transitions, process successors
    if (transitions.get() != 0) {
        //expect to be true, may be changed by transitions
        finalEnforceable = true;
        for (ListOfPairs::List_ptr transition = transitions;
                transition.get() != 0; transition = transition->getNext()) {

            // expect to keep the transition
            bool keepTransition = true;

            // this is the successor state
            shared_ptr<State> successor =
                stateSpace[transition->getValue()->getSecond()];
            // and this is the firing transition
            std::string transitionname = transition->getValue()->getFirst();

            // if final state is not even reachable, we do not need to follow it,
            // if under control of controller port
            if (this->finalReachable && (not successor->finalReachable)
                    && (abstractedSync[transition->getValue()->getFirst()] != "")) {
                keepTransition = false;
//                status(
//                        "pruning from state %d to %d (cause label is %s)",
//                        stateNo,
//                        successor->number,
//                        abstractedSync[transition->getValue()->getFirst()].c_str());
            } else { // otherwise the regular handling

                // if we have no seen the successor so far, process
                if (not seen[successor->number]) {


                    //Do the same for incoming messages ... we do not need states we never wanna reach



                    // first check, if we would violate mb
//                    status("Label: %s, In: %s, Out: %s, Sync: %s", transitionname.c_str(), remainingIn[transitionname].c_str(), remainingOut[transitionname].c_str(), remainingSync[transitionname].c_str());
                    // copy message count, if not receiving or sync transition
                    if (remainingIn[transitionname] == ""
                            && remainingSync[transitionname] == "") {
//                        status("It's neither a receiving nor a sync transition for the interface");
                        successor->messages = messages;
                        successor->lastMessagesReset = lastMessagesReset;
                    } else {
                        successor->lastMessagesReset = successor->number;
                    }
                    // if sending transition, increase count
                    if (remainingOut[transitionname] != "") {
//                        status("checking for message bound of label %s: %d", remainingOut[transitionname].c_str(), successor->messages[remainingOut[transitionname]]);
                        ++(successor->messages[remainingOut[transitionname]]);
                    }
                    // check for messagebound
                    if (successor->messages[remainingOut[transitionname]]
                            <= messageBound) {
                        // messeage bound is okay, so go on
                        successor->prune(seen, active, count);

                    } else {
                        // message bound is violate, so from successor final state
                        // is NOT reachable
                        // as this is an action of the engine we can decide to
                        // remove the transition :)
                        keepTransition = false;
//                        status(
//                                "pruning from state %d to %d (cause label %s causes mbv)",
//                                stateNo,
//                                successor->number,
//                                abstractedSync[transitionname].c_str());
                    }
                } else {
                    // check for message bound violations
                    // is it a circle?
                    if (successor->lastMessagesReset
                            == lastMessagesReset) {
                        if (active[successor->number]) {
                            for (std::map<std::string, unsigned int>::const_iterator message =
                                        messages.begin();
                                    message != messages.end();
                                    ++message) {
                                // if there is a difference, then its bad!
                                if (message->second
                                        - successor->messages[message->first]
                                        > 0) {
                                    keepTransition = false;
                                    break;
//                                status(
//                                        "pruning from state %d to %d (cause label %s causes mbv on circle, because message %s is created)",
//                                        stateNo,
//                                        successor->number,
//                                        abstractedSync[transition->getValue()->getFirst()].c_str(),
//                                        message->first.c_str());
                                }
                            }
                        } else {
                            // not a circle ... non-deterministic decision?
                            int smallerCount = 0;
                            for (std::map<std::string, unsigned int>::const_iterator message =
                                        messages.begin();
                                    message != messages.end();
                                    ++message) {
                                // if there is a difference, then its bad!
                                if (message->second < successor->messages[message->first] ) {
                                    ++smallerCount;
                                } else {
                                    if (message->second > successor->messages[message->first] ) {
                                        smallerCount = 0;
                                        break;
                                    }
                                }
                            }
                            if (smallerCount > 0) {
                                keepTransition = false;
                                status(
                                    "pruning from state %d to %d (cause label %s causes mbv on internal decision)",
                                    number,
                                    successor->number,
                                    abstractedSync[transition->getValue()->getFirst()].c_str());
                            }
                            smallerCount = 0;
                            for (std::map<std::string, unsigned int>::const_iterator message =
                                        successor->messages.begin();
                                    message != successor->messages.end();
                                    ++message) {
                                // if there is a difference, then its bad!
                                if (message->second < messages[message->first] ) {
                                    ++smallerCount;
                                } else {
                                    if (message->second > messages[message->first] ) {
                                        smallerCount = 0;
                                        break;
                                    }
                                }
                            }
                            if (smallerCount > 0) {
                                keepTransition = false;
                                status(
                                    "pruning from state %d to %d (cause label %s causes mbv on internal decision)",
                                    number,
                                    successor->number,
                                    abstractedSync[transition->getValue()->getFirst()].c_str());
                            }
                        }
                    }
                }

                // if we still keep the transition, now check if violate enforceability
                //evaluate transition
                if (abstractedSync[transitionname] != ""
                        && not successor->finalEnforceable) {
                    keepTransition = false;
                }

            }
            // if from the successor a final state is still reachable,
            // we keep it
            if (keepTransition) {
                finalReachable = finalReachable || successor->finalReachable;
                if (lowlink == successor->lowlink) {
                    finalEnforceable = finalEnforceable
                                       && successor->finalReachable;
                } else {
                    finalEnforceable = finalEnforceable
                                       && successor->finalEnforceable;
                }
                // finalInAllSuccessors = finalInAllSuccessors && successor->finalReachable;
                // add only, if successor only leads to a final state
                if (newTransitions.get() == 0) {
                    newTransitions = ListOfPairs::List_ptr(
                                         new ListOfPairs(transition->getValue()));
                } else {
                    newTransitions->push_back(transition->getValue());
                }
            } else {
                ++count;
            }
        }
    }
    this->finalReachable = finalReachable || this->isFinal; // || finalEnforceable;
    this->finalEnforceable = finalEnforceable || this->isFinal;
    transitions = newTransitions;
    active[number] = false;

}
