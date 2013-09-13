/*****************************************************************************\
 BSD -- generating BSD automata

 Copyright (c) 2013 Simon Heiden

 BSD is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 BSD is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with BSD.  If not, see <http://www.gnu.org/licenses/>.
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

std::map<InnerMarking_ID, int> * BSD::dfs = NULL;
std::map<InnerMarking_ID, int> * BSD::lowlink = NULL;
int BSD::maxdfs = 0;
std::stack<InnerMarking_ID> * BSD::S = NULL;
std::map<InnerMarking_ID, bool>* BSD::inStack = NULL;

std::list<std::pair<BSDNode*, BSDNode*> >* BSD::bisimtemp = NULL;


/******************
 * STATIC METHODS *
 ******************/

/*!
 This function sets up the BSD automaton graph structure and the U node and the empty node.
 */
void BSD::initialize() {
	// the BSD automaton
	graph = new BSDNodeList;

	// the U node
	U = new BSDNode;
	U->lambda = 0;
	U->pointer = new BSDNode*[Label::events+1];
	U->isU = true;

	// the empty node
	emptyset = new BSDNode;
	emptyset->lambda = 3;
	emptyset->pointer = new BSDNode*[Label::events+1];
	emptyset->isU = false;

	// set up the pointers ( which are all loops to the same nodes )
	for (Label_ID id = 2; id <= Label::events; ++id) {
		U->pointer[id] = U;
		emptyset->pointer[id] = emptyset;
	}

	// a temporary list to save already visited markings
	templist = new MarkingList;

	dfs = new std::map<InnerMarking_ID, int>;
	lowlink = new std::map<InnerMarking_ID, int>;
	S = new std::stack<InnerMarking_ID>;
	inStack = new std::map<InnerMarking_ID, bool>;
}

void BSD::finalize() {
	delete templist;
	delete dfs;
	delete lowlink;
	delete S;
	delete inStack;
}


/*========================================================
 *-------------------- BSD computation -------------------
 *========================================================*/


/*!
 \brief Creates the BSD automaton based on the given reachability graph.

 */
void BSD::computeBSD() {
	// clear existing graph
	graph->clear();

	// start with the initial marking
	std::list<MarkingList>* SCCs = computeClosureTarjan(0);

	// if the bound was broken in the initial node then the BSD automaton consists of only
	// the U node
	if (SCCs == NULL) {
		status("bound broken in initial node...");
		graph->push_back(U);
		return;
	}

	setup(*SCCs);
	delete SCCs;

	// iterate through the graph (start with the initial)
	// new nodes will be inserted on the fly at the back of the list
	std::list<BSDNode *>::iterator it = graph->begin();
	while (it != graph->end()) {
		// iterate through all the labels (except of \tau=0 and bound_broken=1)
		for (Label_ID id = 2/*sic!*/; id <= Label::events; ++id) {
			// compute the successor node after taking a step with current label
			computeSuccessor(**it, id);
		}
		++it;
	}

	// add the U node and the empty node at the back of the node list
	graph->push_back(U);
	graph->push_back(emptyset);
}


/*!
 \brief Compute the successor node of the given BSD node after performing a step with given label (if possible)

 \param[in]	node	the current node of the BSD automaton
 \param[in]	label	the label id

 \return pointer to computed BSD node or NULL if the bound was broken or if no step was possible
 */
void BSD::computeSuccessor(BSDNode &node, Label_ID label) {
	status("computing closure of BSD node %x after step with label %s", &node, Label::id2name[label].c_str());
	std::list<MarkingList> resultlist;
	// iterate through all marking ids in the BSD node
	for (MarkingList::const_iterator it = node.list.begin(); it != node.list.end(); ++it) {
		status("node %x, visiting marking id: %u", &node, *it);
		// iterate through all successors of the marking
		for (uint8_t i = 0; i < InnerMarking::inner_markings[*it]->out_degree; ++i) {
			// if there is a successor with the given label id then compute the closure of the successor marking
			if (InnerMarking::inner_markings[*it]->labels[i] == label) {
				std::list<MarkingList>* SCCs = computeClosureTarjan(InnerMarking::inner_markings[*it]->successors[i]);

				// if the bound was broken by taking a step with the current label then add a pointer from this node
				// to the U node with the current label and abort the computation
				if (SCCs == NULL) {
					node.pointer[label] = U;
					return;
				}

				// merge the closure with the already computed closures
				mergeSCCsWithoutDuplicates(resultlist, *SCCs);
				delete SCCs;

				// if label was found and the step was performed then break the for-loop
				break;
			}
		}
	}

	// if no step was possible then add a pointer from this node to the empty node with the current label
	if (resultlist.empty()) {
		node.pointer[label] = emptyset;
		return;
	}

	// else test if the node already exists and add a pointer from this node to the inserted (or existing) node
	node.pointer[label] = setup(resultlist);
}


