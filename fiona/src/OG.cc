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
 * \file    OG.cc
 *
 * \brief   functions for Operating Guidelines (OG)
 *
 * \author  responsible: Peter Massuthe <massuthe@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "OG.h"
#include "state.h"
#include "options.h"
#include "debug.h"
#include "successorNodeList.h"
#include "owfn.h"
#include "vertex.h"


//! \fn operatingGuidelines::operatingGuidelines(oWFN * _PN)
//! \param _PN
//! \brief constructor
operatingGuidelines::operatingGuidelines(oWFN * _PN) : communicationGraph(_PN) {
	
	 if (options[O_BDD] == true || options[O_OTF]) {
		unsigned int nbrLabels = PN->getInputPlaceCount() + PN->getOutputPlaceCount();
		bdd = new BddRepresentation(nbrLabels, (Cudd_ReorderingType)bdd_reordermethod);
	 }
}

//! \fn operatingGuidelines::~operatingGuidelines() 
//! \brief destructor
operatingGuidelines::~operatingGuidelines() {

	  if (options[O_BDD] == true) {
		delete bdd;
	  }

}

//! \fn void operatingGuidelines::buildGraph(vertex * currentNode, double progress_plus)
//! \param currentNode current node of the graph
//! \param progress_plus the additional progress when the subgraph starting at this node is finished 
//! \brief builds up the graph recursively
void operatingGuidelines::buildGraph(vertex * currentNode, double progress_plus) {
		
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

		addProgress(progress_plus);
		printProgress();

		return;
	}

	// get the annotation of the node (CNF) as formula
	computeCNF(currentNode);
	computeCNFformula(currentNode);

	trace(TRACE_1, "=================================================================\n");

