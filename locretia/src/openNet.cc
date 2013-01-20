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
#include "openNet.h"

#include "verbose.h"
#include "util.h"

using std::set;
using std::vector;


/******************
 * STATIC MEMBERS *
 ******************/

pnapi::PetriNet* openNet::net = new pnapi::PetriNet();
pnapi::Place* openNet::counterPlace;

/******************
 * STATIC METHODS *
 ******************/

/*!
 \brief Change the viewpoint to the environment.

 For each interface place (or label), add a real place (interface places are only implicit) and
 a transition which represents the actions of the environment.
 Add an additional place with 'c' tokens which restricts the input and output transitions
 from unbounded firing to a maximum of 'c'.

 \param[in]	net	the petri net on which the function is used on
 \param[in]	c	maximal number of messages to be sent and received by the environment through each channel
 */
void openNet::changeView(pnapi::PetriNet* net, const int c) {
	status("changing viewpoint to asynchronous environment...");

	// create a place which represents the maximal count of messages to be sent...
	counterPlace = &net->createPlace("counter_place", c);

	// iterate through input labels
	set<pnapi::Label *> inputs = net->getInterface().getInputLabels();
	PNAPI_FOREACH(label, inputs)
	{
		// create a place which represents the interface
		pnapi::Place *p = &net->createPlace((*label)->getName() + "_input");

		// iterate through all transitions belonging to the current label
		set<pnapi::Transition *> t_in = (*label)->getTransitions();
		PNAPI_FOREACH(t, t_in)
		{
			// remove the transitions from the current label
			(*t)->removeLabel(**label);
			(*label)->removeTransition(**t);

			// add an arc from the "interface place" to the transition
			// \TODO: weight????!!!
			net->createArc(*p,**t, 1);
		}

		// create a new transition (new input transition)
		pnapi::Transition *t = &net->createTransition((*label)->getName() + "_in");
		// add label
		// \TODO: weight????!!!
		t->addLabel(**label, 1);

		// add an arc from the new transition to the "interface place"
		net->createArc(*t, *p, 1);
		// ...and one from the "counter place" to the new transition
		net->createArc(*counterPlace, *t, 1);
	}

	// iterate through output labels
	inputs = net->getInterface().getOutputLabels();
	PNAPI_FOREACH(label, inputs)
	{
		//create a place which represents the interface
		pnapi::Place *p = &net->createPlace((*label)->getName() + "_output");

		// iterate through all transitions belonging to the current label
		set<pnapi::Transition *> t_in = (*label)->getTransitions();
		PNAPI_FOREACH(t, t_in)
		{
			// remove the transitions from the current label
			(*t)->removeLabel(**label);
			(*label)->removeTransition(**t);

			// add an arc from the transition to the "interface place"
			// \TODO: weight????!!!
			net->createArc(**t, *p, 1);
			// ...and one from the "counter place" to the transition
			net->createArc(*counterPlace, **t, 1);
		}

		// create a new transition (new output transition)
		pnapi::Transition *t = &net->createTransition((*label)->getName() + "_out");
		// add label
		// \TODO: weight????!!!
		t->addLabel(**label,1);

		// add an arc from the "interface place" to the new transition
		net->createArc(*p, *t, 1);
	}

	//\TODO: what about synchronous labels?...
}

// delete the counter place (for output)
void openNet::deleteCounterPlace() {
	net->deletePlace(*counterPlace);
}


/*!
 \brief Adds a random Interface to the net. Every (visible) transition (suffix != "$invisible$") gets either an input or an output label.

 \param[in]	count	the maximal number of created interface labels
 */
