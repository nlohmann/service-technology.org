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
 * \file    AnnotatedGraph.cc
 *
 * \brief   Implementation of class Graph. See AnnotatedGraph.h for further
 *          information.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 */

#include <cassert>
#include <queue>
#include <utility>
#include "AnnotatedGraph.h"

#include "options.h"
#include <sstream>
#include <vector>

// TRUE and FALSE #defined in cudd package may interfere with
// GraphFormulaLiteral::TRUE and ...::FALSE.
#undef TRUE
#undef FALSE

using namespace std;

//extern std::list<string> netfiles;
//extern std::list<string> ogfiles;


//! \brief a basic constructor of AnnotatedGraph
AnnotatedGraph::AnnotatedGraph() :
    root(NULL) {
}


//! \brief a basic destructor of AnnotatedGraph
AnnotatedGraph::~AnnotatedGraph() {
    TRACE(TRACE_5, "AnnotatedGraph::~AnnotatedGraph() : start\n");
    for (unsigned int i = 0; i < setOfNodes.size(); i++) {
        delete setOfNodes[i];
    }
    setOfNodes.clear();
    inputPlacenames.clear();
    outputPlacenames.clear();
    TRACE(TRACE_5, "AnnotatedGraph::~AnnotatedGraph() : end\n");
}


//! \brief returns the name of the graph's source file
string AnnotatedGraph::getFilename() {
    return filename;
}


//! \brief returns the name of the graph's source file
void AnnotatedGraph::setFilename(string filename) {
    this->filename = filename;
}


//! \brief checks wether a root node is set
//! \return returns true if the rootnode is NULL, else false
bool AnnotatedGraph::hasNoRoot() const {
    return getRoot() == NULL;
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
void AnnotatedGraph::setRootToNodeWithName(const string& nodeName) {
    setRoot(getNodeWithName(nodeName));
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
AnnotatedGraphNode* AnnotatedGraph::addNode(const string& nodeName,
                                            GraphFormula* annotation,
                                            GraphNodeColor color) {

    AnnotatedGraphNode* node = new AnnotatedGraphNode(nodeName,
                                                      annotation,
                                                      color,
                                                      setOfNodes.size());
    addNode(node);
    return node;
}


//! \brief A function needed for successful deletion of the graph
void AnnotatedGraph::clearNodeSet() {
    setOfNodes.clear();
}


//! \brief checks if the graph has a node with the given name
//! \param nodeName the name to be matched
//! \return returns true if a node with the given name exists, else false
bool AnnotatedGraph::hasNodeWithName(const string& nodeName) const {
    return getNodeWithName(nodeName) != NULL;
}


//! \brief returns a pointer to the node that matches a given name, or NULL else
//! \param nodeName the name to be matched
//! \return returns a pointer to the found node or NULL
AnnotatedGraphNode* AnnotatedGraph::getNodeWithName(const string& nodeName) const {

    for (nodes_const_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end(); ++node_iter) {

        if ((*node_iter)->getName() == nodeName) {
            return *node_iter;
        }
    }

    return NULL;
}


//! \brief create a new edge in the graph
//! \param srcName a string containing the name of the source node
//! \param dstNodeName a string containing the name of the destination node
//! \param label a string containing the label of the edge
void AnnotatedGraph::addEdge(const string& srcName,
                             const string& dstNodeName,
                             const string& label) {

    AnnotatedGraphNode* src = getNodeWithName(srcName);
    AnnotatedGraphNode* dstNode = getNodeWithName(dstNodeName);

    AnnotatedGraphEdge* transition = new AnnotatedGraphEdge(dstNode, label);
    src->addLeavingEdge(transition);
}


//! \brief create a new edge in the graph
//! \param srcName a string containing the name of the source node
//! \param dstNodeName a string containing the name of the destination node
//! \param label a string containing the label of the edge
void AnnotatedGraph::addEdge(AnnotatedGraphNode * src,
                             AnnotatedGraphNode * dst,
                             const string& label) {

    AnnotatedGraphEdge* transition = new AnnotatedGraphEdge(dst, label);
    src->addLeavingEdge(transition);
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


//! \brief checks, whether this AnnotatedGraph is acyclic
//! \return true on positive check, otherwise: false
bool AnnotatedGraph::isAcyclic() {
    TRACE(TRACE_5, "AnnotatedGraph::isAcyclic(...): start\n");

    // map the number of every nodes predecessors to the node
    map<AnnotatedGraphNode*, unsigned int> predecessors;

    // if the graph has no root it is empty and thus acyclic
    if (hasNoRoot()) {
        trace( "The graph is empty and thus acyclic!\n");
        return true;
    }

    // A set that contains all nodes of the graph in the beginning
    set<AnnotatedGraphNode*> allNodes;

    // First iteration:
    // - determine the number of predecessors for every node
    // - copy every node into the set of allNodes
    for (vector<AnnotatedGraphNode*>::iterator currentNode = setOfNodes.begin(); currentNode != setOfNodes.end(); currentNode++) {

        // Iterate all transitions of that node
        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =
            (*currentNode)->getLeavingEdgesConstIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            // If the target node of this edge is a blue node
            if (edge->getDstNode()->getColor() == BLUE) {
                // increase the number of predecessors of the target node
                predecessors[edge->getDstNode()] += 1;
            }
        }
        delete edgeIter;

        // copy this node into the set of all nodes
        allNodes.insert(*currentNode);
    }

    // now we define a queue that will hold all nodes that have no predecessor (left)
    queue<AnnotatedGraphNode*> zeroNodes;

    // Initialize the queue
    for (vector<AnnotatedGraphNode*>::iterator currentNode = setOfNodes.begin(); currentNode != setOfNodes.end(); currentNode++) {
        if(predecessors[(*currentNode)] == 0) {
            zeroNodes.push(*currentNode);
        }
    }

    // while there are still nodes that have no predecessor
    while (zeroNodes.size() != 0) {
        // erase this node from zeroNodes as well as from the set allNodes
        AnnotatedGraphNode* currentNode = zeroNodes.front();
        zeroNodes.pop();
        allNodes.erase(currentNode);

        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =
            (currentNode)->getLeavingEdgesConstIterator();

        // Iterate over all edges of this node
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            // If the target node of this edge is a blue node
            if (edge->getDstNode()->getColor() == BLUE) {
                // Since this node was erased it decreases the number of predecessors
                // for its follower nodes
                predecessors[edge->getDstNode()] -= 1;

                // If a follower node has no predecessors left it is taken into the queue
                if (predecessors[edge->getDstNode()] == 0) {
                    zeroNodes.push(edge->getDstNode());
                }
            }
        }
    }

    TRACE(TRACE_5, "AnnotatedGraph::isAcyclic(...): end\n");

    // If all nodes could have been erased this way, this graph is
    // acyclic. If there are still nodes left, there must be a cycle.
    if (allNodes.size() == 0) {
        return true;
    } else {
        return false;
    }
}


/*  Old algorithm that counts all services that match the OG
 *  and are a subgraph of the dual OG at the same time

//! \brief computes the number of services determined by this OG
//! \return number of Services
unsigned int AnnotatedGraph::numberOfServices() {

    TRACE(TRACE_5, "AnnotatedGraph::numberOfServices(...): start\n");

    TRACE(TRACE_1, "Removing false nodes...\n");
    removeReachableFalseNodes();

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

    TRACE(TRACE_2, "Computing true assignments for all nodes\n");
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
        TRACE(TRACE_5, "Collecting labels of outgoing edges for current node\n");
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

    TRACE(TRACE_1, "Starting recursive computation of number of Services\n");
    // process Instances recursively
    number = numberOfServicesRecursively(activeNodes, followers,
            validFollowerCombinations, eliminateRedundantCounting, instances);

    if (instances > 100000) {
        TRACE(TRACE_2, "Valid Number of instances exceeded.\n");
        trace(
                TRACE_0,
                "The number of strategies is approx INFINITY ;), aborting further calculation.\n");
        TRACE(TRACE_5, "AnnotatedGraph::numberOfServices(...): end\n");
        return number;
    } else {
        TRACE(TRACE_5, "AnnotatedGraph::numberOfServices(...): end\n");
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

    if (instances % 10000 == 0 && instances != 0) {
        if (instances > 100000) {
            return 0;
        }
        TRACE(TRACE_2, "Processed number of instances: "
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
    //TRACE(TRACE_5, "Number returned was: " + intToString(number) + "\n");
    //TRACE(TRACE_5, "Current Instances are: " + intToString(instances) + "\n");

    return number;
}

    End of the old algorithm */


//! \brief computes the number of tree-shaped service automata matching with this OG
//! \return number of Strategies
long double AnnotatedGraph::numberOfStrategies() {

    TRACE(TRACE_5, "AnnotatedGraph::numberOfStrategies(...): start\n");

    TRACE(TRACE_1, "Removing false nodes...\n");
    removeReachableFalseNodes();

    // if this og is empty there are no strategies for it
    if (root == NULL) {
        return 0;
    }

    // define variables that will be used in the recursive function
    map<AnnotatedGraphNode*, list <list<AnnotatedGraphNode*> > > validFollowerCombinations;
    map<AnnotatedGraphNode*, long double> eliminateRedundantCounting;

    // define variables that will be used in the preprocessing before starting the recursion
    set<string> labels;
    list<GraphFormulaAssignment> assignmentList;
    GraphFormulaAssignment possibleAssignment;
    map<string, AnnotatedGraphEdge*> edges;

    TRACE(TRACE_2, "Computing true assignments for all nodes\n");
    // Preprocess all nodes of the OG in order to fill the variables needed in the recursion
    for (nodes_t::const_iterator iNode = setOfNodes.begin();
         iNode != setOfNodes.end(); ++iNode) {

        // reset the temporary variables for every node
        labels.clear();
        assignmentList.clear();
        edges.clear();
        possibleAssignment = GraphFormulaAssignment();

        // the literals "true" and "final" are always set to true in
        // every assignment
        possibleAssignment.setToTrue("true");
        possibleAssignment.setToTrue("final");

        // get the labels of all outgoing edges,
        // save those labels in a set and fill a mapping that allows finding the
        // outgoing edges for a label
        TRACE(TRACE_5, "Collecting labels of outgoing edges for current node\n");
        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =(*iNode)->getLeavingEdgesConstIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            labels.insert(edge->getLabel());
            edges[edge->getLabel()] = edge;
        }

        // get rid of the iterator
        delete edgeIter;

        // return the number of true assignments for this node's formula
        processAssignmentsRecursively(labels,
                                      possibleAssignment,
                                      (*iNode),
                                      assignmentList);

        // create a temporary variable for a list of nodes
        list<AnnotatedGraphNode*> followerNodes;

        // for every true assignment a list of nodes will be created. These are the nodes which are
        // reached by outgoing edges of which the labels were true in the assignment. This set is then saved in
        // a map for the currently proceeded node.
        for (list<GraphFormulaAssignment>::iterator assignment = assignmentList.begin();
             assignment != assignmentList.end(); assignment++) {

            followerNodes = list<AnnotatedGraphNode*>();
            for (set<string>::iterator label = labels.begin(); label
                    != labels.end(); label++) {

                if (assignment->get((*label))) {
                    followerNodes.push_back(edges[(*label)]->getDstNode());
                }
            }
            validFollowerCombinations[(*iNode)].push_back(followerNodes);
        }
    }

    long double number = 0;

    TRACE(TRACE_1, "Starting recursive computation of number of Strategies\n");

    // process the nodes recursively
    number = numberOfStrategiesRecursively(root,
                                           validFollowerCombinations,
                                           eliminateRedundantCounting);

    TRACE(TRACE_5, "AnnotatedGraph::numberOfStrategies(...): end\n");
    return number;
}