/*!
 \brief Given a list of SCCs as input, this function checks if the node is already in the graph and if not
 	 	 it creates a new node and assigns a lambda value to it. It also sets up the other needed structures.

 \param[in]	SCCs	list of marking lists (SCCs)

 \return boolean value showing if the bound was broken
 */
BSDNode* BSD::setup(std::list<MarkingList> &SCCs) {
	MarkingList list;
	for (std::list<MarkingList>::const_iterator it = SCCs.begin(); it != SCCs.end(); ++it) {
		list.insert(list.end(), it->begin(), it->end());
	}
	list.sort();

	// set up the result node and return it
	BSDNode* result = new BSDNode;
	result->list = list;
	result->isU = false;
	BSDNode* p = NULL;

	// check for the node in the graph and if not present insert it

	// the size of the list of markings (closure) in the given node
	unsigned int nodesize = result->list.size();

	// iterate through the graph
	for (std::list<BSDNode *>::iterator it = graph->begin(); it != graph->end(); ++it) {
		// we only have to test nodes with same sized marking lists
		if (nodesize == (**it).list.size()) {
			// check for equality of the nodes. If so then delete the given node and
			// return a pointer to the found node in the graph
			if (checkEquality(result->list, (**it).list)) {
				delete result;
				p = *it;
			}
		}
	}

	// if the node wasn't found in the graph then insert it at the back of the list
	// and set up the pointers (structure)
	// and we may as well compute the lambda values here
	if (p == NULL) {
		graph->push_back(result);
		graph->back()->pointer = new BSDNode*[Label::events+1];

		// return the pointer to the inserted node
		p = graph->back();

		assignLambda(p, SCCs);
	}

	return p;
}


/*!
 \brief Compute \tau-closure of a given reachable marking with Tarjan's algorithm

 \param[in]	id	the id of a marking

 \return list of SCCs (sorted) in the closure or NULL if bound was broken
 */
std::list<MarkingList>* BSD::computeClosureTarjan(InnerMarking_ID id) {
	// clear all helper structures
	dfs->clear();
	lowlink->clear();
	inStack->clear();

	templist->clear();

	// input: graph G = (node->list (V), E)

	maxdfs = 0;						// counter for dfs

	// make sure the stack is empty
	while (!S->empty())
		S->pop();

	return tarjanClosure(id);		// the call to tarjan visits all markings reachable from v
}

/*!
 \brief recursive and for BSD computation adjusted tarjan algorithm

 \param[in]	id	the id of a marking

 \return list of SCCs (sorted) in the closure or NULL if bound was broken
 */
std::list<MarkingList>* BSD::tarjanClosure(InnerMarking_ID markingID) {
	status("visiting marking %u", markingID);
	// add the current marking to the closure
	templist->push_back(markingID);

	(*dfs)[markingID] = maxdfs;			// set dfs index of current marking v
	(*lowlink)[markingID] = maxdfs;		// v.lowlink <= v.dfs
	maxdfs++;							// increment counter
	S->push(markingID);					// push v on top of stack
	(*inStack)[markingID] = true;		// set to true (v is in stack)

	std::list<MarkingList>* result = new std::list<MarkingList>;

	// iterate through neighbour markings v' of v
	status("\titerating through successors of marking %u:", markingID);
	for (uint8_t i = 0; i < InnerMarking::inner_markings[markingID]->out_degree; ++i) {
		// if the given bound is broken return true
		if (InnerMarking::inner_markings[markingID]->labels[i] == BOUND) {
			status("\tbound broken from marking %u", markingID);
			delete result;
			return NULL;
		}

		InnerMarking_ID idNeighbour = InnerMarking::inner_markings[markingID]->successors[i];
		// only consider \tau-steps (other steps don't matter for the closures)
		if (InnerMarking::inner_markings[markingID]->labels[i] == TAU) {
			status("\ttau step possible from marking %u to marking %u", markingID, InnerMarking::inner_markings[markingID]->successors[i]);
			bool visited = false; // v' visited?

			// test if the marking was already visited
			for (MarkingList::const_iterator it = templist->begin(); it != templist->end(); ++it) {
				if (*it == idNeighbour) {
					status("marking already visited");
					visited = true;
					break;
				}
			}

			if (!visited) {
				std::list<MarkingList>* temp = tarjanClosure(idNeighbour);	// recursive call

				if (temp == NULL) {
					status("\tbound broken (recursive abort, marking %u)", markingID);
					delete result;
					return NULL;
				}

				result->insert(result->end(), temp->begin(), temp->end());		// insert into the result list
				delete temp;

				(*lowlink)[markingID] = std::min((*lowlink)[markingID], (*lowlink)[idNeighbour]); // v.lowlink := min(v.lowlink, v'.lowlink);
			} else { // v' is visited
				// if v' is in the stack S
				if ((*inStack)[idNeighbour]) {
					(*lowlink)[markingID] = std::min((*lowlink)[markingID], (*dfs)[idNeighbour]); // v.lowlink := min(v.lowlink, v'.dfs);
				}
			}
		}
	}

	// if v.lowlink == v.dfs
	if ((*lowlink)[markingID] == (*dfs)[markingID]) { // root of a SCC
		MarkingList SCC;
		// compute the SCC
		InnerMarking_ID id = 0;
		do {
			id = S->top();			// take top of stack v*
			S->pop();				// remove top of stack
			(*inStack)[id] = false;	// set to false (v* isn't in stack any more)
			SCC.push_back(id);		// add v* to the SCC
		} while (id != markingID);

		SCC.sort();
		result->push_back(SCC);
	}

	return result;
}


