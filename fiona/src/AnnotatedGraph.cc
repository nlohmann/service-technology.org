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
 * \file    AnnotatedGraph.cc
 *
 * \brief   Implementation of class Graph. See AnnotatedGraph.h for further
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
#include "AnnotatedGraph.h"

// TRUE and FALSE #defined in cudd package may interfere with
// GraphFormulaLiteral::TRUE and ...::FALSE.
#undef TRUE
#undef FALSE

using namespace std;


//! \brief a basic constructor of AnnotatedGraph
AnnotatedGraph::AnnotatedGraph(): root(NULL), covConstraint(NULL) {
}


//! \brief a basic destructor of AnnotatedGraph
AnnotatedGraph::~AnnotatedGraph() {
	trace(TRACE_5, "AnnotatedGraph::~AnnotatedGraph() : start\n");
    trace(TRACE_5, "AnnotatedGraph::~AnnotatedGraph() : end\n");
}


//! \brief add an already existing Node to the Graphs node set
//! \param node a pointer to a AnnotatedGraphNode
void AnnotatedGraph::addNode(AnnotatedGraphNode* node) {
    setOfNodes.push_back(node);
}


//! \brief create a new node and add it to the graphs node set
//! \param nodeName a string containing the name of the new node
//! \param GraphFormula a pointer to a GraphFormula being the annotation of the new node
//! \param color color of the node
//! \return returns a pointer to the created AnnotatedGraphNode
AnnotatedGraphNode* AnnotatedGraph::addNode(const std::string& nodeName,
                          GraphFormula* annotation,
                          GraphNodeColor color) {

    AnnotatedGraphNode* node = new AnnotatedGraphNode(nodeName, annotation, color, setOfNodes.size());
    addNode(node);
    return node;
}


//! \brief create a new edge in the graph
//! \param srcName a string containing the name of the source node
//! \param dstNodeName a string containing the name of the destination node
//! \param label a string containing the label of the edge
void AnnotatedGraph::addEdge(const std::string& srcName,
                    const std::string& dstNodeName,
                    const std::string& label) {

    AnnotatedGraphNode* src = getNodeWithName(srcName);
    AnnotatedGraphNode* dstNode = getNodeWithName(dstNodeName);

    AnnotatedGraphEdge* transition= new AnnotatedGraphEdge(dstNode,label);
    src->addLeavingEdge(transition);
}


//! \brief checks if the graph has a node with the given name
//! \param nodeName the name to be matched
//! \return returns true if a node with the given name exists, else false
bool AnnotatedGraph::hasNodeWithName(const std::string& nodeName) const {
    return getNodeWithName(nodeName) != NULL;
}


//! \brief returns a pointer to the node that matches a given name, or NULL else
//! \param nodeName the name to be matched
//! \return returns a pointer to the found node or NULL
AnnotatedGraphNode* AnnotatedGraph::getNodeWithName(const std::string& nodeName) const {
    
    for (nodes_const_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end(); ++node_iter) {
            if ((*node_iter)->getName() == nodeName) {
                return *node_iter;
        }
    }

    return NULL;
}


//! \brief returns a AnnotatedGraphNode pointer to the root node
//! \return returns a pointer to the root node
AnnotatedGraphNode* AnnotatedGraph::getRoot() const {
    return root;
}


//! \brief sets the root node of the graph to a given node
//! \param newRoot a pointer to the node to become the new root
void AnnotatedGraph::setRoot(AnnotatedGraphNode* newRoot) {
    root = newRoot;
}


//! \brief sets the root node of the graph to one matching the given name
//! \param nodeName a string containing the name of the node to become the new root
void AnnotatedGraph::setRootToNodeWithName(const std::string& nodeName) {
    setRoot(getNodeWithName(nodeName));
}


//! \brief checks wether a root node is set
//! \return returns true if the rootnode is NULL, else false
bool AnnotatedGraph::hasNoRoot() const {
    return getRoot() == NULL;
}


//! \brief removes all nodes that have annotations that cannot become
//!        true. The function continues removing until no node fullfils
//!        the mentioned criterion
void AnnotatedGraph::removeFalseNodes() {

    trace(TRACE_5, "AnnotatedGraph::removeFalseNodes(): start\n");

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

    trace(TRACE_5, "AnnotatedGraph::removeFalseNodes(): end\n");
}


//! \brief removes all edges that have a given node as destination
//! \param nodeToDelete a pointer to the node that will be deleted
void AnnotatedGraph::removeEdgesToNodeFromAllOtherNodes(const AnnotatedGraphNode* nodeToDelete) {

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if (*iNode != nodeToDelete) {
            (*iNode)->removeEdgesToNode(nodeToDelete);
        }
    }
}


//! \brief removes all edges that have a given node as source
//! \param nodeToDelete a pointer to the node that will be deleted
void AnnotatedGraph::removeEdgesFromNodeToAllOtherNodes(AnnotatedGraphNode* nodeToDelete) {

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if (*iNode != nodeToDelete) {
            nodeToDelete->removeEdgesToNode(*iNode);
        }
    }
}


