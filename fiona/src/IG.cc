/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg               *
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
#include "GraphFormula.h"
#include "owfnTransition.h"
#include "binDecision.h"
#include "GraphNode.h"

//! \fn interactionGraph::interactionGraph(oWFN * _PN) 
//! \param _PN
//! \brief constructor
interactionGraph::interactionGraph(oWFN* _PN) :
    CommunicationGraph(_PN) {
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

    // build the IG, whether the reduced or not reduced one is built is being decided in that
    // function itself
    buildGraph(getRoot());

    computeGraphStatistics();
}


//! \fn void interactionGraph::buildGraph(GraphNode * node)
//! \param node current node of the graph
//! \brief builds up the graph recursively
void interactionGraph::buildGraph(GraphNode* currentNode) {

    // at this point, the states inside the current node node are already computed!

    trace(TRACE_1, "\n-----------------------------------------------------------------\n");
    trace(TRACE_1, "\t current node: ");
    trace(TRACE_1, currentNode->getName() + "\n");
    if (debug_level >= TRACE_2) {
        cout << "\t (" << currentNode << ")" << endl;
    }

    trace(TRACE_2, "\t number of states in node: ");
    trace(TRACE_2, intToString(currentNode->reachGraphStateSet.size()) + "\n");

    if (currentNode->getColor() == RED) {
        // this may happen due to a message bound violation in current node
        // then, function calculateReachableStatesFull sets node color RED
        trace(TRACE_3, "\t\t\t node " + currentNode->getName() + " has color RED\n");
        trace(TRACE_1, "\n-----------------------------------------------------------------\n");
        return;
    }

    setOfMessages sendingEvents;
    setOfMessages receivingEvents;

    if (options[O_CALC_REDUCED_IG]) {
    	// build up the reduced interaction graph
    	// rules applied: "combine receiving events" + "receiving before sending"
    	receivingEvents = combineReceivingEvents(currentNode, sendingEvents);
    } else {
    	// build up the interaction graph without applying reduction rules
    	getActivatedEventsComputeCNF(currentNode, sendingEvents, receivingEvents);
    }

    // collection of all activated events
    setOfMessages activatedEvents;
    
    // collect the receiving events
    for (setOfMessages::iterator iter = receivingEvents.begin(); iter != receivingEvents.end(); iter++) {
       	activatedEvents.insert(*iter);
    }
    // collect the sending events
    for (setOfMessages::iterator iter = sendingEvents.begin(); iter != sendingEvents.end(); iter++) {
    	activatedEvents.insert(*iter);
    }
    
    // priority map of all activated events
    PriorityMap pmIG;
    
    // multiset of the currently considered activated event
    // we need a multiset here because such an activated event might be the combination of more than
    // one event
    messageMultiSet mmSet;
    
    // what type of edge are we creating
    GraphEdgeType typeOfEdge;

    // create the priority of all activated events
    pmIG.fillForIG(activatedEvents, PN, currentNode->getAnnotation());
    
    // iterate over all activated events
    // in the order which is given by the PriorityMap pmIG
    while(!pmIG.emptyIG()) {
        mmSet = pmIG.popIG();

        // find out what kind of event we consider now
        // since a multiset of messages contains only one type of messages,
        // we just check the first message/event of the multiset
        if (PN->getPlace(*mmSet.begin())->getType() == INPUT) {
        	typeOfEdge = SENDING;
        } else {
        	typeOfEdge = RECEIVING;
        }
        
        // do we actually need to calculate a new node? 
        if (((options[O_MESSAGES_MAX] == false) && (options[O_EVENT_USE_MAX] == false))
            || checkMaximalEvents(mmSet, currentNode, typeOfEdge)) {
        	
        	if (typeOfEdge == SENDING) {
        		trace(TRACE_2, "\t\t\t\t    sending event: ");
        	} else {
        		trace(TRACE_2, "\t\t\t\t    receiving event: ");
            }
        	
        	trace(TRACE_2, PN->createLabel(mmSet) + "\n");
        	
            // create new GraphNode of the graph
            GraphNode* v = new GraphNode();
            
            if (typeOfEdge == SENDING) {
            	// sending event
            	calculateSuccStatesSendingEvent(mmSet, currentNode, v);
            	
            	if (v->getColor() == RED) {
            		// message bound violation occured during calculateSuccStatesSendingEvent
            		trace(TRACE_2, "\t\t\t\t    sending event: ");
                	trace(TRACE_2, PN->createLabel(mmSet));
            		trace(TRACE_2, " at node " + currentNode->getName() + " suppressed (message bound violated)\n");

                    currentNode->removeLiteralFromAnnotation(PN->createLabel(mmSet));
            		
            		numberDeletedVertices--;
            		delete v;
            		v = NULL;
            	}            	
            } else {
            	// receiving event
                calculateSuccStatesReceivingEvent(mmSet, currentNode, v);
            }

            if (v != NULL && currentNode->getColor() != RED && 
            		addGraphNode(currentNode, v, mmSet, typeOfEdge)) {
            	buildGraph(v);
            	trace(TRACE_1, "\t backtracking to node " + currentNode->getName() + "\n");
            	
            	if (v->getColor() == RED) {
            		currentNode->removeLiteralFromAnnotation(PN->createLabel(mmSet));
            	}
            }
        } else {
            trace(TRACE_2, "\t\t\t            event suppressed (max_occurence reached)\n");

            currentNode->removeLiteralFromAnnotation(PN->createLabel(mmSet));
        }


        if (currentNode->getAnnotation()->equals() == FALSE) {
            currentNode->setColor(RED);
            trace(TRACE_3, "\t\t any further event suppressed (annotation of node ");
            trace(TRACE_3, currentNode->getName() + " is unsatisfiable)\n");
            trace(TRACE_5, "\t\t formula was " + currentNode->getAnnotation()->asString());
            trace(TRACE_3, "\n");
            return;
        }
    }

    if (currentNode->getColor() != RED) {
    	currentNode->analyseNode();
    }
    trace(TRACE_5, "node analysed\n");

    trace(TRACE_1, "\t\t\t node " + currentNode->getName() + " has color " + toUpper(currentNode->getColor().toString()) + "\n");
}