/*!
 \brief merge two lists of (sorted) marking lists (SCCs) and skip duplicates

 \param[in]	result	the first list (which will also be used as the resulting list after merging)
 \param[in]	temp	the second list
 */
void BSD::mergeSCCsWithoutDuplicates(std::list<MarkingList> &result, std::list<MarkingList> &temp) {
	std::list<MarkingList> temporary;

	// iterate through the second list whose elements shall be inserted into the first list
	for (std::list<MarkingList>::iterator ittemp = temp.begin(); ittemp != temp.end(); ++ittemp) {
		// found an equal list?
		bool found_equal = false;
		unsigned int temp_size = ittemp->size();
		// iterate through the second list and look for the element of the first list
		for (std::list<MarkingList>::iterator itresult = result.begin(); itresult != result.end(); ++itresult) {
			// only check lists of equal size
			if (temp_size == itresult->size()) {
				found_equal = checkEquality(*ittemp, *itresult);
				// if the two lists are equal then break the inner for-loop
				if (found_equal)
					break;
			}
		}

		// if the element is not in the result list save it temporarily for later insertion
		if (!found_equal) {
			temporary.push_back(*ittemp);
		}
	}

	// insert all elements from the second list which weren't found in the first list
	result.insert(result.end(), temporary.begin(), temporary.end());
}


/*!
 \brief Check if the two given (sorted AND same size!!!) marking lists are equal

 \param[in]	list1	marking list 1
 \param[in]	list2	marking list 2

 \return boolean value showing if lists are equal or not
 */
bool BSD::checkEquality(MarkingList &list1, MarkingList &list2) {
	MarkingList::iterator it1 = list1.begin();
	MarkingList::iterator it2 = list2.begin();

	// iterate through the lists
	while (it1 != list1.end() /*&& it2 != list2.end()*/) { //shortened because we know that both lists are of the same size
		// check for equality of elements
		if (*it1 != *it2) {
			return false;
		}
		++it1;
		++it2;
	}

	// all elements are equal so return true
	return true;
}


///*!
// \brief Assign the lambda values to the nodes in the graph
//
// \param[in]	graph	the BSD automaton (list of BSD node pointers)
// */
//void BSD::assignLambdas(BSDNodeList *graph) {
//	// iterate through all nodes of the graph
//	// (U and empty node will be added later so we don't need to take care of that here)
//	for (BSDNodeList::const_iterator it = graph->begin(); it != graph->end(); ++it) {
//		assignLambda(*it);
//	}
//}


/*!
 \brief Assign the lambda value to a node of the graph

 \param[in]	node	a node of the BSD automaton
 \param[in]	SCCs	a list of marking lists (SCCs of the node)
 */