//! \brief checks, whether this AnnotatedGraph simulates the given simulant
//! \return true on positive check, otherwise: false
//! \param smallerOG the simulant that should be simulated
bool AnnotatedGraph::simulates(AnnotatedGraph* smallerOG) {
    trace(TRACE_5, "AnnotatedGraph::simulates(AnnotatedGraph *smallerOG): start\n");
    // Simulation is impossible without a simulant.
    if (smallerOG == NULL)
        return false;

    // We need to remember the pairs of nodes we already visited.
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > visitedNodes;

    // Get things moving...
    bool result = false;
    if (simulatesRecursive(root, smallerOG->getRoot(), visitedNodes)) {
        result = true;
    }

    trace(TRACE_5, "AnnotatedGraph::simulates(AnnotatedGraph *smallerOG): end\n");
    return result;
}


//! \brief checks, whether the part of an AnnotatedGraph below myNode simulates
//         the part of an AnnotatedGraph below simNode
//! \return true on positive check, otherwise: false
//! \param myNode a node in this AnnotatedGraph
//! \param simNode a node in the simulant
//! \param visitedNodes Holds all visited pairs of nodes.
bool AnnotatedGraph::simulatesRecursive(AnnotatedGraphNode *myNode,
                               AnnotatedGraphNode *simNode,
                               set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> >& visitedNodes) {

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
    AnnotatedGraphNode::LeavingEdges::ConstIterator
            simEdgeIter = simNode->getLeavingEdgesConstIterator();

    while (simEdgeIter->hasNext()) {
        AnnotatedGraphEdge* simEdge = simEdgeIter->getNext();
        trace(TRACE_4, "\t\t\t checking event " + simEdge->getLabel() + "\n");

        AnnotatedGraphEdge* myEdge = myNode->getEdgeWithLabel(simEdge->getLabel());

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
void AnnotatedGraph::filter(AnnotatedGraph *rhsOG) {
    trace(TRACE_5, "AnnotatedGraph::filter(AnnotatedGraph *rhsOG): start\n");

    if (rhsOG == NULL )
        return;

    // we may need a seperated "true" node, so we construct it
    // if we do not need the node, it remains isolated; there shouldn't be conflicts
    GraphFormulaTrue* trueAnnotation= new GraphFormulaTrue();
    AnnotatedGraphNode* trueNode= new AnnotatedGraphNode("_true", trueAnnotation);
    addNode(trueNode);

    //We need to remember the nodes we already visited.
    set<AnnotatedGraphNode*> *VisitedNodes;
    VisitedNodes = new set<AnnotatedGraphNode*>;
    VisitedNodes->insert(trueNode);

    // start the filter construction by top down traversation
    filterRecursive(getRoot(), rhsOG->getRoot(), VisitedNodes);

    trace(TRACE_5, "AnnotatedGraph::filter(AnnotatedGraph *rhsOG): end\n");
}


//! \brief filters the current OG through a given OG below myNode (rhsNode respectively)
//!        in such a way, that the complete OG given as the operand simulates the current OG
//! \param myNode a node in the current OG
//! \param rhsNode a node in the operand
//! \param VisitedNodes a set of Nodes as a reminder of the already visited nodes; starts as empty
void AnnotatedGraph::filterRecursive(AnnotatedGraphNode *myNode,
                            AnnotatedGraphNode *rhsNode,
                            set<AnnotatedGraphNode*> *VisitedNodes) {
    trace(TRACE_5, "AnnotatedGraph::filterRecursive(...): begin\n");

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
    trace(TRACE_5, "AnnotatedGraph::filterRecursive: pre order creation\n");
    AnnotatedGraphNode::LeavingEdges::Iterator
            rhsEdgeIter = rhsNode->getLeavingEdgesIterator();
    while (rhsEdgeIter->hasNext()) {
        AnnotatedGraphEdge* rhsEdge = rhsEdgeIter->getNext();

        AnnotatedGraphEdge* myEdge = myNode->getEdgeWithLabel(rhsEdge->getLabel());
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

    trace(TRACE_5, "AnnotatedGraph::filterRecursive: checking annotations\n");
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
    trace(TRACE_5, "AnnotatedGraph::filterRecursive: traverse through OG\n");
    rhsEdgeIter = rhsNode->getLeavingEdgesIterator();
    while (rhsEdgeIter->hasNext()) {
        AnnotatedGraphEdge* rhsEdge = rhsEdgeIter->getNext();

        AnnotatedGraphEdge* myEdge = myNode->getEdgeWithLabel(rhsEdge->getLabel());
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
    trace(TRACE_5, "AnnotatedGraph::filterRecursive: post order creation\n");
    rhsEdgeIter = rhsNode->getLeavingEdgesIterator();
    while (rhsEdgeIter->hasNext()) {
        AnnotatedGraphEdge* rhsEdge = rhsEdgeIter->getNext();

        AnnotatedGraphEdge* myEdge = myNode->getEdgeWithLabel(rhsEdge->getLabel());
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

    trace(TRACE_5, "AnnotatedGraph::filterRecursive(...): end\n");
}


//! \brief checks, whether this AnnotatedGraph is acyclic
//! \return true on positive check, otherwise: false
bool AnnotatedGraph::isAcyclic() {
    trace(TRACE_5, "AnnotatedGraph::isAcyclic(...): start\n");

    // Define a set for every Node, that will contain all transitive parent nodes
    map<AnnotatedGraphNode*, set<AnnotatedGraphNode*> > parentNodes;

    // Define a queue for all nodes that still need to be tested and initialize it
    queue<AnnotatedGraphNode*> testNodes;
    testNodes.push(root);
    AnnotatedGraphNode* testNode;

    // While there are still nodes in the queue
    while (!testNodes.empty()) {

        testNode = testNodes.front();
        testNodes.pop();

        // A node counts as a parent node to it self for the purpose of cycles
        parentNodes[testNode].insert(testNode);

        // Iterate all transitions of that node
        AnnotatedGraphNode::LeavingEdges::ConstIterator
                edgeIter = testNode->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            // If the Node is the source of that transition and if the Destination is a valid node
            if (edge->getDstNode()->getColor() == BLUE) {

                // Return false if an outgoing transition points at a transitive parent node,
                // else add the destination to the queue and update its transitive parent nodes
                if (parentNodes[testNode].find(edge->getDstNode())
                        != parentNodes[testNode].end()) {
                    delete edgeIter;
                    trace(TRACE_5, "AnnotatedGraph::isAcyclic(...): end\n");
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
    trace(TRACE_5, "AnnotatedGraph::isAcyclic(...): end\n");
    return true;
}


//! \brief computes the number of services determined by this OG
//! \return number of Services
unsigned int AnnotatedGraph::numberOfServices() {

    trace(TRACE_5, "AnnotatedGraph::numberOfServices(...): start\n");

    trace(TRACE_1, "Removing false nodes...\n");
    removeFalseNodes();

    if (root == NULL) {
        return 0;
    }

    // define variables that will be used in the recursive function
    map<AnnotatedGraphNode*, list <set<AnnotatedGraphNode*> > > validFollowerCombinations;
    set<AnnotatedGraphNode*> activeNodes;
    map<AnnotatedGraphNode*, unsigned int> followers;
    map<set<AnnotatedGraphNode*>, unsigned int> eliminateRedundantCounting;

    // define variables that will be used in the preprocessing before starting the recursion
    set<string> labels;
    list<GraphFormulaAssignment> assignmentList;
    GraphFormulaAssignment possibleAssignment;
    map<string, AnnotatedGraphEdge*> edges;

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
        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =(*iNode)->getLeavingEdgesConstIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
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
        set<AnnotatedGraphNode*> followerNodes;

        // for every true assignment in the list a set of nodes will be created. These are the nodes which are
        // reached by outgoing edges of which the labels were true in the assignment. This set is then saved in
        // a map for the currently proceeded node.
        for (list<GraphFormulaAssignment>::iterator
                assignment = assignmentList.begin(); assignment
                != assignmentList.end(); assignment++) {

            followerNodes = set<AnnotatedGraphNode*>();
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
        trace(TRACE_5, "AnnotatedGraph::numberOfServices(...): end\n");
        return 0;
    } else {
        trace(TRACE_5, "AnnotatedGraph::numberOfServices(...): end\n");
        return number;
    }
}


//! \brief compute the number of possible services for a finished instance or proceed the active nodes
//! \param activeNodes a set of node pointers containing the currently visited nodes
//! \param followers a map that contains the follower nodes of every node in the graph
//! \param validFollowerCombinations a map that contains all sets of follower nodes that succeed to
//!        fullfill the annotation of the predecessor with the edges leading to them            
//! \param eliminateRedundantCounting map containing all already computed number of services for 
//!        possible active node sets 
//! \param instances number of already processed sets of active Nodes
//! \return number of Services for the current set of active nodes
unsigned int AnnotatedGraph::numberOfServicesRecursively(set<AnnotatedGraphNode*> activeNodes,
                                                map<AnnotatedGraphNode*, unsigned int>& followers,
                                                map<AnnotatedGraphNode*, list<set<AnnotatedGraphNode*> > >& validFollowerCombinations,
                                                map<set<AnnotatedGraphNode*>, unsigned int>& eliminateRedundantCounting,
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
    list< set<AnnotatedGraphNode*> > oldList;
    list< set<AnnotatedGraphNode*> > newList;
    set<AnnotatedGraphNode*> tempSet;
    bool first = true;
    bool usingNew = true;
    bool finalInstance = true;

    // process all active nodes of this instance
    for (set<AnnotatedGraphNode*>::iterator activeNode = activeNodes.begin(); activeNode
            != activeNodes.end(); activeNode++) {

        // if the active node has no valid outgoing edges, do nothing. If that happens
        // with all active nodes, the finalInstance variable will stay true
        if (followers[(*activeNode)] != 0) {

            finalInstance = false;

            // if this is the first iteration for this node, fill the newList with all combinations
            // of followers of the currently proceeded active Node and continues the loop
            if (first) {

                first = false;
                for (list<set<AnnotatedGraphNode*> >::iterator
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
                for (list<set<AnnotatedGraphNode*> >::iterator
                        oldListSet = oldList.begin(); oldListSet
                        != oldList.end(); oldListSet++) {

                    for (list<set<AnnotatedGraphNode*> >::iterator
                            combination = validFollowerCombinations[(*activeNode)].begin(); combination
                            != validFollowerCombinations[(*activeNode)].end(); combination++) {

                        tempSet = set<AnnotatedGraphNode*>();

                        for (set<AnnotatedGraphNode*>::iterator insertionNode = (*combination).begin(); insertionNode != (*combination).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        for (set<AnnotatedGraphNode*>::iterator insertionNode = (*oldListSet).begin(); insertionNode != (*oldListSet).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        newList.push_back(tempSet);
                    }
                }
                usingNew = false;
            } else {

                oldList.clear();
                for (list<set<AnnotatedGraphNode*> >::iterator
                        newListSet = newList.begin(); newListSet
                        != newList.end(); newListSet++) {

                    for (list<set<AnnotatedGraphNode*> >::iterator
                            combination = validFollowerCombinations[(*activeNode)].begin(); combination
                            != validFollowerCombinations[(*activeNode)].end(); combination++) {

                        tempSet = set<AnnotatedGraphNode*>();

                        for (set<AnnotatedGraphNode*>::iterator insertionNode = (*combination).begin(); insertionNode != (*combination).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        for (set<AnnotatedGraphNode*>::iterator insertionNode = (*newListSet).begin(); insertionNode != (*newListSet).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }
                        oldList.push_back(tempSet);
                    }
                }
                usingNew = true;
            }
        } else {
            bool valid = false;
            for (list<set<AnnotatedGraphNode*> >::iterator
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
        for (list<set<AnnotatedGraphNode*> >::iterator oldListSet = oldList.begin(); oldListSet
                != oldList.end(); oldListSet++) {

            number += numberOfServicesRecursively((*oldListSet), followers,
                                                  validFollowerCombinations,
                                                  eliminateRedundantCounting,
                                                  instances);
        }
    } else {
        for (list<set<AnnotatedGraphNode*> >::iterator newListSet = newList.begin(); newListSet
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
//! \param labels a set labels not yet set to a value, used for the recursion
//! \param possibleAssignments the currently processed assignment containing all label valuse that
//!        already have been set
//! \param testNode a pointer to the node currently tested            
//! \param assignmentList a list of assignment that already have turned true in recursion 
//! \return number of true Assignments
unsigned int AnnotatedGraph::processAssignmentsRecursively(set<string> labels,
                                                  GraphFormulaAssignment possibleAssignment,
                                                  AnnotatedGraphNode* testNode,
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


//! \brief Returns the product OG of this OG and the passed one. The caller has to delete the returned 
//!        AnnotatedGraph.
//! \param rhs the OG to be used for computing the product
//! \return returns the product OG
AnnotatedGraph* AnnotatedGraph::product(const AnnotatedGraph* rhs) {
    trace(TRACE_5, "AnnotatedGraph::product(const AnnotatedGraph* rhs): start\n");

    // this will be the product OG
    AnnotatedGraph* productOG = new AnnotatedGraph;

    // If one of both OGs is empty, their product is empty, too.
    if (hasNoRoot() || rhs->hasNoRoot()) {
        return productOG;
    }

    // first we build a new root node that has name and annotation constructed
    // from the root nodes of OG and the rhs OG.
    AnnotatedGraphNode* currentOGNode = this->getRoot();
    AnnotatedGraphNode* currentRhsNode = rhs->getRoot();

    std::string currentName;
    currentName = currentOGNode->getName() + "x"+ currentRhsNode->getName();

    GraphFormulaCNF* currentFormula = createProductAnnotation(currentOGNode,
                                                              currentRhsNode);

    // building the new root node of the product OG
    AnnotatedGraphNode* productNode = new AnnotatedGraphNode(currentName, currentFormula);
    productOG->addNode(productNode);
    productOG->setRoot(productNode);

    // builds the successor nodes of the root nodes of OG and rhs OG
    // therefore, we perform a coordinated dfs through OG and the rhs OG
    buildProductOG(currentOGNode, currentRhsNode, productOG);

    if (!options[O_DIAGNOSIS]) {
        productOG->removeFalseNodes();
    }

    trace(TRACE_5, "AnnotatedGraph::product(const AnnotatedGraph* rhs): end\n");

    return productOG;
}


//! \brief Returns the product OG of all given OGs. The caller has to delete the AnnotatedGraph
//! \param ogs a list of OGs
//! \return returns the product OG
AnnotatedGraph* AnnotatedGraph::product(const ogs_t& ogs) {
    assert(ogs.size() > 1);

    ogs_t::const_iterator iOG = ogs.begin();
    AnnotatedGraph* firstOG = *iOG++;
    AnnotatedGraph* productOG = firstOG->product(*iOG);

    for (++iOG; iOG != ogs.end(); ++iOG) {
        AnnotatedGraph* oldProductOG = productOG;
        productOG = productOG->product(*iOG);
        delete oldProductOG;
    }

    return productOG;
}


//! \brief Recursive coordinated dfs through OG and rhs OG.
//! \param currentOGNode the current node of the OG
//! \param currentRhsNode the current node of the rhs OG
//! \param productOG the resulting product OG
void AnnotatedGraph::buildProductOG(AnnotatedGraphNode* currentOGNode,
                           AnnotatedGraphNode* currentRhsNode,
                           AnnotatedGraph* productOG) {

    trace(TRACE_5,
          "AnnotatedGraph::buildProductOG(AnnotatedGraphNode* currentOGNode, AnnotatedGraphNode* currentRhsNode, AnnotatedGraph* productOG): start\n");

    // at this time, the node constructed from currentOGNode and
    // currentRhsNode is already inserted

    assert(productOG->getRoot() != NULL);

    // iterate over all outgoing edges from current node of OG
    std::string currentLabel;
    AnnotatedGraphNode::LeavingEdges::ConstIterator
            edgeIter = currentOGNode->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();

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
            AnnotatedGraphNode* newOGNode;
            newOGNode = currentOGNode->followEdgeWithLabel(currentLabel);

            AnnotatedGraphNode* newRhsNode;
            newRhsNode = currentRhsNode->followEdgeWithLabel(currentLabel);

            // build the new node of the product OG
            // that has name and annotation constructed from current nodes of OG and rhs OG
            std::string newProductName;
            newProductName = newOGNode->getName() + "x"+ newRhsNode->getName();
            // if the node is new, add that node to the OG
            AnnotatedGraphNode* found = productOG->getNodeWithName(newProductName);

            if (found != NULL) {
                // the node was known before, so we just have to add a new edge
                productOG->addEdge(currentName, newProductName, currentLabel);

                trace(
                      TRACE_5,
                      "AnnotatedGraph::buildProductOG(AnnotatedGraphNode* currentOGNode, AnnotatedGraphNode* currentRhsNode, AnnotatedGraph* productOG): end\n");
            } else {
                // we computed a new node, so we add a node and an edge
                // trace(TRACE_0, "adding node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + "\n");

                GraphFormulaCNF
                        * newProductFormula = createProductAnnotation(
                                                                      newOGNode,
                                                                      newRhsNode);

                AnnotatedGraphNode* newProductNode= new AnnotatedGraphNode(newProductName, newProductFormula);

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
          "AnnotatedGraph::buildProductOG(AnnotatedGraphNode* currentOGNode, AnnotatedGraphNode* currentRhsNode, AnnotatedGraph* productOG): end\n");
}

//! \brief Creates and returns the annotation for the product node of the given two nodes.
//!        The caller has to delete the formula after usage.
//! \param lhs the first node
//! \param rhs the second node
//! \return returns the Formula of the product node
GraphFormulaCNF* AnnotatedGraph::createProductAnnotation(const AnnotatedGraphNode* lhs,
                                                const AnnotatedGraphNode* rhs) const {

    GraphFormulaMultiaryAnd* conjunction= new GraphFormulaMultiaryAnd(
            lhs->getAnnotation()->getDeepCopy(),
            rhs->getAnnotation()->getDeepCopy());

    GraphFormulaCNF* cnf = conjunction->getCNF();
    delete conjunction;
    cnf->simplify();
    return cnf;
}


//! \brief Produces from the given OG file names the default prefix of the
//!        product OG output file.
//! \param ogfiles a set of the filenames of the used OGs
//! \return returns a string for the name of the product og
std::string AnnotatedGraph::getProductOGFilePrefix(const ogfiles_t& ogfiles) {
    assert(ogfiles.size() != 0);

    ogfiles_t::const_iterator iOgFile = ogfiles.begin();
    string productFilePrefix = stripOGFileSuffix(*iOgFile);

    for (++iOgFile; iOgFile != ogfiles.end(); ++iOgFile) {
        productFilePrefix += "_X_"
                + stripOGFileSuffix(platform_basename(*iOgFile));
    }

    return productFilePrefix;
}


//! \brief strips the OG file suffix from filename and returns the result
//! \param filename name of the og file
//! \param returns the filename without suffix
std::string AnnotatedGraph::stripOGFileSuffix(const std::string& filename) {

    static const string ogFileSuffix = ".og";
    if (filename.substr(filename.size() - ogFileSuffix.size()) == ogFileSuffix) {
        return filename.substr(0, filename.size() - ogFileSuffix.size());
    }

    return filename;
}


//! \brief creates a dot output of the graph and calls dot to create an image from it
//! \param filenamePrefix a string containing the prefix of the output file name
//! \param dotGraphTitle a title for the graph to be shown in the image
void AnnotatedGraph::printDotFile(const std::string& filenamePrefix,
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

    std::map<AnnotatedGraphNode*, bool> visitedNodes;
    printGraphToDot(getRoot(), dotFileHandle, visitedNodes);

    dotFileHandle << "}";
    dotFileHandle.close();

    // prepare dot command line for printing
    string cmd = "dot -Tpng \"" + dotFile + "\" -o \""+ pngFile + "\"";

    // print commandline and execute system command
    trace(TRACE_0, cmd + "\n\n");
    system(cmd.c_str());
}


//! \brief creates a dot output of the graph and calls dot to create an image from it
//! \param filenamePrefix a string containing the prefix of the output file name
void AnnotatedGraph::printDotFile(const std::string& filenamePrefix) const {
    printDotFile(filenamePrefix, filenamePrefix);
}


//! \brief dfs through the graph printing each node and edge to the output stream
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes maps nodes to Bools remembering already visited nodes
void AnnotatedGraph::printGraphToDot(AnnotatedGraphNode* v,
                            fstream& os,
                            std::map<AnnotatedGraphNode*, bool>& visitedNodes) const {

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

        AnnotatedGraphNode::LeavingEdges::ConstIterator
                edgeIter = v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();

            // remember the label of the egde
            currentLabel = edge->getLabel();
            AnnotatedGraphNode* successor = v->followEdgeWithLabel(currentLabel);
            assert(successor != NULL);

            os << "p"<< v->getName() << "->"<< "p"<< successor->getName()
                    << " [label=\""<< currentLabel
                    << "\", fontcolor=black, color= blue];\n";
            printGraphToDot(successor, os, visitedNodes);
        }
        delete edgeIter;
    }
}


//! \brief A function needed for successful deletion of the graph
void AnnotatedGraph::clearNodeSet() {
    setOfNodes.clear();
}


//! \brief Prints this OG in OG file format to a file with the given prefix. The
//!        suffix is added automatically by this method.
//! \param filenamePrefix a prefix for th filename
void AnnotatedGraph::printOGFile(const std::string& filenamePrefix) const {
    fstream ogFile(addOGFileSuffix(filenamePrefix).c_str(), ios_base::out | ios_base::trunc);

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

        AnnotatedGraphNode* node = *iNode;
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

        AnnotatedGraphNode* node = *iNode;
        if (!node->isToShow(root)) {
            continue;
        }

        AnnotatedGraphNode::LeavingEdges::ConstIterator iEdge =
            node->getLeavingEdgesConstIterator();

        while (iEdge->hasNext()) {
            AnnotatedGraphEdge* edge = iEdge->getNext();
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


//! \brief adds the suffix ".og" to a string
//! \param filePrefix a string to be modified
//! \return returns the string with the og suffix
std::string AnnotatedGraph::addOGFileSuffix(const std::string& filePrefix) {
    return filePrefix + ".og";
}


//! \brief Get all transitions from the graph, each associated to a specific label
//! return returns the transition map
AnnotatedGraph::TransitionMap AnnotatedGraph::getTransitionMap() {
    trace(TRACE_3, "AnnotatedGraph::getTransitionMap()::begin()\n");
    TransitionMap tm;

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if ((*iNode)->isBlue() && (parameters[P_SHOW_EMPTY_NODE] || (*iNode)->reachGraphStateSet.size() != 0)) {
            AnnotatedGraphNode::LeavingEdges::Iterator iEdge = (*iNode)->getLeavingEdgesIterator();
            while (iEdge->hasNext()) {
                AnnotatedGraphEdge* edge = iEdge->getNext();
                if (edge->getDstNode()->isBlue() && (parameters[P_SHOW_EMPTY_NODE]
                        || edge->getDstNode()->reachGraphStateSet.size() != 0))
                    tm[edge->getLabel()].insert((*iNode)->getName() + "@"+ edge->getLabel() + "@"
                            + edge->getDstNode()->getName());
            }
        }
    }

    trace(TRACE_3, "AnnotatedGraph::getTransitionMap()::end()\n");
    return tm;
}


//! \brief Get all transitions from the graph with a label from the given label set, 
//! each associated to a specific label
//! return returns the transition map
AnnotatedGraph::TransitionMap AnnotatedGraph::getTransitionMap(set<string>* labels) {
    trace(TRACE_3, "AnnotatedGraph::getTransitionMap(set<string>* labels)::begin()\n");
    TransitionMap tm;

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if ((*iNode)->isBlue() && (parameters[P_SHOW_EMPTY_NODE] || (*iNode)->reachGraphStateSet.size() != 0)) {
            AnnotatedGraphNode::LeavingEdges::Iterator iEdge = (*iNode)->getLeavingEdgesIterator();
            while (iEdge->hasNext()) {
                GraphEdge* edge = iEdge->getNext();
                if ((labels->find(edge->getLabel()) != labels->end()) && edge->getDstNode()->isBlue() && 
                    (parameters[P_SHOW_EMPTY_NODE] || edge->getDstNode()->reachGraphStateSet.size() != 0))
                    tm[edge->getLabel()].insert((*iNode)->getName() + "@"+ edge->getLabel() + "@"
                            + edge->getDstNode()->getName());
            }
        }
    }

    trace(TRACE_3, "AnnotatedGraph::getTransitionMap()::end()\n");
    return tm;
}


//! \brief Create the formula describing the coverability criteria when covering labels from the given set
//! \param labels the set that containts all the events that shall be covered; omitting is equal NULL and refers to
//! covering the whole interface set
//! \return returns the coverability formula 
void AnnotatedGraph::createCovFormula(set<string>* labels = NULL) {
    trace(TRACE_3, "AnnotatedGraph::createCovFormula(set<string>* labels)::begin()\n");

    GraphFormulaCNF *formula= new GraphFormulaCNF;
    TransitionMap tm;
    
    if (!labels) {
        tm = getTransitionMap(labels);
    }
    else {
        tm = getTransitionMap();
    }
    
    for (TransitionMap::iterator i = tm.begin(); i != tm.end(); i++) {
        GraphFormulaMultiaryOr *clause= new GraphFormulaMultiaryOr;
        for (EdgeSet::iterator j = i->second.begin(); j != i->second.end(); j++) {
            GraphFormulaLiteral *literal= new GraphFormulaLiteral(*j);
            clause->addSubFormula(literal);
        }
        formula->addClause(clause);
    }

    covConstraint = formula;

    trace(TRACE_3, "AnnotatedGraph::createCovFormula(): " + formula->asString() + "\n");
    trace(TRACE_3, "AnnotatedGraph::createCovFormula()::end()\n");
}


//! \brief Create the formula describing the event-structure of the graph
//! NOTE: the graph has to be acyclic!
//! \return returns the structure formula
GraphFormulaMultiaryAnd *AnnotatedGraph::createStructureFormula() {
    trace(TRACE_3, "AnnotatedGraph::createStructureFormula()::begin()\n");
 
    assert(isAcyclic());
    
    GraphFormulaMultiaryAnd *formula = createStructureFormulaRecursively(getRoot());

    trace(TRACE_3, "AnnotatedGraph::createStructureFormula(): " + formula->asString() + "\n");
    trace(TRACE_3, "AnnotatedGraph::createStrctureFormula()::end()\n");
    return formula;
}


//! \brief Creates the formula describing the event-structure of the subgraph under node
//! NOTE: the subgraph has to be acyclic!
//! \param node the root of the subgraph
//! \return returns the structure formula of the subgraph
GraphFormulaMultiaryAnd *AnnotatedGraph::createStructureFormulaRecursively(AnnotatedGraphNode *node) {
    trace(TRACE_5, "AnnotatedGraph::createStructureFormulaRecursively()::begin()\n");
    
    GraphFormulaMultiaryAnd *formula = new GraphFormulaMultiaryAnd;
    
    if (node == NULL) {
        return formula;
    }
    
    GraphFormulaCNF *annotation = node->getAnnotation();
    for (GraphFormulaMultiaryAnd::iterator i = annotation->begin();
          i != annotation->end(); i++) {

         GraphFormulaMultiaryOr* clause = dynamic_cast<GraphFormulaMultiaryOr*>(*i);
         GraphFormulaMultiaryOr* new_clause = new GraphFormulaMultiaryOr;
         // iterate over disjunctive clauses
         for (GraphFormulaMultiaryOr::iterator j = clause->begin(); j != clause->end(); j++) {

             GraphFormulaLiteral* lit = dynamic_cast<GraphFormulaLiteral*>(*j);
             GraphFormulaLiteral* new_lit;
             if((dynamic_cast<GraphFormulaFixed*>(*j) == NULL) && (dynamic_cast<GraphFormulaLiteralFinal*>(*j) == NULL)) {
                 new_lit = new GraphFormulaLiteral(node->getName() + "@" + lit->asString() + "@"
                         + node->followEdgeWithLabel(lit->asString())->getName());
             }
             else {
                 new_lit = new GraphFormulaLiteral(lit->asString());
             }
             GraphFormulaMultiaryAnd* new_and = new GraphFormulaMultiaryAnd(new_lit, 
                     createStructureFormulaRecursively(node->followEdgeWithLabel(lit->asString())));
             new_clause->addSubFormula(new_and);
         }
         formula->addSubFormula(new_clause);
    }
    
    trace(TRACE_5, "AnnotatedGraph::createStructureFormulaRecursively(): " + formula->asString() + "\n");
    trace(TRACE_5, "AnnotatedGraph::createStructureFormulaRecursively()::end()\n");
    return formula;
}


//! \brief remove a node from the AnnotatedGraph
//! \param node node to remove
void AnnotatedGraph::removeNode(AnnotatedGraphNode* node) {
    assert(node);
    // setOfNodes.erase(node);  // only valid if container is a std::set

    for (vector<AnnotatedGraphNode*>::iterator testnode = setOfNodes.begin(); testnode
            != setOfNodes.end(); testnode++) {
        if ((*testnode) == node) {
            setOfNodes.erase(testnode);
            break;
        }
    }

}


//! \brief removes all nodes annotated with true and all their incoming/outgoing transitions
void AnnotatedGraph::removeNodesAnnotatedWithTrue() {
    // iterate over all nodes of the AnnotatedGraph
    for (nodes_iterator node_iter = setOfNodes.begin(); node_iter
            != setOfNodes.end(); ++node_iter) {
        AnnotatedGraphNode* currNode = *node_iter;
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
//! \complexity O(n+m), with n nodes and m edges in the given AnnotatedGraph
void AnnotatedGraph::constructDualService() {
    // iterate over all nodes of the AnnotatedGraph
    for (nodes_iterator node_iter = setOfNodes.begin(); node_iter
            != setOfNodes.end(); ++node_iter) {
        AnnotatedGraphNode* currNode = *node_iter;
        // iterate over all leaving edges of the current node
        AnnotatedGraphNode::LeavingEdges::Iterator
                edge_iter = currNode->getLeavingEdgesIterator();
        while (edge_iter->hasNext()) {
            AnnotatedGraphEdge* currEdge = edge_iter->getNext();
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
void AnnotatedGraph::fixDualService() {
    // prepare deadlock creation
    AnnotatedGraphNode* deadlock = NULL;
    map<std::string, AnnotatedGraphNode*> deadlockMap;
    unsigned int currNumberOfDeadlocks = 0;
    if (!options[O_PV_MULTIPLE_DEADLOCKS]) {
        // create new deadlock node
        deadlock = new AnnotatedGraphNode("deadlock", new GraphFormulaFixed(false, GraphFormulaLiteral::FALSE));
    }

    // iterate over all nodes of the AnnotatedGraph
    for (nodes_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end(); ++node_iter) {

        AnnotatedGraphNode* currNode = *node_iter;

        // 1st fix: if !a is disabled in q of OG, add ?a->Deadlock in q of dual service
        // preparation: assume all receive events are disabled in current node (dual service)
        set<std::string> disabledRecvEvents = recvEvents;

        // 2nd fix: if ?z is enabled in q of OG, but ?z does not occur in the
        //	annotation of q create a new node without ?z and a tau-transition
        // preparation: assume we don't have to apply the 2nd fix in current node
        bool apply2ndFix = false;
        // preparation: we need to store the leaving edges for the new node somewhere
        set<AnnotatedGraphEdge*> newNodesEdges;
        AnnotatedGraphNode::LeavingEdges::Iterator edge_iter = currNode->getLeavingEdgesIterator();
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
            AnnotatedGraphEdge* currEdge = edge_iter->getNext();
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
                        new AnnotatedGraphNode("deadlock" +
                                      intToString(currNumberOfDeadlocks++),
                                      new GraphFormulaFixed(false,
                                                            GraphFormulaLiteral::FALSE));
                    deadlockMap[*event_iter] = deadlock;
                }
            }
            // create new leaving edge from current node to the deadlock node
            //   labeled with current event
            AnnotatedGraphEdge* disabledEvent= new AnnotatedGraphEdge(deadlock, *event_iter);
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
            AnnotatedGraphNode* newNode= new AnnotatedGraphNode(currNode->getName() + "_",
                    new GraphFormulaFixed(true, ""), currNode->getColor());
            trace(TRACE_5, "created new node " + newNode->getName() + "\n");
            // create tau transition from current to new node
            AnnotatedGraphEdge* tauTransition= new AnnotatedGraphEdge(newNode, GraphFormulaLiteral::TAU);
            currNode->addLeavingEdge(tauTransition);
            trace(TRACE_5, "created tau transition from " + currNode->getName()
                    +" to "+ newNode->getName() + "\n");

            // add current node's leaving edges to new node 
            //   except those that don't occur in the annotation
            trace(TRACE_5, "adding leaving edges to new node\n");
            for (set<AnnotatedGraphEdge*>::iterator edge_iter = newNodesEdges.begin(); edge_iter
                    != newNodesEdges.end(); ++edge_iter) {
                // create new leaving edge
                AnnotatedGraphEdge* newEdge= new AnnotatedGraphEdge((*edge_iter)->getDstNode(),
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
                AnnotatedGraphEdge* disabledEvent= new AnnotatedGraphEdge(deadlock, *event_iter);
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
        map<std::string, AnnotatedGraphNode*>::iterator deadlockIter;
        for (deadlockIter = deadlockMap.begin(); deadlockIter
                != deadlockMap.end(); ++deadlockIter) {
            this->addNode(deadlockIter->second);
        }
    }
}


//! \brief transforms the graph into its public view
void AnnotatedGraph::transformToPublicView() {
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

