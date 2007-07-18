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
 * \file    communicationGraph.cc
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
#include "graphEdge.h"
#include "GraphFormula.h"
#include "options.h"
#include "debug.h"
#include "binDecision.h"
#include "communicationGraph.h"
#include "fiona.h"
#include "pnapi/petrinet.h"
#include <cassert>

using namespace std;
using PNapi::PetriNet;

double global_progress = 0;
int show_progress = 0;


//! \param _PN
//! \brief constructor
communicationGraph::communicationGraph(oWFN * _PN) :
    root(NULL) {

    PN = _PN;
}


//! \brief destructor
communicationGraph::~communicationGraph() {
    trace(TRACE_5, "communicationGraph::~communicationGraph() : start\n");
    GraphNodeSet::iterator iter;

    for (iter = setOfVertices.begin(); iter != setOfVertices.end(); iter++) {
        delete *iter;
    }

    trace(TRACE_5, "communicationGraph::~communicationGraph() : end\n");
}


//! \return pointer to root
//! \brief returns a pointer to the root node of the graph
GraphNode * communicationGraph::getRoot() const {
    return root;
}


//! \return number of nodes
//! \brief returns the number of nodes of the graph (IG or OG)
unsigned int communicationGraph::getNumberOfNodes() const {
    return setOfVertices.size();
//    return numberOfNodes;
}


//! \brief calculates the root node of the graph
// for OG only
void communicationGraph::calculateRootNode() {

    trace(TRACE_5, "void reachGraph::calculateRootNode(): start\n");

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
    setOfVertices.insert(root);

    trace(TRACE_5, "void reachGraph::calculateRootNode(): end\n");
}


//! \param toAdd the GraphNode we are looking for in the graph
//! \brief this function uses the find method from the template set
GraphNode * communicationGraph::findGraphNodeInSet(GraphNode * toAdd) {

    GraphNodeSet::iterator iter = setOfVertices.find(toAdd);
    if (iter != setOfVertices.end()) {
        return *iter;
    } else {
        return NULL;
    }
}


//! \param toAdd a reference to the GraphNode that is to be added to the graph
//! \param messages the label of the edge between the current GraphNode and the one to be added
//! \param type the type of the edge (SENDING, RECEIVING)
//! \brief adding a new GraphNode/ edge to the graph
//!
//! if the graph already contains nodes, we first search the graph for a node that matches the new node
//! if we did not find a node, we add the new node to the graph (here we add the new node to the
//! successor node list of the current graph and add the current node to the list of predecessor nodes
//! of the new node; after that the current GraphNode becomes to be the new node)
//!
//! if we actually found a node matching the new one, we just create an edge between the current node
//! and the node we have just found, the found one gets the current node as a predecessor node

// for IG
bool communicationGraph::addGraphNode (GraphNode * sourceNode, GraphNode * toAdd, messageMultiSet messages, GraphEdgeType type) {
    trace(TRACE_5, "reachGraph::AddGraphNode (GraphNode * sourceNode, GraphNode * toAdd, messageMultiSet messages, GraphEdgeType type) : start\n");

    if (getNumberOfNodes() == 0) {                // graph contains no nodes at all
        root = toAdd;                           // the given node becomes the root node
        setOfVertices.insert(toAdd);
    } else {
        GraphNode * found = findGraphNodeInSet(toAdd); //findGraphNode(toAdd);

        string label;
        bool comma = false;
        unsigned int offset = 0;

        if (type == RECEIVING) {
            offset = PN->getInputPlaceCount();
        }

        if (found == NULL) {
            // copy the events used from the parent node
            for (unsigned int i = 0; i < (PN->getInputPlaceCount() + PN->getOutputPlaceCount()); i++) {
                toAdd->eventsUsed[i] = sourceNode->eventsUsed[i];
            }
        }

        for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
            if (comma) {
              label += ", ";
            }
            label += PN->getPlace(*iter)->getLabelForCommGraph();
            comma = true;

            unsigned int i = 0;
            if (type == RECEIVING) {
                while (i < PN->getOutputPlaceCount() && PN->getOutputPlace(i)->index != *iter) {
                    i++;
                }
            } else {
                while (i < PN->getInputPlaceCount() && PN->getInputPlace(i)->index != *iter) {
                    i++;
                }
            }
            if (found == 0) {
                toAdd->eventsUsed[offset + i]++;
            } else {
                found->eventsUsed[offset + i]++;
            }
        }

        if (found == NULL) {
            trace(TRACE_1, "\n\t new successor node computed:");

            toAdd->setNumber(getNumberOfNodes());
            GraphEdge* edgeSucc = new GraphEdge(toAdd, label);
            sourceNode->addSuccessorNode(edgeSucc);
            setOfVertices.insert(toAdd);

            trace(TRACE_5, "reachGraph::AddGraphNode (GraphNode * sourceNode, GraphNode * toAdd, messageMultiSet messages, GraphEdgeType type) : end\n");
            return true;
        } else {
            trace(TRACE_1, "\t successor node already known: " + intToString(found->getNumber()) + "\n");

            GraphEdge * edgeSucc = new GraphEdge(found, label);
            sourceNode->addSuccessorNode(edgeSucc);
            delete toAdd;

            trace(TRACE_5, "reachGraph::AddGraphNode (GraphNode * sourceNode, GraphNode * toAdd, messageMultiSet messages, GraphEdgeType type) : end\n");
            return false;
        }
    }

    return false;
}


