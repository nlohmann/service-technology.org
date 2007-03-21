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
 * \file    IG.cc
 *
 * \brief   functions for Interaction Graphs (IG)
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "IG.h"
#include "state.h"
#include "options.h"
#include "debug.h"
#include "CNF.h"
#include "owfn.h"
#include "owfnTransition.h"

#include <vector>			// for combining receiving events

//! \fn interactionGraph::interactionGraph(oWFN * _PN) 
//! \param _PN
//! \brief constructor
interactionGraph::interactionGraph(oWFN * _PN) : communicationGraph(_PN) {
}

//! \fn interactionGraph::~interactionGraph() 
//! \brief destructor !to be implemented!
interactionGraph::~interactionGraph() {
	
}

//! \fn void interactionGraph::buildGraph()
//! \brief builds the graph starting with the root node
void interactionGraph::buildGraph() {
    calculateRootNode(); // creates the root node and calculates its reachability graph (set of states)

	if (options[O_CALC_REDUCED_IG]) {
		buildReducedGraph(root);
	} else {
		buildGraph(root);
	}
}

//! \fn bool interactionGraph::checkMaximalEvents(messageMultiSet messages, vertex * currentNode, edgeType typeOfPlace)
//! \param messages
//! \param currentNode the node from which the input event is to be sent
//! \param typeOfPlace
//! \brief checks whether the set of input messages contains at least one input message
//! that has been sent at its maximum
bool interactionGraph::checkMaximalEvents(messageMultiSet messages, vertex * currentNode, edgeType typeOfPlace) {
	trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet input, vertex * currentNode, bool typeOfPlace): start\n");
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
		if (typeOfPlace == sending) {
			unsigned int i = 0;
			while (i < PN->placeInputCnt-1 && PN->inputPlacesArray[i] && 
						PN->inputPlacesArray[i]->index != *iter) {
				i++;	
			}

			if (currentNode->eventsUsed[i] >= PN->inputPlacesArray[i]->max_occurence) {
				// this input event shall not be sent anymore, so quit here
				trace(TRACE_3, "maximal occurrences of event ");
				trace(TRACE_3, PN->inputPlacesArray[i]->name);
				trace(TRACE_3, " reached\n");

				trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet input, vertex * currentNode, bool typeOfPlace): end\n");
				return false;
			}
		} else if (typeOfPlace == receiving) {
			unsigned int i = 0;
			while (i < PN->placeOutputCnt-1 && PN->outputPlacesArray[i] && 
						PN->outputPlacesArray[i]->index != *iter) {
				i++;	
			}
			if (currentNode->eventsUsed[i + PN->placeInputCnt] >= PN->outputPlacesArray[i]->max_occurence) {
				// this output event shall not be received anymore, so quit here
				trace(TRACE_3, "maximal occurrences of event ");
				trace(TRACE_3, PN->outputPlacesArray[i]->name);
				trace(TRACE_3, " reached\n");
				trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet input, vertex * currentNode, bool typeOfPlace): end\n");				
				return false;		
			}
		}
	}
	// everything is fine
	trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet input, vertex * currentNode, bool typeOfPlace): end\n");
	return true;
}

