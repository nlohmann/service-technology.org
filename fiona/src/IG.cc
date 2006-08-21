#include "mynew.h"
#include "IG.h"
//#include "vertex.h"
//#include "owfn.h"
//#include "stateList.h"
//#include "main.h"

#include "state.h"
#include "options.h"
#include "debug.h"
#include "CNF.h"
#include "owfn.h"

//#include <iostream>
#include <vector>			// for combining receiving events
//#include <string>

//! \fn interactionGraph::interactionGraph(oWFN * _PN) 
//! \param _PN
//! \brief constructor
interactionGraph::interactionGraph(oWFN * _PN) : reachGraph(_PN) {
}

//! \fn interactionGraph::~interactionGraph() 
//! \brief destructor !to be implemented!
interactionGraph::~interactionGraph() {
	
}

//! \fn void interactionGraph::buildGraph()
//! \brief builds the graph starting with the root node
void interactionGraph::buildGraph() {	
	if (parameters[P_CALC_REDUCED_IG]) {
		buildReducedGraph(root);
	} else {
		buildGraph(root);
	}
}

//! \fn void interactionGraph::buildGraph(vertex * node)
//! \param node current node of the graph
//! \brief builds up the graph recursively
void interactionGraph::buildGraph(vertex * currentNode) {

	unsigned int elementInput = 0;
	unsigned int elementOutput = 0;
	
	setOfMessages inputSet;
	setOfMessages outputSet;
	
//	if (parameters[P_CALC_ALL_STATES]) {
		// get the activated events and compute the CNF of the node
		getActivatedEventsComputeCNF(currentNode, inputSet, outputSet);
//	} else {
//		inputSet = PN->inputMessages;
//		outputSet = PN->outputMessages;	
//	}

	actualDepth++;

	trace(TRACE_1, "\n=================================================================\n");
	trace(TRACE_1, "\t current node: ");
	trace(TRACE_1, intToString(currentNode->getNumber()) + ", \t current depth: " + intToString(actualDepth) + "\n");

	trace(TRACE_3, "\t number of states in node: ");
	trace(TRACE_3, intToString(currentNode->setOfStates.size()) + "\n");

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
		return;
	}
	
	// iterate over all elements of inputSet
	for (setOfMessages::iterator iter = inputSet.begin(); iter != inputSet.end(); iter++) {
		
		vertex * v = new vertex();	// create new vertex of the graph
		currentVertex = currentNode;
		
		calculateSuccStatesInput(*iter, currentNode, v);

		if (AddVertex (v, *iter, sending)) {

#ifdef LOOP
	cout << "calc next node? [y,n]" << endl;
	char a = getchar();
	if (a == 'y') {
#endif
			
			buildGraph(v);
			trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
			analyseNode(currentNode, false);
			trace(TRACE_5, "node analysed\n");
#ifdef LOOP
	}
#endif	

		}
	}
	
	for (setOfMessages::iterator iter = outputSet.begin(); iter != outputSet.end(); iter++) {
		vertex * v = new vertex();	// create new vertex of the graph
		currentVertex = currentNode;
				
		calculateSuccStatesOutput(*iter, currentNode, v);
		
		if (AddVertex (v, *iter, receiving)) {

#ifdef LOOP
	cout << "calc next node? [y,n]" << endl;
	char a = getchar();
	if (a == 'y') {
#endif
			buildGraph(v);
			trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
			analyseNode(currentNode, false);
			trace(TRACE_5, "node analysed\n");

#ifdef LOOP
	}
#endif

		}	
	}	
		
	analyseNode(currentNode, true);
	trace(TRACE_5, "node analysed\n");

	actualDepth--;
	
	string color;
	if (currentNode->getColor() == RED) {
		color = "RED";
	} else if (currentNode->getColor() == BLUE) {
		color = "BLUE";
	} else {
		color = "BLACK";
	}

	trace(TRACE_1, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + color + "\n");	
}

//! \fn void interactionGraph::buildReducedGraph(vertex * currentNode)
//! \param currentNode current node of the graph
//! \brief builds up the graph recursively
void interactionGraph::buildReducedGraph(vertex * currentNode) {

	setOfMessages inputSet;
	setOfMessages outputSet;
	
	// initialize node
	if (PN->getInputPlaceCnt() > 0) {
	//	inputSet = receivingBeforeSending(currentNode);		// per node
	}
	
//	if (PN->getOutputPlaceCnt() > 0) {
		outputSet = combineReceivingEvents(currentNode, inputSet);
//	}
	
	actualDepth++;

	trace(TRACE_1, "\n=================================================================\n");
	trace(TRACE_1, "\t current node: ");
	trace(TRACE_1, intToString(currentNode->getNumber()) + ", \t current depth: " + intToString(actualDepth) + "\n");

	trace(TRACE_3, "\t number of states in node: ");
	trace(TRACE_3, intToString(currentNode->setOfStates.size()) + "\n");

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
		return;
	}

	// iterate over all elements of inputSet
