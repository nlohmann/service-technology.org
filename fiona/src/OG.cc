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
 * \file    OG.cc
 *
 * \brief   functions for Operating Guidelines (OG)
 *
 * \author  responsible: Peter Massuthe <massuthe@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "OG.h"
#include "state.h"
#include "options.h"
#include "debug.h"
#include "owfn.h"
#include "AnnotatedGraphNode.h"
#include "GraphFormula.h"
#include "binDecision.h"

// #define TRUE 1 in cudd package can interfere with CommGraphLiteral::TRUE.
// So, we undefine TRUE.
#undef TRUE


//! \brief constructor
//! \param _PN
OG::OG(oWFN * _PN) :
    CommunicationGraph(_PN) {

    if (options[O_BDD] == true || options[O_OTF]) {
        unsigned int nbrLabels = PN->getInputPlaceCount() + PN->getOutputPlaceCount();
        bdd = new BddRepresentation(nbrLabels, (Cudd_ReorderingType)bdd_reordermethod);
    }
}


//! \brief destructor
OG::~OG() {
    if (options[O_BDD] == true) {
        delete bdd;
    }
}


//! \brief Computes the OG of the associated PN.
void OG::buildGraph() {

    // creates the root node and calculates its reachability graph (set of states)
    calculateRootNode();

    if (options[O_OTF]) {
        bdd->convertRootNode(getRoot());
    }

    setOfNodes.push_back(getRoot());

    // start building up the rest of the graph
    // second parameter means: if finished, 100% of the graph is constructed
    buildGraph(getRoot(), 1);
    correctNodeColorsAndShortenAnnotations();

    computeGraphStatistics();
}


