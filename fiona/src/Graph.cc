/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe, Leonard Kern*
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
 * \file    Graph.cc
 *
 * \brief   Implementation of class Graph. See Graph.h for further
 *          information.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include <cassert>
#include <queue>
#include <utility>
#include "Graph.h"

// TRUE and FALSE #defined in cudd package may interfere with
// GraphFormulaLiteral::TRUE and ...::FALSE.
#undef TRUE
#undef FALSE

using namespace std;


Graph::Graph() :
    root(NULL) {
}


Graph::~Graph() {
    for (nodes_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end(); ++node_iter) {

        delete *node_iter;
    }
}


void Graph::addNode(GraphNode* node) {
    setOfNodes.push_back(node);
}


GraphNode* Graph::addNode(const std::string& nodeName,
                          GraphFormula* annotation,
                          GraphNodeColor color) {

    GraphNode* node = new GraphNode(nodeName, annotation, color, setOfNodes.size());
    addNode(node);
    return node;
}


void Graph::addEdge(const std::string& srcName,
                    const std::string& dstNodeName,
                    const std::string& label) {

    GraphNode* src = getNodeWithName(srcName);
    GraphNode* dstNode = getNodeWithName(dstNodeName);

    GraphEdge* transition= new GraphEdge(dstNode,label);
    src->addLeavingEdge(transition);
}


bool Graph::hasNodeWithName(const std::string& nodeName) const {
    return getNodeWithName(nodeName) != NULL;
}


GraphNode* Graph::getNodeWithName(const std::string& nodeName) const {

    for (nodes_const_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end(); ++node_iter) {

        if ((*node_iter)->getName() == nodeName) {
            return *node_iter;
        }
    }

    return NULL;
}


GraphNode* Graph::getRoot() const {
    return root;
}


void Graph::setRoot(GraphNode* newRoot) {
    root = newRoot;
}


void Graph::setRootToNodeWithName(const std::string& nodeName) {
    setRoot(getNodeWithName(nodeName));
}


bool Graph::hasNoRoot() const {
    return getRoot() == NULL;
}


void Graph::removeFalseNodes() {

    trace(TRACE_5, "Graph::removeFalseNodes(): start\n");

    bool nodesHaveChanged = true;

    while (nodesHaveChanged) {
        nodesHaveChanged = false;
        nodes_iterator iNode = setOfNodes.begin();

        while (iNode != setOfNodes.end()) {
            GraphFormulaAssignment* iNodeAssignment = (*iNode)->getAssignment();
            if (!(*iNode)->assignmentSatisfiesAnnotation(*iNodeAssignment)) {

                removeEdgesToNodeFromAllOtherNodes(*iNode);
                if (*iNode == getRoot()) {
                    setRoot(NULL);
                }
                delete *iNode;
                iNode = setOfNodes.erase(iNode);
                nodesHaveChanged = true;
            } else {
                ++iNode;
            }

            delete iNodeAssignment;
        }
    }

    trace(TRACE_5, "Graph::removeFalseNodes(): end\n");
}


void Graph::removeEdgesToNodeFromAllOtherNodes(const GraphNode* nodeToDelete) {

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if (*iNode != nodeToDelete) {
            (*iNode)->removeEdgesToNode(nodeToDelete);
        }
    }
}


void Graph::removeEdgesFromNodeToAllOtherNodes(GraphNode* nodeToDelete) {

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if (*iNode != nodeToDelete) {
            nodeToDelete->removeEdgesToNode(*iNode);
        }
    }
}


//! \brief checks, whether this Graph simulates the given simulant
//! \return true on positive check, otherwise: false
//! \param smallerOG the simulant that should be simulated
bool Graph::simulates(Graph* smallerOG) {
    trace(TRACE_5, "Graph::simulates(Graph *smallerOG): start\n");
    // Simulation is impossible without a simulant.
    if (smallerOG == NULL)
        return false;

    // We need to remember the pairs of nodes we already visited.
    set<pair<GraphNode*, GraphNode*> > visitedNodes;

    // Get things moving...
    bool result = false;
    if (simulatesRecursive(root, smallerOG->getRoot(), visitedNodes)) {
        result = true;
    }

    trace(TRACE_5, "Graph::simulates(Graph *smallerOG): end\n");
    return result;
}


//! \brief checks, whether the part of an Graph below myNode simulates
//         the part of an Graph below simNode
//! \return true on positive check, otherwise: false
//! \param myNode a node in this Graph
//! \param simNode a node in the simulant
//! \param visitedNodes Holds all visited pairs of nodes.
bool Graph::simulatesRecursive(GraphNode *myNode,
                               GraphNode *simNode,
                               set<pair<GraphNode*, GraphNode*> >& visitedNodes) {

    // checking, whether myNode simulates simNode; result is true, iff
    // 1) anno of simNode implies anno of myNode and
    // 2) myNode has each outgoing event of simNode, too

    assert(myNode);
    assert(simNode);

    trace(TRACE_2, "\t checking whether node " + myNode->getName());
    trace(TRACE_2, " simulates node " + simNode->getName() + "\n");

    // If we already visited this pair of nodes, then we're done.
    if (visitedNodes.find(make_pair(myNode, simNode)) != visitedNodes.end()) {
        return true;
    } else {
        visitedNodes.insert(make_pair(myNode, simNode));
    }

    // first we check implication of annotations: simNode -> myNode
    trace(TRACE_3, "\t\t checking annotations...\n");
    GraphFormulaCNF* simNodeAnnotationInCNF = simNode->getAnnotation()->getCNF();
    GraphFormulaCNF* myNodeAnnotationInCNF = myNode->getAnnotation()->getCNF();

    if (simNodeAnnotationInCNF->implies(myNodeAnnotationInCNF)) {
        trace(TRACE_3, "\t\t\t annotations ok\n");
        trace(TRACE_4, "\t\t\t   " + simNode->getAnnotation()->asString() + "\n");
        trace(TRACE_4, "\t\t\t   ->\n");
        trace(TRACE_4, "\t\t\t   " + myNode->getAnnotation()->asString() + "\n");

        delete simNodeAnnotationInCNF;
        delete myNodeAnnotationInCNF;
    } else {
        trace(TRACE_3, "\t\t\t annotation implication false\n");
        trace(TRACE_4, "\t\t\t   " + simNode->getAnnotation()->asString() + "\n");
        trace(TRACE_4, "\t\t\t   -/->\n");
        trace(TRACE_4, "\t\t\t   " + myNode->getAnnotation()->asString() + "\n");

        delete simNodeAnnotationInCNF;
        delete myNodeAnnotationInCNF;

        trace(TRACE_2, "\t\t simulation failed (annotation)\n");

        return false;
    }

    // now we check whether myNode has each outgoing event of simNode
    trace(TRACE_3, "\t\t checking edges...\n");
    GraphNode::LeavingEdges::ConstIterator
            simEdgeIter = simNode->getLeavingEdgesConstIterator();

    while (simEdgeIter->hasNext()) {
        GraphEdge* simEdge = simEdgeIter->getNext();
        trace(TRACE_4, "\t\t\t checking event " + simEdge->getLabel() + "\n");

        GraphEdge* myEdge = myNode->getEdgeWithLabel(simEdge->getLabel());

        if (myEdge == NULL) {
            // simNode has edge which myNode hasn't
            trace(TRACE_2, "\t\t simulation failed (edges)\n");
            delete simEdgeIter;
            return false;
        } else {
            trace(TRACE_4, "\t\t\t event present, going down\n");

            if (!simulatesRecursive(myEdge->getDstNode(),
                                    simEdge->getDstNode(),
                                    visitedNodes)) {
                delete simEdgeIter;
                return false;
            }
        }
    }
    delete simEdgeIter;

    // All checks were successful.
    return true;
}


