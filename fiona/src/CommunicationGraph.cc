/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.

 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    CommunicationGraph.cc
 *
 * \brief   common functions for IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */


#include "mynew.h"
#include "state.h"
#include "AnnotatedGraphEdge.h"
#include "GraphFormula.h"
#include "options.h"
#include "debug.h"
#include "CommunicationGraph.h"
#include "config.h"
#include "set_helpers.h"
#include <cassert>


using namespace std;


double global_progress = 0;
int show_progress = 0;


//! \brief constructor
//! \param _PN
CommunicationGraph::CommunicationGraph(oWFN * _PN) {
    PN = _PN;
    tempBinDecision = (binDecision *) 0;
    filename = PN->filename;
}


//! \brief destructor
CommunicationGraph::~CommunicationGraph() {
    TRACE(TRACE_5, "CommunicationGraph::~CommunicationGraph() : start\n");
    TRACE(TRACE_5, "Deleting CommunicationGraph of file " + filename + "\n");

    for (unsigned int i = 0; i < setOfNodes.size(); i++) {
        setOfSortedNodes.erase(setOfNodes[i]);
        delete setOfNodes[i];
    }
    setOfNodes.clear();


    GraphNodeSet::iterator iter;

    for (iter = setOfSortedNodes.begin(); iter != setOfSortedNodes.end(); ++iter) {
      delete *iter;
    }
    setOfSortedNodes.clear();

    if (tempBinDecision) {
        delete tempBinDecision;
    }

    if (PN) {
        delete PN;
    }

    TRACE(TRACE_5, "CommunicationGraph::~CommunicationGraph() : end\n");
}

//! \brief returns the number of nodes
//! \return number of nodes
unsigned int CommunicationGraph::getNumberOfNodes() const {
    return setOfSortedNodes.size();
}


//! \brief calculates the root node of the graph
//!        for IG and OG
void CommunicationGraph::calculateRootNode() {

    TRACE(TRACE_5, "void CommunicationGraph::calculateRootNode(): start\n");

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
        if (parameters[P_SINGLE]) {
            // calc the reachable states from that marking using stubborn set method taking
            // care of deadlocks
            // --> the current state is stored in the node, the states reachable from the current state
            //     are not stored in the node
            PN->calculateReachableStatesStubbornDeadlocks(setOfStatesStubbornTemp, root);
        } else if (parameters[P_REPRESENTATIVE]) {
            // --> store the current state and all "minimal" states in the node
            PN->calculateReducedSetOfReachableStatesStoreInNode(setOfStatesStubbornTemp, root);
        }
    }

    root->setNumber(0);
    root->setName(intToString(0));
    setOfSortedNodes.insert(root);
    // setOfNodes.push_back(root); <-- happens in the buildGraph-functions.

    TRACE(TRACE_5, "void CommunicationGraph::calculateRootNode(): end\n");
}

//! \brief remove a node from the CommunicationGraph
//! \param node node to remove
void CommunicationGraph::removeNode(AnnotatedGraphNode* node) {

    TRACE(TRACE_5, "void CommunicationGraph::removeNode(): start\n");
    assert(node);

    setOfSortedNodes.erase(node);  // only valid if container is a std::set

    for (vector<AnnotatedGraphNode*>::iterator testnode = setOfNodes.begin(); testnode
            != setOfNodes.end(); testnode++) {
        if ((*testnode) == node) {
            setOfNodes.erase(testnode);
            break;
        }
    }
    TRACE(TRACE_5, "void CommunicationGraph::removeNode(): end\n");

}



