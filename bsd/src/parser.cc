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

	status("parsing...");
	while (std::getline(is, line)) {
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

			// we are only interested in parsing the edges (which also have labels assigned)
			if (line.find("->") != std::string::npos && line.find("label") != std::string::npos) {
				// (should be) form of the lines: "(node).lambda" -> "(node).lambda" [...,label="...",...];
				std::list<std::string> tokens;
				// tokenize the current line
				Tokenize(line, tokens, "(). \"->\t");

				// iterate through the tokens and set node names and lambda values accordingly
				std::list<std::string>::const_iterator it = tokens.begin();
				std::string node1 = "";
				if (it != tokens.end()) {
					node1 = *it;
					// status("node 1: %s", node1.c_str());
					++it;
				} else
					continue;
				std::string lambda1 = "";
				if (it != tokens.end()) {
					lambda1 = *it;
					// status("lambda 1: %s", lambda1.c_str());
					++it;
				} else
					continue;
				std::string node2 = "";
				if (it != tokens.end()) {
					node2 = *it;
					// status("node 2: %s", node2.c_str());
					++it;
				} else
					continue;
				std::string lambda2 = "";
				if (it != tokens.end()) {
					lambda2 = *it;
					// status("lambda 2: %s", lambda2.c_str());
					++it;
				} else
					continue;

				// collect the labels and also all options
				std::string labels = "";
				std::string options = "";
				bool nextAreLabels = false;
				while (it != tokens.end()) {
					options += *it;
					if (nextAreLabels)
						labels = *it;
					if (it->find("label") != std::string::npos)
						nextAreLabels = true;
					else
						nextAreLabels = false;
					++it;
				}
				// status("labels: %s", labels.c_str());

				unsigned int p_node1 = -1;
				unsigned int p_node2 = -1;
				// iterate through the already parsed nodes and search for the two current parsed nodes
				for (unsigned int i = 0; i < currentnode; ++i) {
					// status("searching...");
					if (p_node1 == -1 && graph->names[i] == node1) {
						p_node1 = i;
					}
					if (p_node2 == -1 && graph->names[i] == node2) {
						p_node2 = i;
					}
					// stop searching if both nodes were found
					if (p_node1 != -1 && p_node2 != -1) {
						break;
					}
				}


				// if node 1 was not found then insert it into the graph
				if (p_node1 == -1) {
					if (currentnode >= graph->nodes)
						abort(11, "Parsed parameter doesn't match parsed nodes' count.");
					graph->names[currentnode] = node1;
					graph->lambdas[currentnode] = atoi(lambda1.c_str());

					// if the node is the U or empty node then set the pointers accordingly
					if (node1 == "U") {
						graph->U = currentnode;
					} else if (node1 == "empty") {
						graph->emptyset = currentnode;
					}

					p_node1 = currentnode;
					++currentnode;
				}

				// if node 2 was not found then insert it into the graph
				if (p_node2 == -1) {
					if (currentnode >= graph->nodes)
						abort(11, "Parsed parameter doesn't match parsed nodes' count.");
					graph->names[currentnode] = node2;
					graph->lambdas[currentnode] = atoi(lambda2.c_str());

					p_node2 = currentnode;
					++currentnode;
				}


				// tokenize the label string (might be several labels on one edge)
				tokens.clear();
				Tokenize(labels, tokens, ", ");
				it = tokens.begin();
				while (it != tokens.end()) {
					// status("found label: %s", it->c_str());
					// save the parsed label and set a new id if it has not yet been processed
					if (graph->name2id->find(*it) == graph->name2id->end()) {
						if (idcounter >= graph->events)
							abort(12, "Parsed parameter doesn't match parsed labels' count.");
						(*graph->name2id)[*it] = idcounter;
						(*graph->id2name)[idcounter] = *it;

						if (options.find("/*sending*/") != std::string::npos) {
							(*graph->is_sending_label)[idcounter] = true;
						} else if (options.find("/*receiving*/") != std::string::npos) {
							(*graph->is_sending_label)[idcounter] = false;
						} else {
							abort(10, "couldn't parse graph. /*sending*/ or /*receiving*/ missing");
						}

						// increase the label id counter
						++idcounter;
					}
					// set the pointer accordingly
					// status("set the pointer from node %u with label %u to node %u", p_node1, ((*graph->name2id)[*it]), p_node2);
					graph->pointer[p_node1][((*graph->name2id)[*it])] = p_node2;
					++it;
				}
			}
		}
	}

	if (state < 4) {
		abort(10, "Input file misses bound, nodes and/or events attributes.");
	}

	// the cases when there are less parsed nodes or labels than given in the attributes
	if (currentnode < graph->nodes)
		abort(11, "Parsed parameter doesn't match parsed nodes' count.");
	if (idcounter < graph->events)
		abort(12, "Parsed parameter doesn't match parsed labels' count.");

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
