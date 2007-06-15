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
#include "GraphNode.h"


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


//! \fn void operatingGuidelines::buildGraph(GraphNode * currentNode, double progress_plus)
//! \param currentNode current node of the graph
//! \param progress_plus the additional progress when the subgraph starting at this node is finished 
//! \brief builds up the graph recursively
void operatingGuidelines::buildGraph(GraphNode * currentNode, double progress_plus) {

    // currentNode is the root of the currently considered subgraph
    // at this point, the states inside currentNode are already computed!

	trace(TRACE_1, "\n=================================================================\n");
	trace(TRACE_1, "\t current node: ");
	trace(TRACE_1, intToString(currentNode->getNumber()) + "\n");
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

	trace(TRACE_1, "=================================================================\n");

//	double your_progress = progress_plus * (1 / double(PN->getInputPlaceCount() + PN->getOutputPlaceCnt()));
	double your_progress = progress_plus * (1 / double(PN->getInputPlaceCount()));

	oWFN::Places_t::size_type i = 0;

	// iterate over all elements of inputSet of the oWFN
	trace(TRACE_2, "\t\t\t iterating over inputSet of the oWFN\n");
	while (i < PN->getInputPlaceCount()) {

		trace(TRACE_2, "\t\t\t\t    sending event: !");
		trace(TRACE_2, string(PN->getInputPlace(i)->name) + "\n");

		if (currentNode->eventsUsed[i] < PN->getInputPlace(i)->max_occurence
            || (options[O_EVENT_USE_MAX] == false)) {

			GraphNode* v = new GraphNode(PN->getInputPlaceCount() + PN->getOutputPlaceCount());	// create new GraphNode of the graph

			trace(TRACE_5, "calculating successor states\n");

			calculateSuccStatesInput(PN->getInputPlace(i)->index, currentNode, v);

			if (v->getColor() == RED) {
				// message bound violation occured during calculateSuccStatesInput
				trace(TRACE_2, "\t\t\t\t\t    sending event: !");
				trace(TRACE_2, PN->getInputPlace(i)->name);
				trace(TRACE_2, " suppressed (message bound violated)\n");

                currentNode->removeLiteralFromFormula(i, SENDING);

				addProgress(your_progress);
				printProgress();

                numberDeletedVertices--;      // elsewise deletion of v is counted twice
				delete v;
			} else {
				// was the new node v computed before? 
                GraphNode* found = findGraphNodeInSet(v);
                
                if (found == NULL) {
                    // node v is new, so the node as well as the edge to it is added 
                    AddGraphNode(currentNode, v, i, SENDING);

                    // going down with sending event...
                    buildGraph(v, your_progress);

                    trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
                    if (v->getColor() == RED) {
                        currentNode->removeLiteralFromFormula(i, SENDING);
                    }
				} else {
                    // node was computed before, so only add a new edge to the old node
                    trace(TRACE_1, "\t computed successor node already known: " + intToString(found->getNumber()) + "\n");

                    // draw a new SENDING edge to the old node
                    string edgeLabel = PN->getInputPlace(i)->getLabelForCommGraph();
                    GraphEdge* newEdge = new GraphEdge(found, edgeLabel);
                    currentNode->addSuccessorNode(newEdge);

					// Still, if that node was computed red before, the literal
					// of the edge from currentNode to the old node must be removed
					// in the annotation of currentNode.
					if (found->getColor() == RED) {
						currentNode->removeLiteralFromFormula(i, SENDING);
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

            currentNode->removeLiteralFromFormula(i, SENDING);

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
		testAssignment->setToTrue(PN->getInputPlace(j)->getLabelForCommGraph());
	}

	if (currentNode->getCNF_formula()->value(*testAssignment) == false) {
		assert(false);
		cout << "node " << currentNode->getNumber() << " went red early. formula was " << currentNode->getCNF_formula()->asString() << endl;
	}

	delete testAssignment;
	*/

	i = 0;
	// iterate over all output places of the oWFN (receiving events in OG)
	trace(TRACE_2, "\t\t\t iterating over output places of the oWFN\n");
	while (i < PN->getOutputPlaceCount()) {

		trace(TRACE_2, "\t\t\t\t  receiving event: ?");
		trace(TRACE_2, string(PN->getOutputPlace(i)->name) + "\n");

		if (currentNode->eventsUsed[i + PN->getInputPlaceCount()] < PN->getOutputPlace(i)->max_occurence
            || (options[O_EVENT_USE_MAX] == false)) {

			GraphNode* v = new GraphNode(PN->getInputPlaceCount() + PN->getOutputPlaceCount());	// create new GraphNode of the graph
			calculateSuccStatesOutput(PN->getOutputPlace(i)->index, currentNode, v);

			// was the new node computed before? 
			GraphNode* found = findGraphNodeInSet(v);
			
			if (found == NULL) {
                // node v is new, so the node as well as the edge to it is added
                AddGraphNode(currentNode, v, i, RECEIVING);

                // going down with receiving event...
				// buildGraph(v, your_progress);
				buildGraph(v, 0);
				
                trace(TRACE_1, "\t\t backtracking to node " + intToString(currentNode->getNumber()) + "\n");
				if (v->getColor() == RED) {
					currentNode->removeLiteralFromFormula(i, RECEIVING);
				}
			} else {
                // node was computed before, so only add a new edge to the old node
                trace(TRACE_1, "\t computed successor node already known: " + intToString(found->getNumber()) + "\n");

                // draw a new RECEIVING edge to the old node
                string edgeLabel = PN->getOutputPlace(i)->getLabelForCommGraph();
                GraphEdge* newEdge = new GraphEdge(found, edgeLabel);
                currentNode->addSuccessorNode(newEdge);

				// Still, if that node was computed red before, the literal
				// of the edge from currentNode to the old node must be removed in the
				// annotation of currentNode.
				if (found->getColor() == RED) {
					currentNode->removeLiteralFromFormula(i, RECEIVING);
				}
				delete v;

//				addProgress(your_progress);
//				printProgress();
			}
		} else {
			trace(TRACE_2, "\t\t\t\t\t  receiving event: ?");
			trace(TRACE_2, string(PN->getOutputPlace(i)->name));
			trace(TRACE_2, " suppressed (max_occurence reached)\n");

			currentNode->removeLiteralFromFormula(i, RECEIVING);

//			addProgress(your_progress);
//			printProgress();
		}
		i++;
    }

    // finished iterating over successors
    trace(TRACE_2, "\t\t\t no events left...\n");

    analyseNode(currentNode);

	trace(TRACE_3, "\t\t\t node " + intToString(currentNode->getNumber()) + " has color " + toUpper(currentNode->getColor().toString()));
	trace(TRACE_3, " via formula " + currentNode->getCNF_formula()->asString() + "\n");

	if (options[O_OTF]) {
		//cout << "currentNode: " << currentNode->getNumber() << endl;	
		bdd->addOrDeleteLeavingEdges(currentNode);
	}
}


void operatingGuidelines::computeCNF(GraphNode* node) const {
	
    trace(TRACE_5, "operatingGuidelines::computeCNF(GraphNode * node): start\n");
	
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
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getInputPlace(i)->getLabelForCommGraph());
					myclause->addSubFormula(myliteral);
				}

				// get all activated output events
				for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
					if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getOutputPlace(i)->getLabelForCommGraph());
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
					CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getInputPlace(i)->getLabelForCommGraph());
					myclause->addSubFormula(myliteral);
				}

				// get the activated output events
				for (unsigned int i = 0; i < PN->getOutputPlaceCount(); i++) {
					if (PN->CurrentMarking[PN->getOutputPlace(i)->index] > 0) {
						CommGraphFormulaLiteral* myliteral = new CommGraphFormulaLiteral(PN->getOutputPlace(i)->getLabelForCommGraph());
						myclause->addSubFormula(myliteral);
					}
				}

				node->addClause(myclause);
			}
		}
	}

	trace(TRACE_5, "operatingGuidelines::computeCNF(GraphNode * node): end\n");
}


