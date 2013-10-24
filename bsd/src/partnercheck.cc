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
#include "partnercheck.h"
#include "InnerMarking.h"
#include "Label.h"

#include "verbose.h"

using std::set;
using std::vector;


/******************
 * STATIC MEMBERS *
 ******************/

std::list<unsigned int>* partnercheck::bisimtemp = NULL;


/******************
 * STATIC METHODS *
 ******************/

/*========================================================
 *------------------- b-partner check --------------------
 *========================================================*/

/*!
 \brief Checks for two BSD automata BSD_b(N1) and BSD_b(N2) if the two nets N1 and N2 are b-partners.

 The underlying open nets have to be composable and the composed net has to be closed.

 \param[in]	graph1	the first BSD automaton
 \param[in]	graph2	the second BSD automaton

 \return boolean value showing if the underlying nets are b-partners of one another
 */
bool partnercheck::check_b_partner(parsedGraph & graph1, parsedGraph & graph2) {
	// check if the interface sizes differ
	if (graph1.events != graph2.events) {
		message("Size of interface differs! No bisimulation possible.");
		return false;
	}

	// check if the bounds differ
	if (graph1.bound != graph2.bound) {
		message("bound differs! (net 1 - bound: %i, net 2 - bound: %i)", graph1.bound, graph2.bound);
		return false;
	}

	status("computing mapping...");
	// compute a mapping between label ids of net 1 and net 2 if possible
	std::map<Label_ID, Label_ID>* mapping = computeMappingBSD(graph1, graph2);

	// if no mapping could be found return false
	if (mapping == NULL) {
		message("Interfaces do not match. Nets not composable");
		return false;
	}

	// create a list to store nodes of the bisimulation to be computed
	bisimtemp = new std::list<unsigned int>[graph1.nodes];

	status("computing bisimulation...");
	// compute the bisimulation
	bool result = computeBiSimBSD(graph1, graph2, 0, 0, mapping);

	printBiSim(graph1, graph2, bisimtemp);

	// delete the list and mapping
	delete[] bisimtemp;
	delete mapping;

	// return the result
	return result;
}


/*!
 \brief Computes the bisimulation relation recursively

 \param[in]	graph1	the first BSD automaton
 \param[in]	graph2	the second BSD automaton
 \param[in]	node_g1		a node of the first graph
 \param[in]	node_g2		a node of the second graph
 \param[in]	mapping		the mapping between labels of graph 1 and 2

 \return boolean value showing if the underlying nets can be b-partners of one another
 */
bool partnercheck::computeBiSimBSD(parsedGraph & graph1, parsedGraph & graph2, unsigned int node_g1, unsigned int node_g2, std::map<Label_ID, Label_ID> * mapping) {
	// check if the node pair was already visited
	for (std::list<unsigned int>::const_iterator it = bisimtemp[node_g1].begin(); it != bisimtemp[node_g1].end(); ++it) {
		if (*it == node_g2)
			return true;
	}

	// for all nodes (node1,node2) in the bisimulation the following has to hold:
	// lambda(node1) + lambda(node2) > 3
	if (graph1.lambdas[node_g1] + graph2.lambdas[node_g2] <= 3) {
		status("%s and %s don't meet the requirement! aborting computation...", graph1.names[node_g1].c_str(), graph2.names[node_g2].c_str());
		// return false recursively (abort)
		return false;
	} else {
		// add the node to the list of visited nodes
		bisimtemp[node_g1].push_back(node_g2);
	}

	// check successors
	for (unsigned int id = 0; id < graph1.events; ++id) {
		if (!computeBiSimBSD(graph1, graph2, graph1.pointer[node_g1][id], graph2.pointer[node_g2][(*mapping)[id]], mapping)) {
			// abort recursively
			return false;
		}
	}

	// if all went well return true
	return true;
}

/*!
 \brief compute mapping from labels of graph 1 to graph 2 (they may have different IDs for the corresponding labels)

 \param[in]	graph1	the first BSD automaton (list of BSD node pointers)
 \param[in]	graph2	the second BSD automaton (list of BSD node pointers)

 \return mapping
 */
std::map<Label_ID, Label_ID>* partnercheck::computeMappingBSD(parsedGraph & graph1, parsedGraph & graph2) {
	std::map<Label_ID, Label_ID>* result = new std::map<Label_ID, Label_ID>;

	// iterate through the label ids of graph 1
	for (std::map<Label_ID, std::string>::const_iterator it1 = graph1.id2name->begin(); it1 != graph1.id2name->end(); ++it1) {
//		// skip the \tau and bound_broken label
//		if (it1->first < 2)
//			continue;
		bool found = false;
		// iterate through the label ids of graph 2
		for (std::map<Label_ID, std::string>::const_iterator it2 = graph2.id2name->begin(); it2 != graph2.id2name->end(); ++it2) {
//			// skip the \tau and bound_broken label
//			if (it2->first < 2)
//				continue;
			// check for equality of the label names
			if (it1->second == it2->second) {
				found = true;
				// the label has to be sending on the one side and receiving on the other side or vice versa
				if ((*graph1.is_sending_label)[it1->first] != (*graph2.is_sending_label)[it2->first]) {
					// add the mapping from id 1 to id 2
					(*result)[it1->first] = it2->first;
					break;
				} else {
					// abort
					status("label %s of net 1 and net 2 are of equal type (sending/receiving)", it1->second.c_str(), it2->second.c_str());
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


void partnercheck::match(BSDgraph & graph) {

}


/*========================================================
 *---------------------- TEST OUTPUT ---------------------
 *========================================================*/

/*!
 \brief print the bisimulation relation

 \param[in]	graph1	the first parsed automaton
 \param[in]	graph2	the second parsed automaton
 \param[in]	bisim	the bisimulation relation
 */
void partnercheck::printBiSim(parsedGraph & graph1, parsedGraph & graph2, std::list<unsigned int>* bisim) {
	std::stringstream temp (std::stringstream::in | std::stringstream::out);
	temp << std::endl;
//	temp << "U: " << graph.U << ", empty: " << graph.emptyset << std::endl;
	for (int i = 0; i < graph1.nodes; ++i) {
		temp << graph1.names[i] << " <-> ";
		for (std::list<unsigned int>::const_iterator it = bisim[i].begin(); it != bisim[i].end(); ++it) {
			temp << graph2.names[*it] << "; ";
		}
		temp << std::endl;
	}

	status("%s", temp.str().c_str());
}
