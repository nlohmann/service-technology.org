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
 * \file    OGFromFile.cc
 *
 * \brief   Implementation of class OGFromFile. See OGFromFile.h for further
 *          information.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include <cassert>
#include <queue>
#include "OGFromFile.h"
#include "GraphNode.h"
#include "debug.h"
#include "options.h"
#include "main.h"
#include "GraphFormula.h"

// TRUE and FALSE #defined in cudd package may interfere with
// GraphFormulaLiteral::TRUE and ...::FALSE.
#undef TRUE
#undef FALSE

using namespace std;


//OGFromFileNode::OGFromFileNode(const std::string& _name,
//                               GraphFormula* _annotation,
//                               GraphNodeColor _color) :
//
//    // initialize father
//    GraphNodeCommon<OGFromFileNode>(_name, _annotation, _color) {
//
//}
//

OGFromFile::OGFromFile() :
    root(NULL) {
}


OGFromFile::~OGFromFile() {
    for (nodes_iterator node_iter = nodes.begin(); node_iter != nodes.end(); ++node_iter) {

        delete *node_iter;
    }
}


void OGFromFile::addNode(GraphNodeCommon<OGFromFileNode>* node) {
    nodes.insert(node);
}


GraphNodeCommon<OGFromFileNode>* OGFromFile::addNode(const std::string& nodeName,
                                                     GraphFormula* annotation,
                                                     GraphNodeColor color) {

    GraphNodeCommon<OGFromFileNode>* node = new GraphNodeCommon<OGFromFileNode>(nodeName, annotation, color);
    addNode(node);
    return node;
}


void OGFromFile::addTransition(const std::string& srcName,
                               const std::string& dstNodeName,
                               const std::string& label) {

    GraphNodeCommon<OGFromFileNode>* src = getNodeWithName(srcName);
    GraphNodeCommon<OGFromFileNode>* dstNode = getNodeWithName(dstNodeName);
    assert(src != NULL);
    assert(dstNode != NULL);
    GraphEdge<OGFromFileNode>* transition = new GraphEdge<OGFromFileNode>(dstNode,label);
    src->addLeavingEdge(transition);
}


bool OGFromFile::hasNodeWithName(const std::string& nodeName) const {
    return getNodeWithName(nodeName) != NULL;
}


GraphNodeCommon<OGFromFileNode>* OGFromFile::getNodeWithName(const std::string& nodeName) const {

    for (nodes_const_iterator node_iter = nodes.begin(); node_iter
            != nodes.end(); ++node_iter) {

        if ((*node_iter)->getName() == nodeName) {
            return *node_iter;
        }
    }

    return NULL;
}


GraphNodeCommon<OGFromFileNode>* OGFromFile::getRoot() const {
    return root;
}


void OGFromFile::setRoot(GraphNodeCommon<OGFromFileNode>* newRoot) {
    root = newRoot;
}


void OGFromFile::setRootToNodeWithName(const std::string& nodeName) {
    setRoot(getNodeWithName(nodeName));
}


bool OGFromFile::hasNoRoot() const {
    return getRoot() == NULL;
}


void OGFromFile::removeFalseNodes() {

    trace(TRACE_5, "OGFromFile::removeFalseNodes(): start\n");

    bool nodesHaveChanged = true;

    while (nodesHaveChanged) {
        nodesHaveChanged = false;
        nodes_iterator iNode = nodes.begin();

        while (iNode != nodes.end()) {
            GraphFormulaAssignment* iNodeAssignment = (*iNode)->getAssignment();
            if (!(*iNode)->assignmentSatisfiesAnnotation(*iNodeAssignment)) {
                removeTransitionsToNodeFromAllOtherNodes(*iNode);
                if (*iNode == getRoot()) {
                    setRoot(NULL);
                }
                delete *iNode;
                nodes.erase(iNode++);
                nodesHaveChanged = true;
            } else {
                ++iNode;
            }

            delete iNodeAssignment;
        }
    }

    trace(TRACE_5, "OGFromFile::removeFalseNodes(): end\n");
}


void OGFromFile::removeTransitionsToNodeFromAllOtherNodes(const GraphNodeCommon<OGFromFileNode>* nodeToDelete) {

    for (nodes_iterator iNode = nodes.begin(); iNode != nodes.end(); ++iNode) {
        if (*iNode != nodeToDelete) {
            (*iNode)->removeTransitionsToNode(nodeToDelete);
        }
    }
}