//! \fn void operatingGuidelines::convertToBdd()
//! \brief converts an OG into its BDD representation
void operatingGuidelines::convertToBdd() {
	trace(TRACE_5, "operatingGuidelines::convertToBdd(): start\n");
	
    bool visitedNodes[getNumberOfNodes()];

    for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
        visitedNodes[i] = 0;
    }

    bdd->convertRootNode(root);
    bdd->generateRepresentation(root, visitedNodes);
    bdd->reorder((Cudd_ReorderingType)bdd_reordermethod);
    bdd->printMemoryInUse();

    trace(TRACE_5,"operatingGuidelines::convertToBdd(): end\n");
}


//! \fn void operatingGuidelines::convertToBddFull()
//! \brief converts an OG into its BDD representation including the red nodes and the markings of the nodes
void operatingGuidelines::convertToBddFull() {
	trace(TRACE_5, "operatingGuidelines::convertToBddFull(): start\n");
	
    bool visitedNodes[getNumberOfNodes()];

    for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
        visitedNodes[i] = 0;
    }

	trace(TRACE_0, "\nHIT A KEY TO CONTINUE (convertToBddFull)\n");
	//getchar();
	unsigned int nbrLabels = PN->getInputPlaceCount() + PN->getOutputPlaceCount();
	BddRepresentation * testbdd = new BddRepresentation(nbrLabels, (Cudd_ReorderingType)bdd_reordermethod, getNumberOfNodes(), true);
	testbdd->convertRootNode(root);
	testbdd->setMaxPlaceBits(root,visitedNodes);
	for (unsigned int i = 0; i < getNumberOfNodes(); i++) {
		visitedNodes[i] = 0;
	}
	cout << endl;
	
	testbdd->testSymbRepresentation(root, visitedNodes);
	testbdd->reorder((Cudd_ReorderingType)bdd_reordermethod);
	testbdd->printMemoryInUse();
	testbdd->printDotFile();
	delete testbdd;
    trace(TRACE_5,"operatingGuidelines::convertToBdd(): end\n");
}