//	while (elementInput&& (currentNode->getColor() != RED)) {
	for (setOfMessages::iterator iter = inputSet.begin(); iter != inputSet.end(); iter++) {

		vertex * v = new vertex();		// create new vertex of the graph
		currentVertex = currentNode;
		
		calculateSuccStatesInput(*iter, currentNode, v);
	//	v->setStateList(newNodeStateList);
		
		if (AddVertex (v, *iter, sending)) {
			buildReducedGraph(v);
			trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
			analyseNode(currentNode, false);
			trace(TRACE_5, "node analysed\n");
		}
	}


	for (setOfMessages::iterator iter = outputSet.begin(); iter != outputSet.end(); iter++) {
//	while (elementOutput&& (currentNode->getColor() != RED)) {
		vertex * v = new vertex();	// create new vertex of the graph
		currentVertex = currentNode;
					
		calculateSuccStatesOutput(*iter, currentNode, v);
		//v->setStateList(newNodeStateList);

		
		if (AddVertex (v, *iter, receiving)) {
			buildReducedGraph(v);
			trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
			analyseNode(currentNode, false);
			trace(TRACE_5, "node analysed\n");
		}	
	}	
		
	actualDepth--;
	analyseNode(currentNode, true);
	trace(TRACE_5, "node analysed\n");
	
	string color;
	if (currentNode->getColor() == RED) {
		color = "RED";
	} else if (currentNode->getColor() == BLUE) {
		color = "BLUE";
	} else {
		color = "BLACK";
	}
	
	trace(TRACE_1, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + color + "\n");
}

//! \fn void interactionGraph::getActivatedEventsComputeCNF(vertex * node, setOfMessages & inputMessages, setOfMessages & outputMessages) {
//! \param node the node for which the activated input events are calculated
//! \param inputMessages
//! \param outputMessages
//! \brief creates a list of all activated input events (messages) of the current node
void interactionGraph::getActivatedEventsComputeCNF(vertex * node, setOfMessages & inputMessages, setOfMessages & outputMessages) {
	trace(TRACE_5, "interactionGraph::getActivatedInputEvents(vertex * node): start\n");

	int i;
	StateSet::iterator iter;		
	
	if (!parameters[P_CALC_ALL_STATES]) { // in case of the state reduced graph
	
		for (iter = node->setOfStatesTemp.begin(); iter != node->setOfStatesTemp.end(); iter++) {
	
	#ifdef DEBUG
		//cout << "\t state " << PN->printMarking((*iter)->myMarking) << " activates the input events: " << endl;
	#endif		
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {	// we just consider the maximal states only
				
	
				clause * cl = new clause();			// create a new clause for this particular state
				(*iter)->decode(PN);
				
				i = 0;
				// get the activated input events
				while ((*iter)->quasiFirelist && (*iter)->quasiFirelist[i]) {
					for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin();
								index != (*iter)->quasiFirelist[i]->messageSet.end();
								index++) {
	
						messageMultiSet input;				// multiset holding one input message
						input.insert(*index);
						
						inputMessages.insert(input);
						cl->addLiteral(PN->Places[*index]->name);
					}
					i++;
				}
	
				// get the activated output events			
				for (int i = 0; i < PN->getPlaceCnt(); i++) {
					if (PN->Places[i]->type == OUTPUT && PN->CurrentMarking[i] > 0) {
						messageMultiSet output;
						output.insert(i);
						
						outputMessages.insert(output);
						cl->addLiteral(PN->Places[i]->name);	
					}	
				}
				node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
			}
		}
	} else {
		for (iter = node->setOfStates.begin(); iter != node->setOfStates.end(); iter++) {
	
	#ifdef DEBUG
		//cout << "\t state " << PN->printMarking((*iter)->myMarking) << " activates the input events: " << endl;
	#endif		
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {	// we just consider the maximal states only
				
	
				clause * cl = new clause();			// create a new clause for this particular state
				(*iter)->decode(PN);
				
				i = 0;
				// get the activated input events
				while ((*iter)->quasiFirelist && (*iter)->quasiFirelist[i]) {
					for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin();
								index != (*iter)->quasiFirelist[i]->messageSet.end();
								index++) {
	
						messageMultiSet input;				// multiset holding one input message
						input.insert(*index);
						
						inputMessages.insert(input);
						cl->addLiteral(PN->Places[*index]->name);
					}
					i++;
				}
	
				// get the activated output events			
				for (int i = 0; i < PN->getPlaceCnt(); i++) {
					if (PN->Places[i]->type == OUTPUT && PN->CurrentMarking[i] > 0) {
						messageMultiSet output;
						output.insert(i);
						
						outputMessages.insert(output);
						cl->addLiteral(PN->Places[i]->name);	
					}	
				}
				node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
			}
		}
	}			
	trace(TRACE_5, "interactionGraph::getActivatedInputEvents(vertex * node): end\n");
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
// reduction rules
/////////////////////////////////////////////////////////////////////////////////////////////