void BSD::assignLambda(BSDNode *node, std::list<MarkingList> &SCCs) {
	if (node == U) {
		node->lambda = 3;
		return;
	}
	if (node == emptyset) {
		node->lambda = 0;
		return;
	}

	// assume that there doesn't exist a marking m that is a stop except for inputs
	node->lambda = 2;

	// iterate through all SCCs
	for (std::list<MarkingList>::const_iterator itSCC = SCCs.begin(); itSCC != SCCs.end(); ++itSCC) {
		bool found_outlabel = false;
		for (MarkingList::const_iterator itlist = node->list.begin(); itlist != node->list.end(); ++itlist) {
			// iterate through all successor labels
			for (uint8_t i = 0; i < InnerMarking::inner_markings[*itlist]->out_degree; ++i) {
				// test if the label is receiving (for the environment)
				if (RECEIVING(InnerMarking::inner_markings[*itlist]->labels[i])) {
					found_outlabel = true;
					break;
				}
			}
		}

		if (!found_outlabel) {
			node->lambda = 1;     // found a stop except for inputs
			break;
		}
	}
}


/*========================================================
 *---------------------- TEST OUTPUT ---------------------
 *========================================================*/


/*!
 \brief print the BSD automaton in the shell (if verbose is switched on)

 \param[in]	graph	the BSD automaton (list of BSD node pointers)
 */
void BSD::printBSD(BSDNodeList *graph) {
	std::stringstream temp (std::stringstream::in | std::stringstream::out);
	temp << "U: " << U << ", empty: " << emptyset << std::endl;
	for (BSDNodeList::const_iterator it = graph->begin(); it != graph->end(); ++it) {
		temp << *it << ":  lambda: " << (unsigned int)(*it)->lambda << ",  list: (";
		for (MarkingList::const_iterator listit = (*it)->list.begin(); listit != (*it)->list.end();) {
			temp << *listit;
			if (++listit != (*it)->list.end()) {
				temp << ", ";
			}
		}
		temp << ")" << std::endl << "\t";
		for (unsigned int id = 2; id <= Label::events; ++id) {
			temp << "(" << Label::id2name[id] << ",";
			if ((*it)->pointer[id]->isU)
				temp << U;
			else
				temp << (*it)->pointer[id];
			temp << ")";
			if (id != Label::events) {
				temp << ", ";
			}
		}
		temp << std::endl;
		temp << std::endl;
	}

	status("%s", temp.str().c_str());
}


/*!
 \brief print a marking list in the shell (if verbose is switched on)

 \param[in]	list	the marking list to be printed
 */
void BSD::printlist(MarkingList *list) {
	std::stringstream temp (std::stringstream::in | std::stringstream::out);

	for (MarkingList::const_iterator it = list->begin(); it != list->end(); ++it) {
		temp << *it << ", ";
	}

	status("list: %s", temp.str().c_str());
}


/*========================================================
 *---------------------- Bisimulation --------------------
 *========================================================*/


/*!
 \brief Checks theorem 1.3 (Bisimulation of two BSD automata and lambda values)

 The underlying open nets have to be composable and the composed net has to be closed.

 \param[in]	graph1	the first BSD automaton (list of BSD node pointers)
 \param[in]	graph2	the second BSD automaton (list of BSD node pointers)

 \return boolean value showing if the underlying nets are br-controllers of one another
 */
bool BSD::checkBiSimAndLambda(BSDgraph & graph1, BSDgraph & graph2) {
	// check if the interface sizes differ
	if (graph1.receive_events != graph2.send_events ||
		graph1.send_events != graph2.receive_events) {
		message("Size of interface differs! No bisimulation possible.");
		return false;
	}

	// compute a mapping between label ids of net 1 and net 2 if possible
	std::map<Label_ID, Label_ID>* mapping = computeMapping(graph1, graph2);

	// if no mapping could be found return false
	if (mapping == NULL) {
		message("Interfaces do not match. Nets not composable");
		return false;
	}

	// create a list to store nodes of the bisimulation to be computed
	bisimtemp = new std::list<std::pair<BSDNode*, BSDNode*> >;

	// compute the bisimulation
	bool result = computeBiSim(*graph1.graph->begin(), *graph2.graph->begin(), mapping, graph1.events);

	// delete the list and mapping
	delete bisimtemp;
	delete mapping;

	// return the result
	return result;
}


/*!
 \brief Checks theorem 1.3 (Bisimulation of two BSD automata and lambda values)

 \param[in]	node_g1		a node of the first graph
 \param[in]	node_g2		a node of the second graph
 \param[in]	mapping		the mapping between labels of graph 1 and 2
 \param[in]	events		the number of total labels

 \return boolean value showing if the underlying nets could be br-controllers of one another
 */