//	double your_progress = progress_plus * (1 / double(PN->getInputPlaceCount() + PN->getOutputPlaceCnt()));
	double your_progress = progress_plus * (1 / double(PN->getInputPlaceCount()));

	unsigned int i = 0;

	// iterate over all elements of inputSet of the oWFN
	trace(TRACE_2, "\t\t\t iterating over inputSet of the oWFN\n");
	while (i < PN->getInputPlaceCount()) {

		trace(TRACE_2, "\t\t\t\t    sending event: !");
		trace(TRACE_2, string(PN->getInputPlace(i)->name) + "\n");
		
		if (currentNode->eventsUsed[i] < PN->getInputPlace(i)->max_occurence
            || (options[O_EVENT_USE_MAX] == false)) {

			vertex * v = new vertex(PN->getInputPlaceCount() + PN->getOutputPlaceCount());	// create new vertex of the graph
			currentVertex = currentNode;
			
			trace(TRACE_5, "calculating successor states\n");

			calculateSuccStatesInput(PN->getInputPlace(i)->index, currentNode, v);

			if (v->getColor() == RED) {
				// message bound violation occured during calculateSuccStatesInput
				trace(TRACE_2, "\t\t\t\t\t    sending event: !");
				trace(TRACE_2, PN->getInputPlace(i)->name);
				trace(TRACE_2, " suppressed (message bound violated)\n");

				numberDeletedVertices--;

				addProgress(your_progress);
				printProgress();
				
				delete v;
			} else {
				// was the new node computed before? 
				vertex * found = findVertexInSet(v);
			
				if (found == NULL) {
					// node was new, hence going down with sending event...
					AddVertex(v, i, sending, true);
					buildGraph(v, your_progress);
	
					if (v->getColor() == RED) {
						currentNode->removeLiteralFromFormula(i, sending);
					}

					trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
					actualDepth--;
				} else {
					// In case the successor node was already known, an edge to the old
					// node is drawn by function AddVertex.
					AddVertex(found, i, sending, false);

					// Still, if that node was computed red before, the literal
					// of the edge from currentNode to the old node must be removed
					// in the annotation of currentNode.
					if (found->getColor() == RED) {
						currentNode->removeLiteralFromFormula(i, sending);
					}
					delete v;

					addProgress(your_progress);
					printProgress();
				}
			}
		} else {
			trace(TRACE_2, "\t\t\t\t\t    sending event: !");
			trace(TRACE_2, string(PN->getInputPlace(i)->name));
			trace(TRACE_2, " suppressed (max_occurence reached)\n");

			currentNode->removeLiteralFromFormula(i, sending);

			addProgress(your_progress);
			printProgress();
		}
		i++;
	}


	/**
	@todo This check, whether a node's annotation is satisfiable, should be
	used after the new and smarter algorithm for choosing the next
	considered event is implemented.
	// early checking if the node's annotation cannot be made true
	CommGraphFormulaAssignment* testAssignment = currentNode->getAssignment();

	for (unsigned int j = 0; j < PN->getInputPlaceCount(); j++) {
		//cout << PN->getInputPlace(j)->name << endl;
		testAssignment->setToTrue('!' + PN->getInputPlace(j)->name);
	}

	if (currentNode->getCNF_formula()->value(*testAssignment) == false) {
		assert(false);
		cout << "node " << currentNode->getNumber() << " went red early. formula was " << currentNode->getCNF_formula()->asString() << endl;
	}

	delete testAssignment;
	*/

	i = 0;
	// iterate over all elements of outputSet of the oWFN
	trace(TRACE_2, "\t\t\t iterating over outputSet of the oWFN\n");
	while (i < PN->getOutputPlaceCount()) {

		trace(TRACE_2, "\t\t\t\t  receiving event: ?");
		trace(TRACE_2, string(PN->getOutputPlace(i)->name) + "\n");
	    
		if (currentNode->eventsUsed[i + PN->getInputPlaceCount()] < PN->getOutputPlace(i)->max_occurence
            || (options[O_EVENT_USE_MAX] == false)) {
				
			vertex * v = new vertex(PN->getInputPlaceCount() + PN->getOutputPlaceCount());	// create new vertex of the graph
			currentVertex = currentNode;
			
			calculateSuccStatesOutput(PN->getOutputPlace(i)->index, currentNode, v);
			
			// was the new node computed before? 
			vertex * found = findVertexInSet(v);
			
			if (found == NULL) {
				// node was new, hence going down with receiving event...
				AddVertex(v, i, receiving, true);
				// buildGraph(v, your_progress);
				buildGraph(v, 0);
				
				if (v->getColor() == RED) {
					currentNode->removeLiteralFromFormula(i, receiving);
				}

				trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
				actualDepth--;
			} else {
				// In case the successor node v was already known, an edge to the old
				// node is drawn by function AddVertex.
				AddVertex(found, i, receiving, false);

				// Still, if that node was computed red before, the literal
				// of the edge from currentNode to the old node must be removed in the
				// annotation of currentNode.
				if (found->getColor() == RED) {
					currentNode->removeLiteralFromFormula(i, receiving);
				}
				delete v;

//				addProgress(your_progress);
//				printProgress();
			}
		} else {
			trace(TRACE_2, "\t\t\t\t\t  receiving event: ?");
			trace(TRACE_2, string(PN->getOutputPlace(i)->name));
			trace(TRACE_2, " suppressed (max_occurence reached)\n");

			currentNode->removeLiteralFromFormula(i, receiving);

//			addProgress(your_progress);
//			printProgress();
		}
		i++;
	}

	// finished iterating over successors
	trace(TRACE_2, "\t\t\t no events left...\n");

	analyseNode(currentNode);

	string color = "";
	if (currentNode->getColor() == RED) {
		color = "RED";
	} else {
		color = "BLUE";
	}
	trace(TRACE_3, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + color);
	trace(TRACE_3, " via formula " + currentNode->getCNF_formula()->asString() + "\n");	
		
	if (options[O_OTF]) {
		//cout << "currentNode: " << currentNode->getNumber() << endl;	
		bdd->addOrDeleteLeavingEdges(currentNode);
	}
}