//! \brief Builds the OG of the associated PN recursively starting at
//!        currentNode.
//! \param currentNode Current node of the graph from which the build
//!        algorithm starts.
//! \param progress_plus The additional progress when the subgraph
//!        starting at this node is finished.
//! \pre The states of currentNode have been calculated, currentNode's
//!      number has been set (e.g. by calling currentNode->setNumber), and
//!      setOfVertices contains currentNode. That means, buildGraph can be
//!      called with root, if calculateRootNode() has been called.
void OG::buildGraph(AnnotatedGraphNode* currentNode, double progress_plus) {

    // currentNode is the root of the currently considered subgraph
    // at this point, the states inside currentNode are already computed!

    trace(TRACE_1, "\n-----------------------------------------------------------------\n");
    trace(TRACE_1, "\t current node: ");
    trace(TRACE_1, currentNode->getName() + "\n");
    if (debug_level >= TRACE_2) {
        cout << "\t (" << currentNode << ")" << endl;
    }
    trace(TRACE_2, "\t number of states in node: ");
    trace(TRACE_2, intToString(currentNode->reachGraphStateSet.size()) + "\n");

    if (currentNode->getColor() == RED) {
        // this may happen due to a message bound violation in current node
        // then, function calculateReachableStatesFull sets node color RED
        trace(TRACE_3, "\t\t\t node " + currentNode->getName());
        trace(TRACE_3, " has color RED (message bound violated)\n");
        trace(TRACE_1, "\n-----------------------------------------------------------------\n");

        addProgress(progress_plus);
        printProgress();

        return;
    }

    // get the annotation of the node as CNF formula
    computeCNF(currentNode);
    trace(TRACE_3, "\t first computation of formula yields: ");
    trace(TRACE_3, currentNode->getAnnotation()->asString() + "\n");
    trace(TRACE_1, "\n-----------------------------------------------------------------\n");

//    double your_progress = progress_plus * (1 / double(PN->getInputPlaceCount() + PN->getOutputPlaceCnt()));
    double your_progress = progress_plus * (1 / double(PN->getInputPlaceCount()));

    PriorityMap pm;
    PriorityMap::KeyType currentEvent;

    pm.fill(PN, currentNode->getAnnotation());

    // iterate over all input and output places of the oWFN
    // in the order which is given by PriorityMap pm
    while(!pm.empty()) {

        currentEvent = pm.pop();

        if (currentEvent->getType() == INPUT) {
            trace(TRACE_2, "\t\t\t    sending event: ");
        } else if (currentEvent->getType() == OUTPUT) {
            trace(TRACE_2, "\t\t\t  receiving event: ");
        }
        trace(TRACE_2, currentEvent->getLabelForCommGraph() + "\n");

        if (currentEvent->max_occurence < 0 || 
            (currentEvent->getType() == INPUT &&
             currentEvent->max_occurence > currentNode->eventsUsedInput[PN->getInputPlaceIndex(currentEvent)]) || 
            (currentEvent->getType() == OUTPUT &&
             currentEvent->max_occurence > currentNode->eventsUsedOutput[PN->getOutputPlaceIndex(currentEvent)])) {
            // we have to consider this event

            AnnotatedGraphNode* v = new AnnotatedGraphNode();    // create new AnnotatedGraphNode of the graph

            trace(TRACE_5, "\t\t\t\t    calculating successor states\n");
            if (currentEvent->getType() == INPUT) {
                calculateSuccStatesInput(PN->getPlaceIndex(currentEvent), currentNode, v);
            } else if (currentEvent->getType() == OUTPUT) {
                calculateSuccStatesOutput(PN->getPlaceIndex(currentEvent), currentNode, v);
            }

            if (currentEvent->getType() == INPUT && v->getColor() == RED) {
                // message bound violation occured during calculateSuccStatesInput
                trace(TRACE_2, "\t\t\t            event suppressed (message bound violated)\n");

                currentNode->removeLiteralFromAnnotation(currentEvent->getLabelForCommGraph());

                addProgress(your_progress);
                printProgress();

                numberDeletedVertices--;      // elsewise deletion of v is counted twice
                delete v;
            } else {
                // was the new node v computed before?
                AnnotatedGraphNode* found = findGraphNodeInSet(v);

                if (found == NULL) {
                    trace(TRACE_1, "\t computed successor node new\n");
                    // node v is new, so the node as well as the edge to it is added

                    addGraphNode(currentNode, v);
                    if (currentEvent->getType() == INPUT) {
                        add(currentNode, v, PN->getInputPlaceIndex(currentEvent), SENDING);

                        // going down with sending event ...
                        buildGraph(v, your_progress);
                    } else if (currentEvent->getType() == OUTPUT) {
                        add(currentNode, v, PN->getOutputPlaceIndex(currentEvent), RECEIVING);

                        // going down with receiving event ...
                        buildGraph(v, 0);
                    }

                    trace(TRACE_1, "\t backtracking to node " + currentNode->getName() + "\n");
                    if (v->getColor() == RED) {
                        currentNode->removeLiteralFromAnnotation(currentEvent->getLabelForCommGraph());
                    }
                } else {
                    // node was computed before, so only add a new edge to the old node
                    trace(TRACE_1, "\t computed successor node already known: " + found->getName());
                    trace(TRACE_1, " (color " + toUpper(found->getColor().toString()) + ")");
                    trace(TRACE_1, "\n");

                    // draw a new edge to the old node
                    string edgeLabel = currentEvent->getLabelForCommGraph();
                    AnnotatedGraphEdge* newEdge = new AnnotatedGraphEdge(found, edgeLabel);
                    currentNode->addLeavingEdge(newEdge);

                    // Still, if that node was computed red before, the literal
                    // of the edge from currentNode to the old node must be removed
                    // in the annotation of currentNode.
                    if (found->getColor() == RED) {
                        currentNode->removeLiteralFromAnnotation(currentEvent->getLabelForCommGraph());
                    }
                    delete v;

                    if (currentEvent->getType() == INPUT) {
                        addProgress(your_progress);
                        printProgress();
                    }
                }
            }
        } else {
            trace(TRACE_2, "\t\t\t            event suppressed (max_occurence reached)\n");

            currentNode->removeLiteralFromAnnotation(currentEvent->getLabelForCommGraph());

            if (currentEvent->getType() == INPUT) {
                addProgress(your_progress);
                printProgress();
            }
        }
        
        if (!options[O_DIAGNOSIS]) {
            // do not optimize when trying diagnosis
            if (currentNode->getAnnotation()->equals() == FALSE) {
                currentNode->setColor(RED);
                trace(TRACE_3, "\t\t any further event suppressed (annotation of node ");
                trace(TRACE_3, currentNode->getName() + " is unsatisfiable)\n");
                trace(TRACE_5, "\t\t formula was " + currentNode->getAnnotation()->asString());
                trace(TRACE_3, "\n");
                return;
            }
        }
    }

    // finished iterating over successors
    trace(TRACE_2, "\t\t  no events left...\n");

    if (currentNode->getColor() != RED) {
        currentNode->analyseNode();
    }

    trace(TRACE_3, "\t\t node " + currentNode->getName() + " has color " + toUpper(currentNode->getColor().toString()));
    trace(TRACE_3, " via formula " + currentNode->getAnnotation()->asString() + "\n");

    if (options[O_OTF]) {
        //cout << "currentNode: " << currentNode->getName() << endl;
        bdd->addOrDeleteLeavingEdges(currentNode);
    }
}