//! \brief compute the number of tree-shaped service automata matching from this node
//! \param activeNode a node pointing to the current node
//! \param validFollowerCombinations a map that contains all sets of follower nodes that succeed to
//!        fullfill the annotation of the current node with the edges leading to them
//! \param eliminateRedundantCounting map containing all already computed number of tree service
//!           automata for every node. This map is filled meanwhile the recursion
//! \return number of Strategies for the current node
long double AnnotatedGraph::numberOfStrategiesRecursively(
        AnnotatedGraphNode* activeNode,
        map<AnnotatedGraphNode*, list<list<AnnotatedGraphNode*> > >& validFollowerCombinations,
        map<AnnotatedGraphNode*, long double>& eliminateRedundantCounting) {

    // if the number for the active Node has already been computed, use the saved value
    if (eliminateRedundantCounting[activeNode] != 0) {
        TRACE(TRACE_3 ,"    value for node " + activeNode->getName() + " was already calculated\n");
        return eliminateRedundantCounting[activeNode];
    }

    TRACE(TRACE_2 ,"    first time entering node: " + activeNode->getName() + "\n");

    // define the counting variable
    long double number = 0;

    TRACE(TRACE_3 ,"    this node has " +  intToString(validFollowerCombinations[activeNode].size()) + " follower Combinations\n");

    // iterate over the list of valid follower combinations
    for (list<list<AnnotatedGraphNode*> >::iterator followerList =
         validFollowerCombinations[activeNode].begin();
         followerList != validFollowerCombinations[activeNode].end();
         followerList++) {

        TRACE(TRACE_4, "        current followerCombination: ");
        // if there is no need for followers in order to fulfill
        // the annotation, then count an additional automaton for
        // this node
        if ((*followerList).size() == 0) {
            //if (validFollowerCombinations[activeNode].size() == 1) {
                number += 1;
            //}
            TRACE(TRACE_4, "empty\n");
            continue;
        }

        // the value of the current combination is the product of the values of
        // all the nodes the edges of the current combination point to
        long double currentCombinationValue = 1;

        // detailed information for high debug levels
        if (debug_level >= TRACE_4) {
            bool firstIteration = true;
            for (list<AnnotatedGraphNode*>::iterator follower =
                   followerList->begin(); follower != followerList->end(); follower++) {
                if (!firstIteration) {
                    TRACE(TRACE_4, ", ");
                } else {
                    firstIteration = false;
                }
                TRACE(TRACE_4, (*follower)->getName());
            }
        TRACE(TRACE_4, "\n");
        }

        // DEBUG
        int i = 0;

        // Iterate over all follower nodes from this valid follower combination
        // and after having done the recursion for them, multiply
        for (list<AnnotatedGraphNode*>::iterator follower =
            followerList->begin(); follower != followerList->end(); follower++) {
            currentCombinationValue *= numberOfStrategiesRecursively( (*follower),
                    validFollowerCombinations, eliminateRedundantCounting);
            TRACE(TRACE_3, "    backtracking to node: " + activeNode->getName() + "\n");
            i++;
        }

        // add the value of the current combination to the active node's value
        number += currentCombinationValue;
    }

    // use a stringstream to turn a long double into a string for debug output
    stringstream longDoubleZahl;
    longDoubleZahl << number;
    TRACE(TRACE_2 ,"    node " + activeNode->getName() + " returned a value of " + longDoubleZahl.str() + "\n");

    // eliminitae redundant counting by saving the value for this node
    eliminateRedundantCounting[activeNode] = number;

    // return the value of this active node to the caller
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
        if (testNode->assignmentSatisfiesAnnotation(possibleAssignment)) {
            assignmentList.push_back(possibleAssignment);
            return 1;
        }
        return 0;
    }

    // define variables
    unsigned int returnValue = 0;
    unsigned int tempValue = 0;
    string label;
    label = (*labels.begin());
    labels.erase(labels.begin());

    // if this was the last label ...
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
        // count the number of all assignments which are true following this label being set to false
        tempValue = processAssignmentsRecursively(labels, possibleAssignment,
                testNode, assignmentList);
        // increase the number of true assignments accordingly
        returnValue += tempValue;

        // set it to True
        possibleAssignment.setToTrue(label);
        // count the number of all assignments which are true following this label being set to true
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


//! \brief finds all nodes that have annotations that cannot become
//!        true. The function continues removing until no node fullfils
//!        the mentioned criterion
void AnnotatedGraph::findFalseNodes(std::vector<AnnotatedGraphNode*>* falseNodes) {

    TRACE(TRACE_5, "AnnotatedGraph::findFalseNodes(): start\n");

    // if the og is empty, dont search for false nodes
    if (getRoot()->getColor() == RED) {
        return;
    }

    nodes_iterator iNode = setOfNodes.begin();

    while (iNode != setOfNodes.end()) {

        GraphFormulaAssignment* iNodeAssignment = (*iNode)->getAssignment();
        if (!(*iNode)->assignmentSatisfiesAnnotation(*iNodeAssignment)) {
            falseNodes->push_back(*iNode);
        }
        ++iNode;
    }
    TRACE(TRACE_5, "AnnotatedGraph::findFalseNodes(): end\n");
}


//! \brief Removes all unneeded literals from all reachable nodes
//!        and removes reachable nodes which do not satisfy their own annotation.
void AnnotatedGraph::removeReachableFalseNodes() {

    TRACE(TRACE_5, "AnnotatedGraph::removeReachableFalseNodes(): start\n");
    if (getRoot() == NULL) return;

    set<AnnotatedGraphNode*> visitedNodes, redNodes;
    map<AnnotatedGraphNode*, set<AnnotatedGraphNode*> > parentNodes;

    removeReachableFalseNodesInit(getRoot(), redNodes, parentNodes, visitedNodes);
    removeReachableFalseNodesRecursively(redNodes, parentNodes);

    TRACE(TRACE_5, "AnnotatedGraph::removeReachableFalseNodes(): end\n");
}


//! \brief Helps AnnotatedGraph::removeReachableFalseNodes by recursively:
//!        1. Constructing a parent function for all reachable nodes.
//!        2. Shorten the annotation of every reachable node by removing event-literals which do not appear as the label of an outgoing edge.
//!        3. Analyzes all blue nodes.
//!        4. Gathers a first set of candidates for deletion: All red, reachable nodes.
//! \param currentNode The node to be processed
//! \param redNodes Set for storing first candidates (see 4.)
//! \param parentNodes Map for storing the parent function (see 1.)
//! \param visitedNodes Contains a pointer to each processed node
void AnnotatedGraph::removeReachableFalseNodesInit(AnnotatedGraphNode* currentNode,
                                                   set<AnnotatedGraphNode*>& redNodes,
                                                   map<AnnotatedGraphNode*, set<AnnotatedGraphNode*> >& parentNodes,
                                                   set<AnnotatedGraphNode*>& visitedNodes) {

    TRACE(TRACE_5, "AnnotatedGraph::removeReachableFalseNodesInit(): start\n");

    // This node should not be processed again.
    visitedNodes.insert(currentNode);

    // Collect all event-related literals from the annotation of the current node.
    set<string> eventsInNode;
    currentNode->getAnnotation()->getEventLiterals(eventsInNode);

    // If the current node is blue, it is analysed
    if (currentNode->getColor() == BLUE) {
        currentNode->analyseNode(true);
    }

    // If the node is already red or became red during analysis, insert it into the set of candidates
    if (currentNode->getColor() == RED) {
        redNodes.insert(currentNode);
    }

    // Iterate over all leaving edges of the current node
    AnnotatedGraphNode::LeavingEdges::Iterator edgeIter = currentNode->getLeavingEdgesIterator();
    while (edgeIter->hasNext()) {

        AnnotatedGraphEdge* currentEdge = edgeIter->getNext();
        AnnotatedGraphNode* destNode = currentEdge->getDstNode();

        // Remove the current label from the set of event-related literals
        eventsInNode.erase(currentEdge->getLabel());

        // Insert this node as a parent of his child
        parentNodes[destNode].insert(currentNode);

        // If this child has not yet been processed, descend recursively
        if (visitedNodes.find(destNode) == visitedNodes.end()) {
            removeReachableFalseNodesInit(destNode, redNodes, parentNodes, visitedNodes);
        }

    }
    // Delete the LeavingEdgesIterator from the heap
    delete edgeIter;

    // Iterate over all event-related literals that were not removed during the iteration over the edges
    for (set<string>::iterator unneededEvent = eventsInNode.begin(); unneededEvent != eventsInNode.end(); ++unneededEvent) {
        // Remove the literal from the formula, since it is not needed.
        currentNode->getAnnotation()->removeLiteral(*unneededEvent);
    }

    TRACE(TRACE_5, "AnnotatedGraph::removeReachableFalseNodesInit(): end\n");
}


//! \brief Proceeds a set of candidates for false nodes, removes proven false nodes
//!        and designates their parents as candidates for the next recursive step.
//! \param candidates Current candidates, in the first step filled with red nodes only.
//! \param parentNodes The parent node function as created by AnnotatedGraph::removeReachableFalseNodesInit.
void AnnotatedGraph::removeReachableFalseNodesRecursively(set<AnnotatedGraphNode*>& candidates, map<AnnotatedGraphNode*, set<AnnotatedGraphNode*> >& parentNodes) {

    TRACE(TRACE_5, "AnnotatedGraph::removeReachableFalseNodesRecursively(): s\n");

    // The set to be filled with the new candidates
    set<AnnotatedGraphNode*> newCandidates;

    // Iterate over all candidates of this recursive step
    for (set<AnnotatedGraphNode*>::iterator candidate = candidates.begin(); candidate != candidates.end(); ++candidate) {
        // If the candidate is a blue node, re-analyse it
        if ((*candidate)->getColor() == BLUE) {
            (*candidate)->analyseNode(true);
        }

        // If the candidate was a red node or became red as result of the analysis
        if ((*candidate)->getColor() == RED) {

            // If the candidate is the root node, set the root to null and stop the recursion
            if (*candidate == getRoot()) {
                removeNode(*candidate);
                setRoot(NULL);
                return;
            }

            // Iterate over all outgoing edges of the candidate and erase this node as a parent from all its children
            AnnotatedGraphNode::LeavingEdges::Iterator edgeIter = (*candidate)->getLeavingEdgesIterator();
            while (edgeIter->hasNext()) {
                AnnotatedGraphEdge* currentEdge = edgeIter->getNext();
                parentNodes[currentEdge->getDstNode()].erase(*candidate);
            }
            delete edgeIter;

            // Iterate over all parent nodes and designate as new candidates after removing the edge(s)
            for (set<AnnotatedGraphNode*>::iterator parentOfCandidate = parentNodes[(*candidate)].begin(); parentOfCandidate != parentNodes[(*candidate)].end(); ++parentOfCandidate) {
                (*parentOfCandidate)->removeEdgesToNode(*candidate);
                newCandidates.insert(*parentOfCandidate);
            }

            // Remove the candidate from the graph
            removeNode(*candidate);
            // If this node was designated by a another current candidate as a new candidate,
            // remove it from the set since it does not exist anymore.
            newCandidates.erase(*candidate);
            delete *candidate;
        }
    }
    // If there are no new candidates, stop the recursion, else proceed with the new candidates.
    if (newCandidates.empty()) {
        return;
    } else {
        removeReachableFalseNodesRecursively(newCandidates, parentNodes);
    }

    TRACE(TRACE_5, "AnnotatedGraph::removeReachableFalseNodesRecursively(): end\n");
}


