/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
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
#include "GraphEdge.h"
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

//! \param _PN
//! \brief constructor
CommunicationGraph::CommunicationGraph(oWFN * _PN) :
    root(NULL) {

    PN = _PN;
}


//! \brief destructor
CommunicationGraph::~CommunicationGraph() {
    trace(TRACE_5, "CommunicationGraph::~CommunicationGraph() : start\n");
    GraphNodeSet::iterator iter;

    for (iter = setOfVertices.begin(); iter != setOfVertices.end(); iter++) {
        delete *iter;
    }

    trace(TRACE_5, "CommunicationGraph::~CommunicationGraph() : end\n");
}


//! \return pointer to root
//! \brief returns a pointer to the root node of the graph
GraphNode * CommunicationGraph::getRoot() const {
    return root;
}


unsigned int CommunicationGraph::getNumberOfNodes() const {
    return setOfVertices.size();
}


unsigned int CommunicationGraph::getNumberOfStoredStates() const {
    return nStoredStates;
}


unsigned int CommunicationGraph::getNumberOfEdges() const {
    return nEdges;
}


unsigned int CommunicationGraph::getNumberOfBlueNodes() const {
    return nBlueNodes;
}


unsigned int CommunicationGraph::getNumberOfBlueEdges() const {
    return nBlueEdges;
}


//! \brief calculates the root node of the graph
// for IG and OG
void CommunicationGraph::calculateRootNode() {

    trace(TRACE_5, "void CommunicationGraph::calculateRootNode(): start\n");

    assert(setOfVertices.size() == 0);

    // create new OG root node
    root = new GraphNode(PN->getInputPlaceCount() + PN->getOutputPlaceCount());

    // calc the reachable states from that marking
    if (options[O_CALC_ALL_STATES]) {
        PN->calculateReachableStatesFull(root);
    } else {
        PN->calculateReachableStatesInputEvent(root);
    }

    root->setNumber(0);
    root->setName(intToString(0));
    setOfVertices.insert(root);

    trace(TRACE_5, "void CommunicationGraph::calculateRootNode(): end\n");
}


//! \param toAdd the GraphNode we are looking for in the graph
//! \brief this function uses the find method from the template set
GraphNode * CommunicationGraph::findGraphNodeInSet(GraphNode * toAdd) {

    GraphNodeSet::iterator iter = setOfVertices.find(toAdd);
    if (iter != setOfVertices.end()) {
        return *iter;
    } else {
        return NULL;
    }
}


//! \param node the node to be analysed
//! \brief analyses the node and sets its color
void CommunicationGraph::analyseNode(GraphNode* node) {

    trace(TRACE_5, "CommunicationGraph::analyseNode(GraphNode* node) : start\n");

    trace(TRACE_3, "\t\t\t analysing node ");
    trace(TRACE_3, node->getNumber() + "...\n");

    assert(node->getColor() == BLUE);

    // analyse node by its formula
    GraphNodeColor analysedColor = node->analyseNodeByFormula();
    node->setColor(analysedColor);

    trace(TRACE_5, "CommunicationGraph::analyseNode(GraphNode* node) : end\n");
}



void CommunicationGraph::computeGraphStatistics() {
    computeNumberOfStatesAndEdges();
    computeNumberOfBlueNodesEdges();
}

void CommunicationGraph::computeNumberOfStatesAndEdges() {

    std::map<GraphNode*, bool> visitedNodes;

//    for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
//        visitedNodes[i] = false;
//    }
//
    nStoredStates = 0;
    nEdges        = 0;

    computeNumberOfStatesAndEdgesHelper(root, visitedNodes);
}


