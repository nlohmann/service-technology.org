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
#include <math.h>
#include "generateLog.h"
#include "InnerMarking.h"
#include "serviceAutomaton.h"
#include "Label.h"

#include "verbose.h"

using std::set;
using std::vector;


/******************
 * STATIC MEMBERS *
 ******************/


/******************
 * STATIC METHODS *
 ******************/

/****************************************
 * OUTPUT FUNCTIONS (STATIC AND MEMBER) *
 ****************************************/

/*!
 \brief Creates a XES log with traces of the given net and writes it to the given output stream.

 \param[in,out]	file 				the output stream to write the log to
 \param[in,out]	filename			the filename
 \param[in]		isOWFN				is the input an OWFN? (or a SA)
 \param[in]		trace_count  		the total number of traces to be created
 \param[in]		trace_min_length	the minimal length of a trace
 \param[in]		trace_max_length	the maximal length of a trace
 \param[in]		repeated_max		the maximal number of repeated traces
 \param[in]		is_exp				sets exponential distribution
 \param[in]		finalEnd			only generate traces that end in a final state?
 \param[in]		enforceTraceCount	enforces the total number of traces to be created
 */
void generateLog::createLog(std::ostream& file, std::string& filename, const bool isOWFN,
												  const int trace_count ,
												  const int trace_min_length,
												  const int trace_max_length,
												  const int repeated_max,
												  const bool is_exp,
												  const double lambda,
												  const bool finalEnd,
												  const bool enforceTraceCount) {
	status("creating log-file with %i traces, each with lengths %i to %i",
			trace_count, trace_min_length, trace_max_length);

    fileHeader(file, filename);

    // initialize the random number generator
    std::srand(time(NULL));

    int length, repeatedcount = 1, counter = 1;
    bool finishedTrace = false;

    double r = trace_max_length - trace_min_length + 1;
    //double lambda = exp(1);
    //double normalizer = lambda * exp(lambda) - 1;
    double normalizer = 1 - exp(-lambda);

    // create 'trace_count' traces
    for (int i = 0; i < trace_count; ++i) {
    	// randomize the (maximal) length of the next trace
    	length = trace_min_length + (int)(r * std::rand()/(RAND_MAX+1.0));
    	//length = (std::rand() % (trace_max_length + 1 - trace_min_length)) + trace_min_length;

    	if (repeated_max > 1) {
    		// randomize the number of repeated traces based on the current trace
    		if (is_exp) {
    			// use exponential distribution
    			//repeatedcount = 1 + (int)((double)repeated_max * (lambda * (exp(lambda * std::rand()/(RAND_MAX+1.0))) - 1) / normalizer);
    			repeatedcount = 1 + (int)((double)repeated_max * (double)(1 - exp(-lambda * std::rand()/(RAND_MAX+1.0))) / normalizer);
    		} else {
    			// use equal distribution
    			repeatedcount = 1 + (int)((double)repeated_max * std::rand()/(RAND_MAX+1.0));
    		}
    		//status("repeat: %i, %f", repeatedcount, (1 - exp(-exp(1)*std::rand()/(RAND_MAX+1.0)))/(1-exp(-exp(1))));
    	}

    	finishedTrace = false;

    	// If the log has to contain a defined number of traces then repeat the procedure of creating
    	// a trace until it is written to the output. This may not be the case if the trace doesn't end
    	// in a final state and the user requested only to create those traces.
    	// Else just discard the current trace and start with a new one.
    	if (enforceTraceCount)
    		while(!finishedTrace)
    			finishedTrace = create_trace(file, isOWFN, counter, length, repeatedcount, finalEnd);
    	else
    		finishedTrace = create_trace(file, isOWFN, counter, length, repeatedcount, finalEnd);

    	if (finishedTrace)
    		++counter;
    }

    fileFooter(file);

    status("log-file done with %i traces, each with lengths %i to %i",
    			trace_count, trace_min_length, trace_max_length);
}

/*
 * the header of the XES log
 */
