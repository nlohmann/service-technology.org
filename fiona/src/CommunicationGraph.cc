// *****************************************************************************\
// * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
// *                                                                           *
// * This file is part of Fiona.                                               *
// *                                                                           *
// * Fiona is free software; you can redistribute it and/or modify it          *
// * under the terms of the GNU General Public License as published by the     *
// * Free Software Foundation; either version 2 of the License, or (at your    *
// * option) any later version.                                                *
// *                                                                           *
// * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
// * more details.                                                             *
// *                                                                           *
// * You should have received a copy of the GNU General Public License along   *
// * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
// * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
// *****************************************************************************/

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
#include "vertex.h"
#include "successorNodeList.h"
#include "enums.h"
#include "options.h"
#include "debug.h"
#include "CNF.h"
#include <cassert>


using namespace std;


//! \fn communicationGraph::communicationGraph(oWFN * _PN)
//! \param _PN
//! \brief constructor
communicationGraph::communicationGraph(oWFN * _PN) :
    numberOfNodes(0),
    numberOfEdges(0),
    actualDepth(0),
    numberOfBlueNodes(0),
    numberOfBlackNodes(0),
    numberOfBlueEdges(0),
    numberOfStatesAllNodes(0) {

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


//! \fn unsigned int communicationGraph::getNumberOfVertices() const
//! \return number of vertices
//! \brief returns the number of vertices of the graph
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


//! \fn unsigned int communicationGraph::getNumberBlueOfEdges() const
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
    root = new vertex(PN->placeInputCnt + PN->placeOutputCnt);

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

        char * label = new char[256];
        bool comma = false;
        unsigned int offset = 0;

        strcpy(label, "");
        
        if (type == receiving) {
            offset = PN->placeInputCnt;
        } 

        for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
            if (comma) {
                strcat(label, ", ");
            }
            strcat(label, PN->Places[*iter]->name);
            comma = true;
            
            unsigned int i = 0;
            if (type == receiving) {
	            while (i < PN->placeOutputCnt && PN->outputPlacesArray[i]->index != *iter) {
					i++;	
				}
            } else {
	            while (i < PN->placeInputCnt && PN->inputPlacesArray[i]->index != *iter) {
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
            currentVertex->addSuccessorNode(edgeSucc);

			currentVertex->setAnnotationEdges(edgeSucc);
			
            currentVertex = toAdd;
            numberOfEdges++;

            setOfVertices.insert(toAdd);

            numberOfStatesAllNodes += toAdd->reachGraphStateSet.size();

            delete[] label;

            trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) : end\n");

            return true;
        } else {
            trace(TRACE_1, "\t successor node already known: " + intToString(found->getNumber()) + "\n");

            graphEdge * edgeSucc = new graphEdge(found, label, type);
            currentVertex->addSuccessorNode(edgeSucc);

			currentVertex->setAnnotationEdges(edgeSucc);
			
            numberOfEdges++;

            delete toAdd;
            delete[] label;

            trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) : end\n");

            return false;
        }
    }
}


