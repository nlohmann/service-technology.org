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
#include "CSD.h"
#include "openNet.h"

#include "verbose.h"

using std::set;
using std::vector;


/******************
 * STATIC MEMBERS *
 ******************/


/******************
 * STATIC METHODS *
 ******************/

/*========================================================
 *-------------------- CSD computation -------------------
 *========================================================*/

/*!
 \brief Creates the CSD automaton based on the BSD automaton.

 */
void CSD::computeCSD(BSDgraph & graph) {
	bool graphChanged = true;

	// repeat while graph changes
	while (graphChanged) {
		// assume the graph doesn't change
		graphChanged = false;

		// iterate through all nodes
		for (std::list<BSDNode *>::const_iterator it = graph.graph->begin(); it != graph.graph->end(); ++it) {
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


/*========================================================
 *------------------- maximal b-partner ------------------
 *========================================================*/

/*!
 \brief Creates the maximal b-partner from a CSD automaton (from MP actually).

 */
void CSD::computeMaxPartner(parsedGraph & graph) {
	status("computing maximal %i-partner...", graph.bound);
	pnapi::PetriNet* net = openNet::net;

	std::list<pnapi::Place* > finalplaces;

	// in case there is only one node (the U node) the resulting net is empty
	if (graph.nodes == 1)
		return;

	pnapi::Port * port = &net->getInterface().addPort("port");

	pnapi::Condition * condition = new pnapi::Condition;
	(*condition) = false;


	pnapi::Place * places[graph.nodes];
	pnapi::Label * labels[graph.events];

	for (unsigned int i = 0; i < graph.nodes; ++i) {
		if (i == graph.U) {
			continue;
		}
		places[i] = &net->createPlace(graph.names[i], 0);
		if (i == 0)
			places[i]->setTokenCount(1);
		// add [Q] to the final markings if lambda(Q) = 2
		if (graph.lambdas[i] == 2)
			finalplaces.push_back(places[i]);
	}

	for (Label_ID edge = 0; edge < graph.events; ++edge) {
		if ((*graph.is_sending_label)[edge]) {
			labels[edge] = &net->getInterface().addLabel((*graph.id2name)[edge], pnapi::Label::INPUT, "port");
		} else {
			labels[edge] = &net->getInterface().addLabel((*graph.id2name)[edge], pnapi::Label::OUTPUT, "port");
		}
	}

	pnapi::Place * duplicate = NULL;
	pnapi::Transition * tauTrans = NULL;
	for (unsigned int i = 0; i < graph.nodes; ++i) {
		if (i == graph.U) {
			continue;
		}

		bool has_in_successor = false;
		for (Label_ID edge = 0; edge < graph.events; ++edge) {
			// search for a successor reachable with an input label (except U)
			if (graph.pointer[i][edge] != graph.U && !(*graph.is_sending_label)[edge]) {
				has_in_successor = true;
				break;
			}
		}

		bool dup_exists = false;
		if (graph.lambdas[i] != 1 && has_in_successor) {
			// Q --tau--> Q'
			duplicate = &net->createPlace(graph.names[i] + "'", 0);
			tauTrans = &net->createTransition();
			net->createArc(*places[i], *tauTrans);
			net->createArc(*tauTrans, *duplicate);

			if (graph.lambdas[i] == 2) {
				// add [Q'] to the final markings if lambda(Q) = 2
//				status("adding a place to the final condition");
				finalplaces.push_back(duplicate);
			}

			dup_exists = true;
		}

		for (Label_ID edge = 0; edge < graph.events; ++edge) {
			if (graph.pointer[i][edge] == graph.U) {
				continue;
			}

			// Q --x--> succ(Q)
			pnapi::Transition * transition = &net->createTransition(); // \todo: name?
			transition->addLabel(*labels[edge], 1);
			net->createArc(*places[i], *transition);
			net->createArc(*transition, *places[graph.pointer[i][edge]]);

			if (dup_exists && (*graph.is_sending_label)[edge]) {
				// Q' --x--> succ(Q)
				transition = &net->createTransition(); // \todo: name?
				transition->addLabel(*labels[edge], 1);
				net->createArc(*duplicate, *transition);
				net->createArc(*transition, *places[graph.pointer[i][edge]]);
			}
		}

		duplicate = NULL;
	}

	pnapi::Condition * condition2 = new pnapi::Condition;
	// iterate through all the memorized places to create the final condition
	for (std::list<pnapi::Place* >::const_iterator it = finalplaces.begin(); it != finalplaces.end(); ++it) {
		(*condition2) = (**it == 1);
		condition2->allOtherPlacesEmpty(*openNet::net);
		(*condition) = ((condition->getFormula()) || (condition2->getFormula()));
	}
	delete condition2;

	net->getFinalCondition() = condition->getFormula();
	delete condition;
}
