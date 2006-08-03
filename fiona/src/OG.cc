#include "mynew.h"
#include "OG.h"
#include "graph.h"

#include "options.h"
#include "debug.h"
#include "successorNodeList.h"
#include "BddRepresentation.h" 
#include "CNF.h"


//! \fn operatingGuidelines::operatingGuidelines(oWFN * _PN) 
//! \param _PN
//! \brief constructor
operatingGuidelines::operatingGuidelines(oWFN * _PN) : reachGraph(_PN) {
	  if (options[O_BDD] == true) {
		unsigned int nbrLabels = PN->placeInputCnt + PN->placeOutputCnt;
		bdd = new BddRepresentation(root, nbrLabels, (Cudd_ReorderingType)bdd_reordermethod);
	  }
}

//! \fn operatingGuidelines::~operatingGuidelines() 
//! \brief destructor !to be implemented!
operatingGuidelines::~operatingGuidelines() {
	  if (options[O_BDD] == true) {
		delete bdd;
	  }
}

//! \fn void operatingGuidelines::buildGraph()
//! \brief builds the graph starting with the root node
void operatingGuidelines::buildGraph() {
	buildGraph(root);
//	root->deleteRedSuccessorNodes(this);
}


//! \fn void operatingGuidelines::buildGraph(vertex * currentNode)
//! \param currentNode current node of the graph
//! \brief builds up the graph recursively
void operatingGuidelines::buildGraph(vertex * currentNode) {

	actualDepth++;

	trace(TRACE_1, "\n=================================================================\n");
	trace(TRACE_1, "\t current node: ");
	trace(TRACE_1, intToString(currentNode->getNumber()) + ", \t current depth: " + intToString(actualDepth) + "\n");

	trace(TRACE_3, "\t number of states in node: ");
	trace(TRACE_3, intToString(currentNode->setOfStates.size()) + "\n");

	// get the annotation of the node (CNF)
	if (parameters[P_CALC_ALL_STATES]) {
		computeCNF(currentNode);					// calculate CNF of this node
	}
	
	if (terminateBuildingGraph(currentNode)) {
		string color;
		
		if (currentNode->getColor() == RED) {
			color = "RED";
		} else if (currentNode->getColor() == BLUE) {
			color = "BLUE";
		} else {
			color = "BLACK";
		}

		trace(TRACE_1, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + color + " (leaf)\n");		
	
//		if (currentNode->getColor() == RED) {
//			setOfVertices.erase(currentNode);
//			delete currentNode;
//			trace(TRACE_1, "\t\t\t\t so it was deleted\n");
//		}
			
		return;
	}
	
	int i = 0;
		
	trace(TRACE_5, "iterating over inputSet\n");
	// iterate over all elements of inputSet
	while (i < PN->placeInputCnt) { // && (currentNode->getColor() != RED)) {

		trace(TRACE_3, "\t\t\t\t    sending event: !");
		trace(TRACE_3, string(PN->Places[PN->inputPlacesArray[i]]->name) + "\n");
		
		// hack per command line option until static analysis gives enough information
	    if (options[O_EVENT_USE_MAX] == false) {
	    	// no information means that every event can happen <commDepth> often
		    if (options[O_COMM_DEPTH] == true) {
		    	events_manual = commDepth_manual;
		    } else {
		    	events_manual = PN->commDepth;
		    }
	    }
	    
		if (currentNode->eventsUsed[i] < events_manual){
			
			vertex * v = new vertex(PN->placeInputCnt + PN->placeOutputCnt);	// create new vertex of the graph
			currentVertex = currentNode;
			
			trace(TRACE_5, "calculating successor states\n");
			calculateSuccStatesInput(PN->inputPlacesArray[i], currentNode, v);
			trace(TRACE_5, "calculating successor states succeeded\n");

			if (AddVertex (v, i, sending)) {
				buildGraph(v);				// going down with sending event...

				trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");

			//	currentNode->deleteRedSuccessorNodes(this);
				
//				if (currentNode->getSuccessorNodes() != NULL) {
//					graphEdge * edgeTmp = currentNode->getSuccessorNodes()->getFirstElement();
//				
//					while (edgeTmp) {
//					//	cout << "successor: " << edgeTmp->getNode()->getNumber() << endl;
//						edgeTmp = edgeTmp->getNextElement();
//					}				
//				} else {
//					//cout << "no successors!" << endl;	
//				}
				
				analyseNode(currentNode, false);
				trace(TRACE_5, "node analysed\n");

				actualDepth--; 
			}
		} // end of hack
		i++;
	}

	i = 0;
		
	trace(TRACE_5, "iterating over outputSet\n");
	// iterate over all elements of outputSet
	while (i < PN->placeOutputCnt) { // && (currentNode->getColor() != RED)) {

		trace(TRACE_3, "\t\t\t\t  receiving event: ?");
		trace(TRACE_3, string(PN->Places[PN->outputPlacesArray[i]]->name) + "\n");
		
		// hack per command line option until static analysis gives enough information
	    if (options[O_EVENT_USE_MAX] == false) {
	    	// no information means that every event can happen <commDepth> often
		    if (options[O_COMM_DEPTH] == true) {
		    	events_manual = commDepth_manual;
		    } else {
		    	events_manual = PN->commDepth;
		    }
	    }
	    
		if (currentNode->eventsUsed[i + PN->placeInputCnt] < events_manual) {
				
			vertex * v = new vertex(PN->placeInputCnt + PN->placeOutputCnt);	// create new vertex of the graph
			currentVertex = currentNode;
			
			trace(TRACE_5, "calculating successor states\n");
			calculateSuccStatesOutput(PN->outputPlacesArray[i], currentNode, v);
			trace(TRACE_5, "calculating successor states succeeded\n");
			
			if (AddVertex (v, i, receiving)) {
				buildGraph(v);				// going down with receiving event...

				trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");

		//		currentNode->deleteRedSuccessorNodes(this);
				analyseNode(currentNode, false);
				trace(TRACE_5, "node analysed\n");

				actualDepth--;
			}
		} // end of hack
		i++;	
	}

	trace(TRACE_2, "\t\t\t\t no events left...\n");
	analyseNode(currentNode, true);
	
	string color;
	if (currentNode->getColor() == RED) {
		color = "RED";
	} else if (currentNode->getColor() == BLUE) {
		color = "BLUE";
		
	} else {
		color = "BLACK";
	}

	trace(TRACE_1, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + color + "\n");

	if (options[O_BDD] == true){
		bdd->addOrDeleteLeavingEdges(currentNode);
		
	//	currentNode->resetIteratingSuccNodes();
		
		if (currentNode->setOfStates.size() != 0){
			 
			graphEdge* element;
			//cout << "currentNode: " << currentNode->getNumber() << "\tdelete node: ";
			while((element = currentNode->getNextEdge()) != NULL){
				vertex* vNext = element->getNode();
				//cout << vNext->getNumber()<< "  ";
				setOfVertices.erase(vNext); 
				delete vNext; 
			}
			//cout << endl;
		}
		
	}


//
//	if (currentNode->getColor() == RED) {
//		trace(TRACE_1, "\t\t\t\t so it was deleted\n");
//		setOfVertices.erase(currentNode);		
//		delete currentNode;
//	}
}

