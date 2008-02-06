/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider                                               *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    CommunicationGraph.cc
 *
 * \brief   common functions for IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */


#include "mynew.h"
#include "state.h"
#include "AnnotatedGraphEdge.h"
#include "GraphFormula.h"
#include "options.h"
#include "debug.h"
#include "CommunicationGraph.h"
#include "fiona.h"
#include "set_helpers.h"
#include <cassert>

using namespace std;


double global_progress = 0;
int show_progress = 0;

// for the STG->oWFN translation
extern void STG2oWFN_main();
extern FILE *stg_yyin;


//! \brief constructor
//! \param _PN
CommunicationGraph::CommunicationGraph(oWFN * _PN) {
    PN = _PN;
    tempBinDecision = (binDecision *) 0;
}


//! \brief destructor
CommunicationGraph::~CommunicationGraph() {
    trace(TRACE_5, "CommunicationGraph::~CommunicationGraph() : start\n");
    GraphNodeSet::iterator iter;

    for (iter = setOfSortedNodes.begin(); iter != setOfSortedNodes.end(); iter++) {
        delete *iter;
    }

    if (tempBinDecision) {
    	delete tempBinDecision;
    }
    
    trace(TRACE_5, "CommunicationGraph::~CommunicationGraph() : end\n");
}


//! \brief returns the number of nodes
//! \return number of nodes
unsigned int CommunicationGraph::getNumberOfNodes() const {
    return setOfSortedNodes.size();
}


//! \brief returns the number of stored states
//!        may only be called after computeGraphStatistics()
//! \return number stored states
unsigned int CommunicationGraph::getNumberOfStoredStates() const {
    return nStoredStates;
}


//! \brief returns the number of edges
//!        may only be called after computeGraphStatistics()
//! \return number edges
unsigned int CommunicationGraph::getNumberOfEdges() const {
    return nEdges;
}


//! \brief returns the number of blue nodes
//!        may only be called after computeGraphStatistics()
//! \return number of blue nodes
unsigned int CommunicationGraph::getNumberOfBlueNodes() const {
    return nBlueNodes;
}


//! \brief returns the number of blue edges
//!        may only be called after computeGraphStatistics()
//! \return number blue edges
unsigned int CommunicationGraph::getNumberOfBlueEdges() const {
    return nBlueEdges;
}


//! \brief calculates the root node of the graph
//!        for IG and OG
void CommunicationGraph::calculateRootNode() {

    trace(TRACE_5, "void CommunicationGraph::calculateRootNode(): start\n");

    assert(setOfSortedNodes.size() == 0);

    // create new OG root node
    root = new AnnotatedGraphNode();

    // calc the reachable states from that marking
    if (options[O_CALC_ALL_STATES]) {
    	// no state reduction
        PN->calculateReachableStatesFull(root);
    } else {
    	// state reduction
    	
    	// forget about all the states we have calculated so far
    	setOfStatesStubbornTemp.clear();
    	
        PN->calculateReducedSetOfReachableStatesInputEvent(setOfStatesStubbornTemp, &tempBinDecision, root);
    
        for (StateSet::iterator iter2 = setOfStatesStubbornTemp.begin(); iter2
                                        != setOfStatesStubbornTemp.end(); iter2++) {
                    	
        	root->addState(*iter2);            	
        }
        if (tempBinDecision) {
        //	delete tempBinDecision;
        }
    }

    root->setNumber(0);
    root->setName(intToString(0));
    setOfSortedNodes.insert(root);

    trace(TRACE_5, "void CommunicationGraph::calculateRootNode(): end\n");
}


//! \brief this function uses the find method from the template set
//! \param toAdd the AnnotatedGraphNode we are looking for in the graph
//! \return toAdd itself or NULL if toAdd could not be found 
AnnotatedGraphNode* CommunicationGraph::findGraphNodeInSet(AnnotatedGraphNode* toAdd) {

    GraphNodeSet::iterator iter = setOfSortedNodes.find(toAdd);
    if (iter != setOfSortedNodes.end()) {
        return *iter;
    } else {
        return NULL;
    }
}