//! \brief removes all nodes that have been disconnected from the root
//!        node due to other node removals
void AnnotatedGraph::removeUnreachableNodes() {

    TRACE(TRACE_5, "AnnotatedGraph::removUnreachableNodes(): start\n");

    set<AnnotatedGraphNode*> unreachableNodes;

    // copy the nodes of the annotated graph into a set, assuming that all
    // are unreachable from the root node
    for (nodes_iterator nodeIter = setOfNodes.begin(); nodeIter != setOfNodes.end(); nodeIter++) {
        unreachableNodes.insert(*nodeIter);
    }

    // start following connections through the graph from the root node
    // and remove every reachable node from the set of unreachable nodes
    if (!hasNoRoot()) {
        removeUnreachableNodesRecursively(getRoot(), unreachableNodes);
    }

    // remove all disconnected nodes
    for (set<AnnotatedGraphNode*>::iterator nodeIter = unreachableNodes.begin();
         nodeIter != unreachableNodes.end(); ++nodeIter) {

        AnnotatedGraphNode* currentNode = *nodeIter;

        TRACE(TRACE_3, "\t\tremoved unreachable node: " + (*nodeIter)->getName() + "\n");

        AnnotatedGraphNode::LeavingEdges::Iterator edgeIter = currentNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* currentEdge = edgeIter->getNext();
            currentNode->removeLiteralFromAnnotation(currentEdge->getLabel());
            delete currentEdge;
            edgeIter->remove();
        }

        delete edgeIter;

        removeNode(currentNode);
        delete currentNode;
    }

    TRACE(TRACE_5, "AnnotatedGraph::removeUnreachableNodes(): end\n");
}


//! \brief collects all connected Nodes in a set
//! \param currentNode Current node to be added to the set
//! \param connectedNodes set of connected nodes
void AnnotatedGraph::removeUnreachableNodesRecursively(AnnotatedGraphNode* currentNode,
                                                        set<AnnotatedGraphNode*>& unreachableNodes) {

    TRACE(TRACE_5, "AnnotatedGraph::removeUnreachableNodesRecursively(AnnotatedGraphNode* currentNode, set<AnnotatedGraphNode*>& unreachableNodes): start\n");

    // remove the node from the set of unreachable nodes
    unreachableNodes.erase(currentNode);

    TRACE(TRACE_4, "\t\tnode " + currentNode->getName() + " is reachable.\n");

    // iterate over all edges
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = currentNode->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();

        // if the edge leads to a node that is yet assumed unreachable
        if (unreachableNodes.find(edge->getDstNode()) != unreachableNodes.end()) {
            removeUnreachableNodesRecursively(edge->getDstNode(), unreachableNodes);
        }
    }
    delete edgeIter;

    TRACE(TRACE_5, "AnnotatedGraph::removeUnreachableNodesRecursively(AnnotatedGraphNode* currentNode, set<AnnotatedGraphNode*>& unreachableNodes): end\n");
}