//! \brief adds the node toAdd to the set of all nodes
//! and copies the eventsUsed array from the sourceNode
// for OG
void communicationGraph::addGraphNode(GraphNode* sourceNode, GraphNode* toAdd) {

    trace(TRACE_5, "reachGraph::AddGraphNode(GraphNode* sourceNode, GraphNode * toAdd, unsigned int label, GraphEdgeType type): start\n");

    assert(getNumberOfNodes() > 0);
    assert(setOfVertices.size() > 0);

    // preparing the new node
    toAdd->setNumber(getNumberOfNodes());
    for (oWFN::Places_t::size_type i = 0; i < (PN->getInputPlaceCount() + PN->getOutputPlaceCount()); i++) {
        toAdd->eventsUsed[i] = sourceNode->eventsUsed[i];
    }
    setOfVertices.insert(toAdd);

    trace(TRACE_5, "reachGraph::AddGraphNode (GraphNode* sourceNode, GraphNode * toAdd, unsigned int label, GraphEdgeType type): end\n");
}


//! adds an SENDING or RECEIVING edge from sourceNode to destNode
//! and adds destNode to the successor list of sourceNode
//! and increases eventsUsed of destNode by one  
// for OG
void communicationGraph::addGraphEdge(GraphNode* sourceNode, GraphNode* destNode, oWFN::Places_t::size_type label, GraphEdgeType type) {

    trace(TRACE_5, "reachGraph::AddGraphEdge(GraphNode* sourceNode, GraphNode* destNode, unsigned int label, GraphEdgeType type): start\n");

    assert(sourceNode != NULL);
    assert(destNode != NULL);

    // preparing the new edge
    string edgeLabel;
    if (type == SENDING) {
        edgeLabel = PN->getInputPlace(label)->getLabelForCommGraph();
        destNode->eventsUsed[label]++;
    } else {
        edgeLabel = PN->getOutputPlace(label)->getLabelForCommGraph();
        destNode->eventsUsed[PN->getInputPlaceCount() + label]++;
    }

    // add a new edge to the new node
    GraphEdge* newEdge = new GraphEdge(destNode, edgeLabel);

    // add the new node to successorNodeList
    sourceNode->addSuccessorNode(newEdge);

    trace(TRACE_5, "reachGraph::AddGraphEdge(GraphNode* sourceNode, GraphNode* destNode, unsigned int label, GraphEdgeType type): end\n");
}


//! \param node the node to be analysed
//! \brief analyses the node and sets its color
void communicationGraph::analyseNode(GraphNode* node) {

    trace(TRACE_5, "communicationGraph::analyseNode(GraphNode* node) : start\n");

    trace(TRACE_3, "\t\t\t analysing node ");
    trace(TRACE_3, intToString(node->getNumber()) + "...\n");

    assert(node->getColor() == BLUE);

    // analyse node by its formula
    GraphNodeColor analysedColor = node->analyseNodeByFormula();
    node->setColor(analysedColor);

    trace(TRACE_5, "communicationGraph::analyseNode(GraphNode* node) : end\n");
}