//! \brief Computes statistics about this graph. They can be printed by
//!        printGraphStatistics().
void CommunicationGraph::computeGraphStatistics() {
    computeNumberOfStatesAndEdges();
    computeNumberOfBlueNodesEdges();
}


//! \brief Computes the total number of all states stored in all nodes and the
//!        number of all edges in this graph.
void CommunicationGraph::computeNumberOfStatesAndEdges() {

    std::map<AnnotatedGraphNode*, bool> visitedNodes;
    nStoredStates = 0;
    nEdges = 0;

    computeNumberOfStatesAndEdgesHelper(root, visitedNodes);
}


//! \brief Helps computeNumberOfStatesAndEdges to computes the total number of all
//!        states stored in all nodes and the number of all edges in this graph.
//!        This is done recursively (dfs).
//! \param v Current node in the iteration process.
//! \param visitedNodes[] Array of bool storing the nodes that we have
//!        already looked at.
void CommunicationGraph::computeNumberOfStatesAndEdgesHelper(AnnotatedGraphNode* v,
                                                             std::map<AnnotatedGraphNode*, bool>& visitedNodes) {

    assert(v != NULL);

    // counting the current node
    visitedNodes[v] = true;

    nStoredStates += v->reachGraphStateSet.size();

    // iterating over all successors
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* leavingEdge = edgeIter->getNext();

        AnnotatedGraphNode* vNext = (AnnotatedGraphNode *)leavingEdge->getDstNode();
        assert(vNext != NULL);

        nEdges++;

        if ((vNext != v) && !visitedNodes[vNext]) {
            computeNumberOfStatesAndEdgesHelper(vNext, visitedNodes);
        }
    }
    delete edgeIter;
}


//! \brief Computes the number of all blue to be shown nodes and edges in this
//!        graph.
void CommunicationGraph::computeNumberOfBlueNodesEdges() {

    std::map<AnnotatedGraphNode*, bool> visitedNodes;
    nBlueNodes = 0;
    nBlueEdges = 0;

    computeNumberOfBlueNodesEdgesHelper(root, visitedNodes);
}


//! \brief Helps computeNumberOfBlueNodesEdges() to computes the number of all blue
//!        to be shown nodes and edges in this graph.
//!        This is done recursively (dfs).
//! \param v Current node in the iteration process.
//! \param visitedNodes[] Array of bool storing the nodes that we have
//!        already looked at.
void CommunicationGraph::computeNumberOfBlueNodesEdgesHelper(AnnotatedGraphNode* v,
                                                             std::map<AnnotatedGraphNode*, bool>& visitedNodes) {

    assert(v != NULL);

    // counting the current node
    visitedNodes[v] = true;

    if (v->getColor() == BLUE &&
        (parameters[P_SHOW_EMPTY_NODE] || v->reachGraphStateSet.size() != 0)) {

        nBlueNodes++;

        // iterating over all successors
        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* leavingEdge = edgeIter->getNext();

            AnnotatedGraphNode* vNext = (AnnotatedGraphNode *)leavingEdge->getDstNode();
            assert(vNext != NULL);

            if (vNext->getColor() == BLUE &&
                (parameters[P_SHOW_EMPTY_NODE] || vNext->reachGraphStateSet.size() != 0)) {

                nBlueEdges++;
            }

            if ((vNext != v) && !visitedNodes[vNext]) {
                computeNumberOfBlueNodesEdgesHelper(vNext, visitedNodes);
            }
        } // while
        delete edgeIter;
    }
}


//! \brief checks if the given state activates at least one output event
//! \param s the state that is checked for activating output events
//! \return returns true, if the given state activates at least one output event, else false
bool CommunicationGraph::stateActivatesOutputEvents(State * s) {
    s->decode(PN);

    for (unsigned int i = 0; i < PN->getPlaceCount(); i++) {
        if (PN->getPlace(i)->type == OUTPUT && PN->CurrentMarking[i] > 0) {
            return true;
        }
    }
    return false;
}