//! \fn void operatingGuidelines::computeCNF(vertex * node)
//! \param node the node for which the annotation is calculated
//! \brief calculates the annotation (CNF) for the node
void operatingGuidelines::computeCNF(vertex* node) {
	
	trace(TRACE_5, "operatingGuidelines::computeCNF(vertex * node): start\n");
	
	// initially, the annoation is empty (and therefore equivalent to true)
	assert(node->getAnnotation() == NULL);
	
	StateSet::iterator iter;			// iterator over the states of the node
	
	if (options[O_CALC_ALL_STATES]) {
	// NO state reduction
		
		// iterate over all states of the node
		for (iter = node->reachGraphStateSet.begin();
			 iter != node->reachGraphStateSet.end(); iter++) {
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {
				// we just consider the maximal states only
				
				// get the marking of this state
				(*iter)->decodeShowOnly(PN);

				// this clause's first literal
				literal * myFirstLiteral = new literal();

				// get all input events
				for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
					myFirstLiteral->addLiteral(PN->getInputPlace(i)->name);
				}
				
				// get all activated output events
				for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
					if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
						myFirstLiteral->addLiteral(PN->getOutputPlace(i)->name);
					}
				}

				node->addClause(myFirstLiteral, (*iter)->type == FINALSTATE);
			}
		}
	} else {
	// WITH state reduction

		// iterate over all states of the node
		for (iter = PN->setOfStatesTemp.begin();
			 iter != PN->setOfStatesTemp.end(); iter++) {
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE) {
				// we just consider the maximal states only
				
				// get the marking of this state
				(*iter)->decodeShowOnly(PN);
							
				// this clause's first literal
				literal * myFirstLiteral = new literal();
				
				// get all the input events
				for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
					myFirstLiteral->addLiteral(PN->getInputPlace(i)->name);
				}

				// get the activated output events
				for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
					if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
						myFirstLiteral->addLiteral(PN->getOutputPlace(i)->name);
					}
				}

				node->addClause(myFirstLiteral, (*iter)->type == FINALSTATE);
			}
		}
	}

	trace(TRACE_5, "operatingGuidelines::computeCNF(vertex * node): end\n");
}


//! \fn void operatingGuidelines::computeCNF(vertex * node)
//! \param node the node for which the annotation is calculated
//! \brief calculates the annotation (CNF) for the node
void operatingGuidelines::computeCNFformula(vertex* node) {
	
	trace(TRACE_5, "operatingGuidelines::computeCNF(vertex * node): start\n");
	
	// initially, the annoation is empty (and therefore equivalent to true)
	assert(node->getCNF_formula()->asString() == "true");
	
	StateSet::iterator iter;			// iterator over the states of the node
	
	if (options[O_CALC_ALL_STATES]) {
	// NO state reduction
		
		// iterate over all states of the node
		for (iter = node->reachGraphStateSet.begin();
			 iter != node->reachGraphStateSet.end(); iter++) {
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE)  {
				// we just consider the maximal states only
				
				// get the marking of this state
				(*iter)->decodeShowOnly(PN);
							
				// this clause's first literal
				CommGraphFormulaMultiaryOr* myclause = new CommGraphFormulaMultiaryOr();

				// in case of a final state we add special literal "final" to the clause
				if ((*iter)->type == FINALSTATE) {

					node->hasFinalStateInStateSet = true;
					
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteralFinal();
					myclause->addSubFormula(myliteral);
				}
				
				// get all input events
				for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral('!' + PN->getInputPlace(i)->name);
					myclause->addSubFormula(myliteral);
				}
				
				// get all activated output events
				for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
					if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral('?' + PN->getOutputPlace(i)->name);
						myclause->addSubFormula(myliteral);
					}
				}

				node->addClause(myclause);
				
				// TODO: an dieser stelle prüfen, ob myclause false ist -> dann knoten rot machen
			}
		}
	} else {
	// WITH state reduction

		// iterate over all states of the node
		for (iter = PN->setOfStatesTemp.begin();
			 iter != PN->setOfStatesTemp.end(); iter++) {
			if ((*iter)->type == DEADLOCK || (*iter)->type == FINALSTATE) {
				// we just consider the maximal states only
				
				// get the marking of this state
				(*iter)->decodeShowOnly(PN);
							
				// this clause's first literal
				CommGraphFormulaMultiaryOr* myclause = new CommGraphFormulaMultiaryOr();
				
				// in case of a final state we add special literal "final" to the clause
				if ((*iter)->type == FINALSTATE) {
					node->hasFinalStateInStateSet = true;

					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteralFinal();
					myclause->addSubFormula(myliteral);
				}
				
				// get all the input events
				for (unsigned int i = 0; i < PN->getInputPlaceCount(); i++) {
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral('!' + PN->getInputPlace(i)->name);
					myclause->addSubFormula(myliteral);
				}

				// get the activated output events
				for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
					if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral('?' + PN->getOutputPlace(i)->name);
						myclause->addSubFormula(myliteral);
					}
				}

				node->addClause(myclause);
			}
		}
	}

	trace(TRACE_5, "operatingGuidelines::computeCNF(vertex * node): end\n");
}