//! \brief filters the current OG through a given OG in such a way,
//!        that the operand simulates the filter; the current OG is created empty
//!        if such a simulation is not possible
//! \param rhsOG the operator OG
void Graph::filter(Graph *rhsOG) {
    trace(TRACE_5, "Graph::filter(Graph *rhsOG): start\n");

    if (rhsOG == NULL )
        return;

    // we may need a seperated "true" node, so we construct it
    // if we do not need the node, it remains isolated; there shouldn't be conflicts
    GraphFormulaTrue* trueAnnotation= new GraphFormulaTrue();
    GraphNode* trueNode= new GraphNode("_true", trueAnnotation);
    addNode(trueNode);

    //We need to remember the nodes we already visited.
    set<GraphNode*> *VisitedNodes;
    VisitedNodes = new set<GraphNode*>;
    VisitedNodes->insert(trueNode);

    // start the filter construction by top down traversation
    filterRecursive(getRoot(), rhsOG->getRoot(), VisitedNodes);

    trace(TRACE_5, "Graph::filter(Graph *rhsOG): end\n");
}


//! \brief filters the current OG through a given OG below myNode (rhsNode respectively)
//!        in such a way, that the complete OG given as the operand simulates the current OG
//! \param myNode a node in the current OG
//! \param rhsNode a node in the operand
//! \param VisitedNodes a set of Nodes as a reminder of the already visited nodes; starts as empty
void Graph::filterRecursive(GraphNode *myNode,
                            GraphNode *rhsNode,
                            set<GraphNode*> *VisitedNodes) {
    trace(TRACE_5, "Graph::filterRecursive(...): begin\n");

    // nothing to be done
    if (myNode == NULL)
        return;

    // nothing to be done
    if (rhsNode == NULL)
        return;

    // we iterate within the operand, thus look at the rhs
    if (VisitedNodes->find(rhsNode) != VisitedNodes->end()) {
        // if we already visited this node in the operand then we're done
        return;
    } else {
        // otherwise mark node as visited
        VisitedNodes->insert(rhsNode);
    }

    GraphFormulaCNF* rhsNodeAnnotationInCNF = rhsNode->getAnnotation()->getCNF();
    GraphFormulaCNF* myNodeAnnotationInCNF = myNode->getAnnotation()->getCNF();

    // iterate over each outgoing edge of the operand node
    trace(TRACE_5, "Graph::filterRecursive: pre order creation\n");
    GraphNode::LeavingEdges::Iterator
            rhsEdgeIter = rhsNode->getLeavingEdgesIterator();
    while (rhsEdgeIter->hasNext()) {
        GraphEdge* rhsEdge = rhsEdgeIter->getNext();

        GraphEdge* myEdge = myNode->getEdgeWithLabel(rhsEdge->getLabel());
        if (myEdge == NULL) {
            // the operand node has an edge which the current og node doesn't
            if (rhsEdge->getType() == SENDING) {
                // if it is an ! event, we cannot filter it properly
                removeEdgesToNodeFromAllOtherNodes(myNode);
                removeEdgesFromNodeToAllOtherNodes(myNode);
                delete rhsEdgeIter;
                delete rhsNodeAnnotationInCNF;
                delete myNodeAnnotationInCNF;
                return;
            }
            if (rhsEdge->getType() == RECEIVING) {
                // if it is an ? event, we allow this communication, but we won't get final again
                addEdge(myNode->getName(), "_true", rhsEdge->getLabel());
                myNode->removeLiteralFromAnnotationByHiding(rhsEdge->getLabel());
                // rhsNodeAnnotationInCNF->removeLiteral(rhsEdge->getLabel());
            }
        }
    }
    delete rhsEdgeIter;

    trace(TRACE_5, "Graph::filterRecursive: checking annotations\n");
    if (rhsNodeAnnotationInCNF->implies(myNodeAnnotationInCNF)) {
        // implication succesfull ... simulation is possible
        delete rhsNodeAnnotationInCNF;
        delete myNodeAnnotationInCNF;
    } else {
        // implication failed ... we cannot construct a simulation; abort
        removeEdgesToNodeFromAllOtherNodes(myNode);
        removeEdgesFromNodeToAllOtherNodes(myNode);
        delete rhsNodeAnnotationInCNF;
        delete myNodeAnnotationInCNF;
        return;
    }

    // traverse the operand og
    trace(TRACE_5, "Graph::filterRecursive: traverse through OG\n");
    rhsEdgeIter = rhsNode->getLeavingEdgesIterator();
    while (rhsEdgeIter->hasNext()) {
        GraphEdge* rhsEdge = rhsEdgeIter->getNext();

        GraphEdge* myEdge = myNode->getEdgeWithLabel(rhsEdge->getLabel());
        if (myEdge == NULL) {
            // this should not happen! every edge in the operand IS present in the current og
            delete rhsEdgeIter;
            return;
        } else {
            // iterate over the outgoin edges
            filterRecursive(myEdge->getDstNode(), rhsEdge->getDstNode(),
                            VisitedNodes);
        }
    }
    delete rhsEdgeIter;

    // after top down construction of the filter,
    // we backtrack and make sure the filter remains consistent
    trace(TRACE_5, "Graph::filterRecursive: post order creation\n");
    rhsEdgeIter = rhsNode->getLeavingEdgesIterator();
    while (rhsEdgeIter->hasNext()) {
        GraphEdge* rhsEdge = rhsEdgeIter->getNext();

        GraphEdge* myEdge = myNode->getEdgeWithLabel(rhsEdge->getLabel());
        if (myEdge == NULL) {
            if (rhsEdge->getType() == SENDING) {
                removeEdgesToNodeFromAllOtherNodes(myNode);
                removeEdgesFromNodeToAllOtherNodes(myNode);
                delete rhsEdgeIter;
                return;
            }
            if (rhsEdge->getType() == RECEIVING) {
                addEdge(myNode->getName(), "_true", rhsEdge->getLabel());
                myNode->removeLiteralFromAnnotationByHiding(rhsEdge->getLabel());
            }
        }
    }
    delete rhsEdgeIter;

    trace(TRACE_5, "Graph::filterRecursive(...): end\n");
}


