/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe, Leonard Kern*
 *                                                                           *
 * Copyright 2008                                                            *
 *   Peter Massuthe, Daniela Weinberg, Christian Gierds                      *
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

//extern std::list<std::string> netfiles;
//extern std::list<std::string> ogfiles;


//! \brief a basic constructor of AnnotatedGraph
AnnotatedGraph::AnnotatedGraph() :
    root(NULL), covConstraint(NULL) {
}


//! \brief a basic destructor of AnnotatedGraph
AnnotatedGraph::~AnnotatedGraph() {
    trace(TRACE_5, "AnnotatedGraph::~AnnotatedGraph() : start\n");
    for (unsigned int i = 0; i < setOfNodes.size(); i++) {
        delete setOfNodes[i];
    }
    setOfNodes.clear();
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

    AnnotatedGraphNode* node = new AnnotatedGraphNode(nodeName, annotation,
                                                      color, setOfNodes.size());
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

    AnnotatedGraphEdge* transition = new AnnotatedGraphEdge(dstNode, label);
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

    bool modified = true;

    // As long as something changes...
    while (modified) {

        modified = false;

        // A set to store the nodes that violate their annotations
        set<AnnotatedGraphNode*> falseNodes;

        // Iterate over all nodes
        for (nodes_iterator nodeIter = setOfNodes.begin();
             nodeIter != setOfNodes.end(); ++nodeIter) {

            AnnotatedGraphNode* currentNode = *nodeIter;

            trace(TRACE_2, "\t\tprocessing node: " + (*nodeIter)->getName() + "\n");

            // A set to store edges that lead to false nodes
            set<AnnotatedGraphEdge*> falseEdges;

            // Iterate over all edges of the current node
            for (AnnotatedGraphNode::LeavingEdges::Iterator edgeIter = currentNode->getLeavingEdgesIterator();
                 edgeIter->hasNext();) {

                AnnotatedGraphEdge* currentEdge = edgeIter->getNext();

                trace(TRACE_4, "\t\t\tprocessing edge: " + (*nodeIter)->getName()
                      + " - " + currentEdge->getLabel() + " > "
                      + currentEdge->getDstNode()->getName() + "\n");

                // Check wether the edge leads to a false node
                if (falseNodes.find(currentEdge->getDstNode()) != falseNodes.end()) {
                    // Insert the false edge into the set
                    falseEdges.insert(currentEdge);

                    trace(TRACE_4, "\t\t\tedge is leaving to a false node\n");
                }
            }

            // Something has changed!
            modified = falseEdges.size() > 0;

            // Iterate over all false edges found for that current node to delete them
            for (set<AnnotatedGraphEdge*>::iterator edgeIter = falseEdges.begin();
                 edgeIter != falseEdges.end(); ++edgeIter) {

                currentNode->removeEdgesToNode((*edgeIter)->getDstNode());
            }

            // Evaluate the annotation of the current node 
            if (!currentNode->assignmentSatisfiesAnnotation(*(currentNode->getAssignment()))) {
                // Insert the false node into the set
                falseNodes.insert(currentNode);

                trace(TRACE_2, "\t\tnode " + (*nodeIter)->getName() + " cannot satisfy its annotation.\n");
                trace(TRACE_3, "\t\tannotation: " + (*nodeIter)->getAnnotationAsString() + " \n");
            }
        }

        // Something has changed!
        modified = modified || falseNodes.size() > 0;

        // Iterate over the set of false nodes to delete them
        for (set<AnnotatedGraphNode*>::iterator nodeIter = falseNodes.begin();
             nodeIter != falseNodes.end(); ++nodeIter) {

            // If the root node has to be deleted, we set the root to NULL. 
            if (*nodeIter == getRoot()) {
                setRoot(NULL);
            }

            trace(TRACE_1, "\tremoved false node: " + (*nodeIter)->getName() + "\n");

            // Remove all edges leading to this node and the node itself. 
            removeEdgesToNodeFromAllOtherNodes(*nodeIter);
            removeNode(*nodeIter);
            delete *nodeIter;
        }
    }

    trace(TRACE_0, "removing disconnected nodes ...\n");

    // Remove any nodes, that have been disconnected from the root node
    removeDisconnectedNodes();

    // Check whether the root node was also removed
    if (hasNoRoot()) {
        trace(TRACE_0, "root node was removed!\n");
    }

    trace(TRACE_5, "AnnotatedGraph::removeFalseNodes(): end\n");
}


//! \brief removes all nodes that have been disconnected from the root
//!        node due to other node removals
void AnnotatedGraph::removeDisconnectedNodes() {

    trace(TRACE_5, "AnnotatedGraph::removeDisconnectedNodes(): start\n");

    // create sets of connected and disconnected nodes
    set<AnnotatedGraphNode*> connectedNodes;
    set<AnnotatedGraphNode*> disconnectedNodes;

    // gather the connected nodes from the rootnode if there is a root node
    if (!hasNoRoot()) {
        removeDisconnectedNodesRecursively(getRoot(), connectedNodes);
    }

    // insert all nodes which are not connected into the set of disconnected nodes
    for (nodes_iterator nodeIter = setOfNodes.begin(); nodeIter != setOfNodes.end(); ++nodeIter) {
        if (connectedNodes.find(*nodeIter) == connectedNodes.end()) {
            disconnectedNodes.insert(*nodeIter);
        }
    }

    // remove all disconnected nodes
    for (set<AnnotatedGraphNode*>::iterator nodeIter = disconnectedNodes.begin();
         nodeIter != disconnectedNodes.end(); ++nodeIter) {
        AnnotatedGraphNode* currentNode = *nodeIter;

        trace(TRACE_1, "\tremoved disconnected node: " + (*nodeIter)->getName() + "\n");

        removeEdgesToNodeFromAllOtherNodes(currentNode);
        removeNode(currentNode);
        delete currentNode;
    }

    trace(TRACE_5, "AnnotatedGraph::removeDisconnectedNodes(): end\n");
}


//! \brief collects all connected Nodes in a set
//! \param currentNode Current node to be added to the set
//! \param connectedNodes set of connected nodes
void AnnotatedGraph::removeDisconnectedNodesRecursively(AnnotatedGraphNode* currentNode,
                                                        set<AnnotatedGraphNode*>& connectedNodes) {

    trace(TRACE_5, "AnnotatedGraph::removeDisconnectedNodesRecursively(): start\n");

    // add the node to the connected nodes
    connectedNodes.insert(currentNode);

    trace(TRACE_3, "\tnode " + currentNode->getName() + " is connected.\n");

    // iterate over all edges
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = currentNode->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();

        // if the edge leads to a node that has not been determined as connected yet, proceed it
        if (connectedNodes.find(edge->getDstNode()) == connectedNodes.end()) {
            removeDisconnectedNodesRecursively(edge->getDstNode(), connectedNodes);
        }
    }

    trace(TRACE_5, "AnnotatedGraph::removeDisconnectedNodesRecursively(): end\n");
}


//! \brief finds all nodes that have annotations that cannot become
//!        true. The function continues removing until no node fullfils
//!        the mentioned criterion
void AnnotatedGraph::findFalseNodes(std::vector<AnnotatedGraphNode*>* falseNodes) {

    trace(TRACE_5, "AnnotatedGraph::findFalseNodes(): start\n");

    nodes_iterator iNode = setOfNodes.begin();

    while (iNode != setOfNodes.end()) {
        GraphFormulaAssignment* iNodeAssignment = (*iNode)->getAssignment();
        if (!(*iNode)->assignmentSatisfiesAnnotation(*iNodeAssignment)) {
            falseNodes->push_back(*iNode);
        }
        ++iNode;
    }
    trace(TRACE_5, "AnnotatedGraph::findFalseNodes(): end\n");
}


//! \brief returns the name of the graph's source file
std::string AnnotatedGraph::getFilename() {
    return filename;
}


//! \brief returns the name of the graph's source file
void AnnotatedGraph::setFilename(std::string filename) {
    this->filename = filename;
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
    trace(TRACE_5, "AnnotatedGraph::simulates(AnnotatedGraph* smallerOG): start\n");

    // Simulation is impossible without a simulant.
    if (smallerOG == NULL) {
        return false;
    }

    if (smallerOG->getRoot() == NULL) {
        return true;
    } else if (root == NULL) {
        return false;
    }

    // We need to remember the pairs of nodes we already visited.
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > visitedNodes;

    // Get things moving...
    bool result = false;
    if (simulatesRecursive(root, smallerOG->getRoot(), visitedNodes, this, smallerOG)) {
        result = true;
    }

    trace(TRACE_5, "AnnotatedGraph::simulates(AnnotatedGraph* smallerOG): end\n");
    return result;
}


