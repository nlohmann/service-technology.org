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
#include "GraphNode.h"
#include "GraphFormula.h"
#include "binDecision.h"

// #define TRUE 1 in cudd package can interfere with CommGraphLiteral::TRUE.
// So, we undefine TRUE.
#undef TRUE


//! \param _PN
//! \brief constructor
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


void OG::buildGraph() {

    // creates the root node and calculates its reachability graph (set of states)
    calculateRootNode();

    if (options[O_OTF]) {
        bdd->convertRootNode(getRoot());
    }

    // start building up the rest of the graph
    // second parameter means: if finished, 100% of the graph is constructed
    setOfNodes.push_back(getRoot());
    buildGraph(getRoot(), 1);
    correctNodeColorsAndShortenAnnotations();

    computeGraphStatistics();
}


void OG::buildGraph(GraphNode* currentNode, double progress_plus) {

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

    oWFN::Places_t::size_type i = 0;
    PriorityMap pm;
    PriorityMap::KeyType key;

    pm.fill(currentNode->getAnnotation());

    // iterate over all input and output places of the oWFN
    while(!pm.empty())
    {
        key = pm.pop();
        i = key.first;

        trace(TRACE_2, "\t\t  iterating over inputSet of the oWFN\n");
        if(key.second == INPUT) {

            trace(TRACE_2, "\t\t\t    sending event: !");
            trace(TRACE_2, string(PN->getInputPlace(i)->name) + "\n");

            if (PN->getInputPlace(i)->max_occurence < 0 ||
                PN->getInputPlace(i)->max_occurence > currentNode->eventsUsed[i]) {
                // we have to consider this event

                GraphNode* v = new GraphNode();    // create new GraphNode of the graph

                trace(TRACE_5, "\t\t\t\t    calculating successor states\n");
                calculateSuccStatesInput(PN->getPlaceIndex(PN->getInputPlace(i)), currentNode, v);

                if (v->getColor() == RED) {
                    // message bound violation occured during calculateSuccStatesInput
                    trace(TRACE_2, "\t\t\t            event suppressed (message bound violated)\n");

                    currentNode->removeLiteralFromAnnotation(
                        PN->getInputPlace(i)->getLabelForCommGraph());

                    addProgress(your_progress);
                    printProgress();

                    numberDeletedVertices--;      // elsewise deletion of v is counted twice
                    delete v;
                } else {
                    // was the new node v computed before?
                    GraphNode* found = findGraphNodeInSet(v);

                    if (found == NULL) {
                        trace(TRACE_1, "\t computed successor node new\n");
                        // node v is new, so the node as well as the edge to it is added

                        addGraphNode(currentNode, v);
                        addGraphEdge(currentNode, v, i, SENDING);

                        // going down with sending event...
                        buildGraph(v, your_progress);

                        trace(TRACE_1, "\t backtracking to node " + currentNode->getName() + "\n");
                        if (v->getColor() == RED) {
                            currentNode->removeLiteralFromAnnotation(
                                PN->getInputPlace(i)->getLabelForCommGraph());
                        }
                    } else {
                        // node was computed before, so only add a new edge to the old node
                        trace(TRACE_1, "\t computed successor node already known: " + found->getName());
                        trace(TRACE_1, " (color " + toUpper(found->getColor().toString()) + ")");
                        trace(TRACE_1, "\n");

                        // draw a new SENDING edge to the old node
                        string edgeLabel = PN->getInputPlace(i)->getLabelForCommGraph();
                        GraphEdge* newEdge = new GraphEdge(found, edgeLabel);
                        currentNode->addLeavingEdge(newEdge);

                        // Still, if that node was computed red before, the literal
                        // of the edge from currentNode to the old node must be removed
                        // in the annotation of currentNode.
                        if (found->getColor() == RED) {
                            currentNode->removeLiteralFromAnnotation(
                                PN->getInputPlace(i)->getLabelForCommGraph());
                        }
                        delete v;

                        addProgress(your_progress);
                        printProgress();
                    }
                }
            } else {
                trace(TRACE_2, "\t\t\t            event suppressed (max_occurence reached)\n");

                currentNode->removeLiteralFromAnnotation(
                    PN->getInputPlace(i)->getLabelForCommGraph());

                addProgress(your_progress);
                printProgress();
            }
            i++;
        }

        // early checking if the node's annotation cannot be made true
        if (currentNode->getAnnotation()->equals() == FALSE) {
            trace(TRACE_3, "\t\t any further event suppressed (annotation of node ");
            trace(TRACE_3, currentNode->getName() + " is unsatisfiable)\n");
            trace(TRACE_5, "\t\t formula was " + currentNode->getAnnotation()->asString());
            trace(TRACE_3, "\n");
            currentNode->setColor(RED);

            // an dieser stelle alle ?-literale loeschen???

            return;
        }

        if(key.second == OUTPUT) {

            trace(TRACE_2, "\t\t\t  receiving event: ?");
            trace(TRACE_2, string(PN->getOutputPlace(i)->name) + "\n");

            if (PN->getOutputPlace(i)->max_occurence < 0 ||
                PN->getOutputPlace(i)->max_occurence > currentNode->eventsUsed[i + PN->getInputPlaceCount()]) {

                GraphNode* v = new GraphNode();    // create new GraphNode of the graph
                calculateSuccStatesOutput(PN->getPlaceIndex(PN->getOutputPlace(i)), currentNode, v);

                // was the new node computed before?
                GraphNode* found = findGraphNodeInSet(v);

                if (found == NULL) {
                    trace(TRACE_1, "\t computed successor node new\n");
                    // node v is new, so the node as well as the edge to it is added
                    addGraphNode(currentNode, v);
                    addGraphEdge(currentNode, v, i, RECEIVING);

                    // going down with receiving event...
                    // buildGraph(v, your_progress);
                    buildGraph(v, 0);

                    trace(TRACE_1, "\t backtracking to node " + currentNode->getName() + "\n");
                    if (v->getColor() == RED) {
                        currentNode->removeLiteralFromAnnotation(
                            PN->getOutputPlace(i)->getLabelForCommGraph());
                    }
                } else {
                    // node v was computed before, so only add a new edge to the old node
                    trace(TRACE_1, "\t computed successor node already known: " + found->getName());
                    trace(TRACE_1, " (color " + toUpper(found->getColor().toString()) + ")");
                    trace(TRACE_1, "\n");

                    // draw a new RECEIVING edge to the old node
                    string edgeLabel = PN->getOutputPlace(i)->getLabelForCommGraph();
                    GraphEdge* newEdge = new GraphEdge(found, edgeLabel);
                    currentNode->addLeavingEdge(newEdge);

                    // Still, if that node was computed red before, the literal
                    // of the edge from currentNode to the old node must be removed in the
                    // annotation of currentNode.
                    if (found->getColor() == RED) {
                        currentNode->removeLiteralFromAnnotation(edgeLabel);
                    }
                    delete v;

                    // addProgress(your_progress);
                    // printProgress();
                }
            } else {
                trace(TRACE_2, "\t\t\t            event suppressed (max_occurence reached)\n");

                currentNode->removeLiteralFromAnnotation(
                    PN->getOutputPlace(i)->getLabelForCommGraph());

                // addProgress(your_progress);
                // printProgress();
            }

            // check whether annotation is still satisfiable
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
//! and copies the eventsUsed array from the sourceNode
void OG::addGraphNode(GraphNode* sourceNode, GraphNode* toAdd) {

    trace(TRACE_5, "reachGraph::AddGraphNode(GraphNode* sourceNode, GraphNode * toAdd): start\n");

    assert(getNumberOfNodes() > 0);
    assert(setOfSortedNodes.size() > 0);

    // preparing the new node
    toAdd->setNumber(getNumberOfNodes());
    toAdd->setName(intToString(getNumberOfNodes()));

    for (oWFN::Places_t::size_type i = 0;
         i < (PN->getInputPlaceCount() + PN->getOutputPlaceCount()); i++) {
        toAdd->eventsUsed[i] = sourceNode->eventsUsed[i];
    }
    setOfSortedNodes.insert(toAdd);
    setOfNodes.push_back(toAdd);

    trace(TRACE_5, "reachGraph::AddGraphNode (GraphNode* sourceNode, GraphNode * toAdd): end\n");
}


//! adds an SENDING or RECEIVING edge from sourceNode to destNode
//! and adds destNode to the successor list of sourceNode
//! and increases eventsUsed of destNode by one
void OG::addGraphEdge(GraphNode* sourceNode,
                      GraphNode* destNode,
                      oWFN::Places_t::size_type label,
                      GraphEdgeType type) {

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

    // add the edge to the leaving edges list
    sourceNode->addLeavingEdge(newEdge);

    trace(TRACE_5, "reachGraph::AddGraphEdge(GraphNode* sourceNode, GraphNode* destNode, unsigned int label, GraphEdgeType type): end\n");
}


//! \param input the sending event currently performed
//! \param oldNode the old node carrying the states
//! \param newNode the new node wich is computed
//! \brief for each state of the old node:
//! add input message and build reachability graph and add all states to new node
void OG::calculateSuccStatesInput(unsigned int input,
                                  GraphNode* oldNode,
                                  GraphNode* newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode* oldNode, GraphNode* newNode) : start\n");

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
                trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode* oldNode, GraphNode* newNode) : end\n");
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
            trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode* oldNode, GraphNode* newNode) : end\n");
            return;
        }
    }

    trace(TRACE_3, "\t\t\t\t input event added without message bound violation\n");
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, GraphNode* oldNode, GraphNode* newNode) : end\n");
    return;
}