//! \param sourceNode a reference to the father of toAdd (needed for implicitly adding the edge, too)
//! \param toAdd a reference to the GraphNode that is to be added to the graph
//! \param messages the label of the edge between the current GraphNode and the one to be added
//! \param type the type of the edge (SENDING, RECEIVING)
//! \brief adding a new GraphNode/ edge to the graph
//!
//! if the graph already contains nodes, we first search the graph for a node that matches the new node
//! if we did not find a node, we add the new node to the graph (here we add the new node to the
//! successor node list of the current graph and add the current node to the list of predecessor nodes
//! of the new node; after that the current GraphNode becomes to be the new node)
//!
//! if we actually found a node matching the new one, we just create an edge between the current node
//! and the node we have just found, the found one gets the current node as a predecessor node
bool interactionGraph::addGraphNode(GraphNode* sourceNode,
                                    GraphNode* toAdd,
                                    messageMultiSet messages,
                                    GraphEdgeType type) {
    trace(TRACE_5, "interactionGraph::AddGraphNode (GraphNode * sourceNode, GraphNode * toAdd, messageMultiSet messages, GraphEdgeType type) : start\n");

    if (getNumberOfNodes() == 0) { // graph contains no nodes at all
        root = toAdd; // the given node becomes the root node
        setOfSortedNodes.insert(toAdd);
       // setOfNodes.push_back(toAdd);
    } else {
        GraphNode* found = findGraphNodeInSet(toAdd); //findGraphNode(toAdd);

        string label;
        bool comma = false;

        if (found == NULL) {
            for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
                toAdd->eventsUsedInput[i] = sourceNode->eventsUsedInput[i];
            }
            
            for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
                toAdd->eventsUsedOutput[i] = sourceNode->eventsUsedOutput[i];
            }
        }

        for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
            if (comma) {
                label += ", ";
            }
            label += PN->getPlace(*iter)->getLabelForCommGraph();
            comma = true;

            unsigned int i = 0;
            if (type == RECEIVING) {
                while (i < PN->getOutputPlaceCount() && PN->getPlaceIndex(PN->getOutputPlace(i)) != *iter) {
                    i++;
                }
            } else {
                while (i < PN->getInputPlaceCount() && PN->getPlaceIndex(PN->getInputPlace(i)) != *iter) {
                    i++;
                }
            }
            if (found == 0) {
                if (type == RECEIVING) {
                    toAdd->eventsUsedOutput[i]++;
                } else {
                    toAdd->eventsUsedInput[i]++;
                }
            } else {
                if (type == RECEIVING) {
                    found->eventsUsedOutput[i]++;
                } else {
                    found->eventsUsedInput[i]++;
                }
            }
        }

        if (found == NULL) {
            trace(TRACE_1, "\n\t new successor node computed:");

            toAdd->setNumber(getNumberOfNodes());
            toAdd->setName(intToString(getNumberOfNodes()));

            GraphEdge* edgeSucc = new GraphEdge(toAdd, label);
            sourceNode->addLeavingEdge(edgeSucc);
            setOfSortedNodes.insert(toAdd);
           // setOfNodes.push_back(toAdd);
            
            trace(TRACE_5, "interactionGraph::AddGraphNode (GraphNode * sourceNode, GraphNode * toAdd, messageMultiSet messages, GraphEdgeType type) : end\n");
            return true;
        } else {
            trace(TRACE_1, "\t successor node already known: " + found->getName() + "\n");

            GraphEdge* edgeSucc = new GraphEdge(found, label);
            sourceNode->addLeavingEdge(edgeSucc);

            // Still, if that node was computed red before, the literal
            // of the edge from currentNode to the old node must be removed
            // in the annotation of currentNode.
            if (found->getColor() == RED) {
            	sourceNode->removeLiteralFromAnnotation(label);
            }
            
            delete toAdd;

            trace(TRACE_5, "interactionGraph::AddGraphNode (GraphNode * sourceNode, GraphNode * toAdd, messageMultiSet messages, GraphEdgeType type) : end\n");
            return false;
        }
    }

    return false;
}