//! \brief adds the node toAdd to the set of all nodes
//!        and copies the eventsUsed array from the sourceNode
//! \param sourceNode node to copy eventsUsed from
//! \param toAdd node to be added
void OG::addGraphNode(AnnotatedGraphNode* sourceNode, AnnotatedGraphNode* toAdd) {

    trace(TRACE_5, "reachGraph::AddGraphNode(AnnotatedGraphNode* sourceNode, AnnotatedGraphNode * toAdd): start\n");

    assert(getNumberOfNodes() > 0);
    assert(setOfSortedNodes.size() > 0);

    // preparing the new node
    toAdd->setNumber(getNumberOfNodes());
    toAdd->setName(intToString(getNumberOfNodes()));

    for (oWFN::Places_t::size_type i = 0; i < PN->getInputPlaceCount(); i++ ) {
        toAdd->eventsUsedInput[i] = sourceNode->eventsUsedInput[i];
    }
    
    for (oWFN::Places_t::size_type i = 0; i < PN->getOutputPlaceCount(); i++ ) {
        toAdd->eventsUsedOutput[i] = sourceNode->eventsUsedOutput[i];
    }

    setOfSortedNodes.insert(toAdd);
    setOfNodes.push_back(toAdd);

    trace(TRACE_5, "reachGraph::AddGraphNode (AnnotatedGraphNode* sourceNode, AnnotatedGraphNode * toAdd): end\n");
}


//! \brief adds an SENDING or RECEIVING edge from sourceNode to destNode,
//!        adds destNode to the successor list of sourceNode and
//!        increases eventsUsed of destNode by one
//! \param sourceNode source of the new edge
//! \param destNode target of the new edge
//! \param label label of the new edge
//! \param type type of the new edge
void OG::add(AnnotatedGraphNode* sourceNode,
                      AnnotatedGraphNode* destNode,
                      oWFN::Places_t::size_type label,
                      GraphEdgeType type) {

    trace(TRACE_5, "reachGraph::Add(AnnotatedGraphNode* sourceNode, AnnotatedGraphNode* destNode, unsigned int label, Type type): start\n");

    assert(sourceNode != NULL);
    assert(destNode != NULL);

    // preparing the new edge
    string edgeLabel;
    if (type == SENDING) {
        edgeLabel = PN->getInputPlace(label)->getLabelForCommGraph();
        destNode->eventsUsedInput[label]++;
    } else {
        edgeLabel = PN->getOutputPlace(label)->getLabelForCommGraph();
        destNode->eventsUsedOutput[label]++;
    }

    // add a new edge to the new node
    AnnotatedGraphEdge* newEdge = new AnnotatedGraphEdge(destNode, edgeLabel);

    // add the edge to the leaving edges list
    sourceNode->addLeavingEdge(newEdge);

    trace(TRACE_5, "reachGraph::Add(AnnotatedGraphNode* sourceNode, AnnotatedGraphNode* destNode, unsigned int label, Type type): end\n");
}


