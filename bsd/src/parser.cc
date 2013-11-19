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
#include "parser.h"
#include "InnerMarking.h"
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

/*========================================================
 *--------------------- DOT to BSD parser ----------------
 *========================================================*/

/*!
 \brief Parses a DOT file which represents a BSD or CSD automaton.

 There have to be parameters in the comments specifying the bound, the node count and the event count:
 	 bound:             x
 	 nodes:             y
 	 labels (total):    z

 The initial node of the graph is assumed to be the first node parsed. There is a test on correct label
 and node count. However, there is no test on correct graph input, meaning that the parser assumes that
 for each node there is given all z labels. If some labels are missing for some nodes then they are
 obviously also missing in the parsed graph. This may lead to wrong results or segmentation faults.

 The format of the edges should be: "(node).lambda" -> "(node).lambda" [...,label="...",...];

 \param[in]	is		the input stream (the input file)

 \return the parsed graph
 */
parsedGraph * parser::dot2graph_parse(std::istream & is) {
	std::string line;
	Label_ID idcounter = 0;
	int state = 0;
	unsigned int currentnode = 0;

	parsedGraph * graph = new parsedGraph;

	graph->id2name = new std::map<Label_ID, std::string>;
	graph->name2id = new std::map<std::string, Label_ID>;
	graph->is_sending_label = new std::map<Label_ID, bool>;

	int linecounter = 0;
	status("parsing...");
	while (std::getline(is, line)) {
		++linecounter;
		if (state < 4) {
			// search for the bound
			if (line.find("bound:") != std::string::npos) {
				line = line.substr(line.find("bound:") + 6, line.length());
				// Skip delimiters at beginning.
				std::string::size_type start = line.find_first_not_of(" \t", 0);
				// Find first "non-delimiter".
				std::string::size_type end = line.find_first_of(" \t", start);
				graph->bound = atoi(line.substr(start, end - start).c_str());
				++state;
			}

			// search for the node count
			if (line.find("nodes:") != std::string::npos) {
				line = line.substr(line.find("nodes:") + 6, line.length());
				// Skip delimiters at beginning.
				std::string::size_type start = line.find_first_not_of(" \t", 0);
				// Find first "non-delimiter".
				std::string::size_type end = line.find_first_of(" \t", start);
				graph->nodes = atoi(line.substr(start, end - start).c_str());
				++state;
			}

			// search for the label count
			if (line.find("labels (total):") != std::string::npos) {
				line = line.substr(line.find("labels (total):") + 15, line.length());
				// Skip delimiters at beginning.
				std::string::size_type start = line.find_first_not_of(" \t", 0);
				// Find first "non-delimiter".
				std::string::size_type end = line.find_first_of(" \t", start);
				graph->events = atoi(line.substr(start, end - start).c_str());
				++state;
			}

			if (state == 3) {
				graph->names = new std::string[graph->nodes];
				graph->lambdas = new int[graph->nodes];

				graph->pointer = new unsigned int*[graph->nodes];
				for (unsigned int i = 0; i < graph->nodes; ++i)
					graph->pointer[i] = new unsigned int[graph->events];
				++state;
			}

		} else {

			if (line.find("label") != std::string::npos && line.find("lambda") != std::string::npos) {
				// we are parsing the nodes
				// (should be) form of the lines: nodeID [label=<...>]; /*lambda=X*/
				++currentnode;

				std::string::size_type start = line.find_first_not_of(" \t", 0);
				std::string::size_type end = line.find_first_of(" \t", start);
				int nodeID = atoi(line.substr(start, end - start).c_str());

				if (nodeID >= ((int)graph->nodes - 2) && graph->nodes > 1)
					abort(10, "Error in dot file in line %i. Parsed node id (%i) is too high.", linecounter, nodeID);

				// if this is the U node or the empty node then move it to the back of the array
				if (nodeID == -1)
					nodeID = graph->nodes - 1;
				if (nodeID == -2)
					nodeID = graph->nodes - 2;

				graph->names[nodeID] = line.substr(line.find("label=") + 6, line.length());
				// Find first "non-delimiter".
				end = graph->names[nodeID].find_first_of("]", 0);
				graph->names[nodeID] = graph->names[nodeID].substr(0, end);

				graph->lambdas[nodeID] = atoi(line.substr(line.find("lambda=") + 7, 1).c_str());
			} else if (line.find("->") != std::string::npos && line.find("label") != std::string::npos) {
				// we are parsing the edges (which also have labels assigned)
				// (should be) form of the lines: nodeID_1 -> nodeID_2 [label="...",...]; /*...*/
				std::list<std::string> tokens;
				// tokenize the current line (should produce: 'nodeID_1' '->' 'nodeID_2' ...)
				Tokenize(line, tokens, " \t");

				unsigned int id_node1;
				unsigned int id_node2;
				// iterate through the tokens and set node ids accordingly
				std::list<std::string>::const_iterator it = tokens.begin();
				if (it != tokens.end()) {
					id_node1 = atoi((*it).c_str());
					// if this is the U node or the empty node then move it to the back of the array
					if (id_node1 == -1)
						id_node1 = graph->nodes - 1;
					if (id_node1 == -2)
						id_node1 = graph->nodes - 2;
					++it;
				} else
					abort(10, "error in dot file in line %i", linecounter);
				if (it != tokens.end()) {
					++it;
				} else
					abort(10, "error in dot file in line %i", linecounter);
				if (it != tokens.end()) {
					id_node2 = atoi((*it).c_str());
					// if this is the U node or the empty node then move it to the back of the array
					if (id_node2 == -1)
						id_node2 = graph->nodes - 1;
					if (id_node2 == -2)
						id_node2 = graph->nodes - 2;
					++it;
				} else
					abort(10, "error in dot file in line %i", linecounter);

				// collect the labels and also all options
				std::string labels = "";
				if (line.find("label=") != std::string::npos) {
					labels = line.substr(line.find("label=") + 7, line.length());
					// Find first "non-delimiter".
					std::string::size_type end = labels.find_first_of("\"", 0);
					labels = labels.substr(0, end);
				}
				// status("labels: %s", labels.c_str());

				// tokenize the label string (might be several labels on one edge)
				tokens.clear();
				Tokenize(labels, tokens, ", ");
				it = tokens.begin();
				while (it != tokens.end()) {
					// save the parsed label and set a new id if it has not yet been processed
					if (graph->name2id->find(*it) == graph->name2id->end()) {
						status("found new label: %s", it->c_str());
						if (idcounter >= graph->events)
							abort(10, "Error in dot file in line %i. Parsed label count is higher than parsed parameter.", linecounter);
						(*graph->name2id)[*it] = idcounter;
						(*graph->id2name)[idcounter] = *it;

						if (line.find("/*sending*/") != std::string::npos) {
							(*graph->is_sending_label)[idcounter] = true;
						} else if (line.find("/*receiving*/") != std::string::npos) {
							(*graph->is_sending_label)[idcounter] = false;
						} else {
							abort(10, "Error in dot file in line %i. /*sending*/ or /*receiving*/ missing.", linecounter);
						}

						// increase the label id counter
						++idcounter;
					}
					// set the pointer accordingly
					// status("set the pointer from node %u with label %u to node %u", id_node1, ((*graph->name2id)[*it]), id_node2);
					graph->pointer[id_node1][((*graph->name2id)[*it])] = id_node2;
					++it;
				}
			}
		}
	}

	if (state < 4) {
		abort(10, "Error in dot file. Input file misses bound, nodes and/or events attributes.");
	}

	// the cases when there are less parsed nodes or labels than given in the attributes
	if (currentnode < graph->nodes)
		abort(10, "Error in dot file. Parsed node count is lower than parsed parameter.");
	if (idcounter < graph->events)
		abort(10, "Error in dot file. Parsed label count is lower than parsed parameter.");

	// the ids of the U node and empty node are placed at the end of the id array
	graph->U = graph->nodes - 1;
	if (graph->nodes >= 2) {
		graph->emptyset = graph->nodes - 2;
	}

	return graph;
}