//! \brief checks, whether this Graph is acyclic
//! \return true on positive check, otherwise: false
bool Graph::isAcyclic() {
    trace(TRACE_5, "Graph::isAcyclic(...): start\n");

    // Define a set for every Node, that will contain all transitive parent nodes
    map<GraphNode*, set<GraphNode*> > parentNodes;

    // Define a queue for all nodes that still need to be tested and initialize it
    queue<GraphNode*> testNodes;
    testNodes.push(root);
    GraphNode* testNode;

    // While there are still nodes in the queue
    while (!testNodes.empty()) {

        testNode = testNodes.front();
        testNodes.pop();

        // A node counts as a parent node to it self for the purpose of cycles
        parentNodes[testNode].insert(testNode);

        // Iterate all transitions of that node
        GraphNode::LeavingEdges::ConstIterator
                edgeIter = testNode->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            GraphEdge* edge = edgeIter->getNext();
            // If the Node is the source of that transition and if the Destination is a valid node
            if (edge->getDstNode()->getColor() == BLUE) {

                // Return false if an outgoing transition points at a transitive parent node,
                // else add the destination to the queue and update its transitive parent nodes
                if (parentNodes[testNode].find(edge->getDstNode())
                        != parentNodes[testNode].end()) {
                    delete edgeIter;
                    trace(TRACE_5, "Graph::isAcyclic(...): end\n");
                    return false;
                } else {
                    testNodes.push(edge->getDstNode());
                    parentNodes[edge->getDstNode()].insert(parentNodes[testNode].begin(),
                                      parentNodes[testNode].end());
                }
            }
        }
        delete edgeIter;
    }
    trace(TRACE_5, "Graph::isAcyclic(...): end\n");
    return true;
}


//! \brief computes the number of services determined by this OG
//! \return number of Services
unsigned int Graph::numberOfServices() {

    trace(TRACE_5, "Graph::numberOfServices(...): start\n");

    trace(TRACE_1, "Removing false nodes...\n");
    removeFalseNodes();

    if (root == NULL) {
        return 0;
    }

    // define variables that will be used in the recursive function
    map<GraphNode*, list <set<GraphNode*> > > validFollowerCombinations;
    set<GraphNode*> activeNodes;
    map<GraphNode*, unsigned int> followers;
    map<set<GraphNode*>, unsigned int> eliminateRedundantCounting;

    // define variables that will be used in the preprocessing before starting the recursion
    set<string> labels;
    list<GraphFormulaAssignment> assignmentList;
    GraphFormulaAssignment possibleAssignment;
    map<string, GraphEdge*> edges;

    trace(TRACE_2, "Computing true assignments for all nodes\n");
    // Preprocess all nodes of the OG in order to fill the variables needed in the recursion
    for (nodes_t::const_iterator iNode = setOfNodes.begin(); iNode
            != setOfNodes.end(); ++iNode) {

        // reset the temporary variables for every node
        labels.clear();
        assignmentList.clear();
        edges.clear();
        possibleAssignment = GraphFormulaAssignment();

        // get the labels of all outgoing edges, that reach a blue destination
        // save those labels in a set and fill a mapping that allows finding the
        // outgoing edges for a label. (does not work with non-determinism yet)
        trace(TRACE_5, "Collecting labels of outgoing edges for current node\n");
        GraphNode::LeavingEdges::ConstIterator edgeIter =(*iNode)->getLeavingEdgesConstIterator();
        while (edgeIter->hasNext()) {
            GraphEdge* edge = edgeIter->getNext();
            if (edge->getDstNode()->getColor() == BLUE) {
                labels.insert(edge->getLabel());
                edges[edge->getLabel()] = edge;
            }
        }

        // get rid of the iterator
        delete edgeIter;

        // return the number of true assignments for this node's formula and simultaniously
        // fill the given list with those true assignments
        followers[(*iNode)] = processAssignmentsRecursively(labels,
                                                         possibleAssignment,
                                                         (*iNode),
                                                         assignmentList);

        // create a temporary variable for a set of nodes
        set<GraphNode*> followerNodes;

        // for every true assignment in the list a set of nodes will be created. These are the nodes which are
        // reached by outgoing edges of which the labels were true in the assignment. This set is then saved in
        // a map for the currently proceeded node.
        for (list<GraphFormulaAssignment>::iterator
                assignment = assignmentList.begin(); assignment
                != assignmentList.end(); assignment++) {

            followerNodes = set<GraphNode*>();
            for (set<string>::iterator label = labels.begin(); label
                    != labels.end(); label++) {

                if (assignment->get((*label))) {
                    followerNodes.insert(edges[(*label)]->getDstNode());
                }
            }
            validFollowerCombinations[(*iNode)].push_back(followerNodes);
        }
    }

    // initialize the first instance for the recursive function
    activeNodes.insert(root);

    unsigned int number = 0;
    unsigned int instances = 0;

    trace(TRACE_1, "Starting recursive computation of number of Services\n");
    // process Instances recursively
    number = numberOfServicesRecursively(activeNodes, followers,
                                         validFollowerCombinations,
                                         eliminateRedundantCounting, instances);

    if (instances > 100000) {
        trace(TRACE_2, "Valid Number of instances exceeded.\n");
        trace(TRACE_0,
              "The number of services cannot be computed in a reasonable amount of time.\n");
        trace(TRACE_0, "The return value will be set to 0.\n");
        trace(TRACE_5, "Graph::numberOfServices(...): end\n");
        return 0;
    } else {
        trace(TRACE_5, "Graph::numberOfServices(...): end\n");
        return number;
    }
}


