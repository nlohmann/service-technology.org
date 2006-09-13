#include "mynew.h"
#include "state.h"
#include "vertex.h"
#include "successorNodeList.h"
#include "enums.h"
#include "options.h"
#include "debug.h"


using namespace std;

//! \fn communicationGraph::communicationGraph(oWFN * _PN)
//! \param _PN
//! \brief constructor
communicationGraph::communicationGraph(oWFN * _PN) :
    numberOfVertices(0),
    numberOfEdges(0),
    actualDepth(0),
    numberBlueNodes(0),
    numberBlackNodes(0),
    numberBlueEdges(0),
    numberOfStatesAllNodes(0) {

    PN = _PN;
}

//! \fn communicationGraph::~communicationGraph()
//! \brief destructor !to be implemented!
communicationGraph::~communicationGraph() {
	vertexSet::iterator iter;
	
	for (iter = setOfVertices.begin(); iter != setOfVertices.end(); iter++) {
		delete *iter;	
	}
	
	delete root;
}

//! \fn vertex * communicationGraph::getRoot() const
//! \return pointer to root
//! \brief returns a pointer to the root node of the graph
vertex * communicationGraph::getRoot() const {
    return root;
}

//! \fn unsigned int communicationGraph::NumberOfEdges () const
//! \return number of edges
//! \brief returns the number of edges of the graph
unsigned int communicationGraph::getNumberOfEdges () const {
    return numberOfEdges;
}

//! \fn unsigned int communicationGraph::NumberOfVertices () const
//! \return number of vertices
//! \brief returns the number of vertices of the graph
unsigned int communicationGraph::getNumberOfVertices () const {
    return numberOfVertices;
}

//! \fn void communicationGraph::calculateRootNode()
//! \brief calculates the root node of the graph
void communicationGraph::calculateRootNode() {
    trace(TRACE_5, "void reachGraph::calculateRootNode(): start\n");

    // initialize graph => calculate root node
    root = new vertex(PN->placeInputCnt + PN->placeOutputCnt);

    // calc the reachable states from that marking
    if (options[O_CALC_ALL_STATES]) {
        PN->calculateReachableStatesFull(root, true);
    } else {
        PN->calculateReachableStatesInputEvent(root, true);
    }

    currentVertex = root;

    numberOfStatesAllNodes += root->reachGraphStateSet.size();
    
	numberOfVertices++;

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


//! \fn int communicationGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type)
//! \param toAdd a reference to the vertex that is to be added to the graph
//! \param label the label of the edge between the current vertex and the one to be added
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
int communicationGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) {
    trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) : start\n");

    if (numberOfVertices == 0) {                // graph contains no nodes at all
        root = toAdd;                           // the given node becomes the root node
        currentVertex = root;
        numberOfVertices++;

        setOfVertices.insert(toAdd);
    } else {
        vertex * found = findVertexInSet(toAdd); //findVertex(toAdd);

        char * label = new char[256];
        bool comma = false;

        strcpy(label, "");

        for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
            if (comma) {
                strcat(label, ", ");
            }
            strcat(label, PN->Places[*iter]->name);
            comma = true;
        }

        if (found == NULL) {

//            cout << "with event " << label << " (type " << type << " ):" << endl;
            trace(TRACE_1, "\n\t new successor node computed:");

            toAdd->setNumber(numberOfVertices++);

            graphEdge * edgeSucc = new graphEdge(toAdd, label, type);
            currentVertex->addSuccessorNode(edgeSucc);

			currentVertex->setAnnotationEdges(edgeSucc);
			
            currentVertex = toAdd;
            numberOfEdges++;

            setOfVertices.insert(toAdd);

            numberOfStatesAllNodes += toAdd->reachGraphStateSet.size();

            delete[] label;

            trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) : end\n");

            return 1;
        } else {
            trace(TRACE_1, "\t successor node already known: " + intToString(found->getNumber()) + "\n");

            graphEdge * edgeSucc = new graphEdge(found, label, type);
            currentVertex->addSuccessorNode(edgeSucc);

			currentVertex->setAnnotationEdges(edgeSucc);

            numberOfEdges++;

            delete toAdd;
            delete[] label;

            trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, messageMultiSet messages, edgeType type) : end\n");

            return 0;
        }
    }
}