//! \brief adds toAddValue to global progress value
//! \param toAddValue the additional progress value
void CommunicationGraph::addProgress(double toAddValue) {

    trace(TRACE_4, "\t adding ");

    // double2int in per cent = trunc(100*value)
    trace(TRACE_4, intToString(int(100 * toAddValue)));
    trace(TRACE_4, ",");
    // precision 4 digits after comma = (x * 100 * 1000) mod 1000

    int aftercomma = int(100 * 10000 * toAddValue) % 10000;

    if (aftercomma < 10)
        trace(TRACE_4, "0");
    if (aftercomma < 100)
        trace(TRACE_4, "0");
    if (aftercomma < 1000)
        trace(TRACE_4, "0");

    trace(TRACE_4, intToString(aftercomma));

    trace(TRACE_4, " to progress\n");

    global_progress += toAddValue;
}


//! \brief prints the current global progress value depending whether the value
//!        changed significantly and depending on the debug-level set
void CommunicationGraph::printProgress() {

//    return;

    int progress_step_size = 5;
    int current_progress = int(100 * global_progress);

    if (current_progress >= (show_progress + progress_step_size)) {
        // significant progress change
        if (debug_level == TRACE_0) {
            trace(TRACE_0, " " + intToString(current_progress) + " ");
        } else {
            trace(TRACE_0, "    progress: " + intToString(current_progress) + " %\n");
        }
        // assigning next progress value to show
        show_progress = current_progress;
    }
}


//! \brief prints the current global progress value depending whether the value
//!        changed significantly and depending on the debug-level set
void CommunicationGraph::printProgressFirst() {

    trace(TRACE_0, "    ");
//    return;

    if (debug_level == TRACE_0) {
        trace(TRACE_0, "progress (in %): 0 ");
    } else {
        trace(TRACE_0, "progress: 0 %\n");
    }
}


//! \brief Prints statistics about this graph. May only be called after
//!       computeGraphStatistics().
void CommunicationGraph::printGraphStatistics() {
    trace(TRACE_0, "    number of nodes: " + intToString(getNumberOfNodes()) + "\n");
    trace(TRACE_0, "    number of edges: " + intToString(getNumberOfEdges()) + "\n");
    trace(TRACE_0, "    number of deleted nodes: " + intToString(numberDeletedVertices) + "\n");
    trace(TRACE_0, "    number of blue nodes: " + intToString(getNumberOfBlueNodes()) + "\n");
    trace(TRACE_0, "    number of blue edges: " + intToString(getNumberOfBlueEdges()) + "\n");
    trace(TRACE_0, "    number of states calculated: " + intToString(State::state_count) + "\n");
    trace(TRACE_0, "    number of states stored in nodes: " + intToString(getNumberOfStoredStates()) + "\n");
}