void operatingGuidelines::convertToBdd() {
	trace(TRACE_5, "operatingGuidelines::convertToBdd(): start\n");
	
	vertex * tmp = root;
    bool visitedNodes[numberOfNodes];

    for (unsigned int i = 0; i < numberOfNodes; i++) {
        visitedNodes[i] = 0;
    }
   
    //unsigned int nbrLabels = PN->getInputPlaceCount() + PN->getOutputPlaceCount();
    this->bdd->convertRootNode(root);
    this->bdd->generateRepresentation(tmp, visitedNodes);
    this->bdd->reorder((Cudd_ReorderingType)bdd_reordermethod);
    trace(TRACE_5,"operatingGuidelines::convertToBdd(): end\n");
}


void operatingGuidelines::printOGFile() const {
    string ogFilename = netfile;
    if (options[O_CALC_ALL_STATES]) {
        ogFilename += ".a";
    }
    ogFilename += ".og";
    fstream ogFile(ogFilename.c_str(), ios_base::out | ios_base::trunc);

    bool visitedNodes[numberOfNodes];
    for (unsigned int i = 0; i < numberOfNodes; ++i) {
        visitedNodes[i] = false;
    }

    ogFile << "NODES" << endl;
    printNodesToOGFile(root, ogFile, visitedNodes);
    ogFile << ';' << endl << endl;

    ogFile << "INITIALNODE" << endl;
    ogFile << "  " << NodeNameForOG(root) << ';' << endl << endl;

    for (unsigned int i = 0; i < numberOfNodes; ++i) {
        visitedNodes[i] = false;
    }
    ogFile << "TRANSITIONS" << endl;
    printTransitionsToOGFile(root, ogFile, visitedNodes);
    ogFile << ';' << endl;

    ogFile.close();
}


void operatingGuidelines::printNodesToOGFile(vertex * v, fstream& os,
    bool visitedNodes[]) const {

    if (v == NULL)
        return;

    // print node name (separated by comma to previous node if needed)
    if (v != root) {
        os << ',' << endl;
    }
    os << "  " << NodeNameForOG(v);

    // print node annotation
//    os << " : " << v->getCNFString();
    os << " : " << v->getCNF_formula()->asString();

    // mark current node as visited
    visitedNodes[v->getNumber()] = true;

    // recursively process successor nodes that have not been visited yet
    graphEdge* edge;
    v->resetIteratingSuccNodes();
    while ((edge = v->getNextSuccEdge()) != NULL) {
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


void operatingGuidelines::printTransitionsToOGFile(vertex * v, fstream& os,
    bool visitedNodes[]) const {

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
    while ((edge = v->getNextSuccEdge()) != NULL) {
        vertex* vNext = edge->getNode();

        // only output blue nodes
        if (vNext->getColor() != BLUE)
            continue;

        // output transition (separated by comma to previous transition if
        // needed)
        if (firstTransitionSeen) {
            os << ',' << endl;
        } else {
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


string operatingGuidelines::NodeNameForOG(const vertex* v) const {
    assert(v != NULL);
    return intToString(v->getNumber());
}