void CommunicationGraph::computeNumberOfStatesAndEdgesHelper(
    GraphNode*    v,
    std::map<GraphNode*, bool>& visitedNodes) {

    assert(v != NULL);

    // counting the current node
    visitedNodes[v] = true;

    nStoredStates += v->reachGraphStateSet.size();

    // iterating over all successors
    GraphNode::LeavingEdges::ConstIterator edgeIter =
        v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        GraphEdge<>* leavingEdge = edgeIter->getNext();

        GraphNode* vNext = leavingEdge->getDstNode();
        assert(vNext != NULL);

        nEdges++;

        if ((vNext != v) && !visitedNodes[vNext]) {
            computeNumberOfStatesAndEdgesHelper(vNext, visitedNodes);
        }
    }
    delete edgeIter;
}


void CommunicationGraph::computeNumberOfBlueNodesEdges() {

    std::map<GraphNode*, bool> visitedNodes;

//    for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
//        visitedNodes[i] = false;
//    }

    nBlueNodes = 0;
    nBlueEdges = 0;

    computeNumberOfBlueNodesEdgesHelper(root, visitedNodes);
}


void CommunicationGraph::computeNumberOfBlueNodesEdgesHelper(
    GraphNode* v,
    std::map<GraphNode*, bool>& visitedNodes) {

    assert(v != NULL);

    // counting the current node
    visitedNodes[v] = true;

    if (v->getColor() == BLUE &&
        (parameters[P_SHOW_EMPTY_NODE] || v->reachGraphStateSet.size() != 0)) {

        nBlueNodes++;

        // iterating over all successors
        GraphNode::LeavingEdges::ConstIterator edgeIter =
            v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            GraphEdge<>* leavingEdge = edgeIter->getNext();

            GraphNode* vNext = leavingEdge->getDstNode();
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


//! \param s the state that is checked for activating output events
//! \brief returns true, if the given state activates at least one output event
bool CommunicationGraph::stateActivatesOutputEvents(State * s) {
    s->decode(PN);

    for (unsigned int i = 0; i < PN->getPlaceCount(); i++) {

        if (PN->getPlace(i)->type == OUTPUT && PN->CurrentMarking[i] > 0) {
            return true;
        }
    }
    return false;
}


//! \param toAddValue the additional progress value
//! \brief adds toAddValue to global progress value
void CommunicationGraph::addProgress(double toAddValue) {

    trace(TRACE_4, "\t adding ");

    // double2int in per cent = trunc(100*value)
    trace(TRACE_4, intToString(int(100 * toAddValue)));
    trace(TRACE_4, ",");
    // precision 4 digits after comma = (x * 100 * 1000) mod 1000

    int aftercomma = int(100 * 10000 * toAddValue) % 10000;

    if (aftercomma <   10) trace(TRACE_4, "0");
    if (aftercomma <  100) trace(TRACE_4, "0");
    if (aftercomma < 1000) trace(TRACE_4, "0");

    trace(TRACE_4, intToString(aftercomma));

    trace(TRACE_4, " to progress\n");

    global_progress += toAddValue;
}


//! \brief prints the current global progress value depending whether the value
//! changed significantly and depending on the debug-level set
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
//! changed significantly and depending on the debug-level set
void CommunicationGraph::printProgressFirst() {

    trace(TRACE_0, "    ");
//    return;

    if (debug_level == TRACE_0) {
        trace(TRACE_0, "progress (in %): 0 ");
    } else {
        trace(TRACE_0, "progress: 0 %\n");
    }
}


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

    unsigned int maxWritingSize = 5000;        // number relevant for .out file
    unsigned int maxPrintingSize = 500;        // number relevant to generate png

    if (getNumberOfNodes() <= maxWritingSize) {

        trace(TRACE_0, "creating the dot file of the graph...\n");
        GraphNode* rootNode = root;

        string outfilePrefixWithOptions =
            options[O_OUTFILEPREFIX] ? outfilePrefix : PN->filename;

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

        std::map<GraphNode*, bool> visitedNodes;
//        for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
//            visitedNodes[i] = 0;
//        }

        printGraphToDotRecursively(rootNode, dotFile, visitedNodes);

        dotFile << "}";
        dotFile.close();

        // prepare dot command line for printing
        string imgFileName = outfilePrefixWithOptions + ".png";
        string dotCmd = "dot -Tpng \"" + dotFileName + "\" -o \"" +
            imgFileName + "\"";

        // print commandline and execute system command
        if ((options[O_SHOW_NODES] && getNumberOfNodes() <= maxPrintingSize) ||
           (!options[O_SHOW_NODES] && getNumberOfBlueNodes() <= maxPrintingSize)) {
            // print only, if number of nodes is lower than required
            // if option is set to show all nodes, then we compare the number of all nodes
            // otherwise, we compare the number of blue nodes only
            trace(TRACE_0, dotCmd + "\n");
            system(dotCmd.c_str());

//            // on windows machines, the png file can be shown per system call
//            if (parameters[P_OG]) {
//                trace(TRACE_0, "initiating command to show the graphics...\n");
//                string showCmd = "cmd /c \"start " + imgFileName + "\"";
//                system(showCmd.c_str());
//            }

        } else {
            trace(TRACE_0, "graph is too big to create the graphics :( ");
            trace(TRACE_0, "(greater " + intToString(maxPrintingSize) + ")\n");
            trace(TRACE_0, dotCmd + "\n");
        }
    } else {
        trace(TRACE_0, "graph is too big to create dot file\n");
    }
}


//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \brief breadthsearch through the graph printing each node and edge to the output stream
void CommunicationGraph::printGraphToDotRecursively(GraphNode * v, fstream& os, std::map<GraphNode*, bool>& visitedNodes) {

    assert(v != NULL);

    if (!v->isToShow(root))
        return;

    os << "p" << v->getNumber() << " [label=\"# " << v->getNumber() << "\\n";

    StateSet::iterator iter;  // iterator over the stateList's elements

    if (parameters[P_SHOW_STATES_PER_NODE] || parameters[P_SHOW_DEADLOCKS_PER_NODE]) {

        for (iter = v->reachGraphStateSet.begin(); iter != v->reachGraphStateSet.end(); iter++) {

            (*iter)->decode(PN);    // need to decide if it is an external or internal deadlock

            string kindOfDeadlock = "i"; // letter for 'i' internal or 'e' external deadlock
            unsigned int i;

            switch ((*iter)->type) {
                case DEADLOCK:
                    os << "[" << PN->getCurrentMarkingAsString() << "]";
                    os << " (";

                    if (PN->transNrQuasiEnabled > 0) {
                        kindOfDeadlock = "e";
                    } else {
                        for (i = 0; i < PN->getOutputPlaceCount(); i++) {
                            if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
                                kindOfDeadlock = "e";
                                continue;
                            }
                        }
                    }
                    os << kindOfDeadlock << "DL" << ")" << "\\n";
                    break;
                case FINALSTATE:
                    os << "[" << PN->getCurrentMarkingAsString() << "]";
                    os << " (";
                    os << "FS" << ")" << "\\n";
                    break;
                default:
                    if (parameters[P_SHOW_STATES_PER_NODE]) {
                        os << "[" << PN->getCurrentMarkingAsString() << "]";
                        os << " (" << "TR" << ")" << "\\n";
                    }
                    break;
            }
        }
    }

    if (parameters[P_OG]) {
        // add annotation to node
        v->getAnnotation()->simplify();
        os << v->getAnnotation()->asString();
    }

    if (options[O_DIAGNOSIS]) {
        os << "\", fontcolor=black, color=" << v->getDiagnosisColor().toString();
    } else {
        os << "\", fontcolor=black, color=" << v->getColor().toString();

        if (v->getColor() == RED) {
            os << ", style=dashed";
        }
    }
        
    os << "];\n";

    visitedNodes[v] = true;

    GraphNode::LeavingEdges::ConstIterator edgeIter =
        v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        GraphEdge<> * element = edgeIter->getNext();
        GraphNode * vNext = element->getDstNode();

        if (!vNext->isToShow(root))
            continue;

        os << "p" << v->getNumber() << "->" << "p" << vNext->getNumber()
            << " [label=\"" << element->getLabel();
        
        if (options[O_DIAGNOSIS]) {
            os << "\", fontcolor=black, color=" << vNext->getDiagnosisColor().toString();
        } else {
            os << "\", fontcolor=black, color=" << vNext->getColor().toString();
        }

        os << "];\n";
        if ((vNext != v) && !visitedNodes[vNext]) {
            printGraphToDotRecursively(vNext, os, visitedNodes);
        }
    } // while
}





