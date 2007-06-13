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
//#include "CNF.h"
#include "owfnTransition.h"


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

  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

    calculateRootNode(); // creates the root node and calculates its reachability graph (set of states)

	if (options[O_CALC_REDUCED_IG]) {
		buildReducedGraph(root);
	} else {
		buildGraph(root);
	}
}


//! \fn void interactionGraph::buildGraph(GraphNode * node)
//! \param node current node of the graph
//! \brief builds up the graph recursively
void interactionGraph::buildGraph(GraphNode * currentNode) {
	
	// at this point, the states inside the current node node are already computed!
	
	actualDepth++;

	trace(TRACE_1, "\n=================================================================\n");
	trace(TRACE_1, "\t current node: ");
	trace(TRACE_1, intToString(currentNode->getNumber()) + ", \t current depth: " + intToString(actualDepth) + "\n");
	if (debug_level >= TRACE_2) {
		cout << "\t (" << currentNode << ")" << endl;
	}

	trace(TRACE_2, "\t number of states in node: ");
	trace(TRACE_2, intToString(currentNode->reachGraphStateSet.size()) + "\n");

	if (currentNode->getColor() == RED) {
		// this may happen due to a message bound violation in current node
		// then, function calculateReachableStatesFull sets node color RED
		trace(TRACE_3, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color RED\n");
		trace(TRACE_1, "=================================================================\n");
		return;
	}

	setOfMessages inputSet;
	setOfMessages outputSet;
	
	getActivatedEventsComputeCNF(currentNode, inputSet, outputSet);

	trace(TRACE_1, "=================================================================\n");

	trace(TRACE_2, "iterating over inputSet\n");
	// iterate over all elements of inputSet
	for (setOfMessages::iterator iter = inputSet.begin();
         iter != inputSet.end(); iter++) {

		if ((options[O_EVENT_USE_MAX] == false) ||
             checkMaximalEvents(*iter, currentNode, SENDING)) {
	
			trace(TRACE_2, "\t\t\t\t    sending event: !");
			
			GraphNode * v = new GraphNode(PN->getInputPlaceCount() + PN->getOutputPlaceCount());	// create new GraphNode of the graph
			currentGraphNode = currentNode;
			
			calculateSuccStatesInput(*iter, currentNode, v);
			
			if (v->getColor() == RED) {
				// message bound violation occured during calculateSuccStatesInput
				trace(TRACE_2, "\t\t\t\t    sending event: !");
			//	trace(TRACE_2, PN->getInputPlace(*iter)->name);
				trace(TRACE_2, " at node " + intToString(currentNode->getNumber()) + " suppressed (message bound violated)\n");
	
				numberDeletedVertices--;
				delete v;
			} else {
				if (AddGraphNode (v, *iter, SENDING)) {

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
        if ((options[O_EVENT_USE_MAX] == false) ||
		     checkMaximalEvents(*iter, currentNode, RECEIVING)) {
			
			trace(TRACE_2, "\t\t\t\t    output event: ?");
	
			GraphNode * v = new GraphNode(PN->getInputPlaceCount() + PN->getOutputPlaceCount());	// create new GraphNode of the graph
			currentGraphNode = currentNode;
			
			calculateSuccStatesOutput(*iter, currentNode, v);
			
			if (AddGraphNode (v, *iter, RECEIVING)) {
		
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
	analyseNode(currentNode);
	trace(TRACE_5, "node analysed\n");

	actualDepth--;
	
	trace(TRACE_1, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + toUpper(currentNode->getColor().toString()) + "\n");	
}

//! \fn void interactionGraph::buildReducedGraph(GraphNode * currentNode)
//! \param currentNode current node of the graph
//! \brief builds up the graph recursively
void interactionGraph::buildReducedGraph(GraphNode * currentNode) {

	if (currentNode->getColor() == RED) {
		// this may happen due to a message bound violation in current node
		// then, function calculateReachableStatesFull sets node color RED
		trace(TRACE_3, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color RED\n");
		trace(TRACE_1, "=================================================================\n");
		return;
	}

	trace(TRACE_1, "\n=================================================================\n");
	trace(TRACE_1, "\t current node: ");
	trace(TRACE_1, intToString(currentNode->getNumber()) + ", \t current depth: " + intToString(actualDepth) + "\n");

	trace(TRACE_3, "\t number of states in node: ");
	trace(TRACE_3, intToString(currentNode->reachGraphStateSet.size()) + "\n");

	setOfMessages inputSet;
	setOfMessages outputSet;
	
	// initialize node
	if (PN->getInputPlaceCount() > 0) {
	//	inputSet = receivingBeforeSending(currentNode);		// per node
	}
	
//	if (PN->getOutputPlaceCnt() > 0) {
		outputSet = combineReceivingEvents(currentNode, inputSet);
//	}

	actualDepth++;

  	PN->setOfStatesTemp.clear();
  	PN->visitedStates.clear();

	trace(TRACE_3, "iterating over inputSet\n");
	// iterate over all elements of inputSet
	for (setOfMessages::iterator iter = inputSet.begin(); iter != inputSet.end(); iter++) {
        if ((options[O_EVENT_USE_MAX] == false) ||
             checkMaximalEvents(*iter, currentNode, SENDING)) {

			trace(TRACE_2, "\t\t\t\t    input event: ?");

			GraphNode * v = new GraphNode(PN->getInputPlaceCount() + PN->getOutputPlaceCount());		// create new GraphNode of the graph
			currentGraphNode = currentNode;
			
			calculateSuccStatesInput(*iter, currentNode, v);
			
			if (AddGraphNode (v, *iter, SENDING)) {
				buildReducedGraph(v);
				trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
				//analyseNode(currentNode, false);
				//trace(TRACE_5, "node analysed\n");
			}
		}
	}


	trace(TRACE_3, "iterating over outputSet\n");
	for (setOfMessages::iterator iter = outputSet.begin(); iter != outputSet.end(); iter++) {
        if ((options[O_EVENT_USE_MAX] == false) ||
             checkMaximalEvents(*iter, currentNode, RECEIVING)) {
		
			trace(TRACE_2, "\t\t\t\t    output event: ?");
		
			GraphNode * v = new GraphNode(PN->getInputPlaceCount() + PN->getOutputPlaceCount());	// create new GraphNode of the graph
			currentGraphNode = currentNode;
						
			calculateSuccStatesOutput(*iter, currentNode, v);
	
			
			if (AddGraphNode (v, *iter, RECEIVING)) {
				buildReducedGraph(v);
				trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
				//analyseNode(currentNode, false);
				//trace(TRACE_5, "node analysed\n");
			}	
		}	
	}
			
	actualDepth--;
	analyseNode(currentNode);
	trace(TRACE_5, "node analysed\n");
	
	trace(TRACE_1, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + toUpper(currentNode->getColor().toString()) + "\n");
}


//! \fn bool interactionGraph::checkMaximalEvents(messageMultiSet messages, GraphNode * currentNode, GraphEdgeType typeOfPlace)
//! \param messages
//! \param currentNode the node from which the input event is to be sent
//! \param typeOfPlace
//! \brief checks whether the set of input messages contains at least one input message
//! that has been sent at its maximum
bool interactionGraph::checkMaximalEvents(messageMultiSet messages, GraphNode * currentNode, GraphEdgeType typeOfPlace) {
	trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet input, GraphNode * currentNode, bool typeOfPlace): start\n");
	for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
		if (typeOfPlace == SENDING) {
			unsigned int i = 0;
			while (i < PN->getInputPlaceCount()-1 && PN->getInputPlace(i) && 
						PN->getInputPlace(i)->index != *iter) {
				i++;	
			}

			if (currentNode->eventsUsed[i] >= PN->getInputPlace(i)->max_occurence) {
				// this input event shall not be sent anymore, so quit here
				trace(TRACE_3, "maximal occurrences of event ");
				trace(TRACE_3, PN->getInputPlace(i)->name);
				trace(TRACE_3, " reached\n");

				trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet input, GraphNode * currentNode, bool typeOfPlace): end\n");
				return false;
			}
		} else if (typeOfPlace == RECEIVING) {
			unsigned int i = 0;
			while (i < PN->getOutputPlaceCount()-1 && PN->getOutputPlace(i) && 
						PN->getOutputPlace(i)->index != *iter) {
				i++;	
			}
			if (currentNode->eventsUsed[i + PN->getInputPlaceCount()] >= PN->getOutputPlace(i)->max_occurence) {
				// this output event shall not be received anymore, so quit here
				trace(TRACE_3, "maximal occurrences of event ");
				trace(TRACE_3, PN->getOutputPlace(i)->name);
				trace(TRACE_3, " reached\n");
				trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet input, GraphNode * currentNode, bool typeOfPlace): end\n");				
				return false;		
			}
		}
	}
	// everything is fine
	trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet input, GraphNode * currentNode, bool typeOfPlace): end\n");
	return true;
}


//! \fn void interactionGraph::getActivatedEventsComputeCNF(GraphNode * node, setOfMessages & inputMessages, setOfMessages & outputMessages) {
//! \param node the node for which the activated input events are calculated
//! \param inputMessages the set of input messages (sending events) that are activated in the current node 
//! \param outputMessages the set of output messages (receiving events) that are activated in the current node 
//! \brief creates a list of all activated sending and receiving events (input messages and output messages) of the current node
void interactionGraph::getActivatedEventsComputeCNF(GraphNode * node, setOfMessages & inputMessages, setOfMessages & outputMessages) {
	trace(TRACE_5, "interactionGraph::getActivatedEventsComputeCNF(GraphNode * node): start\n");

	int i;
	StateSet::iterator iter;

	if (!options[O_CALC_ALL_STATES]) { // in case of the state reduced graph
	
		// as the current node has been computed a temporal set of states was created
		// this set of states includes the deadlocks as well
		for (iter = PN->setOfStatesTemp.begin(); iter != PN->setOfStatesTemp.end(); iter++) {
			// we just consider the maximal states only
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {	
				
				// this clause's first literal
				CommGraphFormulaMultiaryOr* myclause = new CommGraphFormulaMultiaryOr();
								
				//literal * cl = new literal();			// create a new clause for this particular state
			    
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
				
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
						myclause->addSubFormula(myliteral);
						
						//cl->addLiteral(PN->getPlace(*index)->getLabelForCommGraph());
						
			//			cout << "\t" << PN->getPlace(*index)->name << endl;
					}
					i++;
				}
	
				// get the activated output events			
				for (unsigned int i = 0; i < PN->getPlaceCount(); i++) {
					if (PN->getPlace(i)->type == OUTPUT && PN->CurrentMarking[i] > 0) {
						messageMultiSet output;
						output.insert(i);
						
						outputMessages.insert(output);
						
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getPlace(i)->getLabelForCommGraph());
						myclause->addSubFormula(myliteral);
						
						//cl->addLiteral(PN->getPlace(i)->getLabelForCommGraph());	
					}	
				}
				
				// in case of a final state we add special literal "final" to the clause
				if ((*iter)->type == FINALSTATE) {
					node->hasFinalStateInStateSet = true;

					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteralFinal();
					myclause->addSubFormula(myliteral);
				}
				
				node->addClause(myclause);
				//node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
			}
		}
	} else {
		// we calculate the graph with the whole set of states stored in the nodes
		// so we use the set of states that were actually stored in the node 
		for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {
	
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {	// we just consider the maximal states only
				
				// this clause's first literal
				CommGraphFormulaMultiaryOr* myclause = new CommGraphFormulaMultiaryOr();
	
//				literal * cl = new literal();			// create a new clause for this particular state
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
						
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
						myclause->addSubFormula(myliteral);
						
						//cl->addLiteral(PN->getPlace(*index)->getLabelForCommGraph());
				//		cout << "\t" << PN->getPlace(*index)->name << endl;
					}
					i++;
				}
				// get the activated output events			
				for (unsigned int i = 0; i < PN->getPlaceCount(); i++) {
					if (PN->getPlace(i)->type == OUTPUT && PN->CurrentMarking[i] > 0) {
						messageMultiSet output;
						output.insert(i);
						
						outputMessages.insert(output);
						
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getPlace(i)->getLabelForCommGraph());
						myclause->addSubFormula(myliteral);
						
						//cl->addLiteral(PN->getPlace(i)->getLabelForCommGraph());	
					}	
				}
				
				// in case of a final state we add special literal "final" to the clause
				if ((*iter)->type == FINALSTATE) {
					node->hasFinalStateInStateSet = true;

					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteralFinal();
					myclause->addSubFormula(myliteral);
				}
				
				node->addClause(myclause);
				//node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
			}
		}

	}			
	trace(TRACE_5, "interactionGraph::getActivatedInputEvents(GraphNode * node): end\n");
	
}