// for OG
int communicationGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type) {

	trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type): start\n");

	int offset = 0;

    if (numberOfVertices == 0) {                // graph contains no nodes at all
        root = toAdd;                           // the given node becomes the root node
        currentVertex = root;
        numberOfVertices++;
        setOfVertices.insert(toAdd);
    } else {
        vertex * found = findVertexInSet(toAdd);
        char * edgeLabel;

        if (type == sending) {
            edgeLabel = PN->inputPlacesArray[label]->name;
        } else {
            edgeLabel = PN->outputPlacesArray[label]->name;
        }

//        if (options[O_BDD] == true || found == NULL) {
        if (found == NULL) {

//			if (true) {
//            cout << "with event " << label << " (type " << type << " ):" << endl;
            trace(TRACE_1, "\n\t new successor node computed:");
            toAdd->setNumber(numberOfVertices++);

            graphEdge * edgeSucc = new graphEdge(toAdd, edgeLabel, type);
            currentVertex->addSuccessorNode(edgeSucc);

			currentVertex->setAnnotationEdges(edgeSucc);
			
            for (int i = 0; i < (PN->placeInputCnt + PN->placeOutputCnt); i++) {
                toAdd->eventsUsed[i] = currentVertex->eventsUsed[i];
            }

            if (type == receiving) {
                offset = PN->placeInputCnt;
            }

            toAdd->eventsUsed[offset + label]++;

            // \begin{hack} for online shop example
            if (type == sending) {
                if (strcmp(PN->inputPlacesArray[label]->name, "in.abort") == 0) {
                    for (int i = 0; i < (PN->placeInputCnt + PN->placeOutputCnt); i++) {
                        toAdd->eventsUsed[i]++;
                    }
                }
            }
			// \end{hack}

            currentVertex = toAdd;
            numberOfEdges++;

            setOfVertices.insert(toAdd);

           	numberOfStatesAllNodes += toAdd->reachGraphStateSet.size();
	
			trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type): end\n");

            return 1;
        } else {
            trace(TRACE_5, "\t computed successor node already known: " + intToString(found->getNumber()) + "\n");

            graphEdge * edgeSucc = new graphEdge(found, edgeLabel, type);
            currentVertex->addSuccessorNode(edgeSucc);

			currentVertex->setAnnotationEdges(edgeSucc);

            numberOfEdges++;

            if (type == receiving) {
                offset = PN->placeInputCnt;
            }

            found->eventsUsed[offset + label]++;

            // \begin{hack} for online shop example
            if (type == sending) {
                if (strcmp(PN->inputPlacesArray[label]->name, "in.abort") == 0) {
                    for (int i = 0; i < (PN->placeInputCnt + PN->placeOutputCnt); i++) {
                         found->eventsUsed[i]++;
                    }
                }
            }
            // \end{hack}

            delete toAdd;

			trace(TRACE_5, "reachGraph::AddVertex (vertex * toAdd, unsigned int label, edgeType type): end\n");

            return 0;
        }
    }
}


//! \fn void communicationGraph::calculateSuccStatesInput(unsigned int input, vertex * node)
//! \param input the sending event currently performed
//! \param node the node for which the successor states are to be calculated
//! \return true iff message bound violation occured
//! \brief calculates the set of successor states in case of an input message (sending event)
bool communicationGraph::calculateSuccStatesInput(unsigned int input, vertex * node, vertex * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : start\n");

    bool messageboundviolation = false;
    
    StateSet::iterator iter;              // iterator over the state set's elements
  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

    for (iter = node->reachGraphStateSet.begin();
         iter != node->reachGraphStateSet.end(); iter++) {

		// get the marking of this state
		(*iter)->decode(PN);
		
		// test for each marking of current node if message bound k reached
		// then supress new sending event
		if (options[O_MESSAGES_MAX] == true) {      // k-message-bounded set
			if (PN->CurrentMarking[PN->Places[input]->index] == messages_manual) {
				// adding input message to state already using full message bound
				trace(TRACE_5, "violation found\n");
			    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : end\n");
				return true;
			}
		}
        
        PN->addInputMessage(input);                 // add the input message to the current marking
        
        if (options[O_CALC_ALL_STATES]) {
            PN->calculateReachableStatesFull(newNode, false);   // calc the reachable states from that marking
        } else {
            PN->calculateReachableStatesInputEvent(newNode, false);       // calc the reachable states from that marking
        }
	}
    
	trace(TRACE_5, "no violation found\n");
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : end\n");
    
    return false;
}