//! \brief checks, whether the part of an AnnotatedGraph below myNode simulates
//         the part of an AnnotatedGraph below simNode
//! \return true on positive check, otherwise: false
//! \param myNode a node in this AnnotatedGraph
//! \param simNode a node in the simulant
//! \param visitedNodes Holds all visited pairs of nodes.
bool AnnotatedGraph::simulatesRecursive(AnnotatedGraphNode* myNode,
                                        AnnotatedGraphNode* simNode,
                                        set<pair<AnnotatedGraphNode*,
                                        AnnotatedGraphNode*> >& visitedNodes,
                                        AnnotatedGraph* greaterOG,
                                        AnnotatedGraph* smallerOG) {

    // checking, whether myNode simulates simNode; result is true, iff
    // 1) anno of simNode implies anno of myNode and
    // 2) myNode has each outgoing event of simNode, too

    assert(myNode);
    assert(simNode);

    trace(TRACE_2, "\t checking whether node " + myNode->getName());
    trace(TRACE_3, " of " + greaterOG->getFilename());
    trace(TRACE_2, " simulates node " + simNode->getName());
    trace(TRACE_3, " of " + smallerOG->getFilename());
    trace(TRACE_2, "\n");

    // If we already visited this pair of nodes, then we're done.
    if (visitedNodes.find(make_pair(myNode, simNode)) != visitedNodes.end()) {
        trace(TRACE_3, "\t already been checked\n");
        return true;
    } else {
        visitedNodes.insert(make_pair(myNode, simNode));
    }

    // 1st step:
    // first we check implication of annotations: simNode -> myNode
    trace(TRACE_3, "\t\t checking annotations (2nd node's annotation implies 1st?)...\n");
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
        trace(TRACE_2, "\t\t simulation failed (annotation)\n");

        trace(TRACE_4, "\t\t\t   " + simNode->getAnnotation()->asString() + "\n");
        trace(TRACE_4, "\t\t\t   -/->\n");
        trace(TRACE_4, "\t\t\t   " + myNode->getAnnotation()->asString() + "\n");

        // reporting all leaving edges of both nodes for debugging
        AnnotatedGraphNode::LeavingEdges::Iterator edgeIter;

        trace(TRACE_4, "\t\t\t   leaving edges of node ");
        trace(TRACE_4, simNode->getName() + " of ");
        trace(TRACE_4, smallerOG->getFilename());
        trace(TRACE_4, " (" + intToString(simNode->getLeavingEdgesCount())
                + "):\n");

        edgeIter = simNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            trace(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> "
                    + edge->getDstNode()->getName() + " (");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    trace(TRACE_4, "BLUE");
                    break;
                case RED:
                    trace(TRACE_4, "RED");
                    break;
            }
            trace(TRACE_4, ")\n");
        }

        trace(TRACE_4, "\t\t\t   leaving edges of node ");
        trace(TRACE_4, myNode->getName() + " of ");
        trace(TRACE_4, greaterOG->getFilename());
        trace(TRACE_4, " (" + intToString(myNode->getLeavingEdgesCount())
                + "):\n");

        edgeIter = myNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            trace(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> "
                    + edge->getDstNode()->getName() + "(");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    trace(TRACE_4, "BLUE");
                    break;
                case RED:
                    trace(TRACE_4, "RED");
                    break;
            }
            trace(TRACE_4, ")\n");
        }

        delete edgeIter;
        delete simNodeAnnotationInCNF;
        delete myNodeAnnotationInCNF;

        return false;
    }

    // 2nd step:
    // now we check whether myNode has each outgoing event of simNode
    trace(TRACE_3, "\t\t checking edges...\n");
    AnnotatedGraphNode::LeavingEdges::ConstIterator simEdgeIter = simNode->getLeavingEdgesConstIterator();

    while (simEdgeIter->hasNext()) {
        AnnotatedGraphEdge* simEdge = simEdgeIter->getNext();

        trace(TRACE_4, "\t\t\t checking event " + simEdge->getLabel() + "\n");

        AnnotatedGraphEdge* myEdge = myNode->getEdgeWithLabel(simEdge->getLabel());

        if (myEdge == NULL) {
            // simNode has edge which myNode hasn't
            trace(TRACE_2, "\t\t simulation failed (edges)\n");

            // reporting all leaving edges of both nodes for debugging
            AnnotatedGraphNode::LeavingEdges::Iterator edgeIter;

            trace(TRACE_4, "\t\t\t   leaving edges of node ");
            trace(TRACE_4, simNode->getName() + " of ");
            trace(TRACE_4, smallerOG->getFilename());
            trace(TRACE_4, " (" + intToString(simNode->getLeavingEdgesCount()) + "):\n");

            edgeIter = simNode->getLeavingEdgesIterator();
            while (edgeIter->hasNext()) {
                AnnotatedGraphEdge* edge = edgeIter->getNext();
                trace(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> " + edge->getDstNode()->getName() + " (");
                switch (edge->getDstNode()->getColor()) {
                    case BLUE:
                        trace(TRACE_4, "BLUE");
                        break;
                    case RED:
                        trace(TRACE_4, "RED");
                        break;
                }
                trace(TRACE_4, ")\n");
            }

            trace(TRACE_4, "\t\t\t   leaving edges of node ");
            trace(TRACE_4, myNode->getName() + " of ");
            trace(TRACE_4, greaterOG->getFilename());
            trace(TRACE_4, " (" + intToString(myNode->getLeavingEdgesCount()) + "):\n");

            edgeIter = myNode->getLeavingEdgesIterator();
            while (edgeIter->hasNext()) {
                AnnotatedGraphEdge* edge = edgeIter->getNext();
                trace(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> "
                        + edge->getDstNode()->getName() + "(");
                switch (edge->getDstNode()->getColor()) {
                    case BLUE:
                        trace(TRACE_4, "BLUE");
                        break;
                    case RED:
                        trace(TRACE_4, "RED");
                        break;
                }
                trace(TRACE_4, ")\n");
            }

            delete edgeIter;
            delete simEdgeIter;

            return false;
        } else {
            trace(TRACE_4, "\t\t\t event present, going down\n");

            if (!simulatesRecursive(myEdge->getDstNode(),
                    simEdge->getDstNode(), visitedNodes, greaterOG, smallerOG)) {
                delete simEdgeIter;
                return false;
            }
        }
    }
    delete simEdgeIter;

    // All checks were successful.
    return true;
}


//! \brief checks, whether this AnnotatedGraph is equivalent to the given one
//! \return true on positive check, otherwise: false
//! \param secondOG the AnnotatedGraph that is checked for equivalence
bool AnnotatedGraph::isEquivalent(AnnotatedGraph* secondOG) {
    trace(TRACE_5, "AnnotatedGraph::isEquivalent(AnnotatedGraph* secondOG): start\n");

    // Simulation is impossible without a simulant.
    if (secondOG == NULL) {
        return false;
    }

    if (secondOG->getRoot() == NULL) {
        return true;
    } else if (root == NULL) {
        return false;
    }

    // We need to remember the pairs of nodes we already visited.
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > visitedNodes;

    // Get things moving...
    bool result = false;
    if (isEquivalentRecursive(root, secondOG->getRoot(), visitedNodes, this,
            secondOG)) {
        result = true;
    }

    trace(TRACE_5, "AnnotatedGraph::isEquivalent(AnnotatedGraph* secondOG): end\n");
    return result;
}


//! \brief checks, whether the given two AnnotatedGraphNodes of this AnnotatedGraph
//! are equivalent
//! \return true on positive check, otherwise: false
//! \param leftNode the first AnnotatedGraphNode
//! \param rightNode the second AnnotatedGraphNode
bool AnnotatedGraph::isEquivalent(AnnotatedGraphNode* leftNode,
                                  AnnotatedGraphNode* rightNode) {
    trace(TRACE_5, "AnnotatedGraph::isEquivalent(AnnotatedGraphNode* leftNode, AnnotatedGraphNode* rightNode): start\n");

    assert(leftNode);
    assert(rightNode);

    // We need to remember the pairs of nodes we already visited.
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > visitedNodes;

    // Get things moving...
    bool result = false;
    if (isEquivalentRecursive(leftNode, rightNode, visitedNodes, this, this)) {
        result = true;
    }

    trace(TRACE_5, "AnnotatedGraph::isEquivalent(AnnotatedGraphNode* firstNode, AnnotatedGraphNode* secondNode): end\n");
    return result;
}


//! \brief checks, whether the part of an AnnotatedGraph below myNode is equivalent
//         to the part of an AnnotatedGraph below simNode
//! \return true on positive check, otherwise: false
//! \param leftNode a node in this AnnotatedGraph
//! \param rightNode a node in the second AnnotatedGraph
//! \param visitedNodes Holds all visited pairs of nodes.
//! \param leftOG the AnnotatedGraph corresponding to leftNode
//! \param rightOG the AnnotatedGraph corresponding to rightNode
bool AnnotatedGraph::isEquivalentRecursive(AnnotatedGraphNode* leftNode,
                                           AnnotatedGraphNode* rightNode,
                                           set<pair<AnnotatedGraphNode*,
                                           AnnotatedGraphNode*> >& visitedNodes,
                                           AnnotatedGraph* leftOG,
                                           AnnotatedGraph* rightOG) {

    // checking, whether myNode simulates simNode; result is true, iff
    // 1) anno of simNode implies anno of myNode and
    // 2) myNode has each outgoing event of simNode, too

    assert(leftNode);
    assert(rightNode);

    trace(TRACE_2, "\t checking whether node " + leftNode->getName());
    trace(TRACE_3, " of " + leftOG->getFilename());
    trace(TRACE_2, " is equivalent to node " + rightNode->getName());
    trace(TRACE_3, " of " + rightOG->getFilename());
    trace(TRACE_2, "\n");

    // If we already visited this pair of nodes, then we're done.
    if (visitedNodes.find(make_pair(leftNode, rightNode)) != visitedNodes.end()) {
        trace(TRACE_3, "\t already been checked\n");
        return true;
    } else {
        visitedNodes.insert(make_pair(leftNode, rightNode));
    }

    // iterator used for reporting and for recursively going down
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter;

    // remember whether equivalence of node is true/false for reporting
    bool result = true;

    // 1st step:
    // first we check equivalence of annotations: leftNode <-> rightNode
    trace(TRACE_3, "\t\t checking equivalence of annotations...\n");
    GraphFormulaCNF* leftNodeAnnotationInCNF = leftNode->getAnnotation()->getCNF();
    GraphFormulaCNF* rightNodeAnnotationInCNF = rightNode->getAnnotation()->getCNF();

    if (leftNodeAnnotationInCNF->implies(rightNodeAnnotationInCNF)) {
        trace(TRACE_3, "\t\t first annotation implication is ok\n");
        trace(TRACE_4, "\t\t\t   " + leftNode->getAnnotation()->asString() + "\n");
        trace(TRACE_4, "\t\t\t   ->\n");
        trace(TRACE_4, "\t\t\t   " + rightNode->getAnnotation()->asString() + "\n");

        if (rightNodeAnnotationInCNF->implies(leftNodeAnnotationInCNF)) {
            trace(TRACE_3, "\t\t second annotation implication is ok\n");
            trace(TRACE_4, "\t\t\t   " + rightNode->getAnnotation()->asString() + "\n");
            trace(TRACE_4, "\t\t\t   ->\n");
            trace(TRACE_4, "\t\t\t   " + leftNode->getAnnotation()->asString() + "\n");
        } else {
            trace(TRACE_3, "\t\t\t annotation implication false\n");
            trace(TRACE_4, "\t\t\t   " + rightNode->getAnnotation()->asString() + "\n");
            trace(TRACE_4, "\t\t\t   -/->\n");
            trace(TRACE_4, "\t\t\t   " + leftNode->getAnnotation()->asString() + "\n");

            result = false;
        }
    } else {
        trace(TRACE_2, "\t\t equivalence failed (annotation)\n");
        trace(TRACE_3, "\t\t\t annotation implication false\n");
        trace(TRACE_4, "\t\t\t   " + leftNode->getAnnotation()->asString() + "\n");
        trace(TRACE_4, "\t\t\t   -/->\n");
        trace(TRACE_4, "\t\t\t   " + rightNode->getAnnotation()->asString() + "\n");

        result = false;
    }

    delete rightNodeAnnotationInCNF;
    delete leftNodeAnnotationInCNF;

    if (result == false) {
        // reporting all leaving edges of both nodes for debugging
        AnnotatedGraphNode::LeavingEdges::Iterator edgeIter;

        trace(TRACE_4, "\t\t\t   leaving edges of node ");
        trace(TRACE_4, rightNode->getName() + " of ");
        trace(TRACE_4, rightOG->getFilename());
        trace(TRACE_4, " (" + intToString(rightNode->getLeavingEdgesCount())
                + "):\n");

        edgeIter = rightNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            trace(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> " + edge->getDstNode()->getName() + " (");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    trace(TRACE_4, "BLUE");
                    break;
                case RED:
                    trace(TRACE_4, "RED");
                    break;
            }
            trace(TRACE_4, ")\n");
        }

        trace(TRACE_4, "\t\t\t   leaving edges of node ");
        trace(TRACE_4, leftNode->getName() + " of ");
        trace(TRACE_4, leftOG->getFilename());
        trace(TRACE_4, " (" + intToString(leftNode->getLeavingEdgesCount()) + "):\n");

        edgeIter = leftNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            trace(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> " + edge->getDstNode()->getName() + "(");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    trace(TRACE_4, "BLUE");
                    break;
                case RED:
                    trace(TRACE_4, "RED");
                    break;
            }
            trace(TRACE_4, ")\n");
        }

        delete edgeIter;

        return false;
    }

    // 2nd step:
    // now we check whether both nodes have the same outgoing events
    trace(TRACE_3, "\t\t checking edges...\n");

    trace(TRACE_3, "\t\t\t checking whether each edge of ");
    trace(TRACE_3, leftNode->getName());
    trace(TRACE_3, " of " + leftOG->getFilename() + "\n");
    trace(TRACE_3, "\t\t\t                 is present at ");
    trace(TRACE_3, rightNode->getName());
    trace(TRACE_3, " of " + rightOG->getFilename() + "...\n");

    edgeIter = leftNode->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* leftEdge = edgeIter->getNext();
        trace(TRACE_4, "\t\t\t\t checking event " + leftEdge->getLabel() + "\n");

        AnnotatedGraphEdge* rightEdge = rightNode->getEdgeWithLabel(leftEdge->getLabel());
        if (rightEdge == NULL) {
            // first node has edge which second node hasn't
            trace(TRACE_4, "\t\t\t\t event missing\n");
            result = false;
            break;
        } else {
            trace(TRACE_4, "\t\t\t\t event present\n");
        }
    }

    trace(TRACE_3, "\t\t\t checking whether each edge of ");
    trace(TRACE_3, rightNode->getName());
    trace(TRACE_3, " of " + rightOG->getFilename() + "\n");
    trace(TRACE_3, "\t\t\t                 is present at ");
    trace(TRACE_3, leftNode->getName());
    trace(TRACE_3, " of " + leftOG->getFilename() + "...\n");

    edgeIter = rightNode->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* rightEdge = edgeIter->getNext();
        trace(TRACE_4, "\t\t\t\t checking event " + rightEdge->getLabel() + "\n");

        AnnotatedGraphEdge* leftEdge = leftNode->getEdgeWithLabel(rightEdge->getLabel());
        if (leftEdge == NULL) {
            // first node has edge which second node hasn't
            trace(TRACE_4, "\t\t\t\t event missing\n");
            result = false;
            break;
        } else {
            trace(TRACE_4, "\t\t\t\t event present\n");
        }
    }

    if (result == false) {
        trace(TRACE_2, "\t\t equivalence failed (edges)\n");
        // reporting all leaving edges of both nodes for debugging
        AnnotatedGraphNode::LeavingEdges::Iterator edgeIter;

        trace(TRACE_4, "\t\t\t   leaving edges of node ");
        trace(TRACE_4, rightNode->getName() + " of ");
        trace(TRACE_4, rightOG->getFilename());
        trace(TRACE_4, " (" + intToString(rightNode->getLeavingEdgesCount()) + "):\n");

        edgeIter = rightNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            trace(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> "
                    + edge->getDstNode()->getName() + " (");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    trace(TRACE_4, "BLUE");
                    break;
                case RED:
                    trace(TRACE_4, "RED");
                    break;
            }
            trace(TRACE_4, ")\n");
        }

        trace(TRACE_4, "\t\t\t   leaving edges of node ");
        trace(TRACE_4, leftNode->getName() + " of ");
        trace(TRACE_4, leftOG->getFilename());
        trace(TRACE_4, " (" + intToString(leftNode->getLeavingEdgesCount()) + "):\n");

        edgeIter = leftNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            trace(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> " + edge->getDstNode()->getName() + "(");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    trace(TRACE_4, "BLUE");
                    break;
                case RED:
                    trace(TRACE_4, "RED");
                    break;
            }
            trace(TRACE_4, ")\n");
        }

        delete edgeIter;
        return false;
    }

    // we know: both nodes have the same outgoing edges
    trace(TRACE_4, "\t\t\t all events present at both nodes, going down\n");

    edgeIter = leftNode->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* leftEdge = edgeIter->getNext();
        trace(TRACE_4, "\t\t\t performing event " + leftEdge->getLabel() + "\n");
        AnnotatedGraphEdge* rightEdge = rightNode->getEdgeWithLabel(leftEdge->getLabel());

        if (!isEquivalentRecursive(leftEdge->getDstNode(),
                                   rightEdge->getDstNode(),
                                   visitedNodes,
                                   leftOG,
                                   rightOG)) {
            delete edgeIter;
            return false;
        }
    }

    delete edgeIter;

    // All checks were successful.
    return true;
}


