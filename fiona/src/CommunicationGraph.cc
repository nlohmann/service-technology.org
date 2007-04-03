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
#include "options.h"
#include "debug.h"
#include "binDecision.h"
#include "cnf_formula.h"
#include <cassert>

double global_progress = 0;
int show_progress = 0;

using namespace std;


//! \fn communicationGraph::communicationGraph(oWFN * _PN)
//! \param _PN
//! \brief constructor
communicationGraph::communicationGraph(oWFN * _PN) :
    root(NULL),
    currentVertex(NULL),
//    global_progress(0),
//    show_progress(0),
    numberOfNodes(0),
    numberOfEdges(0),
    numberOfBlueNodes(0),
    numberOfBlueEdges(0),
    numberOfBlackNodes(0),
    numberOfStatesAllNodes(0),
    actualDepth(0) {

    PN = _PN;
}


//! \fn communicationGraph::~communicationGraph()
//! \brief destructor
communicationGraph::~communicationGraph() {
	trace(TRACE_5, "communicationGraph::~communicationGraph() : start\n");
	vertexSet::iterator iter;
	
	for (iter = setOfVertices.begin(); iter != setOfVertices.end(); iter++) {
		delete *iter;
	}
	
//	delete root; 		// not necessary for OG   ...and for IG?????
	trace(TRACE_5, "communicationGraph::~communicationGraph() : end\n");
}


//! \fn vertex * communicationGraph::getRoot() const
//! \return pointer to root
//! \brief returns a pointer to the root node of the graph
vertex * communicationGraph::getRoot() const {
    return root;
}


//! \fn unsigned int communicationGraph::getNumberOfNodes() const
//! \return number of nodes
//! \brief returns the number of nodes of the graph (IG or OG)
unsigned int communicationGraph::getNumberOfNodes() const {
    return numberOfNodes;
}


//! \fn unsigned int communicationGraph::getNumberOfEdges() const
//! \return number of edges
//! \brief returns the number of edges of the graph
unsigned int communicationGraph::getNumberOfEdges() const {
    return numberOfEdges;
}


//! \fn unsigned int communicationGraph::getNumberOfBlueNodes() 
//! \return number of blue nodes
//! \brief returns the number of blue nodes of the graph
unsigned int communicationGraph::getNumberOfBlueNodes() {
    return numberOfBlueNodes;
}


//! \fn unsigned int communicationGraph::getNumberOfBlueEdges() const
//! \return number of blue edges
//! \brief returns the number of blue edges of the graph
unsigned int communicationGraph::getNumberOfBlueEdges() const {
    return numberOfBlueEdges;
}


//! \fn unsigned int communicationGraph::getNumberOfBlackNodes() const
//! \return number of black nodes
//! \brief returns the number of black nodes of the graph
unsigned int communicationGraph::getNumberOfBlackNodes() const {
    return numberOfBlackNodes;
}


//! \fn unsigned int communicationGraph::getNumberOfStatesAllNodes() const
//! \return number of states stored in all nodes
unsigned int communicationGraph::getNumberOfStatesAllNodes() const {
    return numberOfStatesAllNodes;
}


//! \fn void communicationGraph::calculateRootNode()
//! \brief calculates the root node of the graph
// for OG only
void communicationGraph::calculateRootNode() {
    trace(TRACE_5, "void reachGraph::calculateRootNode(): start\n");

    // create new OG root node
    root = new vertex(PN->getInputPlaceCount() + PN->getOutputPlaceCount());

    // calc the reachable states from that marking
    if (options[O_CALC_ALL_STATES]) {
        PN->calculateReachableStatesFull(root);
    } else {
        PN->calculateReachableStatesInputEvent(root, true);
    }

    currentVertex = root;

    numberOfStatesAllNodes += root->reachGraphStateSet.size();
    
	numberOfNodes++;
	numberOfBlueNodes++;
	
	setOfVertices.insert(root);

    trace(TRACE_5, "void reachGraph::calculateRootNode(): end\n");
}


//! \fn vertex * communicationGraph::findVertexInSet(vertex * toAdd)
//! \param toAdd the vertex we are looking for in the graph
//! \brief this function uses the find method from the template set
vertex * communicationGraph::findVertexInSet(vertex * toAdd) {
    vertexSet::iterator iter = setOfVertices.find(toAdd);
    if (iter != setOfVertices.end()) {
        return *iter;
    } else {
        return NULL;
    }
}