//! \brief creates a dot file of the graph
void CommunicationGraph::printGraphToDot() {

    unsigned int maxSizeForDotFile = 5000; // number relevant for .out file
    unsigned int maxSizeForPNGFile = 500; // number relevant to generate png

    if (((parameters[P_SHOW_RED_NODES] || parameters[P_SHOW_ALL_NODES]) && 
    		(getNumberOfNodes() <= maxSizeForDotFile))
    	 ||
    	 ((parameters[P_SHOW_EMPTY_NODE] || parameters[P_SHOW_BLUE_NODES]) && 
    	 getNumberOfBlueNodes() <= maxSizeForDotFile)) {

        trace(TRACE_0, "creating the dot file of the graph...\n");
        AnnotatedGraphNode* rootNode = root;

        string outfilePrefixWithOptions = options[O_OUTFILEPREFIX] ? outfilePrefix : PN->filename;

        if (!options[O_CALC_ALL_STATES]) {
            outfilePrefixWithOptions += ".R";
        }

        if (options[O_DIAGNOSIS]) {
            outfilePrefixWithOptions += ".diag";
        } else {
            if (parameters[P_OG]) {
                outfilePrefixWithOptions += ".OG";
            } else {
                outfilePrefixWithOptions += ".IG";
            }
        }

        string dotFileName = outfilePrefixWithOptions + ".out";
        fstream dotFile(dotFileName.c_str(), ios_base::out | ios_base::trunc);
        dotFile << "digraph g1 {\n";
        dotFile << "graph [fontname=\"Helvetica\", label=\"";

        if (options[O_DIAGNOSIS]) {
            dotFile << "diagnosis of ";
        } else {
            parameters[P_OG] ? dotFile << "OG of " : dotFile << "IG of ";
        }

        dotFile << PN->filename;
        dotFile << " (parameters:";
        if (parameters[P_IG] && options[O_CALC_REDUCED_IG]) {
            dotFile << " -r";
        }
        if (parameters[P_IG] && !options[O_CALC_ALL_STATES]) {
                    dotFile << " -R";
                }
        if (options[O_MESSAGES_MAX]) {
            dotFile << " -m" << intToString(messages_manual);
        }
        if (options[O_EVENT_USE_MAX]) {
            dotFile << " -e" << intToString(events_manual);
        }
        dotFile << ")";
        dotFile << "\"];\n";
        dotFile << "node [fontname=\"Helvetica\" fontsize=10];\n";
        dotFile << "edge [fontname=\"Helvetica\" fontsize=10];\n";

        std::map<AnnotatedGraphNode*, bool> visitedNodes;


        // filling the file with nodes and edges
        printGraphToDotRecursively(rootNode, dotFile, visitedNodes);


        dotFile << "}";
        dotFile.close();
        // ... dot file created (.out) //

        // prepare dot command line creating the picture
        string imgFileName = outfilePrefixWithOptions + ".png";
        string dotCmd = "dot -Tpng \"" + dotFileName + "\" -o \"" + imgFileName + "\"";

        // print only, if number of nodes is lower than required
        // if option is set to show all nodes, then we compare the number of all nodes
        // otherwise, we compare the number of blue nodes only
        bool graphToBigForPNG = false;
        std::string reasonForFail = "";

        if (parameters[P_SHOW_RED_NODES] || parameters[P_SHOW_ALL_NODES]) {
            // we have to check number of ALL nodes
            if (getNumberOfNodes() > maxSizeForPNGFile) {
                reasonForFail = "more than " + intToString(maxSizeForPNGFile) + " nodes";
                graphToBigForPNG = true;
            }
        } else if (parameters[P_SHOW_EMPTY_NODE] || parameters[P_SHOW_BLUE_NODES]) {
            // we have to check number of BLUE nodes
            if (getNumberOfBlueNodes() > maxSizeForPNGFile) {
                reasonForFail = "more than " + intToString(maxSizeForPNGFile) + " blue nodes";
                graphToBigForPNG = true;
            }
        }

        if (graphToBigForPNG) {
            trace(TRACE_0, "graph is too big to create the graphics :( ");
            trace(TRACE_0, "(" + reasonForFail + ")\n");
            trace(TRACE_0, dotCmd + "\n");
        } else {
            if (!parameters[P_NOPNG]) {
                // print commandline and execute system command
                trace(TRACE_0, dotCmd + "\n");
                system(dotCmd.c_str());
            }

//            // on windows machines, the png file can be shown per system call
//            if (parameters[P_OG]) {
//                trace(TRACE_0, "initiating command to show the graphics...\n");
//                string showCmd = "cmd /c \"start " + imgFileName + "\"";
//                system(showCmd.c_str());
//            }

        }
    } else {
        trace(TRACE_0, "graph is too big to create dot file\n");
    }
}