//! \brief for each state of the old node: add input message
//!        and build reachability graph and add all states to new node
//! \param input the sending event currently performed
//! \param oldNode the old node carrying the states
//! \param newNode the new node wich is computed
void OG::calculateSuccStatesInput(unsigned int input,
                                  AnnotatedGraphNode* oldNode,
                                  AnnotatedGraphNode* newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, AnnotatedGraphNode* oldNode, AnnotatedGraphNode* newNode) : start\n");

    StateSet::iterator iter; // iterator over the state set's elements

    for (iter = oldNode->reachGraphStateSet.begin();
         iter != oldNode->reachGraphStateSet.end(); iter++) {

        assert(*iter != NULL);
        // get the marking of this state
        (*iter)->decode(PN);

        // test for each marking of current node if message bound k reached
        // then supress new sending event
        if (options[O_MESSAGES_MAX] == true) { // k-message-bounded set
            if (PN->CurrentMarking[PN->getPlaceIndex(PN->getPlace(input))] == messages_manual) {
                // adding input message to state already using full message bound
                trace(TRACE_3, "\t\t\t\t\t adding input event would cause message bound violation\n");
                trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, AnnotatedGraphNode* oldNode, AnnotatedGraphNode* newNode) : end\n");
                newNode->setColor(RED);
                return;
            }
        }

        // asserted: adding input message does not violate message bound
        PN->addInputMessage(input); // add the input message to the current marking

        if (options[O_CALC_ALL_STATES]) {
            PN->calculateReachableStatesFull(newNode); // calc the reachable states from that marking
        } else {
            PN->setOfStatesTemp.clear();
            PN->visitedStates.clear();
            PN->calculateReachableStatesInputEvent(newNode); // calc the reachable states from that marking
        }

        if (newNode->getColor() == RED) {
            // a message bound violation occured during computation of reachability graph
            trace(TRACE_3, "\t\t\t\t found message bound violation during calculating EG in node\n");
            trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, AnnotatedGraphNode* oldNode, AnnotatedGraphNode* newNode) : end\n");
            return;
        }
    }

    trace(TRACE_3, "\t\t\t\t input event added without message bound violation\n");
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, AnnotatedGraphNode* oldNode, AnnotatedGraphNode* newNode) : end\n");
    return;
}


//! \brief calculates the set of successor states in case of an output message
//! \param output the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
void OG::calculateSuccStatesOutput(unsigned int output,
                                   AnnotatedGraphNode* node,
                                   AnnotatedGraphNode* newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, AnnotatedGraphNode* node, AnnotatedGraphNode* newNode) : start\n");

    StateSet::iterator iter; // iterator over the stateList's elements

    if (options[O_CALC_ALL_STATES]) {
        for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter
                != node->reachGraphStateSet.end(); iter++) {
            (*iter)->decode(PN);
            if (PN->removeOutputMessage(output)) { // remove the output message from the current marking
                PN->calculateReachableStatesFull(newNode); // calc the reachable states from that marking
            }
        }
    } else {
        PN->setOfStatesTemp.clear();
        PN->visitedStates.clear();
        owfnPlace * outputPlace = PN->getPlace(output);

        StateSet stateSet;

        for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter
                != node->reachGraphStateSet.end(); iter++) {

            (*iter)->decode(PN);
            // calculate temporary state set with the help of stubborn set method
            PN->calculateReachableStates(stateSet, outputPlace, newNode);
        }

        for (StateSet::iterator iter2 = stateSet.begin(); iter2
                != stateSet.end(); iter2++) {
            (*iter2)->decode(PN); // get the marking of the state

            if (PN->removeOutputMessage(output)) { // remove the output message from the current marking
                PN->calculateReachableStatesOutputEvent(newNode); // calc the reachable states from that marking
            }
        }

        delete PN->tempBinDecision;
        PN->tempBinDecision = NULL;
    }
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, AnnotatedGraphNode* node, AnnotatedGraphNode* newNode) : end\n");
}