//! \fn bool interactionGraph::checkMaximalEvents(messageMultiSet messages, GraphNode * currentNode, GraphEdgeType typeOfPlace)
//! \param messages
//! \param currentNode the node from which the input event is to be sent
//! \param typeOfPlace
//! \brief checks whether the set of input messages contains at least one input message
//! that has been sent at its maximum
bool interactionGraph::checkMaximalEvents(messageMultiSet messages,
                                          GraphNode* currentNode,
                                          GraphEdgeType typeOfPlace) {
    trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet messages, GraphNode * currentNode, bool typeOfPlace): start\n");

    map<unsigned int, int> numberOfInputMessages;
    map<unsigned int, int> numberOfOutputMessages;

    // count the messages, since there may be more than one message of a kind assigned to an edge	
    for (messageMultiSet::iterator iter = messages.begin(); iter != messages.end(); iter++) {
        if (typeOfPlace == SENDING) {
            unsigned int i = 0;
            while (i < PN->getInputPlaceCount()-1 &&
                   PN->getInputPlace(i) &&
                   PN->getPlaceIndex(PN->getInputPlace(i)) != *iter) {
                i++;
            }
            // count the occurance of this message
            numberOfInputMessages[i]++;

        } else if (typeOfPlace == RECEIVING) {
            unsigned int i = 0;
            while (i < PN->getOutputPlaceCount()-1 &&
                   PN->getOutputPlace(i) &&
                   PN->getPlaceIndex(PN->getOutputPlace(i)) != *iter) {
                i++;
            }
            // count the occurance of this message
            numberOfOutputMessages[i]++;
        }
    }

    map<unsigned int, int>::const_iterator iter;

    // now we check each message stored in the map, if its occurance violates our boundaries or not    
    if (typeOfPlace == SENDING) {
        for (iter = numberOfInputMessages.begin(); iter != numberOfInputMessages.end(); ++iter) {
            if (options[O_EVENT_USE_MAX] == true) { // max use of events set
                if (currentNode->eventsUsedInput[iter->first] + iter->second > PN->getInputPlace(iter->first)->max_occurence) {

                    // this input event shall not be sent anymore, so quit here
                    trace(TRACE_3, "maximal occurrences of event ");
                    trace(TRACE_3, PN->getInputPlace(iter->first)->name);
                    trace(TRACE_3, " reached\n");

                    trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet messages, GraphNode * currentNode, bool typeOfPlace): end\n");
                    return false;
                }
            }
        }
    } else if (typeOfPlace == RECEIVING) {
        for (iter = numberOfOutputMessages.begin(); iter != numberOfOutputMessages.end(); ++iter) {
            if (options[O_EVENT_USE_MAX] == true) { // max use of events set
                if (currentNode->eventsUsedOutput[iter->first] +iter->second> PN->getOutputPlace(iter->first)->max_occurence) {

                    // this output event shall not be received anymore, so quit here
                    trace(TRACE_3, "maximal occurrences of event ");
                    trace(TRACE_3, PN->getOutputPlace(iter->first)->name);
                    trace(TRACE_3, " reached\n");
                    trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet messages, GraphNode * currentNode, bool typeOfPlace): end\n");
                    return false;
                }
            }
        }
    }

    // everything is fine
    trace(TRACE_5, "oWFN::checkMaximalEvents(messageMultiSet messages, GraphNode * currentNode, bool typeOfPlace): end\n");
    return true;
}