//! \fn bool communicationGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type)
//! \param toAdd a reference to the vertex that is to be added to the graph
//! \param messages the label of the edge between the current vertex and the one to be added
//! \param type the type of the edge (sending, receiving)
//! \brief adding a new vertex/ edge to the graph
//!
//! if the graph already contains nodes, we first search the graph for a node that matches the new node
//! if we did not find a node, we add the new node to the graph (here we add the new node to the
//! successor node list of the current graph and add the current node to the list of predecessor nodes
//! of the new node; after that the current vertex becomes to be the new node)
//!
//! if we actually found a node matching the new one, we just create an edge between the current node
//! and the node we have just found, the found one gets the current node as a predecessor node

// for IG
bool communicationGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) {
    trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) : start\n");

    if (numberOfNodes == 0) {                // graph contains no nodes at all
        root = toAdd;                           // the given node becomes the root node
        currentVertex = root;
        numberOfNodes++;

        setOfVertices.insert(toAdd);
    } else {
        vertex * found = findVertexInSet(toAdd); //findVertex(toAdd);

        string label;
        bool comma = false;
        unsigned int offset = 0;
        
        if (type == receiving) {
            offset = PN->getInputPlaceCount();
        } 

		if (found == NULL) {
			// copy the events used from the parent node
			for (unsigned int i = 0; i < (PN->getInputPlaceCount() + PN->getOutputPlaceCount()); i++) {
            	toAdd->eventsUsed[i] = currentVertex->eventsUsed[i];
        	}
		}
			
        for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
            if (comma) {
              label += ", ";
            }
           	label += string(PN->getPlace(*iter)->name);
            comma = true;
            
            unsigned int i = 0;
            if (type == receiving) {
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

            toAdd->setNumber(numberOfNodes++);

            graphEdge * edgeSucc = new graphEdge(toAdd, label, type);
            numberOfEdges++;

            currentVertex->addSuccessorNode(edgeSucc);
			currentVertex->setAnnotationEdges(edgeSucc);

            currentVertex = toAdd;

			if (currentVertex->getColor() != RED) {
		        graphEdge * edgePred = new graphEdge(currentVertex, label, type);
				toAdd->addPredecessorNode(edgePred);
			}
			
            setOfVertices.insert(toAdd);

            numberOfStatesAllNodes += toAdd->reachGraphStateSet.size();

            trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) : end\n");

            return true;
        } else {
            trace(TRACE_1, "\t successor node already known: " + intToString(found->getNumber()) + "\n");

            graphEdge * edgeSucc = new graphEdge(found, label, type);
            numberOfEdges++;

            currentVertex->addSuccessorNode(edgeSucc);
			currentVertex->setAnnotationEdges(edgeSucc);

            if (currentVertex->getColor() != RED) {
	    	    graphEdge * edgePred = new graphEdge(currentVertex, label, type);
				found->addPredecessorNode(edgePred);            
            }
            
            delete toAdd;

            trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) : end\n");

            return false;
        }
    }
    
    return false;
}


