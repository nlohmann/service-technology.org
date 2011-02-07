/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009, 2010  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>.
 \*****************************************************************************/

#include "config.h"

#if defined(HAVE_LIBCONFIG__) and HAVE_LIBCONFIG__ == 1

#include <utility> // just for make_pair
#include <algorithm> // for sort
#include <list>
#include <map>

#include "libconfig.h++"
#include "pnapi/pnapi.h"
#include "verbose.h"
#include "helper.h"
#include "macros.h"
#include "Output.h"

#include "diagnosis.h"
bool Diagnosis::DiagnosisInformation::operator==(
        const DiagnosisInformation & d1) const {
    if (d1.type != type) {
        return false;
    } else if (d1.pendingMessages != pendingMessages) {
        return false;
    } else if (d1.requiredMessages != requiredMessages) {
        return false;
    } else if (d1.previouslyAppliedRules != previouslyAppliedRules) {
        return false;
    } else if (d1.netsInFinalState != netsInFinalState) {
        return false;
    }
    return true;
}

bool Diagnosis::DiagnosisInformation::operator<=(
        const DiagnosisInformation & d2) const {
    //                    if (type != "MA") {
    //                        return *this == d2;
    //                    }
    if (type != d2.type or netsInFinalState != d2.netsInFinalState) {
        return false;
    }
    if (std::includes(d2.requiredMessages.begin(),
            d2.requiredMessages.end(), requiredMessages.begin(),
            requiredMessages.end())) {
        if (std::includes(d2.pendingMessages.begin(),
                d2.pendingMessages.end(), pendingMessages.begin(),
                pendingMessages.end())) {
            if (std::includes(d2.previouslyAppliedRules.begin(),
                    d2.previouslyAppliedRules.end(),
                    previouslyAppliedRules.begin(),
                    previouslyAppliedRules.end())) {
                return true;
            }
        }
    }
    return false;
}