void operatingGuidelines::printOGFile() const {
    string ogFilename = netfile;
    if (!options[O_CALC_ALL_STATES]) {
        ogFilename += ".R";
    }
    ogFilename += ".og";
    fstream ogFile(ogFilename.c_str(), ios_base::out | ios_base::trunc);

    bool visitedNodes[getNumberOfNodes()];
    for (unsigned int i = 0; i < getNumberOfNodes(); ++i) {
        visitedNodes[i] = false;
    }

    ogFile << "NODES" << endl;
    printNodesToOGFile(root, ogFile, visitedNodes);
    ogFile << ';' << endl << endl;

    ogFile << "INITIALNODE" << endl;
    ogFile << "  " << NodeNameForOG(root) << ';' << endl << endl;

    for (unsigned int i = 0; i < getNumberOfNodes(); ++i) {
        visitedNodes[i] = false;
    }
    ogFile << "TRANSITIONS" << endl;
    printTransitionsToOGFile(root, ogFile, visitedNodes);
    ogFile << ';' << endl;

    ogFile.close();
}


void operatingGuidelines::printNodesToOGFile(GraphNode * v, fstream& os,
    bool visitedNodes[]) const {

    if (v == NULL)
        return;

    // print node name (separated by comma to previous node if needed)
    if (v != root) {
        os << ',' << endl;
    }
    os << "  " << NodeNameForOG(v);

    // print node annotation
    os << " : " << v->getCNF_formula()->asString();

    os << " : " << v->getColor().toString();

    // mark current node as visited
    visitedNodes[v->getNumber()] = true;

    // recursively process successor nodes that have not been visited yet
    GraphEdge* edge;
    v->resetIteratingSuccNodes();
    while ((edge = v->getNextSuccEdge()) != NULL) {
        GraphNode* vNext = edge->getDstNode();

        // do not process nodes already visited
        if (visitedNodes[vNext->getNumber()])
            continue;

        if (!vNext->isToShow(root))
            continue;

        printNodesToOGFile(vNext, os, visitedNodes);
    }
}


void operatingGuidelines::printTransitionsToOGFile(GraphNode * v, fstream& os,
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
    GraphEdge* edge;
    v->resetIteratingSuccNodes();
    while ((edge = v->getNextSuccEdge()) != NULL) {
        GraphNode* vNext = edge->getDstNode();

        if (!vNext->isToShow(root))
            continue;

        // output transition (separated by comma to previous transition if
        // needed)
        if (firstTransitionSeen) {
            os << ',' << endl;
        } else {
            firstTransitionSeen = true;
        }

        os << "  " << NodeNameForOG(v) << " -> " << NodeNameForOG(vNext);
        os << " : " << edge->getLabel();

        // do not process nodes already visited
        if (visitedNodes[vNext->getNumber()])
            continue;

        printTransitionsToOGFile(vNext, os, visitedNodes);
    }
}


string operatingGuidelines::NodeNameForOG(const GraphNode* v) const {
    assert(v != NULL);
    return intToString(v->getNumber());
}