//! \param output the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an output message
void OG::calculateSuccStatesOutput(unsigned int output,
                                   GraphNode* node,
                                   GraphNode* newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, GraphNode* node, GraphNode* newNode) : start\n");

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
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, GraphNode* node, GraphNode* newNode) : end\n");
}


void OG::correctNodeColorsAndShortenAnnotations() {
    // This is a fixpoint operation. Do the following until nothing changes:
    //   1. Turn one blue node that should be red into a red one.

    bool graphChangedInLoop = true;
    bool graphChanged = false;
    while (graphChangedInLoop) {
        graphChangedInLoop = false;

        for (GraphNodeSet::iterator iNode = setOfSortedNodes.begin(); iNode
                != setOfSortedNodes.end(); ++iNode) {

            GraphNode* node = *iNode;
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

        GraphNode* node = *iNode;
        node->removeUnneededLiteralsFromAnnotation();
    }
}


void OG::computeCNF(GraphNode* node) const {

    trace(TRACE_5, "OG::computeCNF(GraphNode * node): start\n");

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

    trace(TRACE_5, "OG::computeCNF(GraphNode * node): end\n");
}


//! \brief converts an OG into its BDD representation
void OG::convertToBdd() {
    trace(TRACE_5, "OG::convertToBdd(): start\n");

    std::map<GraphNode*, bool> visitedNodes;

    bdd->convertRootNode(root);
    bdd->generateRepresentation(root, visitedNodes);
    bdd->reorder((Cudd_ReorderingType)bdd_reordermethod);
    bdd->printMemoryInUse();

    trace(TRACE_5, "OG::convertToBdd(): end\n");
}


//! \brief converts an OG into its BDD representation including the red nodes and the markings of the nodes
void OG::convertToBddFull() {
    trace(TRACE_5, "OG::convertToBddFull(): start\n");

    std::map<GraphNode*, bool> visitedNodes;

    trace(TRACE_0, "\nHIT A KEY TO CONTINUE (convertToBddFull)\n");
    //getchar();
    unsigned int nbrLabels = PN->getInputPlaceCount()
            + PN->getOutputPlaceCount();
    BddRepresentation * testbdd = new BddRepresentation(nbrLabels, (Cudd_ReorderingType)bdd_reordermethod, getNumberOfNodes(), true);
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



PriorityMap::KeyType PriorityMap::pop() {
    KeyType key;
    int min_depth = INT_MAX;
    int max_occ = 0;

    key.first = 0;
    key.second = INTERNAL;

    MapType::iterator i;
    for(i = map.begin(); i != map.end(); i++) {
        if(i->second.first ==  min_depth) {
            if(i->second.second >= max_occ) {
                key.first = i->first.first;
                key.second = i->first.second;
                min_depth = i->second.first;
                max_occ   = i->second.second;
            }
        }
        else if(i->second.first <= min_depth) {
            key.first = i->first.first;
            key.second = i->first.second;
            min_depth = i->second.first;
            max_occ   = i->second.second;
        }
    }
    map.erase(key);

    return key;
}

void PriorityMap::fill(GraphFormulaCNF *annotation) {
    oWFN::Places_t::size_type i;
    KeyType key;

    key.second = INPUT;
    for(i = 0; i < PN->getInputPlaceCount(); i++) {

        key.first = i;
        map[key].first  = INT_MAX;
        map[key].second = 0;

        for(GraphFormulaMultiaryAnd::iterator j = annotation->begin(); j != annotation->end(); j++) {

            GraphFormulaMultiaryOr *clause = dynamic_cast<GraphFormulaMultiaryOr*>(*j);
            for(GraphFormulaMultiaryOr::iterator k = clause->begin(); k != clause->end(); k++) {

                GraphFormulaLiteral *lit = dynamic_cast<GraphFormulaLiteral*>(*k);
                if(lit->asString() == PN->getInputPlace(i)->getLabelForCommGraph()) {
                    map[key].second++;
                    if(clause->size() < map[key].first) map[key].first = clause->size();
                }
            }
        }
    }

    key.second = OUTPUT;
    for(i = 0; i < PN->getOutputPlaceCount(); i++) {

        key.first = i;
        map[key].first  = INT_MAX;
        map[key].second = 0;

        for(GraphFormulaMultiaryAnd::iterator j = annotation->begin(); j != annotation->end(); j++) {

            GraphFormulaMultiaryOr *clause = dynamic_cast<GraphFormulaMultiaryOr*>(*j);
            for(GraphFormulaMultiaryOr::iterator k = clause->begin(); k != clause->end(); k++) {

                GraphFormulaLiteral *lit = dynamic_cast<GraphFormulaLiteral*>(*k);
                if(lit->asString() == PN->getOutputPlace(i)->getLabelForCommGraph()) {
                    map[key].second++;
                    if(clause->size() < map[key].first) map[key].first = clause->size();
                }
            }
        }
    }

}

bool PriorityMap::empty() const {
    return map.empty();
}