//! \param input the sending event currently performed
//! \param oldNode the old node carrying the states
//! \param newNode the new node wich is computed
//! \brief for each state of the old node:
//! add input message and build reachability graph and add all states to new node
// for OG only
void communicationGraph::calculateSuccStatesInput(unsigned int input, GraphNode * oldNode, GraphNode * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode * node) : start\n");

    StateSet::iterator iter;              // iterator over the state set's elements

    for (iter = oldNode->reachGraphStateSet.begin();
         iter != oldNode->reachGraphStateSet.end(); iter++) {

        assert(*iter != NULL);
        // get the marking of this state
        (*iter)->decode(PN);

        // test for each marking of current node if message bound k reached
        // then supress new sending event
        if (options[O_MESSAGES_MAX] == true) {      // k-message-bounded set
            if (PN->CurrentMarking[PN->getPlace(input)->index] == messages_manual) {
                // adding input message to state already using full message bound
                trace(TRACE_3, "\t\t\t\t\t adding input event would cause message bound violation\n");
                trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode * node) : end\n");
                newNode->setColor(RED);
                return;
            }
        }

        // asserted: adding input message does not violate message bound
        PN->addInputMessage(input); // add the input message to the current marking

        if (options[O_CALC_ALL_STATES]) {
            PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
        } else {
            PN->setOfStatesTemp.clear();
            PN->visitedStates.clear();
            PN->calculateReachableStatesInputEvent(newNode);       // calc the reachable states from that marking
        }

        if (newNode->getColor() == RED) {
            // a message bound violation occured during computation of reachability graph
            trace(TRACE_3, "\t\t\t\t\t found message bound violation during calculating EG in node\n");
            trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode * node) : end\n");
            return;
        }
    }

    trace(TRACE_3, "\t\t\t\t\t input event added without message bound violation\n");
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode * node) : end\n");
    return;
}


//! \param input (multi) set of input messages
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an input message
// for IG
void communicationGraph::calculateSuccStatesInput(messageMultiSet input, GraphNode * node, GraphNode * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(messageMultiSet input, GraphNode * node, GraphNode * newNode) : start\n");

    PN->setOfStatesTemp.clear();
    PN->visitedStates.clear();

    if (TRACE_2 <= debug_level) {
        for (messageMultiSet::iterator iter1 = input.begin(); iter1 != input.end(); iter1++) {
            trace(TRACE_2, PN->getPlace(*iter1)->name);
            trace(TRACE_2, " ");
        }
        trace(TRACE_2, "\n");
    }

    for (StateSet::iterator iter = node->reachGraphStateSet.begin();
         iter != node->reachGraphStateSet.end();
         iter++) {

        (*iter)->decode(PN);

        // test for each marking of current node if message bound k reached
        // then supress new sending event
        if (options[O_MESSAGES_MAX] == true) {      // k-message-bounded set
            // iterate over the set of input messages
            for (messageMultiSet::iterator iter = input.begin(); iter != input.end(); iter++) {
                if (PN->CurrentMarking[PN->getPlace(*iter)->index] == messages_manual) {
                    // adding input message to state already using full message bound
                    trace(TRACE_3, "\t\t\t\t\t adding input event would cause message bound violation\n");
                    trace(TRACE_3, PN->getPlace(*iter)->name);
                    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode * node) : end\n");
                    return;
                }
            }
        }

        PN->addInputMessage(input);                 // add the input message to the current marking
        if (options[O_CALC_ALL_STATES]) {
            PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
        } else {
            PN->calculateReachableStatesInputEvent(newNode);       // calc the reachable states from that marking
        }
        if (newNode->getColor() == RED) {
            // a message bound violation occured during computation of reachability graph
            trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode * node) : end\n");
            return;
        }
    }

    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(messageMultiSet input, GraphNode * node, GraphNode * newNode) : end\n");
    return;
}