//! \brief takes an AnnotatedGraph, finds equivalent nodes and minimizes
//!        the graph such that it still characterizes the same strategies
void AnnotatedGraph::minimizeGraph() {
    TRACE(TRACE_5, "AnnotatedGraph::minimizeGraph(): start\n");
    trace( "starting minimization...\n");

    TRACE(TRACE_1, "number of nodes before minimization: " + intToString(setOfNodes.size()) + "\n");

    time_t seconds, seconds2;
    seconds = time(NULL);

    // terminology:
    // greater node means a node stored behind current node in setOfNodes

    // false nodes are removed to increase performance only;
    // should also work if nodes are unsatisfiable
    TRACE(TRACE_1, "removing false nodes...\n");
    removeReachableFalseNodes();
    removeUnreachableNodes();

    // we only have to minimize if at least two blue nodes are present...
    if (setOfNodes.size() >= 2) {

        // iterators to consider all pairs of nodes
        nodes_const_iterator iNode, jNode;

        // remember for a node the first greater equivalent node
        map<AnnotatedGraphNode*, AnnotatedGraphNode*> firstEquivalentNode;

        // needed for redirecting incoming edges of equivalent nodes
        map<AnnotatedGraphNode*, AnnotatedGraphNode::LeavingEdges> myIncomingEdges;

        // iterator over all edges for collecting information to myIncomingEdges
        // and for redirecting incoming edges of equivalent nodes
        AnnotatedGraphNode::LeavingEdges::ConstIterator iEdge;

        // 1) checking for each node whether there is a greater node that is equivalent
        for (iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {

            // remember ingoing edges of each node for later merging of equivalent nodes
            iEdge = (*iNode)->getLeavingEdgesConstIterator();
            while (iEdge->hasNext()) {
                AnnotatedGraphEdge* edge = iEdge->getNext();
                // node is the predeccessor
                myIncomingEdges[edge->getDstNode()].add(edge);
            }

            // iterate over each greater node, trying to find at least one equivalent one
            for (jNode = iNode + 1; jNode != setOfNodes.end(); ++jNode) {

                if (isEquivalent(*iNode, *jNode)) {
                    TRACE(TRACE_2, "\tnodes:\t(" + (*iNode)->getName());
                    TRACE(TRACE_2, ", " + (*jNode)->getName() + ")");
                    TRACE(TRACE_2, "\t ...are equivalent :)\n");

                    // remember that they are equivalent
                    firstEquivalentNode[*iNode] = *jNode;

                    // stop inner loop if first equivalent node found and
                    // continue outer loop
                    break;
                } else {
                    TRACE(TRACE_3, "\tnodes:\t(" + (*iNode)->getName());
                    TRACE(TRACE_3, ", " + (*jNode)->getName() + ")");
                    TRACE(TRACE_3, "\t ...are NOT equivalent :(\n");
                }
            }
        }

        // now merging equivalent OG nodes

        // redirect incoming arcs of a node to its first greater equivalent node
        // and delete the node afterwards
        for (iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
            if (firstEquivalentNode[*iNode] == NULL) {
                continue;
            } else {
                // current node has equivalent nodes, so redirect its incoming edges
                iEdge = myIncomingEdges[*iNode].getConstIterator();
                while (iEdge->hasNext()) {
                    AnnotatedGraphEdge* edge = iEdge->getNext();
                    edge->setDstNode(firstEquivalentNode[*iNode]);
                    // for (possibly) later merging the first equivalent node
                    // with another equivalent node, add all redirected edges
                    // also to list of incoming edges
                    myIncomingEdges[firstEquivalentNode[*iNode]].add(edge);
                }

                // if root node has equivalent nodes, then set root flag to new node
                if (*iNode == getRoot()) {
                    setRoot(firstEquivalentNode[*iNode]);
                }
            }
        }

        // remove all nodes that became unreachable
        removeUnreachableNodes();

    } else {
        // we have less than 2 blue nodes :(
        trace("Too few (blue) nodes. Minimizing skipped...\n\n");
    }

    seconds2 = time(NULL);
    cout << "    " << difftime(seconds2, seconds) << " s consumed" << endl;
    trace( "finished minimization...\n\n");

    // nicht noetig, weil final flag wird bewahrt
    // assignFinalNodes();
    TRACE(TRACE_1, "number of nodes after minimization: " + intToString(setOfNodes.size()) + "\n\n");

/*
    if (!options[O_OUTFILEPREFIX]) {
        outfilePrefix = stripOGFileSuffix(this->getFilename()) + ".minimal";
    }

    if (!options[O_NOOUTPUTFILES]) {
        trace("Saving minimized annotated graph to:\n");
        trace(AnnotatedGraph::addOGFileSuffix(outfilePrefix));
        trace("\n\n");

        // the second parameter is false, since this OG has no underlying oWFN
        createOGFile(outfilePrefix, false);

        if (!parameters[P_NOPNG]) {
            createDotFile(outfilePrefix + ".og");
        }
    }
*/

    TRACE(TRACE_5, "AnnotatedGraph::minimizeGraph(): end\n");
}


//! \brief checks, whether this AnnotatedGraph simulates the given simulant
//! \return true on positive check, otherwise: false
//! \param smallerOG the simulant that should be simulated
bool AnnotatedGraph::simulates(AnnotatedGraph* smallerOG) {
    TRACE(TRACE_5, "AnnotatedGraph::simulates(AnnotatedGraph* smallerOG): start\n");

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

    TRACE(TRACE_5, "AnnotatedGraph::simulates(AnnotatedGraph* smallerOG): end\n");
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

    TRACE(TRACE_2, "\t checking whether node " + myNode->getName());
    TRACE(TRACE_3, " of " + greaterOG->getFilename());
    TRACE(TRACE_2, " simulates node " + simNode->getName());
    TRACE(TRACE_3, " of " + smallerOG->getFilename());
    TRACE(TRACE_2, "\n");

    // If we already visited this pair of nodes, then we're done.
    if (visitedNodes.find(make_pair(myNode, simNode)) != visitedNodes.end()) {
        TRACE(TRACE_3, "\t already been checked\n");
        return true;
    } else {
        visitedNodes.insert(make_pair(myNode, simNode));
    }

    // 1st step:
    // first we check implication of annotations: simNode -> myNode
    TRACE(TRACE_3, "\t\t checking annotations (2nd node's annotation implies 1st?)...\n");
    GraphFormulaCNF* simNodeAnnotationInCNF = simNode->getAnnotation()->getCNF();
    GraphFormulaCNF* myNodeAnnotationInCNF = myNode->getAnnotation()->getCNF();

    if (simNodeAnnotationInCNF->implies(myNodeAnnotationInCNF)) {
        TRACE(TRACE_3, "\t\t\t annotations ok\n");
        TRACE(TRACE_4, "\t\t\t   " + simNode->getAnnotation()->asString() + "\n");
        TRACE(TRACE_4, "\t\t\t   ->\n");
        TRACE(TRACE_4, "\t\t\t   " + myNode->getAnnotation()->asString() + "\n");

        delete simNodeAnnotationInCNF;
        delete myNodeAnnotationInCNF;
    } else {
        TRACE(TRACE_2, "\t\t simulation failed (annotation)\n");

        TRACE(TRACE_4, "\t\t\t   " + simNode->getAnnotation()->asString() + "\n");
        TRACE(TRACE_4, "\t\t\t   -/->\n");
        TRACE(TRACE_4, "\t\t\t   " + myNode->getAnnotation()->asString() + "\n");

        // reporting all leaving edges of both nodes for debugging
        AnnotatedGraphNode::LeavingEdges::Iterator edgeIter;

        TRACE(TRACE_4, "\t\t\t   leaving edges of node ");
        TRACE(TRACE_4, simNode->getName() + " of ");
        TRACE(TRACE_4, smallerOG->getFilename());
        TRACE(TRACE_4, " (" + intToString(simNode->getLeavingEdgesCount())
                + "):\n");

        edgeIter = simNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            TRACE(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> "
                    + edge->getDstNode()->getName() + " (");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    TRACE(TRACE_4, "BLUE");
                    break;
                case RED:
                    TRACE(TRACE_4, "RED");
                    break;
            }
            TRACE(TRACE_4, ")\n");
        }

        TRACE(TRACE_4, "\t\t\t   leaving edges of node ");
        TRACE(TRACE_4, myNode->getName() + " of ");
        TRACE(TRACE_4, greaterOG->getFilename());
        TRACE(TRACE_4, " (" + intToString(myNode->getLeavingEdgesCount())
                + "):\n");

        edgeIter = myNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            TRACE(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> "
                    + edge->getDstNode()->getName() + "(");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    TRACE(TRACE_4, "BLUE");
                    break;
                case RED:
                    TRACE(TRACE_4, "RED");
                    break;
            }
            TRACE(TRACE_4, ")\n");
        }

        delete edgeIter;
        delete simNodeAnnotationInCNF;
        delete myNodeAnnotationInCNF;

        return false;
    }

    // 2nd step:
    // now we check whether myNode has each outgoing event of simNode
    TRACE(TRACE_3, "\t\t checking edges...\n");
    AnnotatedGraphNode::LeavingEdges::ConstIterator simEdgeIter = simNode->getLeavingEdgesConstIterator();

    while (simEdgeIter->hasNext()) {
        AnnotatedGraphEdge* simEdge = simEdgeIter->getNext();

        TRACE(TRACE_4, "\t\t\t checking event " + simEdge->getLabel() + "\n");

        AnnotatedGraphEdge* myEdge = myNode->getEdgeWithLabel(simEdge->getLabel());

        if (myEdge == NULL) {
            // simNode has edge which myNode hasn't
            TRACE(TRACE_2, "\t\t simulation failed (edges)\n");

            // reporting all leaving edges of both nodes for debugging
            AnnotatedGraphNode::LeavingEdges::Iterator edgeIter;

            TRACE(TRACE_4, "\t\t\t   leaving edges of node ");
            TRACE(TRACE_4, simNode->getName() + " of ");
            TRACE(TRACE_4, smallerOG->getFilename());
            TRACE(TRACE_4, " (" + intToString(simNode->getLeavingEdgesCount()) + "):\n");

            edgeIter = simNode->getLeavingEdgesIterator();
            while (edgeIter->hasNext()) {
                AnnotatedGraphEdge* edge = edgeIter->getNext();
                TRACE(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> " + edge->getDstNode()->getName() + " (");
                switch (edge->getDstNode()->getColor()) {
                    case BLUE:
                        TRACE(TRACE_4, "BLUE");
                        break;
                    case RED:
                        TRACE(TRACE_4, "RED");
                        break;
                }
                TRACE(TRACE_4, ")\n");
            }

            TRACE(TRACE_4, "\t\t\t   leaving edges of node ");
            TRACE(TRACE_4, myNode->getName() + " of ");
            TRACE(TRACE_4, greaterOG->getFilename());
            TRACE(TRACE_4, " (" + intToString(myNode->getLeavingEdgesCount()) + "):\n");

            edgeIter = myNode->getLeavingEdgesIterator();
            while (edgeIter->hasNext()) {
                AnnotatedGraphEdge* edge = edgeIter->getNext();
                TRACE(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> "
                        + edge->getDstNode()->getName() + "(");
                switch (edge->getDstNode()->getColor()) {
                    case BLUE:
                        TRACE(TRACE_4, "BLUE");
                        break;
                    case RED:
                        TRACE(TRACE_4, "RED");
                        break;
                }
                TRACE(TRACE_4, ")\n");
            }

            delete edgeIter;
            delete simEdgeIter;

            return false;
        } else {
            TRACE(TRACE_4, "\t\t\t event present, going down\n");

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
    TRACE(TRACE_5, "AnnotatedGraph::isEquivalent(AnnotatedGraph* secondOG): start\n");

    // Simulation is impossible without a simulant.
    if (secondOG == NULL) {
        return false;
    }

    // if one of the OGs is empty, both need to be empty in order to be equivalent
    if (root == NULL || secondOG->getRoot() == NULL) {
        return (root == NULL && secondOG->getRoot() == NULL);
    }

    // We need to remember the pairs of nodes we already visited.
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > visitedNodes;

    // Get things moving...
    bool result = false;
    if (isEquivalentRecursive(root, secondOG->getRoot(), visitedNodes, this,
            secondOG)) {
        result = true;
    }

    TRACE(TRACE_5, "AnnotatedGraph::isEquivalent(AnnotatedGraph* secondOG): end\n");
    return result;
}


//! \brief checks, whether the given two AnnotatedGraphNodes of this AnnotatedGraph
//! are equivalent
//! \return true on positive check, otherwise: false
//! \param leftNode the first AnnotatedGraphNode
//! \param rightNode the second AnnotatedGraphNode
bool AnnotatedGraph::isEquivalent(AnnotatedGraphNode* leftNode,
                                  AnnotatedGraphNode* rightNode) {
    TRACE(TRACE_5, "AnnotatedGraph::isEquivalent(AnnotatedGraphNode* leftNode, AnnotatedGraphNode* rightNode): start\n");

    assert(leftNode);
    assert(rightNode);

    // We need to remember the pairs of nodes we already visited.
    set<pair<AnnotatedGraphNode*, AnnotatedGraphNode*> > visitedNodes;

    // Get things moving...
    bool result = false;
    if (isEquivalentRecursive(leftNode, rightNode, visitedNodes, this, this)) {
        result = true;
    }

    TRACE(TRACE_5, "AnnotatedGraph::isEquivalent(AnnotatedGraphNode* firstNode, AnnotatedGraphNode* secondNode): end\n");
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

    TRACE(TRACE_2, "\t checking whether node " + leftNode->getName());
    TRACE(TRACE_3, " of " + leftOG->getFilename());
    TRACE(TRACE_2, " is equivalent to node " + rightNode->getName());
    TRACE(TRACE_3, " of " + rightOG->getFilename());
    TRACE(TRACE_2, "\n");

    // If we already visited this pair of nodes, then we're done.
    if (visitedNodes.find(make_pair(leftNode, rightNode)) != visitedNodes.end()) {
        TRACE(TRACE_3, "\t already been checked\n");
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
    TRACE(TRACE_3, "\t\t checking equivalence of annotations...\n");
    GraphFormulaCNF* leftNodeAnnotationInCNF = leftNode->getAnnotation()->getCNF();
    GraphFormulaCNF* rightNodeAnnotationInCNF = rightNode->getAnnotation()->getCNF();

    if (leftNodeAnnotationInCNF->implies(rightNodeAnnotationInCNF)) {
        TRACE(TRACE_3, "\t\t first annotation implication is ok\n");
        TRACE(TRACE_4, "\t\t\t   " + leftNode->getAnnotation()->asString() + "\n");
        TRACE(TRACE_4, "\t\t\t   ->\n");
        TRACE(TRACE_4, "\t\t\t   " + rightNode->getAnnotation()->asString() + "\n");

        if (rightNodeAnnotationInCNF->implies(leftNodeAnnotationInCNF)) {
            TRACE(TRACE_3, "\t\t second annotation implication is ok\n");
            TRACE(TRACE_4, "\t\t\t   " + rightNode->getAnnotation()->asString() + "\n");
            TRACE(TRACE_4, "\t\t\t   ->\n");
            TRACE(TRACE_4, "\t\t\t   " + leftNode->getAnnotation()->asString() + "\n");
        } else {
            TRACE(TRACE_3, "\t\t\t annotation implication false\n");
            TRACE(TRACE_4, "\t\t\t   " + rightNode->getAnnotation()->asString() + "\n");
            TRACE(TRACE_4, "\t\t\t   -/->\n");
            TRACE(TRACE_4, "\t\t\t   " + leftNode->getAnnotation()->asString() + "\n");

            result = false;
        }
    } else {
        TRACE(TRACE_2, "\t\t equivalence failed (annotation)\n");
        TRACE(TRACE_3, "\t\t\t annotation implication false\n");
        TRACE(TRACE_4, "\t\t\t   " + leftNode->getAnnotation()->asString() + "\n");
        TRACE(TRACE_4, "\t\t\t   -/->\n");
        TRACE(TRACE_4, "\t\t\t   " + rightNode->getAnnotation()->asString() + "\n");

        result = false;
    }

    delete rightNodeAnnotationInCNF;
    delete leftNodeAnnotationInCNF;

    if (result == false) {
        // reporting all leaving edges of both nodes for debugging
        AnnotatedGraphNode::LeavingEdges::Iterator edgeIter;

        TRACE(TRACE_4, "\t\t\t   leaving edges of node ");
        TRACE(TRACE_4, rightNode->getName() + " of ");
        TRACE(TRACE_4, rightOG->getFilename());
        TRACE(TRACE_4, " (" + intToString(rightNode->getLeavingEdgesCount())
                + "):\n");

        edgeIter = rightNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            TRACE(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> " + edge->getDstNode()->getName() + " (");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    TRACE(TRACE_4, "BLUE");
                    break;
                case RED:
                    TRACE(TRACE_4, "RED");
                    break;
            }
            TRACE(TRACE_4, ")\n");
        }

        TRACE(TRACE_4, "\t\t\t   leaving edges of node ");
        TRACE(TRACE_4, leftNode->getName() + " of ");
        TRACE(TRACE_4, leftOG->getFilename());
        TRACE(TRACE_4, " (" + intToString(leftNode->getLeavingEdgesCount()) + "):\n");

        edgeIter = leftNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            TRACE(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> " + edge->getDstNode()->getName() + "(");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    TRACE(TRACE_4, "BLUE");
                    break;
                case RED:
                    TRACE(TRACE_4, "RED");
                    break;
            }
            TRACE(TRACE_4, ")\n");
        }

        delete edgeIter;

        return false;
    }

    // 2nd step:
    // now we check whether both nodes have the same outgoing events
    TRACE(TRACE_3, "\t\t checking edges...\n");

    TRACE(TRACE_3, "\t\t\t checking whether each edge of ");
    TRACE(TRACE_3, leftNode->getName());
    TRACE(TRACE_3, " of " + leftOG->getFilename() + "\n");
    TRACE(TRACE_3, "\t\t\t                 is present at ");
    TRACE(TRACE_3, rightNode->getName());
    TRACE(TRACE_3, " of " + rightOG->getFilename() + "...\n");

    edgeIter = leftNode->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* leftEdge = edgeIter->getNext();
        TRACE(TRACE_4, "\t\t\t\t checking event " + leftEdge->getLabel() + "\n");

        AnnotatedGraphEdge* rightEdge = rightNode->getEdgeWithLabel(leftEdge->getLabel());
        if (rightEdge == NULL) {
            // first node has edge which second node hasn't
            TRACE(TRACE_4, "\t\t\t\t event missing\n");
            result = false;
            break;
        } else {
            TRACE(TRACE_4, "\t\t\t\t event present\n");
        }
    }

    TRACE(TRACE_3, "\t\t\t checking whether each edge of ");
    TRACE(TRACE_3, rightNode->getName());
    TRACE(TRACE_3, " of " + rightOG->getFilename() + "\n");
    TRACE(TRACE_3, "\t\t\t                 is present at ");
    TRACE(TRACE_3, leftNode->getName());
    TRACE(TRACE_3, " of " + leftOG->getFilename() + "...\n");

    edgeIter = rightNode->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* rightEdge = edgeIter->getNext();
        TRACE(TRACE_4, "\t\t\t\t checking event " + rightEdge->getLabel() + "\n");

        AnnotatedGraphEdge* leftEdge = leftNode->getEdgeWithLabel(rightEdge->getLabel());
        if (leftEdge == NULL) {
            // first node has edge which second node hasn't
            TRACE(TRACE_4, "\t\t\t\t event missing\n");
            result = false;
            break;
        } else {
            TRACE(TRACE_4, "\t\t\t\t event present\n");
        }
    }

    if (result == false) {
        TRACE(TRACE_2, "\t\t equivalence failed (edges)\n");
        // reporting all leaving edges of both nodes for debugging
        AnnotatedGraphNode::LeavingEdges::Iterator edgeIter;

        TRACE(TRACE_4, "\t\t\t   leaving edges of node ");
        TRACE(TRACE_4, rightNode->getName() + " of ");
        TRACE(TRACE_4, rightOG->getFilename());
        TRACE(TRACE_4, " (" + intToString(rightNode->getLeavingEdgesCount()) + "):\n");

        edgeIter = rightNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            TRACE(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> "
                    + edge->getDstNode()->getName() + " (");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    TRACE(TRACE_4, "BLUE");
                    break;
                case RED:
                    TRACE(TRACE_4, "RED");
                    break;
            }
            TRACE(TRACE_4, ")\n");
        }

        TRACE(TRACE_4, "\t\t\t   leaving edges of node ");
        TRACE(TRACE_4, leftNode->getName() + " of ");
        TRACE(TRACE_4, leftOG->getFilename());
        TRACE(TRACE_4, " (" + intToString(leftNode->getLeavingEdgesCount()) + "):\n");

        edgeIter = leftNode->getLeavingEdgesIterator();
        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();
            TRACE(TRACE_4, "\t\t\t\t\t" + edge->getLabel() + " --> " + edge->getDstNode()->getName() + "(");
            switch (edge->getDstNode()->getColor()) {
                case BLUE:
                    TRACE(TRACE_4, "BLUE");
                    break;
                case RED:
                    TRACE(TRACE_4, "RED");
                    break;
            }
            TRACE(TRACE_4, ")\n");
        }

        delete edgeIter;
        return false;
    }

    // we know: both nodes have the same outgoing edges
    TRACE(TRACE_4, "\t\t\t all events present at both nodes, going down\n");

    edgeIter = leftNode->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* leftEdge = edgeIter->getNext();
        TRACE(TRACE_4, "\t\t\t performing event " + leftEdge->getLabel() + "\n");
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