//! \fn void interactionGraph::getActivatedEventsComputeCNF(GraphNode * node, setOfMessages & inputMessages, setOfMessages & outputMessages) {
//! \param node the node for which the activated input events are calculated
//! \param inputMessages the set of input messages (sending events) that are activated in the current node 
//! \param outputMessages the set of output messages (receiving events) that are activated in the current node 
//! \brief creates a list of all activated sending and receiving events (input messages and output messages) of the current node
void interactionGraph::getActivatedEventsComputeCNF(GraphNode* node,
                                                    setOfMessages& inputMessages,
                                                    setOfMessages& outputMessages) {
    trace(TRACE_5, "interactionGraph::getActivatedEventsComputeCNF(GraphNode * node): start\n");

    int i;
    StateSet::iterator iter;

    if (!options[O_CALC_ALL_STATES]) { // in case of the state reduced graph

        // as the current node has been computed a temporal set of states was created
        // this set of states includes the deadlocks as well
        for (iter = PN->setOfStatesTemp.begin(); iter != PN->setOfStatesTemp.end(); iter++) {
            // we just consider the maximal states only
            if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE) {

                // this clause's first literal
                GraphFormulaMultiaryOr* myclause = new GraphFormulaMultiaryOr();

                //literal * cl = new literal();			// create a new clause for this particular state

                (*iter)->decode(PN); // get the marking for this state

                if ((*iter)->quasiFirelist) { // delete the list of quasi enabled transitions
                    delete [] (*iter)->quasiFirelist;
                    (*iter)->quasiFirelist = NULL;
                }
                (*iter)->quasiFirelist = PN->quasiFirelist(); // get the firelist of the quasi enabled transitions

                i = 0;
                // get the activated input events
                //	cout << "state " << PN->getCurrentMarkingAsString() << " activates the input events: " << (*iter)->quasiFirelist << ", " << (*iter)->quasiFirelist[i] << endl;
                while ((*iter)->quasiFirelist && (*iter)->quasiFirelist[i]) {
                    for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin(); index
                            != (*iter)->quasiFirelist[i]->messageSet.end(); index++) {

                        messageMultiSet input; // multiset holding one input message
                        input.insert(*index);

                        inputMessages.insert(input);

                        GraphFormulaLiteral* myliteral = new GraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
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

                        GraphFormulaLiteral* myliteral = new GraphFormulaLiteral(PN->getPlace(i)->getLabelForCommGraph());
                        myclause->addSubFormula(myliteral);

                        //cl->addLiteral(PN->getPlace(i)->getLabelForCommGraph());	
                    }
                }

                // in case of a final state we add special literal "final" to the clause
                if ((*iter)->type == FINALSTATE) {
                    node->hasFinalStateInStateSet = true;

                    GraphFormulaLiteral* myliteral = new GraphFormulaLiteralFinal();
                    myclause->addSubFormula(myliteral);
                }

                node->addClause(myclause);
                //node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
            }
        }
    } else {
        // we calculate the graph with the whole set of states stored in the nodes
        // so we use the set of states that were actually stored in the node 
        for (iter = node->reachGraphStateSet.begin(); iter
                != node->reachGraphStateSet.end(); iter++) {

            if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE) { // we just consider the maximal states only

                // this clause's first literal
                GraphFormulaMultiaryOr* myclause = new GraphFormulaMultiaryOr();

                //				literal * cl = new literal();			// create a new clause for this particular state
                (*iter)->decode(PN);

                i = 0;
                // get the activated input events
                //	cout << "state " << PN->getCurrentMarkingAsString() << " activates the input events: " << (*iter)->quasiFirelist << ", " << (*iter)->quasiFirelist[i] << endl;
                while ((*iter)->quasiFirelist && (*iter)->quasiFirelist[i]) {
                    for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin(); index
                            != (*iter)->quasiFirelist[i]->messageSet.end(); index++) {

                        messageMultiSet input; // multiset holding one input message
                        input.insert(*index);

                        inputMessages.insert(input);

                        GraphFormulaLiteral* myliteral = new GraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
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

                        GraphFormulaLiteral* myliteral = new GraphFormulaLiteral(PN->getPlace(i)->getLabelForCommGraph());
                        myclause->addSubFormula(myliteral);

                        //cl->addLiteral(PN->getPlace(i)->getLabelForCommGraph());	
                    }
                }

                // in case of a final state we add special literal "final" to the clause
                if ((*iter)->type == FINALSTATE) {
                    node->hasFinalStateInStateSet = true;

                    GraphFormulaLiteral
                            * myliteral = new GraphFormulaLiteralFinal();
                    myclause->addSubFormula(myliteral);
                }

                node->addClause(myclause);
                //node->addClause(cl, (*iter)->type == FINALSTATE); 	// attach the new clause to the node
            }
        }

    }
    trace(TRACE_5, "interactionGraph::getActivatedInputEvents(GraphNode * node): end\n");

}