//! \brief Turns all blue nodes that should be red into red ones and
//!        simplifies their annotations by removing unneeded literals.
//! \pre OG has been built by buildGraph().
//! \note Niels made this public since he needs it for distributed controllability.
void OG::correctNodeColorsAndShortenAnnotations() {
    // This is a fixpoint operation. Do the following until nothing changes:
    //   1. Turn one blue node that should be red into a red one.

    bool graphChangedInLoop = true;
    bool graphChanged = false;
    while (graphChangedInLoop) {
        graphChangedInLoop = false;

        for (GraphNodeSet::iterator iNode = setOfSortedNodes.begin(); iNode
                != setOfSortedNodes.end(); ++iNode) {

            AnnotatedGraphNode* node = *iNode;
            if (node->getColor() == RED) {
                continue;
            }

            node->analyseNode();
            if (node->getColor() == RED) {
                graphChangedInLoop = true;
                graphChanged = true;
            }
        }
    }

    // Shorten all annotations by removing unneeded literals if any node turned
    // red during the previous color correction process.
    if (!graphChanged) {
        return;
    }

    for (GraphNodeSet::iterator iNode = setOfSortedNodes.begin(); iNode
            != setOfSortedNodes.end(); ++iNode) {

        AnnotatedGraphNode* node = *iNode;
        node->removeUnneededLiteralsFromAnnotation();
    }
}