//! \brief creates a STG file of the graph
void CommunicationGraph::printGraphToSTG() {
    trace(TRACE_0, "\ncreating the STG file of the graph...\n");
    GraphNode* rootNode = root;
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
    std::map<GraphNode*, bool> visitedNodes;
//    bool visitedNodes[getNumberOfNodes()];
//    for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
//        visitedNodes[i] = false;
//    }

    // traverse the nodes recursively
    printGraphToSTGRecursively(rootNode, dotFile, visitedNodes);

    // the initial marking
    dotFile << ".marking {p" << rootNode->getNumber() << "}" << endl;

    // end and close file
    dotFile << ".end" << endl;
    dotFile.close();

    // prepare Petrify command line for printing
    if (parameters[P_OG]) {
        buffer = string(HAVE_PETRIFY) + " " + PN->filename + ".OG.stg -dead -ip -o " + PN->filename + ".OG.pn";
    } else {
        buffer = string(HAVE_PETRIFY) + " " + PN->filename + ".IG.stg -dead -ip -o " + PN->filename + ".IG.pn";
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


//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \brief breadthsearch through the graph printing each node and edge to the output stream
void CommunicationGraph::printGraphToSTGRecursively(GraphNode * v, fstream& os, std::map<GraphNode*, bool>& visitedNodes) {
    assert(v != NULL);
    
    if (!v->isToShow(root))
        return;
    
    visitedNodes[v] = true;
    
    // arcs
    GraphNode::LeavingEdges::ConstIterator edgeIter =
        v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        GraphEdge<> *element = edgeIter->getNext();
        GraphNode *vNext = element->getDstNode();
	
        if (!vNext->isToShow(root))
            continue;
	
        string this_edges_label = element->getLabel().substr(1, element->getLabel().size());
        os << "p" << v->getNumber() << " ";
        
        if (element->getLabel().substr(0,1) == "!") {
            os << "out.";
        } else if (element->getLabel().substr(0,1) == "?") {
            os << "in.";
        }
        
        os << this_edges_label;
        os << " p" << vNext->getNumber() << endl;

        if ((vNext != v) && !visitedNodes[vNext]) {
            printGraphToSTGRecursively(vNext, os, visitedNodes);
        }
    }
    delete edgeIter;
}


bool CommunicationGraph::annotateGraphDistributedly() {
    GraphNode* rootNode = root;

    // mark all nodes as unvisited
    std::map<GraphNode*, bool> visitedNodes;
    
    // traverse the nodes recursively
    return annotateGraphDistributedlyRecursively(rootNode, visitedNodes);
}


bool CommunicationGraph::annotateGraphDistributedlyRecursively(GraphNode *v, std::map<GraphNode*, bool>& visitedNodes) {
    assert(v != NULL);
    set<string> disabled, enabled;

    if (!v->isToShow(root))
        return false;    
    
    // save for each state the outgoing labels;
    static map<GraphNode*, set<string> > outgoing_labels;

    // store outgoing lables
    GraphNode::LeavingEdges::ConstIterator edgeIter =
        v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        GraphEdge<> *element = edgeIter->getNext();
        if (element->getDstNode() != NULL &&
            element->getDstNode()->isToShow(root) ) {
            outgoing_labels[v].insert(element->getLabel());
        }
    }
    
    // standard procedurce
    visitedNodes[v] = true;
    
    edgeIter = v->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        GraphEdge<> *element = edgeIter->getNext();
        GraphNode *vNext = element->getDstNode();
 	
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
            enabled = setDifference(setDifference( outgoing_labels[vNext],
                                                   PN->getPort(PN->getPortForLabel(element->getLabel()))),
                                    outgoing_labels[v] );
            
            if (!enabled.empty()) {
                cerr << "  in state " << v->getNumber() << ": " << element->getLabel() <<
                    " enables " << enabled.size() <<
                    " elements: ";

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
            disabled = setDifference(setDifference( outgoing_labels[v],
                                                    PN->getPort(PN->getPortForLabel(element->getLabel()))),
                                     outgoing_labels[vNext] );
                        
            if (!disabled.empty()) {
                cerr << "  in state " << v->getNumber() << ": " << element->getLabel() <<
                    " disables " << disabled.size() <<
                    " elements" << endl;
               
                /*
                for (set<string>::const_iterator label = disabled.begin();
                     label != disabled.end(); label++) {
                    if ( removeLabeledSuccessor(v, *label) ) {
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


void CommunicationGraph::removeLabeledSuccessor(GraphNode *v, std::string label) {
    GraphNode::LeavingEdges::Iterator edgeIter = v->getLeavingEdgesIterator();

    while (edgeIter->hasNext()) {
        GraphEdge<>* element = edgeIter->getNext();

        if (element->getLabel() == label) {
            // GraphNode *vNext = element->getDstNode();
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


void CommunicationGraph::diagnose() {
    cerr << endl << "Diagnosis:" << endl;
    cerr << "  Lass mich Arzt, ich bin durch!" << endl << endl;
    
    GraphNode* rootNode = root;
    
    // mark all nodes as unvisited
    std::map<GraphNode*, bool> visitedNodes;

    diagnose_recursively(rootNode, visitedNodes);
}

GraphNodeDiagnosisColor_enum CommunicationGraph::diagnose_recursively(GraphNode *v, std::map<GraphNode*, bool>& visitedNodes) {
    assert(v != NULL);

    /* DO I NEED THIS?
    if (!v->isToShow(root)) {
        cerr << "skipping node " << v->getNumber() << endl;
        return GraphNodeDiagnosisColor(DIAG_UNSET);
    }
    */
    
    // if color is known already, return it!
    if (v->getDiagnosisColor() != DIAG_UNSET) {
        assert( visitedNodes[v] );
        return v->getDiagnosisColor();
    }
    
    visitedNodes[v] = true;
    

    bool external_deadlock_seen = false;
    bool final_state_seen = false;
    
    // iterate the states to check if there are internal deadlocks or final states
    for (StateSet::const_iterator state = v->reachGraphStateSet.begin();
         state != v->reachGraphStateSet.end(); state++) {
        (*state)->decode(PN);
        
        switch ((*state)->type) {
            case DEADLOCK: {
                // check if state is internal deadlock
                bool internal_deadlock = true;
                
                if (PN->transNrQuasiEnabled > 0) {
                    internal_deadlock = false;
                } else {
                    for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
                        if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
                            internal_deadlock = false;
                            continue;
                        }
                    }
                }

                ///////////////////////////////////////////////
                // CASE 1: NODE HAS INTERNAL DEADLOCK => RED //
                ///////////////////////////////////////////////
                if (internal_deadlock) {
                    v->setDiagnosisColor(DIAG_RED);
                    cerr << "  node " << v->getNumber() << " is " << v->getDiagnosisColor().toString() << " (early)" << endl;
                    return DIAG_RED;        
                }
                
                
                external_deadlock_seen = external_deadlock_seen || !internal_deadlock;
                break;
            }
                
            case FINALSTATE: {
                final_state_seen = true;
                break;
            }
                
            default: break;
        }
    }
    
    
    //////////////////////////////////////////////////////////
    // CASE 2: NODE HAS FINAL STATE AND NO DEADLOCK => BLUE //
    //////////////////////////////////////////////////////////
    if (final_state_seen && !external_deadlock_seen) {
        v->setDiagnosisColor(DIAG_BLUE);
        cerr << "  node " << v->getNumber() << " is " << v->getDiagnosisColor().toString() << " (early)" << endl;
        return DIAG_BLUE;
    }
        
    
    // node color cannot be quickly derived, so the children have to be considered
    set<GraphNodeDiagnosisColor_enum> childrenDiagnosisColors;
    
    //cerr << "  node " << v->getNumber() << ": no quick answer" << endl;
    
    GraphNode::LeavingEdges::ConstIterator edgeIter =
        v->getLeavingEdgesConstIterator();


    while (edgeIter->hasNext()) {
        GraphEdge<> *element = edgeIter->getNext();
        GraphNode *vNext = element->getDstNode();
	
        if (!vNext->isToShow(root))
            continue;
        
        if (vNext != v) {
            childrenDiagnosisColors.insert( diagnose_recursively(vNext, visitedNodes) );
        }
    }
    delete edgeIter;
    
    
    bool red_child = (childrenDiagnosisColors.find(DIAG_RED) != childrenDiagnosisColors.end());
    bool blue_child = (childrenDiagnosisColors.find(DIAG_BLUE) != childrenDiagnosisColors.end());;
    bool orange_child = (childrenDiagnosisColors.find(DIAG_ORANGE) != childrenDiagnosisColors.end());;
    
    
    
    ///////////////////////////////////////////////////////////////////////
    // CASE 3: NODE HAS A FINAL STATE AND AN EXTERNAL DEADLOCK => ORANGE //
    ///////////////////////////////////////////////////////////////////////
    if (final_state_seen && external_deadlock_seen) {
        v->setDiagnosisColor(DIAG_ORANGE);
        cerr << "  node " << v->getNumber() << " is " << v->getDiagnosisColor().toString() << " (ED/FS mix)" << endl;
        return DIAG_ORANGE;
    }
    
    
    /////////////////////////////////////////////////
    // CASE 4: NODE HAS ONLY BLUE CHILDREN => BLUE //
    /////////////////////////////////////////////////
    if (blue_child && !red_child && !orange_child) {
        v->setDiagnosisColor(DIAG_BLUE);
        cerr << "  node " << v->getNumber() << " is " << v->getDiagnosisColor().toString() << " (only blue children)" << endl;
        return DIAG_BLUE;
    }
    
    
    ///////////////////////////////////////////////
    // CASE 5: NODE HAS ONLY RED CHILDREN => RED //
    ///////////////////////////////////////////////
    if (red_child && !orange_child && !blue_child) {
        v->setDiagnosisColor(DIAG_RED);
        cerr << "  node " << v->getNumber() << " is " << v->getDiagnosisColor().toString() << " (only red children)" << endl;
        return DIAG_RED;
    }


    //////////////////////////////////////////////////////
    // CASE 6: NODE HAS NOT ONLY RED CHILDREN => ORANGE //
    //////////////////////////////////////////////////////
    if (red_child && (blue_child || orange_child)) {
        v->setDiagnosisColor(DIAG_ORANGE);
        cerr << "  node " << v->getNumber() << " is " << v->getDiagnosisColor().toString() << " (blue and red children)" << endl;
        return DIAG_ORANGE;
    }


    ////////////////////////////
    // ANYTHING ELSE => GREEN //
    ////////////////////////////
    v->setDiagnosisColor(DIAG_GREEN);
    cerr << "  node " << v->getNumber() << " is " << v->getDiagnosisColor().toString() << endl;
    return DIAG_GREEN;
}
