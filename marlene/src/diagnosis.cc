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

Diagnosis::Diagnosis(std::string filename, MarkingInformation & pmi, unsigned int messageBound) : dgraph(new DGraph), mi(pmi), live(args_info.live_arg, "Info file for live"), messageBound(messageBound), superfluous(false) //  messageBound
{

    // variable to store information from diagnosis file in LibConfig format
    libconfig::Config diagInfo;
    // try to open file or print parse error
    try {
        diagInfo.readFile(filename.c_str());
    }
    catch (libconfig::ParseException pex)
    {
        message("Parse error `%s' in diagnosis file `%s' in line %d.", pex.getError(), pex.getFile(), pex.getLine());
    }

    // look up result and criterion of diagnosis
    bool result = false;
    std::string criterion;

    diagInfo.lookupValue("controllability.result", result);
    diagInfo.lookupValue("controllability.criterion", criterion);

    // if result is true, then nets are adaptable and we do not need to do anything
    if (result)
    {
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

    for ( int i = 0; i < states.getLength(); ++i)
    {
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
        try
        {
            // iterate over all successors
            for ( int j = 0; j < states[i]["successors"].getLength(); ++j)
            {
                // get label of transition
                std::string labelName = states[i]["successors"][j][0];
                unsigned int lid = dgraph->getIDForLabel(labelName);
                // get successor ID
                int nodeId = states[i]["successors"][j][1];
                unsigned int nid = dgraph->getIDForName(nodeId);

                //status("Successor: label = %d, node = %d", lid, nid);
                // remember the successor for the current node
                init.successors.push_back(std::make_pair( lid, nid));

                // node nid has one more predecessor
                dgraph->noOfPredecessors[nid] += 1;
            }
        }
        catch (libconfig::SettingNotFoundException ex)
        {
            status("Exception: Path = %s, what = %s", ex.getPath(), ex.what());
            message("Malformed diagnosis file?");
            exit(1);
        }

        // add node
        DNode * node = new DNode(init);
        dgraph->nodeMap[init.id] = node;
        //if ( init.has_deadlock )
        // look for all deadlocks inside the current node
        try
        {
            for(int j = 0; j < states[i]["internalDeadlocks"].getLength(); ++j)
            {
                node->deadlockMarkings.push_back(mi.getIDForMarking(states[i]["internalDeadlocks"][j]));
            }
            /*
            int dlID = 0;
            states[i].lookupValue("deadlockMarking", dlID);
            status("Deadlock marking %d = %d", dlID, mi.getIDForMarking(dlID));
            node->deadlockMarkings.push_back(mi.getIDForMarking(dlID));
            */

            dgraph->deadlockNodes.push_back(node);
        }
        catch (libconfig::SettingNotFoundException ex)
        {
 //           status("Exception: Path = %s, what = %s", ex.getPath(), ex.what());
        }

        // look for all livelocks inside the current node
        try
        {
            for(int j = 0; j < states[i]["internalLivelocks"].getLength(); ++j)
            {
                for(int k = 0; k < states[i]["internalLivelocks"][j].getLength(); ++k)
                {
                    node->livelockMarkings.push_back(mi.getIDForMarking(states[i]["internalLivelocks"][j][k]));
                }
            }
            /*
            int dlID = 0;
            states[i].lookupValue("deadlockMarking", dlID);
            status("Deadlock marking %d = %d", dlID, mi.getIDForMarking(dlID));
            node->deadlockMarkings.push_back(mi.getIDForMarking(dlID));
            */

            dgraph->livelockNodes.push_back(node);
        }
        catch (libconfig::SettingNotFoundException ex)
        {
            // status("Exception: Path = %s, what = %s", ex.getPath(), ex.what());
        }

        dgraph->nodes.push_back(node);


    }

    dgraph->collectRules();

    // diagInfo.write(stderr);
}

Diagnosis::~Diagnosis()
{
    delete dgraph;
    //for (std::set< DiagnosisInformation >::iterator iter = diagnosisInformation.begin(); iter != diagnosisInformation.end(); ++iter)
    {
        //delete(&(*iter));
    }
    diagnosisInformation.clear();
}

void Diagnosis::evaluateDeadlocks(std::vector< pnapi::PetriNet *> & nets, pnapi::PetriNet & engine)
{
    FUNCIN
    if (superfluous)
    {
        FUNCOUT
        return;
    }

    // prefix of the engine
    std::string prefix = "engine.";

    // iterate over all nodes containing deadlocks
    for ( unsigned int i = 0; i < dgraph->deadlockNodes.size(); ++i )
    {
        DNode * node = dgraph->deadlockNodes[i];
        {

            message("Deadlock %d (node %d)", i + 1, node->getID());
            for ( unsigned int j = 0; j < node->deadlockMarkings.size(); ++j)
            {
                DiagnosisInformation * dI = new DiagnosisInformation;
                dI->type = "DL";

                Marking & m = *(mi.markings[node->deadlockMarkings[j]]);
                dI->pendingMessages = m.getPendingMessages(engine, prefix, messageBound);
                sort(dI->pendingMessages.begin(), dI->pendingMessages.end());

                for ( unsigned int net = 0; net < nets.size(); ++net )
                {
                    std::string prefix = "net" + toString(net + 1) + ".";
                    std::vector< std::string > required = m.getRequiredMessages(*(nets[net]), prefix);
                    std::string isFinal = required.back();
                    required.pop_back();
                    dI->requiredMessages.insert(dI->requiredMessages.end(), required.begin(), required.end() );

                    if ("yes" == isFinal)
                    {
                        dI->netsInFinalState.insert(net+1);
                    }
                }
                sort(dI->requiredMessages.begin(), dI->requiredMessages.end());

                dI->previouslyAppliedRules = node->rulesApplied;

                diagnosisInformation.insert(*dI);
            }
        }
    }

    FUNCOUT
}

void Diagnosis::evaluateLivelocks(std::vector< pnapi::PetriNet *> & nets, pnapi::PetriNet & engine)
{
    FUNCIN
    if (superfluous)
    {
        FUNCOUT
        return;
    }

    std::string prefix = "engine.";

    // iterate over all nodes containing livelocks
    for ( unsigned int i = 0; i < dgraph->livelockNodes.size(); ++i )
    {
        DNode * node = dgraph->livelockNodes[i];
        {

            message("Livelock %d (node %d, %d)", i + 1, node->getID(), dgraph->getNameForID(node->getID()));
            for ( unsigned int j = 0; j < node->livelockMarkings.size(); ++j)
            {
                DiagnosisInformation * dI = new DiagnosisInformation;
                dI->type = "LL";
                Marking & m = *(mi.markings[node->livelockMarkings[j]]);
                dI->pendingMessages = m.getPendingMessages(engine, prefix, messageBound);
                sort(dI->pendingMessages.begin(), dI->pendingMessages.end());

                for ( unsigned int net = 0; net < nets.size(); ++net )
                {
                    std::string prefix = "net" + toString(net + 1) + ".";
                    std::vector< std::string > required = m.getRequiredMessages(*(nets[net]), prefix);
                    std::string isFinal = required.back();
                    required.pop_back();
                    dI->requiredMessages.insert(dI->requiredMessages.end(), required.begin(), required.end() );
//                    std::cerr << "size of required: " << dI->requiredMessages.size() << std::endl;

                    if ("yes" == isFinal)
                    {
                        dI->netsInFinalState.insert(net+1);
                    }
                }
                sort(dI->requiredMessages.begin(), dI->requiredMessages.end());

                dI->previouslyAppliedRules = node->rulesApplied;

                diagnosisInformation.insert(*dI);
            }
        }
    }

    FUNCOUT
}

void Diagnosis::outputLive() const
{
    for ( std::set< DiagnosisInformation >::const_iterator di = diagnosisInformation.begin(); di != diagnosisInformation.end(); ++di)
    {
        live.stream() << di->getLive();
    }
}

std::string Diagnosis::DiagnosisInformation::getLive() const
{
    std::string result = type + ";";
    bool first = true;
    for (std::set< unsigned int >::iterator net = netsInFinalState.begin(); net != netsInFinalState.end(); ++net)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            result += ",";
        }
        result += toString(*net);
    }
    result += ";";

    first = true;
    for (std::vector< std::string >::const_iterator message = pendingMessages.begin(); message != pendingMessages.end(); ++message)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            result += ",";
        }
        result += *message;
    }
    result += ";";

    first = true;
    for (std::vector< std::string >::const_iterator message = requiredMessages.begin(); message != requiredMessages.end(); ++message)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            result += ",";
        }
        result += *message;
    }
    result += ";";

    first = true;
    for (std::set< std::string >::iterator rule = previouslyAppliedRules.begin(); rule != previouslyAppliedRules.end(); ++rule)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            result += ",";
        }
        result += *rule;
    }
    result += ";";

    result += "\n";

    return result;

}