//! \fn void operatingGuidelines::computeCNF(vertex * node)
//! \param node the node for which the annotation is calculated
//! \brief calculates the annotation (CNF) for the node
void operatingGuidelines::computeCNF(vertex * node) {
	
	trace(TRACE_5, "operatingGuidelines::computeCNF(vertex * node): start\n");
	StateSet::iterator iter;			// iterator over the states of the node
	
	// iterate over all states of the node
	for (iter = node->setOfStates.begin();
		 iter != node->setOfStates.end(); iter++) {
		if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {
			// we just consider the maximal states only
			
			clause * cl = new clause();
			
			// get the marking of this state
			(*iter)->decodeShowOnly(PN);
						
			for (int i = 0; i < PN->placeOutputCnt; i++) {
				// get the activated output events
				if (PN->CurrentMarking[PN->outputPlacesArray[i]] > 0) {
					cl->addLiteral(PN->Places[PN->outputPlacesArray[i]]->name);	
				}	
			}			
			
			// get all the input events
			for (int i = 0; i < PN->placeInputCnt; i++) {
				cl->addLiteral(PN->Places[PN->inputPlacesArray[i]]->name);
			}
			
			node->addClause(cl, (*iter)->type == FINALSTATE);
		}			
	}
	
	trace(TRACE_5, "operatingGuidelines::computeCNF(vertex * node): end\n");
}

//! \fn bool operatingGuidelines::terminateBuildingGraph(vertex * node)
//! \param node the vertex to be checked
//! \brief figure out when to terminate the building of the graph
//! return true, if building up shall be terminated, false otherwise 
bool operatingGuidelines::terminateBuildingGraph(vertex * node) {
	trace(TRACE_5, "bool operatingGuidelines::terminateBuildingGraph(vertex * node): start\n");
	
//    analyseNode(node, false);

	if (analyseNode(node, false) == TERMINATE) {
		trace(TRACE_5, "node analysed\n");
		trace(TRACE_5, "bool operatingGuidelines::terminateBuildingGraph(vertex * node): end\n");
		return true;
	}
	
	trace(TRACE_5, "node analysed\n");
	
	// do not change termination | here, but set commDepth in main.cc
	//                           v       to desired value
	if (actualDepth > PN->getCommDepth()) {	// we have reached the maximal communication depth
		if (node->getColor() != BLUE) {
			node->setColor(RED);
		}
		trace(TRACE_5, "bool operatingGuidelines::terminateBuildingGraph(vertex * node): end\n");
		return true;
	}
	trace(TRACE_5, "bool operatingGuidelines::terminateBuildingGraph(vertex * node): end (returning false)\n");
	return false;
}

void operatingGuidelines::convertToBdd() {
	
	vertex * tmp = root;
    bool visitedNodes[numberOfVertices];

    for (int i = 0; i < numberOfVertices; i++) {
        visitedNodes[i] = 0;
    }
   
    unsigned int nbrLabels = PN->placeInputCnt + PN->placeOutputCnt;
    BddRepresentation * bdd = new BddRepresentation(root, nbrLabels); 
    bdd->generateRepresentation(tmp, visitedNodes);
    bdd->reorder();
    //bdd->print(); 
    bdd->printDotFile();
    
    delete bdd;
}