// for OG
//vertex * communicationGraph::AddVertex(vertex * toAdd, unsigned int label, edgeType type, bool isnew) {
void communicationGraph::AddVertex(vertex * toAdd, unsigned int label, edgeType type, bool isnew) {

	trace(TRACE_5, "reachGraph::AddVertex(vertex * toAdd, unsigned int label, edgeType type): start\n");
	// If the last argument is true, then the node toAdd represents a fresh node
	// that has to be added.
	
	// If the last argument is false, then the node toAdd represents the node that
	// was computed before and only the edge to that node has to be added (and not the node itself).
	
	int offset = 0;

    assert(numberOfNodes > 0);
    assert(setOfVertices.size() > 0);
    
    string edgeLabel;
    if (type == sending) {
        edgeLabel = PN->getInputPlace(label)->name;
    } else {
        edgeLabel = PN->getOutputPlace(label)->name;
	}

//	if (options[O_BDD] == true || isnew) {
    if (isnew) {
        trace(TRACE_1, "\n\t new successor node computed:");
        toAdd->setNumber(numberOfNodes++);
        numberOfBlueNodes++;			// all nodes initially blue

		// draw a new edge to the new node
        graphEdge * edgeSucc = new graphEdge(toAdd, edgeLabel, type);
        numberOfEdges++;

		currentVertex->addSuccessorNode(edgeSucc);
		currentVertex->setAnnotationEdges(edgeSucc);

        for (unsigned int i = 0; i < (PN->getInputPlaceCount() + PN->getOutputPlaceCount()); i++) {
            toAdd->eventsUsed[i] = currentVertex->eventsUsed[i];
        }

        if (type == receiving) {
            offset = PN->getInputPlaceCount();
        }

        toAdd->eventsUsed[offset + label]++;
        
        if (currentVertex->getColor() != RED) {
	        graphEdge * edgePred = new graphEdge(currentVertex, edgeLabel, type);
			toAdd->addPredecessorNode(edgePred);
        }
        
        currentVertex = toAdd;

        setOfVertices.insert(toAdd);

       	numberOfStatesAllNodes += toAdd->reachGraphStateSet.size();

		trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type): end\n");

    } else {
		trace(TRACE_1, "\t computed successor node already known: " + intToString(toAdd->getNumber()) + "\n");

		// draw a new edge to the old node
		graphEdge * edgeSucc = new graphEdge(toAdd, edgeLabel, type);
		numberOfEdges++;

		currentVertex->addSuccessorNode(edgeSucc);
		currentVertex->setAnnotationEdges(edgeSucc);

		if (type == receiving) {
			offset = PN->getInputPlaceCount();
		}

		if (currentVertex->getColor() != RED) {
	        graphEdge * edgePred = new graphEdge(currentVertex, edgeLabel, type);
			toAdd->addPredecessorNode(edgePred);
		}

		trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type): end\n");
    }
}


//! \fn void communicationGraph::analyseNode(vertex* node)
//! \param node the node to be analysed
//! \brief analyses the node and sets its color
void communicationGraph::analyseNode(vertex* node) {

	trace(TRACE_5, "communicationGraph::analyseNode(vertex* node) : start\n");

	trace(TRACE_3, "\t\t\t analysing node ");
	trace(TRACE_3, intToString(node->getNumber()) + "...\n");

	assert(node->getColor() == BLUE);

	vertexColor analysedColor;

	if (false) {
//	if (parameters[P_OG]) {
		// analyse node by its formula
		analysedColor = node->analyseNodeByFormula();
	} else {
		// analyse node by its CNF
		analysedColor = node->analyseNode();
	}

	node->setColor(analysedColor);

	if (node->getColor() == RED) {
		numberOfBlueNodes--;
	}

	trace(TRACE_5, "communicationGraph::analyseNode(vertex* node) : end\n");
}


//! \fn void communicationGraph::calculateSuccStatesInput(unsigned int input, vertex * oldNode, vertex * newNode)
//! \param input the sending event currently performed
//! \param oldNode the old node carrying the states
//! \param newNode the new node wich is computed
//! \brief for each state of the old node:
//! add input message and build reachability graph and add all states to new node
// for OG only
void communicationGraph::calculateSuccStatesInput(unsigned int input, vertex * oldNode, vertex * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : start\n");

    StateSet::iterator iter;              // iterator over the state set's elements
  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

    for (iter = oldNode->reachGraphStateSet.begin();
         iter != oldNode->reachGraphStateSet.end(); iter++) {

		// get the marking of this state
		(*iter)->decode(PN);
		
		// test for each marking of current node if message bound k reached
		// then supress new sending event
		if (options[O_MESSAGES_MAX] == true) {      // k-message-bounded set
			if (PN->CurrentMarking[PN->getPlace(input)->index] == messages_manual) {
				// adding input message to state already using full message bound
				trace(TRACE_3, "\t\t\t\t\t adding input event would cause message bound violation\n");
			    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : end\n");
				newNode->setColor(RED);
				return;
			}
		}

        // asserted: adding input message does not violate message bound
        PN->addInputMessage(input);			// add the input message to the current marking

        if (options[O_CALC_ALL_STATES]) {
            PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
        } else {
            PN->calculateReachableStatesInputEvent(newNode, false);       // calc the reachable states from that marking
        }
        
        if (newNode->getColor() == RED) {
        	// a message bound violation occured during computation of reachability graph
		    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : end\n");
        	return;
        }
	}
    
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : end\n");
    return;
}