bool Diagnosis::DiagnosisInformation::operator()(
        const DiagnosisInformation & d1, const DiagnosisInformation & d2) const {

    status("Comparing %s and %s", d1.getLive().c_str(),
            d2.getLive().c_str());
    // d1 is surely less than d2 if type is lexicographical less
    if (d1.type < d2.type) {
        status("%s < %s", d1.type.c_str(), d2.type.c_str());
        return true;
    } else if (d1.type == d2.type) {
        // if type is the same, it depends on the size of pending messages
        if (d1.pendingMessages.size() < d2.pendingMessages.size()
                or (d1.pendingMessages.size() == d2.pendingMessages.size()
                        and d1.pendingMessages < d2.pendingMessages)) {
            status("less pending");
            return true;
        }
        if (d1.pendingMessages == d2.pendingMessages) {
            if (d1.requiredMessages.size() < d2.requiredMessages.size()
                    or (d1.requiredMessages.size()
                            == d2.requiredMessages.size()
                            and d1.requiredMessages < d2.requiredMessages)) {
                status("less required");
                return true;
            }
            if (d1.requiredMessages == d2.requiredMessages) {
                if (d1.previouslyAppliedRules.size()
                        < d2.previouslyAppliedRules.size()
                        or (d1.previouslyAppliedRules.size()
                                == d2.previouslyAppliedRules.size()
                                and d1.previouslyAppliedRules
                                        < d2.previouslyAppliedRules)) {
                    status("less rules");
                    return true;
                }
                if (d1.previouslyAppliedRules == d2.previouslyAppliedRules) {
                    if (d1.netsInFinalState.size()
                            < d2.netsInFinalState.size()
                            or (d1.netsInFinalState.size()
                                    == d2.netsInFinalState.size()
                                    and d1.netsInFinalState
                                            < d2.netsInFinalState)) {
                        status("less final");
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/*
 bool Diagnosis::DiagnosisInformation::operator()(
 const DiagnosisInformation & d1, const DiagnosisInformation & d2) const {
 if (d1.type < d2.type) {
 return true;
 if (d1.pendingMessages < d2.pendingMessages) {
 return true;
 }
 if (d1.requiredMessages < d2.requiredMessages) {
 return true;
 }
 if (d1.previouslyAppliedRules < d2.previouslyAppliedRules) {
 return true;
 }
 if (d1.netsInFinalState < d2.netsInFinalState) {
 return true;
 }
 return false;
 }
 }
 */
Diagnosis::Diagnosis(std::string filename, MarkingInformation & pmi,
        unsigned int messageBound) :
    dgraph(new DGraph), mi(pmi), live(args_info.live_arg,
            "Info file for live"), messageBound(messageBound), superfluous(
            false) //  messageBound
{

    // variable to store information from diagnosis file in LibConfig format
    libconfig::Config diagInfo;
    // try to open file or print parse error
    try {
        diagInfo.readFile(filename.c_str());
    } catch (libconfig::ParseException pex) {
        message("Parse error `%s' in diagnosis file `%s' in line %d.",
                pex.getError(), pex.getFile(), pex.getLine());
    }

    // look up result and criterion of diagnosis
    bool result = false;
    std::string criterion;

    diagInfo.lookupValue("controllability.result", result);
    diagInfo.lookupValue("controllability.criterion", criterion);

    // if result is true, then nets are adaptable and we do not need to do anything
    if (result) {
        superfluous = true;
        message("The nets are adaptable, please run in non-diagnosis mode.");
        return;
    }

    // look up number of inner markings and nodes
    int innermarkings = 0;
    int nodes = 0;

    diagInfo.lookupValue("statistics.inner_markings", innermarkings);
    diagInfo.lookupValue("statistics.nodes", nodes);

    // setting the initial node
    int initialNode;
    diagInfo.lookupValue("diagnosis.initial_states.[0]", initialNode);
    dgraph->initialNode = dgraph->getIDForName(initialNode);

    // parsing all nodes
    libconfig::Setting & states = diagInfo.lookup("diagnosis.states");

    for (int i = 0; i < states.getLength(); ++i) {
        DNode::Initializer init;

        // id of a node
        int id = 0;
        states[i].lookupValue("id", id);

        init.id = dgraph->getIDForName(id);
        // does the node have deadlocks of livelocks?
        states[i].lookupValue("internalDeadlock", init.has_deadlock);
        states[i].lookupValue("internalLivelock", init.has_livelock);

        // status("Node %d has deadlock: %s", id, (init.has_deadlock?"yes":"no"));

        // get all successors of the node
        try {
            // iterate over all successors
            for (int j = 0; j < states[i]["successors"].getLength(); ++j) {
                // get label of transition
                std::string labelName = states[i]["successors"][j][0];
                unsigned int lid = dgraph->getIDForLabel(labelName);
                // get successor ID
                int nodeId = states[i]["successors"][j][1];
                unsigned int nid = dgraph->getIDForName(nodeId);

                //status("Successor: label = %d, node = %d", lid, nid);
                // remember the successor for the current node
                init.successors.push_back(std::make_pair(lid, nid));

                // node nid has one more predecessor
                dgraph->noOfPredecessors[nid] += 1;
            }
        } catch (libconfig::SettingNotFoundException ex) {
            status("Exception: Path = %s, what = %s", ex.getPath(),
                    ex.what());
            message("Malformed diagnosis file?");
            exit(1);
        }

        // add node
        DNode * node = new DNode(init);
        dgraph->nodeMap[init.id] = node;
        //if ( init.has_deadlock )
        // look for all deadlocks inside the current node
        try {
            for (int j = 0; j < states[i]["internalDeadlocks"].getLength(); ++j) {
                node->deadlockMarkings.push_back(mi.getIDForMarking(
                        states[i]["internalDeadlocks"][j]));
            }
            /*
             int dlID = 0;
             states[i].lookupValue("deadlockMarking", dlID);
             status("Deadlock marking %d = %d", dlID, mi.getIDForMarking(dlID));
             node->deadlockMarkings.push_back(mi.getIDForMarking(dlID));
             */

            dgraph->deadlockNodes.push_back(node);
        } catch (libconfig::SettingNotFoundException ex) {
            //           status("Exception: Path = %s, what = %s", ex.getPath(), ex.what());
        }

        // look for all livelocks inside the current node
        try {
            for (int j = 0; j < states[i]["internalLivelocks"].getLength(); ++j) {
                for (int k = 0; k
                        < states[i]["internalLivelocks"][j].getLength(); ++k) {
                    node->livelockMarkings.push_back(mi.getIDForMarking(
                            states[i]["internalLivelocks"][j][k]));
                }
            }
            /*
             int dlID = 0;
             states[i].lookupValue("deadlockMarking", dlID);
             status("Deadlock marking %d = %d", dlID, mi.getIDForMarking(dlID));
             node->deadlockMarkings.push_back(mi.getIDForMarking(dlID));
             */

            dgraph->livelockNodes.push_back(node);
        } catch (libconfig::SettingNotFoundException ex) {
            // status("Exception: Path = %s, what = %s", ex.getPath(), ex.what());
        }

        // look for all unresolvable waitstates inside the current node
        try {
            for (int j = 0; j
                    < states[i]["unresolvableWaitstates"].getLength(); ++j) {
                node->waitstateMarkings.push_back(mi.getIDForMarking(
                        states[i]["unresolvableWaitstates"][j]));
            }
            /*
             int dlID = 0;
             states[i].lookupValue("deadlockMarking", dlID);
             status("Deadlock marking %d = %d", dlID, mi.getIDForMarking(dlID));
             node->deadlockMarkings.push_back(mi.getIDForMarking(dlID));
             */

            // dgraph->deadlockNodes.push_back(node);
        } catch (libconfig::SettingNotFoundException ex) {
            //           status("Exception: Path = %s, what = %s", ex.getPath(), ex.what());
        }

        dgraph->nodes.push_back(node);

    }

    dgraph->collectRules();

    // diagInfo.write(stderr);
}

Diagnosis::~Diagnosis() {
    delete dgraph;
    //for (std::set< DiagnosisInformation >::iterator iter = diagnosisInformation.begin(); iter != diagnosisInformation.end(); ++iter)
    {
        //delete(&(*iter));
    }
    diagnosisInformation.clear();
}

void Diagnosis::readMPPs(std::vector<std::string> & resultfiles) {

    for (int i = 0; i < resultfiles.size(); ++i) {
        DGraph mpp;
        std::map<std::string, bool> inInterface;
        try {
            libconfig::Config diagInfo;
            // try to open file or print parse error
            try {
                diagInfo.readFile(resultfiles[i].c_str());
            } catch (libconfig::ParseException pex) {
                message(
                        "Parse error `%s' in diagnosis file `%s' in line %d.",
                        pex.getError(), pex.getFile(), pex.getLine());
            }

            // look up number of inner markings and nodes
            int innermarkings = 0;
            int nodes = 0;

            diagInfo.lookupValue("statistics.inner_markings", innermarkings);
            diagInfo.lookupValue("statistics.nodes", nodes);

            status("%s has %d inner markings and %d nodes.",
                    resultfiles[i].c_str(), innermarkings, nodes);

            // setting the initial node
            int initialNode;
            diagInfo.lookupValue("sa.initial_states.[0]", initialNode);
            mpp.initialNode = mpp.getIDForName(initialNode);

            // getting all interface names
            libconfig::Setting & ch_in = diagInfo.lookup(
                    "sa.channels_input");
            for (int i = 0; i < ch_in.getLength(); ++i) {
                inInterface[ch_in[i]] = true;
            }
            libconfig::Setting & ch_out = diagInfo.lookup(
                    "sa.channels_output");
            for (int i = 0; i < ch_out.getLength(); ++i) {
                inInterface[ch_out[i]] = true;
            }
            libconfig::Setting & ch_sync = diagInfo.lookup(
                    "sa.channels_synchronous");
            for (int i = 0; i < ch_sync.getLength(); ++i) {
                inInterface[ch_sync[i]] = true;
            }

            // parsing all nodes
            libconfig::Setting & states = diagInfo.lookup("sa.states");

            for (int i = 0; i < states.getLength(); ++i) {
                DNode::Initializer init;

                // id of a node
                int id = 0;
                states[i].lookupValue("id", id);

                init.id = mpp.getIDForName(id);
                // does the node have deadlocks of livelocks?
                // states[i].lookupValue("internalDeadlock", init.has_deadlock);
                // states[i].lookupValue("internalLivelock", init.has_livelock);

                // status("Node %d has deadlock: %s", id, (init.has_deadlock?"yes":"no"));

                // get all successors of the node
                try {
                    // iterate over all successors
                    for (int j = 0; j < states[i]["successors"].getLength(); ++j) {
                        // get label of transition
                        std::string labelName =
                                states[i]["successors"][j][0];
                        unsigned int lid = mpp.getIDForLabel(labelName);
                        // get successor ID
                        int nodeId = states[i]["successors"][j][1];
                        unsigned int nid = mpp.getIDForName(nodeId);

                        //status("Successor: label = %d, node = %d", lid, nid);
                        // remember the successor for the current node
                        init.successors.push_back(std::make_pair(lid, nid));

                        // node nid has one more predecessor
                        mpp.noOfPredecessors[nid] += 1;
                    }
                } catch (libconfig::SettingNotFoundException ex) {
                    status("Exception: Path = %s, what = %s", ex.getPath(),
                            ex.what());
                    message("Malformed diagnosis file?");
                    exit(1);
                }

                // add node
                DNode * node = new DNode(init);
                mpp.nodeMap[init.id] = node;

                mpp.nodes.push_back(node);

            }
        } catch (libconfig::SettingNotFoundException ex) {
            status("Exception: Path = %s, what = %s", ex.getPath(),
                    ex.what());
            exit(1);
        }

        // simulation relation to Diagnosis Information
        DNode * diNode = dgraph->nodeMap[dgraph->initialNode];
        DNode * mppNode = mpp.nodeMap[mpp.initialNode];
        std::list<std::pair<DNode*, DNode*> > stack;
        stack.push_front(std::make_pair(diNode, mppNode));
        std::map<std::pair<DNode*, DNode*>, bool> nodePairSeen;

        do {
            nodePairSeen[stack.front()] = true;
            diNode = stack.front().first;
            mppNode = stack.front().second;
            stack.pop_front();
            status("Nodes %d and %d are related", dgraph->getNameForID(
                    diNode->getID()), mpp.getNameForID(mppNode->getID()));
            std::map<unsigned int, bool> hasSuccessor;
            for (int x = 0; x < diNode->successors.size(); ++x) {
                for (int y = 0; y < mppNode->successors.size(); ++y) {
                    unsigned int diLid = diNode->successors[x].first;
                    unsigned int diNid = diNode->successors[x].second;
                    unsigned int mppLid = mppNode->successors[y].first;
                    unsigned int mppNid = mppNode->successors[y].second;

                    hasSuccessor[mppLid] = hasSuccessor[mppLid] && true;

                    std::string diLabel = dgraph->getLabelForID(diLid);
                    std::string mppLabel = mpp.getLabelForID(mppLid);

                    //                    status("May be related edges %s and %s?", diLabel.c_str(), mppLabel.c_str());

                    std::string::size_type k = diLabel.find(
                            "sync_t_receive_");
                    if (k == 0 && k != std::string::npos) {
                        diLabel = diLabel.substr(std::string(
                                "sync_t_receive_").size());
                    }
                    std::string::size_type l = diLabel.find("sync_t_send_");
                    if (l == 0 && l != std::string::npos) {
                        diLabel = diLabel.substr(
                                std::string("sync_t_send_").size());
                    }

                    status(" ... stripped %s and %s?", diLabel.c_str(),
                            mppLabel.c_str());
                    std::pair<DNode*, DNode*> newNodePair;
                    bool putOnStack = false;
                    if (diLabel == mppLabel) {
                        newNodePair
                                = std::make_pair(dgraph->nodeMap[diNid],
                                        mpp.nodeMap[mppNid]);
                        putOnStack = true;
                        hasSuccessor[mppLid] = true;
                    } else if (diLabel.find("sync_rule_") == 0
                            || not inInterface[diLabel]) {
                        putOnStack = true;
                        newNodePair = std::make_pair(
                                dgraph->nodeMap[diNid], mppNode);
                    }
                    if (putOnStack and not nodePairSeen[newNodePair]) {
                        stack.push_front(newNodePair);
                        status("Put next node pair on stack");
                    }
                }
            }
            // check whether there was an unused label (in MPP but not in Diagnosis Information)
            for (std::map<unsigned int, bool>::iterator iter =
                    hasSuccessor.begin(); iter != hasSuccessor.end(); ++iter) {
                if (not iter->second) {
                    status("--> %s has no successor, %d",
                            mpp.getLabelForID(iter->first).c_str(),
                            iter->second);
                    diNode->missedAlternatives.insert(mpp.getLabelForID(
                            iter->first));
                    dgraph->alternativeNodes.insert(diNode);
                    // look at all the waitstates
                    // for (int i = 0; i < diNode->waitstateMarkings.size(); ++i) {
                    //    Marking & m = *(mi.markings[node->waitstateMarkings[i]]);
                    //    m.getRequiredMessages()
                    //}
                }
            }
        } while (stack.size() > 0);

    }
}

void Diagnosis::evaluateDeadlocks(std::vector<pnapi::PetriNet *> & nets,
        pnapi::PetriNet & engine) {
    FUNCIN
    if (superfluous) {
        FUNCOUT
        return;
    }

    // prefix of the engine
    std::string prefix = "engine.";

    // iterate over all nodes containing deadlocks
    for (unsigned int i = 0; i < dgraph->deadlockNodes.size(); ++i) {
        DNode * node = dgraph->deadlockNodes[i];
        {

            message("Deadlock %d (node %d)", i + 1, node->getID());
            for (unsigned int j = 0; j < node->deadlockMarkings.size(); ++j) {
                DiagnosisInformation * dI = new DiagnosisInformation;
                dI->type = "DL";

                Marking & m = *(mi.markings[node->deadlockMarkings[j]]);
                dI->pendingMessages = m.getPendingMessages(engine, prefix,
                        messageBound);
                sort(dI->pendingMessages.begin(), dI->pendingMessages.end());

                for (unsigned int net = 0; net < nets.size(); ++net) {
                    std::string prefix = "net" + toString(net + 1) + ".";
                    std::vector<std::string> required =
                            m.getRequiredMessages(*(nets[net]), prefix);
                    std::string isFinal = required.back();
                    required.pop_back();
                    dI->requiredMessages.insert(dI->requiredMessages.end(),
                            required.begin(), required.end());

                    if ("yes" == isFinal) {
                        dI->netsInFinalState.insert(net + 1);
                    }
                }
                sort(dI->requiredMessages.begin(),
                        dI->requiredMessages.end());

                dI->previouslyAppliedRules = node->rulesApplied;

                diagnosisInformation.insert(*dI);
            }
        }
    }

    FUNCOUT
}

void Diagnosis::evaluateLivelocks(std::vector<pnapi::PetriNet *> & nets,
        pnapi::PetriNet & engine) {
    FUNCIN
    if (superfluous) {
        FUNCOUT
        return;
    }

    std::string prefix = "engine.";

    // iterate over all nodes containing livelocks
    for (unsigned int i = 0; i < dgraph->livelockNodes.size(); ++i) {
        DNode * node = dgraph->livelockNodes[i];
        {

            message("Livelock %d (node %d, %d)", i + 1, node->getID(),
                    dgraph->getNameForID(node->getID()));
            for (unsigned int j = 0; j < node->livelockMarkings.size(); ++j) {
                DiagnosisInformation * dI = new DiagnosisInformation;
                dI->type = "LL";
                Marking & m = *(mi.markings[node->livelockMarkings[j]]);
                dI->pendingMessages = m.getPendingMessages(engine, prefix,
                        messageBound);
                sort(dI->pendingMessages.begin(), dI->pendingMessages.end());

                for (unsigned int net = 0; net < nets.size(); ++net) {
                    std::string prefix = "net" + toString(net + 1) + ".";
                    std::vector<std::string> required =
                            m.getRequiredMessages(*(nets[net]), prefix);
                    std::string isFinal = required.back();
                    required.pop_back();
                    dI->requiredMessages.insert(dI->requiredMessages.end(),
                            required.begin(), required.end());
                    //                    std::cerr << "size of required: " << dI->requiredMessages.size() << std::endl;

                    if ("yes" == isFinal) {
                        dI->netsInFinalState.insert(net + 1);
                    }
                }
                sort(dI->requiredMessages.begin(),
                        dI->requiredMessages.end());

                dI->previouslyAppliedRules = node->rulesApplied;
                if (dI->requiredMessages.size() > 0)
                    diagnosisInformation.insert(*dI);
            }
        }
    }

    FUNCOUT
}

void Diagnosis::evaluateAlternatives(std::vector<pnapi::PetriNet *> & nets,
        pnapi::PetriNet & engine) {
    FUNCIN
    if (superfluous) {
        FUNCOUT
        return;
    }

    // prefix of the engine
    std::string prefix = "engine.";

    // iterate over all nodes containing deadlocks
    for (std::set<DNode *>::iterator i = dgraph->alternativeNodes.begin(); i
            != dgraph->alternativeNodes.end(); ++i) {
        DNode * node = *i;
        if (node->successors.size() > 0) {

            message("Missed Alternative (node %d, %d)", node->getID(),
                    dgraph->getNameForID(node->getID()));
            for (unsigned int j = 0; j < node->waitstateMarkings.size(); ++j) {
                DiagnosisInformation * dI = new DiagnosisInformation;
                dI->type = "MA";

                Marking & m = *(mi.markings[node->waitstateMarkings[j]]);
                dI->pendingMessages = m.getPendingMessages(engine, prefix,
                        messageBound);
                sort(dI->pendingMessages.begin(), dI->pendingMessages.end());

                for (unsigned int net = 0; net < nets.size(); ++net) {
                    std::string prefix = "net" + toString(net + 1) + ".";
                    std::vector<std::string> required =
                            m.getRequiredMessages(*(nets[net]), prefix);
                    std::string isFinal = required.back();
                    required.pop_back();
                    for (int rm = 0; rm < required.size(); ++rm) {
                        if (node->missedAlternatives.find(required[rm])
                                != node->missedAlternatives.end()) {
                            status(" message %s was missed AND required",
                                    required[rm].c_str());
                            dI->requiredMessages.push_back(required[rm]);
                        }
                    }

                    if ("yes" == isFinal) {
                        dI->netsInFinalState.insert(net + 1);
                    }
                }
                sort(dI->requiredMessages.begin(),
                        dI->requiredMessages.end());

                dI->previouslyAppliedRules = node->rulesApplied;

                diagnosisInformation.insert(*dI);
            }
        }
    }

    FUNCOUT
}

void Diagnosis::outputLive() const {

    for (std::set<DiagnosisInformation>::const_iterator di =
            diagnosisInformation.begin(); di != diagnosisInformation.end(); ++di) {
        bool covered = false;
        std::set<DiagnosisInformation>::const_iterator next = di;
        ++next;
        for (; next != diagnosisInformation.end() and not covered; ++next) {
            if (*di <= *next) {
                covered = true;
            }
        }
        if (not covered) {
            live.stream() << di->getLive();
        }
    }
}

std::string Diagnosis::DiagnosisInformation::getLive() const {
    std::string result = type + ";";
    bool first = true;
    for (std::set<unsigned int>::iterator net = netsInFinalState.begin(); net
            != netsInFinalState.end(); ++net) {
        if (first) {
            first = false;
        } else {
            result += ",";
        }
        result += toString(*net);
    }
    result += ";";

    first = true;
    for (std::vector<std::string>::const_iterator message =
            pendingMessages.begin(); message != pendingMessages.end(); ++message) {
        if (first) {
            first = false;
        } else {
            result += ",";
        }
        result += *message;
    }
    result += ";";

    first = true;
    for (std::vector<std::string>::const_iterator message =
            requiredMessages.begin(); message != requiredMessages.end(); ++message) {
        if (first) {
            first = false;
        } else {
            result += ",";
        }
        result += *message;
    }
    result += ";";

    first = true;
    for (std::set<std::string>::iterator rule =
            previouslyAppliedRules.begin(); rule
            != previouslyAppliedRules.end(); ++rule) {
        if (first) {
            first = false;
        } else {
            result += ",";
        }
        result += *rule;
    }
    result += ";";

    result += "\n";

    return result;

}

DGraph::DGraph() :
    nodeId(0), labelId(0) {
    FUNCIN

    FUNCOUT
}

DGraph::~DGraph() {
    FUNCIN
    for (unsigned int i = 0; i < nodes.size(); ++i) {
        delete nodes[i];
    }
    FUNCOUT
}

unsigned int DGraph::getIDForName(int nameId) {
    FUNCIN
    unsigned int tempId = name2id[nameId];
    if (tempId != 0) {
        return tempId;
    }
    tempId = ++nodeId;

    // status("Node %d got id %d", nameId, tempId);

    name2id[nameId] = tempId;
    id2name[tempId] = nameId;
    FUNCOUT
    return tempId;
}

int DGraph::getNameForID(unsigned int id) {
    FUNCIN
    FUNCOUT
    return id2name[id];
}

unsigned int DGraph::getIDForLabel(std::string & labelName) {
    FUNCIN
    unsigned int tempId = label2id[labelName];
    if (tempId != 0) {
        return tempId;
    }
    tempId = ++labelId;

    // status("Label %s got id %d", labelName.c_str(), tempId);

    label2id[labelName] = tempId;
    id2label[tempId] = labelName;
    FUNCOUT
    return tempId;
}

std::string & DGraph::getLabelForID(unsigned int id) {
    FUNCIN
    FUNCOUT
    return id2label[id];
}

void DGraph::collectRules() {
    std::list<unsigned int> queue;
    std::map<unsigned int, bool> seen;

    queue.push_back(initialNode);

    while (not queue.empty()) {
        unsigned int id = *(queue.begin());
        queue.pop_front();
        DNode * node = nodeMap[id];
        seen[id] = true;

        for (unsigned int s = 0; s < node->successors.size(); ++s) {
            std::string label = getLabelForID(node->successors[s].first);
            DNode * snode = nodeMap[node->successors[s].second];
            unsigned int before = snode->rulesApplied.size();

            if (label.find("sync_rule_") == 0) {
                snode->rulesApplied.insert(label.substr(10));
                // status("adding rule %s to node %d", label.substr(10).c_str(), node->successors[s].second);
            }

            PNAPI_FOREACH(rule, node->rulesApplied) {
                snode->rulesApplied.insert(*rule);
            }

            if (((snode->rulesApplied.size() - before) > 0
                    or not seen[node->successors[s].second])) {
                queue.remove(node->successors[s].second);
                queue.push_back(node->successors[s].second);
            }
        }

    }
}

DNode::DNode(Initializer & init) :
    id(init.id), has_deadlock(init.has_deadlock), has_livelock(
            init.has_livelock), successors(init.successors) {

}

DNode::Initializer::Initializer() :
    id(0), has_deadlock(false), has_livelock(false) {

}

#endif