//! \brief this function uses the find method from the template set
//! \param toFind the AnnotatedGraphNode we are looking for in the graph
//! \return toFind itself or NULL if toFind could not be found
AnnotatedGraphNode* CommunicationGraph::findGraphNodeInSet(AnnotatedGraphNode* toFind) {
    GraphNodeSet::iterator iter = setOfSortedNodes.find(toFind);
    if (iter != setOfSortedNodes.end()) {
        return *iter;
    } else {
        return NULL;
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

    TRACE(TRACE_4, "\t adding ");

    // double2int in per cent = trunc(100*value)
    TRACE(TRACE_4, intToString(int(100 * toAddValue)));
    TRACE(TRACE_4, ",");
    // precision 4 digits after comma = (x * 100 * 1000) mod 1000

    int aftercomma = int(100 * 10000 * toAddValue) % 10000;

    if (aftercomma < 10)
        TRACE(TRACE_4, "0");
    if (aftercomma < 100)
        TRACE(TRACE_4, "0");
    if (aftercomma < 1000)
        TRACE(TRACE_4, "0");

    TRACE(TRACE_4, intToString(aftercomma));

    TRACE(TRACE_4, " to progress\n");

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
#ifndef NDEBUG
        if (debug_level == TRACE_0) {
#endif
            trace( " " + intToString(current_progress) + " ");
#ifndef NDEBUG
        } else {
            trace( "    progress: " + intToString(current_progress) + " %\n");
        }
#endif
        // assigning next progress value to show
        show_progress = current_progress;
    }
}


//! \brief prints the current global progress value depending whether the value
//!        changed significantly and depending on the debug-level set
void CommunicationGraph::printProgressFirst() {

    trace( "    ");

#ifndef NDEBUG
    if (debug_level == TRACE_0) {
#endif
        trace( "progress (in %): 0 ");
#ifndef NDEBUG
    } else {
        trace( "progress: 0 %\n");
    }
#endif
}


//! \brief creates a dot output of the graph, uses "diagnosis|OG|IG of <filename>" as title.
//! \param filenamePrefix a string containing the prefix of the output file name
string CommunicationGraph::createDotFile(string& filenamePrefix) const {

    string title;

    if (parameters[P_DIAGNOSIS]) {
        title = "diagnosis of ";
    } else {
        parameters[P_OG] ? title = "OG of " : title = "IG of ";
    }

    if (PN != 0) {
        title = PN->filename;
    } else {
        title = filenamePrefix.substr(0, filenamePrefix.rfind("."));
    }

    return createDotFile(filenamePrefix, title);

}


//! \brief creates a dot output of the graph, uses the filename as title.
//! \param filenamePrefix a string containing the prefix of the output file name
string CommunicationGraph::createDotFile(string& filenamePrefix, const string& dotFileTitle) const {

    unsigned int maxSizeForDotFile = 5000; // number relevant for .out file

    if (((parameters[P_SHOW_RED_NODES] || parameters[P_SHOW_ALL_NODES]) &&
            (getNumberOfNodes() <= maxSizeForDotFile))
         ||
         ((parameters[P_SHOW_EMPTY_NODE] || parameters[P_SHOW_BLUE_NODES]) &&
         getNumberOfBlueNodes() <= maxSizeForDotFile)) {

        trace( "creating the dot file of the graph...\n");
        AnnotatedGraphNode* rootNode = root;


        string dotFileName = filenamePrefix + ".out";
        fstream dotFile(dotFileName.c_str(), ios_base::out | ios_base::trunc);
        if (!dotFile.good()) {
            dotFile.close();
            TRACE(TRACE_0, "Error: A file error occured. Exit.");
            setExitCode(EC_FILE_ERROR);
        }
        dotFile << "digraph g1 {\n";
        dotFile << "graph [fontname=\"Helvetica\", label=\"";

        dotFile << dotFileTitle;

        dotFile << " (parameters:";
        if (parameters[P_IG] && options[O_CALC_REDUCED_IG]) {
            dotFile << " -r";
        }
        if (parameters[P_IG] && !options[O_CALC_ALL_STATES]) {
            dotFile << " -R";
            if (parameters[P_REPRESENTATIVE]) {
                dotFile << " -p representative";
            } else {
                dotFile << " -p single";
            }
        }
        if (parameters[P_RESPONSIVE]) {
        	dotFile << " -p responsive";
        }
        if (options[O_MESSAGES_MAX]) {
            dotFile << " -m" << intToString(messages_manual);
        }
        if (options[O_READ_EVENTS]) {
        	dotFile << " -E";
        }
        if (options[O_EVENT_USE_MAX]) {
            dotFile << " -e" << intToString(events_manual);
        }
        dotFile << ")";
        dotFile << "\"];\n";
        dotFile << "node [fontname=\"Helvetica\" fontsize=10];\n";
        dotFile << "edge [fontname=\"Helvetica\" fontsize=10];\n";

        if (rootNode == NULL) {
            // print the empty OG...
            dotFile << "p0"
                    << " [label=\"#0\", fontcolor=black, color=red, style=dashed];\n";
        } else {
            std::map<AnnotatedGraphNode*, bool> visitedNodes;

        // filling the file with nodes and edges
        createDotFileRecursively(rootNode, dotFile, visitedNodes);
        }

        dotFile << "}";
        dotFile.close();
        // ... dot file created (.out) //

		return dotFileName;

    } else {
        trace( "graph is too big to create dot file\n");
    }

    return "";
}