//! \fn void communicationGraph::calculateSuccStatesInput(messageMultiSet input, vertex * node, vertex * newNode)
//! \param input (multi) set of input messages
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an input message
// for IG
void communicationGraph::calculateSuccStatesInput(messageMultiSet input, vertex * node, vertex * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(messageMultiSet input, vertex * node, vertex * newNode) : start\n");

  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

	if (TRACE_2 <= debug_level) {
		for (messageMultiSet::iterator iter1 = input.begin(); iter1 != input.end(); iter1++) {
			trace(TRACE_2, PN->getPlace(*iter1)->name);
			trace(TRACE_2, " ");			
		}
		trace(TRACE_2, "\n");
	}

    for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
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
				    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : end\n");
					return;
				}
			}
		}        
        
        PN->addInputMessage(input);                 // add the input message to the current marking
        if (options[O_CALC_ALL_STATES]) {
            PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
        } else {
            PN->calculateReachableStatesInputEvent(newNode, false);       // calc the reachable states from that marking
        }
        if (newNode->getColor() == RED) {
        	// a message bound violation occured during computation of reachability graph
		    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : end\n");
        	return;
        }        
    }
    
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(messageMultiSet input, vertex * node, vertex * newNode) : end\n");
	return;
}

//! \fn void communicationGraph::calculateSuccStatesOutput(unsigned int output, vertex * node, vertex * newNode)
//! \param output the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an output message
// for OG
void communicationGraph::calculateSuccStatesOutput(unsigned int output, vertex * node, vertex * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, vertex * node, vertex * newNode) : start\n");

	StateSet::iterator iter;                      // iterator over the stateList's elements
  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

    if (options[O_CALC_ALL_STATES]) {
		for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
	    	(*iter)->decode(PN);
	    	if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
	       			PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
	    	}
		}
    } else {
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
	//	binDeleteAll(PN->tempBinDecision);
		delete PN->tempBinDecision;
		PN->tempBinDecision = NULL;
    }
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, vertex * node, vertex * newNode) : end\n");
}

//! \fn void communicationGraph::calculateSuccStatesOutput(messageMultiSet output, vertex * node, vertex * newNode)
//! \param output the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an output message
// for IG
void communicationGraph::calculateSuccStatesOutput(messageMultiSet output, vertex * node, vertex * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(messageMultiSet output, vertex * node, vertex * newNode) : start\n");

  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();
        
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
		StateSet stateSet;
//		stateSet.clear();

		for (StateSet::iterator iter = node->reachGraphStateSet.begin(); 
				iter != node->reachGraphStateSet.end(); iter++) {
        		
			(*iter)->decode(PN);
			// calculate temporary state set with the help of stubborn set method
			PN->calculateReachableStates(stateSet, output, newNode);	
		}

		for (StateSet::iterator iter2 = stateSet.begin(); iter2 != stateSet.end(); iter2++) {		
			(*iter2)->decode(PN); // get the marking of the state
			
			if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
				PN->calculateReachableStatesOutputEvent(newNode);   // calc the reachable states from that marking
			}
		}
	//	binDeleteAll(PN->tempBinDecision);
		delete PN->tempBinDecision;
		PN->tempBinDecision = NULL;
    }

    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(messageMultiSet output, vertex * node, vertex * newNode) : end\n");
}

//! \fn void communicationGraph::printNodeStatistics()
//! \brief computes number of blue nodes and edges and prints them
void communicationGraph::printNodeStatistics() {
    vertex * tmp = root;
    bool visitedNodes[numberOfNodes];

    for (unsigned int i = 0; i < numberOfNodes; i++) {
        visitedNodes[i] = false;
    }
    
    numberOfBlueNodes = 0;
    numberOfBlueEdges = 0;
    
    // calculate the number of blue nodes and edges first
    computeNumberOfBlueNodesEdges(tmp, visitedNodes);	
	
    trace(TRACE_0, "\n    number of blue nodes: " + intToString(getNumberOfBlueNodes()) + "\n");
    if (getNumberOfBlackNodes() > 0) {
        trace(TRACE_0, "\n    number of black nodes: " + intToString(getNumberOfBlackNodes()) + "\n");
    }
    trace(TRACE_0, "    number of blue edges: " + intToString(getNumberOfBlueEdges()) + "\n");
    trace(TRACE_0, "    number of states stored in nodes: " + intToString(getNumberOfStatesAllNodes()) + "\n\n");
}


