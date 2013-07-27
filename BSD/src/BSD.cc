/*****************************************************************************\
 Locretia -- generating logs...

 Copyright (c) 2012 Simon Heiden

 Locretia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Locretia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Locretia.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <config.h>
#include <string>
#include <sstream>
#include "BSD.h"
#include "InnerMarking.h"
#include "Label.h"

#include "verbose.h"

using std::set;
using std::vector;

BSD::BSD() {
}

/******************
 * STATIC MEMBERS *
 ******************/


BSDNodeList* BSD::graph = NULL;
BSDNode* BSD::U = NULL;
BSDNode* BSD::emptyset = NULL;
MarkingList* BSD::templist = NULL;


/******************
 * STATIC METHODS *
 ******************/

void BSD::initialize() {
	graph = new BSDNodeList;

	U = new BSDNode;
	U->lambda = 0;
	U->pointer = new BSDNode*[Label::events+1];

	emptyset = new BSDNode;
	emptyset->lambda = 3;
	emptyset->pointer = new BSDNode*[Label::events+1];

	for (Label_ID id = 2; id <= Label::events; ++id) {
		U->pointer[id] = U;
		emptyset->pointer[id] = emptyset;
	}

	templist = new MarkingList;
}


/*!
 \brief Creates the BSD automaton based on the given reachability graph.

 */
void BSD::computeBSD() {
	// clear existing graph
	graph->clear();

	// start with the initial marking
	BSDNode *initial = new BSDNode;

	// compute the \tau-closure of the initial marking
	bool boundbroken = computeClosure(0);

	if (boundbroken) {
		status("bound broken in initial node...");
		graph->push_back(U);
		return;
	}

	templist->sort();
	initial->list = *templist;
	initial->pointer = new BSDNode*[Label::events+1];

	// add the initial closure to the (still empty) graph
	graph->push_back(initial);

//	printBSD(graph);

	// iterate through the graph (start with the initial)
	std::list<BSDNode *>::iterator it = graph->begin();
	while (it != graph->end()) {
		// iterate through all the labels (except of \tau=0 and bound_broken=1)
		for (Label_ID id = 2; id <= Label::events; ++id) {
			// compute the closure after step with current label
			BSDNode* temp = computeClosure(**it, id);
			// if bound was not broken or the goal node is the empty node
			if (temp != NULL) {
				// check for the node in the graph and if not present insert it
				BSDNode* p = checkInsertIntoGraph(*temp);
				// set the pointer with the current label to the node
				(**it).pointer[id] = p;
			}
		}
		++it;
//		printBSD(graph);
	}

	assignLambdas(graph);

	graph->push_back(U);
	graph->push_back(emptyset);
}

// compute closure of BSD node after performing step with given label
BSDNode* BSD::computeClosure(BSDNode &node, Label_ID label) {
	status("computing closure of node %x with label %s", &node, Label::id2name[label].c_str());
	BSDNode* result = new BSDNode;
	MarkingList resultlist;
	// iterate through all marking ids in the BSD node
	for (MarkingList::const_iterator it = node.list.begin(); it != node.list.end(); ++it) {
		status("marking id: %u", *it);
		// iterate through all successors of the marking
		for (uint8_t i = 0; i < InnerMarking::inner_markings[*it]->out_degree; ++i) {
			// if there is a successor with the given label id then compute the closure of the successor marking
			if (InnerMarking::inner_markings[*it]->labels[i] == label) {
				templist->clear();
				bool boundbroken = computeClosure(InnerMarking::inner_markings[*it]->successors[i]);

//				printlist(templist);

				if (boundbroken) {
					//\todo: bound broken, set pointer to U
					node.pointer[label] = U;
					return NULL;
				}

				templist->sort();
				// add the closure to the already computed closures
				mergeWithoutDuplicates(resultlist, *templist);

				// if label was used then break the for-loop
				break;
			}
		}

	}

	if (resultlist.empty()) {
		//\todo: empty node, set pointer to \emptyset
		node.pointer[label] = emptyset;
		return NULL;
	}

	result->list = resultlist;
	result->pointer = new BSDNode*[Label::events+1];
	return result;
}

//MarkingList* BSD::computeClosure(MarkingList &list) {
//	MarkingList* resultlist = new MarkingList;
//	for (MarkingList::const_iterator it = list.begin(); it != list.end(); ++it) {
//		bool boundbroken = computeClosure(*it);
//		if (!boundbroken) {
//			templist->sort();
//			// temp is already sorted...
//			mergeWithoutDuplicates(*resultlist, *templist);
//			templist->clear();
//		} else {
//			// if bound was broken then return NULL \todo!?
//			templist->clear();
//			delete resultlist;
//			return NULL;
//		}
//	}
//
//	return resultlist;
//}

