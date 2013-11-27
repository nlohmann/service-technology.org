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


/******************
 * STATIC MEMBERS *
 ******************/


std::list<BSDNode *>* BSD::graph = NULL;
BSDNode* BSD::U = NULL;
BSDNode* BSD::emptyset = NULL;

MarkingList* BSD::templist = NULL;

std::map<InnerMarking_ID, int> * BSD::dfs = NULL;
std::map<InnerMarking_ID, int> * BSD::lowlink = NULL;
int BSD::maxdfs = 0;
std::stack<InnerMarking_ID> * BSD::S = NULL;
std::map<InnerMarking_ID, bool>* BSD::inStack = NULL;


/******************
 * STATIC METHODS *
 ******************/

/*!
 This function sets up the BSD automaton graph structure and the U node and the empty node.
 */
void BSD::initialize() {
	// the BSD automaton
	graph = new std::list<BSDNode *>;

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

 \param[in] og_flag indicates whether an og shall be computed later on

 */
void BSD::computeBSD(int og_flag) {
	// clear existing graph
	graph->clear();

	std::list<MarkingList> tempSCC;

	// the U node
	U = new BSDNode;
	U->pointer = new BSDNode*[Label::events+1];
	U->isU = true;
	assignLambda(U, tempSCC);

	// the empty node
	emptyset = new BSDNode;
	emptyset->pointer = new BSDNode*[Label::events+1];
	emptyset->isU = false;
	assignLambda(emptyset, tempSCC);

	// set up the pointers ( which are all loops to the same nodes )
	for (Label_ID id = 2; id <= Label::events; ++id) {
		U->pointer[id] = U;
		emptyset->pointer[id] = emptyset;
	}

	// start with the initial marking
	std::list<MarkingList>* SCCs = computeClosureTarjan(0);

	// if the bound was broken in the initial node then the BSD automaton consists of only
	// the U node
	if (SCCs == NULL) {
		status("bound broken in initial node...");
		graph->push_back(U);
		return;
	}

	setup(*SCCs, og_flag);
	delete SCCs;

	// iterate through the graph (start with the initial)
	// new nodes will be inserted on the fly at the back of the list
	std::list<BSDNode *>::iterator it = graph->begin();
	while (it != graph->end()) {
		// iterate through all the labels (except of \tau=0 and bound_broken=1)
		for (Label_ID id = 2/*sic!*/; id <= Label::events; ++id) {
			// compute the successor node after taking a step with current label
			computeSuccessor(**it, id, og_flag);
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
 \param[in] og_flag indicates whether an og shall be computed later on

 \return pointer to computed BSD node or NULL if the bound was broken or if no step was possible
 */
void BSD::computeSuccessor(BSDNode &node, Label_ID label, int og_flag) {
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
	node.pointer[label] = setup(resultlist, og_flag);
}


/*!
 \brief Given a list of SCCs as input, this function checks if the node is already in the graph and if not
 	 	 it creates a new node and assigns a lambda value to it. It also sets up the other needed structures.

 \param[in]	SCCs	list of marking lists (SCCs)
 \param[in] og_flag indicates whether an og shall be computed later on

 \return boolean value showing if the bound was broken
 */
BSDNode* BSD::setup(std::list<MarkingList> &SCCs, int og_flag) {
	MarkingList list;
	for (std::list<MarkingList>::const_iterator it = SCCs.begin(); it != SCCs.end(); ++it) {
		list.insert(list.end(), it->begin(), it->end());
	}
	list.sort();

	// check for the node in the graph and if not present insert it
	BSDNode* p = NULL;

	// the size of the list of markings (closure) in the given node
	unsigned int nodesize = list.size();

	// iterate through the graph
	for (std::list<BSDNode *>::iterator it = graph->begin(); it != graph->end(); ++it) {
		// we only have to test nodes with same sized marking lists
		if (nodesize == (**it).list.size()) {
			// check for equality of the nodes. If so then delete the given node and
			// return a pointer to the found node in the graph
			if (checkEquality(list, (**it).list)) {
				p = *it;
			}
		}
	}

	// if the node wasn't found in the graph then insert it at the back of the list
	// and set up the pointers (structure)
	// and we may as well compute the lambda values here
	if (p == NULL) {
		// set up the result node and return it
		graph->push_back(new BSDNode);
		p = graph->back();
		p->list = list;
		p->isU = false;
		p->pointer = new BSDNode*[Label::events+1];

		assignLambda(p, SCCs);

		// remember the SCCs in case of formula computation
		if (og_flag) {
			p->SCCs = SCCs;
		}
	}

	return p;
}


/*!
 \brief Compute tau-closure of a given reachable marking with Tarjan's algorithm

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

 \param[in]	markingID	the id of a marking

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

	// if the given bound is broken return NULL
	if (InnerMarking::inner_markings[markingID]->is_bad) {
		status("\tbound broken from marking %u", markingID);
		return NULL;
	}

	std::list<MarkingList>* result = new std::list<MarkingList>;

	// iterate through neighbour markings v' of v
	status("\titerating through successors of marking %u:", markingID);
	for (uint8_t i = 0; i < InnerMarking::inner_markings[markingID]->out_degree; ++i) {
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
					//abort(1337,"hit the mark");
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


/*!
 \brief Assign the lambda value to a node of the graph

 \param[in]	node	a node of the BSD automaton
 \param[in]	SCCs	a list of marking lists (SCCs of the node)
 */
void BSD::assignLambda(BSDNode *node, std::list<MarkingList> &SCCs) {
	if (node == U) {
		node->lambda = 0;
		return;
	}

	if (node == emptyset) {
		node->lambda = 4;
		return;
	}

	// assume that there doesn't exist a marking m that is a stop except for inputs
	node->lambda = 3;

	// iterate through all SCCs
	for (std::list<MarkingList>::const_iterator itSCC = SCCs.begin(); itSCC != SCCs.end(); ++itSCC) {
		// is there a transition that leads out of the SCC (\tau or sending label)
		bool found_outlabel = false;
		// iterate through the markings of the SCC
		for (MarkingList::const_iterator itlist = itSCC->begin(); itlist != itSCC->end(); ++itlist) {
			// iterate through all successor labels
			for (uint8_t i = 0; i < InnerMarking::inner_markings[*itlist]->out_degree; ++i) {
				// test if the label is receiving (for the environment)
				if (RECEIVING(InnerMarking::inner_markings[*itlist]->labels[i])) {
					found_outlabel = true;
					break;
				} else if (InnerMarking::inner_markings[*itlist]->labels[i] == TAU) {
					// test if the \tau-successor is in the same SCC
					bool found_succ_in_SCC = false;
					InnerMarking_ID succ = InnerMarking::inner_markings[*itlist]->successors[i];
					// iterate through the markings of the SCC
					for (MarkingList::const_iterator it = itSCC->begin(); it != itSCC->end(); ++it) {
						if (*it == succ) {
							found_succ_in_SCC = true;
							break;
						}
					}
					// if the \tau-successor is not in the same SCC we found a transition out of the SCC
					if (!found_succ_in_SCC) {
						found_outlabel = true;
						break;
					}
				}
			}
		}

		if (!found_outlabel) {
			node->lambda = 2; // found a stop except for inputs but no dead except for inputs (yet)

			bool found_final = false;
			for (MarkingList::const_iterator it = itSCC->begin(); it != itSCC->end(); ++it) {
				if (InnerMarking::inner_markings[*it]->is_final) {
					found_final = true;
					break;
				}
			}
			if (!found_final) {
				node->lambda = 1;   // found a dead except for inputs
				return;             // abort the computation
			}
		}
	}
}