/*!
 \brief Tokenizes the given string into the given list of strings devided by the chars in the delimiters.

 \param[in]		str			the string to be tokenized
 \param[out]	tokens		list of tokens	(list of strings)
 \param[in]		delimiters	the delimiters	(string)
 */
void parser::Tokenize(const std::string& str, std::list<std::string>& tokens, const std::string& delimiters) {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


/*========================================================
 *---------------------- TEST OUTPUT ---------------------
 *========================================================*/

/*!
 \brief print the BSD automaton in the shell (if verbose is switched on)

 \param[in]	graph	the BSD automaton
 */
void parser::printParsedGraph(parsedGraph & graph) {
	std::stringstream temp (std::stringstream::in | std::stringstream::out);
	temp << std::endl;
//	temp << "U: " << graph.U << ", empty: " << graph.emptyset << std::endl;
	for (int i = 0; i < graph.nodes; ++i) {
		temp << graph.names[i] << ":  lambda: " << graph.lambdas[i] << std::endl;
		for (unsigned int id = 0; id < graph.events; ++id) {
			temp  << "    (" << (*graph.id2name)[id] << " -> " << graph.names[graph.pointer[i][id]] << "), ";
			if ((*graph.is_sending_label)[id])
				temp << "sending";
			else
				temp << "receiving";
			temp << std::endl;
		}
		temp << std::endl;
	}

	status("%s", temp.str().c_str());
}
