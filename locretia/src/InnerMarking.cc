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
#include <climits>
#include <algorithm>
#include <time.h>
#include <string>
#include <sstream>
#include "InnerMarking.h"
#include "Label.h"

#include "verbose.h"
#include "util.h"

using std::set;
using std::vector;


/******************
 * STATIC MEMBERS *
 ******************/

std::map<InnerMarking_ID, InnerMarking*> InnerMarking::markingMap;
std::map<InnerMarking_ID, bool> InnerMarking::finalMarkingReachableMap;
pnapi::PetriNet* InnerMarking::net = new pnapi::PetriNet();
bool InnerMarking::is_acyclic = true;
InnerMarking** InnerMarking::inner_markings = NULL;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::receivers;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::senders;
std::map<Label_ID, std::set<InnerMarking_ID> > InnerMarking::synchs;
InnerMarking::_stats InnerMarking::stats;
pnapi::Place* InnerMarking::counterPlace;

/******************
 * STATIC METHODS *
 ******************/

/*!
 Copy the markings from the mapping markingMap to C-style arrays.
 Additionally, a mapping is filled to quickly determine whether a marking can
 become transient if a message with a given label was sent to the net.

 \todo replace the mapping receivers and synchs by a single two-dimensional
       C-style array or do this check in the constructor
 */
void InnerMarking::initialize() {
    assert(stats.markings == markingMap.size());
    inner_markings = new InnerMarking*[stats.markings];

    // copy data from STL mapping (used during parsing) to a C array
    for (InnerMarking_ID i = 0; i < stats.markings; ++i) {
        assert(markingMap[i]);

        inner_markings[i] = markingMap[i];
        inner_markings[i]->is_bad = (inner_markings[i]->is_bad or
                                     (finalMarkingReachableMap.find(i) != finalMarkingReachableMap.end()
                                      and finalMarkingReachableMap[i] == false));

        // register markings that may become activated by sending a message
        // to them, by receiving a message or by synchronization
        for (uint8_t j = 0; j < inner_markings[i]->out_degree; ++j) {
            if (SENDING(inner_markings[i]->labels[j])) {
                receivers[inner_markings[i]->labels[j]].insert(i);
            }
            if (RECEIVING(inner_markings[i]->labels[j])) {
                senders[inner_markings[i]->labels[j]].insert(i);
            }
            if (SYNC(inner_markings[i]->labels[j])) {
                synchs[inner_markings[i]->labels[j]].insert(i);
            }
        }
    }

    // destroy temporary STL mappings
    markingMap.clear();
    finalMarkingReachableMap.clear();

    status("stored %d inner markings (%d final, %d bad, %d inevitable deadlocks)",
           stats.markings, stats.final_markings, stats.bad_states, stats.inevitable_deadlocks);

    if (stats.final_markings == 0) {
        message("%s: %s", _cimportant_("warning"), _cwarning_("no final marking found"));
    }
}

/*!
 \brief Change the viewpoint to the environment.

 For each interface place (or  label), add a real place (interface places are only implicit) and
 a transition which represents the actions of the environment.
 Add an additional place with 'c' tokens which restricts the input and output transitions
 from unbounded firing to a maximum of 'c'.

 \param[in]	net	the petri net on which the function is used on
 \param[in]	c	maximal number of messages to be sent and received by the environment through each channel
 */
void InnerMarking::changeView(pnapi::PetriNet* net, const int c) {
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

void InnerMarking::deleteCounterPlace() {
	// delete the counter place (for output)
	net->deletePlace(*counterPlace);
}


/*!
 \brief Adds a random Interface to the net. Every (visible) transition gets either an input or an output label.

 \param[in]	count	the maximal number of created interface labels
 */
void InnerMarking::addInterface(int count) {
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
			//(*t)->setName(s.erase(s.length()-13));
		}
	}

	int choose = 0;

	// if the count of requested interface places is higher than the count of
	// visible transitions then a "full" interface is generated
	if (count >= visible_trans.size())
		count = -1;

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
 \brief Checks if a final state is reachable with the given maximal trace length. Iterative depth search is used.

 \param[in]		trace_max_length	the given maximal length of a trace

 \return minimal trace length to be able to reach a final state or '-1' if no final state is reachable
 */
int InnerMarking::isFinalStateReachable(const int trace_max_length) {
	status("checking if a final state is reachable in %i steps...", trace_max_length);

	int result = -1;

	for (int depth = 1; depth <= trace_max_length; ++depth) {
		int counter = 0;
		result = traverse(0, depth, counter);

		if (result != -1)
			break;
	}

	if (result == -1) {
		status("no final state is reachable!");
	} else {
		status("at least one final state is reachable in %i steps!", result);
	}

	return result;
}

