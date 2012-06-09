/*****************************************************************************\
 Wendy -- Synthesizing Partners for Services

 Copyright (c) 2009 Niels Lohmann, Christian Sura, and Daniela Weinberg

 Wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Wendy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <config.h>
#include <climits>
#include <algorithm>
#include <time.h>
#include "InnerMarking.h"
#include "Label.h"
#include "cmdline.h"
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
 As for input labels, add an additional place with 'c' tokens which restricts the input transitions
 from unbounded firing to a maximum of 'c'.

 \param[in]	c	maximal number of messages to be sent(!) by the environment through each channel
 	 	 	 	 (receiving messages is not restricted)
 */
void InnerMarking::changeView(const int c) {
	status("changing viewpoint to environment...");

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

void InnerMarking::createLabeledEnvironment() {
	status("creating labeled sync/async environment...");

	// iterate through input labels
	set<pnapi::Label *> inputs = net->getInterface().getInputLabels();
	PNAPI_FOREACH(label, inputs)
	{
		// iterate through all transitions belonging to the current label
		set<pnapi::Transition *> t_in = (*label)->getTransitions();
		PNAPI_FOREACH(t, t_in)
		{
			// remove the transitions from the current label
			(*t)->removeLabel(**label);
			(*label)->removeTransition(**t);

			// label the transition with the input label
			(*t)->setName((*label)->getName());
		}
	}

	// iterate through output labels
	inputs = net->getInterface().getOutputLabels();
	PNAPI_FOREACH(label, inputs)
	{
		// iterate through all transitions belonging to the current label
		set<pnapi::Transition *> t_in = (*label)->getTransitions();
		PNAPI_FOREACH(t, t_in)
		{
			// remove the transitions from the current label
			(*t)->removeLabel(**label);
			(*label)->removeTransition(**t);

			// label the transition with the output label
			(*t)->setName((*label)->getName());
		}
	}
	//\TODO: what about synchronous labels?...

	net->getInterface().clear();

	status("done with creating labeled sync/async environment...");
}

/****************************************
 * OUTPUT FUNCTIONS (STATIC AND MEMBER) *
 ****************************************/

/*
 * the header of the XES log
 */
void InnerMarking::fileHeader(std::ostream& file, std::string& filename) {
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
    	 << "<log xes.version=\"1.0\" xes.features=\"nested-attributes\" openxes.version=\"1.0RC7\" xmlns=\"http://www.xes-standard.org\">\n"
    	 << "\t<extension name=\"Lifecycle\" prefix=\"lifecycle\" uri=\"http://www.xes-standard.org/lifecycle.xesext\"/>\n"
    	 << "\t<extension name=\"Organizational\" prefix=\"org\" uri=\"http://www.xes-standard.org/org.xesext\"/>\n"
    	 << "\t<extension name=\"Time\" prefix=\"time\" uri=\"http://www.xes-standard.org/time.xesext\"/>\n"
    	 << "\t<extension name=\"Concept\" prefix=\"concept\" uri=\"http://www.xes-standard.org/concept.xesext\"/>\n"
    	 << "\t<extension name=\"Semantic\" prefix=\"semantic\" uri=\"http://www.xes-standard.org/semantic.xesext\"/>\n"
    	 << "\t<global scope=\"trace\">\n"
    	 << "\t\t<string key=\"" << TRACE_KEY_NAME << "\" value=\"__INVALID__\"/>\n"
//    	 << "\t\t<int key=\"" << TRACE_KEY_LENGTH << "\" value=\"0\"/>\n"
    	 <<	"\t</global>\n"
    	 <<	"\t<global scope=\"event\">\n"
    	 <<	"\t\t<string key=\"" << EVENT_KEY_LABEL << "\" value=\"__INVALID__\"/>\n"
    	 << "\t\t<string key=\"lifecycle:transition\" value=\"complete\"/>\n"
//    	 <<	"\t\t<int key=\"" << EVENT_KEY_NUMBER << "\" value=\"0\"/>\n"
    	 <<	"\t</global>\n"
    	 << "\t<classifier name=\"MXML Legacy Classifier\" keys=\"concept:name lifecycle:transition\"/>\n"
    	 << "\t<classifier name=\"" << EVENT_CLASSIFIER << "\" keys=\"" << EVENT_KEY_LABEL << "\"/>\n"
    	 << "\t<classifier name=\"Resource\" keys=\"org:resource\"/>\n"
    	 << "\t<string key=\"source\" value=\"" << PACKAGE_NAME << " " << PACKAGE_VERSION << "\"/>\n"
    	 << "\t<string key=\"concept:name\" value=\"" << filename << "\"/>\n"
    	 << "\t<string key=\"lifecycle:model\" value=\"standard\"/>\n";
}

/*
 * the footer of the XES log
 */
void InnerMarking::fileFooter(std::ostream& file) {
    file << "</log>";
}

/*
 * traverses the reachability graph of the net (just for test purposes...)
 */
void InnerMarking::traverse(const InnerMarking_ID& markingID) {
	for (InnerMarking_ID i = 0; i < inner_markings[markingID]->out_degree; ++i) {
		status("edge from node %i to node %i with label %s", markingID, inner_markings[markingID]->successors[i], Label::id2name[inner_markings[markingID]->labels[i]].c_str());
		traverse(inner_markings[markingID]->successors[i]);
	}
}

/*!
 \brief creates a trace of the given net and writes it to the given output stream.

 \param[in,out]	file 			the output stream to write the trace to
 \param[in]		trace_number  	the number of the trace
 \param[in]		trace_maxlength	the maximal length of the trace
 */
void InnerMarking::create_trace(std::ostream& file, const int trace_number, const int trace_maxlength) {
	status("creating trace %i with maximal length %i", trace_number, trace_maxlength);

	time_t rawtime;
	struct tm * ti;
	time(&rawtime);
	ti = localtime ( &rawtime );

	std::stringstream timestring (std::stringstream::in | std::stringstream::out);

	int year = ti->tm_year + 1900;
	timestring << year << "-";
	ti->tm_mon++;
	if (ti->tm_mon < 10) timestring << "0";
	timestring << ti->tm_mon << "-";
	if (ti->tm_mday < 10) timestring << "0";
	timestring << ti->tm_mday << "T";
	if (ti->tm_hour < 10) timestring << "0";
	timestring << ti->tm_hour << ":";
	if (ti->tm_min < 10) timestring << "0";
	timestring << ti->tm_min << ":";
	if (ti->tm_sec < 10) timestring << "0";
	timestring << ti->tm_sec;

	InnerMarking_ID id = 0;
	int counter = 0, choose = 0;
	std::stringstream tempstring (std::stringstream::in | std::stringstream::out);
	// create events until the maximal length is reached or no transitions can fire
	while (counter < trace_maxlength && inner_markings[id]->out_degree > 0) {
		++counter;
		//choose a transition from outgoing transitions
		choose = std::rand() % inner_markings[id]->out_degree;

		//create log entry for event
		tempstring << "\t\t<event>\n"
				   << "\t\t\t<string key=\"org:resource\" value=\"UNDEFINED\"/>\n"
				   << "\t\t\t<date key=\"time:timestamp\" value=\"" << timestring.str() << /*".000+00:00*/"\"/>\n"
				   << "\t\t\t<string key=\"" << EVENT_KEY_LABEL << "\" value=\"" << Label::id2name[inner_markings[id]->labels[choose]] << "\"/>\n"
				   //<< "\t\t\t<int key=\"" << EVENT_KEY_NUMBER << "\" value=\"" << counter << "\"/>\n"
				   << "\t\t\t<string key=\"lifecycle:transition\" value=\"complete\"/>\n"
				   << "\t\t</event>\n";
		//set successor marking id
		id = inner_markings[id]->successors[choose];
	}

	// write the trace to the output stream
	file << "\t<trace>\n"
		 << "\t\t<string key=\"" << TRACE_KEY_NAME << "\" value=\"Case" << trace_number << ".0\"/>\n"
		 //<< "\t\t<int key=\"" << TRACE_KEY_LENGTH << "\" value=\"" << counter << "\"/>\n"
		 << tempstring.str()
		 << "\t</trace>\n";

	tempstring.clear();
//	status("done with trace %i, length: %i", trace_number, counter);
}

/*!
 \brief Creates a XES log with traces of the given net and writes it to the given output stream.

 \param[in,out]	file 				the output stream to write the log to
 \param[in]		trace_count  		the total number of traces to be created
 \param[in]		trace_min_length	the minimal length of a trace
 \param[in]		trace_max_length	the maximal length of a trace
 */
void InnerMarking::create_log(std::ostream& file, std::string& filename,
												  const int trace_count ,
												  const int trace_min_length,
												  const int trace_max_length) {
	status("creating log-file with %i traces, each with lengths %i to %i",
			trace_count, trace_min_length, trace_max_length);

    fileHeader(file, filename);

    // initialize the random number generator
    std::srand(time(NULL));

    int length, counter = 0;
    // create 'trace_count' traces
    for (int i = 0; i < trace_count; ++i) {
    	// randomize the (maximal) length of the next trace
    	length = (std::rand() % (trace_max_length + 1 - trace_min_length)) + trace_min_length;
    	create_trace(file, ++counter, length);
    }

    fileFooter(file);

    status("log-file done with %i traces, each with lengths %i to %i",
    			trace_count, trace_min_length, trace_max_length);
}

/*!
 \brief Adds a random Interface to the net. Every transition gets either an input or an output label.
 */
void InnerMarking::addInterface(const int count) {
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
 \brief Adds the final condition to the net. \TODO
 */
void InnerMarking::addFinalCondition() {
	//net->getFinalCondition();
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

void InnerMarking::output_results(Results& r) {
    r.add("statistics.inner_markings", InnerMarking::stats.markings);
    r.add("statistics.inner_markings_final", InnerMarking::stats.final_markings);
    r.add("statistics.inner_markings_bad", InnerMarking::stats.bad_states);
    r.add("statistics.inner_markings_inevitable_bad", InnerMarking::stats.inevitable_deadlocks);
}