//! \param output the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an output message
// for OG
void communicationGraph::calculateSuccStatesOutput(unsigned int output, GraphNode * node, GraphNode * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, GraphNode * node, GraphNode * newNode) : start\n");

    StateSet::iterator iter;                      // iterator over the stateList's elements

    if (options[O_CALC_ALL_STATES]) {
        for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
            (*iter)->decode(PN);
            if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
                PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
            }
        }
    } else {
        PN->setOfStatesTemp.clear();
        PN->visitedStates.clear();
        owfnPlace * outputPlace = PN->getPlace(output);

        StateSet stateSet;

        for (StateSet::iterator iter = node->reachGraphStateSet.begin();
                iter != node->reachGraphStateSet.end(); iter++) {

            (*iter)->decode(PN);
            // calculate temporary state set with the help of stubborn set method
            PN->calculateReachableStates(stateSet, outputPlace, newNode);
        }

        for (StateSet::iterator iter2 = stateSet.begin(); iter2 != stateSet.end(); iter2++) {
            (*iter2)->decode(PN); // get the marking of the state

            if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
                PN->calculateReachableStatesOutputEvent(newNode);   // calc the reachable states from that marking
            }
        }

        delete PN->tempBinDecision;
        PN->tempBinDecision = NULL;
    }
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, GraphNode * node, GraphNode * newNode) : end\n");
}


//! \param output the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an output message
// for IG
void communicationGraph::calculateSuccStatesOutput(messageMultiSet output, GraphNode * node, GraphNode * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(messageMultiSet output, GraphNode * node, GraphNode * newNode) : start\n");

    if (TRACE_2 <= debug_level) {
        for (messageMultiSet::iterator iter1 = output.begin(); iter1 != output.end(); iter1++) {
            trace(TRACE_2, PN->getPlace(*iter1)->name);
            trace(TRACE_2, " ");
        }
        trace(TRACE_2, "\n");
    }

    if (options[O_CALC_ALL_STATES]) {
        for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
            (*iter)->decode(PN);
            if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
                PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
            }
        }
    } else {
        PN->setOfStatesTemp.clear();
        PN->visitedStates.clear();

        StateSet stateSet;
        // stateSet.clear();

        for (StateSet::iterator iter = node->reachGraphStateSet.begin();
            iter != node->reachGraphStateSet.end(); iter++) {

            (*iter)->decode(PN);
            // calculate temporary state set with the help of stubborn set method
            // TODO: Fix this memory leak.
            // The following method sets tempBinDecision to NULL before
            // filling tempBinDecision anew without deleting the old one.
            // Consequently, some binDecisions become unreachable and
            // cannot be deleted at the end of this method. This produces a
            // memory leak. Not setting tempBinDecision to NULL in the
            // following method call obviously fixes this memory leak. But this
            // would cause unintended behaviour, wouldn't it? I figure that
            // each State in stateSet needs a separate tempBinDecision. Then,
            // each of those tempBinDecision must be kept alive until the
            // following for loop is completed because otherwise the just
            // calculated States would deleted by the binDecision destructor
            // causing a segmentation fault while trying to call decode() on
            // one those deleted states in the following for loop.
            PN->calculateReachableStates(stateSet, output, newNode);
        }

        for (StateSet::iterator iter2 = stateSet.begin(); iter2 != stateSet.end(); iter2++) {
            (*iter2)->decode(PN); // get the marking of the state

            if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
                PN->calculateReachableStatesOutputEvent(newNode);   // calc the reachable states from that marking
            }
        }
        // binDeleteAll(PN->tempBinDecision);
        delete PN->tempBinDecision;
        PN->tempBinDecision = NULL;
    }

    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(messageMultiSet output, GraphNode * node, GraphNode * newNode) : end\n");
}