//! \fn void communicationGraph::calculateSuccStatesInput(messageMultiSet input, vertex * node)
//! \param input (multi) set of input messages
//! \param node the node for which the successor states are to be calculated
//! \brief calculates the set of successor states in case of an input message
bool communicationGraph::calculateSuccStatesInput(messageMultiSet input, vertex * node, vertex * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(messageMultiSet input, vertex * node) : start\n");

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
					trace(TRACE_0, "\t message bound violation detected (sending event)");
					trace(TRACE_0, PN->Places[*iter]->name);
				    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(unsigned int input, vertex * node) : end\n");
					return true;
				}
			}
		}        
        
        PN->addInputMessage(input);                 // add the input message to the current marking
        if (options[O_CALC_ALL_STATES]) {
            PN->calculateReachableStatesFull(newNode, false);   // calc the reachable states from that marking
        } else {
            PN->calculateReachableStatesInputEvent(newNode, false);       // calc the reachable states from that marking
        }
    }
    
    trace(TRACE_5, "reachGraph::calculateSuccStatesInput(messageMultiSet input, vertex * node) : end\n");
	return false;
}

//! \fn void communicationGraph::calculateSuccStatesOutput(unsigned int output, vertex * node)
//! \param output the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \brief calculates the set of successor states in case of an output message
void communicationGraph::calculateSuccStatesOutput(unsigned int output, vertex * node, vertex * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, vertex * node) : start\n");

    StateSet::iterator iter;                      // iterator over the stateList's elements
  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

    for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {

        (*iter)->decode(PN);
        
        if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
            // if there is a state for which an output event was activated, catch that state
            if (options[O_CALC_ALL_STATES]) {
                PN->calculateReachableStatesFull(newNode, true);   // calc the reachable states from that marking
            } else {
                PN->calculateReachableStatesOutputEvent(newNode, true);   // calc the reachable states from that marking
            }
        }
    }
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(unsigned int output, vertex * node) : end\n");
}