//! \fn setOfMessages interactionGraph::combineReceivingEvents(vertex * node)
//! \param node the node for which the activated output events are calculated
//! \brief creates a list of all output messages of the current node
setOfMessages interactionGraph::combineReceivingEvents(vertex * node, setOfMessages & inputMessages) {
#ifdef DEBUG
	cout << "interactionGraph::combineReceivingEvents(vertex * node): start" << endl;
#endif

	int i;
	
	std::vector<StateSet::iterator> statesVector; // remember those states that activate an output event
	
	setOfMessages listOfOutputMessageLists;	// list 
	
	messageMultiSet outputMessages;				// multiset of all input messages of the current state

	StateSet::iterator iter;		
	
	clause * cl = new clause();
	
	bool found = false;
	bool skip = false;
	
	for (iter = node->setOfStates.begin(); iter != node->setOfStates.end(); iter++) {

		if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {  // we just consider the maximal states only

#ifdef DEBUG
	cout << "\t state "<< (*iter) << " activates the output events: " << endl;
#endif		
			int i = 0;
			int k = 0;
			clause * cl = new clause();			// create a new clause for this particular state

			// "receiving before sending" reduction rule
			while (!stateActivatesOutputEvents(*iter) && 
						(*iter)->quasiFirelist && 
						(*iter)->quasiFirelist[i]) {
				
				for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin();
							index != (*iter)->quasiFirelist[i]->messageSet.end();
							index++) {

					messageMultiSet input;				// multiset holding one input message
					input.insert(*index);
					
					inputMessages.insert(input);
					
					cl->addLiteral(PN->Places[*index]->name);
				}
				i++;
			}			
			
			messageMultiSet outputMessages;		// multiset of all input messages of the current state
			
			(*iter)->decode(PN);
			
			for (i = 0; i < PN->getPlaceCnt(); i++) {
				
				if (PN->Places[i]->type == OUTPUT && PN->CurrentMarking[i] > 0) {	
					for (int z = 0; z < PN->CurrentMarking[i]; z++) {			
						outputMessages.insert(i);
					}
					
					found = true;	
#ifdef DEBUG
	cout << "\t\t" << PN->Places[i]->name << endl;
#endif
				}	
			}
			
			bool subset = false;
			bool supset = false;
			
			char * label;
			
			if (found) {
				
				for (setOfMessages::iterator iter1 = listOfOutputMessageLists.begin(); 
					iter1 != listOfOutputMessageLists.end();
					iter1++) {
					
					subset = false;
			
					for (messageMultiSet::iterator tmp2 = (*iter1).begin(); tmp2 != (*iter1).end(); tmp2++) {
						if (outputMessages.count(*tmp2) >= (*iter1).count(*tmp2)) {
							subset = true;
						} else {
							subset = false;
							break;
						}
					}
					
					if (subset) {
						label = PN->createLabel(*iter1);
						break;
					}
				}
				
				
				if (!subset) {
					for (setOfMessages::iterator iter1 = listOfOutputMessageLists.begin(); 
						iter1 != listOfOutputMessageLists.end();
						iter1++) {
				
						supset = false;
					
						for (messageMultiSet::iterator tmp2 = outputMessages.begin(); tmp2 != outputMessages.end(); tmp2++) {
							if (outputMessages.count(*tmp2) <= (*iter1).count(*tmp2)) {
								supset = true;
							} else {
								supset = false;
								break;
							}
						}
						if (supset) {
							listOfOutputMessageLists.erase(iter1);
							label = PN->createLabel(outputMessages);
							break;
						}
					}
					
					if (supset) {
						cl->addLiteral(label);	
						listOfOutputMessageLists.insert(outputMessages);
					}	
				} else {
					cl->addLiteral(label);	
				}
    			
				if (!subset && !supset) {
					listOfOutputMessageLists.insert(outputMessages);
					cl->addLiteral(PN->createLabel(outputMessages));
				} 
				
				found = false;
				skip = false;
			}
			node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
		}
		
	}

    /* check the set of output-messages for containing subsets */
    /* e.g. the set contains [a, b] and [a, b, c] */
    /* [a, b] is subset of [a, b, c], therefore the set [a, b, c] gets removed */
    
   	return listOfOutputMessageLists;		
}