//! \brief breadthsearch through the graph printing each node and edge to the output stream
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
void CommunicationGraph::printGraphToDotRecursively(AnnotatedGraphNode* v,
                                                    fstream& os,
                                                    std::map<AnnotatedGraphNode*, bool>& visitedNodes) {

    assert(v != NULL);

    // continue only if current node v is to show
    if (!v->isToShow(root))
        return;

    os << "p" << v->getNumber() << " [label=\"# " << v->getNumber() << "\\n";

    StateSet::iterator iter; // iterator over the stateList's elements

    if (parameters[P_SHOW_STATES_PER_NODE] || parameters[P_SHOW_DEADLOCKS_PER_NODE]) {

        for (iter = v->reachGraphStateSet.begin(); iter != v->reachGraphStateSet.end(); iter++) {

            // a marking is printed if it is a deadlock, a final state or if the parameter "-s allstates"
            if ((*iter)->type == DEADLOCK ||
                (*iter)->type == FINALSTATE ||
                parameters[P_SHOW_STATES_PER_NODE] ) {
                (*iter)->decode(PN);
                os << "(" << *iter << ") " ;
                os << "[" << PN->getCurrentMarkingAsString() << "]";
                os << " (";

                // print the suffix (state type)
                switch ((*iter)->exactType()) {
                    case I_DEADLOCK:    os << "iDL";    break;
                    case E_DEADLOCK:    os << "eDL";    break;
                    case FINALSTATE:    os << "FS";     break;
                    case TRANS:         os << "TR";     break;
                    default:            assert(false);
                }

                os << ")" << "\\n";
            }
        }
    }

    if (parameters[P_OG]) {
        // add annotation to node
        v->getAnnotation()->simplify();
        os << v->getAnnotation()->asString();
    }

    // the diagnosis mode uses different colors
    if (options[O_DIAGNOSIS]) {
        os << "\", fontcolor=black, color=" << v->getDiagnosisColor().toString();
    } else {
        os << "\", fontcolor=black, color=" << v->getColor().toString();

        if (v->getColor() == RED) {
            os << ", style=dashed";
        }
    }

    bool finalNode = false;

    for ( nodes_t::const_iterator checkNode = finalNodes.begin(); checkNode != finalNodes.end(); checkNode++) {
        if ((*checkNode) == v) {
            finalNode = true;
        }
    }

    if (finalNode) {
        os << ", peripheries=2";
    }

    os << "];\n";

    visitedNodes[v] = true;

    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
         AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();

        if (!vNext->isToShow(root))
            continue;

        os << "p" << v->getNumber() << "->"
           << "p" << vNext->getNumber()
           << " [label=\"" << element->getLabel();

        // the diagnosis mode uses different colors
        if (options[O_DIAGNOSIS]) {
            os << "\", fontcolor=black, color=" << vNext->getDiagnosisColor().toString();
        } else {
            os << "\", fontcolor=black, color=" << vNext->getColor().toString();
        }

        // in diagnosis mode, draw "unenforcable" arcs dashed
        if (options[O_DIAGNOSIS] && !v->edgeEnforcable(element)) {
            os << ", style=dashed";
        }

        os << "];\n";
        if ((vNext != v) && !visitedNodes[vNext]) {
            printGraphToDotRecursively(vNext, os, visitedNodes);
        }
    } // while
    delete edgeIter;
}


