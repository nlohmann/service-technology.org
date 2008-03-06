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


//! \brief a basic constructor of AnnotadedGraph
Graph::Graph() :
    root(NULL) {
}


//! \brief a basic destructor of AnnotadedGraph
Graph::~Graph() {
	trace(TRACE_5, "Graph::~Graph() : start\n");
    for (nodes_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end(); ++node_iter) {

        delete *node_iter;
    }
    trace(TRACE_5, "Graph::~Graph() : end\n");
}


//! \brief add an already existing Node to the Graphs node set
//! \param node a pointer to a GraphNode
void Graph::addNode(GraphNode* node) {
    setOfNodes.push_back(node);
}

//! \brief add an already existing Node to the Graphs final node set
//! \param node a pointer to a GraphNode
void Graph::makeNodeFinal(GraphNode* node) {
    finalNodes.push_back(node);
}


//! \brief create a new node and add it to the graphs node set
//! \param nodeName a string containing the name of the new node
//! \param color color of the node
//! \return returns a pointer to the created GraphNode
GraphNode* Graph::addNode(const std::string& nodeName,
                          GraphNodeColor color) {

    GraphNode* node = new GraphNode(nodeName, color, setOfNodes.size());
    addNode(node);
    return node;
}


//! \brief create a new edge in the graph
//! \param srcName a string containing the name of the source node
//! \param dstNodeName a string containing the name of the destination node
//! \param label a string containing the label of the edge
void Graph::addEdge(const std::string& srcName,
                    const std::string& dstNodeName,
                    const std::string& label) {

    GraphNode* src = getNodeWithName(srcName);
    GraphNode* dstNode = getNodeWithName(dstNodeName);

    GraphEdge* transition= new GraphEdge(dstNode,label);
    src->addLeavingEdge(transition);
}


//! \brief checks if the graph has a node with the given name
//! \param nodeName the name to be matched
//! \return returns true if a node with the given name exists, else false
bool Graph::hasNodeWithName(const std::string& nodeName) const {
    return getNodeWithName(nodeName) != NULL;
}


//! \brief returns a pointer to the node that matches a given name, or NULL else
//! \param nodeName the name to be matched
//! \return returns a pointer to the found node or NULL
GraphNode* Graph::getNodeWithName(const std::string& nodeName) const {

    for (nodes_const_iterator node_iter = setOfNodes.begin();
         node_iter != setOfNodes.end(); ++node_iter) {

        if ((*node_iter)->getName() == nodeName) {
            return *node_iter;
        }
    }

    return NULL;
}


//! \brief returns a GraphNode pointer to the root node
//! \return returns a pointer to the root node
GraphNode* Graph::getRoot() const {
    return root;
}


//! \brief sets the root node of the graph to a given node
//! \param newRoot a pointer to the node to become the new root
void Graph::setRoot(GraphNode* newRoot) {
    root = newRoot;
}


//! \brief sets the root node of the graph to one matching the given name
//! \param nodeName a string containing the name of the node to become the new root
void Graph::setRootToNodeWithName(const std::string& nodeName) {
    setRoot(getNodeWithName(nodeName));
}


//! \brief checks wether a root node is set
//! \return returns true if the rootnode is NULL, else false
bool Graph::hasNoRoot() const {
    return getRoot() == NULL;
}


//! \brief removes all edges that have a given node as destination
//! \param nodeToDelete a pointer to the node that will be deleted
void Graph::removeEdgesToNodeFromAllOtherNodes(const GraphNode* nodeToDelete) {

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if (*iNode != nodeToDelete) {
            (*iNode)->removeEdgesToNode(nodeToDelete);
        }
    }
}


//! \brief removes all edges that have a given node as source
//! \param nodeToDelete a pointer to the node that will be deleted
void Graph::removeEdgesFromNodeToAllOtherNodes(GraphNode* nodeToDelete) {

    for (nodes_iterator iNode = setOfNodes.begin(); iNode != setOfNodes.end(); ++iNode) {
        if (*iNode != nodeToDelete) {
            nodeToDelete->removeEdgesToNode(*iNode);
        }
    }
}

