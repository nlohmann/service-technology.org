#include "serviceAutomaton.h"
#include "verbose.h"


/// "mapping" from event id to its label
std::vector<std::string> ServiceAutomaton::labels;

/// mapping from label to its ID
std::map<std::string, unsigned int> ServiceAutomaton::labelIDs;

/// ID of first specification receive label
unsigned int ServiceAutomaton::firstReceiveID = 1; // sic! (0 is the ID for TAU)
/// ID of last specification receive label
unsigned int ServiceAutomaton::lastReceiveID = 0;
/// ID of first specification send label
unsigned int ServiceAutomaton::firstSendID = 0;
/// ID of last specification send label
unsigned int ServiceAutomaton::lastSendID = 0;


/*!
 * \brief constructor
 *
 * \note TAU will not explicitly written in labelIDs since the STL map
 *       will return 0 for every unknown label; i.e. all events not being
 *       explicitly marked as sending or receiving events are rendered to TAU.
 *
 * \todo May not call abort() on interface mismatch in order to process further test cases.
 */
ServiceAutomaton::ServiceAutomaton(std::vector<std::string>& sending, std::vector<std::string>& receiving, bool isSpecification):
    initialState(0) {
    if (isSpecification) {
        // reset counters
        firstReceiveID = 1;
        firstSendID = (lastReceiveID = receiving.size()) + 1;
        lastSendID = lastReceiveID + sending.size();

        // reset tables
        labels.clear();
        labelIDs.clear();

        // write labels
        labels.push_back("TAU");
        for (unsigned int i = 0; i < receiving.size(); ++i) {
            labelIDs[receiving[i]] = labels.size();
            labels.push_back(receiving[i]);
        }
        for (unsigned int i = 0; i < sending.size(); ++i) {
            labelIDs[sending[i]] = labels.size();
            labels.push_back(sending[i]);
        }
    } else {
        // check interface
        if ((firstReceiveID + sending.size() - 1 != lastReceiveID) ||
                (firstSendID + receiving.size() - 1 != lastSendID)) { // see for-loops below
            abort(0, "number of corresponding interface ports mismatches");
        }
        for (unsigned int i = 0; i < sending.size(); ++i) {
            // we are sending, so specification must be receiving
            if ((labelIDs[sending[i]] < firstReceiveID) || (labelIDs[sending[i]] > lastReceiveID)) {
                abort(0, "no corresponding receive port found for sending port '%s'", sending[i].c_str());
            }
        }
        for (unsigned int i = 0; i < receiving.size(); ++i) {
            // we are receiving, so specification must be sending
            if (labelIDs[receiving[i]] < firstSendID) {
                abort(0, "no corresponding sending port found for receive port '%s'", receiving[i].c_str());
            }
        }
    }
}

/*!
 * \brief mark the initial state
 */
void ServiceAutomaton::setInitialState(unsigned int state) {
    initialState = state;
}

/*!
 * \brief mark a final state
 */
void ServiceAutomaton::setFinalState(unsigned int state) {
    finalStates.insert(state);
}

/*!
 * \brief mark a state without successor
 */
void ServiceAutomaton::setNoSuccessorState(unsigned int state) {
    noSuccessorStates.insert(state);
}

/*!
 * \brief add a successor
 */
void ServiceAutomaton::addSuccessor(unsigned int source, std::string& event, unsigned int target) {
    stateSpace[source][labelIDs[event]].push_back(target);
}