//! \brief checks, whether this OGFromFile simulates the given simulant
//! \return true on positive check, otherwise: false
//! \param smallerOG the simulant that should be simulated
bool OGFromFile::simulates(OGFromFile *smallerOG) {
    trace(TRACE_5, "OGFromFile::simulates(OGFromFile *smallerOG): start\n");
    //Simulation is impossible without a simulant.
    if (smallerOG == NULL )
        return false;

    //We need to remember the nodes we already visited.
    set<GraphNodeCommon<OGFromFileNode>*> *myVisitedNodes, *simVisitedNodes;
    myVisitedNodes = new set<GraphNodeCommon<OGFromFileNode>*>;
    simVisitedNodes = new set<GraphNodeCommon<OGFromFileNode>*>;

    //Get things moving...
    bool result = false;
    if (simulatesRecursive(root, myVisitedNodes, smallerOG->getRoot(),
                           simVisitedNodes))
        result = true;

    trace(TRACE_5, "OGFromFile::simulates(OGFromFile *smallerOG): end\n");
    return result;
}


//! \brief checks, whether the part of an OGFromFile below myNode simulates
//         the part of an OGFromFile below simNode
//! \return true on positive check, otherwise: false
//! \param myNode a node in this OGFromFile
//! \param myVisitedNodes a set containing the visited nodes in this OGFromFile
//! \param simNode a node in the simulant
//! \param simVisitedNodes same as myVisitedNodes in the simulant
bool OGFromFile::simulatesRecursive(GraphNodeCommon<OGFromFileNode> *myNode,
                                    set<GraphNodeCommon<OGFromFileNode>*> *myVisitedNodes,
                                    GraphNodeCommon<OGFromFileNode> *simNode,
                                    set<GraphNodeCommon<OGFromFileNode>*> *simVisitedNodes) {
    //If the simulant has no further nodes then myNode simulates simNode.
    if (simNode == NULL) {
        return true;
    }
    //If simNode has a subgraph but myNode does not
    //then myNode cannot simulate simNode.
    if (myNode == NULL) {
        return false;
    }
    //The above two checks shouldn't matter anyway because there should be no
    //edges pointing to NULL, or should they? Let's just keep those checks
    //there for now.

    //If we already visited this node in the simulant, then we're done.
    if (simVisitedNodes->find(simNode) != simVisitedNodes->end()) {
        return true;
    } else {
        simVisitedNodes->insert(simNode);
    }
    //If we have visited this node in the simulator, but not in the simulant,
    //then we screwed up badly (I think). Simulation isn't possible, for sure.
    if (myVisitedNodes->find(myNode) != myVisitedNodes->end()) {
        return false;
    }

    trace(TRACE_5, "OGFromFile::simulateRecursive: checking annotations\n");

    GraphFormulaCNF* simNodeAnnotationInCNF = simNode->getAnnotation()->getCNF();
    GraphFormulaCNF* myNodeAnnotationInCNF = myNode->getAnnotation()->getCNF();
    if (simNodeAnnotationInCNF->implies(myNodeAnnotationInCNF)) {
        trace(TRACE_5, "OGFromFile::simulatesRecursive: annotations ok\n");
    } else {
        trace(TRACE_5, "OGFromFile::simulatesRecursive: annotations incompatible\n");
        delete simNodeAnnotationInCNF;
        delete myNodeAnnotationInCNF;
        return false;
    }
    delete simNodeAnnotationInCNF;
    delete myNodeAnnotationInCNF;

    //Now, we have to check whether the two graphs are compatible.
    trace(TRACE_5, "Iterating over the transitions of the smallerOG's node.\n");
    GraphNodeCommon<OGFromFileNode>::LeavingEdges::ConstIterator
            simEdgeIter =simNode->getLeavingEdgesConstIterator();

    while (simEdgeIter->hasNext()) {
        GraphEdge<OGFromFileNode>* simEdge = simEdgeIter->getNext();
        trace(TRACE_5, "Trying to find the transition in the simulator.\n");

        GraphEdge<OGFromFileNode>
                * myEdge =myNode->getTransitionWithLabel(simEdge->getLabel());

        if (myEdge == NULL) {
            delete simEdgeIter;
            return false;
        } else {
            trace(TRACE_5, "These two nodes seem compatible.\n");
            if (!simulatesRecursive (myEdge->getDstNode(), myVisitedNodes,
                                     simEdge->getDstNode(), simVisitedNodes)) {
                delete simEdgeIter;
                return false;
            }
        }
    }
    delete simEdgeIter;

    //All checks were successful.
    return true;
}