//! \brief computes the number of possible services for a finished instance or proceed the active nodes
//! \return number of Services
unsigned int Graph::numberOfServicesRecursively(set<GraphNode*> activeNodes,
                                                map<GraphNode*, unsigned int>& followers,
                                                map<GraphNode*, list<set<GraphNode*> > >& validFollowerCombinations,
                                                map<set<GraphNode*>, unsigned int>& eliminateRedundantCounting,
                                                unsigned int& instances) {
    if (instances % 10000== 0&& instances != 0) {
        if (instances > 100000) {
            return 0;
        }
        trace(TRACE_2, "Processed number of instances: "
                + intToString(instances) + "\n");
    }
    instances++;

    // if an Instance with the same active Nodes has already been computed, use the saved value
    if (eliminateRedundantCounting[activeNodes] != 0) {
        return eliminateRedundantCounting[activeNodes];
    }

    // define needed variables
    unsigned int number = 0;
    list< set<GraphNode*> > oldList;
    list< set<GraphNode*> > newList;
    set<GraphNode*> tempSet;
    bool first = true;
    bool usingNew = true;
    bool finalInstance = true;

    // process all active nodes of this instance
    for (set<GraphNode*>::iterator activeNode = activeNodes.begin(); activeNode
            != activeNodes.end(); activeNode++) {

        // if the active node has no valid outgoing edges, do nothing. If that happens
        // with all active nodes, the finalInstance variable will stay true
        if (followers[(*activeNode)] != 0) {

            finalInstance = false;

            // if this is the first iteration for this node, fill the newList with all combinations
            // of followers of the currently proceeded active Node and continues the loop
            if (first) {

                first = false;
                for (list<set<GraphNode*> >::iterator
                        combination = validFollowerCombinations[(*activeNode)].begin(); combination
                        != validFollowerCombinations[(*activeNode)].end(); combination++) {

                    newList.push_back((*combination));
                }
                usingNew = false;
                continue;
            }

            // the next two blocks work similarly. Either one takes the current list of followerSets
            // as it was left by the last node, produces a new set for every combination of its own following
            // sets and the already existing ones and saves it in the other list. This is executed for every node
            // resulting in a list of all followingSet-tuples of all the active nodes
            if (usingNew) {

                newList.clear();
                for (list<set<GraphNode*> >::iterator
                        oldListSet = oldList.begin(); oldListSet
                        != oldList.end(); oldListSet++) {

                    for (list<set<GraphNode*> >::iterator
                            combination = validFollowerCombinations[(*activeNode)].begin(); combination
                            != validFollowerCombinations[(*activeNode)].end(); combination++) {

                        tempSet = set<GraphNode*>();

                        for (set<GraphNode*>::iterator insertionNode = (*combination).begin(); insertionNode != (*combination).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        for (set<GraphNode*>::iterator insertionNode = (*oldListSet).begin(); insertionNode != (*oldListSet).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        newList.push_back(tempSet);
                    }
                }
                usingNew = false;
            } else {

                oldList.clear();
                for (list<set<GraphNode*> >::iterator
                        newListSet = newList.begin(); newListSet
                        != newList.end(); newListSet++) {

                    for (list<set<GraphNode*> >::iterator
                            combination = validFollowerCombinations[(*activeNode)].begin(); combination
                            != validFollowerCombinations[(*activeNode)].end(); combination++) {

                        tempSet = set<GraphNode*>();

                        for (set<GraphNode*>::iterator insertionNode = (*combination).begin(); insertionNode != (*combination).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        for (set<GraphNode*>::iterator insertionNode = (*newListSet).begin(); insertionNode != (*newListSet).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }
                        oldList.push_back(tempSet);
                    }
                }
                usingNew = true;
            }
        } else {
            bool valid = false;
            for (list<set<GraphNode*> >::iterator
                    combination = validFollowerCombinations[(*activeNode)].begin(); combination
                    != validFollowerCombinations[(*activeNode)].end(); combination++) {
                if ((*combination).empty()) {
                    valid = true;
                }
            }
            if (!valid) {
                eliminateRedundantCounting[activeNodes] = 0;
                return 0;
            }
        }
    }

    // if none of the active nodes had followers this is a finished service of the OG
    if (finalInstance) {
        eliminateRedundantCounting[activeNodes] = 1;
        return 1;
    }

    // if there were sets of following nodes, create a new instance of active nodes for every tuple of
    // following sets and add the results to this instance's number of services
    if (usingNew) {
        for (list<set<GraphNode*> >::iterator oldListSet = oldList.begin(); oldListSet
                != oldList.end(); oldListSet++) {

            number += numberOfServicesRecursively((*oldListSet), followers,
                                                  validFollowerCombinations,
                                                  eliminateRedundantCounting,
                                                  instances);
        }
    } else {
        for (list<set<GraphNode*> >::iterator newListSet = newList.begin(); newListSet
                != newList.end(); newListSet++) {

            number += numberOfServicesRecursively((*newListSet), followers,
                                                  validFollowerCombinations,
                                                  eliminateRedundantCounting,
                                                  instances);
        }
    }

    // as soon as the counting for this set of active nodes is finished, save the number to prevent redundancy
    eliminateRedundantCounting[activeNodes] = number;

    // return the number of services for this instance
    //trace(TRACE_5, "Number returned was: " + intToString(number) + "\n");
    //trace(TRACE_5, "Current Instances are: " + intToString(instances) + "\n");

    return number;
}


//! \brief computes the number of true assignments for the given formula of an OG node and additionally
//!        saves them in an assignmentList for every node. The function works by recursively
//!        computing and checking the powerset of all labels of the node
//! \return number of true Assignments
unsigned int Graph::processAssignmentsRecursively(set<string> labels,
                                                  GraphFormulaAssignment possibleAssignment,
                                                  GraphNode* testNode,
                                                  list<GraphFormulaAssignment>& assignmentList) {

    // If there is no outgoing transition, return immediatly
    if (labels.empty()) {
        possibleAssignment.setToTrue("true");
        possibleAssignment.setToTrue("final");
        if (testNode->assignmentSatisfiesAnnotation(possibleAssignment)) {
            assignmentList.push_back(possibleAssignment);
        }
        return 0;
    }

    // define variables
    unsigned int returnValue = 0;
    unsigned int tempValue = 0;
    string label;
    label = (*labels.begin());
    labels.erase(labels.begin());

    // if this was the last lable ...
    if (labels.empty()) {

        // set it to False
        possibleAssignment.setToFalse(label);
        if (testNode->assignmentSatisfiesAnnotation(possibleAssignment)) {
            // Increase the Number of true Assigments by one and save the assignment if the assignment is true
            returnValue += 1;
            assignmentList.push_back(possibleAssignment);
        }

        // set it to True
        possibleAssignment.setToTrue(label);
        if (testNode->assignmentSatisfiesAnnotation(possibleAssignment)) {
            // increase the Number of true Assigments by one and save the assignment if the assignment is true
            returnValue += 1;
            assignmentList.push_back(possibleAssignment);
        }
        // If this is a label inbetween or at the start
    } else {
        // set it to False
        possibleAssignment.setToFalse(label);
        // count the number of all true assignments which are true following this label being set to false
        tempValue = processAssignmentsRecursively(labels, possibleAssignment,
                                                  testNode, assignmentList);
        // increase the number of true assignments accordingly
        returnValue += tempValue;

        // set it to True
        possibleAssignment.setToTrue(label);
        // count the number of all true assignments which are true following this label being set to true
        tempValue = processAssignmentsRecursively(labels, possibleAssignment,
                                                  testNode, assignmentList);
        // increase the number of true assignments accordingly
        returnValue += tempValue;
    }

    // reinsert the label aftwewards
    labels.insert(labels.begin(), label);

    // return the number of true assignments
    return returnValue;
}


Graph* Graph::product(const Graph* rhs) {
    trace(TRACE_5, "Graph::product(const Graph* rhs): start\n");

    // this will be the product OG
    Graph* productOG = new Graph;

    // If one of both OGs is empty, their product is empty, too.
    if (hasNoRoot() || rhs->hasNoRoot()) {
        return productOG;
    }

    // first we build a new root node that has name and annotation constructed
    // from the root nodes of OG and the rhs OG.
    GraphNode* currentOGNode = this->getRoot();
    GraphNode* currentRhsNode = rhs->getRoot();

    std::string currentName;
    currentName = currentOGNode->getName() + "x"+ currentRhsNode->getName();

    GraphFormulaCNF* currentFormula = createProductAnnotation(currentOGNode,
                                                              currentRhsNode);

    // building the new root node of the product OG
    GraphNode* productNode = new GraphNode(currentName, currentFormula);
    productOG->addNode(productNode);
    productOG->setRoot(productNode);

    // builds the successor nodes of the root nodes of OG and rhs OG
    // therefore, we perform a coordinated dfs through OG and the rhs OG
    buildProductOG(currentOGNode, currentRhsNode, productOG);

    productOG->removeFalseNodes();
    trace(TRACE_5, "Graph::product(const Graph* rhs): end\n");

    return productOG;
}


Graph* Graph::product(const ogs_t& ogs) {
    assert(ogs.size() > 1);

    ogs_t::const_iterator iOG = ogs.begin();
    Graph* firstOG = *iOG++;
    Graph* productOG = firstOG->product(*iOG);

    for (++iOG; iOG != ogs.end(); ++iOG) {
        Graph* oldProductOG = productOG;
        productOG = productOG->product(*iOG);
        delete oldProductOG;
    }

    return productOG;
}


void Graph::buildProductOG(GraphNode* currentOGNode,
                           GraphNode* currentRhsNode,
                           Graph* productOG) {

    trace(
          TRACE_5,
          "Graph::buildProductOG(GraphNode* currentOGNode, GraphNode* currentRhsNode, Graph* productOG): start\n");

    // at this time, the node constructed from currentOGNode and
    // currentRhsNode is already inserted

    assert(productOG->getRoot() != NULL);

    // iterate over all outgoing edges from current node of OG
    std::string currentLabel;
    GraphNode::LeavingEdges::ConstIterator
            edgeIter = currentOGNode->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        GraphEdge* edge = edgeIter->getNext();

        // remember the label of the egde
        currentLabel = edge->getLabel();

        // if the rhs automaton allows this edge
        if (currentRhsNode->hasEdgeWithLabel(currentLabel)) {

            // remember the name of the old node of the product OG
            std::string currentName;
            currentName = currentOGNode->getName() + "x"
                    + currentRhsNode->getName();
            assert(productOG->hasNodeWithName(currentName));

            // compute both successors and recursively call buildProductOG again
            GraphNode* newOGNode;
            newOGNode = currentOGNode->followEdgeWithLabel(currentLabel);

            GraphNode* newRhsNode;
            newRhsNode = currentRhsNode->followEdgeWithLabel(currentLabel);

            // build the new node of the product OG
            // that has name and annotation constructed from current nodes of OG and rhs OG
            std::string newProductName;
            newProductName = newOGNode->getName() + "x"+ newRhsNode->getName();
            // if the node is new, add that node to the OG
            GraphNode* found = productOG->getNodeWithName(newProductName);

            if (found != NULL) {
                // the node was known before, so we just have to add a new edge
                productOG->addEdge(currentName, newProductName, currentLabel);

                trace(
                      TRACE_5,
                      "Graph::buildProductOG(GraphNode* currentOGNode, GraphNode* currentRhsNode, Graph* productOG): end\n");
            } else {
                // we computed a new node, so we add a node and an edge
                // trace(TRACE_0, "adding node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + "\n");

                GraphFormulaCNF
                        * newProductFormula = createProductAnnotation(
                                                                      newOGNode,
                                                                      newRhsNode);

                GraphNode* newProductNode= new GraphNode(newProductName, newProductFormula);

                productOG->addNode(newProductNode);

                // going down recursively
                productOG->addEdge(currentName, newProductName, currentLabel);

                buildProductOG(newOGNode, newRhsNode, productOG);
            }
        }
    }
    delete edgeIter;
    trace(
          TRACE_5,
          "Graph::buildProductOG(GraphNode* currentOGNode, GraphNode* currentRhsNode, Graph* productOG): end\n");
}


GraphFormulaCNF* Graph::createProductAnnotation(const GraphNode* lhs,
                                                const GraphNode* rhs) const {

    GraphFormulaMultiaryAnd* conjunction= new GraphFormulaMultiaryAnd(
            lhs->getAnnotation()->getDeepCopy(),
            rhs->getAnnotation()->getDeepCopy());

    GraphFormulaCNF* cnf = conjunction->getCNF();
    delete conjunction;
    cnf->simplify();
    return cnf;
}


std::string Graph::getProductOGFilePrefix(const ogfiles_t& ogfiles) {
    assert(ogfiles.size() != 0);

    ogfiles_t::const_iterator iOgFile = ogfiles.begin();
    string productFilePrefix = stripOGFileSuffix(*iOgFile);

    for (++iOgFile; iOgFile != ogfiles.end(); ++iOgFile) {
        productFilePrefix += "_X_"
                + stripOGFileSuffix(platform_basename(*iOgFile));
    }

    return productFilePrefix;
}


std::string Graph::stripOGFileSuffix(const std::string& filename) {

    static const string ogFileSuffix = ".og";
    if (filename.substr(filename.size() - ogFileSuffix.size()) == ogFileSuffix) {
        return filename.substr(0, filename.size() - ogFileSuffix.size());
    }

    return filename;
}


void Graph::printDotFile(const std::string& filenamePrefix,
                         const std::string& dotGraphTitle) const {

    trace(TRACE_0, "creating the dot file of the OG...\n");

    string dotFile = filenamePrefix + ".out";
    string pngFile = filenamePrefix + ".png";
    fstream dotFileHandle(dotFile.c_str(), ios_base::out | ios_base::trunc);
    dotFileHandle << "digraph g1 {\n";
    dotFileHandle << "graph [fontname=\"Helvetica\", label=\"";
    dotFileHandle << dotGraphTitle;
    dotFileHandle << "\"];\n";
    dotFileHandle << "node [fontname=\"Helvetica\" fontsize=10];\n";
    dotFileHandle << "edge [fontname=\"Helvetica\" fontsize=10];\n";

    std::map<GraphNode*, bool> visitedNodes;
    printGraphToDot(getRoot(), dotFileHandle, visitedNodes);

    dotFileHandle << "}";
    dotFileHandle.close();

    // prepare dot command line for printing
    string cmd = "dot -Tpng \"" + dotFile + "\" -o \""+ pngFile + "\"";

    // print commandline and execute system command
    trace(TRACE_0, cmd + "\n\n");
    system(cmd.c_str());
}


void Graph::printDotFile(const std::string& filenamePrefix) const {
    printDotFile(filenamePrefix, filenamePrefix);
}


//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes maps nodes to Bools remembering already visited nodes
//! \brief dfs through the graph printing each node and edge to the output stream
void Graph::printGraphToDot(GraphNode* v,
                            fstream& os,
                            std::map<GraphNode*, bool>& visitedNodes) const {

    if (v == NULL) {
        // print the empty OG...
        os << "p0"
                << " [label=\"#0\", fontcolor=black, color=red, style=dashed];\n";
        return;
    }

    if (visitedNodes[v] != true) {
        os << "p"<< v->getName() << " [label=\"# "<< v->getName() << "\\n";
        os << v->getAnnotation()->asString();
        os << "\", fontcolor=black, color=blue];\n";
        visitedNodes[v] = true;

        std::string currentLabel;

        GraphNode::LeavingEdges::ConstIterator
                edgeIter = v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            GraphEdge* edge = edgeIter->getNext();

            // remember the label of the egde
            currentLabel = edge->getLabel();
            GraphNode* successor = v->followEdgeWithLabel(currentLabel);
            assert(successor != NULL);

            os << "p"<< v->getName() << "->"<< "p"<< successor->getName()
                    << " [label=\""<< currentLabel
                    << "\", fontcolor=black, color= blue];\n";
            printGraphToDot(successor, os, visitedNodes);
        }
        delete edgeIter;
    }
}


// A function needed for successful deletion of the graph
void Graph::clearNodeSet() {
    setOfNodes.clear();
}


//! \brief prints all nodes and transitions of an OG to file .og
void Graph::printOGFile(const std::string& filenamePrefix) const {
    fstream ogFile(addOGFileSuffix(filenamePrefix).c_str(), ios_base::out | ios_base::trunc);

    
    cout << "number of nodes: " << setOfNodes.size() << endl;
    
    if (hasNoRoot()) {
        // print file for empty OG
        ogFile << "NODES" << endl
               << "  0 : " << GraphFormulaLiteral::FALSE << " : "
               << GraphNodeColor(RED).toString() << ';' << endl << endl
               << "INITIALNODE" << endl
               << "  0;" << endl << endl
               << "TRANSITIONS" << endl
               << "  ;" << endl;

        ogFile.close();
        return;
    }

    ogFile << "NODES" << endl;
    bool printedFirstNode = false;
    for (nodes_t::const_iterator iNode = setOfNodes.begin();
         iNode != setOfNodes.end(); ++iNode) {

        GraphNode* node = *iNode;
        if (!node->isToShow(root)) {
            continue;
        }

        if (printedFirstNode) {
            ogFile << ',' << endl;
        }

        ogFile << "  " << node->getName() << " : "
               << node->getAnnotationAsString() << " : " << node->getColor().toString();

        printedFirstNode = true;
    }
    ogFile << ';'<< endl << endl;

    ogFile << "INITIALNODE"<< endl;
    assert(getRoot() != NULL);
    ogFile << "  " << getRoot()->getName() << ';' << endl << endl;

    ogFile << "TRANSITIONS" << endl;
    bool printedFirstEdge = false;
    for (nodes_t::const_iterator iNode = setOfNodes.begin();
         iNode != setOfNodes.end(); ++iNode) {

        GraphNode* node = *iNode;
        if (!node->isToShow(root)) {
            continue;
        }

        GraphNode::LeavingEdges::ConstIterator iEdge =
            node->getLeavingEdgesConstIterator();

        while (iEdge->hasNext()) {
            GraphEdge* edge = iEdge->getNext();
            if (!edge->getDstNode()->isToShow(root)) {
                continue;
            }

            if (printedFirstEdge) {
                ogFile << ',' << endl;
            }

            ogFile << "  " << node->getName() << " -> "
                   << edge->getDstNode()->getName() << " : " << edge->getLabel();

            printedFirstEdge = true;
        }
        delete iEdge;
    }
    ogFile << ';' << endl;

    ogFile.close();
}


std::string Graph::addOGFileSuffix(const std::string& filePrefix) {
    return filePrefix + ".og";
}


Graph::TransitionMap Graph::getTransitionMap() {
    trace(TRACE_3, "Graph::getTransitionMap()::begin()\n");
    TransitionMap tm;

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if ((*iNode)->isBlue() && (parameters[P_SHOW_EMPTY_NODE] || (*iNode)->reachGraphStateSet.size() != 0)) {
            GraphNode::LeavingEdges::Iterator iEdge = (*iNode)->getLeavingEdgesIterator();
            while (iEdge->hasNext()) {
                GraphEdge* edge = iEdge->getNext();
                if (edge->getDstNode()->isBlue() && (parameters[P_SHOW_EMPTY_NODE]
                        || edge->getDstNode()->reachGraphStateSet.size() != 0))
                    tm[edge->getLabel()].insert((*iNode)->getName() + "@"+ edge->getLabel() + "@"
                            + edge->getDstNode()->getName());
            }
        }
    }

    trace(TRACE_3, "Graph::getTransitionMap()::end()\n");
    return tm;
}