//! \param input (multi) set of input messages
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an input message
void interactionGraph::calculateSuccStatesSendingEvent(messageMultiSet input,
                                                GraphNode* node,
                                                GraphNode* newNode) {
    trace(TRACE_5, "interactionGraph::calculateSuccStatesInput(messageMultiSet input, GraphNode * node, GraphNode * newNode) : start\n");

    PN->setOfStatesTemp.clear();
    PN->visitedStates.clear();

    /*if (TRACE_2 <= debug_level) {
        for (messageMultiSet::iterator iter1 = input.begin();
             iter1 != input.end(); iter1++) {
            trace(TRACE_2, PN->getPlace(*iter1)->name);
            trace(TRACE_2, " ");
        }
        trace(TRACE_2, "\n");
    }
*/
    for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter
            != node->reachGraphStateSet.end(); iter++) {

        (*iter)->decode(PN);

        // test for each marking of current node if message bound k reached
        // then supress new sending event
        if (options[O_MESSAGES_MAX] == true) { // k-message-bounded set
            // iterate over the set of input messages
            for (messageMultiSet::iterator iter = input.begin(); iter
                    != input.end(); iter++) {
                if (PN->CurrentMarking[PN->getPlaceIndex(PN->getPlace(*iter))] == messages_manual) {
                    // adding input message to state already using full message bound
                    trace(TRACE_3, "\t\t\t\t\t adding input event would cause message bound violation\n");
                    trace(TRACE_3, PN->getPlace(*iter)->name);
                    trace(TRACE_5, "interactionGraph::calculateSuccStatesInput(messageMultiSet input, GraphNode * node) : end\n");
                    newNode->setColor(RED);
                    return;
                }
            }
        }

        PN->addInputMessage(input); // add the input message to the current marking
        if (options[O_CALC_ALL_STATES]) {
            PN->calculateReachableStatesFull(newNode); // calc the reachable states from that marking
        } else {
            PN->calculateReachableStatesInputEvent(newNode); // calc the reachable states from that marking
        }
        if (newNode->getColor() == RED) {
            // a message bound violation occured during computation of reachability graph
            trace(TRACE_5, "interactionGraph::calculateSuccStatesInput(messageMultiSet input, GraphNode * node) : end\n");
            return;
        }
    }

    trace(TRACE_5, "IG::calculateSuccStatesInput(messageMultiSet input, GraphNode * node, GraphNode * newNode) : end\n");
    return;
}