//! \brief Returns the product OG of this OG and the passed one. The caller has to delete the returned
//!        AnnotatedGraph.
//! \param rhs the OG to be used for computing the product
//! \return returns the product OG
AnnotatedGraph* AnnotatedGraph::product(const AnnotatedGraph* rhs) {

    TRACE(TRACE_5, "AnnotatedGraph::product(const AnnotatedGraph* rhs): start\n");

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

    string currentName;
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
        productOG->removeReachableFalseNodes();
    }

    // Assign the final nodes for the product
    productOG->assignFinalNodes();

    TRACE(TRACE_5, "AnnotatedGraph::product(const AnnotatedGraph* rhs): end\n");

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
                                    AnnotatedGraphNode* currentRhsNode,
                                    AnnotatedGraph* productOG) {

    TRACE(TRACE_5, "AnnotatedGraph::buildProductOG(AnnotatedGraphNode* currentOGNode, AnnotatedGraphNode* currentRhsNode, AnnotatedGraph* productOG): start\n");

    // at this time, the node constructed from currentOGNode and
    // currentRhsNode is already inserted

    assert(productOG->getRoot() != NULL);

    // iterate over all outgoing edges from current node of OG
    string currentLabel;
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =
            currentOGNode->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* edge = edgeIter->getNext();

        // remember the label of the egde
        currentLabel = edge->getLabel();

        // if the rhs automaton allows this edge
        if (currentRhsNode->hasEdgeWithLabel(currentLabel)) {

            // remember the name of the old node of the product OG
            string currentName;
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
            string newProductName;
            newProductName = newOGNode->getName() + "x"+ newRhsNode->getName();
            // if the node is new, add that node to the OG
            AnnotatedGraphNode* found =
                    productOG->getNodeWithName(newProductName);

            if (found != NULL) {
                // the node was known before, so we just have to add a new edge
                productOG->addEdge(currentName, newProductName, currentLabel);
            } else {
                // we computed a new node, so we add a node and an edge
                // trace( "adding node " + newNode->getName() + " with annotation " + newNode->getAnnotation()->asString() + "\n");

                GraphFormulaCNF* newProductFormula =
                    createProductAnnotation(newOGNode, newRhsNode);

                AnnotatedGraphNode* newProductNode =
                    new AnnotatedGraphNode(newProductName, newProductFormula);

                productOG->addNode(newProductNode);

                // going down recursively
                productOG->addEdge(currentName, newProductName, currentLabel);

                buildProductOG(newOGNode, newRhsNode, productOG);
            }
        }
    }

    delete edgeIter;

    TRACE(TRACE_5, "AnnotatedGraph::buildProductOG(AnnotatedGraphNode* currentOGNode, AnnotatedGraphNode* currentRhsNode, AnnotatedGraph* productOG): end\n");
}


//! \brief Creates and returns the annotation for the product node of the given two nodes.
//!        The caller has to delete the formula after usage.
//! \param lhs the first node
//! \param rhs the second node
//! \return returns the Formula of the product node
GraphFormulaCNF* AnnotatedGraph::createProductAnnotation(const AnnotatedGraphNode* lhs,
                                                         const AnnotatedGraphNode* rhs) const {

    GraphFormulaMultiaryAnd* conjunction =
        new GraphFormulaMultiaryAnd(lhs->getAnnotation()->getDeepCopy(),
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
string AnnotatedGraph::getProductOGFilePrefix(const ogfiles_t& ogfiles) {
    assert(ogfiles.size() != 0);

    ogfiles_t::const_iterator iOgFile = ogfiles.begin();
    string productFilePrefix = stripOGFileSuffix(*iOgFile);

    for (++iOgFile; iOgFile != ogfiles.end(); ++iOgFile) {
        productFilePrefix += "_X_" + stripOGFileSuffix(platform_basename(*iOgFile));
    }

    return productFilePrefix;
}


//! \brief strips the OG file suffix from filename and returns the result
//! \param filename name of the og file
//! \param returns the filename without suffix
string AnnotatedGraph::stripOGFileSuffix(const std::string& filename) {

    static const string ogFileSuffix = ".og";
    if (filename.substr(filename.size() - ogFileSuffix.size()) == ogFileSuffix) {
        return filename.substr(0, filename.size() - ogFileSuffix.size());
    }

    return filename;
}


//! \brief creates a dot output of the graph
//! \param filenamePrefix a string containing the prefix of the output file name
//! \param dotGraphTitle a title for the graph to be shown in the image
string AnnotatedGraph::createDotFile(string& filenamePrefix,
                                     const string& dotGraphTitle) const {

    unsigned int maxSizeForDotFile = 5000; // number relevant for .out file

    if (((parameters[P_SHOW_RED_NODES] || parameters[P_SHOW_ALL_NODES]) &&
            (getNumberOfNodes() <= maxSizeForDotFile))
         ||
         ((parameters[P_SHOW_EMPTY_NODE] || parameters[P_SHOW_BLUE_NODES]) &&
         getNumberOfBlueNodes() <= maxSizeForDotFile)) {

            trace( "creating the dot file of the OG...\n");
        
            string dotFile = filenamePrefix + ".out";
            fstream dotFileHandle(dotFile.c_str(), ios_base::out | ios_base::trunc);
            if (!dotFileHandle.good()) {
                dotFileHandle.close();
                return "";
            }
            dotFileHandle << "digraph g1 {\n";
            dotFileHandle << "graph [fontname=\"Helvetica\", label=\"";
            dotFileHandle << dotGraphTitle;
            dotFileHandle << "\"];\n";
            dotFileHandle << "node [fontname=\"Helvetica\" fontsize=10];\n";
            dotFileHandle << "edge [fontname=\"Helvetica\" fontsize=10];\n";
        
            std::map<AnnotatedGraphNode*, bool> visitedNodes;
            createDotFileRecursively(getRoot(), dotFileHandle, visitedNodes);
        
            dotFileHandle << "}";
            dotFileHandle.close();
        
            // ... dot file created (.out) //
            return dotFile;
    } else {
            trace( "graph is too big to create dot file\n");
    }
    
    return "";
}


//! \brief annotates a given dot output file and stores it to a .dot-File.
//! \param filenamePrefix a string containing the prefix of the output file name
//! \param dotFileName the base dot file
string AnnotatedGraph::createAnnotatedDotFile(string& filenamePrefix, std::string& dotFileName) const {

        string annotatedDotFileName = filenamePrefix + ".dot";
        // annotate .dot file
        system(("dot -Tdot " + dotFileName + " -o " + annotatedDotFileName).c_str());

        return annotatedDotFileName;
}


//! \brief calls dot to create an image (.png) of the given dot file (.out)
//! \param filenamePrefix a string containing the prefix of the output file name
//! \param dotFileName the base .out file
string AnnotatedGraph::createPNGFile(string& filenamePrefix, std::string& dotFileName) const {

    string pngFile = filenamePrefix + ".png";

    // prepare dot command line for printing
    string cmd = "dot -Tpng \"" + dotFileName + "\" -o \""+ pngFile + "\"";

    // print commandline and execute system command
    trace( cmd + "\n\n");
    system(cmd.c_str());

    return pngFile;

}


//! \brief creates a dot output of the graph, uses the filename as title.
//! \param filenamePrefix a string containing the prefix of the output file name
string AnnotatedGraph::createDotFile(string& filenamePrefix) const {
    return createDotFile(filenamePrefix, filenamePrefix);
}


//! \brief dfs through the graph printing each node and edge to the output stream
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes maps nodes to Bools remembering already visited nodes
void AnnotatedGraph::createDotFileRecursively(AnnotatedGraphNode* v,
                                              fstream& os,
                                              std::map<AnnotatedGraphNode*, bool>& visitedNodes) const {

    if (v == NULL) {
        // print the empty OG...
        os << "p0"
                << " [label=\"#0\", fontcolor=black, color=red, style=dashed];\n";
        return;
    }

    if (visitedNodes[v] != true) {

        if (v->isFinal()) {
            os << "p"<< v->getName() << " [label=\"# "<< v->getName() << "\\n";
            os << v->getAnnotation()->asString();
            os << "\", fontcolor=black, color=blue, peripheries=2];\n";
        } else {
            os << "p"<< v->getName() << " [label=\"# "<< v->getName() << "\\n";
            os << v->getAnnotation()->asString();
            os << "\", fontcolor=black, color=blue];\n";
        }

        visitedNodes[v] = true;

        string currentLabel;

        AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter =
            v->getLeavingEdgesConstIterator();

        while (edgeIter->hasNext()) {
            AnnotatedGraphEdge* edge = edgeIter->getNext();

            // remember the label of the egde
            currentLabel = edge->getLabel();
            AnnotatedGraphNode* successor = edge->getDstNode();
            assert(successor != NULL);

            os << "p" << v->getName() << "->" << "p" << successor->getName()
               << " [label=\""<< currentLabel
               << "\", fontcolor=black, color= blue];\n";

            createDotFileRecursively(successor, os, visitedNodes);
        }
        delete edgeIter;
    }
}


//! \brief Computes input and output placenames from the graph.
void AnnotatedGraph::computeInterfaceFromGraph() {

    inputPlacenames.clear();
    outputPlacenames.clear();

    map<AnnotatedGraphNode*, bool> visitedNodes;    // visited nodes
    AnnotatedGraphNode* rootNode = getRoot();       // root node

    computeInterfaceRecursively(rootNode, visitedNodes);
}


//! \brief depth-first-search through the graph for computing input/output placenames
//! \param v a pointer to a AnnotatedGraphNode which is the current node
//! \param visitedNodes a reference to a map from AnnotatedGraphNode-Pointers
//to boolean values which is used for marking all nodes that we have looked at so far
void AnnotatedGraph::computeInterfaceRecursively(AnnotatedGraphNode* v,
                                                std::map<AnnotatedGraphNode*, bool>& visitedNodes) {

    assert(v != NULL);
    visitedNodes[v] = true; // mark current node as visited

    //if ( !v->isToShow(root, (PN != NULL)) )
    //    return;

    // go through all arcs
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();

        //if ( !vNext->isToShow(root, (PN != NULL)) ) {
        //    continue; // continue if node is not to show
        //}


        // build inputPlacenames and outputPlacenames
		string currentLabel = element->getLabel();
        string currentPlacename = currentLabel.substr(1);

        if (currentLabel[0] == '?') {
            inputPlacenames.insert(currentPlacename);
        } else if (currentLabel[0] == '!') {
            outputPlacenames.insert(currentPlacename);
        } else {
            cerr << "Error in computeInterfaceRecursively: Found unknown label." << endl;
        }

        // recursion
        if ( vNext != v && visitedNodes.find(vNext) == visitedNodes.end() )
        {
            computeInterfaceRecursively(vNext, visitedNodes);
        }
    }
    delete edgeIter;
}


//! \brief Prints this OG in OG file format to a file with the given prefix. The
//!        suffix is added automatically by this method.
//! \param filenamePrefix a prefix for th filename
//! \param hasOWFN gives true if this Annotated Graph is also a Communication Graph
//!                that contains it's oWFN. Important to determine whether a true
//!                annotated node is the empty node or not.
string AnnotatedGraph::createOGFile(const string& filenamePrefix, bool hasOWFN) const {

    TRACE(TRACE_5, "AnnotatedGraph::createOGFile(): start\n");
    TRACE(TRACE_1, "creating .og file for " + filenamePrefix + "\n");

    fstream ogFile(addOGFileSuffix(filenamePrefix).c_str(), ios_base::out | ios_base::trunc);

    if (!ogFile.good()) {
            ogFile.close();
            TRACE(TRACE_0, "Error: A file error occured. Exit.");
            exit(EC_FILE_ERROR);
    }
    if (hasNoRoot()) {
        // print file for empty OG
        ogFile << "NODES" << endl << "  0 : " << GraphFormulaLiteral::FALSE
                << " : " << GraphNodeColor(RED).toString() << ';' << endl << endl << "INITIALNODE" << endl
                << "  0;" << endl << endl << "TRANSITIONS" << endl << "  ;"
                << endl;

        ogFile.close();
        return addOGFileSuffix(filenamePrefix);
    }

    ogFile << "INTERFACE" << endl;
    ogFile << "  INPUT" << endl;
    bool printedFirstPlace = false;
    for (set<string>::const_iterator iter = inputPlacenames.begin(); iter != inputPlacenames.end(); iter++) {
        if (printedFirstPlace) ogFile << "," << endl;
        ogFile << "    " << *iter;
        printedFirstPlace = true;
    }
    ogFile << ";" << endl;

    ogFile << "  OUTPUT" << endl;
    printedFirstPlace = false;
    for (set<string>::const_iterator iter = outputPlacenames.begin(); iter != outputPlacenames.end(); iter++) {
        if (printedFirstPlace) ogFile << "," << endl;
        ogFile << "    " << *iter;
        printedFirstPlace = true;
    }
    ogFile << ";" << endl << endl;


    ogFile << "NODES" << endl;
    bool printedFirstNode = false;
    for (nodes_t::const_iterator iNode = setOfNodes.begin(); iNode
            != setOfNodes.end(); ++iNode) {

        AnnotatedGraphNode* node = *iNode;
        if (!node->isToShow(root, hasOWFN)) {
            continue;
        }

        if (printedFirstNode) {
            ogFile << ',' << endl;
        }

        ogFile << "  " << node->getName() << " : ";
        string annotation = node->getAnnotationAsString();
        if ( annotation.find(", ") != string::npos ) {
            ogFile << "[" << annotation << "]";
        } else {
            ogFile << annotation;
        }
        ogFile << " : " << node->getColor().toString();

        if (node->isFinal()) {
            ogFile << " : finalnode";
        }

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
        if (!node->isToShow(root, hasOWFN)) {
            continue;
        }

        AnnotatedGraphNode::LeavingEdges::ConstIterator iEdge =
                node->getLeavingEdgesConstIterator();

        while (iEdge->hasNext()) {
            AnnotatedGraphEdge* edge = iEdge->getNext();
            if (!edge->getDstNode()->isToShow(root, hasOWFN)) {
                continue;
            }

            if (printedFirstEdge) {
                ogFile << ',' << endl;
            }

            ogFile << "  " << node->getName() << " -> " << edge->getDstNode()->getName() << " : ";
            string annotation = edge->getLabel();
            if ( annotation.find(", ") != string::npos ) {
                ogFile << "[" << annotation << "]";
            } else {
                ogFile << annotation;
            }

            printedFirstEdge = true;
        }
        delete iEdge;
    }

    ogFile << ';' << endl;
    ogFile.close();

    TRACE(TRACE_5, "AnnotatedGraph::createOGFile(): end\n");

    return addOGFileSuffix(filenamePrefix);
}


//! \brief adds the suffix ".og" to a string
//! \param filePrefix a string to be modified
//! \return returns the string with the og suffix
string AnnotatedGraph::addOGFileSuffix(const std::string& filePrefix) {
    return filePrefix + ".og";
}


//! \brief remove a node from the AnnotatedGraph
//! \param node node to remove
void AnnotatedGraph::removeNode(AnnotatedGraphNode* node) {

    TRACE(TRACE_5, "AnnotatedGraph::removeNode(): start\n");
    assert(node);

    for (vector<AnnotatedGraphNode*>::iterator testnode = setOfNodes.begin(); testnode
            != setOfNodes.end(); testnode++) {
        if ((*testnode) == node) {
            setOfNodes.erase(testnode);
            break;
        }
    }
    TRACE(TRACE_5, "AnnotatedGraph::removeNode(): end\n");
}


//! \brief removes the emoty node only in case, that this og was created
//!        from an owfn. In case that the OG was read from a file, all
//!        true annotated nodes will be removed
//! \param fromOWFN the algorithm needs to know if there are addition informations
//!                 available from the oWFN
void AnnotatedGraph::removeTrueNodes(bool fromOWFN) {

    // in order to ensure consistency the nodes will be removed after
    // the iteration
    set<AnnotatedGraphNode*> deleteNodes;

    // iterate over all nodes of the AnnotatedGraph
    for (nodes_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end();
         ++node_iter) {

        AnnotatedGraphNode* currNode = *node_iter;

        // If this OG was constructed from an oWFN the empty node
        // can be easily identified containing no states.

        // If this OG was read from an OG file, the states stored
        // in a node are not present and thus the empty node is not
        // identifiable. In this case all true nodes will be
        // deleted although there exist services which do not maintain
        // their behaviour, this is a TODO

        if (fromOWFN && currNode->reachGraphStateSet.empty()) {
            // is an empty node and will be removed
            removeEdgesToNodeFromAllOtherNodes(currNode);
            removeNode(currNode);
            TRACE(TRACE_2, "        Removing emptyNode: " + currNode->getName());
            delete currNode;
            return;
        } else if (!fromOWFN && currNode->getAnnotationAsString() == GraphFormulaLiteral::TRUE) {
            // node is annotated with true
            // remove node and all its incoming/outgoing transitions
            deleteNodes.insert(currNode);
            TRACE(TRACE_2, "        Removing node: " + currNode->getName());
            TRACE(TRACE_3, "\n            Annotation: " + currNode->getAnnotationAsString());
            TRACE(TRACE_2, "\n");
            delete currNode;
            continue;
        }
    }

    // remove all true annotated nodes, in case this OG was read from a file
    for (set<AnnotatedGraphNode*>::iterator node_iter = deleteNodes.begin();
         node_iter != deleteNodes.end();
         ++node_iter) {
        removeEdgesToNodeFromAllOtherNodes(*node_iter);
        removeNode(*node_iter);
    }
}


//! \brief constructs the dual service, by transforming all sending
//!        events into receiving events and vice versa
void AnnotatedGraph::constructDualService() {

    // iterate over all nodes of the AnnotatedGraph
    for (nodes_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end();
         ++node_iter) {

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
                    trace(
                            "discovered an event with unknown type (label was "
                                    +currEdge->getLabel() + ")\n");
                    // do nothing
                    break;
            }
        }
        delete edge_iter;
    }
}