void generateLog::fileHeader(std::ostream& file, std::string& filename) {
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
void generateLog::fileFooter(std::ostream& file) {
    file << "</log>";
}


/*!
 \brief creates a trace of the given net and writes it to the given output stream.

 \param[in,out]	file 			the output stream to write the trace to
 \param[in]		isOWFN			is the input an OWFN? (or a SA)
 \param[in]		trace_number  	the number of the trace
 \param[in]		trace_maxlength	the maximal length of the trace
 \param[in]		repeatedcount	the number this trace is repeated in the log
 \param[in]		finalEnd		only generate traces that end in a final state?

 \return bool if the trace has been written to the output
 */
bool generateLog::create_trace(std::ostream& file, const bool isOWFN, const int trace_number,
							   const int trace_max_length, const int repeated_count, const bool finalEnd) {
	status("creating trace %i with maximal length %i", trace_number, trace_max_length);

	bool trace_is_final = false;

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

	int counter = 0, choose = 0;
	std::stringstream tempstring (std::stringstream::in | std::stringstream::out);
	// create events until the maximal length is reached or no transitions can fire
	if (isOWFN) {
		// if the input an OWFN then create the trace based on the reachability graph of the inner:
		InnerMarking_ID id = 0;
		while (counter < trace_max_length && InnerMarking::inner_markings[id]->out_degree > 0) {
			++counter;
			//choose a transition from outgoing transitions
			choose = std::rand() % InnerMarking::inner_markings[id]->out_degree;

			//create log entry for event
			tempstring << "\t\t<event>\n"
					<< "\t\t\t<string key=\"org:resource\" value=\"UNDEFINED\"/>\n"
					<< "\t\t\t<date key=\"time:timestamp\" value=\"" << timestring.str() << ".000+00:00\"/>\n"
					<< "\t\t\t<string key=\"" << EVENT_KEY_LABEL << "\" value=\"" << Label::id2name[InnerMarking::inner_markings[id]->labels[choose]] << "\"/>\n"
					//<< "\t\t\t<int key=\"" << EVENT_KEY_NUMBER << "\" value=\"" << counter << "\"/>\n"
					<< "\t\t\t<string key=\"lifecycle:transition\" value=\"complete\"/>\n"
					<< "\t\t</event>\n";
			//set successor marking id
			id = InnerMarking::inner_markings[id]->successors[choose];
		}
		if (InnerMarking::inner_markings[id]->is_final)
			trace_is_final = true;
	} else {
		// if the input is a SA then build the trace by using the structure of the automaton:
		pnapi::State* state = *(serviceAutomaton::sa->getInitialStates().begin());
		while (counter < trace_max_length && state->getPostset().size() > 0) {
			++counter;
			//choose an edge from outgoing edges
			choose = std::rand() % state->getPostset().size();
			set<pnapi::Edge *>::iterator myIterator = state->getPostsetEdges().begin();
			for(int i = 0; i < choose; ++i)
				++myIterator;
			pnapi::Edge* edge = *myIterator;

			//create log entry for event
			tempstring << "\t\t<event>\n"
					<< "\t\t\t<string key=\"org:resource\" value=\"UNDEFINED\"/>\n"
					<< "\t\t\t<date key=\"time:timestamp\" value=\"" << timestring.str() << ".000+00:00\"/>\n"
					<< "\t\t\t<string key=\"" << EVENT_KEY_LABEL << "\" value=\"" << edge->getLabel() << "\"/>\n"
					//<< "\t\t\t<int key=\"" << EVENT_KEY_NUMBER << "\" value=\"" << counter << "\"/>\n"
					<< "\t\t\t<string key=\"lifecycle:transition\" value=\"complete\"/>\n"
					<< "\t\t</event>\n";
			//set successor
			state = &edge->getDestination();
		}
		if (state->isFinal())
			trace_is_final = true;
	}

	bool finishedTrace = false;

	// if the traces have to end in a final state but this trace doesn't then ignore the trace
	// else write the trace to the output
	if (finalEnd && !trace_is_final) {
		status("ignored trace %i, length: %i, because it didn't end in a final state", trace_number, counter);
	} else {
		for (int i = 0; i < repeated_count; ++i) {
			// write the trace to the output stream
			file << "\t<trace>\n"
					<< "\t\t<string key=\"" << TRACE_KEY_NAME << "\" value=\"Case" << trace_number << ".0\"/>\n"
					//<< "\t\t<int key=\"" << TRACE_KEY_LENGTH << "\" value=\"" << counter << "\"/>\n"
					<< tempstring.str()
					<< "\t</trace>\n";
		}
		status("done with trace %i, length: %i. (copied %i times)", trace_number, counter, repeated_count);
		finishedTrace = true;
	}
	tempstring.clear();

	return finishedTrace;
}