//! \brief checks, whether this OGFromFile is acyclic
//! \return true on positive check, otherwise: false
bool OGFromFile::isAcyclic() {
    trace(TRACE_5, "Test if the given OG is acyclic: start\n");

    // Define a set vor every Node, that will contain all transitive parent nodes
    map<GraphNodeCommon<OGFromFileNode>*, set<GraphNodeCommon<OGFromFileNode>*> > parentNodes;

    // Define a queue for all nodes that still need to be tested and initialize it
    queue<GraphNodeCommon<OGFromFileNode>*> testNodes;
    testNodes.push(root);
    GraphNodeCommon<OGFromFileNode>* testNode;

    // While there are still nodes in the queue
    while (!testNodes.empty()) {

        testNode = testNodes.front();
        testNodes.pop();

        // A node counts as a parent node to it self for the purpose of cycles
        parentNodes[testNode].insert(testNode);

        // Iterate all transitions of that node
        GraphNodeCommon<OGFromFileNode>::LeavingEdges::ConstIterator
                edgeIter =testNode->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            GraphEdge<OGFromFileNode>* edge = edgeIter->getNext();
            // If the Node is the source of that transition and if the Destination is a valid node
            if (edge->getDstNode()->getColor() == BLUE) {

                // Return false if an outgoing transition points at a transitive parent node,
                // else add the destination to the queue and update its transitive parent nodes
                if (parentNodes[testNode].find(edge->getDstNode())
                        != parentNodes[testNode].end()) {
                    delete edgeIter;
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
    return true;
}


//! \brief computes the number of services determined by this OG
//! \return number of Services
unsigned int OGFromFile::numberOfServices() {

    // define variables that will be used in the recursive function
    map<GraphNodeCommon<OGFromFileNode>*, list < set<GraphNodeCommon<OGFromFileNode>*> > >
            validFollowerCombinations;
    set<GraphNodeCommon<OGFromFileNode>*> activeNodes;
    map<GraphNodeCommon<OGFromFileNode>*, unsigned int> followers;
    map<set<GraphNodeCommon<OGFromFileNode>*>, unsigned int> eliminateRedundantCounting;

    // define variables that will be used in the preprocessing before starting the recursion
    set<string> labels;
    list<GraphFormulaAssignment> assignmentList;
    GraphFormulaAssignment possibleAssignment;
    map<string, GraphEdge<OGFromFileNode>*> edges;

    // Preprocess all nodes of the OG in order to fill the variables needed in the recursion
    for (nodes_t::const_iterator iNode = nodes.begin(); iNode != nodes.end(); ++iNode) {

        // reset the temporary variables for every node
        labels.clear();
        assignmentList.clear();
        edges.clear();
        possibleAssignment = GraphFormulaAssignment();

        // get the labels of all outgoing edges, that reach a blue destination
        // save those labels in a set and fill a mapping that allows finding the
        // outgoing edges for a label. (does not work with non-determinism yet)
        GraphNodeCommon<OGFromFileNode>::LeavingEdges::ConstIterator edgeIter =(*iNode)->getLeavingEdgesConstIterator();
        while (edgeIter->hasNext()) {
            GraphEdge<OGFromFileNode>* edge = edgeIter->getNext();
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
        set<GraphNodeCommon<OGFromFileNode>*> followerNodes;

        // for every true assignment in the list a set of nodes will be created. These are the nodes which are
        // reached by outgoing edges of which the labels were true in the assignment. This set is then saved in
        // a map for the currently proceeded node.
        for (list<GraphFormulaAssignment>::iterator
                assignment = assignmentList.begin(); assignment
                != assignmentList.end(); assignment++) {

            followerNodes = set<GraphNodeCommon<OGFromFileNode>*>();
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

    // process Instances recursively
    return numberOfServicesRecursively(activeNodes, followers,
                                       validFollowerCombinations,
                                       eliminateRedundantCounting);
}


//! \brief computes the number of possible services for a finished instance or proceed the active nodes
//! \return number of Services
unsigned int OGFromFile::numberOfServicesRecursively(set<GraphNodeCommon<OGFromFileNode>*> activeNodes,
                                                     map<GraphNodeCommon<OGFromFileNode>*, unsigned int>& followers,
                                                     map<GraphNodeCommon<OGFromFileNode>*,list<set<GraphNodeCommon<OGFromFileNode>*> > >& validFollowerCombinations,
                                                     map<set<GraphNodeCommon<OGFromFileNode>*>, unsigned int>& eliminateRedundantCounting) {

    // if an Instance with the same active Nodes has already been computed, use the saved value
    if (eliminateRedundantCounting[activeNodes] != 0) {
        return eliminateRedundantCounting[activeNodes];
    }

    // define needed variables
    unsigned int number = 0;
    list< set<GraphNodeCommon<OGFromFileNode>*> > oldList;
    list< set<GraphNodeCommon<OGFromFileNode>*> > newList;
    set<GraphNodeCommon<OGFromFileNode>*> tempSet;
    bool first = true;
    bool usingNew = true;
    bool finalInstance = true;

    // process all active nodes of this instance
    for (set<GraphNodeCommon<OGFromFileNode>*>::iterator activeNode = activeNodes.begin(); activeNode
            != activeNodes.end(); activeNode++) {

        // if the active node has no valid outgoing edges, do nothing. If that happens
        // with all active nodes, the finalInstance variable will stay true
        if (followers[(*activeNode)] != 0) {

            finalInstance = false;

            // if this is the first iteration for this node, fill the newList with all combinations
            // of followers of the currently proceeded active Node and continues the loop
            if (first) {

                first = false;
                for (list<set<GraphNodeCommon<OGFromFileNode>*> >::iterator
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
                for (list<set<GraphNodeCommon<OGFromFileNode>*> >::iterator
                        oldListSet = oldList.begin(); oldListSet
                        != oldList.end(); oldListSet++) {

                    for (list<set<GraphNodeCommon<OGFromFileNode>*> >::iterator
                            combination = validFollowerCombinations[(*activeNode)].begin(); combination
                            != validFollowerCombinations[(*activeNode)].end(); combination++) {

                        tempSet = set<GraphNodeCommon<OGFromFileNode>*>();

                        for (set<GraphNodeCommon<OGFromFileNode>*>::iterator insertionNode = (*combination).begin(); insertionNode != (*combination).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        for (set<GraphNodeCommon<OGFromFileNode>*>::iterator insertionNode = (*oldListSet).begin(); insertionNode != (*oldListSet).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        newList.push_back(tempSet);
                    }
                }
                usingNew = false;
            } else {

                oldList.clear();
                for (list<set<GraphNodeCommon<OGFromFileNode>*> >::iterator
                        newListSet = newList.begin(); newListSet
                        != newList.end(); newListSet++) {

                    for (list<set<GraphNodeCommon<OGFromFileNode>*> >::iterator
                            combination = validFollowerCombinations[(*activeNode)].begin(); combination
                            != validFollowerCombinations[(*activeNode)].end(); combination++) {

                        tempSet = set<GraphNodeCommon<OGFromFileNode>*>();

                        for (set<GraphNodeCommon<OGFromFileNode>*>::iterator insertionNode = (*combination).begin(); insertionNode != (*combination).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }

                        for (set<GraphNodeCommon<OGFromFileNode>*>::iterator insertionNode = (*newListSet).begin(); insertionNode != (*newListSet).end(); insertionNode++) {

                            tempSet.insert((*insertionNode));
                        }
                        oldList.push_back(tempSet);
                    }
                }
                usingNew = true;
            }
        }
    }

    // if none of the active nodes had followers this is a finished service of the OG and thus returns 1
    if (finalInstance) {
        eliminateRedundantCounting[activeNodes] = 1;
        return 1;
    }

    // if there were sets of following nodes, create a new instance of active nodes for every tuple of
    // following sets and add the results to this instance's number of services
    if (usingNew) {
        for (list<set<GraphNodeCommon<OGFromFileNode>*> >::iterator oldListSet = oldList.begin(); oldListSet
                != oldList.end(); oldListSet++) {
            number += numberOfServicesRecursively((*oldListSet), followers,
                                                  validFollowerCombinations,
                                                  eliminateRedundantCounting);
        }
    } else {
        for (list<set<GraphNodeCommon<OGFromFileNode>*> >::iterator newListSet = newList.begin(); newListSet
                != newList.end(); newListSet++) {
            number += numberOfServicesRecursively((*newListSet), followers,
                                                  validFollowerCombinations,
                                                  eliminateRedundantCounting);
        }
    }

    // as soon as the counting for this set of active nodes is finished, save the number to prevent redundancy
    eliminateRedundantCounting[activeNodes] = number;

    // return the number of services for this instance
    return number;
}


//! \brief computes the number of true assignments for the given formula of an OG node and additionally
//!        saves them in an assignmentList for every node. The function works by recursively
//!        computing and checking the powerset of all labels of the node
//! \return number of true Assignments
unsigned int OGFromFile::processAssignmentsRecursively(set<string> labels,
                                                       GraphFormulaAssignment possibleAssignment,
                                                       GraphNodeCommon<OGFromFileNode>* testNode,
                                                       list<GraphFormulaAssignment>& assignmentList) {
    // If there is no outgoing transition, return immediatly
    if (labels.empty()) {
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


OGFromFile* OGFromFile::product(const OGFromFile* rhs) {
    trace(TRACE_5, "OGFromFile::product(const OGFromFile* rhs): start\n");

    // this will be the product OG
    OGFromFile* productOG = new OGFromFile;

    // If one of both OGs is empty, their product is empty, too.
    if (hasNoRoot() || rhs->hasNoRoot()) {
        return productOG;
    }

    // first we build a new root node that has name and annotation constructed
    // from the root nodes of OG and the rhs OG.
    GraphNodeCommon<OGFromFileNode>* currentOGNode = this->getRoot();
    GraphNodeCommon<OGFromFileNode>* currentRhsNode = rhs->getRoot();

    std::string currentName;
    currentName = currentOGNode->getName() + "x"+ currentRhsNode->getName();

    GraphFormulaCNF* currentFormula = createProductAnnotation(currentOGNode,
                                                              currentRhsNode);

    // building the new root node of the product OG
    GraphNodeCommon<OGFromFileNode>* productNode = new GraphNodeCommon<OGFromFileNode>(currentName,
            currentFormula);
    productOG->addNode(productNode);
    productOG->setRoot(productNode);

    // builds the successor nodes of the root nodes of OG and rhs OG
    // therefore, we perform a coordinated dfs through OG and the rhs OG
    buildProductOG(currentOGNode, currentRhsNode, productOG);

    productOG->removeFalseNodes();
    trace(TRACE_5, "OGFromFile::product(const OGFromFile* rhs): end\n");

    return productOG;
}


OGFromFile* OGFromFile::product(const ogs_t& ogs) {
    assert(ogs.size() > 1);

    ogs_t::const_iterator iOG = ogs.begin();
    OGFromFile* firstOG = *iOG++;
    OGFromFile* productOG = firstOG->product(*iOG);

    for (++iOG; iOG != ogs.end(); ++iOG) {
        OGFromFile* oldProductOG = productOG;
        productOG = productOG->product(*iOG);
        delete oldProductOG;
    }

    return productOG;
}


void OGFromFile::buildProductOG(GraphNodeCommon<OGFromFileNode>* currentOGNode,
                                GraphNodeCommon<OGFromFileNode>* currentRhsNode,
                                OGFromFile* productOG) {

    trace(TRACE_5, "OGFromFile::buildProductOG(GraphNodeCommon<OGFromFileNode>* currentOGNode, GraphNodeCommon<OGFromFileNode>* currentRhsNode, OGFromFile* productOG): start\n");

    // at this time, the node constructed from currentOGNode and
    // currentRhsNode is already inserted

    assert(productOG->getRoot() != NULL);

    // iterate over all outgoing edges from current node of OG
    std::string currentLabel;
    GraphNodeCommon<OGFromFileNode>::LeavingEdges::ConstIterator
            edgeIter =currentOGNode->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        GraphEdge<OGFromFileNode>* edge = edgeIter->getNext();

        // remember the label of the egde
        currentLabel = edge->getLabel();

        // if the rhs automaton allows this edge
        if (currentRhsNode->hasTransitionWithLabel(currentLabel)) {

            // remember the name of the old node of the product OG
            std::string currentName;
            currentName = currentOGNode->getName() + "x" + currentRhsNode->getName();
            assert(productOG->hasNodeWithName(currentName));

            // compute both successors and recursively call buildProductOG again
            GraphNodeCommon<OGFromFileNode>* newOGNode;
            newOGNode = currentOGNode->fireTransitionWithLabel(currentLabel);

            GraphNodeCommon<OGFromFileNode>* newRhsNode;
            newRhsNode = currentRhsNode->fireTransitionWithLabel(currentLabel);

            // build the new node of the product OG
            // that has name and annotation constructed from current nodes of OG and rhs OG
            std::string newProductName;
            newProductName = newOGNode->getName() + "x" + newRhsNode->getName();
            // if the node is new, add that node to the OG
            GraphNodeCommon<OGFromFileNode>* found = productOG->getNodeWithName(newProductName);

            if (found != NULL) {
                // the node was known before, so we just have to add a new edge
                productOG->addTransition(currentName, newProductName,
                                         currentLabel);

                trace(TRACE_5, "OGFromFile::buildProductOG(GraphNodeCommon<OGFromFileNode>* currentOGNode, GraphNodeCommon<OGFromFileNode>* currentRhsNode, OGFromFile* productOG): end\n");
            } else {
                // we computed a new node, so we add a node and an edge
                // trace(TRACE_0, "adding node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + "\n");

                GraphFormulaCNF* newProductFormula = createProductAnnotation(newOGNode, newRhsNode);

                GraphNodeCommon<OGFromFileNode>* newProductNode = new GraphNodeCommon<OGFromFileNode>(newProductName, newProductFormula);

                productOG->addNode(newProductNode);

                // going down recursively
                productOG->addTransition(currentName, newProductName, currentLabel);

                buildProductOG(newOGNode, newRhsNode, productOG);
            }
        }
    }
    delete edgeIter;
    trace(TRACE_5, "OGFromFile::buildProductOG(GraphNodeCommon<OGFromFileNode>* currentOGNode, GraphNodeCommon<OGFromFileNode>* currentRhsNode, OGFromFile* productOG): end\n");
}


GraphFormulaCNF* OGFromFile::createProductAnnotation(const GraphNodeCommon<OGFromFileNode>* lhs,
                                                     const GraphNodeCommon<OGFromFileNode>* rhs) const {

    GraphFormulaMultiaryAnd* conjunction = new GraphFormulaMultiaryAnd(
            lhs->getAnnotation()->getDeepCopy(),
            rhs->getAnnotation()->getDeepCopy());

    GraphFormulaCNF* cnf = conjunction->getCNF();
    delete conjunction;
    cnf->simplify();
    return cnf;
}


std::string OGFromFile::getProductOGFilePrefix(const ogfiles_t& ogfiles) {
    assert(ogfiles.size() != 0);

    ogfiles_t::const_iterator iOgFile = ogfiles.begin();
    string productFilePrefix = stripOGFileSuffix(*iOgFile);

    for (++iOgFile; iOgFile != ogfiles.end(); ++iOgFile) {
        productFilePrefix += "_X_" + stripOGFileSuffix(platform_basename(*iOgFile));
    }

    return productFilePrefix;
}


std::string OGFromFile::stripOGFileSuffix(const std::string& filename) {

    static const string ogFileSuffix = ".og";
    if (filename.substr(filename.size() - ogFileSuffix.size()) == ogFileSuffix) {
        return filename.substr(0, filename.size() - ogFileSuffix.size());
    }

    return filename;
}


void OGFromFile::printDotFile(const std::string& filenamePrefix,
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

    std::map<GraphNodeCommon<OGFromFileNode>*, bool> visitedNodes;
    printGraphToDot(getRoot(), dotFileHandle, visitedNodes);

    dotFileHandle << "}";
    dotFileHandle.close();

    // prepare dot command line for printing
    string cmd = "dot -Tpng \"" + dotFile + "\" -o \""+ pngFile + "\"";

    // print commandline and execute system command
    trace(TRACE_0, cmd + "\n\n");
    system(cmd.c_str());
}


void OGFromFile::printDotFile(const std::string& filenamePrefix) const {
    printDotFile(filenamePrefix, filenamePrefix);
}


//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes maps nodes to Bools remembering already visited nodes
//! \brief dfs through the graph printing each node and edge to the output stream
void OGFromFile::printGraphToDot(GraphNodeCommon<OGFromFileNode>* v,
                                 fstream& os,
                                 std::map<GraphNodeCommon<OGFromFileNode>*, bool>& visitedNodes) const {

    if (v == NULL) {
        // print the empty OG...
        os << "p0" << " [label=\"#0\", fontcolor=black, color=red, style=dashed];\n";
        return;
    }

    if (visitedNodes[v] != true) {
        os << "p"<< v->getName() << " [label=\"# "<< v->getName() << "\\n";
        os << v->getAnnotation()->asString();
        os << "\", fontcolor=black, color=blue];\n";
        visitedNodes[v] = true;

        std::string currentLabel;

        GraphNodeCommon<OGFromFileNode>::LeavingEdges::ConstIterator
                edgeIter =v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            GraphEdge<OGFromFileNode>* edge = edgeIter->getNext();

            // remember the label of the egde
            currentLabel = edge->getLabel();
            GraphNodeCommon<OGFromFileNode>
                    * successor = v->fireTransitionWithLabel(currentLabel);
            assert(successor != NULL);

            os << "p" << v->getName() << "->" << "p" << successor->getName()
               << " [label=\"" << currentLabel
               << "\", fontcolor=black, color= blue];\n";
            printGraphToDot(successor, os, visitedNodes);
        }
        delete edgeIter;
    }
}


//! \brief prints all nodes and transitions of an OG to file .og
void OGFromFile::printOGFile(const std::string& filenamePrefix) const {
    fstream ogFile(addOGFileSuffix(filenamePrefix).c_str(), ios_base::out | ios_base::trunc);

    if (hasNoRoot()) {
        // print file for empty OG
        ogFile << "NODES" << endl
               << "  0 : " << GraphFormulaLiteral::FALSE << " : "<< GraphNodeColor(RED).toString() << ';' << endl << endl
               << "INITIALNODE" << endl
               << "  0;" << endl << endl
               << "TRANSITIONS" << endl
               << "  ;" << endl;

        ogFile.close();
        return;
    }

    ogFile << "NODES"<< endl;
    bool printedFirstNode = false;
    for (nodes_t::const_iterator iNode = nodes.begin(); iNode != nodes.end(); ++iNode) {

        if (printedFirstNode) {
            ogFile << ','<< endl;
        }

        GraphNodeCommon<OGFromFileNode>* node = *iNode;
        ogFile << "  " << node->getName() << " : "
               << node->getAnnotationAsString() << " : " << node->getColor().toString();

        printedFirstNode = true;
    }
    ogFile << ';'<< endl << endl;

    ogFile << "INITIALNODE" << endl;
    assert(getRoot() != NULL);
    ogFile << "  " << getRoot()->getName() << ';' << endl << endl;

    ogFile << "TRANSITIONS" << endl;
    bool printedFirstTransition = false;
    for (nodes_t::const_iterator iNode = nodes.begin(); iNode != nodes.end(); ++iNode) {

        GraphNodeCommon<OGFromFileNode>* node = *iNode;
        GraphNodeCommon<OGFromFileNode>::LeavingEdges::ConstIterator
                iEdge = node->getLeavingEdgesConstIterator();

        while (iEdge->hasNext()) {
            if (printedFirstTransition) {
                ogFile << ',' << endl;
            }

            GraphEdge<OGFromFileNode>* edge = iEdge->getNext();
            ogFile << "  " << node->getName() << " -> " << edge->getDstNode()->getName() << " : " << edge->getLabel();

            printedFirstTransition = true;
        }
        delete iEdge;
    }
    ogFile << ';'<< endl;

    ogFile.close();
}


std::string OGFromFile::addOGFileSuffix(const std::string& filePrefix) {
    return filePrefix + ".og";
}