//! \brief takes an AnnotatedGraph, finds equivalent nodes and minimizes
//!        the graph such that it still characterizes the same strategies
void AnnotatedGraph::minimizeGraph() {
    trace(TRACE_5, "AnnotatedGraph::minimizeGraph(): start\n");
    cout << "starting minimization...\n" << endl;

    // false nodes removed to increase performance only
    // should also work if nodes are unsatisfiable
    trace(TRACE_1, "removing false nodes...\n");
    removeFalseNodes();

    if (hasNoRoot()) {
        trace("The annotated graph is empty. No need for further minimizing :)\n\n");
        return;
    }

    cout << "number of nodes: " << setOfNodes.size() << endl;


// 1) preparing a map of all pairs of nodes
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > nodePairs;
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > areEquivalent;
    map<AnnotatedGraphNode*, set<AnnotatedGraphNode*> > isEquivalentWith;

    nodes_t::const_iterator iNode, jNode;
    for (iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        for (jNode = iNode + 1; jNode != setOfNodes.end(); ++jNode) {
            nodePairs.insert(make_pair(*iNode, *jNode));
        }
    }
    cout << "number of node pairs: " << nodePairs.size() << "\n" << endl;


// 2) iterating each node pair which is to be considered
    // We need to remember the pairs of nodes we already visited.
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > visitedNodes;

    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> >::const_iterator iNodePairs;
    for (iNodePairs = nodePairs.begin(); iNodePairs != nodePairs.end(); ++iNodePairs) {
        // cout << "node pair: (" << ((*iNodePairs).first)->getName() << ", " << (*iNodePairs).second->getName() << ")" << endl;

        // naive lösung ab hier
        if (isEquivalentRecursive((*iNodePairs).first,
                                  (*iNodePairs).second,
                                  visitedNodes,
                                  this,
                                  this)) {
            cout << "nodes: (" << ((*iNodePairs).first)->getName();
            cout << ", " << (*iNodePairs).second->getName() << ")";// << endl;
            cout << "\t ...are equivalent :)" << endl;

            // remember that they are equivalent
            areEquivalent.insert(*iNodePairs);
            isEquivalentWith[(*iNodePairs).first].insert((*iNodePairs).second);
        } else {
            // cout << "\t ...are NOT equivalent :(" << endl;            
        }
        // naive lösung bis hier
    }
    cout << "number of equivalent pairs: " << areEquivalent.size() << "\n" << endl;

    // at this point each node knows all equivalent nodes that are BEHIND that node
    // in setOfNodes

    // reporting...
    cout << "\nnumber of equivalent pairs: " << areEquivalent.size() << "\n" << endl;    
    set<AnnotatedGraphNode*>::const_iterator lNode;
    for (iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if (isEquivalentWith[*iNode].size() == 0) {
            continue;
        } else {
            cout << "node " << (*iNode)->getName() << " is equivalent to nodes: ";
            for (lNode = isEquivalentWith[*iNode].begin();
                 lNode != isEquivalentWith[*iNode].end();
                 ++lNode) {
                cout << (*lNode)->getName() + ", ";
            }
            cout << endl;
        }
    }
    cout << "\n";


// 3) merging equivalent OG nodes
    // temporary variable storing all predecessors of current node
    nodes_t currentNodePredecessors;

    for (iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if (isEquivalentWith[*iNode].size() == 0) {
            continue;
        } else {
            // TODO: kann sein, dass root node equivalent zu anderen;
            // dann sind predecessors NULL ??
//            assert(getPredecessorNodes(*iNode).size() != 0);
//
//            // current node has equivalent nodes, so redirect its incoming edges
//            for (jNode = getPredecessorNodes(*iNode).begin();
//                 jNode != getPredecessorNodes(*iNode).end();
//                 ++jNode) {
//
//                cout << (*jNode)->getName() + ", ";
//            }
            cout << endl;
        }
    }

    // merging idea: take all incoming edges of current node,
    // redirect them to the first equivalent node behind this node in setOfNodes,
    // delete the current node.
    // works because if node has several equivalent nodes, the second one knows the third :)

    cout << "\nfinished minimization...\n" << endl;
    trace(TRACE_5, "AnnotatedGraph::minimizeGraph(): end\n");
}