//! \brief computes number of blue nodes and edges and prints them
void communicationGraph::printNodeStatistics() {

    unsigned int nNodes;
    unsigned int nStoredStates;
    unsigned int nEdges;

    // calculate the number of all (blue and red) nodes and edges
    computeNumberOfNodesEdges(nNodes, nStoredStates, nEdges);

    unsigned int nBlueNodes;
    unsigned int nBlueEdges;

    // calculate the number of blue nodes and edges, that are reachable from
    // the root
    computeNumberOfBlueNodesEdges(nBlueNodes, nBlueEdges);

    trace(TRACE_0, "    number of nodes: " + intToString(nNodes) + "\n");
    trace(TRACE_0, "    number of edges: " + intToString(nEdges) + "\n");
    trace(TRACE_0, "    number of deleted nodes: " + intToString(numberDeletedVertices) + "\n");
    trace(TRACE_0, "    number of blue nodes: " + intToString(nBlueNodes) + "\n");
    trace(TRACE_0, "    number of blue edges: " + intToString(nBlueEdges) + "\n");
    trace(TRACE_0, "    number of states calculated: " + intToString(State::state_count) + "\n");
    trace(TRACE_0, "    number of states stored in nodes: " + intToString(nStoredStates) + "\n");
}


void communicationGraph::computeNumberOfNodesEdges(
    unsigned int& nNodes,
    unsigned int& nStoredStates,
    unsigned int& nEdges) const {

    bool visitedNodes[getNumberOfNodes()];

    for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
        visitedNodes[i] = false;
    }

    nNodes        = 0;
    nStoredStates = 0;
    nEdges        = 0;

    computeNumberOfNodesEdgesHelper(root, visitedNodes, nNodes, nStoredStates,
        nEdges);
}


//! \param v current node in the iteration process
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \brief dfs through the graph computing the number of all (blue and red) nodes
void communicationGraph::computeNumberOfNodesEdgesHelper(
    GraphNode*    v,
    bool          visitedNodes[],
    unsigned int& nNodes,
    unsigned int& nStoredStates,
    unsigned int& nEdges) const {

    assert(v != NULL);

    // counting the current node
    v->resetIteratingSuccNodes();
    visitedNodes[v->getNumber()] = true;

    nStoredStates += v->reachGraphStateSet.size();
    nNodes++;

    // iterating over all successors
    GraphEdge* leavingEdge;

    while ((leavingEdge = v->getNextSuccEdge()) != NULL) {

        GraphNode* vNext = leavingEdge->getDstNode();
        assert(vNext != NULL);

        nEdges++;

        if ((vNext != v) && !visitedNodes[vNext->getNumber()]) {
            computeNumberOfNodesEdgesHelper(vNext, visitedNodes, nNodes,
                nStoredStates, nEdges);
        }
    }
}


void communicationGraph::computeNumberOfBlueNodesEdges(
    unsigned int& nBlueNodes,
    unsigned int& nBlueEdges) const {

    bool visitedNodes[getNumberOfNodes()];

    for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
        visitedNodes[i] = false;
    }

    nBlueNodes = 0;
    nBlueEdges = 0;

    computeNumberOfBlueNodesEdgesHelper(root, visitedNodes, nBlueNodes,
        nBlueEdges);
}


//! \param v current node in the iteration process
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \brief dfs through the graph computing the number of blue nodes
void communicationGraph::computeNumberOfBlueNodesEdgesHelper(
    GraphNode* v,
    bool       visitedNodes[],
    unsigned int& nBlueNodes,
    unsigned int& nBlueEdges) const {

    assert(v != NULL);

    // counting the current node
    visitedNodes[v->getNumber()] = true;

    if (v->getColor() == BLUE &&
        (parameters[P_SHOW_EMPTY_NODE] || v->reachGraphStateSet.size() != 0)) {

        nBlueNodes++;

        v->resetIteratingSuccNodes();

        // iterating over all successors
        GraphEdge* leavingEdge;
        while ((leavingEdge = v->getNextSuccEdge()) != NULL) {

            GraphNode* vNext = leavingEdge->getDstNode();
            assert(vNext != NULL);

            if (vNext->getColor() == BLUE &&
                (parameters[P_SHOW_EMPTY_NODE] || vNext->reachGraphStateSet.size() != 0)) {

                nBlueEdges++;
            }

            if ((vNext != v) && !visitedNodes[vNext->getNumber()]) {
                computeNumberOfBlueNodesEdgesHelper(vNext, visitedNodes,
                    nBlueNodes, nBlueEdges);
            }
        } // while
    }
}