//! \brief returns the number of nodes in the Graph
//! \return number of nodes
unsigned int Graph::numberOfNodes() {
	return (setOfNodes.size());
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


//! \brief creates a dot output of the graph and calls dot to create an image from it
//! \param filenamePrefix a string containing the prefix of the output file name
//! \param dotGraphTitle a title for the graph to be shown in the image
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


//! \brief creates a dot output of the graph and calls dot to create an image from it
//! \param filenamePrefix a string containing the prefix of the output file name
void Graph::printDotFile(const std::string& filenamePrefix) const {
    printDotFile(filenamePrefix, filenamePrefix);
}


//! \brief dfs through the graph printing each node and edge to the output stream
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes maps nodes to Bools remembering already visited nodes
void Graph::printGraphToDot(GraphNode* v,
                            fstream& os,
                            std::map<GraphNode*, bool>& visitedNodes) const {

    if (v == NULL) {
        // print the empty OG...
        os << "p0" << " [label=\"#0\", fontcolor=black, color=red, style=dashed];\n";
        return;
    }

    if (visitedNodes[v] != true) {

        os << "p"<< v->getName() << " [label=\"# "<< v->getName() << "\\n";
        os << "\", fontcolor=black, color=blue";

        bool finalNode = false;

        for ( nodes_t::const_iterator checkNode = finalNodes.begin(); checkNode != finalNodes.end(); checkNode++) {
            if ((*checkNode) == v) {
                finalNode = true;
            }
        }

        if (finalNode) {
            os << ", peripheries=2";        
        }
        
        os << "];\n";
        visitedNodes[v] = true;

        std::string currentLabel;

        GraphNode::LeavingEdges::ConstIterator edgeIter =
            v->getLeavingEdgesConstIterator();

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

//! \brief creates a simple oWFN from a service automaton
//! \param filenamePrefix a string containing the prefix of the output file name
//! \param dotGraphTitle a title for the graph to be shown in the image
void Graph::transformToOWFN(PNapi::PetriNet* PN) {

	trace(TRACE_0, "creating the oWFN from the service automaton ...\n");
    
    GraphNode* currentNode = getRoot(); 
    set<GraphNode*> visitedNodes;
    set<string> finalNodeNames;
    unsigned int transitionNumber = 0;
    PNapi::Transition* incomingTransition = NULL;
    
    transformToOWFNRecursively(currentNode,visitedNodes,finalNodeNames,PN, transitionNumber, incomingTransition);
    
    for (set<string>::iterator finalNodesIt = finalNodeNames.begin(); finalNodesIt != finalNodeNames.end(); ++finalNodesIt) {
    	// PN->findPlace(*finalNodesIt)->isFinal = true;    	
    	//PNapi::Marking* finalMarking = PN->addFinalMarking();
    	//PN->findPlace(*finalNodesIt)->addToFinalMarking(finalMarking);
        set< PNapi::Place * > finalMarking;
        finalMarking.insert(PN->findPlace(*finalNodesIt));
        PN->final_set_list.push_back(finalMarking);
    }

    
}

void Graph::transformToOWFNRecursively(GraphNode* currentNode,
		   							   set<GraphNode*>& visitedNodes,
		   							   set<string>& finalNodeNames,
		   							   PNapi::PetriNet* PN,
		   							   unsigned int& transitionNumber,
		   							   PNapi::Transition* incomingTransition) {
    
	
    if(visitedNodes.find(currentNode) != visitedNodes.end()) {
        if (incomingTransition != NULL) {
        	PN->newArc(incomingTransition, PN->findPlace("p" + currentNode->getName()));
        } 
     	return;
    }

    PNapi::Place* place = PN->newPlace("p" + currentNode->getName());
  	visitedNodes.insert(currentNode);
  	
  	if (find(finalNodes.begin(), finalNodes.end(),currentNode) != finalNodes.end())
  		finalNodeNames.insert("p" + currentNode->getName());
  		
  	if (incomingTransition != NULL) {
  		PN->newArc(incomingTransition, place);
    }
    else { // Wenn die incomingTransition null ist, sind wir am ersten Knoten --> Initial Marking
    	place->mark();
    }
  	
    
    GraphNode::LeavingEdges::ConstIterator edgeIter =
        currentNode->getLeavingEdgesConstIterator();

    while (edgeIter->hasNext()) {

    	GraphEdge* edge = edgeIter->getNext();
    	string currentLabel = edge->getLabel();
    	
    	PNapi::Transition* t =PN->newTransition("t" + intToString(transitionNumber));
        ++transitionNumber;
        PN->newArc(place,t);

        // Adding places and arcs for input and output. 
    	if (currentLabel[0] == '?') {
    		PNapi::Place* input; 
    		currentLabel.erase(0,1);
    		input = PN->findPlace(currentLabel);
    		if (input == NULL) {
    			input = PN->newPlace(currentLabel, PNapi::IN);
    		} 
            PN->newArc(input,t);
    	} else if (currentLabel[0] == '!') {
    		PNapi::Place* output; 
    		currentLabel.erase(0,1);
    		output = PN->findPlace(currentLabel);
        	if (output == NULL) {
    			output = PN->newPlace(currentLabel, PNapi::OUT);
    		} 
    		PN->newArc(t,output);
    	}
        transformToOWFNRecursively(edge->getDstNode(), visitedNodes, finalNodeNames, PN, transitionNumber, t);
    }
}



//! \brief A function needed for successful deletion of the graph
void Graph::clearNodeSet() {
    setOfNodes.clear();
}