//! \brief creates a STG file of the graph
void CommunicationGraph::printGraphToSTG() {
    trace(TRACE_0, "\ncreating the STG file of the graph...\n");
    AnnotatedGraphNode* rootNode = root;
    string buffer;

    // set file name
    if (options[O_OUTFILEPREFIX]) {
        buffer = outfilePrefix;
    } else {
        buffer = PN->filename;
    }

    if (parameters[P_OG]) {
        if (options[O_CALC_ALL_STATES]) {
            buffer += ".OG.stg";
        } else {
            buffer += ".R.OG.stg";
        }
    } else {
        if (options[O_CALC_ALL_STATES]) {
            buffer += ".IG.stg";
        } else {
            buffer += ".R.IG.stg";
        }
    }

    // create file
    fstream dotFile(buffer.c_str(), ios_base::out | ios_base::trunc);

    // print header
    dotFile << ".model Labeled_Transition_System" << endl;

    // list transitions (use place names)
    dotFile << ".dummy";
    assert(PN != NULL);
    for (unsigned int i = 0; i < PN->getPlaceCount(); i++) {
        if (PN->getPlace(i)->type == INPUT) {
            dotFile << " out." << PN->getPlace(i)->name;
        } else if (PN->getPlace(i)->type == OUTPUT) {
            dotFile << " in." << PN->getPlace(i)->name;
        }
    }
    dotFile << endl;
    dotFile << ".state graph" << endl;

    // mark all nodes as unvisited
    std::map<AnnotatedGraphNode*, bool> visitedNodes;

    // traverse the nodes recursively
    printGraphToSTGRecursively(rootNode, dotFile, visitedNodes);

    // the initial marking
    dotFile << ".marking {p" << rootNode->getNumber() << "}" << endl;

    // end and close file
    dotFile << ".end"<< endl;
    dotFile.close();

    // prepare Petrify command line for printing
    if (parameters[P_OG]) {
        buffer = string(HAVE_PETRIFY) + " " + PN->filename
                 + ".OG.stg -dead -ip -o " + PN->filename + ".OG.pn";
    } else {
        buffer = string(HAVE_PETRIFY) + " " + PN->filename
                 + ".IG.stg -dead -ip -o " + PN->filename + ".IG.pn";
    }

    // print commandline and execute system command
    trace(TRACE_0, buffer + "\n");

    if (HAVE_PETRIFY != "not found") {
        system(buffer.c_str());
    } else {
        trace(TRACE_0, "cannot execute command as Petrify was not found in path\n");
        return;
    }

    // the filename of the generated Petri net
    string generated_stg_file = PN->filename;
    if (parameters[P_OG]) {
        generated_stg_file += ".OG.pn";
    } else {
        generated_stg_file += ".IG.pn";
    }

    std::cerr << "parsing file file `" << generated_stg_file << "'..." << std::endl;
    stg_yyin = fopen(generated_stg_file.c_str(), "r");
    STG2oWFN_main();
}


//! \brief breadthsearch through the graph printing each node and edge to the output stream
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
void CommunicationGraph::printGraphToSTGRecursively(AnnotatedGraphNode* v,
                                                    fstream& os,
                                                    std::map<AnnotatedGraphNode*, bool>& visitedNodes) {
    assert(v != NULL);

    if (!v->isToShow(root))
        return;

    visitedNodes[v] = true;

    // arcs
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();

        if (!vNext->isToShow(root))
            continue;

        string this_edges_label = element->getLabelWithoutPrefix();
        os << "p" << v->getNumber() << " ";

        switch (element->getType()) {
            case SENDING  : os << "out."; break;
            case RECEIVING: os << "in.";  break;
            default: break;
        }

        os << this_edges_label;
        os << " p" << vNext->getNumber() << endl;

        if ((vNext != v) && !visitedNodes[vNext]) {
            printGraphToSTGRecursively(vNext, os, visitedNodes);
        }
    }
    delete edgeIter;
}


//! \brief returns the filename of the owfn this graph has been created from
//! \return filename
string CommunicationGraph::returnOWFnFilename()
{
    return (string(PN->filename));
}

//! \brief DESCRIPTION
//! \return DESCRIPTION
bool CommunicationGraph::annotateGraphDistributedly() {
    AnnotatedGraphNode* rootNode = root;

    // mark all nodes as unvisited
    std::map<AnnotatedGraphNode*, bool> visitedNodes;

    // traverse the nodes recursively
    return annotateGraphDistributedlyRecursively(rootNode, visitedNodes);
}