/////////////////////////////////////////////////////////////////////////////////////////////
// reduction rules
/////////////////////////////////////////////////////////////////////////////////////////////

//! \fn setOfMessages interactionGraph::combineReceivingEvents(GraphNode * node, setOfMessages & inputMessages)
//! \param node the node for which the activated output events are calculated
//! \param inputMessages
//! \brief creates a list of all output messages of the current node
setOfMessages interactionGraph::combineReceivingEvents(GraphNode * node, setOfMessages & inputMessages) {
	
	trace(TRACE_5, "interactionGraph::combineReceivingEvents(GraphNode * node): start\n");

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
			
			// this clause's first literal
			CommGraphFormulaMultiaryOr* myclause = new CommGraphFormulaMultiaryOr();
			
			//literal * cl = new literal();			// create a new clause for this particular state

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
					
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
					myclause->addSubFormula(myliteral);
					
					//cl->addLiteral(PN->getPlace(*index)->getLabelForCommGraph());
				}
				i++;
			}			
			
			messageMultiSet outputMessages;		// multiset of all input messages of the current state
			
			(*iter)->decode(PN);
			
			for (i = 0; i < PN->getPlaceCount(); i++) {
				
				if (PN->getPlace(i)->type == OUTPUT && PN->CurrentMarking[i] > 0) {	
					for (unsigned int z = 0; z < PN->CurrentMarking[i]; z++) {			
						outputMessages.insert(i);
					}
					
					found = true;	
#ifdef DEBUG
	cout << "\t\t" << PN->getPlace(i)->name << endl;
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
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(label);
						myclause->addSubFormula(myliteral);
						
						//cl->addLiteral(label);	
						listOfOutputMessageLists.insert(outputMessages);
					}	
				} else {
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(label);
					myclause->addSubFormula(myliteral);
					//cl->addLiteral(label);	
				}
    			
				if (!subset && !supset) {
					listOfOutputMessageLists.insert(outputMessages);
					
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->createLabel(outputMessages));
					myclause->addSubFormula(myliteral);
					