//! \fn void communicationGraph::computeNumberOfBlueNodesEdges(vertex * v, bool visitedNodes[])
//! \param v current node in the iteration process
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \brief breadthsearch through the graph computing the number of blue nodes
void communicationGraph::computeNumberOfBlueNodesEdges(vertex * v, bool visitedNodes[]) {

	if (v == NULL) {
		return ;
	}

    if (v->getColor() == BLUE && 
    		(parameters[P_SHOW_EMPTY_NODE] || v->reachGraphStateSet.size() != 0)) {
		
		numberOfBlueNodes++;
		
        v->resetIteratingSuccNodes();
        visitedNodes[v->getNumber()] = true;
        graphEdge * element;

        while ((element = v->getNextSuccEdge()) != NULL) {
            vertex * vNext = element->getNode();
            if (vNext->getColor() == BLUE && 
    			(parameters[P_SHOW_EMPTY_NODE] || vNext->reachGraphStateSet.size() != 0)) {

    			numberOfBlueEdges++;		
    		}
            if ((vNext != v) && !visitedNodes[vNext->getNumber()]) {
                computeNumberOfBlueNodesEdges(vNext, visitedNodes);
            }
        } // while
    }
}

//! \fn bool communicationGraph::stateActivatesOutputEvents(State * s)
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


//! \fn void communicationGraph::addProgress(double toAddValue)
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


//! \fn void communicationGraph::printProgress()
//! \brief prints the current global progress value depending whether the value
//! changed significantly and depending on the debug-level set
void communicationGraph::printProgress() {
		
	return;
	
	int progress_step_size = 5;
	int current_progress = int(100 * global_progress);

    if (current_progress >= (show_progress + progress_step_size)) {
    	// significant progress change
		if (debug_level == TRACE_0) {
			trace(TRACE_0, " " + intToString(current_progress) + " ");
		} else {
			trace(TRACE_0, "\t progress: " + intToString(current_progress) + " %\n");
    	}
		// assigning next progress value to show
		show_progress = current_progress;
    }
}


//! \fn void communicationGraph::printProgress()
//! \brief prints the current global progress value depending whether the value
//! changed significantly and depending on the debug-level set
void communicationGraph::printProgressFirst() {
		
	return;
	
	if (debug_level == TRACE_0) {
		trace(TRACE_0, "\t progress (in %): 0 ");
	} else {
		trace(TRACE_0, "\t progress: 0 %\n");
   	}
}


//! \fn void communicationGraph::printDotFile()
//! \brief creates a dot file of the graph
void communicationGraph::printDotFile() {
    
    // unsigned int maxWritingSize = 1000;
    unsigned int maxPrintingSize = 500;
    
    if (true) { // numberOfBlueNodes <= maxWritingSize) {
        
		trace(TRACE_0, "creating the dot file of the graph...\n");	
        vertex * tmp = root;

        bool visitedNodes[numberOfNodes];
        for (unsigned int i = 0; i < numberOfNodes; i++) {
            visitedNodes[i] = 0;
        }

        char buffer[256];
        if (parameters[P_OG]) {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(buffer, "%s.a.OG.out", netfile);
            } else {
                sprintf(buffer, "%s.OG.out", netfile);
            }
        } else {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(buffer, "%s.a.IG.out", netfile);
            } else {
                sprintf(buffer, "%s.IG.out", netfile);
            }
        }

        fstream dotFile(buffer, ios_base::out | ios_base::trunc);
        dotFile << "digraph g1 {\n";
        dotFile << "graph [fontname=\"Helvetica\", label=\"";
        parameters[P_OG] ? dotFile << "OG of " : dotFile << "IG of ";
        dotFile << netfile;
        dotFile << "\"];\n";
        dotFile << "node [fontname=\"Helvetica\" fontsize=10];\n";
        dotFile << "edge [fontname=\"Helvetica\" fontsize=10];\n";

		numberOfBlueNodes = 0;
		numberOfBlackNodes = 0;

        printGraphToDot(tmp, dotFile, visitedNodes);
        
        dotFile << "}";
        dotFile.close();
        	
        // prepare dot command line for printing
        if (parameters[P_OG]) {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(buffer, "dot -Tpng %s.a.OG.out -o %s.a.OG.png", netfile, netfile);
            } else {
                sprintf(buffer, "dot -Tpng %s.OG.out -o %s.OG.png", netfile, netfile);
            }
        } else {
            if (options[O_CALC_ALL_STATES]) {
                sprintf(buffer, "dot -Tpng %s.a.IG.out -o %s.a.IG.png", netfile, netfile);
            } else {
                sprintf(buffer, "dot -Tpng %s.IG.out -o %s.IG.png", netfile, netfile);
            }
        }

		// print commandline and execute system command
        if ((options[O_SHOW_NODES] && numberOfNodes <= maxPrintingSize) || 
        		(!options[O_SHOW_NODES] && numberOfBlueNodes <= maxPrintingSize)) {
        	// print only, if number of nodes is lower than required 
        	// if option is set to show all nodes, then we compare the number of all nodes
        	// otherwise, we compare the number of blue nodes only
			trace(TRACE_0, string(buffer) + "\n");
            system(buffer);
        } else {
        	trace(TRACE_0, "graph is too big to create the graphics;\n");
			trace(TRACE_0, string(buffer) + "\n");
        }
    } else {
        trace(TRACE_0, "graph is too big to create dot file\n");
    }
}


