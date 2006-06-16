#include "OG.h"
//#include "vertex.h"
//#include "owfn.h"
//#include "stateList.h"
//#include "main.h"
#include "graph.h"

#include "options.h"
#include "debug.h"
#include "successorNodeList.h"

//#include <iostream>


//! \fn operatingGuidelines::operatingGuidelines(oWFN * _PN) 
//! \param _PN
//! \brief constructor
operatingGuidelines::operatingGuidelines(oWFN * _PN) : reachGraph(_PN) {
}

//! \fn operatingGuidelines::~operatingGuidelines() 
//! \brief destructor !to be implemented!
operatingGuidelines::~operatingGuidelines() {
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
	trace(TRACE_3, intToString(currentNode->getStateList()->setOfReachGraphStates.size()) + "\n");

	stateList * newNodeStateList;
	
	// get the annotation of the node (CNF)
	getInputEvents(currentNode);				// all input events considered,
	getActivatedOutputEvents(currentNode);		// but only activated outputs
	
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
		
	// iterate over all elements of inputSet
	while (i < PN->placeInputCnt) { // && (currentNode->getColor() != RED)) {
		trace(TRACE_5, "iterating over inputSet\n");


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
			newNodeStateList = calculateSuccStatesInput(PN->inputPlacesArray[i], currentNode);
			trace(TRACE_5, "calculating successor states succeeded\n");
			v->setStateList(newNodeStateList);

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
		
	// iterate over all elements of outputSet
	while (i < PN->placeOutputCnt) { // && (currentNode->getColor() != RED)) {
		trace(TRACE_5, "iterating over outputSet\n");

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
			newNodeStateList = calculateSuccStatesOutput(PN->outputPlacesArray[i], currentNode);
			trace(TRACE_5, "calculating successor states succeeded\n");
			v->setStateList(newNodeStateList);
			
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
//
//	if (currentNode->getColor() == RED) {
//		trace(TRACE_1, "\t\t\t\t so it was deleted\n");
//		setOfVertices.erase(currentNode);		
//		delete currentNode;
//	}
}


//! \fn void operatingGuidelines::getInputEvents(vertex * node)
//! \param node the node for which the activated input events are calculated
//! \brief calculates all activated input events (messages) of the current node for the annotation
void operatingGuidelines::getInputEvents(vertex * node) {
	
	trace(TRACE_5, "operatingGuidelines::getInputEvents(vertex * node): start\n");
	reachGraphStateSet::iterator iter;			// iterator over the stateList's elements
	
	for (int i = 0; i < PN->placeInputCnt; i++) {
		// iterate over all states of the node
		for (iter = node->getStateList()->setOfReachGraphStates.begin();
			 iter != node->getStateList()->setOfReachGraphStates.end(); iter++) {
			if ((*iter)->state->type == DEADLOCK || (*iter)->state->type == FINALSTATE)  {
				// we just consider the maximal states only
				(*iter)->addClauseElement(PN->Places[PN->inputPlacesArray[i]]->name);
			}
		}			
	}
	trace(TRACE_5, "operatingGuidelines::getInputEvents(vertex * node): end\n");
}


//! \fn void operatingGuidelines::getActivatedOutputEvents(vertex * node)
//! \param node the node for which the activated output events are calculated
//! \brief calculates all activated output events (messages) of the current node for the annotation
void  operatingGuidelines::getActivatedOutputEvents(vertex * node) {
	trace(TRACE_5, "operatingGuidelines::getActivatedOutputEvents(vertex * node): start\n");
	int i;
	
	reachGraphStateSet::iterator iter;			// iterator over the stateList's elements
	
	unsigned int * myMarking = new unsigned int [PN->getPlaceCnt()];
	
	// iterate over all states of the node
	for (iter = node->getStateList()->setOfReachGraphStates.begin();
         iter != node->getStateList()->setOfReachGraphStates.end(); iter++) {
		
		// we just consider the maximal states only
		if ((*iter)->state->type == DEADLOCK  || (*iter)->state->type == FINALSTATE)  {
			int i;
			int k = 0;
			//marking = (*iter)->state->myMarking;
			
			(*iter)->state->decode(myMarking, PN);
						
			for (i = 0; i < PN->placeOutputCnt; i++) {
				
				if (myMarking[PN->outputPlacesArray[i]] > 0) {
					(*iter)->addClauseElement(PN->Places[PN->outputPlacesArray[i]]->name);	
				}	
			}
		}
	}
	trace(TRACE_5, "operatingGuidelines::getActivatedOutputEvents(vertex * node): end\n");
}


//! \fn bool operatingGuidelines::terminateBuildingGraph(vertex * node)
//! \param node the vertex to be checked
//! \brief figure out when to terminate the building of the graph
//! return true, if building up shall be terminated, false otherwise 
bool operatingGuidelines::terminateBuildingGraph(vertex * node) {
	trace(TRACE_5, "bool operatingGuidelines::terminateBuildingGraph(vertex * node): start\n");
	
    analyseNode(node, false);

//	if (analyseNode(node, false) == TERMINATE) {
//		trace(TRACE_5, "node analysed\n");
//		trace(TRACE_5, "bool operatingGuidelines::terminateBuildingGraph(vertex * node): end\n");
//		return true;
//	}
//	
//	trace(TRACE_5, "node analysed\n");
	
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