//					cl->addLiteral(PN->createLabel(outputMessages));
				} 
				
				found = false;
				skip = false;
			}
			
			// in case of a final state we add special literal "final" to the clause
			if ((*iter)->type == FINALSTATE) {
				node->hasFinalStateInStateSet = true;

				CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteralFinal();
				myclause->addSubFormula(myliteral);
			}
			
			node->addClause(myclause);
			//node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
		}
		
	}
	} else {
		for (iter = node->reachGraphStateSet.begin(); iter != node->reachGraphStateSet.end(); iter++) {

		if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {  // we just consider the maximal states only

#ifdef DEBUG
	cout << "\t state "<< (*iter) << " activates the output events: " << endl;
#endif		
			unsigned int i = 0;
			// this clause's first literal
			CommGraphFormulaMultiaryOr* myclause = new CommGraphFormulaMultiaryOr();

//			literal * cl = new literal();			// create a new clause for this particular state

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
					
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
					myclause->addSubFormula(myliteral);
					
					//cl->addLiteral(PN->getPlace(*index)->getLabelForCommGraph());
				}
				i++;
			}			
			
			messageMultiSet outputMessages;		// multiset of all input messages of the current state
			
			(*iter)->decode(PN);
			
			for (i = 0; i < PN->getPlaceCount(); i++) {
				
				if (PN->getPlace(i)->type == OUTPUT && PN->CurrentMarking[i] > 0) {	
					for (unsigned int z = 0; z < PN->CurrentMarking[i]; z++) {			
						outputMessages.insert(i);
					}
					
					found = true;	
#ifdef DEBUG
	cout << "\t\t" << PN->getPlace(i)->name << endl;
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
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(label);
						myclause->addSubFormula(myliteral);
					
						//cl->addLiteral(label);	
						listOfOutputMessageLists.insert(outputMessages);
					}	
				} else {
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(label);
					myclause->addSubFormula(myliteral);
					
					//cl->addLiteral(label);	
				}
    			
				if (!subset && !supset) {
					listOfOutputMessageLists.insert(outputMessages);
					
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->createLabel(outputMessages));
					myclause->addSubFormula(myliteral);
					//cl->addLiteral(PN->createLabel(outputMessages));
				} 
				
				found = false;
				skip = false;
			}

			// in case of a final state we add special literal "final" to the clause
			if ((*iter)->type == FINALSTATE) {
				node->hasFinalStateInStateSet = true;

				CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteralFinal();
				myclause->addSubFormula(myliteral);
			}

			node->addClause(myclause);
			//node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
		}
		
	}		
	}

	trace(TRACE_5, "interactionGraph::combineReceivingEvents(GraphNode * node): end\n");

    /* check the set of output-messages for containing subsets */
    /* e.g. the set contains [a, b] and [a, b, c] */
    /* [a, b] is subset of [a, b, c], therefore the set [a, b, c] is removed */
    
   	return listOfOutputMessageLists;		
}


