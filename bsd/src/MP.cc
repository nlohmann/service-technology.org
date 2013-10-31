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
#include "MP.h"
#include "InnerMarking.h"
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
 *------------------- maximal b-partner ------------------
 *========================================================*/

/*!
 \brief Creates the maximal b-partner or mp_b from a CSD automaton (from MP actually).

 \param[in]	graph	the parsed CSD automaton
 \param[in]	max		true for maximal b-partner computation, false for mp_b computation
 */
void MP::computeMaxPartner(parsedGraph & graph, bool max) {
	status("computing maximal %i-partner...", graph.bound);
	pnapi::PetriNet* net = openNet::net;

	// a list to store the places that will become final markings later
	std::list<pnapi::Place* > finalplaces;

	// in case there is only one node (the U node) the resulting net is empty
	if (graph.nodes == 1)
		return;

	// add a port...? cause why not?
	net->getInterface().addPort("port");

	// the future final condition
	pnapi::Condition * condition = new pnapi::Condition;
	(*condition) = false;

	// store all the "normal" places for future reference...
	pnapi::Place ** places = new pnapi::Place*[graph.nodes];
	// and the upcoming labels...
	pnapi::Label ** labels = new pnapi::Label*[graph.events];

	// iterate through all the graph's nodes
	for (unsigned int i = 0; i < graph.nodes; ++i) {
		// ignore the U node
		if (i == graph.U) {
			continue;
		}

		// create a place for the node
		places[i] = &net->createPlace("", 0);
		// set the initial marking for the first node in the graph
		if (i == 0)
			places[i]->setTokenCount(1);
		// add [Q] to the final markings if lambda(Q) = 2
		if (graph.lambdas[i] == 2)
			finalplaces.push_back(places[i]);
	}

	// iterate through all the events
	for (Label_ID edge = 0; edge < graph.events; ++edge) {
		// add a label for each event and reverse the input and output
		if ((*graph.is_sending_label)[edge]) {
			labels[edge] = &net->getInterface().addLabel((*graph.id2name)[edge], pnapi::Label::INPUT, "port");
		} else {
			labels[edge] = &net->getInterface().addLabel((*graph.id2name)[edge], pnapi::Label::OUTPUT, "port");
		}
	}

	// placeholder for duplicated places and tau transitions
	pnapi::Place * duplicate = NULL;
	pnapi::Transition * tauTrans = NULL;
	// now iterate through all the nodes again (we have now places to reference to)
	for (unsigned int i = 0; i < graph.nodes; ++i) {
		// again, ignore the U node
		if (i == graph.U) {
			continue;
		}

		// search for an edge with an input label
		bool has_in_successor = false;
		for (Label_ID edge = 0; edge < graph.events; ++edge) {
			// search for a successor reachable with an input label (except U)
			if (graph.pointer[i][edge] != graph.U && !(*graph.is_sending_label)[edge]) {
				has_in_successor = true;
				break;
			}
		}

		bool added_duplicate = false;
		// in case of computing a maximal b-partner...
		if (max) {
			// if lambda(Q) = 1 and there's an edge from Q with an input label then add a
			// duplicate place Q' and a tau-transition from Q to Q'
			if (graph.lambdas[i] != 1 && has_in_successor) {
				// add Q' and a tau-transition:
				// Q --tau--> Q'
				duplicate = &net->createPlace(places[i]->getName() + "'", 0);
				tauTrans = &net->createTransition();
				net->createArc(*places[i], *tauTrans);
				net->createArc(*tauTrans, *duplicate);

				if (graph.lambdas[i] == 2) {
					// add [Q'] to the final markings if lambda(Q) = 2
					finalplaces.push_back(duplicate);
				}

				// indicate that we have added a duplicate place for future use
				added_duplicate = true;
			}
		}

		// iterate through all the edges of the current node
		for (Label_ID edge = 0; edge < graph.events; ++edge) {
			// ignore edges that lead to the U node
			if (graph.pointer[i][edge] == graph.U) {
				continue;
			}

			// add a transition:
			// Q --x--> succ(Q)
			pnapi::Transition * transition = &net->createTransition(); // \todo: name?
			transition->addLabel(*labels[edge], 1);
			net->createArc(*places[i], *transition);
			net->createArc(*transition, *places[graph.pointer[i][edge]]);

			// if a duplicate place Q' has been added and this is an output edge
			// then add a transition from Q' to the normal successor of this edge
			// (in case of mp_b construction, added_duplicate is always false!)
			if (added_duplicate && (*graph.is_sending_label)[edge]) {
				// Q' --x--> succ(Q)
				transition = &net->createTransition(); // \todo: name?
				transition->addLabel(*labels[edge], 1);
				net->createArc(*duplicate, *transition);
				net->createArc(*transition, *places[graph.pointer[i][edge]]);
			}
		}

		// reset duplicate pointer just to be safe
		duplicate = NULL;
	}

	delete[] places;
	delete[] labels;

	// compute the final condition
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
	// the net is now finished!
}



/*!
 \brief Compute the og formulas for all nodes of the graph

 \param[in]	graph	the CSD automaton (MP automaton)
 */
void MP::computeFormulas(BSDgraph & graph) {
	// iterate through all nodes of the CSD automaton
	for (std::list<BSDNode *>::const_iterator it = graph.graph->begin(); it != graph.graph->end(); ++it) {
		// skip the U node(s)
		if ((*it)->isU) {
			continue;
		}
		// iterate through all SCCs of the node
		for (std::list<MarkingList>::const_iterator itSCC = (*it)->SCCs.begin(); itSCC != (*it)->SCCs.end(); ++itSCC) {
			std::list<Label_ID> disjunction;
			// iterate through the markings of the SCC
			for (MarkingList::const_iterator itlist = itSCC->begin(); itlist != itSCC->end(); ++itlist) {
				// iterate through all successor labels
				for (uint8_t i = 0; i < InnerMarking::inner_markings[*itlist]->out_degree; ++i) {
					if (InnerMarking::inner_markings[*itlist]->labels[i] != TAU) {
						// skip labels that lead to the U node (we are in MP)
						if ((*it)->pointer[InnerMarking::inner_markings[*itlist]->labels[i]]->isU) {
							continue;
						}
						Label_ID currentLabel = InnerMarking::inner_markings[*itlist]->labels[i];
						// test if the label was already found in the SCC
						bool found_label_in_SCC = false;
						// iterate through the list of labels found so far in the SCC
						for (std::list<Label_ID>::const_iterator itdis = disjunction.begin(); itdis != disjunction.end(); ++itdis) {
							if (*itdis == currentLabel) {
								found_label_in_SCC = true;
								break;
							}
						}
						// if the label wasn't already in the list then add it
						if (!found_label_in_SCC) {
							disjunction.push_back(currentLabel);
						}
					}
				}
			}
			// we added all labels going out of markings of the current SCC to the disjunction
			// now we search for a final marking in the SCC
			for (MarkingList::const_iterator itfin = itSCC->begin(); itfin != itSCC->end(); ++itfin) {
				if (InnerMarking::inner_markings[*itfin]->is_final) {
					// add 0 to the disjunction (we encode 'final' as 0 which is actually the TAU label...
					// ... which we don't have in the formulas, so it's fine)
					disjunction.push_back(0);
					break;
				}
			}

			if (!disjunction.empty()) {
				// sort the labels in the disjunction and add them to the main formula (conjunction)
				disjunction.sort();
				(*it)->formula.push_back(disjunction);
			}
		}

		(*it)->formula.unique();
	}
}