// for OG
vertex * communicationGraph::AddVertex(vertex * toAdd, unsigned int label, edgeType type) {

	trace(TRACE_5, "reachGraph::AddVertex(vertex * toAdd, unsigned int label, edgeType type): start\n");

	int offset = 0;

    assert(numberOfNodes > 0);
    assert(setOfVertices.size() > 0);
    
    char * edgeLabel;
    if (type == sending) {
        edgeLabel = PN->inputPlacesArray[label]->name;
    } else {
        edgeLabel = PN->outputPlacesArray[label]->name;
	}

	// try to find vertex in set of known vertices
    vertex * found = findVertexInSet(toAdd);

//	if (options[O_BDD] == true || found == NULL) {
    if (found == NULL) {
        trace(TRACE_1, "\n\t new successor node computed:");
        toAdd->setNumber(numberOfNodes++);
        numberOfBlueNodes++;			// all nodes initially blue

        graphEdge * edgeSucc = new graphEdge(toAdd, edgeLabel, type);

		currentVertex->addSuccessorNode(edgeSucc);
		currentVertex->setAnnotationEdges(edgeSucc);
	
		currentVertex->setAnnotationEdges(edgeSucc);
		
        for (int i = 0; i < (PN->placeInputCnt + PN->placeOutputCnt); i++) {
            toAdd->eventsUsed[i] = currentVertex->eventsUsed[i];
        }

        if (type == receiving) {
            offset = PN->placeInputCnt;
        }

        toAdd->eventsUsed[offset + label]++;

        currentVertex = toAdd;
        numberOfEdges++;

        setOfVertices.insert(toAdd);

       	numberOfStatesAllNodes += toAdd->reachGraphStateSet.size();

		trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type): end\n");

        return toAdd;
    } else {
        trace(TRACE_1, "\t computed successor node already known: " + intToString(found->getNumber()) + "\n");

        graphEdge * edgeSucc = new graphEdge(found, edgeLabel, type);
        
        if (currentVertex->addSuccessorNode(edgeSucc)) {
	        numberOfEdges++;
        }
		currentVertex->setAnnotationEdges(edgeSucc);

        if (type == receiving) {
            offset = PN->placeInputCnt;
        }

        delete toAdd;
	
//		found->eventsUsed[offset + label]++;
		
		trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type): end\n");

        return NULL;
    }
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
			if (PN->CurrentMarking[PN->Places[input]->index] == messages_manual) {
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

    StateSet::iterator iter;              // iterator over the stateList's elements

  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

    for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
        (*iter)->decode(PN);
        
		// test for each marking of current node if message bound k reached
		// then supress new sending event
		if (options[O_MESSAGES_MAX] == true) {      // k-message-bounded set
			// iterate over the set of input messages
			for (messageMultiSet::iterator iter = input.begin(); iter != input.end(); iter++) {
				if (PN->CurrentMarking[PN->Places[*iter]->index] == messages_manual) {
					// adding input message to state already using full message bound
					trace(TRACE_3, "\t\t\t\t\t adding input event would cause message bound violation\n");
					trace(TRACE_3, PN->Places[*iter]->name);
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

    for (iter = node->reachGraphStateSet.begin();
         iter != node->reachGraphStateSet.end(); iter++) {

		// get the marking of this state
        (*iter)->decode(PN);
        
        if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
            // if there is a state for which an output event was activated, catch that state
            if (options[O_CALC_ALL_STATES]) {
                PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
            } else {
                PN->calculateReachableStatesOutputEvent(newNode, true, PN->Places[output]);   // calc the reachable states from that marking
            }
        }
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

    StateSet::iterator iter;                      // iterator over the stateList's elements
  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

    for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
        
        (*iter)->decode(PN);
        
        if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
            owfnPlace * outputPlace;
            
			// CHANGE THAT!!!!!!!!!!! is just a hack, stubborn set method does not yet work for more than one output event!
			for (messageMultiSet::iterator iter = output.begin(); iter != output.end(); iter++) {
				outputPlace = PN->Places[*iter];
			}
            
            // if there is a state for which an output event was activated, catch that state
            if (options[O_CALC_ALL_STATES]) {
                PN->calculateReachableStatesFull(newNode);   // calc the reachable states from that marking
            } else {
                PN->calculateReachableStatesOutputEvent(newNode, true, outputPlace);   // calc the reachable states from that marking
            }
        }
    }
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(messageMultiSet output, vertex * node, vertex * newNode) : end\n");
}

//! \fn void communicationGraph::printNodeStatistics()
//! \brief computes number of blue nodes and edges and prints them
void communicationGraph::printNodeStatistics() {
    vertex * tmp = root;
    bool visitedNodes[numberOfNodes];

    for (int i = 0; i < numberOfNodes; i++) {
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


//! \fn void communicationGraph::printDotFile()
//! \brief creates a dot file of the graph
void communicationGraph::printDotFile() {
    
    int maxWritingSize = 1000;
    int maxPrintingSize = 500;
    
    if (numberOfBlueNodes <= maxWritingSize) {
        
		trace(TRACE_0, "creating the dot file of the graph...\n");	
        vertex * tmp = root;

        bool visitedNodes[numberOfNodes];
        for (int i = 0; i < numberOfNodes; i++) {
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
        if (numberOfNodes <= maxPrintingSize) {
			trace(TRACE_0, string(buffer) + "\n");
            system(buffer);
        } else {
        	trace(TRACE_0, "graph is too big to create the graphics; ");
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

	if (v == NULL) {
		return ;
	}

    if (v->getColor() == BLUE) {
        numberOfBlueNodes++;
    } else if (v->getColor() == BLACK) {
        numberOfBlackNodes++;
    }

    if (parameters[P_SHOW_ALL_NODES]
        || ( parameters[P_SHOW_NO_RED_NODES] && (v->getColor() != RED))
		|| (!parameters[P_SHOW_NO_RED_NODES] && (v->getColor() == RED))
        || (v->getColor() == BLUE)
        || (v == root)) {
        
        if (parameters[P_SHOW_EMPTY_NODE] || v->reachGraphStateSet.size() != 0) {

            os << "p" << v->getNumber() << " [label=\"# " << v->getNumber() << "\\n";

            string CNF = "";
            bool mult = false;  // more than one clause -> true

            StateSet::iterator iter;  // iterator over the stateList's elements

			if (parameters[P_SHOW_STATES_PER_NODE]) {
            	for (iter = v->reachGraphStateSet.begin(); iter != v->reachGraphStateSet.end(); iter++) {
                	(*iter)->decodeShowOnly(PN);
//					os << "[" << PN->printCurrentMarkingForDot() << "]" << "(" << (*iter) << ")";
                    os << "[" << PN->printCurrentMarkingForDot() << "]";
                    os << " (";
                    switch ((*iter)->type) {
                        case DEADLOCK: os << "DL" << ")"; break;
                        case FINALSTATE: os << "FS" << ")"; break;
                        default: os << "TR" << ")"; break;
                    }                                                
	                os << "\\n";
	            }
            }

            if (parameters[P_OG]) {
                if (v->getColor() == RED) {
                    CNF += "(false)";
                } else if (v->reachGraphStateSet.size() == 0) {
                    CNF += "(true)";
                } else {
                	CNF += v->getCNF();
                }
            }

			os << CNF;

            os << "\", fontcolor=black, color=";

            switch(v->getColor()) {
                case BLUE: os << "blue"; break;
                case RED: os << "red, style=dashed"; break;
                default: os << "black"; break;
            }
            os << "];\n";

            v->resetIteratingSuccNodes();
            visitedNodes[v->getNumber()] = 1;
            graphEdge * element;
            string label;

            while ((element = v->getNextEdge()) != NULL) {
                vertex * vNext = element->getNode();

                if (parameters[P_SHOW_ALL_NODES]
                    || (parameters[P_SHOW_NO_RED_NODES] && vNext->getColor() != RED)
                    || (!parameters[P_SHOW_NO_RED_NODES] && vNext->getColor() == RED)
                    || (vNext->getColor() == BLUE)) {

                    if (parameters[P_SHOW_EMPTY_NODE] || vNext->reachGraphStateSet.size() != 0) {

                        if (vNext != NULL) {
                            if (element->getType() == receiving) {
                                label = "?";
                            } else {
                                label = "!";
                            }
                            os << "p" << v->getNumber() << "->" << "p" << vNext->getNumber() << " [label=\"" << label << element->getLabel() << "\", fontcolor=black, color=";
                            switch (vNext->getColor()) {
                                case RED: os << "red"; break;
                                case BLUE: os << "blue"; numberOfBlueEdges++; break;
                                default: os << "black"; break;
                            }
                            os << "];\n";
                            if ((vNext != v) && !visitedNodes[vNext->getNumber()]) {
                                printGraphToDot(vNext, os, visitedNodes);
                            }
                        }
                    }
                }
            } // while
        }
    }
}

void communicationGraph::printOGFile() const {
    string ogFilename = netfile;
    if (options[O_CALC_ALL_STATES]) {
        ogFilename += ".a";
    }
    ogFilename += ".og";
    fstream ogFile(ogFilename.c_str(), ios_base::out | ios_base::trunc);

    bool visitedNodes[numberOfNodes];

    ogFile << "NODES" << endl;
    for (int i = 0; i < numberOfNodes; ++i) {
        visitedNodes[i] = false;
    }
    printNodesToOGFile(root, ogFile, visitedNodes);
    ogFile << ';' << endl << endl;

    ogFile << "INITIALNODE" << endl;
    ogFile << "  " << NodeNameForOG(root) << ';' << endl << endl;

    ogFile << "TRANSITIONS" << endl;
    for (int i = 0; i < numberOfNodes; ++i) {
        visitedNodes[i] = false;
    }
    printTransitionsToOGFile(root, ogFile, visitedNodes);
    ogFile << ';' << endl;

    ogFile.close();
}

void communicationGraph::printNodesToOGFile(vertex * v, fstream& os,
    bool visitedNodes[]) const
{
    if (v == NULL)
        return;

    // print node name (separated by comma to previous node if needed)
    if (v != root) {
        os << ',' << endl;
    }
    os << "  " << NodeNameForOG(v);

    // print node annotation
    os << " : " << v->getCNF();

    // mark current node as visited
    visitedNodes[v->getNumber()] = true;

    // recursively process successor nodes that have not been visited yet
    graphEdge* edge;
    v->resetIteratingSuccNodes();
    while ((edge = v->getNextEdge()) != NULL) {
        vertex* vNext = edge->getNode();

        // do not process nodes already visited
        if (visitedNodes[vNext->getNumber()])
            continue;

        // only output blue nodes
        if (vNext->getColor() != BLUE)
            continue;

        printNodesToOGFile(vNext, os, visitedNodes);
    }
}

string communicationGraph::NodeNameForOG(const vertex* v) const
{
    assert(v != NULL);
    return "#" + intToString(v->getNumber());
}

void communicationGraph::printTransitionsToOGFile(vertex * v, fstream& os,
    bool visitedNodes[]) const
{
    if (v == NULL)
        return;

    static bool firstTransitionSeen = false;
    if (v == root) {
        firstTransitionSeen = false;
    }

    // mark current node as visited
    visitedNodes[v->getNumber()] = true;

    // recursively process successor nodes that have not been visited yet
    graphEdge* edge;
    v->resetIteratingSuccNodes();
    while ((edge = v->getNextEdge()) != NULL) {
        vertex* vNext = edge->getNode();

        // only output blue nodes
        if (vNext->getColor() != BLUE)
            continue;

        // output transition (separated by comma to previous transition if
        // needed)
        if (firstTransitionSeen) {
            os << ',' << endl;
        }
        else {
            firstTransitionSeen = true;
        }

        os << "  " << NodeNameForOG(v) << " -> " << NodeNameForOG(vNext);
        string labelPrefix = (edge->getType() == receiving) ? "?" : "!";
        os << " : " << labelPrefix << edge->getLabel();

        // do not process nodes already visited
        if (visitedNodes[vNext->getNumber()])
            continue;

        printTransitionsToOGFile(vNext, os, visitedNodes);
    }
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

        while ((element = v->getNextEdge()) != NULL) {
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
    int i;
   
    s->decode(PN);
    
    for (i = 0; i < PN->getPlaceCnt(); i++) {

        if (PN->Places[i]->type == OUTPUT && PN->CurrentMarking[i] > 0) {
            return true;
        }
    }
    return false;
}


//! \fn analysisResult communicationGraph::analyseNode(vertex * node, bool finalAnalysis)
//! \param node the node to be analysed
//! \param finalAnalysis obsolete???
//! \brief analyses the node and sets its color, if the node gets to be red, then TERMINATE is returned
analysisResult communicationGraph::analyseNode(vertex * node, bool finalAnalysis) {

	trace(TRACE_5, "communicationGraph::analyseNode(vertex * node, bool finalAnalysis) : start\n");

    trace(TRACE_3, "\t\t\t analysing node ");
    trace(TRACE_3, intToString(node->getNumber()) + "...\n");

//    if (node->getColor() != RED) {          // red nodes stay red forever
//        if (node->reachGraphStateSet.size() == 0) {
//            // we analyse an empty node; it becomes blue
//            if (node->getColor() != BLUE) {			// not yet counted
//                numberOfBlueNodes++;
//            }
//            node->setColor(BLUE);
//            trace(TRACE_3, "\t\t\t node analysed blue (empty node)");
//            trace(TRACE_3, "\t ...terminate\n");
//			trace(TRACE_5, "communicationGraph::analyseNode(vertex * node, bool finalAnalysis) : end\n");
//            return TERMINATE;
//        } else {
            // we analyse a non-empty node

			vertexColor colorBefore = node->getColor();		// remember the color of the node before the analysis

			analysisResult result = node->analyseNode(finalAnalysis);
			
			vertexColor colorAfter = node->getColor();		// color of the node now

            if (colorBefore != BLUE && colorAfter == BLUE) {
                numberOfBlueNodes++;
            } else if (colorBefore == BLUE && colorAfter != BLUE) {
                numberOfBlueNodes--;
            }

            if (colorBefore != BLACK && colorAfter == BLACK) {
                numberOfBlackNodes++;
            } else if (colorBefore == BLACK && colorAfter != BLACK) {
                numberOfBlackNodes--;
            }

			trace(TRACE_5, "communicationGraph::analyseNode(vertex * node, bool finalAnalysis) : end\n");
			return result;
//        }
//    }
//    trace(TRACE_5, "communicationGraph::analyseNode(vertex * node, bool finalAnalysis) : end\n");
//    return TERMINATE;
}


//! \fn bool communicationGraph::terminateBuildGraph(vertex * currentNode)
//! \param currentNode the node for which termination is decided
//! \brief decides whether a leaf node of the graph is reached;
//! either because of reaching communication depth or because there are no events left
bool communicationGraph::terminateBuildGraph(vertex * currentNode) {
	
	return false;

//	// check whether there are input or output events left
//	// (i.e. their max_occurences value is not reached)
//	int i;
//	
//	for (i = 0; i < PN->getInputPlaceCnt(); i++) {
//		if (currentNode->eventsUsed[i] < PN->inputPlacesArray[i]->max_occurence) {
//			return false;    // at least one event can be sent
//		}
//	}
//	for (i = 0; i < PN->getOutputPlaceCnt(); i++) {
//		if (currentNode->eventsUsed[i + PN->placeInputCnt] < PN->outputPlacesArray[i]->max_occurence) {
//			return false;    // at least one event can be received
//		}
//	}
//	return true;
}