//! \brief applies various fixes to the dual service to ensure
//!        equivalence of the public view's and the original process' OGs
//! \param fromOWFN the algorithm needs to know if there are addition informations
//!                 available from the oWFN
void AnnotatedGraph::fixDualService(bool fromOWFN) {

    // prepare deadlock creation
    AnnotatedGraphNode* deadlock = NULL;
    map<string, AnnotatedGraphNode*> deadlockMap;
    unsigned int currNumberOfDeadlocks = 0;

    // gather created nodes in order to add it to the graph
    set<AnnotatedGraphNode*> createdNodes;

    // if the multiple deadlock option is not activated, create one
    // deadlock node for the whole public view
    if (!options[O_PV_MULTIPLE_DEADLOCKS]) {
        // create new deadlock node
        deadlock = new AnnotatedGraphNode("deadlock", new GraphFormulaFixed(false, GraphFormulaLiteral::FALSE));
    }

    // iterate over all nodes of the AnnotatedGraph
    for (nodes_iterator node_iter = setOfNodes.begin(); node_iter
            != setOfNodes.end(); ++node_iter) {

        AnnotatedGraphNode* currNode = *node_iter;

        // Simplify the annotation so only literals that matter are left over
        currNode->getAnnotation()->simplify();
        string currAnnotation = currNode->getAnnotation()->asString();

        // If !a is disabled in q of OG, add ?a->Deadlock in q of dual service
        // preparation: assume all receive events are disabled in current node (dual service)
        set<string> disabledRecvEvents = recvEvents;

        // preparation: we need to gather outgoing sending transitions
        // which do not appear in the current nodes annotation
        set<string> sendingEventsNotInAnnotation;

        // we need to know whether this node has outgoing sending
        // events that do occur in the annotation
        bool hasSendingEventThatOccursInAnnotation = false;

        // iterate over all leaving edges of the current node
        AnnotatedGraphNode::LeavingEdges::Iterator edge_iter =
                currNode->getLeavingEdgesIterator();
        while (edge_iter->hasNext()) {
            AnnotatedGraphEdge* currEdge = edge_iter->getNext();
            string currLabel = currEdge->getLabel();

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

                    // if the current edges label does not appear, this is a
                    // nonAnnotatedSendingEvent
                    if (currAnnotation.find(currLabel) == string::npos) {
                        currLabel[0] = '!';
                        sendingEventsNotInAnnotation.insert(currLabel);
                    } else {
                        hasSendingEventThatOccursInAnnotation = true;
                    }
                    break;
                default:
                    // type is either INTERNAL or unknown
                    // do nothing
                    break;
            }
        }
        delete edge_iter;

        // iterate over all disabled receive events and create an
        // arc to this node's deadlock node for every such event
        for (set<string>::iterator event_iter = disabledRecvEvents.begin(); event_iter
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
            // labeled with current event
            AnnotatedGraphEdge* disabledEvent = new AnnotatedGraphEdge(deadlock, *event_iter);
            currNode->addLeavingEdge(disabledEvent);
        }

        // In case that this is a true node although the empty node has already been
        // removed and if this node has no outgoing sending or tau transitions, do
        // a local fix for the true annotation of the node. No other fixes will take
        // place for this node.
        string::size_type loc =((currNode)->getAnnotationAsString()).find(GraphFormulaLiteral::TRUE, 0);
        if (loc != string::npos && !hasSendingEventThatOccursInAnnotation && sendingEventsNotInAnnotation.empty()) {
            currNode->addLeavingEdge(new AnnotatedGraphEdge(currNode, GraphFormulaLiteral::TAU));
            // Don't do anything else for this node
            continue;
        }

        // In case that final appears in the OG's formula this node needs to be treated
        // as a final node
        bool hasFinalLiteral = false;
        loc =(currNode->getAnnotationAsString()).find(GraphFormulaLiteral::FINAL, 0);
        if (loc != string::npos) {
            hasFinalLiteral = true;
        }

        // If either this node q is a final Node or has non-annotated sending events
        // this node needs to be fixed. That means a new node q_tau will be created
        // a tau-transition from q to q_tau. Additionally if q was final,
        // q will be set to non-final and instead q_tau will be final. Afterwards
        // q_tau will receive all of q's outgoing transitions, which appear in q's
        // annotation and only receiving ones if q was final.
        if (hasFinalLiteral || !sendingEventsNotInAnnotation.empty()) {
            // create new node
            AnnotatedGraphNode* newNode= new AnnotatedGraphNode(currNode->getName() + "_tau",
                    new GraphFormulaFixed(true, ""), currNode->getColor());
            TRACE(TRACE_3, "            created new node " + newNode->getName()
                    + "\n");

            // create tau transition from current to new node
            AnnotatedGraphEdge* tauTransition= new AnnotatedGraphEdge(newNode, GraphFormulaLiteral::TAU);
            currNode->addLeavingEdge(tauTransition);
            TRACE(TRACE_4, "            created tau transition from "
                    + currNode->getName() +" to "+ newNode->getName() + "\n");

            // add current node's leaving receiving edges to new node
            TRACE(TRACE_4, "            adding leaving edges to new node\n");
            edge_iter = currNode->getLeavingEdgesIterator();
            while (edge_iter->hasNext()) {
                AnnotatedGraphEdge* currEdge = edge_iter->getNext();

                // Only copy edges that represent receiving events or (in case of q not being final
                // are sending events that appear in the annotation
                if (currEdge->getType() == RECEIVING ||
                       (currEdge->getType() == SENDING &&
                        sendingEventsNotInAnnotation.find(currEdge->getLabel()) == sendingEventsNotInAnnotation.end() &&
                        !hasFinalLiteral) ) {

                    // create new leaving edge
                    AnnotatedGraphEdge* newEdge= new AnnotatedGraphEdge((currEdge)->getDstNode(),
                            (currEdge)->getLabel());
                    newNode->addLeavingEdge(newEdge);
                    TRACE(TRACE_5, "            adding edge " + newEdge->getLabel()
                            +" from " + newNode->getName() + " to "
                            + newEdge->getDstNode()->getName() + "\n");
                }
            }

            // In any case the current node is not final any more.
            currNode->setFinal(false);

            //if (hasFinalLiteral && !hasSendingEventThatOccursInAnnotation && nonAnnotatedSendingEvents.empty()) {
            if (hasFinalLiteral) {
            newNode->setFinal(true);
            }

            // insert node into the set of created nodes
            createdNodes.insert(newNode);
        }
    }

    // add deadlocks to the node set of the SA
    if (!options[O_PV_MULTIPLE_DEADLOCKS]) {
        this->addNode(deadlock);
    } else {
        map<string, AnnotatedGraphNode*>::iterator deadlockIter;
        for (deadlockIter = deadlockMap.begin(); deadlockIter
                != deadlockMap.end(); ++deadlockIter) {
            this->addNode(deadlockIter->second);
        }
    }

    // add the newly created nodes to the graph
    TRACE(TRACE_3,"\n        inserting all newly created nodes into the graphs nodeset...\n");
    for (set<AnnotatedGraphNode*>::iterator n = createdNodes.begin(); n
            != createdNodes.end(); ++n) {
        this->addNode((*n));
    }
}