DGraph::DGraph() : nodeId(0), labelId(0)
{
    FUNCIN

    FUNCOUT
}

DGraph::~DGraph()
{
    FUNCIN
    for (unsigned int i = 0; i < nodes.size(); ++i)
    {
        delete nodes[i];
    }
    FUNCOUT
}

unsigned int DGraph::getIDForName( int nameId )
{
    FUNCIN
    unsigned int tempId = name2id[nameId];
    if ( tempId != 0 )
    {
        return tempId;
    }
    tempId = ++nodeId;

    // status("Node %d got id %d", nameId, tempId);

    name2id[nameId] = tempId;
    id2name[tempId] = nameId;
    FUNCOUT
    return tempId;
}

int DGraph::getNameForID( unsigned int id )
{
    FUNCIN
    FUNCOUT
    return id2name[id];
}

unsigned int DGraph::getIDForLabel( std::string & labelName )
{
    FUNCIN
    unsigned int tempId = label2id[labelName];
    if ( tempId != 0 )
    {
        return tempId;
    }
    tempId = ++labelId;

    // status("Label %s got id %d", labelName.c_str(), tempId);

    label2id[labelName] = tempId;
    id2label[tempId] = labelName;
    FUNCOUT
    return tempId;
}

std::string & DGraph::getLabelForID( unsigned int id )
{
    FUNCIN
    FUNCOUT
    return id2label[id];
}