//! \fn void communicationGraph::calculateSuccStatesOutput(unsigned int output, vertex * node)
//! \param output the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \brief calculates the set of successor states in case of an output message
void communicationGraph::calculateSuccStatesOutput(messageMultiSet output, vertex * node, vertex * newNode) {
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(messageMultiSet output, vertex * node) : start\n");

    StateSet::iterator iter;                      // iterator over the stateList's elements
  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

    for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
        
        (*iter)->decode(PN);
        
        if (PN->removeOutputMessage(output)) {      // remove the output message from the current marking
            // if there is a state for which an output event was activated, catch that state
            if (options[O_CALC_ALL_STATES]) {
                PN->calculateReachableStatesFull(newNode, true);   // calc the reachable states from that marking
            } else {
                PN->calculateReachableStatesOutputEvent(newNode, true);   // calc the reachable states from that marking
            }
        }
    }
    trace(TRACE_5, "reachGraph::calculateSuccStatesOutput(messageMultiSet output, vertex * node) : end\n");
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
        numberBlueNodes++;
    } else if (v->getColor() == BLACK) {
        numberBlackNodes++;
    }

    if (parameters[P_SHOW_ALL_NODES]
            || (parameters[P_SHOW_NO_RED_NODES] && (v->getColor() != RED))
            || (parameters[P_SHOW_BLUE_NODES_ONLY] && (v->getColor() == BLUE))
            || (v == root)) {

        if (parameters[P_SHOW_EMPTY_NODE]
            || v->reachGraphStateSet.size() != 0) {

            os << "p" << v->getNumber() << " [label=\"# " << v->getNumber() << "\\n";

            string CNF = "";
            bool mult = false;  // more than one clause -> true

            StateSet::iterator iter;  // iterator over the stateList's elements

            for (iter = v->reachGraphStateSet.begin(); iter != v->reachGraphStateSet.end(); iter++) {

                if (parameters[P_SHOW_STATES_PER_NODE]) {
                	(*iter)->decodeShowOnly(PN);
                    os << "[" << PN->printCurrentMarkingForDot() << "]";
//                    os << "(";
//                    switch ((*iter)->type) {
//                        case DEADLOCK: os << "DL" << ")"; break;
//                        case FINALSTATE: os << "FS" << ")"; break;
//                        default: os << "TR" << ")"; break;
//                    }                                                
                    
//os << "[" << PN->printCurrentMarkingForDot() << "]" << "(" << (*iter) << ")";
                }
//              os << "(";
//                if (v->getColor() != RED) {
//                    switch ((*iter)->type) {
//                        case DEADLOCK:  //if (mult) {
////                                            CNF += " * ";
////                                        }
//                                        if (parameters[P_SHOW_STATES_PER_NODE]) {
//	                                        os << " (DL)";
//                                        }
//                                   //     CNF += "("; CNF += (*iter)->getClause(); CNF += ")"; mult=true;
//                                        break;
//                        case FINALSTATE: //if (mult) {
//                                         //   CNF += " * ";
//                                        //}
//                                        if (parameters[P_SHOW_STATES_PER_NODE]) {
//	                                        os << " (FS)";
//                                        }
//                                       // CNF += "(true)"; mult=true;
//                                        break;
//                        default:
//                                      os << "TR";
////                                      CNF += "true";
//                                        break;
//                    };
//                } //else {
//                  switch ((*iter)->type) {
//                      case DEADLOCK:  os << "DL"; break;
//                      case FINALSTATE: os << "FS"; break;
//                      default: os << "TR"; break;
//                  };
//              }
//              os << ", " << (*iter)->isMinimal();
//              os << ") " << (*iter)->index << "\\n";
				if (parameters[P_SHOW_STATES_PER_NODE]) {
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
//          }      CNF += ">";
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
                        || (parameters[P_SHOW_BLUE_NODES_ONLY] && vNext->getColor() == BLUE)) {

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
                                case BLUE: os << "blue"; numberBlueEdges++; break;
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


//! \fn void communicationGraph::printDotFile()
//! \brief creates a dot file of the graph
void communicationGraph::printDotFile() {
    if (numberOfVertices < 200000) {
        vertex * tmp = root;
        bool visitedNodes[numberOfVertices];

        for (int i = 0; i < numberOfVertices; i++) {
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

        numberBlueNodes = 0;
        numberBlackNodes = 0;

        printGraphToDot(tmp, dotFile, visitedNodes);
        dotFile << "}";
        dotFile.close();
        trace(TRACE_0, "\n    number of blue nodes: " + intToString(numberBlueNodes) + "\n");
        if (numberBlackNodes > 0) {
	        trace(TRACE_0, "\n    number of black nodes: " + intToString(numberBlackNodes) + "\n");
        }
        trace(TRACE_0, "    number of blue edges: " + intToString(numberBlueEdges) + "\n");
        trace(TRACE_0, "    number of states stored in nodes: " + intToString(numberOfStatesAllNodes) + "\n");


        if (numberOfVertices < 900) {
            trace(TRACE_0, "\ncreating the dot file of the graph...\n");
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
            trace(TRACE_0, buffer); trace(TRACE_0, "\n");
            system(buffer);

        } else {
            trace(TRACE_0, "graph is too big to have dot create the graphics\n");
        }
    } else {
        trace(TRACE_0, "graph is too big to create dot file\n");
    }
}


//! \fn bool communicationGraph::stateActivatesOutputEvents(reachGraphState * s)
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
//! \brief analyses the node and sets its color, if the node gets to be red, then TERMINATE is returned
analysisResult communicationGraph::analyseNode(vertex * node, bool finalAnalysis) {

    trace(TRACE_2, "\t\t\t analysing node ");
    trace(TRACE_2, intToString(node->getNumber()) + ": ");

    if (node->getColor() != RED) {          // red nodes stay red forever
        if (node->reachGraphStateSet.size() == 0) {
            // we analyse an empty node; it becomes blue
            if (node->getColor() != BLUE) {
                // not yet counted
                numberBlueNodes++;
            }
            node->setColor(BLUE);
            trace(TRACE_2, "node analysed blue (empty node)");
            trace(TRACE_2, "\t ...terminate\n");
            return TERMINATE;
        } else {
            // we analyse a non-empty node

			vertexColor colorBefore = node->getColor();		// remember the color of the node before the analysis

			analysisResult result = node->analyseNode(finalAnalysis);
			
			vertexColor colorAfter = node->getColor();		// color of the node now

            if (colorBefore != BLUE && colorAfter == BLUE) {
                numberBlueNodes++;
            } else if (colorBefore == BLUE && colorAfter != BLUE) {
                numberBlueNodes--;
            }

            if (colorBefore != BLACK && colorAfter == BLACK) {
                numberBlueNodes++;
            } else if (colorBefore == BLACK && colorAfter != BLACK) {
                numberBlackNodes--;
            }

			return result;
        }
    }
    return TERMINATE;
}