GraphFormulaCNF *Graph::createCovFormula(TransitionMap tm) {
    trace(TRACE_3, "Graph::createCovFormula(TransitionMap)::begin()\n");

    GraphFormulaCNF *formula= new GraphFormulaCNF;

    for (TransitionMap::iterator i = tm.begin(); i != tm.end(); i++) {
        GraphFormulaMultiaryOr *clause= new GraphFormulaMultiaryOr;
        for (EdgeSet::iterator j = i->second.begin(); j != i->second.end(); j++) {
            GraphFormulaLiteral *literal= new GraphFormulaLiteral(*j);
            clause->addSubFormula(literal);
        }
        formula->addClause(clause);
    }

    trace(TRACE_3, "Graph::createCovFormula(): " + formula->asString() + "\n");
    trace(TRACE_3, "Graph::createCovFormula(TransitionMap)::end()\n");
    return formula;
}


//! \brief removes a node from the Graph
//! \param node node to remove
void Graph::removeNode(GraphNode* node) {
    assert(node);
    // setOfNodes.erase(node);  // only valid if container is a std::set

    for (vector<GraphNode*>::iterator testnode = setOfNodes.begin(); testnode
            != setOfNodes.end(); testnode++) {
        if ((*testnode) == node) {
            setOfNodes.erase(testnode);
            break;
        }
    }

}