//! \brief calculates the annotation (CNF) for the node
//! \param node the node for which the annotation is calculated
void OG::computeCNF(AnnotatedGraphNode* node) const {

    trace(TRACE_5, "OG::computeCNF(AnnotatedGraphNode * node): start\n");

    // initially, the annoation is empty (and therefore equivalent to true)
    assert(node->getAnnotation()->equals() == TRUE);
    // assert(node->getAnnotation()->asString() == GraphFormulaLiteral::TRUE);

    StateSet::iterator iter; // iterator over the states of the node

    if (options[O_CALC_ALL_STATES]) {
        // NO state reduction

        // iterate over all states of the node
        for (iter = node->reachGraphStateSet.begin();
             iter != node->reachGraphStateSet.end(); iter++) {

            if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE) {
                // we just consider the maximal states only

                // get the marking of this state
                (*iter)->decodeShowOnly(PN);

                // this clause's first literal
                GraphFormulaMultiaryOr* myclause = new GraphFormulaMultiaryOr();

                // in case of a final state we add special literal "final" to the clause
                if ((*iter)->type == FINALSTATE) {

                    if ((PN->FinalCondition) && (*iter)->cardFireList > 0) {
                        cerr << "\n\t WARNING: found a finalstate which activates a transition";
                        cerr << "\n\t          you shouldn't do this!"<< endl;
                        // transient final states are ignored in annotation, just like
                        // all other transient states
                        delete myclause;
                        continue;
                    } else {
                        node->hasFinalStateInStateSet = true;
                        GraphFormulaLiteral
                                * myliteral = new GraphFormulaLiteralFinal();
                        myclause->addSubFormula(myliteral);
                    }
                }

                // get all input events
                for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
                    GraphFormulaLiteral* myliteral = new GraphFormulaLiteral(PN->getInputPlace(i)->getLabelForCommGraph());
                    myclause->addSubFormula(myliteral);
                }

                // get all activated output events
                for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
                    if (PN->CurrentMarking[PN->getPlaceIndex(PN->getOutputPlace(i))] > 0) {
                        GraphFormulaLiteral
                                * myliteral = new GraphFormulaLiteral(PN->getOutputPlace(i)->getLabelForCommGraph());
                        myclause->addSubFormula(myliteral);
                    }
                }

                node->addClause(myclause);
            }
        }
    } else {
        // WITH state reduction

        // iterate over all states of the node
        for (iter = PN->setOfStatesTemp.begin(); iter
                != PN->setOfStatesTemp.end(); iter++) {

            if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE) {
                // we just consider the maximal states only

                // get the marking of this state
                (*iter)->decodeShowOnly(PN);

                // this clause's first literal
                GraphFormulaMultiaryOr* myclause = new GraphFormulaMultiaryOr();

                // in case of a final state we add special literal "final" to the clause
                if ((*iter)->type == FINALSTATE) {

                    if ((PN->FinalCondition) && (*iter)->cardFireList > 0) {
                        cerr << "\n\t WARNING: found a finalstate which activates a transition";
                        cerr << "\n\t          you shouldn't do this!"<< endl;
                        // transient final states are ignored in annotation, just like
                        // all other transient states
                        delete myclause;
                        continue;
                    } else {
                        node->hasFinalStateInStateSet = true;
                        GraphFormulaLiteral
                                * myliteral = new GraphFormulaLiteralFinal();
                        myclause->addSubFormula(myliteral);
                    }
                }

                // get all the input events
                for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
                    GraphFormulaLiteral* myliteral = new GraphFormulaLiteral(PN->getInputPlace(i)->getLabelForCommGraph());
                    myclause->addSubFormula(myliteral);
                }

                // get the activated output events
                for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
                    if (PN->CurrentMarking[PN->getPlaceIndex(PN->getOutputPlace(i))] > 0) {
                        GraphFormulaLiteral
                                * myliteral = new GraphFormulaLiteral(PN->getOutputPlace(i)->getLabelForCommGraph());
                        myclause->addSubFormula(myliteral);
                    }
                }

                node->addClause(myclause);
            }
        }
    }

    trace(TRACE_5, "OG::computeCNF(AnnotatedGraphNode * node): end\n");
}


//! \brief converts an OG into its BDD representation
void OG::convertToBdd() {
    trace(TRACE_5, "OG::convertToBdd(): start\n");

    std::map<AnnotatedGraphNode*, bool> visitedNodes;

    bdd->convertRootNode(root);
    bdd->generateRepresentation(root, visitedNodes);
    bdd->reorder((Cudd_ReorderingType)bdd_reordermethod);
    bdd->printMemoryInUse();

    trace(TRACE_5, "OG::convertToBdd(): end\n");
}


//! \brief converts an OG into its BDD representation including the red nodes and the markings of the nodes
void OG::convertToBddFull() {
    trace(TRACE_5, "OG::convertToBddFull(): start\n");

    std::map<AnnotatedGraphNode*, bool> visitedNodes;

    trace(TRACE_0, "\nHIT A KEY TO CONTINUE (convertToBddFull)\n");
    //getchar();
    unsigned int nbrLabels = PN->getInputPlaceCount() + PN->getOutputPlaceCount();
    BddRepresentation* testbdd =
        new BddRepresentation(nbrLabels, (Cudd_ReorderingType)bdd_reordermethod, getNumberOfNodes(), true);

    testbdd->convertRootNode(root);
    testbdd->setMaxPlaceBits(root, visitedNodes);

    visitedNodes.clear();
    trace(TRACE_0, "\n");

    testbdd->testSymbRepresentation(root, visitedNodes);
    testbdd->reorder((Cudd_ReorderingType)bdd_reordermethod);
    testbdd->printMemoryInUse();
    testbdd->printDotFile();
    delete testbdd;
    trace(TRACE_5, "OG::convertToBdd(): end\n");
}