//! \fn void communicationGraph::printGraphToDot(vertex * v, fstream& os, bool visitedNodes[])
//! \param v current node in the iteration process
//! \param os output stream
//! \param visitedNodes[] array of bool storing the nodes that we have looked at so far
//! \brief breadthsearch through the graph printing each node and edge to the output stream
void communicationGraph::printGraphToDot(vertex * v, fstream& os, bool visitedNodes[]) {

	assert(v != NULL);

    if (v->getColor() == BLUE) {
        numberOfBlueNodes++;
    }

    if (!v->isToShow(root))
        return;

    os << "p" << v->getNumber() << " [label=\"# " << v->getNumber() << "\\n";

    StateSet::iterator iter;  // iterator over the stateList's elements

    if (parameters[P_SHOW_STATES_PER_NODE]) {
        for (iter = v->reachGraphStateSet.begin(); iter != v->reachGraphStateSet.end(); iter++) {
//                	(*iter)->decodeShowOnly(PN);
            
            (*iter)->decode(PN);	// need to decide if it is an external or internal deadlock
            os << "[" << PN->getCurrentMarkingAsString() << "]";
            os << " (";
            
            string kindOfDeadlock;
            unsigned int i;
            
            switch ((*iter)->type) {
                case DEADLOCK: 	
                                kindOfDeadlock = "i"; // letter for 'i' internal or 'e' external deadlock
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
                                os << kindOfDeadlock << "DL" << ")"; 
                                break;
                                
                case FINALSTATE: os << "FS" << ")"; break;
                
                default: os << "TR" << ")"; break;
            }
            os << "\\n";
        }
    }

    if (parameters[P_OG]) {
//				// add annotation to node
        string CNFString = v->getCNF_formula()->asString();

//				/*
//				 * The following three calls were added by Niels to simplify the string
//				 * representation of CNF formulas in the DOT output of an OG. All
//				 * needed code is implemented in files "cnf_formula.h" and
//				 * "cnf_formula.cc". If the following three calls are removed,
//				 * everything is (as ugly) as before.
//				 */
//
//				// 1. create a CNF_Formula object from the CNF string representation
//				CNF_Formula formula = CNF_Formula(CNFString);
//			
//				// 2. simplify the CNF formula inside the CNF_Formula object
//				formula.simplify();
//			
//				// 3. create a string representation of the simplified formula
//				CNFString = formula.to_string();	
    
        os << CNFString;
    }

    os << "\", fontcolor=black, color=" << v->getColor().toString();
    if (v->getColor() == RED) {
        os << ", style=dashed";
    }
    os << "];\n";

    v->resetIteratingSuccNodes();
    visitedNodes[v->getNumber()] = 1;
    graphEdge * element;
    string label;

    while ((element = v->getNextSuccEdge()) != NULL) {
        vertex * vNext = element->getNode();

        if (!vNext->isToShow(root))
            continue;

        if (element->getType() == receiving) {
            label = "?";
        } else {
            label = "!";
        }
        os << "p" << v->getNumber() << "->" << "p" << vNext->getNumber()
           << " [label=\"" << label << element->getLabel()
           << "\", fontcolor=black, color=" << vNext->getColor().toString();

        if (vNext->getColor() == BLUE) {
            numberOfBlueEdges++;
        }
        os << "];\n";
        if ((vNext != v) && !visitedNodes[vNext->getNumber()]) {
            printGraphToDot(vNext, os, visitedNodes);
        }
    } // while
}