//! \param s the state that is checked for activating output events
//! \brief returns true, if the given state activates at least one output event
bool communicationGraph::stateActivatesOutputEvents(State * s) {
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
void communicationGraph::addProgress(double toAddValue) {

    trace(TRACE_2, "\t adding ");

    // double2int in per cent = trunc(100*value)
    trace(TRACE_2, intToString(int(100 * toAddValue)));
    trace(TRACE_2, ",");
    // precision 4 digits after comma = (x * 100 * 1000) mod 1000

    int aftercomma = int(100 * 10000 * toAddValue) % 10000;

    if (aftercomma <   10) trace(TRACE_2, "0");
    if (aftercomma <  100) trace(TRACE_2, "0");
    if (aftercomma < 1000) trace(TRACE_2, "0");

    trace(TRACE_2, intToString(aftercomma));

    trace(TRACE_2, " to progress\n");

    global_progress += toAddValue;

}


//! \brief prints the current global progress value depending whether the value
//! changed significantly and depending on the debug-level set
void communicationGraph::printProgress() {

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
void communicationGraph::printProgressFirst() {

    trace(TRACE_0, "    ");
//    return;

    if (debug_level == TRACE_0) {
        trace(TRACE_0, "progress (in %): 0 ");
    } else {
        trace(TRACE_0, "progress: 0 %\n");
    }
}


//! \brief creates a dot file of the graph
void communicationGraph::printGraphToDot() {

    // unsigned int maxWritingSize = 1000;
    unsigned int maxPrintingSize = 2000;

    if (true) {
//    if (getNumberOfNodes() <= maxWritingSize) {

        trace(TRACE_0, "creating the dot file of the graph...\n");
        GraphNode * tmp = root;

        char buffer[256];
        if (parameters[P_OG]) {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(buffer, "%s.OG.out", netfile);
            } else {
                sprintf(buffer, "%s.R.OG.out", netfile);
            }
        } else {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(buffer, "%s.IG.out", netfile);
            } else {
                sprintf(buffer, "%s.R.IG.out", netfile);
            }
        }

        fstream dotFile(buffer, ios_base::out | ios_base::trunc);
        dotFile << "digraph g1 {\n";
        dotFile << "graph [fontname=\"Helvetica\", label=\"";
        parameters[P_OG] ? dotFile << "OG of " : dotFile << "IG of ";
        dotFile << netfile;        
        dotFile << " (parameters:";
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

        bool visitedNodes[getNumberOfNodes()];
        for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
            visitedNodes[i] = 0;
        }

        printGraphToDotRecursively(tmp, dotFile, visitedNodes);

        dotFile << "}";
        dotFile.close();

        // prepare dot command line for printing
        if (parameters[P_OG]) {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(buffer, "dot -Tpng \"%s.OG.out\" -o \"%s.OG.png\"", netfile, netfile);
            } else {
                sprintf(buffer, "dot -Tpng \"%s.R.OG.out\" -o \"%s.R.OG.png\"", netfile, netfile);
            }
        } else {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(buffer, "dot -Tpng \"%s.IG.out\" -o \"%s.IG.png\"", netfile, netfile);
            } else {
                sprintf(buffer, "dot -Tpng \"%s.R.IG.out\" -o \"%s.R.IG.png\"", netfile, netfile);
            }
        }

        // print commandline and execute system command
        if (options[O_SHOW_NODES] || getNumberOfNodes() <= maxPrintingSize) {
            // print only, if number of nodes is lower than required
            // if option is set to show all nodes, then we compare the number of all nodes
            // otherwise, we compare the number of blue nodes only
            trace(TRACE_0, string(buffer) + "\n");
            system(buffer);
        } else {
            trace(TRACE_0, "graph is too big to create the graphics :( ");
            trace(TRACE_0, "(greater " + intToString(maxPrintingSize) + ")\n");
//            trace(TRACE_0, intToString(getNumberOfNodes()) + " > " + intToString(maxPrintingSize) + "\n");
            trace(TRACE_0, string(buffer) + "\n");
        }
    } else {
        trace(TRACE_0, "graph is too big to create dot file\n");
    }
    