//! \brief checks, whether this AnnotatedGraph simulates the given simulant
//!        while covering all interface transitions
//! \return true on positive check, otherwise: false
//! \param smallerOG the simulant that should be simulated
bool AnnotatedGraph::covSimulates(AnnotatedGraph* smallerOG) {
    trace(TRACE_5, "AnnotatedGraph::covSimulates(AnnotatedGraph *smallerOG): start\n");

    // Simulation is impossible without a simulant.
    if (smallerOG == NULL) {
        return false;
    }

    // We need to remember the pairs of nodes we already visited.
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > visitedNodes;

    if (smallerOG->getRoot() == NULL) {
        return true;
    } else if (root == NULL) {
        return false;
    }

    createCovConstraint(); // global coverability formula for normal form
    smallerOG->createCovConstraint();

    // Get things moving...
    bool result = false;
    if (covSimulatesRecursive(root, smallerOG->getRoot(), visitedNodes,
            covConstraint, smallerOG->covConstraint)) {
        result = true;
    }

    trace(TRACE_5, "AnnotatedGraph::covSimulates(AnnotatedGraph *smallerOG): end\n");
    return result;
}


//! \brief checks, whether the part of an AnnotatedGraph below myNode simulates
//         the part of an AnnotatedGraph below simNode while covering all interface transitions
//! \return true on positive check, otherwise: false
//! \param myNode a node in this AnnotatedGraph
//! \param simNode a node in the simulant
//! \param visitedNodes Holds all visited pairs of nodes.
bool AnnotatedGraph::covSimulatesRecursive(AnnotatedGraphNode *myNode,
                                           AnnotatedGraphNode *simNode,
                                           set<pair<AnnotatedGraphNode*,
                                           AnnotatedGraphNode*> >& visitedNodes,
                                           GraphFormulaCNF* myCovConstraint,
                                           GraphFormulaCNF* simCovConstraint) {

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

    // compute annotations for coverability uses
    myNode->createCovAnnotation(myCovConstraint);
    simNode->createCovAnnotation(simCovConstraint);

    // first we check implication of annotations: simNode -> myNode
    trace(TRACE_3, "\t\t checking annotations...\n");
    GraphFormulaCNF* simNodeCovAnnotationInCNF = simNode->getCovAnnotation()->getCNF();
    GraphFormulaCNF* myNodeCovAnnotationInCNF = myNode->getCovAnnotation()->getCNF();

    if (simNodeCovAnnotationInCNF->implies(myNodeCovAnnotationInCNF)) {
        trace(TRACE_3, "\t\t\t annotations ok\n");
        trace(TRACE_4, "\t\t\t   " + simNode->getAnnotation()->asString() + "\n");
        trace(TRACE_4, "\t\t\t   ->\n");
        trace(TRACE_4, "\t\t\t   " + myNode->getAnnotation()->asString() + "\n");

        delete simNodeCovAnnotationInCNF;
        delete myNodeCovAnnotationInCNF;
    } else {
        trace(TRACE_3, "\t\t\t annotation implication false\n");
        trace(TRACE_4, "\t\t\t   " + simNode->getAnnotation()->asString() + "\n");
        trace(TRACE_4, "\t\t\t   -/->\n");
        trace(TRACE_4, "\t\t\t   " + myNode->getAnnotation()->asString() + "\n");

        delete simNodeCovAnnotationInCNF;
        delete myNodeCovAnnotationInCNF;

        trace(TRACE_2, "\t\t simulation failed (annotation)\n");

        return false;
    }

    // now we check whether myNode has each outgoing event of simNode
    trace(TRACE_3, "\t\t checking edges...\n");
    AnnotatedGraphNode::LeavingEdges::ConstIterator simEdgeIter = simNode->getLeavingEdgesConstIterator();

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

            if (!covSimulatesRecursive(myEdge->getDstNode(),
                                       simEdge->getDstNode(),
                                       visitedNodes,
                                       myCovConstraint,
                                       simCovConstraint)) {

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

    if (rhsOG == NULL) {
        return;
    }

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
void AnnotatedGraph::filterRecursive(AnnotatedGraphNode* myNode,
                                     AnnotatedGraphNode* rhsNode,
                                     set<AnnotatedGraphNode*> *VisitedNodes) {
    trace(TRACE_5, "AnnotatedGraph::filterRecursive(...): begin\n");

    // nothing to be done
    if (myNode == NULL) {
        return;
    }

    // nothing to be done
    if (rhsNode == NULL) {
        return;
    }

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
    AnnotatedGraphNode::LeavingEdges::Iterator rhsEdgeIter = rhsNode->getLeavingEdgesIterator();
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

        AnnotatedGraphEdge* myEdge =
                myNode->getEdgeWithLabel(rhsEdge->getLabel());
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

        AnnotatedGraphEdge* myEdge =
                myNode->getEdgeWithLabel(rhsEdge->getLabel());
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

    if (hasNoRoot()) {
        trace(TRACE_0, "The Graph is empty and thus acyclic!");
        return true;
    }

    testNodes.push(root);
    AnnotatedGraphNode* testNode;

    // While there are still nodes in the queue
    while (!testNodes.empty()) {

        testNode = testNodes.front();
        testNodes.pop();

        // A node counts as a parent node to it self for the purpose of cycles
        parentNodes[testNode].insert(testNode);

        // Iterate all transitions of that node
        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =
                testNode->getLeavingEdgesConstIterator();

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
    map<AnnotatedGraphNode*, list <set<AnnotatedGraphNode*> > >
            validFollowerCombinations;
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
                possibleAssignment, (*iNode), assignmentList);

        // create a temporary variable for a set of nodes
        set<AnnotatedGraphNode*> followerNodes;

        // for every true assignment in the list a set of nodes will be created. These are the nodes which are
        // reached by outgoing edges of which the labels were true in the assignment. This set is then saved in
        // a map for the currently proceeded node.
        for (list<GraphFormulaAssignment>::iterator assignment =
                assignmentList.begin(); assignment != assignmentList.end(); assignment++) {

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
            validFollowerCombinations, eliminateRedundantCounting, instances);

    if (instances > 100000) {
        trace(TRACE_2, "Valid Number of instances exceeded.\n");
        trace(
                TRACE_0,
                "The number of strategies is approx INFINITY ;), aborting further calculation.\n");
        trace(TRACE_5, "AnnotatedGraph::numberOfServices(...): end\n");
        return number;
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
unsigned int AnnotatedGraph::numberOfServicesRecursively(
        set<AnnotatedGraphNode*> activeNodes,
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
                for (list<set<AnnotatedGraphNode*> >::iterator combination =
                        validFollowerCombinations[(*activeNode)].begin(); combination
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
                for (list<set<AnnotatedGraphNode*> >::iterator oldListSet =
                        oldList.begin(); oldListSet != oldList.end(); oldListSet++) {

                    for (list<set<AnnotatedGraphNode*> >::iterator combination =
                            validFollowerCombinations[(*activeNode)].begin(); combination
                            != validFollowerCombinations[(*activeNode)].end(); combination++) {

                        tempSet = set<AnnotatedGraphNode*>();

                        for (set<AnnotatedGraphNode*>::iterator insertionNode =
                                (*combination).begin(); insertionNode
                                != (*combination).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        for (set<AnnotatedGraphNode*>::iterator insertionNode =
                                (*oldListSet).begin(); insertionNode
                                != (*oldListSet).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        newList.push_back(tempSet);
                    }
                }
                usingNew = false;
            } else {

                oldList.clear();
                for (list<set<AnnotatedGraphNode*> >::iterator newListSet =
                        newList.begin(); newListSet != newList.end(); newListSet++) {

                    for (list<set<AnnotatedGraphNode*> >::iterator combination =
                            validFollowerCombinations[(*activeNode)].begin(); combination
                            != validFollowerCombinations[(*activeNode)].end(); combination++) {

                        tempSet = set<AnnotatedGraphNode*>();

                        for (set<AnnotatedGraphNode*>::iterator insertionNode =
                                (*combination).begin(); insertionNode
                                != (*combination).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        for (set<AnnotatedGraphNode*>::iterator insertionNode =
                                (*newListSet).begin(); insertionNode
                                != (*newListSet).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }
                        oldList.push_back(tempSet);
                    }
                }
                usingNew = true;
            }
        } else {
            bool valid = false;
            for (list<set<AnnotatedGraphNode*> >::iterator combination =
                    validFollowerCombinations[(*activeNode)].begin(); combination
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
        for (list<set<AnnotatedGraphNode*> >::iterator oldListSet =
                oldList.begin(); oldListSet != oldList.end(); oldListSet++) {

            number += numberOfServicesRecursively((*oldListSet), followers,
                    validFollowerCombinations, eliminateRedundantCounting,
                    instances);
        }
    } else {
        for (list<set<AnnotatedGraphNode*> >::iterator newListSet =
                newList.begin(); newListSet != newList.end(); newListSet++) {

            number += numberOfServicesRecursively((*newListSet), followers,
                    validFollowerCombinations, eliminateRedundantCounting,
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
    trace(TRACE_5,
            "AnnotatedGraph::product(const AnnotatedGraph* rhs): start\n");

    // this will be the product OG
    AnnotatedGraph* productOG= new AnnotatedGraph;

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
    AnnotatedGraphNode* productNode= new AnnotatedGraphNode(currentName, currentFormula);
    productOG->addNode(productNode);
    productOG->setRoot(productNode);

    // builds the successor nodes of the root nodes of OG and rhs OG
    // therefore, we perform a coordinated dfs through OG and the rhs OG
    buildProductOG(currentOGNode, currentRhsNode, productOG);

    if (!parameters[P_DIAGNOSIS]) {
        productOG->removeFalseNodes();
    }

    trace(TRACE_5, "AnnotatedGraph::product(const AnnotatedGraph* rhs): end\n");

    return productOG;
}


//! \brief Returns the product OG of all given OGs. The caller has to delete the AnnotatedGraph
//! \param ogs a list of OGs
//! \return returns the product OG
AnnotatedGraph* AnnotatedGraph::product(const ogs_t& ogs) {
    assert(ogs.size()> 1);

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
        AnnotatedGraphNode* currentRhsNode, AnnotatedGraph* productOG) {

    trace(
            TRACE_5,
            "AnnotatedGraph::buildProductOG(AnnotatedGraphNode* currentOGNode, AnnotatedGraphNode* currentRhsNode, AnnotatedGraph* productOG): start\n");

    // at this time, the node constructed from currentOGNode and
    // currentRhsNode is already inserted

    assert(productOG->getRoot() != NULL);

    // iterate over all outgoing edges from current node of OG
    std::string currentLabel;
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =
            currentOGNode->getLeavingEdgesConstIterator();

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
            AnnotatedGraphNode* found =
                    productOG->getNodeWithName(newProductName);

            if (found != NULL) {
                // the node was known before, so we just have to add a new edge
                productOG->addEdge(currentName, newProductName, currentLabel);

                trace(
                        TRACE_5,
                        "AnnotatedGraph::buildProductOG(AnnotatedGraphNode* currentOGNode, AnnotatedGraphNode* currentRhsNode, AnnotatedGraph* productOG): end\n");
            } else {
                // we computed a new node, so we add a node and an edge
                // trace(TRACE_0, "adding node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + "\n");

                GraphFormulaCNF* newProductFormula = createProductAnnotation(
                        newOGNode, newRhsNode);

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
GraphFormulaCNF* AnnotatedGraph::createProductAnnotation(
        const AnnotatedGraphNode* lhs, const AnnotatedGraphNode* rhs) const {

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
void AnnotatedGraph::printGraphToDot(AnnotatedGraphNode* v, fstream& os,
        std::map<AnnotatedGraphNode*, bool>& visitedNodes) const {

    if (v == NULL) {
        // print the empty OG...
        os << "p0"
                << " [label=\"#0\", fontcolor=black, color=red, style=dashed];\n";
        return;
    }

    if (visitedNodes[v] != true) {

        bool finalNode = false;

        for (nodes_t::const_iterator checkNode = finalNodes.begin(); checkNode
                != finalNodes.end(); checkNode++) {
            if ((*checkNode) == v) {
                finalNode = true;
            }
        }

        if (finalNode) {
            os << "p"<< v->getName() << " [label=\"# "<< v->getName() << "\\n";
            os << v->getAnnotation()->asString();
            os << "\", fontcolor=black, color=blue, peripheries=2];\n";
        } else {
            os << "p"<< v->getName() << " [label=\"# "<< v->getName() << "\\n";
            os << v->getAnnotation()->asString();
            os << "\", fontcolor=black, color=blue];\n";
        }

        visitedNodes[v] = true;

        std::string currentLabel;

        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =
                v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();

            // remember the label of the egde
            currentLabel = edge->getLabel();
            AnnotatedGraphNode* successor =
                    v->followEdgeWithLabel(currentLabel);
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
        ogFile << "NODES" << endl << "  0 : " << GraphFormulaLiteral::FALSE
                << " : " << GraphNodeColor(RED).toString() << ';' << endl << endl << "INITIALNODE" << endl
                << "  0;" << endl << endl << "TRANSITIONS" << endl << "  ;"
                << endl;

        ogFile.close();
        return;
    }

    ogFile << "NODES" << endl;
    bool printedFirstNode = false;
    for (nodes_t::const_iterator iNode = setOfNodes.begin(); iNode
            != setOfNodes.end(); ++iNode) {

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
    for (nodes_t::const_iterator iNode = setOfNodes.begin(); iNode
            != setOfNodes.end(); ++iNode) {

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

            ogFile << "  " << node->getName() << " -> " << edge->getDstNode()->getName() << " : " << edge->getLabel();

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

    // itertate over all nodes
    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        AnnotatedGraphNode::LeavingEdges::Iterator iEdge = (*iNode)->getLeavingEdgesIterator();

        // iterate over all transitions from a node
        while (iEdge->hasNext()) {
            AnnotatedGraphEdge* edge = iEdge->getNext();
            // create a transition named "srcNode@label@destNode"
            tm[edge->getLabel()].insert((*iNode)->getName() + "@"+ edge->getLabel() + "@"
                    + edge->getDstNode()->getName());
        }
    }

    trace(TRACE_3, "AnnotatedGraph::getTransitionMap()::end()\n");
    return tm;
}


//! \brief Get all transitions from the graph with a label from the given label set, 
//! each associated to a specific label
//! return returns the transition map
AnnotatedGraph::TransitionMap AnnotatedGraph::getTransitionMap(
        set<string>* labels) {
    trace(TRACE_3,
            "AnnotatedGraph::getTransitionMap(set<string>* labels)::begin()\n");
    TransitionMap tm;

    // itertate over all nodes
    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        AnnotatedGraphNode::LeavingEdges::Iterator iEdge = (*iNode)->getLeavingEdgesIterator();

        // iterate over all transitions from a node
        while (iEdge->hasNext()) {
            GraphEdge* edge = iEdge->getNext();
            if (labels->find(edge->getLabel()) != labels->end()) { // label found in subset?
                // create a transition named "srcNode@label@destNode"
                tm[edge->getLabel()].insert((*iNode)->getName() + "@"+ edge->getLabel() + "@"
                        + edge->getDstNode()->getName());
            }
        }
    }

    trace(TRACE_3,
            "AnnotatedGraph::getTransitionMap(set<string>* labels)::end()\n");
    return tm;
}


//! \brief Create the formula describing the coverability criteria when covering labels from the given set
//! \param labels the set that containts all the events that shall be covered; omitting is equal NULL and refers to
//! covering the whole interface set
//! \return returns the coverability formula 
void AnnotatedGraph::createCovConstraint(set<string>* labels) {
    trace(TRACE_3, "AnnotatedGraph::createCovFormula()::begin()\n");

    GraphFormulaCNF* formula = new GraphFormulaCNF;
    TransitionMap tm;

    if (labels) {
        // using subset of label set
        tm = getTransitionMap(labels);
    } else {
        // using whole label set
        tm = getTransitionMap();
    }

    // iterate over all transitions
    for (TransitionMap::iterator i = tm.begin(); i != tm.end(); i++) {
        GraphFormulaMultiaryOr *clause= new GraphFormulaMultiaryOr;
        // for each transition with the same label, create a disjunction
        for (EdgeSet::iterator j = i->second.begin(); j != i->second.end(); j++) {
            GraphFormulaLiteral *literal= new GraphFormulaLiteral(*j);
            clause->addSubFormula(literal);
        }

        // add disjunction to cnf
        formula->addClause(clause);
    }

    covConstraint = formula;

    trace(TRACE_3, "AnnotatedGraph::createCovFormula(): " + formula->asString()
            + "\n");
    trace(TRACE_3, "AnnotatedGraph::createCovFormula()::end()\n");
}


//! \brief Create the formula describing the event-structure of the graph
//! NOTE: the graph has to be acyclic!
//! \return returns the structure formula
GraphFormulaMultiaryAnd *AnnotatedGraph::createStructureFormula() {
    trace(TRACE_3, "AnnotatedGraph::createStructureFormula()::begin()\n");

    assert(isAcyclic());

    GraphFormulaMultiaryAnd *formula =
            createStructureFormulaRecursively(getRoot());

    trace(TRACE_3, "AnnotatedGraph::createStructureFormula(): "
            + formula->asString() + "\n");
    trace(TRACE_3, "AnnotatedGraph::createStrctureFormula()::end()\n");
    return formula;
}


//! \brief Creates the formula describing the event-structure of the subgraph under node
//! NOTE: the subgraph has to be acyclic!
//! \param node the root of the subgraph
//! \return returns the structure formula of the subgraph
GraphFormulaMultiaryAnd *AnnotatedGraph::createStructureFormulaRecursively(
        AnnotatedGraphNode *node) {
    trace(TRACE_5,
            "AnnotatedGraph::createStructureFormulaRecursively()::begin()\n");

    GraphFormulaMultiaryAnd *formula= new GraphFormulaMultiaryAnd;

    if (node == NULL) {
        return formula;
    }

    GraphFormulaCNF *annotation = node->getAnnotation();
    for (GraphFormulaMultiaryAnd::iterator i = annotation->begin(); i
            != annotation->end(); i++) {

        GraphFormulaMultiaryOr* clause =
                dynamic_cast<GraphFormulaMultiaryOr*>(*i);
        GraphFormulaMultiaryOr* new_clause= new GraphFormulaMultiaryOr;
        // iterate over disjunctive clauses
        for (GraphFormulaMultiaryOr::iterator j = clause->begin(); j
                != clause->end(); j++) {

            GraphFormulaLiteral* lit = dynamic_cast<GraphFormulaLiteral*>(*j);
            GraphFormulaLiteral* new_lit;
            if ((dynamic_cast<GraphFormulaFixed*>(*j) == NULL)
                    && (dynamic_cast<GraphFormulaLiteralFinal*>(*j) == NULL)) {
                new_lit = new GraphFormulaLiteral(node->getName() + "@" + lit->asString() + "@"
                        + node->followEdgeWithLabel(lit->asString())->getName());
            } else {
                new_lit = new GraphFormulaLiteral(lit->asString());
            }
            GraphFormulaMultiaryAnd* new_and= new GraphFormulaMultiaryAnd(new_lit,
                    createStructureFormulaRecursively(node->followEdgeWithLabel(lit->asString())));
            new_clause->addSubFormula(new_and);
        }
        formula->addSubFormula(new_clause);
    }

    trace(TRACE_5, "AnnotatedGraph::createStructureFormulaRecursively(): "
            + formula->asString() + "\n");
    trace(TRACE_5,
            "AnnotatedGraph::createStructureFormulaRecursively()::end()\n");
    return formula;
}


//! \brief remove a node from the AnnotatedGraph
//! \param node node to remove
void AnnotatedGraph::removeNode(AnnotatedGraphNode* node) {

    trace(TRACE_5, "AnnotatedGraph::removeNode(): start\n");
    assert(node);

    for (vector<AnnotatedGraphNode*>::iterator testnode = setOfNodes.begin(); testnode
            != setOfNodes.end(); testnode++) {
        if ((*testnode) == node) {
            setOfNodes.erase(testnode);
            break;
        }
    }
    trace(TRACE_5, "AnnotatedGraph::removeNode(): end\n");

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
            trace(TRACE_2, "        Removing node: " + currNode->getName());
            trace(TRACE_3, "\n            Annotation: "
                    +currNode->getAnnotationAsString());
            trace(TRACE_2, "\n");

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
        AnnotatedGraphNode::LeavingEdges::Iterator edge_iter =
                currNode->getLeavingEdgesIterator();

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

    // NEEDS COMPLETLY NEW COMMENTING DUE TO SEVERE CHANGES

    // prepare deadlock creation
    AnnotatedGraphNode* deadlock= NULL;
    map<std::string, AnnotatedGraphNode*> deadlockMap;
    unsigned int currNumberOfDeadlocks = 0;

    // prepare sinkNode creation 
    AnnotatedGraphNode* sinkNode= NULL;
    bool addSink = false;

    // gather created nodes in order to add it to the graph
    set<AnnotatedGraphNode*> createdNodes;

    // save all nodes where 2ndFixB has been applied, so they are not made final
    set<AnnotatedGraphNode*> FixBNodes;

    if (!options[O_PV_MULTIPLE_DEADLOCKS]) {
        // create new deadlock node
        deadlock = new AnnotatedGraphNode("deadlock", new GraphFormulaFixed(false, GraphFormulaLiteral::FALSE));
    }

    // iterate over all nodes of the AnnotatedGraph
    for (nodes_iterator node_iter = setOfNodes.begin(); node_iter
            != setOfNodes.end(); ++node_iter) {

        AnnotatedGraphNode* currNode = *node_iter;

        // 1st fix: if !a is disabled in q of OG, add ?a->Deadlock in q of dual service
        // preparation: assume all receive events are disabled in current node (dual service)
        set<std::string> disabledRecvEvents = recvEvents;

        // 2nd fix A: if ?z is enabled in q of OG and no ?-literals occur in the
        //	annotation of q create a new node without ?z and a tau-transition
        // preparation: assume we don't have to apply the 2ndA fix in current node
        bool apply2ndFixA = false;

        // 2nd fix B: if there are no ?-literals in the annotation of q, if there is at
        // least one outgoing transition and if q has "final" in it's annotation, create
        // a new final node q_tau in the pvsa and a tau-transition q -> q_tau 
        // preparation: assume we don't have to apply the 2ndB fix in current node        
        bool apply2ndFixB = false;

        // 3rd fix: if q has "final" in it's annotation and there are only outgoing ?-Transition
        // create a new final node q' in the pvsa and a tau-transition q -> q_tau
        // Note: These final nodes can be merged into one, which is then called "sink"
        bool apply3rdFix = false;

        // preparation: we need to store the leaving edges for the new node somewhere
        set<AnnotatedGraphEdge*> newNodesEdges;
        AnnotatedGraphNode::LeavingEdges::Iterator edge_iter =
                currNode->getLeavingEdgesIterator();
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

            // Simplify the annotation so only literals that matter are left over
            currNode->getAnnotation()->simplify();
            std::string currAnnotation = currNode->getAnnotation()->asString();

            // Check whether the 2ndFix B has to be applied
            if (currAnnotation.find("?") == std::string::npos) {
                if (!(currAnnotation.find("final") == std::string::npos)) {
                    apply2ndFixB = true;
                    FixBNodes.insert(currNode);
                }
            }

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
                    // check whether any ?-literals occur in the annotation
                    if (currAnnotation.find("?") == std::string::npos) {
                        // no ?-literals occur in the annotation
                        // we have to apply the 2nd A fix at this node
                        apply2ndFixA = true;
                        trace(TRACE_5, "need to apply 2nd fix A at node "
                                + currNode->getName() + "\n");

                        // erase current transition from the set of transitions of the new node
                        newNodesEdges.erase(currEdge);
                    }

                    // check whether the 3rd Fix needs to be applied
                    if (!(currAnnotation.find("final") == std::string::npos)
                            && !(currAnnotation.find(currLabel)
                                    == std::string::npos)) {
                        apply3rdFix = true;

                        // the sink node will be added to the nodes of the graph at the end
                        addSink = true;
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

        // This should never happen, still it has not been proven yet
        if ((apply2ndFixA && apply2ndFixB)) {
            trace(TRACE_1, "Conflict between 2ndFixA and 2ndFixB!");
            apply2ndFixA = false;
        }

        // iterate over all disabled receive events
        // if 1st fix needs to be applied, the set of disabled receive events is not empty
        for (set<std::string>::iterator event_iter = disabledRecvEvents.begin(); event_iter
                != disabledRecvEvents.end(); ++event_iter) {

            if (options[O_PV_MULTIPLE_DEADLOCKS]) {
                // O_PV_MULTIPLE_DEADLOCKS is set
                // try to read deadlock node from the mapping event->node
                deadlock = deadlockMap[*event_iter];
                // check whether the deadlock node could be determined
                if (deadlock == NULL) {
                    // deadlock node could not be determined
                    // create new deadlock node
                    deadlock = new AnnotatedGraphNode("deadlock" +
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

        // check whether the 2nd fix A needs to be applied at this node
        if (apply2ndFixA) {
            // 2nd fix A needs to be applied at this node
            trace(TRACE_2, "        applying 2nd fixA at node "
                    + currNode->getName() + "\n");
            // create new node
            // as there exists a leaving edge representing a send event in currNode,
            //   currNode cannot be annotated with final (same goes for newNode)
            AnnotatedGraphNode* newNode= new AnnotatedGraphNode(currNode->getName() + "_tau",
                    new GraphFormulaFixed(true, ""), currNode->getColor());
            trace(TRACE_3, "            created new node " + newNode->getName()
                    + "\n");
            // create tau transition from current to new node
            AnnotatedGraphEdge* tauTransition= new AnnotatedGraphEdge(newNode, GraphFormulaLiteral::TAU);
            currNode->addLeavingEdge(tauTransition);
            trace(TRACE_4, "            created tau transition from "
                    + currNode->getName() +" to "+ newNode->getName() + "\n");

            // add current node's leaving edges to new node 
            //   except those that don't occur in the annotation
            trace(TRACE_4, "            adding leaving edges to new node\n");
            for (set<AnnotatedGraphEdge*>::iterator edge_iter =
                    newNodesEdges.begin(); edge_iter != newNodesEdges.end(); ++edge_iter) {
                // create new leaving edge
                AnnotatedGraphEdge* newEdge= new AnnotatedGraphEdge((*edge_iter)->getDstNode(),
                        (*edge_iter)->getLabel());
                newNode->addLeavingEdge(newEdge);
                trace(TRACE_5, "            adding edge " + newEdge->getLabel()
                        +" from " + newNode->getName() + " to "
                        +newEdge->getDstNode()->getName() + "\n");
            }

            // add disabled receive events from 1st fix to new node
            for (set<std::string>::iterator event_iter =
                    disabledRecvEvents.begin(); event_iter
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
                trace(TRACE_5, "                adding edge "
                        + disabledEvent->getLabel() +" from "
                        + newNode->getName() + " to " + deadlock->getName()
                        + "\n");
            }

            trace(TRACE_4, "        successfully applied 2nd fix at node "
                    + currNode->getName() + "\n");

            // insert node into the set of created nodes
            createdNodes.insert(newNode);
        }

        // check whether the 2nd fix B needs to be applied at this node
        if (apply2ndFixB) {
            // 2nd fix B needs to be applied at this node
            trace(TRACE_2, "        applying 2nd fix B at node "
                    + currNode->getName() + "\n");
            // create new node
            // this node is a final node of the pvsa
            AnnotatedGraphNode* newNode= new AnnotatedGraphNode(currNode->getName() + "_tau",
                    new GraphFormulaLiteralFinal(), currNode->getColor());

            trace(TRACE_3, "            created new node " + newNode->getName()
                    + "\n");

            // create tau transition from current to new node
            AnnotatedGraphEdge* tauTransition= new AnnotatedGraphEdge(newNode, GraphFormulaLiteral::TAU);
            currNode->addLeavingEdge(tauTransition);
            trace(TRACE_4, "            created tau transition from "
                    + currNode->getName() +" to "+ newNode->getName() + "\n");

            // add current node's leaving edges to new node 
            //   except those that don't occur in the annotation
            trace(TRACE_4, "            adding leaving edges to new node\n");
            for (set<AnnotatedGraphEdge*>::iterator edge_iter =
                    newNodesEdges.begin(); edge_iter != newNodesEdges.end(); ++edge_iter) {
                // create new leaving edge
                AnnotatedGraphEdge* newEdge= new AnnotatedGraphEdge((*edge_iter)->getDstNode(),
                        (*edge_iter)->getLabel());
                newNode->addLeavingEdge(newEdge);
                trace(TRACE_5, "                adding edge "
                        + newEdge->getLabel() +" from " + newNode->getName()
                        + " to "+newEdge->getDstNode()->getName() + "\n");
            }
            // add disabled receive events from 1st fix to new node
            for (set<std::string>::iterator event_iter =
                    disabledRecvEvents.begin(); event_iter
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
                trace(TRACE_5, "                adding edge "
                        + disabledEvent->getLabel() +" from "
                        + newNode->getName() + " to " + deadlock->getName()
                        + "\n");
            }

            trace(TRACE_3, "        successfully applied 2nd fix at node "
                    + currNode->getName() + "\n");
            createdNodes.insert(newNode);
        }

        // check whether the 3rd fix needs to be applied at this node
        if (apply3rdFix) {
            // 3rd fix needs to be applied at this node
            trace(TRACE_2, "        applying 3rd fix at node "
                    + currNode->getName() + "\n");
            // create new node
            // as there exists a leaving edge representing a send event in currNode,
            //   currNode cannot be annotated with final (same goes for newNode)
            AnnotatedGraphNode* newNode= new AnnotatedGraphNode(currNode->getName() + "_tau",
                    new GraphFormulaLiteralFinal(), currNode->getColor());
            trace(TRACE_3, "            created new node " + newNode->getName()
                    + "\n");
            sinkNode = newNode;

            // create tau transition from current to new node
            AnnotatedGraphEdge* tauTransition= new AnnotatedGraphEdge(sinkNode, GraphFormulaLiteral::TAU);
            currNode->addLeavingEdge(tauTransition);
            trace(TRACE_4, "            created tau transition from "
                    + currNode->getName() +" to "+ sinkNode->getName() + "\n");

            // add current node's leaving edges to new node 
            //   except those that don't occur in the annotation
            trace(TRACE_4, "            adding leaving edges to new node\n");
            for (set<AnnotatedGraphEdge*>::iterator edge_iter =
                    newNodesEdges.begin(); edge_iter != newNodesEdges.end(); ++edge_iter) {

                // create new leaving edge
                if ((*edge_iter)->getType() == RECEIVING) {
                    AnnotatedGraphEdge* newEdge= new AnnotatedGraphEdge((*edge_iter)->getDstNode(),
                            (*edge_iter)->getLabel());
                    sinkNode->addLeavingEdge(newEdge);
                    trace(TRACE_5, "                adding edge "
                            + newEdge->getLabel() +" from "
                            + sinkNode->getName() + " to "+newEdge->getDstNode()->getName() + "\n");
                }
            }

            // add disabled receive events from 1st fix to new node
            for (set<std::string>::iterator event_iter =
                    disabledRecvEvents.begin(); event_iter
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
                sinkNode->addLeavingEdge(disabledEvent);
                trace(TRACE_5, "                adding edge "
                        + disabledEvent->getLabel() +" from "
                        + sinkNode->getName() + " to " + deadlock->getName()
                        + "\n");
            }

            trace(TRACE_3, "        successfully applied 3rd fix at node "
                    + currNode->getName() + "\n");

            // Add the sink node of this specific current node to the created nodes
            createdNodes.insert(sinkNode);
        }

        // check whether the current node is annotated with final
        string::size_type loc =(currNode->getAnnotationAsString()).find(GraphFormulaLiteral::FINAL, 0);
        if ((loc != string::npos) && !hasSendingTransitions
                && FixBNodes.find(currNode)==FixBNodes.end()) {
            // current node is annotated with final and has no sending transitions
            // and is not a node that was already fixed by 2nd Fix B
            // reduce annotation to final only (mark final state of the automaton)

            GraphFormulaMultiaryOr* myclause= new GraphFormulaMultiaryOr();
            myclause->addSubFormula(new GraphFormulaLiteralFinal());
            currNode->setAnnotation(new GraphFormulaCNF(myclause));
        } else {
            // current node is not annotated with final or has sending transitions
            // or was already fixed by 2nd Fix B, remove annotation of the current node
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

    // add the sink node in case it was used and if there was only one sink for the whole pvsa
    if (!(options[O_PV_MULTIPLE_DEADLOCKS]) && addSink) {
        createdNodes.insert(sinkNode);
    }

    // add tau transition destination nodes
    trace(TRACE_3,
            "\n        inserting all newly created nodes into the graphs nodeset...\n");

    for (set<AnnotatedGraphNode*>::iterator n = createdNodes.begin(); n
            != createdNodes.end(); ++n) {
        this->addNode((*n));
    }
}


//! \brief transforms the graph into its public view
void AnnotatedGraph::transformToPublicView(Graph* cleanPV) {
    trace(TRACE_1, "    removing nodes...\n");
    removeFalseNodes();
    removeNodesAnnotatedWithTrue();
    trace(TRACE_2, "\n");

    trace(TRACE_1, "    constructing dual service...\n");
    constructDualService();

    trace(TRACE_1, "    fixing dual service...\n");
    fixDualService();

    trace(TRACE_0, "\nStatistics of the public view service automaton: \n");
    trace(TRACE_0, "  nodes: " + intToString(setOfNodes.size()) + "\n");
    unsigned int edges = 0;
    for (nodes_iterator nodeIter = setOfNodes.begin(); nodeIter
            != setOfNodes.end(); ++nodeIter) {

        edges += (*nodeIter)->getLeavingEdgesCount();
    }
    trace(TRACE_0, "  edges: " + intToString(edges) + "\n\n");

    trace(TRACE_3, "internal translation from OG class to graph class...\n");
    transformOGToService(cleanPV);
    trace(TRACE_3, "\n");
}


//! \brief transforms the public view modified OG to a Service
void AnnotatedGraph::transformOGToService(Graph* cleanPV) {

    map<AnnotatedGraphNode*, GraphNode*> nodeMap;

    for (nodes_t::iterator copyNode = setOfNodes.begin(); copyNode
            != setOfNodes.end(); copyNode++) {

        GraphNode* copiedNode;

        std::string stringVal;
        stringVal.assign((*copyNode)->getName());
        GraphNodeColor colorVal = (*copyNode)->getColor();
        unsigned int numberVal = (*copyNode)->getNumber();

        copiedNode = new GraphNode(stringVal, colorVal, numberVal);

        nodeMap[(*copyNode)] = copiedNode;

        cleanPV->addNode(copiedNode);

        if (root == (*copyNode)) {
            cleanPV->setRoot(copiedNode);
        }

        if (((*copyNode)->getAnnotationAsString()).find(GraphFormulaLiteral::FINAL, 0) != string::npos) {
            cleanPV->makeNodeFinal(copiedNode);
        }

        trace(TRACE_4, "    copied node: " + stringVal + "\n");
    }

    for (nodes_t::iterator copyNode = setOfNodes.begin(); copyNode
            != setOfNodes.end(); copyNode++) {

        AnnotatedGraphNode::LeavingEdges::Iterator edge_iter = (*copyNode)->getLeavingEdgesIterator();
        while (edge_iter->hasNext()) {
            AnnotatedGraphEdge* edge = edge_iter->getNext();
            nodeMap[(*copyNode)]->addLeavingEdge(new GraphEdge( nodeMap[(edge)->getDstNode()], (edge)->getLabel()));
        }
    }

}


// CODE FROM CG FOR STRUCTURAL REDUCTION
void AnnotatedGraph::reduceStructurally() {
    bool isDiamond(AnnotatedGraphNode * node,
            AnnotatedGraph::predecessorMap& predecessors);

    trace(TRACE_5, "AnnotatedGraph::reduceStructurally(): start\n");

    std::map< AnnotatedGraphNode*, bool> processed;
    std::queue< AnnotatedGraphNode*> nodeQueue;

    AnnotatedGraph::predecessorMap predecessors;
    getPredecessorRelation(predecessors);

    nodeQueue.push(getRoot());

    while (!nodeQueue.empty()) {
        AnnotatedGraphNode * node = nodeQueue.front();
        cerr << "testing diamond for node " << node->getNumber() << endl;
        nodeQueue.pop();
        processed[node] = true;
        isDiamond(node, predecessors);
    }

    trace(TRACE_5, "AnnotatedGraph::reduceStructurally(): end\n");
}


bool isDiamond(AnnotatedGraphNode * node,
        AnnotatedGraph::predecessorMap& predecessors) {
    // check for simple diamomd
    std::queue< AnnotatedGraphNode*> testNodes;
    std::map< AnnotatedGraphNode*, bool> inQueue;
    std::list< AnnotatedGraphNode*> diamond;
    std::map< AnnotatedGraphNode*, bool> seen;
    std::map< AnnotatedGraphNode*, std::multiset< std::string> > messages;
    AnnotatedGraphNode* q_end= NULL;

    testNodes.push(node);
    inQueue[node] = true;

    while (!testNodes.empty()) {
        AnnotatedGraphNode * dnode = testNodes.front();
        testNodes.pop();
        inQueue[dnode] = false;

        cerr << " -- testing node " << dnode->getNumber() << endl;

        // if not the first node
        if (!diamond.empty()) {

            cerr << "    +- not the first node " << dnode->getNumber() << endl;

            AnnotatedGraphNode::LeavingEdges::ConstIterator preNodes =
                    predecessors[dnode].getConstIterator();

            cerr << "    | +- checking predecessor nodes:" << endl;
            while (preNodes->hasNext() ) {
                AnnotatedGraphEdge * edge = preNodes->getNext();
                AnnotatedGraphNode * preNode = edge->getDstNode();
                cerr << "    | | +- " << preNode->getNumber() << endl;
                std::string event = edge->getLabel();
                GraphFormulaCNF * preFormula = preNode->getAnnotation()->getDeepCopy();
                GraphFormulaCNF * formula = dnode->getAnnotation()->getDeepCopy();

                if ( !seen[preNode]) {
                    return false;
                }
                cerr << "    | | |    seen before ..." << endl;

                // compare annotations of the nodes
                //				cerr << event << endl;
                //				cerr << preFormula->asString() << endl;
                preFormula->removeLiteral(event);
                //				cerr << preFormula->asString() << endl;
                //				cerr << formula->asString() << endl;
                formula->removeLiteral(event);
                //				cerr << formula->asString() << endl;

                if ( !formula->implies(preFormula)
                        || !preFormula->implies(formula)) {
                    return false;
                }
                cerr << "    | | |    annotation okay ..." << endl;

                if (messages[dnode].empty()) {
                    messages[dnode] = messages[preNode];
                    messages[dnode].insert(edge->getLabelWithoutPrefix());
                } else {
                    std::multiset< std::string> secset = messages[preNode];
                    secset.insert(edge->getLabelWithoutPrefix());
                    if (secset != messages[dnode]) {
                        return false;
                    }
                }
                cerr << "    | | |    message multiset okay ..." << endl;
            }
            delete preNodes;
        }
        diamond.push_back(dnode);
        seen[dnode] = true;

        AnnotatedGraphNode::LeavingEdges::ConstIterator edge =
                dnode->getLeavingEdgesConstIterator();
        while (edge->hasNext()) {
            AnnotatedGraphNode * nextNode = edge->getNext()->getDstNode();
            if (q_end == NULL) {
                // candidate for q_end?
                GraphFormulaCNF * preFormula = q_end->getAnnotation()->getDeepCopy();
                GraphFormulaCNF * formula = dnode->getAnnotation()->getDeepCopy();
                formula->size();

                if ( !inQueue[nextNode]) {
                    testNodes.push(nextNode);
                    inQueue[nextNode] = true;
                }
            }
        }
        delete edge;

    }

    return false;
}


/*!
 *  \brief Calculates the predecessor relation of the graph
 *
 *  For the current structure of a Graph the predecessor relation ...
 *
 *  NOTE: If a node B is reachable from a node B via two different edges
 *        then A is two times the predecessor of B
 *
 *  ??? Vorgänger zweimal drin, wenn über zwei Kanten erreichbar
 */
void AnnotatedGraph::getPredecessorRelation(
        AnnotatedGraph::predecessorMap& resultMap) {
    resultMap.clear();

    // iterate over all nodes of the graph
    for (nodes_const_iterator node = setOfNodes.begin(); node
            != setOfNodes.end(); node++) {

        // iterate over all edges in order to get the node's successors
        AnnotatedGraphNode::LeavingEdges::ConstIterator edge = (*node)->getLeavingEdgesConstIterator();
        while (edge->hasNext() ) {
            AnnotatedGraphEdge * ed = edge->getNext();
            // node is the predeccessor 
            resultMap[ed->getDstNode()].add(new AnnotatedGraphEdge(*node, ed->getLabel() ));
        }
    }
}