//! \fn setOfMessageSet interactionGraph::receivingBeforeSending(vertex * node)
//! \param node the node for which the activated input events are calculated
//! \brief creates a list of all activated input events (messages) of the current node with respect to the
//! receiving before sending rule
setOfMessages interactionGraph::receivingBeforeSending(vertex * node) {

	trace(TRACE_5, "interactionGraph::receivingBeforeSending(vertex * node): start\n");

	int i;

	StateSet::iterator iter;		
	
	setOfMessages inputMessages;	// list of all input messages of the current node
	
	for (iter = node->setOfStates.begin(); 
					iter != node->setOfStates.end(); iter++) {

		if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {				// we just consider the maximal states only
			i = 0;
			clause * cl = new clause();
			
			(*iter)->decode(PN);
			while (!stateActivatesOutputEvents(*iter) && 
						(*iter)->quasiFirelist && 
						(*iter)->quasiFirelist[i]) {
				
				for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin();
							index != (*iter)->quasiFirelist[i]->messageSet.end();
							index++) {

					messageMultiSet input;				// multiset holding one input message
					input.insert(*index);
					
					inputMessages.insert(input);
					
					cl->addLiteral(PN->Places[*index]->name);
				}
				i++;
			}
			if ((*iter)->type == FINALSTATE) {
				node->addClause(cl, (*iter)->type == FINALSTATE);
			} else {
				delete cl;	
			}
		}
	}

	trace(TRACE_5, "number of input events: " + intToString(inputMessages.size()) + "\n" );
	trace(TRACE_5, "interactionGraph::receivingBeforeSending(vertex * node): end\n");
   	return inputMessages;							// return the new state list	
}

//! \fn bool interactionGraph::terminateBuildingGraph(vertex * node) 
//! \param node the node to be inspected
//! \brief return true, if building up shall be terminated at the given node, false otherwise 
bool interactionGraph::terminateBuildingGraph(vertex * node) {
	
	if (analyseNode(node, false) == TERMINATE) {
		trace(TRACE_5, "node analysed\n");
		return true;
	}
//	if (actualDepth > PN->commDepth) {			// we have reached the maximal communication depth
//		if (node->getColor() != BLUE) {
//			node->setColor(RED);
//		}
//		return true;	
//	}
	trace(TRACE_5, "node analysed\n");
	return false;
}

//! \fn stateList * interactionGraph::calculateSuccStatesOutputSet(messageList * output, vertex * node)
//! \param output the output messages that are taken from the marking
//! \param node
//! \brief calculates the set of successor states in case of an output message
void interactionGraph::calculateSuccStatesOutputSet(messageMultiSet output, vertex * node) {
    
	/* iterate over all states of the current node 
	 * and get rid of the output messages in the marking of the state
	 * */
	
#ifdef DEBUG
	cout << "interactionGraph::calculateSuccStatesOutputSet(messageMultiSet output, vertex * node): start" << endl;
	cout << "checking node: " << *node << endl;
#endif
	StateSet::iterator iter;	
	
	// iterate over all states of the current node 
	for (iter = node->setOfStates.begin(); iter != node->setOfStates.end(); iter++) {
	
	
//		PN->setCurrentMarkingFromState(*iter);		// set the net to the marking of the state being considered
		(*iter)->decode(PN);
		if (PN->removeOutputMessage(output)) {	// remove the output message from the current marking
			// if there is a state for which an output event was activated, catch that state
			if (parameters[P_CALC_ALL_STATES]) {
				PN->calculateReachableStatesFull(node, false);	// calc the reachable states from that marking
			} else {
				PN->calculateReachableStatesOutputEvent(node, false);	// calc the reachable states from that marking
			}
		}
	}
}

//! \fn stateList * interactionGraph::calculateSuccStatesInputReduced(char * input, vertex * node)
//! \param input set of input messages
//! \param node the node for which the successor states are to be calculated
//! \brief calculates the set of successor states in case of an input message
void interactionGraph::calculateSuccStatesInputReduced(messageMultiSet input, vertex * node) {
	
#ifdef DEBUG
	cout << "interactionGraph::calculateSuccStatesInputReduced(char * input, vertex * node): start" << endl;
#endif

	StateSet::iterator iter;		
	// iterate over all states of the current node 
	for (iter = node->setOfStates.begin(); iter != node->setOfStates.end(); iter++) {


#ifdef DEBUG
	cout << "add input message " << input << " to state " << (*iter) << endl;
#endif
		if (true) {	// if state is representative for this node
//			PN->setCurrentMarkingFromState(*iter);		// set the net to the marking of the state being considered

			(*iter)->decode(PN);

			PN->addInputMessage(input);					// add the input message to the current marking
			if (parameters[P_CALC_ALL_STATES]) {
				PN->calculateReachableStatesFull(node, false);	// calc the reachable states from that marking
			} else {
				PN->calculateReachableStatesInputEvent(node, false);	// calc the reachable states from that marking
			}
		}
	}
}