#ifdef WINDOWSBLABLA
    if (parameters[P_OG]) {
        char buffer[256];
        cout << "initiating command to show the graphics..." << endl;
        sprintf(buffer, "cmd /c \"start %s.OG.png\"", netfile);
        trace(TRACE_0, string(buffer) + "\n");
        system(buffer);
    }
#endif
}


//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \brief breadthsearch through the graph printing each node and edge to the output stream
void communicationGraph::printGraphToDotRecursively(GraphNode * v, fstream& os, bool visitedNodes[]) {

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

    os << "\", fontcolor=black, color=" << v->getColor().toString();
    if (v->getColor() == RED) {
        os << ", style=dashed";
    }
    os << "];\n";

    v->resetIteratingSuccNodes();
    visitedNodes[v->getNumber()] = 1;
    GraphEdge * element;
    string label;

    while ((element = v->getNextSuccEdge()) != NULL) {
        GraphNode * vNext = element->getDstNode();

        if (!vNext->isToShow(root))
            continue;

        os << "p" << v->getNumber() << "->" << "p" << vNext->getNumber()
           << " [label=\"" << element->getLabel()
           << "\", fontcolor=black, color=" << vNext->getColor().toString();

        os << "];\n";
        if ((vNext != v) && !visitedNodes[vNext->getNumber()]) {
            printGraphToDotRecursively(vNext, os, visitedNodes);
        }
    } // while
}





//! \brief creates a STG file of the graph
void communicationGraph::printGraphToSTG() {
    trace(TRACE_0, "creating the STG file of the graph...\n");
    GraphNode *tmp = root;
    
    // set file name
    string buffer = string(netfile);

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
    	if (PN->getPlace(i)->type == INPUT || PN->getPlace(i)->type == OUTPUT)
            dotFile << " " << PN->getPlace(i)->name;
    }
    dotFile << endl;    
    dotFile << ".state graph" << endl;
    
    // mark all nodes as unvisited
    bool visitedNodes[getNumberOfNodes()];
    for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
        visitedNodes[i] = false;
    }
    
    // traverse the nodes recursively
    printGraphToSTGRecursively(tmp, dotFile, visitedNodes);

    // the initial marking
    dotFile << ".marking {p" << tmp->getNumber() << "}" << endl;

    // end and close file
    dotFile << ".end" << endl;
    dotFile.close();

    // prepare Petrify command line for printing
    if (parameters[P_OG]) {
        buffer = "petrify4.1 " + string(netfile) + ".OG.stg -dead -ip -o " + string(netfile) + ".OG.pn";
    } else {
        buffer = "petrify4.1 " + string(netfile) + ".IG.stg -dead -ip -o " + string(netfile) + ".IG.pn";
    }

    // print commandline and execute system command
    trace(TRACE_0, buffer + "\n");

    if (HAVE_PETRIFY == 1) {
        system(buffer.c_str());
    } else {
        trace(TRACE_0, "cannot execute command as Petrify was not found in path\n");
    }
    
    PetriNet *TheNet = new PetriNet();
    std::cerr << TheNet->information() << std::endl;
}





//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \brief breadthsearch through the graph printing each node and edge to the output stream
void communicationGraph::printGraphToSTGRecursively(GraphNode * v, fstream& os, bool visitedNodes[]) {
    assert(v != NULL);
    
    if (!v->isToShow(root))
        return;
    
    v->resetIteratingSuccNodes();
    visitedNodes[v->getNumber()] = true;
    GraphEdge *element;
    
    // arcs
    while ((element = v->getNextSuccEdge()) != NULL) {
        GraphNode * vNext = element->getDstNode();
	
        if (!vNext->isToShow(root))
            continue;
	
        string this_edges_label = element->getLabel().substr(1, element->getLabel().size());
        os << "p" << v->getNumber() << " " << this_edges_label << " p" << vNext->getNumber() << endl;

        if ((vNext != v) && !visitedNodes[vNext->getNumber()]) {
            printGraphToSTGRecursively(vNext, os, visitedNodes);
        }
    }
}