//! \param receivingEvent the output messages that are taken from the marking
//! \param node the node for which the successor states are to be calculated
//! \param newNode the new node where the new states go into
//! \brief calculates the set of successor states in case of an output message
void interactionGraph::calculateSuccStatesReceivingEvent(messageMultiSet receivingEvent,
                                                 GraphNode* node,
                                                 GraphNode* newNode) {
    trace(TRACE_5, "interactionGraph::calculateSuccStatesOutput(messageMultiSet output, GraphNode * node, GraphNode * newNode) : start\n");

/*    if (TRACE_2 <= debug_level) {
        for (messageMultiSet::iterator iter1 = output.begin(); iter1
                != output.end(); iter1++) {
            trace(TRACE_2, PN->getPlace(*iter1)->name);
            trace(TRACE_2, " ");
        }
        trace(TRACE_2, "\n");
    }
*/
    if (options[O_CALC_ALL_STATES]) {
        for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter
                != node->reachGraphStateSet.end(); iter++) {
            (*iter)->decode(PN);
            if (PN->removeOutputMessage(receivingEvent)) { // remove the output message from the current marking
                PN->calculateReachableStatesFull(newNode); // calc the reachable states from that marking
            }
        }
    } else {
        PN->setOfStatesTemp.clear();
        PN->visitedStates.clear();

        StateSet stateSet;
        // stateSet.clear();

        for (StateSet::iterator iter = node->reachGraphStateSet.begin(); iter
                != node->reachGraphStateSet.end(); iter++) {

            (*iter)->decode(PN);
            // calculate temporary state set with the help of stubborn set method
            // TODO: Fix this memory leak.
            // The following method sets tempBinDecision to NULL before
            // filling tempBinDecision anew without deleting the old one.
            // Consequently, some binDecisions become unreachable and
            // cannot be deleted at the end of this method. This produces a
            // memory leak. Not setting tempBinDecision to NULL in the
            // following method call obviously fixes this memory leak. But this
            // would cause unintended behaviour, wouldn't it? I figure that
            // each State in stateSet needs a separate tempBinDecision. Then,
            // each of those tempBinDecision must be kept alive until the
            // following for loop is completed because otherwise the just
            // calculated States would deleted by the binDecision destructor
            // causing a segmentation fault while trying to call decode() on
            // one those deleted states in the following for loop.
            PN->calculateReachableStates(stateSet, receivingEvent, newNode);
        }

        for (StateSet::iterator iter2 = stateSet.begin(); iter2
                != stateSet.end(); iter2++) {
            (*iter2)->decode(PN); // get the marking of the state

            if (PN->removeOutputMessage(receivingEvent)) { // remove the output message from the current marking
                PN->calculateReachableStatesOutputEvent(newNode); // calc the reachable states from that marking
            }
        }
        // binDeleteAll(PN->tempBinDecision);
        delete PN->tempBinDecision;
        PN->tempBinDecision = NULL;
    }

    trace(TRACE_5, "interactionGraph::calculateSuccStatesOutput(messageMultiSet output, GraphNode * node, GraphNode * newNode) : end\n");
}


/////////////////////////////////////////////////////////////////////////////////////////////
// reduction rules
/////////////////////////////////////////////////////////////////////////////////////////////