//! \brief annotates a given dot output file and stores it to a .dot-File.
//! \param filenamePrefix a string containing the prefix of the output file name
//! \param dotFileName the base dot file
string CommunicationGraph::createAnnotatedDotFile(string& filenamePrefix, string& dotFileName) const {

// Checking the option is not needed anymore since this function has to be called knowingly.
            string annotatedDotFileName = filenamePrefix + ".dot";
            // annotate .dot file
            system((string(CONFIG_DOT) + " -Tdot " + dotFileName + " -o " + annotatedDotFileName).c_str());

            return annotatedDotFileName;
}

//! \brief Calls dot to create an image (png) of an .out-file.
//! \param filenamePrefix The prefix of the annotated dot file, suffix will be added.
//! \param dotFileName The base .out file.
string CommunicationGraph::createPNGFile(string& filenamePrefix, string& dotFileName) const {

    unsigned int maxSizeForPNGFile = 500; // number relevant to generate png


        // prepare dot command line creating the picture
        string imgFileName = filenamePrefix + ".png";
        string dotCmd = "dot -Tpng \"" + dotFileName + "\" -o \"" + imgFileName + "\"";

        // print only, if number of nodes is lower than required
        // if option is set to show all nodes, then we compare the number of all nodes
        // otherwise, we compare the number of blue nodes only
        bool graphToBigForPNG = false;
        string reasonForFail = "";

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
            trace( "graph is too big to create the graphics :( ");
            trace( "(" + reasonForFail + ")\n");
            trace( dotCmd + "\n");
        } else {

            // print commandline and execute system command
            trace( dotCmd + "\n");
            system(dotCmd.c_str());
            return imgFileName;


//            // on windows machines, the png file can be shown per system call
//            if (parameters[P_OG]) {
//                trace( "initiating command to show the graphics...\n");
//                string showCmd = "cmd /c \"start " + imgFileName + "\"";
//                system(showCmd.c_str());
//            }


        }

                return "";


}