//! \brief removes all nodes annotated with true and all their incoming/outgoing transitions
void Graph::removeNodesAnnotatedWithTrue() {
    // iterate over all nodes of the Graph
    for (nodes_iterator node_iter = setOfNodes.begin(); node_iter
            != setOfNodes.end(); ++node_iter) {
        GraphNode* currNode = *node_iter;
        // check whether node is annotated with true
        if (currNode->getAnnotationAsString() == GraphFormulaLiteral::TRUE) {
            // node is annotated with true
            // remove node and all its incoming/outgoing transitions
            removeEdgesToNodeFromAllOtherNodes(currNode);
            removeNode(currNode);
            trace(TRACE_5, "removed node " + currNode->getName()
                    + " (annotation: "+currNode->getAnnotationAsString()
                    + ")\n");
            delete currNode;
        }
    }
}


//! \brief constructs the dual service by toggling all event types of the underlying graph 
//	(SIDE-EFFECT: stores all events into sets by type)
//! \complexity O(n+m), with n nodes and m edges in the given Graph
void Graph::constructDualService() {
    // iterate over all nodes of the Graph
    for (nodes_iterator node_iter = setOfNodes.begin(); node_iter
            != setOfNodes.end(); ++node_iter) {
        GraphNode* currNode = *node_iter;
        // iterate over all leaving edges of the current node
        GraphNode::LeavingEdges::Iterator
                edge_iter = currNode->getLeavingEdgesIterator();
        while (edge_iter->hasNext()) {
            GraphEdge* currEdge = edge_iter->getNext();
            // toggle event type of the current edge
            currEdge->toggleType();
            // store event in the corresponding event set
            switch (currEdge->getType()) {
                case SENDING:
                    sendEvents.insert(currEdge->getLabel());
                    break;
                case RECEIVING:
                    recvEvents.insert(currEdge->getLabel());
                    break;
                case SILENT:
                    // do nothing
                    break;
                default:
                    // should never be reached
                    assert(false);
                    trace(TRACE_0,
                          "discovered an event with unknown type (label was "
                                  +currEdge->getLabel() + ")\n");
                    // do nothing
                    break;
            }
        }
        delete edge_iter;
    }
}