//! \fn setOfMessageSet interactionGraph::receivingBeforeSending(GraphNode * node)
//! \param node the node for which the activated input events are calculated
//! \brief creates a list of all activated input events (messages) of the current node with respect to the
//! receiving before sending rule
setOfMessages interactionGraph::receivingBeforeSending(GraphNode * node) {

	trace(TRACE_5, "interactionGraph::receivingBeforeSending(GraphNode * node): start\n");

	int i;

	StateSet::iterator iter;		
	
	setOfMessages inputMessages;	// list of all input messages of the current node
	
	for (iter = node->reachGraphStateSet.begin(); 
					iter != node->reachGraphStateSet.end(); iter++) {

		if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {				// we just consider the maximal states only
			i = 0;
			
			// this clause's first literal
			CommGraphFormulaMultiaryOr* myclause = new CommGraphFormulaMultiaryOr();
			
//			literal * cl = new literal();
			
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
					
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
					myclause->addSubFormula(myliteral);					
					
					//cl->addLiteral(PN->getPlace(*index)->getLabelForCommGraph());
				}
				i++;
			}
			if ((*iter)->type == FINALSTATE) {
				
				CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteralFinal();
				myclause->addSubFormula(myliteral);
				
				node->addClause(myclause);
				
				//node->addClause(cl, (*iter)->type == FINALSTATE);
			} else {
				delete myclause;
				//delete cl;	
			}
		}
	}

	trace(TRACE_5, "number of input events: " + intToString(inputMessages.size()) + "\n" );
	trace(TRACE_5, "interactionGraph::receivingBeforeSending(GraphNode * node): end\n");
   	return inputMessages;   // return the list of activated input messages
}


//! \fn void interactionGraph::calculateSuccStatesOutputSet(messageMultiSet output, GraphNode * node)
//! \param output the output messages that are taken from the marking
//! \param node
//! \brief calculates the set of successor states in case of an output message
void interactionGraph::calculateSuccStatesOutputSet(messageMultiSet output, GraphNode * node) {
    
	/* iterate over all states of the current node 
	 * and get rid of the output messages in the marking of the state
	 * */
	
#ifdef DEBUG
	cout << "interactionGraph::calculateSuccStatesOutputSet(messageMultiSet output, GraphNode * node): start" << endl;
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
				outputPlace = PN->getPlace(*iter);
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

//! \fn void interactionGraph::calculateSuccStatesInputReduced(messageMultiSet input, GraphNode * node)
//! \param input set of input messages
//! \param node the node for which the successor states are to be calculated
//! \brief calculates the set of successor states in case of an input message
void interactionGraph::calculateSuccStatesInputReduced(messageMultiSet input, GraphNode * node) {
	
#ifdef DEBUG
	cout << "interactionGraph::calculateSuccStatesInputReduced(char * input, GraphNode * node): start" << endl;
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