// CODE FROM CG FOR STRUCTURAL REDUCTION
void AnnotatedGraph::reduceStructurally() {
    bool isDiamond(AnnotatedGraphNode * node,
            AnnotatedGraph::predecessorMap& predecessors);

    TRACE(TRACE_5, "AnnotatedGraph::reduceStructurally(): start\n");

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

    TRACE(TRACE_5, "AnnotatedGraph::reduceStructurally(): end\n");
}


bool isDiamond(AnnotatedGraphNode * node,
               AnnotatedGraph::predecessorMap& predecessors) {

    // check for simple diamond
    std::queue< AnnotatedGraphNode*> testNodes;
    std::map< AnnotatedGraphNode*, bool> inQueue;
    std::list< AnnotatedGraphNode*> diamond;
    std::map< AnnotatedGraphNode*, bool> seen;
    std::map< AnnotatedGraphNode*, std::multiset< string> > messages;
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
                string event = edge->getLabel();
                GraphFormulaCNF * preFormula = preNode->getAnnotation()->getDeepCopy();
                GraphFormulaCNF * formula = dnode->getAnnotation()->getDeepCopy();

                if ( !seen[preNode]) {
                    return false;
                }
                cerr << "    | | |    seen before ..." << endl;

                // compare annotations of the nodes
                //                cerr << event << endl;
                //                cerr << preFormula->asString() << endl;
                preFormula->removeLiteral(event);
                //                cerr << preFormula->asString() << endl;
                //                cerr << formula->asString() << endl;
                formula->removeLiteral(event);
                //                cerr << formula->asString() << endl;

                if ( !formula->implies(preFormula)
                        || !preFormula->implies(formula)) {
                    return false;
                }
                cerr << "    | | |    annotation okay ..." << endl;

                if (messages[dnode].empty()) {
                    messages[dnode] = messages[preNode];
                    messages[dnode].insert(edge->getLabelWithoutPrefix());
                } else {
                    std::multiset< string> secset = messages[preNode];
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
void AnnotatedGraph::getPredecessorRelation(AnnotatedGraph::predecessorMap& resultMap) {

    resultMap.clear();

    // iterate over all nodes of the graph
    for (nodes_const_iterator node = setOfNodes.begin(); node != setOfNodes.end(); node++) {

        // iterate over all edges in order to get the node's successors
        AnnotatedGraphNode::LeavingEdges::ConstIterator edge = (*node)->getLeavingEdgesConstIterator();
        while (edge->hasNext() ) {
            AnnotatedGraphEdge * ed = edge->getNext();
            // node is the predeccessor
            resultMap[ed->getDstNode()].add(new AnnotatedGraphEdge(*node, ed->getLabel() ));
        }
    }
}

// [LUHME XV] Methodenbeschreibung erweitern
//! \brief assigns the final nodes of the OG according to Gierds 2007
void AnnotatedGraph::assignFinalNodes() {
    TRACE(TRACE_5, "AnnotatedGraph::assignFinalNodes(): start\n");

    for (nodes_t::iterator node = setOfNodes.begin(); node != setOfNodes.end(); node++) {
        if (((*node)->getAnnotationAsString()).find(GraphFormulaLiteral::FINAL, 0) != string::npos) {
            // current node has literal final in annotation

            TRACE(TRACE_2, "    found literal final in node " + (*node)->getName() + "\n");
            // check whether all outgoing edges are receiving ones
            bool receivesOnly = true;
            SList<AnnotatedGraphEdge*>::ConstIterator edge = (*node)->getLeavingEdgesConstIterator();
            while (edge->hasNext()) {
                AnnotatedGraphEdge* testedge = (*edge).getNext();
                if (testedge->getType() != RECEIVING && testedge->getDstNode()->isBlue()) {
                    receivesOnly = false;
                    break;
                }
            }
            delete edge;

            // node is final iff literal final AND no non-receiving outgoing edge
            if (receivesOnly) {
                TRACE(TRACE_2, "    set final flag for node " + (*node)->getName() + "\n");
                (*node)->setFinal(true);
            }
        }
    }

    TRACE(TRACE_5, "AnnotatedGraph::assignFinalNodes(): start\n");
}


/**** TRANSFERRED FROM COMMUNICATIONGRAPH START ****/

//! \brief Computes the total number of all nodes, edges and states
//!        stored in all nodes in the graph. Also computes the number
//!           of blue nodes and edges in the graph by remembering if a
//!           'blue path' is processed, which means the nodes and edges
//!           processed were reached over a path of blue nodes and edges.
//!           The function also respects the parameter if empty nodes are
//!           to be shown or not.
void AnnotatedGraph::computeNumberOfNodesAndStatesAndEdges() {

    // Reset statistic variables
    std::map<AnnotatedGraphNode*, t_typeOfVisit> visitedNodes;
    nStoredStates = 0;
    nEdges = 0;
    nBlueEdges = 0;
    nBlueNodes = 0;

    if (hasNoRoot()) {
        return;
    }

    // Call the recursive helper
    computeNumberOfNodesAndStatesAndEdgesHelper(root, visitedNodes, (root->getColor() == BLUE));
}


//! \brief Helps computeNumberOfStatesAndEdges to compute the total number of all
//!        states stored in all nodes, the number of nodes and the number of all
//!        edges in this graph. Also finds out which nodes and edges are to be
//!        counted as blue.
//!        This is done recursively (dfs).
//! \param v Current node in the iteration process.
//! \param visitedNodes[] Array of t_typeOfVisit storing the nodes that we have
//!        already looked at; distinguishes between "a visit only" and "counted as blue".
//! \param onABluePath true if we are on a blue path
void AnnotatedGraph::computeNumberOfNodesAndStatesAndEdgesHelper(AnnotatedGraphNode* v,
                                                                 std::map<AnnotatedGraphNode*, t_typeOfVisit>& visitedNodes,
                                                                 bool onABluePath) {

    assert(v != NULL);

    // marking the current node as visited if it has not been visited so far,
    // suppose it is not counted as blue
    if (visitedNodes.find(v) == visitedNodes.end()) {
    	visitedNodes[v] = VISITED_NOT_COUNTED;
    }

    // Determine if we have a blue node and are on a blue path.
    if (onABluePath && (v->getColor() == BLUE) &&
        (parameters[P_SHOW_EMPTY_NODE] || v->reachGraphStateSet.size() != 0)) {

        ++nBlueNodes;

        // remember that the current node has been counted as blue
        visitedNodes[v] = VISITED_COUNTED_AS_BLUE;
    } else {
        onABluePath = false;
    }

    // Add the new states
    nStoredStates += v->reachGraphStateSet.size();

    // iterating over all successors
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* leavingEdge = edgeIter->getNext();

        AnnotatedGraphNode* vNext = (AnnotatedGraphNode *)leavingEdge->getDstNode();
        assert(vNext != NULL);

        // We have found a new edge
        ++nEdges;

        // Current blue path status
        bool onABluePathInLoop = onABluePath;

        // Determine wether we have a blue edge or not.
        if (onABluePath && vNext->getColor() == BLUE &&
                (parameters[P_SHOW_EMPTY_NODE] || vNext->reachGraphStateSet.size() != 0)) {
            ++nBlueEdges;
        } else {
            onABluePathInLoop = false;
        }

        if (vNext != v) {
            // have we visited the next node before?
            // 1) if not, we check the next node
            // 2) if we have seen the next before, we only consider it again if
            // 		a) we are on a blue path
            // 		b) the next node is blue
            // 		c) we have not yet counted the next (blue) node as a blue node
            //		(it is possible to reach a blue node via a red path before, however there
            //		 may be a blue path which also leads to the blue node)
            if ((visitedNodes.find(vNext) == visitedNodes.end()) ||
                (onABluePathInLoop && vNext->getColor() == BLUE &&
                 visitedNodes[vNext] == VISITED_NOT_COUNTED)) {

                // Call recursively with the current blue path status.
                computeNumberOfNodesAndStatesAndEdgesHelper(vNext, visitedNodes, onABluePathInLoop);
            }
        }
    }
    delete edgeIter;
}


//! \brief Computes statistics about this graph. They can be printed by
//!        printGraphStatistics().
void AnnotatedGraph::computeGraphStatistics() {
    TRACE(TRACE_5, "AnnotatedGraph::computeGraphStatistics(): start\n");
    computeNumberOfNodesAndStatesAndEdges();
    TRACE(TRACE_5, "AnnotatedGraph::computeGraphStatistics(): end\n");
}


//! \brief Prints statistics about this graph. May only be called after
//!       computeGraphStatistics().
void AnnotatedGraph::printGraphStatistics() {
    TRACE(TRACE_5, "AnnotatedGraph::printGraphStatistics(): start\n");
    trace( "    number of nodes: " + intToString(getNumberOfNodes()) + "\n");
    trace( "    number of edges: " + intToString(getNumberOfEdges()) + "\n");
    trace( "    number of deleted nodes: " + intToString(numberDeletedVertices) + "\n");
    trace( "    number of blue nodes: " + intToString(getNumberOfBlueNodes()) + "\n");
    trace( "    number of blue edges: " + intToString(getNumberOfBlueEdges()) + "\n");
    trace( "    number of states calculated: " + intToString(State::state_count) + "\n");
    trace( "    number of states stored in datastructure: " + intToString(State::state_count_stored_in_binDec) + "\n");
    trace( "    number of states stored in nodes: " + intToString(getNumberOfStoredStates()) + "\n");
    TRACE(TRACE_5, "AnnotatedGraph::printGraphStatistics(): end\n");
}


//! \brief returns the number of stored states
//!        may only be called after computeGraphStatistics()
//! \return number stored states
unsigned int AnnotatedGraph::getNumberOfStoredStates() const {
    return nStoredStates;
}


//! \brief returns the number of edges
//!        may only be called after computeGraphStatistics()
//! \return number edges
unsigned int AnnotatedGraph::getNumberOfEdges() const {
    return nEdges;
}


//! \brief returns the number of blue nodes
//!        may only be called after computeGraphStatistics()
//! \return number of blue nodes
unsigned int AnnotatedGraph::getNumberOfBlueNodes() const {
    return nBlueNodes;
}


//! \brief returns the number of blue edges
//!        may only be called after computeGraphStatistics()
//! \return number blue edges
unsigned int AnnotatedGraph::getNumberOfBlueEdges() const {
    return nBlueEdges;
}


//! \brief returns the number of nodes
//! \return number of nodes
unsigned int AnnotatedGraph::getNumberOfNodes() const {
    return setOfNodes.size();
}


//! \brief creates a STG file of the graph
//! \param edgeLabels a reference to a vector of strings containing the old
//         label names from this graph
//! \return the filename of the created STG file
string AnnotatedGraph::printGraphToSTG(vector<string>& edgeLabels) {
    TRACE(TRACE_5, "void AnnotatedGraph::printGraphToSTG() : start\n");

    // build STG file name
    string STGFileName;
    if (options[O_OUTFILEPREFIX]) {
        STGFileName = outfilePrefix;
    } else {
        STGFileName = this->filename;
        STGFileName = STGFileName.substr(0, STGFileName.find(".ig.og"));
        STGFileName = STGFileName.substr(0, STGFileName.find(".og.og"));
    }

    if (parameters[P_OG]) {
        if (options[O_CALC_ALL_STATES]) {
            STGFileName += ".og.stg";
        } else {
            STGFileName += ".R.og.stg";
        }
    } else {
        if (options[O_CALC_ALL_STATES]) {
            STGFileName += ".ig.stg";
        } else {
            STGFileName += ".R.ig.stg";
        }
    }

    // create and fill stringstream for buffering graph information
    map<AnnotatedGraphNode*, bool> visitedNodes; // visited nodes
    AnnotatedGraphNode* rootNode = getRoot();    // root node
    ostringstream STGStringStream;               // used as buffer for graph information

    STGStringStream << ".state graph" << "\n";
    printGraphToSTGRecursively(rootNode, STGStringStream, visitedNodes, edgeLabels);
    STGStringStream << ".marking {p" << rootNode->getNumber() << "}" << "\n";
    STGStringStream << ".end";


    // create STG file, print header, transition information and then add buffered graph information
    fstream STGFileStream(STGFileName.c_str(), ios_base::out | ios_base::trunc | ios_base::binary);
    if (!STGFileStream.good()) {
            STGFileStream.close();
            TRACE(TRACE_0, "Error: A file error occured. Exit.");
            exit(EC_FILE_ERROR);
    }
    STGFileStream << ".model Labeled_Transition_System" << "\n";
    STGFileStream << ".dummy";
    for (int i = 0; i < (int)edgeLabels.size(); i++)
    {
        STGFileStream << " t" << i;
    }
    string STGGraphString = STGStringStream.str();
    STGFileStream << "\n" << STGGraphString << endl;
    STGFileStream.close();

    TRACE(TRACE_5, "void AnnotatedGraph::printGraphToSTG() : end\n");

    return STGFileName;
}


//! \brief depth-first-search through the graph printing each node and edge to the output stream
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
void AnnotatedGraph::printGraphToSTGRecursively(AnnotatedGraphNode * v,
                                                ostringstream& os,
                                                std::map<AnnotatedGraphNode*, bool> & visitedNodes,
                                                std::vector<string> & edgeLabels) {

    assert(v != NULL);
    visitedNodes[v] = true;             // mark current node as visited

    if ( !v->isToShow(root, (PN != NULL)) )
        return;

    // TODO: possibly buggy because for every final node is "FINAL" added?
    if (v->isFinal()) {
        // each label is mapped to his position in edgeLabes
        string currentLabel = "FINAL";
        currentLabel += intToString(v->getNumber());
        int foundPosition = (int)edgeLabels.size();
        edgeLabels.push_back( currentLabel );
        // cout << "added FINAL to edgeLabels in printGraphToSTG" << endl;
        os << "p" << v->getNumber() << " t" << foundPosition << " p00" << endl;
    }

    // go through all arcs
    AnnotatedGraphNode::LeavingEdges::ConstIterator edgeIter = v->getLeavingEdgesConstIterator();
    while (edgeIter->hasNext()) {
        AnnotatedGraphEdge* element = edgeIter->getNext();
        AnnotatedGraphNode* vNext = element->getDstNode();

        if ( !vNext->isToShow(root, (PN != NULL)) ) {
            continue; // continue if node is not to show
        }

        // build label vector:
        // each label is mapped to his position in edgeLabes
        string currentLabel = element->getLabel();
        int foundPosition = -1;
        for (int i = 0; i < (int)edgeLabels.size(); i++) {

            if (currentLabel == edgeLabels.at(i) ) {
                foundPosition = i;
                //cout << "found edge befor" << endl;
                break;
            }
        }
        if (foundPosition == -1) {
            //cout << "didn't found edge befor - add to known labels" << endl;
            foundPosition = (int)edgeLabels.size();
            edgeLabels.push_back(currentLabel );
        }
        assert(foundPosition >= 0);
        assert(currentLabel == edgeLabels.at(foundPosition) );

        // print current transition to stream
        os << "p" << v->getNumber() << " t" << foundPosition << " p" << vNext->getNumber() << endl;


        // recursion
        if ( vNext != v && visitedNodes.find(vNext) == visitedNodes.end() )
        //if ((vNext != v) && !visitedNodes[vNext])
        {
            printGraphToSTGRecursively(vNext, os, visitedNodes, edgeLabels);
        }
    }
    delete edgeIter;
}


string AnnotatedGraph::getSuffix() const {

    string suffix = "";

    if (!options[O_CALC_ALL_STATES]) {
        suffix += ".R";
    }
    if (parameters[P_RESPONSIVE]) {
        suffix += ".responsive";
    }
    if (parameters[P_DIAGNOSIS]) {
        suffix += ".diag";
    } else {
        if (parameters[P_OG]) {
            suffix += ".og";
        } else {
            suffix += ".ig";
        }
    }

    return suffix;
}


/**** TRANSFERRED FROM COMMUNICATIONGRAPH END ****/

//! \brief Computes and prints the statistics of this graph (Convenience method).
void AnnotatedGraph::computeAndPrintGraphStatistics() {
    TRACE(TRACE_5, "void AnnotatedGraph::computeAndPrintGraphStatistics() : start\n");
    computeGraphStatistics();
    printGraphStatistics();
    TRACE(TRACE_5, "void AnnotatedGraph::computeAndPrintGraphStatistics() : end\n");
}


//! \brief Transforms this OG (represented by an AnnotatedGraph) into a Public
//!        View. The method works on the AnnotatedGraph object itself. 
//!        To prevent this, create a copy with toAnnotatedGraph() first.
void AnnotatedGraph::transformToPV(bool fromOWFN) {
// first either the empty node (if the owfn is given) or all true nodes
    // need to be removed.
    TRACE(TRACE_1, "    removing true nodes ...\n");
    removeTrueNodes(fromOWFN);
    TRACE(TRACE_2, "\n");

    TRACE(TRACE_1, "    constructing dual service...\n");
    constructDualService();
    TRACE(TRACE_2, "\n");

    TRACE(TRACE_1, "    fixing dual service...\n");
    fixDualService(fromOWFN);
    TRACE(TRACE_2, "\n");

    TRACE(TRACE_1, "\nStatistics of the public view service automaton: \n");
    TRACE(TRACE_1, "  nodes: " + intToString(setOfNodes.size()) + "\n");
    unsigned int edges = 0;
    for (nodes_iterator nodeIter = setOfNodes.begin(); nodeIter
            != setOfNodes.end(); ++nodeIter) {

        edges += (*nodeIter)->getLeavingEdgesCount();
    }
    TRACE(TRACE_1, "  edges: " + intToString(edges) + "\n\n");
}

/**** Copy functions ****/

//! \brief Copies this AnnotatedGraph-Object into another
//!        AnnotatedGraph-Object (destination).
//!        A deep copy is made, all nodes, edges and annotations are copied.
void AnnotatedGraph::toAnnotatedGraph(AnnotatedGraph* destination) {
    
    // Assign simple members
    destination->filename = filename; 


    // map from every node in the annotated graph to its corresponding
    // node in the graph
    map<AnnotatedGraphNode*, AnnotatedGraphNode*> nodeMap;

    // iterate over all nodes
    for (nodes_t::iterator copyNode = setOfNodes.begin(); copyNode
            != setOfNodes.end(); copyNode++) {

        // pointer for the copied node
        AnnotatedGraphNode* copiedNode;

        // gather all relevent informatiion
        string stringVal;
        stringVal.assign((*copyNode)->getName());
        GraphNodeColor colorVal = (*copyNode)->getColor();
        unsigned int numberVal = (*copyNode)->getNumber();
        GraphFormulaCNF* anno = (*copyNode)->getAnnotation()->getDeepCopy();
        // create the new node with the gathered information
        copiedNode = new AnnotatedGraphNode(stringVal, anno, colorVal, numberVal);

        // insert the new node into the nodemap
        nodeMap[(*copyNode)] = copiedNode;

        // add the new node to the graph
        destination->addNode(copiedNode);

        // if this was the root node mark it as root node in the new
        // graph as well
        if (root == (*copyNode)) {
            destination->setRoot(copiedNode);
        }

        // if this node was final in the annotated graph
        // it is final in the graph too
        copiedNode->setFinal((*copyNode)->isFinal());

        TRACE(TRACE_4, "    copied node: " + stringVal + "\n");
    }

    // create all edges after all nodes have been created in the graph
    for (nodes_t::iterator copyNode = setOfNodes.begin(); copyNode
            != setOfNodes.end(); copyNode++) {

        AnnotatedGraphNode::LeavingEdges::Iterator edge_iter = (*copyNode)->getLeavingEdgesIterator();
        while (edge_iter->hasNext()) {
            AnnotatedGraphEdge* edge = edge_iter->getNext();
            nodeMap[(*copyNode)]->addLeavingEdge(new AnnotatedGraphEdge( nodeMap[(edge)->getDstNode()], (edge)->getLabel()));
        }
    }

}

//! \brief Copies this AnnotatedGraph-Object into a Graph object (destination). 
//!        A deep copy is made, all nodes and edges are copied. 
//!        Since a Graph is not annotated, annotations are not copied.
void AnnotatedGraph::toGraph(Graph* destination) {

    // map from every node in the annotated graph to its corresponding
    // node in the graph
    map<AnnotatedGraphNode*, GraphNode*> nodeMap;

    // iterate over all nodes
    for (nodes_t::iterator copyNode = setOfNodes.begin(); copyNode
            != setOfNodes.end(); copyNode++) {

        // pointer for the copied node
        GraphNode* copiedNode;

        // gather all relevent informatiion
        string stringVal;
        stringVal.assign((*copyNode)->getName());
        GraphNodeColor colorVal = (*copyNode)->getColor();
        unsigned int numberVal = (*copyNode)->getNumber();

        // create the new node with the gathered information
        copiedNode = new GraphNode(stringVal, colorVal, numberVal);

        // insert the new node into the nodemap
        nodeMap[(*copyNode)] = copiedNode;

        // add the new node to the graph
        destination->addNode(copiedNode);

        // if this was the root node mark it as root node in the new
        // graph as well
        if (root == (*copyNode)) {
            destination->setRoot(copiedNode);
        }

        // if this node was final in the annotated graph
        // it is final in the graph too
        copiedNode->setFinal((*copyNode)->isFinal());

        TRACE(TRACE_4, "    copied node: " + stringVal + "\n");
    }

    // create all edges after all nodes have been created in the graph
    for (nodes_t::iterator copyNode = setOfNodes.begin(); copyNode
            != setOfNodes.end(); copyNode++) {

        AnnotatedGraphNode::LeavingEdges::Iterator edge_iter = (*copyNode)->getLeavingEdgesIterator();
        while (edge_iter->hasNext()) {
            AnnotatedGraphEdge* edge = edge_iter->getNext();
            nodeMap[(*copyNode)]->addLeavingEdge(new GraphEdge( nodeMap[(edge)->getDstNode()], (edge)->getLabel()));
        }
    }

}