void openNet::addInterface(int count) {
	status("adding %i random interface transitions...", count);
	net->createPort(PORT_NAME);
	// initialize the random number generator
	std::srand(time(NULL));

	// vector of visible transitions
	vector<pnapi::Transition *> visible_trans;

	std::string s;
	pnapi::Label * l = NULL;

	// iterate through all transitions
	set<pnapi::Transition *> t_in = net->getTransitions();
	PNAPI_FOREACH(t, t_in)
	{
		s = (*t)->getName();
		// if on the end of the transition's name is a '$', the transition is invisible
		// -> "... $invisible$"
		if (*(--s.end()) != '$') {
			visible_trans.push_back(*t);
		} else {
			// delete the $invisible$-tag (if needed)
			//(*t)->setName(s.erase(s.length()-13));
		}
	}

	int choose = 0;

	// if the number of requested interface places is higher than the number of
	// visible transitions then a "full" interface is generated
	if (count >= visible_trans.size()) {
		count = -1;
		status("The number of requested interface places is higher than the number of visible transitions. Reducing to %i places and thus generating a full interface.", visible_trans.size());
	}

	// the following is split up for reasons of performance...
	if (count > visible_trans.size()/2 || count == -1) {
		// if there are more than half of the visible transitions to be added to the interface
		// delete randomly transitions until there are only <count> transitions left
		if (count != -1 && count < visible_trans.size()) {
			int border = visible_trans.size() - count;
			for (int i=0; i < border; i++) {
				choose = std::rand() % visible_trans.size();
				visible_trans.erase(visible_trans.begin() + choose);
			}
		}

		// add the remaining transitions to the interface
		for (int i=0; i < visible_trans.size(); i++) {
			pnapi::Transition * t = visible_trans[i];
			s = t->getName();

			// remove the "\n"
			s.erase(s.length()-2, 2);
			// choose randomly between input and output labels
			if (std::rand() % 2) {
				l = &net->createInputLabel(s, PORT_NAME);
			} else {
				l = &net->createOutputLabel(s, PORT_NAME);
			}

			// add the label to the transition
			t->addLabel(*l);
		}
	} else {
		// else choose randomly transitions and add them to the interface

		for (int i=0; i < count; i++) {
			choose = std::rand() % visible_trans.size();
			pnapi::Transition * t = visible_trans[choose];
			s = t->getName();

			// remove the "\n"
			s.erase(s.length()-2, 2);
			// choose randomly between input and output labels
			if (std::rand() % 2) {
				l = &net->createInputLabel(s, PORT_NAME);
			} else {
				l = &net->createOutputLabel(s, PORT_NAME);
			}

			// add the label to the transition
			t->addLabel(*l);

			// erase already used transitions
			visible_trans.erase(visible_trans.begin() + choose);
		}
	}
}


/*!
 \brief Adds the final condition to the OWFN built from the TPN

 It is assumed that the final place in the TPN is labeled "B\\n".
 */
std::string openNet::addFinalCondition() {
	// search for the final place...
	std::string placeID_B = "";
	set<pnapi::Arc *> s = net->getArcs();
	PNAPI_FOREACH(it, s)
	{
		if ((*it)->getSourceNode().getName() == "B\\n") {
			placeID_B = (*it)->getTargetNode().getName();
			status("final place has the id: '%s'", placeID_B.c_str());
		}
	}
	// the label of the final place is now saved in 'placeID_B'
	std::stringstream replaceString (std::stringstream::in | std::stringstream::out);
	replaceString << "FINALMARKING\n" << "  " << placeID_B << ":\t1\n" << " ;";

	// now change the final condition of the OWFN... (hacking... so what?)
	std::stringstream tempString (std::stringstream::in | std::stringstream::out);
	tempString << pnapi::io::owfn << *net;
	std::string netString = tempString.str();

	try {
		if (placeID_B != "") {
			// in the OWFN the final condition is set to TRUE... so replace that bs
			netString.replace(netString.find("FINALCONDITION"), 22, replaceString.str());
			status("changed the final marking of the OWFN");
		}
	} catch (std::exception& e) {
		status("could not change the final marking...");
	}

	return netString;
}
