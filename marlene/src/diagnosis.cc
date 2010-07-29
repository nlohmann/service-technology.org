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
#include <list>
#include <map>

#include "libconfig.h++"
#include "pnapi/pnapi.h"
#include "verbose.h"
#include "helper.h"
#include "macros.h"

#include "diagnosis.h"

Diagnosis::Diagnosis(std::string filename, MarkingInformation & pmi, unsigned int) : dgraph(new DGraph), mi(pmi), superfluous(false) //  messageBound
{

    libconfig::Config diagInfo;
    try {
        diagInfo.readFile(filename.c_str());
    }
    catch (libconfig::ParseException pex)
    {
        message("Parse error `%s' in diagnosis file `%s' in line %d.", pex.getError(), pex.getFile(), pex.getLine());
    }

    bool result = false;
    std::string criterion;

    diagInfo.lookupValue("controllability.result", result);
    diagInfo.lookupValue("controllability.criterion", criterion);

    if (result)
    {
        superfluous = true;
        message("The nets are adaptable, please run in non-diagnosis mode.");
        return;
    }

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

        int id = 0;
        states[i].lookupValue("id", id);

        init.id = dgraph->getIDForName(id);
        states[i].lookupValue("internalDeadlock", init.has_deadlock);
        states[i].lookupValue("internalLivelock", init.has_livelock);

        // status("Node %d has deadlock: %s", id, (init.has_deadlock?"yes":"no"));

        try
        {
            for ( int j = 0; j < states[i]["successors"].getLength(); ++j)
            {
                std::string labelName = states[i]["successors"][j][0];
                unsigned int lid = dgraph->getIDForLabel(labelName);
                int nodeId = states[i]["successors"][j][1];
                unsigned int nid = dgraph->getIDForName(nodeId);

                //status("Successor: label = %d, node = %d", lid, nid);
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
            status("Exception: Path = %s, what = %s", ex.getPath(), ex.what());
        }

        dgraph->nodes.push_back(node);


    }

    dgraph->collectRules();

    // diagInfo.write(stderr);
}

Diagnosis::~Diagnosis()
{
    delete dgraph;
}

void Diagnosis::evaluateDeadlocks(std::vector< pnapi::PetriNet *> & nets, pnapi::PetriNet & engine)
{
    FUNCIN
    if (superfluous)
    {
        FUNCOUT
        return;
    }

    std::string prefix = "engine.";

    for ( unsigned int i = 0; i < dgraph->deadlockNodes.size(); ++i )
    {
        DNode * node = dgraph->deadlockNodes[i];
        bool seen = false;
        for ( unsigned int prev = 0; prev < i and not seen; ++prev)
        {
            if ( node->deadlockMarkings == dgraph->deadlockNodes[prev]->deadlockMarkings )
            {
                if ( node->rulesApplied == dgraph->deadlockNodes[prev]->rulesApplied)
                {
                    seen = true;
                }
            }
        }
        if ( not seen )
        {
            message("Deadlock %d (node %d)", i + 1, node->getID());
            for ( unsigned int j = 0; j < node->deadlockMarkings.size(); ++j)
            {
                Marking & m = *(mi.markings[node->deadlockMarkings[j]]);
                std::vector< std::string > pending = m.getPendingMessages(engine, prefix);
                for ( unsigned int p = 0; p < pending.size(); ++p )
                {
                    message("Message %s is pending", pending[p].c_str());
                }
                for ( unsigned int net = 0; net < nets.size(); ++net )
                {
                    std::string prefix = "net" + toString(net + 1) + ".";
                    std::vector< std::string > required = m.getRequiredMessages(*(nets[net]), prefix);
                    if (required[required.size()-1] == "yes")
                    {
                        message("Net %s is already in a final state", prefix.c_str());
                    }
                    for ( unsigned int p = 0; p < required.size() - 1; ++p )
                    {
                        message("Message %s is required", required[p].c_str());
                    }
                }
                message("The following rules where previously applied:");
                if ( node->rulesApplied.empty() )
                {
                    std::cerr << "      none." << std::endl;
                }
                else
                {
                    bool first = true;
                    for ( std::set< std::string >::iterator rule = node->rulesApplied.begin(); rule != node->rulesApplied.end(); ++rule)
                    {
                        if ( first )
                        {
                            std::cerr << "      " << *rule;
                            first = false;
                        }
                        else
                        {
                            std::cerr << ", " << *rule;
                        }
                    }
                    std::cerr << std::endl;
                }
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

    for ( unsigned int i = 0; i < dgraph->livelockNodes.size(); ++i )
    {
        DNode * node = dgraph->livelockNodes[i];
        bool seen = false;
        for ( unsigned int prev = 0; prev < i and not seen; ++prev)
        {
            if ( node->livelockMarkings == dgraph->livelockNodes[prev]->livelockMarkings )
            {
                if ( node->rulesApplied == dgraph->livelockNodes[prev]->rulesApplied)
                {
                    seen = true;
                }
            }
        }
        if ( not seen )
        {
            message("Livelock %d (node %d)", i + 1, node->getID());
            for ( unsigned int j = 0; j < node->livelockMarkings.size(); ++j)
            {
                Marking & m = *(mi.markings[node->livelockMarkings[j]]);
                std::vector< std::string > pending = m.getPendingMessages(engine, prefix);
                for ( unsigned int p = 0; p < pending.size(); ++p )
                {
                    message("Message %s is pending", pending[p].c_str());
                }
                for ( unsigned int net = 0; net < nets.size(); ++net )
                {
                    std::string prefix = "net" + toString(net + 1) + ".";
                    std::vector< std::string > required = m.getRequiredMessages(*(nets[net]), prefix);
                    if (required[required.size()-1] == "yes")
                    {
                        message("Net %s is already in a final state", prefix.c_str());
                    }
                    for ( unsigned int p = 0; p < required.size() - 1; ++p )
                    {
                        message("Message %s is required", required[p].c_str());
                    }
                }
            }
            message("The following rules where previously applied:");
            if ( node->rulesApplied.empty() )
            {
                std::cerr << "      none." << std::endl;
            }
            else
            {
                bool first = true;
                for ( std::set< std::string >::iterator rule = node->rulesApplied.begin(); rule != node->rulesApplied.end(); ++rule)
                {
                    if ( first )
                    {
                        std::cerr << "      " << *rule;
                        first = false;
                    }
                    else
                    {
                        std::cerr << ", " << *rule;
                    }
                }
                std::cerr << std::endl;
            }
        }
    }

    FUNCOUT
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

            //status("label %s leads to node %d", label.c_str(), node->successors[s].second);
        }
        // message("The following rules where previously applied for node %d:", id);
        /*
        if ( node->rulesApplied.empty() )
        {
            std::cerr << "      none." << std::endl;
        }
        else
        {
            bool first = true;
            for ( std::set< std::string >::iterator rule = node->rulesApplied.begin(); rule != node->rulesApplied.end(); ++rule)
            {
                if ( first )
                {
                    std::cerr << "      " << *rule;
                    first = false;
                }
                else
                {
                    std::cerr << ", " << *rule;
                }
            }
            std::cerr << std::endl;
        }
        */

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