bool BSD::computeBiSim(BSDNode * node_g1, BSDNode * node_g2, std::map<Label_ID, Label_ID> * mapping, Label_ID events) {
	// check if the pair of nodes is already in the list of visited nodes
	for (std::list<std::pair<BSDNode*, BSDNode*> >::const_iterator it = bisimtemp->begin(); it != bisimtemp->end(); ++it) {
		if (it->first == node_g1 && it->second == node_g2)
			return true;
	}

	// all nodes of the bisimulation have to have added lambda values bigger than 2
	if (node_g1->lambda + node_g2->lambda <= 2) {
		// return false recursively (abort)
		return false;
	} else {
		// add the node to the list of visited nodes
		std::pair<BSDNode*, BSDNode*> temp (node_g1, node_g2);
		bisimtemp->push_back(temp);
	}

	// check successors
	bool valid = true;
	for (unsigned int id = 2; id <= events; ++id) {
		valid = computeBiSim(node_g1->pointer[id], node_g2->pointer[(*mapping)[id]], mapping, events);
		if (!valid) {
			// abort recursively
			return false;
		}
	}

	// if all went well return true
	return true;
}


/*!
 \brief compute mapping from labels of graph 1 to graph 2

 \param[in]	graph1	the first BSD automaton (list of BSD node pointers)
 \param[in]	graph2	the second BSD automaton (list of BSD node pointers)

 \return mapping
 */
std::map<Label_ID, Label_ID>* BSD::computeMapping(BSDgraph & graph1, BSDgraph & graph2) {
	std::map<Label_ID, Label_ID>* result = new std::map<Label_ID, Label_ID>;

	// iterate through the label ids of graph 1
	for (std::map<Label_ID, std::string>::const_iterator it1 = graph1.id2name.begin(); it1 != graph1.id2name.end(); ++it1) {
		// skip the \tau and bound_broken label
		if (it1->first < 2)
			continue;
		bool found = false;
		// iterate through the label ids of graph 2
		for (std::map<Label_ID, std::string>::const_iterator it2 = graph2.id2name.begin(); it2 != graph2.id2name.end(); ++it2) {
			// skip the \tau and bound_broken label
			if (it2->first < 2)
				continue;
			// check for equality of the label names
			if (it1->second.compare(it2->second) == 0) {
				found = true;
				// the label has to be sending on the one side and receiving on the other side or vice versa
				if ((it1->first >= graph1.first_receive && it1->first <= graph1.last_receive &&		// (label 1 receiving and
						it2->first >= graph2.first_send && it2->first <= graph2.last_send) ||		// label 2 sending) or
					(it1->first >= graph1.first_send && it1->first <= graph1.last_send &&			// (label 1 sending and
						it2->first >= graph2.first_receive && it2->first <= graph2.last_receive)) {	// label 2 receiving)
					// add the mapping from id 1 to id 2
					(*result)[it1->first] = it2->first;
					break;
				} else {
					// abort
					status("label %s of net 1 and net 2 are either both input or both output labels", it1->second.c_str(), it2->second.c_str());
					delete result;
					return NULL;
				}
			}
		}
		// if no corresponding label was found then abort
		if (!found) {
			status("label %s of net 1 doesn't match any label of net 2", it1->second.c_str());
			delete result;
			return NULL;
		}
	}

	// return the mapping
	return result;
}


/*========================================================
 *------------------- uBSD computation -------------------
 *========================================================*/


/*!
 \brief Creates the uBSD automaton based on the BSD automaton.

 */
void BSD::computeUBSD(BSDgraph & graph) {
	bool graphChanged = true;

	// repeat while graph changes
	while (graphChanged) {
		// assume the graph doesn't change
		graphChanged = false;

		// iterate through all nodes
		for (BSDNodeList::const_iterator it = graph.graph->begin(); it != graph.graph->end(); ++it) {
			// ignore the current node if the node is the U node
			if ((*it)->isU || *it == graph.emptyset)
				continue;

			if ((*it)->lambda == 1) {
				// iterate through the receiving labels (sending for the environment)
				bool allSuccAreU = true;
				for (unsigned int id = graph.first_send; id <= graph.last_send; ++id) {
					// if a successor is not the U node then stop the iteration, nothing to do here...
					if (!(*it)->pointer[id]->isU) {
						allSuccAreU = false;
						break;
					}
				}

				// if all sending labels successors are the U node then change the current node to the U node
				// and prepare for another round of computation (graph has changed)
				if (allSuccAreU) {
					(*it)->isU = true;
					graphChanged = true;
				}
			}

			// iterate through the sending labels (receiving for the environment)
			for (unsigned int id = graph.first_receive; id <= graph.last_receive; ++id) {
				// if a successor is the U node then stop the iteration...
				if ((*it)->pointer[id]->isU) {
					(*it)->isU = true;
					graphChanged = true;
					break;
				}
			}

		}

	}
}