/*!
 \brief Help funtion for the 'isFinalStateReachable'-function

 \param[in]		markingID		the ID of the current node
 \param[in]		depth			the current depth of the iterative depth search
 \param[in]		counter			the amount of steps taken from the initial node to the current node

 \return amount of steps to reach a final state or '-1' if no final state reachable
 */
int InnerMarking::traverse(const InnerMarking_ID& markingID, int depth, int counter) {
	if (inner_markings[markingID]->is_final)
		return counter;
	if (--depth < 0)
		return -1;

	++counter;
	for (InnerMarking_ID i = 0; i < inner_markings[markingID]->out_degree; ++i) {
		int result = traverse(inner_markings[markingID]->successors[i], depth, counter);
		if (result != -1)
			return result;
	}

	return -1;
}

/*!
 \brief Adds the final condition to the OWFN built from the TPN
 */
std::string InnerMarking::addFinalCondition() {
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

void InnerMarking::finalize() {
    delete net;
    for (InnerMarking_ID i = 0; i < stats.markings; ++i) {
        delete inner_markings[i];
    }
    delete[] inner_markings;

    status("InnerMarking: deleted %d objects", stats.markings);
}


/***************
 * CONSTRUCTOR *
 ***************/

InnerMarking::InnerMarking(const InnerMarking_ID& myId,
                           const std::vector<Label_ID>& _labels,
                           const std::vector<InnerMarking_ID>& _successors,
                           const bool& _is_final)
    : is_final(_is_final), is_waitstate(0), is_bad(0),
      out_degree(_successors.size()), successors(new InnerMarking_ID[out_degree]),
      labels(new Label_ID[out_degree]) {
    assert(_labels.size() == out_degree);
    assert(out_degree < UCHAR_MAX);

    if (++stats.markings % 50000 == 0) {
        message("%8d inner markings", stats.markings);
    }

    // copy given STL vectors to C arrays
    std::copy(_labels.begin(), _labels.end(), labels);
    std::copy(_successors.begin(), _successors.end(), successors);

    // knowing all successors, we can determine the type of the marking...
    determineType(myId);
}


InnerMarking::~InnerMarking() {
    delete[] labels;
    delete[] successors;
}


/******************
 * MEMBER METHODS *
 ******************/

/*!
 The type is determined by checking the labels of the leaving transitions as
 well as the fact whether this marking is a final marking. For the further
 processing, it is sufficient to distinguish three types:

 - the marking is a bad state (is_bad) -- then a knowledge containing
   this inner marking can immediately be considered insane
 - the marking is a final marking (is_final) -- this is needed to distinguish
   deadlocks from final markings
 - the marking is a waitstate (is_waitstate) -- a waitstate is a marking of
   the inner of the net that can only be left by firing a transition that is
   connected to an input place

 This function also implements the detection of inevitable deadlocks. A
 marking is an inevitable deadlock, if it is a deadlock or all its successor
 markings are inevitable deadlocks. The detection exploits the way LoLA
 returns the reachability graph: when a marking is returned, we know that
 the only successors we have not considered yet (i.e. those markings where
 successors[i] == NULL) are also predecessors of this marking and cannot be
 a reason for this marking to be a deadlock. Hence, this marking is an
 inevitable deadlock if all known successors are deadlocks.

 \note except is_final, all types are initialized with 0, so it is sufficient
       to only set values to 1
 */
void InnerMarking::determineType(const InnerMarking_ID& myId) {
    bool is_transient = false;

    // deadlock: no successor markings and not final
    if (out_degree == 0 and not is_final) {
        ++stats.bad_states;
        is_bad = 1;
    }

    if (is_final) {
        ++stats.final_markings;
    }

    // when only deadlocks are considered, we don't care about final markings
    finalMarkingReachableMap[myId] = true;

    // variable to detect whether this marking has only deadlocking successors
    // standard: "true", otherwise evaluate noDeadlockDetection flag
    bool deadlock_inevitable = true;
    for (uint8_t i = 0; i < out_degree; ++i) {
        // if a single successor is not a deadlock, everything is OK
        if (markingMap[successors[i]] != NULL and
                deadlock_inevitable and
                not markingMap[successors[i]]->is_bad) {
            deadlock_inevitable = false;
        }

        // if we have not seen a successor yet, we can't be a deadlock
        if (markingMap[successors[i]] == NULL) {
            deadlock_inevitable = false;
        }

        // a tau or sending (sic!) transition makes this marking transient
        if (SILENT(labels[i]) or RECEIVING(labels[i])) {
            is_transient = true;
        }
    }

    // deadlock cannot be avoided any more -- treat this marking as deadlock
    if (not is_final and
            not is_bad and
            deadlock_inevitable) {
        is_bad = 1;
        ++stats.inevitable_deadlocks;
    }

    // draw some last conclusions
    if (not(is_transient or is_bad)) {
        is_waitstate = 1;
    }
}