void DGraph::collectRules()
{
    std::list< unsigned int > queue;
    std::map< unsigned int, bool > seen;

    queue.push_back(initialNode);

    while ( not queue.empty() )
    {
        unsigned int id = *(queue.begin());
        queue.pop_front();
        DNode * node = nodeMap[id];
        seen[id] = true;

        for ( unsigned int s = 0; s < node->successors.size(); ++s)
        {
            std::string label = getLabelForID( node->successors[s].first );
            DNode * snode = nodeMap[node->successors[s].second];
            unsigned int before = snode->rulesApplied.size();

            if ( label.find("sync_rule_") == 0)
            {
                snode->rulesApplied.insert(label.substr(10));
                // status("adding rule %s to node %d", label.substr(10).c_str(), node->successors[s].second);
            }

            PNAPI_FOREACH(rule, node->rulesApplied)
            {
                snode->rulesApplied.insert(*rule);
            }

            if ( ((snode->rulesApplied.size() - before) > 0 or not seen[node->successors[s].second]) )
            {
                queue.remove(node->successors[s].second);
                queue.push_back(node->successors[s].second);
            }
        }

    }
}

DNode::DNode(Initializer & init) :
    id(init.id), has_deadlock(init.has_deadlock), has_livelock(
        init.has_livelock), successors(init.successors)
{

}

DNode::Initializer::Initializer() : id(0), has_deadlock(false), has_livelock(false)
{

}

#endif