//! \brief DESCRIPTION
//! \param v current node in the iteration process
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \return DESCRIPTION
bool CommunicationGraph::annotateGraphDistributedlyRecursively(AnnotatedGraphNode* v,
                                                               std::map<AnnotatedGraphNode*, bool>& visitedNodes) {
    assert(v != NULL);
    set<string> disabled, enabled;

    if (!v->isToShow(root))
        return false;

    // save for each state the outgoing labels;
    static map<AnnotatedGraphNode*, set<string> > outgoing_labels;

    // store outgoing lables
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        if (element->getDstNode() != NULL &&element->getDstNode()->isToShow(root) ) {
            outgoing_labels[v].insert(element->getLabel());
        }
    }

    // standard procedurce
    visitedNodes[v] = true;

    edgeIter = v->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();

        if (!vNext->isToShow(root))
            continue;

        if ((vNext != v) && !visitedNodes[vNext]) {
            bool done = annotateGraphDistributedlyRecursively(vNext, visitedNodes);

            if (done) {
                delete edgeIter;
                return done;
            }

            ///////////////////////////////////////////
            // 1. organized events that enable others
            ///////////////////////////////////////////
            enabled = setDifference(setDifference(outgoing_labels[vNext],
                                                  PN->getPort(PN->getPortForLabel(element->getLabel()))),
                                    outgoing_labels[v]);

            if (!enabled.empty()) {
                cerr << "  in state " << v->getNumber() << ": "
                     << element->getLabel() << " enables " << enabled.size()
                     << " elements: ";

                // list what's enabled
                for (set<string>::const_iterator label = enabled.begin();
                     label != enabled.end(); label++) {
                    cerr << *label << " ";
                }
                cerr << endl;

                // delete enabled states
                for (set<string>::const_iterator label = enabled.begin();
                     label != enabled.end(); label++) {
                    removeLabeledSuccessor(vNext, *label);
                    outgoing_labels.clear(); // reset the label list
                    return true;
                }
            }

            ///////////////////////////////////////////
            // 2. organize events that disable others
            ///////////////////////////////////////////
            disabled = setDifference(setDifference(outgoing_labels[v],
                                                   PN->getPort(PN->getPortForLabel(element->getLabel()))),
                                     outgoing_labels[vNext]);

            if (!disabled.empty()) {
                cerr << "  in state " << v->getNumber() << ": "
                     << element->getLabel() << " disables " << disabled.size()
                     << " elements" << endl;

                /*
                 for (set<string>::const_iterator label = disabled.begin();
                      label != disabled.end(); label++) {
                    if (removeLabeledSuccessor(v, *label) ) {
                        return true;
                    } else {
                        continue;
                    }
                }

                cerr << disabled.size() << endl;
                 */
            }
        }
    }
    delete edgeIter;

    if (v == root) {
        if (disabled.empty()) {
            cerr << "no nondeterminism left" << endl;
        } else {
            cerr << "nondeterminism left" << endl;
        }
    }

    return false;
}


//! \brief DESCRIPTION
//! \param DESCRIPTION
//! \param DESCRIPTION
void CommunicationGraph::removeLabeledSuccessor(AnnotatedGraphNode* v, std::string label) {

    AnnotatedGraphNode::LeavingEdges::Iterator edgeIter = v->getLeavingEdgesIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();

        if (element->getLabel() == label) {
            // AnnotatedGraphNode *vNext = element->getDstNode();
            //cerr << "    deleting edge " << element->getLabel() << " connecting state " << v->getNumber() << " and " << vNext->getNumber() << endl;
            edgeIter->remove();
            delete edgeIter;
            return;
        }
    }
    delete edgeIter;

    // node not found
    assert(false);
}


//! \brief DESCRIPTION
void CommunicationGraph::diagnose() {
    cerr << "Diagnosis:" << endl;
    if (root->getColor() == BLUE) {
        cerr << "Please note: the net is controllable." << endl;
    }

    std::map<AnnotatedGraphNode*, bool> visitedNodes;
    diagnose_recursively(root, visitedNodes);
    cerr << endl;
}