bool BSD::computeClosure(InnerMarking_ID id) {
	status("touching node %u", id);
	for (MarkingList::const_iterator it = templist->begin(); it != templist->end(); ++it) {
		if (*it == id) {
			status("node already in templist");
			return false;
		}
	}
	// add the current marking to the closure
	templist->push_back(id);
	// iterate through all successors of the given marking
	status("\titerating through succesors of node %u:", id);
	for (uint8_t i = 0; i < InnerMarking::inner_markings[id]->out_degree; ++i) {
		// if the given bound is broken return NULL as an indication
		if (InnerMarking::inner_markings[id]->labels[i] == BOUND) {
			status("\tbound broken from node %u", id);
			return true;
		}
		// only consider \tau-steps
		if (InnerMarking::inner_markings[id]->labels[i] == TAU) {
			status("\ttau step possible from node %u to node %u", id, InnerMarking::inner_markings[id]->successors[i]);
			// compute closure of successor
			bool boundbroken = computeClosure(InnerMarking::inner_markings[id]->successors[i]);
			// if bound was broken return NULL recursively
			if (boundbroken) {
				status("\tbound broken (recursive abort, node %u)", id);
				return true; //\todo?
			}
		}
	}
	return false;
}

void BSD::mergeWithoutDuplicates(MarkingList &result, MarkingList &temp) {
	MarkingList::iterator itresult = result.begin();
	MarkingList::iterator ittemp = temp.begin();

	while (ittemp != temp.end()) {
		if (itresult != result.end()) {
			if (*ittemp < *itresult) {
				// if the current element of the resulting list is greater than the element in the temporary list
				// then insert the smaller element before the greater one and increment the temporary list pointer
				result.insert(itresult, *ittemp);
				++ittemp;
			} else if (*ittemp == *itresult) {
				// if both elements are equal then just increment both pointers (don't insert duplicates)
				++ittemp;
				++itresult;
			} else {
				// if the current element of the temporary list is greater than the element of the resulting list
				// then increment the resulting list pointer to insert it further to the right
				++itresult;
			}
		} else {
			// if the resulting list is at its end then just add the items of the temporary list at the end
			result.push_back(*ittemp);
			++ittemp;
		}
	}
}

BSDNode* BSD::checkInsertIntoGraph(BSDNode &node) {
	uint nodesize = node.list.size();
	for (std::list<BSDNode *>::iterator it = graph->begin(); it != graph->end(); ++it) {
		if (nodesize == (**it).list.size()) {
			if (checkEquality(node.list, (**it).list)) {
				delete &node;
				return *it;
			}
		}
	}

	graph->push_back(&node);
	graph->back()->pointer = new BSDNode*[Label::events+1];

	return graph->back();
}

bool BSD::checkEquality(MarkingList &list1, MarkingList &list2) {
	MarkingList::iterator it1 = list1.begin();
	MarkingList::iterator it2 = list2.begin();

	while (it1 != list1.end() /*&& it2 != list2.end()*/) { //shortened because we know that both lists are of the same size
		if (*it1 != *it2) {
			return false;
		}
		++it1;
		++it2;
	}

	return true;
}

void BSD::assignLambdas(BSDNodeList *graph) {
	// iterate through all nodes of the graph
	for (BSDNodeList::const_iterator it = graph->begin(); it != graph->end(); ++it) {
		// assume that there doesn't exist a marking m that is a stop except for inputs
		(*it)->lambda = 2;
		// iterate through all markings in the current node
		for (MarkingList::const_iterator itlist = (*it)->list.begin(); itlist != (*it)->list.end(); ++itlist) {
			bool receive = false;
			// iterate through all successor labels
			for (uint8_t i = 0; i < InnerMarking::inner_markings[*itlist]->out_degree; ++i) {
				// test if the label is receiving (for the environment)
				if (RECEIVING(InnerMarking::inner_markings[*itlist]->labels[i])) {
					receive = true;
				}
			}
			if (!receive) {
				// found a marking that is a stop except for inputs (no receive possible for the environment)
				(*it)->lambda = 1;
				break;
			}
		}
	}
}

void BSD::printBSD(BSDNodeList *graph) {
	std::stringstream temp (std::stringstream::in | std::stringstream::out);
	temp << "U: " << U << ", empty: " << emptyset << std::endl;
	for (BSDNodeList::const_iterator it = graph->begin(); it != graph->end(); ++it) {
		temp << *it << ":  lambda: " << (uint)(*it)->lambda << ",  list: (";
		for (MarkingList::const_iterator listit = (*it)->list.begin(); listit != (*it)->list.end();) {
			temp << *listit;
			if (++listit != (*it)->list.end()) {
				temp << ", ";
			}
		}
		temp << ")" << std::endl << "\t";
		for (uint id = 2; id <= Label::events; ++id) {
			temp << "(" << Label::id2name[id] << "," << (*it)->pointer[id] << ")";
			if (id != Label::events) {
				temp << ", ";
			}
		}
		temp << std::endl;
		temp << std::endl;
	}

	status("%s", temp.str().c_str());
}

void BSD::printlist(MarkingList *list) {
	std::stringstream temp (std::stringstream::in | std::stringstream::out);

	for (MarkingList::const_iterator it = list->begin(); it != list->end(); ++it) {
		temp << *it << ", ";
	}

	status("list: %s", temp.str().c_str());
}