//! \brief breadthsearch through the graph printing each node and edge to the output stream
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
void CommunicationGraph::createDotFileRecursively(AnnotatedGraphNode* v,
                                                  fstream& os,
                                                  std::map<AnnotatedGraphNode*, bool>& visitedNodes) const {

    // the given node pointer should never be NULL
    assert(v != NULL);

    // continue only if current node v is to show
    if (!v->isToShow(root, (PN != NULL)))
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
        		// os << "(" << *iter << ") " ;
        		if (parameters[P_RESPONSIVE]) {
        			os << "[" << PN->getCurrentMarkingAsString() << "], SCC:" << (*iter)->lowlink << ", tlowlink: " << (*iter)->tlowlink;
        			// check whether this state is representative of the SCC
        			if ((*iter)->tlowlink == (*iter)->dfs) {
        				// yes, it is
        				os << ", rep.";
        				if ((*iter)->repTSCC) {
        					os << " (TSCC)";
        				}
        			}
        		} else {
        			os << "[" << PN->getCurrentMarkingAsString() << "]";
        		}

                //  os << " (";
                // print the suffix (state type)
//                switch ((*iter)->exactType()) {
//                    case I_DEADLOCK:    os << "iDL";    break;
//                    case E_DEADLOCK:    os << "eDL";    break;
//                    case FINALSTATE:    os << "FS";     break;
//                    case TRANS:         os << "TR";     break;
//                    default:            assert(false);
//                }
//
//                os << ")" << " (parent: " << (*iter)->parent << ")\\n";
                os << "\\n";
            }
        }
    }

    if (parameters[P_OG]) {
        // add annotation to node
        v->getAnnotation()->simplify();
        os << v->getAnnotation()->asString();
    }

    // the diagnosis mode uses different colors
    if (parameters[P_DIAGNOSIS]) {
        os << "\", fontcolor=black, color=" << v->getDiagnosisColor().toString();
    } else {
        os << "\", fontcolor=black, color=" << v->getColor().toString();

        if (v->getColor() == RED) {
            os << ", style=dashed";
        }
    }

    if (v->isFinal()) {
        os << ", peripheries=2";
    }

    os << "];\n";

    visitedNodes[v] = true;

    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();

        if (!vNext->isToShow(root, (PN != NULL))) {
            continue;
        }

        os << "p" << v->getNumber() << "->"
           << "p" << vNext->getNumber()
           << " [label=\"" << element->getLabel();

        // the diagnosis mode uses different colors
        if (parameters[P_DIAGNOSIS]) {
            os << "\", fontcolor=black, color=" << vNext->getDiagnosisColor().toString();
        } else {
            os << "\", fontcolor=black, color=" << vNext->getColor().toString();
        }

        // in diagnosis mode, draw "unenforcable" arcs dashed
        if (parameters[P_DIAGNOSIS] && !v->edgeEnforcable(element)) {
            os << ", style=dashed";
        }

        os << "];\n";
        if ((vNext != v) && !visitedNodes[vNext]) {
            createDotFileRecursively(vNext, os, visitedNodes);
        }
    } // while
    delete edgeIter;
}


//! \brief fills the given sets with the names of the inputs and outputs
//!        needed for public view generation. The interfaces of the public view
//!        and the original netmust be the same, even if some output/inputs are
//!        not used
//! \param inputs set to be filled with the names of all inputs
//! \param outputs set to be filled with the names of all outputs
void CommunicationGraph::returnInterface(set<string>& inputs, set<string>& outputs) {

    // Gather all inputs
    unsigned int inputIterator = 0;
    unsigned int endOfIteration = PN->getInputPlaceCount();
    while (inputIterator < endOfIteration) {
        owfnPlace* currentInputPlace = PN->getInputPlace(inputIterator);
        inputs.insert((currentInputPlace->getLabelForMatching()).erase(0,1));
        inputIterator++;
    }

    //Gather all outputs
    unsigned int outputIterator = 0;
    endOfIteration = PN->getOutputPlaceCount();
    while (outputIterator < endOfIteration) {
        owfnPlace* currentOutputPlace = PN->getOutputPlace(outputIterator);
        outputs.insert((currentOutputPlace->getLabelForMatching()).erase(0,1));
        outputIterator++;
    }
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

    if (!v->isToShow(root, (PN != NULL)))
        return false;

    // save for each state the outgoing labels;
    static map<AnnotatedGraphNode*, set<string> > outgoing_labels;

    // store outgoing lables
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        if (element->getDstNode() != NULL && element->getDstNode()->isToShow(root,(PN != NULL)) ) {
            outgoing_labels[v].insert(element->getLabel());
        }
    }

    // standard procedurce
    visitedNodes[v] = true;

    delete edgeIter;
    edgeIter = v->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();

        if (!vNext->isToShow(root, (PN != NULL)))
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
void CommunicationGraph::removeLabeledSuccessor(AnnotatedGraphNode* v, string label) {

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

//! \brief deletes the corresponding oWFN
void CommunicationGraph::deleteOWFN() {
    if (PN) {
        delete PN;
    }

    PN = NULL;
}

//! \brief Adds a node to the CommunicationGraph. The node is inserted in both sets.
void CommunicationGraph::addNode(AnnotatedGraphNode* toAdd) {
    TRACE(TRACE_5, "void CommunicationGraph::addNode(AnnotatedGraphNode*) : start\n");

    setOfNodes.push_back(toAdd);
    setOfSortedNodes.insert(toAdd);

    TRACE(TRACE_5, "void CommunicationGraph::addNode(AnnotatedGraphNode*) : start\n");

}