//! \fn setOfMessages interactionGraph::combineReceivingEvents(GraphNode * node, setOfMessages & inputMessages)
//! \param node the node for which the activated receiving and sending events are calculated
//! \param sendingEvents set of sending events that are activated in the current node
//! \brief creates a list of all receiving events of the current node and creates the set of
//!			sending events
//!			applies the reduction rules: "combine receiving events" and "receiving before sending"
setOfMessages interactionGraph::combineReceivingEvents(GraphNode* node,
                                                       setOfMessages& sendingEvents) {

    trace(TRACE_5, "interactionGraph::combineReceivingEvents(GraphNode * node): start\n");

    // remember those states that activate a receiving event
    std::vector<StateSet::iterator> statesVector; 

    setOfMessages listOfReceivingEvents; 	// list of all the receiving events in this node
    										// is a set of multisets

    messageMultiSet receivingEvent; // multiset of the receiving event of the current state

    StateSet::iterator iter;	// iterator over all states of the current node

    bool found = false;
    bool skip = false;

    if (!options[O_CALC_ALL_STATES]) { // in case of the state reduced graph

        for (iter = PN->setOfStatesTemp.begin(); iter
                != PN->setOfStatesTemp.end(); iter++) {

            if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE) { // we just consider the maximal states only

                unsigned int i = 0;

                // this clause's first literal
                GraphFormulaMultiaryOr* myclause = new GraphFormulaMultiaryOr();

                // "receiving before sending" reduction rule
                while (!stateActivatesOutputEvents(*iter) &&(*iter)->quasiFirelist&&(*iter)->quasiFirelist[i]) {

                    for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin(); index
                            != (*iter)->quasiFirelist[i]->messageSet.end(); index++) {

                        messageMultiSet input; // multiset holding one input message
                        input.insert(*index);

                        sendingEvents.insert(input);

                        GraphFormulaLiteral
                                * myliteral = new GraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
                        myclause->addSubFormula(myliteral);
                    }
                    i++;
                }

                messageMultiSet outputMessages; // multiset of all input messages of the current state

                (*iter)->decode(PN);

                for (i = 0; i < PN->getPlaceCount(); i++) {

                    if (PN->getPlace(i)->type == OUTPUT && PN->CurrentMarking[i] > 0) {
                        for (unsigned int z = 0; z < PN->CurrentMarking[i]; z++) {
                            outputMessages.insert(i);
                        }

                        found = true;
                    }
                }

                bool subset = false;
                bool supset = false;

                string label;

                if (found) {

                    for (setOfMessages::iterator
                            iter1 = listOfReceivingEvents.begin(); iter1
                            != listOfReceivingEvents.end(); iter1++) {

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
                        for (setOfMessages::iterator
                                iter1 = listOfReceivingEvents.begin(); iter1
                                != listOfReceivingEvents.end(); iter1++) {

                            supset = false;

                            for (messageMultiSet::iterator
                                    tmp2 = outputMessages.begin(); tmp2
                                    != outputMessages.end(); tmp2++) {
                                if (outputMessages.count(*tmp2) <= (*iter1).count(*tmp2)) {
                                    supset = true;
                                } else {
                                    supset = false;
                                    break;
                                }
                            }
                            if (supset) {
                            	
                            	node->getAnnotation()->removeLiteralForReal(PN->createLabel(*iter1));
                            	
                                listOfReceivingEvents.erase(iter1);
                                label = PN->createLabel(outputMessages);
                                
                                break;
                            }
                        }

                        if (supset) {
                            GraphFormulaLiteral
                                    * myliteral = new GraphFormulaLiteral(label);
                            myclause->addSubFormula(myliteral);
                            listOfReceivingEvents.insert(outputMessages);
                        }
                    } else {
                        GraphFormulaLiteral
                                * myliteral = new GraphFormulaLiteral(label);
                        myclause->addSubFormula(myliteral);
                    }

                    if (!subset && !supset) {
                        listOfReceivingEvents.insert(outputMessages);

                        GraphFormulaLiteral* myliteral = new GraphFormulaLiteral(PN->createLabel(outputMessages));
                        myclause->addSubFormula(myliteral);
                    }

                    found = false;
                    skip = false;
                }

                // in case of a final state we add special literal "final" to the clause
                if ((*iter)->type == FINALSTATE) {
                    node->hasFinalStateInStateSet = true;

                    GraphFormulaLiteral* myliteral = new GraphFormulaLiteralFinal();
                    myclause->addSubFormula(myliteral);
                }

                node->addClause(myclause);
            }

        }
    } else {
        for (iter = node->reachGraphStateSet.begin(); iter
                != node->reachGraphStateSet.end(); iter++) {

            if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE) { // we just consider the maximal states only

                unsigned int i = 0;
                // this clause's first literal
                GraphFormulaMultiaryOr* myclause = new GraphFormulaMultiaryOr();

                // "receiving before sending" reduction rule
                while (!stateActivatesOutputEvents(*iter) &&
                			(*iter)->quasiFirelist &&
                			(*iter)->quasiFirelist[i]) {

                    for (std::set<unsigned int>::iterator index = (*iter)->quasiFirelist[i]->messageSet.begin(); index
                            != (*iter)->quasiFirelist[i]->messageSet.end(); index++) {

                        messageMultiSet input; // multiset holding one input message
                        input.insert(*index);

                        sendingEvents.insert(input);

                        GraphFormulaLiteral* myliteral = new GraphFormulaLiteral(PN->getPlace(*index)->getLabelForCommGraph());
                        myclause->addSubFormula(myliteral);
                    }
                    i++;
                }

                messageMultiSet outputMessages; // multiset of all output messages of the current state

                (*iter)->decode(PN);

                for (i = 0; i < PN->getPlaceCount(); i++) {

                    if (PN->getPlace(i)->type == OUTPUT && PN->CurrentMarking[i] > 0) {
                        for (unsigned int z = 0; z < PN->CurrentMarking[i]; z++) {
                            outputMessages.insert(i);
                        }

                        found = true;
                    }
                }

                bool subset = false;
                bool supset = false;

                string label;

                if (found) {

                    for (setOfMessages::iterator
                            iter1 = listOfReceivingEvents.begin(); 
                            iter1 != listOfReceivingEvents.end(); iter1++) {

                        subset = false;

                        for (messageMultiSet::iterator tmp2 = (*iter1).begin(); 
                        										tmp2 != (*iter1).end(); tmp2++) {
                        											
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
                        for (setOfMessages::iterator
                                iter2 = listOfReceivingEvents.begin(); 
                                iter2 != listOfReceivingEvents.end(); iter2++) {

                            supset = false;

                            for (messageMultiSet::iterator
                                    tmp2 = outputMessages.begin(); tmp2
                                    != outputMessages.end(); tmp2++) {
                                    	
                                if (outputMessages.count(*tmp2) <= (*iter2).count(*tmp2)) {
                                    supset = true;
                                } else {
                                    supset = false;
                                    break;
                                }
                            }
                            if (supset) {
                                node->getAnnotation()->removeLiteralForReal(PN->createLabel(*iter2));
                                
                                listOfReceivingEvents.erase(iter2);
                                label = PN->createLabel(outputMessages);
                                break;
                            }
                        }

                        if (supset) {
                            GraphFormulaLiteral
                                    * myliteral = new GraphFormulaLiteral(label);
                            myclause->addSubFormula(myliteral);
                            listOfReceivingEvents.insert(outputMessages);
                        }
                    } else {
                        GraphFormulaLiteral
                                * myliteral = new GraphFormulaLiteral(label);
                        myclause->addSubFormula(myliteral);
                    }

                    if (!subset && !supset) {
                        listOfReceivingEvents.insert(outputMessages);
                        GraphFormulaLiteral
                                * myliteral = new GraphFormulaLiteral(PN->createLabel(outputMessages));
                        myclause->addSubFormula(myliteral);
                    }

                    found = false;
                    skip = false;
                }

                // in case of a final state we add special literal "final" to the clause
                if ((*iter)->type == FINALSTATE) {
                    node->hasFinalStateInStateSet = true;

                    GraphFormulaLiteral
                            * myliteral = new GraphFormulaLiteralFinal();
                    myclause->addSubFormula(myliteral);
                }

                node->addClause(myclause);
            }

        }
    }

    trace(TRACE_5, "interactionGraph::combineReceivingEvents(GraphNode * node): end\n");

    /* check the set of output-messages for containing subsets */
    /* e.g. the set contains [a, b] and [a, b, c] */
    /* [a, b] is subset of [a, b, c], therefore the set [a, b, c] is removed */

    return listOfReceivingEvents;
}