//! \brief applies 1st and 2nd fix to dual service in order to obtain a correct result
//! \complexity O(n*2i*o+m), with n nodes, m edges, i input and o output events
void Graph::fixDualService() {
    // prepare deadlock creation
    GraphNode* deadlock = NULL;
    map<std::string, GraphNode*> deadlockMap;
    unsigned int currNumberOfDeadlocks = 0;
    if (!options[O_PV_MULTIPLE_DEADLOCKS]) {
        // create new deadlock node
        deadlock = new GraphNode("deadlock", new GraphFormulaFixed(false, GraphFormulaLiteral::FALSE));
    }

    // iterate over all nodes of the Graph
    for (nodes_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end(); ++node_iter) {

        GraphNode* currNode = *node_iter;

        // 1st fix: if !a is disabled in q of OG, add ?a->Deadlock in q of dual service
        // preparation: assume all receive events are disabled in current node (dual service)
        set<std::string> disabledRecvEvents = recvEvents;

        // 2nd fix: if ?z is enabled in q of OG, but ?z does not occur in the
        //	annotation of q create a new node without ?z and a tau-transition
        // preparation: assume we don't have to apply the 2nd fix in current node
        bool apply2ndFix = false;
        // preparation: we need to store the leaving edges for the new node somewhere
        set<GraphEdge*> newNodesEdges;
        GraphNode::LeavingEdges::Iterator edge_iter = currNode->getLeavingEdgesIterator();
        while (edge_iter->hasNext()) {
            newNodesEdges.insert(edge_iter->getNext());
        }
        delete edge_iter;

        // preparation: we need to know whether the current node has outgoing
        //   sending transitions, when deciding if it's a final node of the PVSA
        bool hasSendingTransitions = false;

        // iterate over all leaving edges of the current node
        edge_iter = currNode->getLeavingEdgesIterator();
        while (edge_iter->hasNext()) {
            GraphEdge* currEdge = edge_iter->getNext();
            std::string currLabel = currEdge->getLabel();
            std::string currAnnotation = currNode->getAnnotationAsString();
            // act on transition type
            switch (currEdge->getType()) {
                case RECEIVING:
                    // current edge represents a receive event
                    // erase this event from the set of disabled receive events
                    disabledRecvEvents.erase(currLabel);
                    break;
                case SENDING:
                    // current edge represents a send event
                    // we want to look for the current edge's label in the current node's
                    // annotation, so we have to toggle the edge type temporarily
                    currLabel[0] = '?';
                    // check whether the current edge's label occurs in the annotation
                    if (currAnnotation.find(currLabel) == std::string::npos) {
                        // current edge's label does not occur in the annotation
                        // we have to apply the 2nd fix at this node
                        apply2ndFix = true;
                        trace(TRACE_5, "need to apply 2nd fix at node "
                                + currNode->getName() + "\n");
                        trace(TRACE_5, "transition " + currLabel
                                + " does not occur in annotation!\n");
                        // erase current transition from the set of transitions of the new node
                        newNodesEdges.erase(currEdge);
                    }
                    // set the flag for outgoing sending transitions
                    hasSendingTransitions = true;
                    break;
                default:
                    // type is either INTERNAL or unknown
                    // do nothing
                    break;
            }
        }
        delete edge_iter;

        // iterate over all disabled receive events
        // if 1st fix needs to be applied, the set of disabled receive events is not empty
        for (set<std::string>::iterator event_iter = disabledRecvEvents.begin();
             event_iter != disabledRecvEvents.end(); ++event_iter) {

            if (options[O_PV_MULTIPLE_DEADLOCKS]) {
                // O_PV_MULTIPLE_DEADLOCKS is set
                // try to read deadlock node from the mapping event->node
                deadlock = deadlockMap[*event_iter];
                // check whether the deadlock node could be determined
                if (deadlock == NULL) {
                    // deadlock node could not be determined
                    // create new deadlock node
                    deadlock =
                        new GraphNode("deadlock" +
                                      intToString(currNumberOfDeadlocks++),
                                      new GraphFormulaFixed(false,
                                                            GraphFormulaLiteral::FALSE));
                    deadlockMap[*event_iter] = deadlock;
                }
            }
            // create new leaving edge from current node to the deadlock node
            //   labeled with current event
            GraphEdge* disabledEvent= new GraphEdge(deadlock, *event_iter);
            currNode->addLeavingEdge(disabledEvent);
        }

        // check whether the 2nd fix needs to be applied at this node
        if (apply2ndFix) {
            // 2nd fix needs to be applied at this node
            trace(TRACE_5, "applying 2nd fix at node " + currNode->getName()
                    + "\n");
            // create new node
            // as there exists a leaving edge representing a send event in currNode,
            //   currNode cannot be annotated with final (same goes for newNode)
            GraphNode* newNode= new GraphNode(currNode->getName() + "_",
                    new GraphFormulaFixed(true, ""), currNode->getColor());
            trace(TRACE_5, "created new node " + newNode->getName() + "\n");
            // create tau transition from current to new node
            GraphEdge* tauTransition= new GraphEdge(newNode, GraphFormulaLiteral::TAU);
            currNode->addLeavingEdge(tauTransition);
            trace(TRACE_5, "created tau transition from " + currNode->getName()
                    +" to "+ newNode->getName() + "\n");

            // add current node's leaving edges to new node 
            //   except those that don't occur in the annotation
            trace(TRACE_5, "adding leaving edges to new node\n");
            for (set<GraphEdge*>::iterator edge_iter = newNodesEdges.begin(); edge_iter
                    != newNodesEdges.end(); ++edge_iter) {
                // create new leaving edge
                GraphEdge* newEdge= new GraphEdge((*edge_iter)->getDstNode(),
                        (*edge_iter)->getLabel());
                newNode->addLeavingEdge(newEdge);
                trace(TRACE_5, "\tadding edge " + newEdge->getLabel() +" from "
                        + newNode->getName() + " to "+newEdge->getDstNode()->getName() + "\n");
            }
            // add disabled receive events from 1st fix to new node
            for (set<std::string>::iterator
                    event_iter = disabledRecvEvents.begin(); event_iter
                    != disabledRecvEvents.end(); ++event_iter) {
                if (options[O_PV_MULTIPLE_DEADLOCKS]) {
                    // O_PV_MULTIPLE_DEADLOCKS is set
                    // try to read deadlock node from the mapping event->node
                    deadlock = deadlockMap[*event_iter];
                    // deadlock node should have been created already by 1st fix
                    assert(deadlock);
                }
                // create new edge from new node to the deadlock node
                //   labeled with current event
                GraphEdge* disabledEvent= new GraphEdge(deadlock, *event_iter);
                newNode->addLeavingEdge(disabledEvent);
                trace(TRACE_5, "\tadding edge " + disabledEvent->getLabel()
                        +" from "+ newNode->getName() + " to "
                        + deadlock->getName() + "\n");
            }

            trace(TRACE_5, "successfully applied 2nd fix at node "
                    + currNode->getName() + "\n");
        }

        // check whether the current node is annotated with final
        string::size_type loc =(currNode->getAnnotationAsString()).find(GraphFormulaLiteral::FINAL, 0);
        if ((loc != string::npos) && !hasSendingTransitions) {
            // current node is annotated with final and has no sending transitions
            // reduce annotation to final only (mark final state of the automaton)

            GraphFormulaMultiaryOr* myclause= new GraphFormulaMultiaryOr();
            myclause->addSubFormula(new GraphFormulaLiteralFinal());
            currNode->setAnnotation(new GraphFormulaCNF(myclause));
        } else {
            // current node is not annotated with final or has sending transitions
            // remove annotation of the current node
            GraphFormulaMultiaryOr* myclause= new GraphFormulaMultiaryOr();
            myclause->addSubFormula(new GraphFormulaFixed(true, GraphFormulaLiteral::TRUE));
            //            myclause->addSubFormula(new GraphFormulaFixed(true, ""));
            currNode->setAnnotation(new GraphFormulaCNF(myclause));
        }
    }

    // add deadlocks to the node set of the SA
    if (!options[O_PV_MULTIPLE_DEADLOCKS]) {
        this->addNode(deadlock);
    } else {
        map<std::string, GraphNode*>::iterator deadlockIter;
        for (deadlockIter = deadlockMap.begin(); deadlockIter
                != deadlockMap.end(); ++deadlockIter) {
            this->addNode(deadlockIter->second);
        }
    }
}


void Graph::transformToPublicView() {
    removeNodesAnnotatedWithTrue();
    constructDualService();
    fixDualService();

    trace(TRACE_0, "PVSA statistics: \n");
    trace(TRACE_0, "  nodes: " + intToString(setOfNodes.size()) + "\n");
    unsigned int edges = 0;
    for (nodes_iterator nodeIter = setOfNodes.begin();
         nodeIter != setOfNodes.end(); ++nodeIter) {

        edges += (*nodeIter)->getLeavingEdgesCount();
    }
    trace(TRACE_0, "  edges: " + intToString(edges) + "\n");
}