//! \fn void interactionGraph::buildGraph(vertex * node)
//! \param node current node of the graph
//! \brief builds up the graph recursively
void interactionGraph::buildGraph(vertex * currentNode) {
	
	setOfMessages inputSet;
	setOfMessages outputSet;
	
	getActivatedEventsComputeCNF(currentNode, inputSet, outputSet);

	actualDepth++;

	trace(TRACE_1, "\n=================================================================\n");
	trace(TRACE_1, "\t current node: ");
	trace(TRACE_1, intToString(currentNode->getNumber()) + ", \t current depth: " + intToString(actualDepth) + "\n");

	trace(TRACE_3, "\t number of states in node: ");
	trace(TRACE_3, intToString(currentNode->reachGraphStateSet.size()) + "\n");

//	if (terminateBuildingGraph(currentNode)) {
//		string color;
//		if (currentNode->getColor() == RED) {
//			color = "RED";
//		} else if (currentNode->getColor() == BLUE) {
//			color = "BLUE";
//		} else {
//			color = "BLACK";
//		}
//
//		trace(TRACE_1, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + color + " (leaf)\n");	
//		return;
//	}
	
	trace(TRACE_5, "iterating over inputSet\n");
	// iterate over all elements of inputSet
	for (setOfMessages::iterator iter = inputSet.begin(); iter != inputSet.end(); iter++) {

		if (checkMaximalEvents(*iter, currentNode, sending)) {
	
			trace(TRACE_2, "\t\t\t\t    sending event: !");
			
			vertex * v = new vertex(PN->placeInputCnt + PN->placeOutputCnt);	// create new vertex of the graph
			currentVertex = currentNode;
			
			calculateSuccStatesInput(*iter, currentNode, v);
			
			if (v->getColor() == RED) {
				// message bound violation occured during calculateSuccStatesInput
				trace(TRACE_2, "\t\t\t\t    sending event: !");
			//	trace(TRACE_2, PN->inputPlacesArray[*iter]->name);
				trace(TRACE_2, " at node " + intToString(currentNode->getNumber()) + " suppressed (message bound violated)\n");
	
				numberDeletedVertices--;
				delete v;
			} else {
				if (AddVertex (v, *iter, sending)) {

#ifdef LOOP
	cout << "calc next node? [y,n]" << endl;
	char a = getchar();
	if (a == 'y') {
#endif
			
					buildGraph(v);
					trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
					//analyseNode(currentNode, false);
					//trace(TRACE_5, "node analysed\n");
#ifdef LOOP
	}
#endif	
				}
			}
		}
	}
	
	trace(TRACE_3, "iterating over outputSet\n");
	for (setOfMessages::iterator iter = outputSet.begin(); iter != outputSet.end(); iter++) {
		if (checkMaximalEvents(*iter, currentNode, receiving)) {
			
			trace(TRACE_2, "\t\t\t\t    output event: ?");
	
			vertex * v = new vertex(PN->placeInputCnt + PN->placeOutputCnt);	// create new vertex of the graph
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
				//analyseNode(currentNode, false);
				//trace(TRACE_5, "node analysed\n");
		
#ifdef LOOP
		}
#endif
		
			}
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

	trace(TRACE_1, "\n=================================================================\n");
	trace(TRACE_1, "\t current node: ");
	trace(TRACE_1, intToString(currentNode->getNumber()) + ", \t current depth: " + intToString(actualDepth) + "\n");

	trace(TRACE_3, "\t number of states in node: ");
	trace(TRACE_3, intToString(currentNode->reachGraphStateSet.size()) + "\n");


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

//	if (terminateBuildingGraph(currentNode)) {
//		string color;
//		if (currentNode->getColor() == RED) {
//			color = "RED";
//		} else if (currentNode->getColor() == BLUE) {
//			color = "BLUE";
//		} else {
//			color = "BLACK";
//		}
//
//		trace(TRACE_1, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + color + " (leaf)\n");	
//		return;
//	}

	trace(TRACE_3, "iterating over inputSet\n");
	// iterate over all elements of inputSet
	for (setOfMessages::iterator iter = inputSet.begin(); iter != inputSet.end(); iter++) {
		if (checkMaximalEvents(*iter, currentNode, sending)) {

			trace(TRACE_2, "\t\t\t\t    input event: ?");

			vertex * v = new vertex(PN->placeInputCnt + PN->placeOutputCnt);		// create new vertex of the graph
			currentVertex = currentNode;
			
			calculateSuccStatesInput(*iter, currentNode, v);
			
			if (AddVertex (v, *iter, sending)) {
				buildReducedGraph(v);
				trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
				//analyseNode(currentNode, false);
				//trace(TRACE_5, "node analysed\n");
			}
		}
	}


	trace(TRACE_3, "iterating over outputSet\n");
	for (setOfMessages::iterator iter = outputSet.begin(); iter != outputSet.end(); iter++) {
		if (checkMaximalEvents(*iter, currentNode, receiving)) {
		
			trace(TRACE_2, "\t\t\t\t    output event: ?");
		
			vertex * v = new vertex(PN->placeInputCnt + PN->placeOutputCnt);	// create new vertex of the graph
			currentVertex = currentNode;
						
			calculateSuccStatesOutput(*iter, currentNode, v);
	
			
			if (AddVertex (v, *iter, receiving)) {
				buildReducedGraph(v);
				trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
				//analyseNode(currentNode, false);
				//trace(TRACE_5, "node analysed\n");
			}	
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
//! \param inputMessages the set of input messages (sending events) that are activated in the current node 
//! \param outputMessages the set of output messages (receiving events) that are activated in the current node 
//! \brief creates a list of all activated sending and receiving events (input messages and output messages) of the current node
void interactionGraph::getActivatedEventsComputeCNF(vertex * node, setOfMessages & inputMessages, setOfMessages & outputMessages) {
	trace(TRACE_5, "interactionGraph::getActivatedEventsComputeCNF(vertex * node): start\n");

	int i;
	StateSet::iterator iter;	

	if (!options[O_CALC_ALL_STATES]) { // in case of the state reduced graph
	
		// as the current node has been computed a temporal set of states was created
		// this set of states includes the deadlocks as well
		for (iter = PN->setOfStatesTemp.begin(); iter != PN->setOfStatesTemp.end(); iter++) {
			// we just consider the maximal states only
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {	
				
				literal * cl = new literal();			// create a new clause for this particular state
			    (*iter)->decode(PN);				// get the marking for this state

				if ((*iter)->quasiFirelist) {		// delete the list of quasi enabled transitions
				    delete [] (*iter)->quasiFirelist;
				    (*iter)->quasiFirelist = NULL;
				}	    
				(*iter)->quasiFirelist = PN->quasiFirelist();	// get the firelist of the quasi enabled transitions
				
				i = 0;
				// get the activated input events
			//	cout << "state " << PN->getCurrentMarkingAsString() << " activates the input events: " << (*iter)->quasiFirelist << ", " << (*iter)->quasiFirelist[i] << endl;
				while ((*iter)->quasiFirelist && (*iter)->quasiFirelist[i]) {
					for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin();
								index != (*iter)->quasiFirelist[i]->messageSet.end();
								index++) {
	
						messageMultiSet input;				// multiset holding one input message
					 	input.insert(*index);
						
						inputMessages.insert(input);
						cl->addLiteral(PN->Places[*index]->name);
						
			//			cout << "\t" << PN->Places[*index]->name << endl;
					}
					i++;
				}
	
				// get the activated output events			
				for (unsigned int i = 0; i < PN->getPlaceCnt(); i++) {
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
		// we calculate the graph with the whole set of states stored in the nodes
		// so we use the set of states that were actually stored in the node 
		for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
	
	#ifdef DEBUG
		//cout << "\t state " << PN->getMarkingAsString((*iter)->myMarking) << " activates the input events: " << endl;
	#endif		
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {	// we just consider the maximal states only
				
	
				literal * cl = new literal();			// create a new clause for this particular state
				(*iter)->decode(PN);
				
				i = 0;
				// get the activated input events
			//	cout << "state " << PN->getCurrentMarkingAsString() << " activates the input events: " << (*iter)->quasiFirelist << ", " << (*iter)->quasiFirelist[i] << endl;
				while ((*iter)->quasiFirelist && (*iter)->quasiFirelist[i]) {
					for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin();
								index != (*iter)->quasiFirelist[i]->messageSet.end();
								index++) {
	
						messageMultiSet input;				// multiset holding one input message
						input.insert(*index);
						
						inputMessages.insert(input);
						cl->addLiteral(PN->Places[*index]->name);
				//		cout << "\t" << PN->Places[*index]->name << endl;
					}
					i++;
				}
				// get the activated output events			
				for (unsigned int i = 0; i < PN->getPlaceCnt(); i++) {
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

//! \fn setOfMessages interactionGraph::combineReceivingEvents(vertex * node, setOfMessages & inputMessages)
//! \param node the node for which the activated output events are calculated
//! \param inputMessages
//! \brief creates a list of all output messages of the current node
setOfMessages interactionGraph::combineReceivingEvents(vertex * node, setOfMessages & inputMessages) {
	
	trace(TRACE_5, "interactionGraph::combineReceivingEvents(vertex * node): start\n");

	std::vector<StateSet::iterator> statesVector; 	// remember those states that activate an output event
	
	setOfMessages listOfOutputMessageLists;			// list 
	
	messageMultiSet outputMessages;					// multiset of all input messages of the current state

	StateSet::iterator iter;		
	
	bool found = false;
	bool skip = false;
	
	if (!options[O_CALC_ALL_STATES]) { // in case of the state reduced graph
	
		for (iter = PN->setOfStatesTemp.begin(); iter != PN->setOfStatesTemp.end(); iter++) {

		if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {  // we just consider the maximal states only

#ifdef DEBUG
	cout << "\t state "<< (*iter) << " activates the output events: " << endl;
#endif		
			unsigned int i = 0;
			literal * cl = new literal();			// create a new clause for this particular state

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
					for (unsigned int z = 0; z < PN->CurrentMarking[i]; z++) {			
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
			
			string label;
			
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
	} else {
		for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {

		if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {  // we just consider the maximal states only

#ifdef DEBUG
	cout << "\t state "<< (*iter) << " activates the output events: " << endl;
#endif		
			unsigned int i = 0;
			literal * cl = new literal();			// create a new clause for this particular state

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
					for (unsigned int z = 0; z < PN->CurrentMarking[i]; z++) {			
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
			
			string label;
			
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
	}

	trace(TRACE_5, "interactionGraph::combineReceivingEvents(vertex * node): end\n");

    /* check the set of output-messages for containing subsets */
    /* e.g. the set contains [a, b] and [a, b, c] */
    /* [a, b] is subset of [a, b, c], therefore the set [a, b, c] is removed */
    
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
	
	for (iter = node->reachGraphStateSet.begin(); 
					iter != node->reachGraphStateSet.end(); iter++) {

		if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {				// we just consider the maximal states only
			i = 0;
			literal * cl = new literal();
			
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
   	return inputMessages;   // return the list of activated input messages
}

////! \fn bool interactionGraph::terminateBuildingGraph(vertex * node) 
////! \param node the node to be inspected
////! \brief return true, if building up shall be terminated at the given node, false otherwise 
//bool interactionGraph::terminateBuildingGraph(vertex * node) {
//	
////	if (analyseNode(node, false) == TERMINATE) {
////		trace(TRACE_5, "node analysed\n");
////		return true;
////	}
////	if (actualDepth > PN->commDepth) {			// we have reached the maximal communication depth
////		if (node->getColor() != BLUE) {
////			node->setColor(RED);
////		}
////		return true;	
////	}
//	//trace(TRACE_5, "node analysed\n");
//	return false;
//}

//! \fn void interactionGraph::calculateSuccStatesOutputSet(messageMultiSet output, vertex * node)
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
	for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
	
	
//		PN->setCurrentMarkingFromState(*iter);		// set the net to the marking of the state being considered
		(*iter)->decode(PN);
		if (PN->removeOutputMessage(output)) {	// remove the output message from the current marking
			owfnPlace * outputPlace;
			
			// CHANGE THAT!!!!!!!!!!! is just a hack, stubborn set method does not yet work for more than one output event!
			for (messageMultiSet::iterator iter = output.begin(); iter != output.end(); iter++) {
				outputPlace = PN->Places[*iter];
			}
			
			// if there is a state for which an output event was activated, catch that state
			if (options[O_CALC_ALL_STATES]) {
				PN->calculateReachableStatesFull(node);	// calc the reachable states from that marking
			} else {
				PN->calculateReachableStatesOutputEvent(node);	// calc the reachable states from that marking
			}
		}
	}
}

//! \fn void interactionGraph::calculateSuccStatesInputReduced(messageMultiSet input, vertex * node)
//! \param input set of input messages
//! \param node the node for which the successor states are to be calculated
//! \brief calculates the set of successor states in case of an input message
void interactionGraph::calculateSuccStatesInputReduced(messageMultiSet input, vertex * node) {
	
#ifdef DEBUG
	cout << "interactionGraph::calculateSuccStatesInputReduced(char * input, vertex * node): start" << endl;
#endif

	StateSet::iterator iter;		
	// iterate over all states of the current node 
	for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {


#ifdef DEBUG
	cout << "add input message " << input << " to state " << (*iter) << endl;
#endif
		if (true) {	// if state is representative for this node
//			PN->setCurrentMarkingFromState(*iter);		// set the net to the marking of the state being considered

			(*iter)->decode(PN);

			PN->addInputMessage(input);					// add the input message to the current marking
			if (options[O_CALC_ALL_STATES]) {
				PN->calculateReachableStatesFull(node);	// calc the reachable states from that marking
			} else {
				PN->calculateReachableStatesInputEvent(node, false);	// calc the reachable states from that marking
			}
		}
	}
}
