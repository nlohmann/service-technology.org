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
#include "openNet.h"

#include "verbose.h"
#include "util.h"

using std::set;
using std::vector;


/******************
 * STATIC MEMBERS *
 ******************/

pnapi::PetriNet* openNet::net = NULL;

/******************
 * STATIC METHODS *
 ******************/

void openNet::initialize() {
	net = new pnapi::PetriNet();
}

/*!
 \brief Change the viewpoint to the environment.

 For each interface place (or label), add a real place (interface places are only implicit) and
 a transition which represents the actions of the environment.
 Add an additional complement place for each interface transition with 'b' tokens which restricts
 the input and output transitions from unbounded firing to a maximum of 'b' tokens lying on the
 interface places.

 \param[in]	net	the petri net on which the function is used on
 \param[in]	b	bound b
 */
void openNet::changeView(pnapi::PetriNet* net, const int b) {
	status("changing viewpoint to asynchronous environment...");

//	// create a place which represents the maximal count of messages to be sent...
//	counterPlace = &net->createPlace("counter_place", c);

	// iterate through input labels
	set<pnapi::Label *> inputs = net->getInterface().getInputLabels();
	PNAPI_FOREACH(label, inputs)
	{
		// create a place which represents the interface
		pnapi::Place *p = &net->createPlace((*label)->getName() + "_input");

		// create a complement place with bound b
		pnapi::Place *compPlace = &net->createPlace((*label)->getName() + "_comp", b+1);

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

			// add an arc from the transition to the complement place
			net->createArc(**t, *compPlace, 1);
		}

		// create a new transition (new input transition)
		pnapi::Transition *t = &net->createTransition((*label)->getName() + "_in");
		// add label
		// \TODO: weight????!!!
		t->addLabel(**label, 1);

		// add an arc from the new transition to the "interface place"
		net->createArc(*t, *p, 1);
		// ...and one from the complement place to the new transition
		net->createArc(*compPlace, *t, 1);
	}

	// iterate through output labels
	inputs = net->getInterface().getOutputLabels();
	PNAPI_FOREACH(label, inputs)
	{
		//create a place which represents the interface
		pnapi::Place *p = &net->createPlace((*label)->getName() + "_output");

		// create a complement place with bound b
		pnapi::Place *compPlace = &net->createPlace((*label)->getName() + "_comp", b+1);

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

			// add an arc from the complement place to the transition
			net->createArc(*compPlace, **t, 1);
		}

		// create a new transition (new output transition)
		pnapi::Transition *t = &net->createTransition((*label)->getName() + "_out");
		// add label
		// \TODO: weight????!!!
		t->addLabel(**label,1);

		// add an arc from the "interface place" to the new transition
		net->createArc(*p, *t, 1);
		// ...and one from the new transition to the complement place
		net->createArc(*t, *compPlace, 1);
	}

	//\TODO: what about synchronous labels?...

	// made bound_broken a sync label just for better recognition... \todo
	pnapi::Label *label = &net->getInterface().addSynchronousLabel("bound_broken", "");
	set<pnapi::Place *> places = net->getPlaces();
	PNAPI_FOREACH(place, places)
	{
		if ((*place)->getName().find("_comp") == std::string::npos) {
			// create a new transition (new input transition)
			pnapi::Transition *t = &net->createTransition();
			// add label
			// \TODO: weight????!!!
			t->addLabel(*label, b+1);

			// add arcs
			net->createArc(*t, **place, b+1);
			net->createArc(**place, *t, b+1);
		}
	}
}