//! \brief DESCRIPTION
//! \param v current node in the iteration process
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \return DESCRIPTION
GraphNodeDiagnosisColor_enum CommunicationGraph::diagnose_recursively(AnnotatedGraphNode* v, std::map<AnnotatedGraphNode*, bool>& visitedNodes) {
    assert(v != NULL);

    // if color is known already, return it
    if (v->getDiagnosisColor() != DIAG_UNSET) {
        assert(visitedNodes[v]);
        return v->getDiagnosisColor();
    }

    visitedNodes[v] = true;

    bool internal_deadlock_seen = false;
    bool external_deadlock_seen = false;
    bool final_state_seen = false;

    // iterate the states to check if there are internal deadlocks or final states
    for (StateSet::const_iterator state = v->reachGraphStateSet.begin();
         state != v->reachGraphStateSet.end(); state++) {
        (*state)->decode(PN);

        switch ((*state)->exactType()) {
            case I_DEADLOCK: internal_deadlock_seen = true; break;
            case E_DEADLOCK: external_deadlock_seen = true; break;
            case FINALSTATE: final_state_seen = true;       break;
            default: break;
        }
    }

    
    ///////////////////////////////////////////////
    // CASE 1: NODE HAS INTERNAL DEADLOCK => RED //
    ///////////////////////////////////////////////
    if (internal_deadlock_seen) {
        return v->setDiagnosisColor(DIAG_RED);
    }

    //////////////////////////////////////////////////////////
    // CASE 2: NODE HAS FINAL STATE AND NO DEADLOCK => BLUE //
    //////////////////////////////////////////////////////////
    if (final_state_seen && !external_deadlock_seen) {
        assert (!internal_deadlock_seen);
        return v->setDiagnosisColor(DIAG_BLUE);
    }
    

    // node color cannot be quickly derived, so the children have to be considered
    set<GraphNodeDiagnosisColor_enum> childrenDiagnosisColors;

    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();

        if (vNext != v) {
            childrenDiagnosisColors.insert(diagnose_recursively(vNext, visitedNodes));
        }
    }
    delete edgeIter;

    // collect information about the child nodes
    bool red_child = (childrenDiagnosisColors.find(DIAG_RED)
            != childrenDiagnosisColors.end());
    bool blue_child = (childrenDiagnosisColors.find(DIAG_BLUE)
            != childrenDiagnosisColors.end());

    
    /////////////////////////////////////////////////
    // CASE 3: NODE HAS ONLY BLUE CHILDREN => BLUE //
    /////////////////////////////////////////////////
    if (childrenDiagnosisColors.size() == 1 && blue_child) {
        assert (!internal_deadlock_seen);
        return v->setDiagnosisColor(DIAG_BLUE);
    }

    //////////////////////////////////////////////////////////////
    // CASE 4: NODE HAS A FINAL STATE AND A RED CHILD => VIOLET //
    //////////////////////////////////////////////////////////////
    if (final_state_seen && red_child) {
        assert (!internal_deadlock_seen);
        assert (external_deadlock_seen);
        return v->setDiagnosisColor(DIAG_VIOLET);
    }
    
    ///////////////////////////////////////////////
    // CASE 5: NODE HAS ONLY RED CHILDREN => RED //
    ///////////////////////////////////////////////
    if (childrenDiagnosisColors.size() == 1 && red_child) {
        assert (!internal_deadlock_seen);
        assert (external_deadlock_seen);
        return v->setDiagnosisColor(DIAG_RED);
    }
    
    ///////////////////////////////////////////////////////
    // CASE 6: RED SUCCESSOR CANNOT BE AVOIDED => VIOLET //
    ///////////////////////////////////////////////////////
    if (red_child && !v->coloredSuccessorsAvoidable(DIAG_RED)) {
        assert (!internal_deadlock_seen);
        assert (external_deadlock_seen);
        return v->setDiagnosisColor(DIAG_VIOLET);
    }
    
    ///////////////////////////////////////////////////////////////////////
    // CASE 7: IF AN EDGE BECOMES RED IN A SUCCESSOR => SUCCESSOR ORANGE //
    ///////////////////////////////////////////////////////////////////////
    edgeIter = v->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();
        
        if (v->changes_color(element)) {
            if (vNext->getDiagnosisColor() == DIAG_UNSET) {
                vNext->setDiagnosisColor(DIAG_ORANGE);
            }
        }
    }
    delete edgeIter;
    
    cerr << "found no color for " << v->getNumber() << endl;
    
    // no color found yet
    return v->setDiagnosisColor(DIAG_UNSET);
}
