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

#ifdef HAVE_CONFIG
#include <config.h>
#endif

#include <libgen.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "verbose.h"
#include "Output.h"
#include <sstream>

extern std::string invocation;

using std::endl;


/******************
 * STATIC MEMBERS *
 ******************/

#ifdef HAVE_CONFIG
std::string Output::tempfileTemplate = std::string("/tmp/") + PACKAGE_TARNAME + "-XXXXXX";
#else
std::string Output::tempfileTemplate = "/tmp/temp-XXXXXX";
#endif
bool Output::keepTempfiles = true;

std::list<BSDNode *>* Output::templist = NULL;


/***************
 * CONSTRUCTOR *
 ***************/

/*!
 This constructor creates a temporary file using createTmp() as helper.
*/
Output::Output() :
    os(*(new std::ofstream(createTmp(), std::ofstream::out | std::ofstream::trunc))),
    filename(temp), kind("") {
    status("writing to temporary file '%s'", _cfilename_(filename));
}

/*!
 This constructor creates a file with the given filename. In case the
 filename matches "-", no file is created, but std::cout is used as output.
*/
Output::Output(const std::string& str, const std::string& kind) :
    os((!str.compare("-")) ?
       std::cout :
       * (new std::ofstream(str.c_str(), std::ofstream::out | std::ofstream::trunc))
      ),
    filename(str), temp(NULL), kind(kind) {
    if (not os.good()) {
        abort(13, "could not write to file '%s'", _cfilename_(filename));
    }

    if (str.compare("-")) {
        status("writing %s to file '%s'", _coutput_(kind), _cfilename_(filename));
    } else {
        status("writing %s to standard output", _coutput_(kind));
    }
}


/**************
 * DESTRUCTOR *
 **************/

/*!
 This destructor closes the associated file. Unless the class is configured
 to keep temporary files (by calling setKeepTempfiles()), temporary files are
 deleted after closing.
*/
Output::~Output() {
    if (&os != &std::cout) {
        delete(&os);
        if (temp == NULL) {
            status("closed file '%s'", _cfilename_(filename));
        } else {
            if (keepTempfiles) {
                status("closed temporary file '%s'", _cfilename_(filename));
            } else {
                if (remove(filename.c_str()) == 0) {
                    status("closed and deleted temporary file '%s'", _cfilename_(filename));
                } else {
                    // this should never happen, because mkstemp creates temp
                    // files in mode 0600.
                    status("closed, but could not delete temporary file '%s'", _cfilename_(filename));
                }
            }
            free(temp);
        }
    }
}


/************
 * OPERATOR *
 ************/

/*!
 This implicit conversation operator allows to use Output objects like
 ostream streams.
*/
Output::operator std::ostream& () const {
    return os;
}


/********************
 * MEMBER FUNCTIONS *
 ********************/

std::string Output::name() const {
    return filename;
}

std::ostream& Output::stream() const {
    return os;
}

/*!
 This function creates a temporary file using mkstemp(). It uses the value
 of the tmpfile parameter as template. In case of MinGW compilations, the
 basename has to be used to avoid problems with path names.

 \return name of already opened temp file

 \note mkstemp already opens the temp file, so there is no need to check
       whether the creation of the std::ofstream succeeded.
*/
char* Output::createTmp() {
#ifdef __MINGW32__
    temp = basename(const_cast<char*>(tempfileTemplate.c_str()));
    if (mktemp(temp) == NULL) {
        abort(13, "could not create temporary file '%s'", _cfilename_(basename(const_cast<char*>(tempfileTemplate.c_str()))));
    };
#else
    temp = strdup(tempfileTemplate.c_str());
    if (mkstemp(temp) == -1) {
        abort(14, "could not create temporary file '%s'", _cfilename_(temp));
    };
#endif
    return temp;
}


/***************************
 * STATIC MEMBER FUNCTIONS *
 ***************************/

void Output::setTempfileTemplate(std::string s) {
    tempfileTemplate = s;
}

void Output::setKeepTempfiles(bool b) {
    keepTempfiles = b;
}


/*************************************************************************
 ***** DOT output
 *************************************************************************/

/*!
 * \brief dot output
 */
std::ostream & Output::dotoutput(std::ostream & os, BSDgraph & graph, std::string & filename, bool CSD, int bound)
{
//	digraph graphname {
//	     a -> b;
//	     b -> d [label=f];
//	 }

	os //< output everything to this stream

		<< "/*" << endl
		<< "  -- Don't delete or change the comment section. Parsing depends on several attributes in here. --" << endl
		<< "  generator:   " << PACKAGE_STRING << endl //net.getMetaInformation(os, pnapi::io::CREATOR, PACKAGE_STRING) << endl
		<< "  input file:  " << filename << ".owfn" << endl
		<< "  invocation:   " << invocation << endl
		<< "  type:        ";
	if (CSD)
		os << "CSD" << endl;
	else
		os << "BSD" << endl;

	os  << "  bound:             " << bound << endl
		<< "  #labels (total):   " << (unsigned int)graph.events-1 << endl
		<< "  #sending labels:   " << (unsigned int)graph.receive_events << endl
		<< "  #receiving labels: " << (unsigned int)graph.send_events << endl;

	if (CSD) {
		templist = new std::list<BSDNode *>;
		// fill the templist (to get only the reachable nodes)
		traverse(*(graph.graph->begin()));
		templist->push_back(graph.U);
	} else {
		templist = graph.graph;
	}

	// map the nodes to IDs
	std::map<BSDNode *, int> nodeIDs;
	unsigned int counter = 0;
	for (std::list<BSDNode *>::const_iterator it = templist->begin(); it != templist->end(); ++it) {
		if (*it != graph.U && *it != graph.emptyset && (!CSD || !(*it)->isU)) {
			nodeIDs[*it] = counter;
			++counter;
		} else if (*it == graph.U || (CSD && (*it)->isU)) {
			nodeIDs[*it] = -1;
		} else if (*it == graph.emptyset) {
			nodeIDs[*it] = -2;
		}
	}

	os << "  #nodes:            " << (unsigned int)templist->size() << endl;

	os << "  computation time:  " << (graph.comp_time) << " s" << endl
	   << "  memory consumed:   " << graph.memory_consumption << " KB" << endl;

	os << "*/" << endl << endl
	   << "digraph {" << endl;

	os << "\tnode [shape=record,style=rounded];" << endl;

	// define a fake initial node... (is there a better way?) \todo
	os << "\tinitialNode [shape=point,label=\"\",style=invis,weight=100];" << endl;
	// an arrow to the 'real' initial node
	os << "\tinitialNode -> " << nodeIDs[*(templist->begin())] << ";" << endl;

	for (std::list<BSDNode *>::const_iterator it = templist->begin(); it != templist->end(); ++it) {
		// new format
		os << "\t" << nodeIDs[*it]  << " [label=<" << dotnodeName(*it, nodeIDs, graph.U, graph.emptyset, CSD) << ">];"
				<< " /*lambda=" << (unsigned int)(*it)->lambda << "*/" << endl;

		if (*it == graph.U || *it == graph.emptyset) {
			if ((unsigned int)graph.last_receive - (unsigned int)graph.first_receive >= 0) {
				os << "\t" << nodeIDs[*it] << " -> " << nodeIDs[*it] << " [label=\"";
				for (unsigned int id = graph.first_receive; id < graph.last_receive; ++id) {
					os << graph.id2name[id] << ",";
				}
				os << graph.id2name[(unsigned int)graph.last_receive] << "\",color=red];  /*sending*/" << endl;
			}

			if ((unsigned int)graph.last_send - (unsigned int)graph.first_send >= 0) {
				os << "\t" << nodeIDs[*it] << " -> " << nodeIDs[*it] << " [label=\"";
				for (unsigned int id = graph.first_send; id < graph.last_send; ++id) {
					os << graph.id2name[id] << ",";
				}
				os << graph.id2name[(unsigned int)graph.last_send] << "\",color=blue];  /*receiving*/" << endl;
			}
		} else {
			for (unsigned int id = 2; id <= graph.events; ++id) {
				os << "\t" << nodeIDs[*it] << " -> ";
				// the target node may be not in the node id map if it is a U node
				if ((*it)->successors[id]->isU) {
					os << nodeIDs[graph.U];
				} else {
					os << nodeIDs[(*it)->successors[id]];
				}
				os << " [label=\"" << graph.id2name[id] << "\"";
				if (id >= graph.first_receive && id <= graph.last_receive) {
					os << ",color=red]; /*sending*/";
				} else {
					os << ",color=blue]; /*receiving*/";
				}
				os << endl;
			}
		}
	}

	if (CSD)
		delete templist;

	os << "\tfootnote [shape=box,label=<red: &Sigma;<SUP>out</SUP>,  blue: &Sigma;<SUP>in</SUP>>];" << endl;

	return os << "}" << endl;
}

// search for all reachable nodes and put them into the templist (CSD automaton output)
void Output::traverse(BSDNode* node) {
	if (node->isU)
		return;

	// test if the marking was already visited
	for (std::list<BSDNode *>::const_iterator it = templist->begin(); it != templist->end(); ++it) {
		if (*it == node) {
			return;
		}
	}
	// if not then add the current marking to the closure
	templist->push_back(node);

	for (unsigned int id = 2; id <= Label::events; ++id) {
		traverse(node->successors[id]);
	}
}

std::string Output::dotnodeName(BSDNode * node, std::map<BSDNode *, int> & IDs, BSDNode* U, BSDNode* emptyset, bool CSD) {
	std::stringstream temp (std::stringstream::in | std::stringstream::out);
	if (node == U || (CSD && node->isU)) {
		temp << "U";
	} else if (node == emptyset) {
		temp << "Q<SUB>&empty;</SUB>";
	} else {
		temp << "Q<SUB>" << IDs[node] << "</SUB>";
	}

	temp << "|" << (unsigned int)node->lambda;

	return temp.str();
}


/*************************************************************************
 ***** OG output
 *************************************************************************/

/*!
 * \brief og output
 */
std::ostream & Output::ogoutput(std::ostream & os, BSDgraph & graph, std::string & filename, int bound)
{
	os  //< output everything to this stream

	<< "{" << endl
	<< "  generated by: " << PACKAGE_STRING << endl
	<< "  input file:   " << filename << ".owfn" << endl
	<< "  invocation:   " << invocation << endl
	<< "  bound:        " << bound << endl
	<< "}" << endl << endl;

	os << "INTERFACE" << endl;

	    if (graph.receive_events > 0) {
	        os << "  INPUT" << endl << "    ";
	        bool first = true;
	        for (Label_ID l = graph.first_receive; l <= graph.last_receive; ++l) {
	            if (not first) {
	                os << ", ";
	            }
	            first = false;
	            os << graph.id2name[l];
	        }
	        os << ";\n";
	    }

	    if (graph.send_events > 0) {
	        os << "  OUTPUT" << endl << "    ";
	        bool first = true;
	        for (Label_ID l = graph.first_send; l <= graph.last_send; ++l) {
	            if (not first) {
	                os << ", ";
	            }
	            first = false;
	            os << graph.id2name[l];
	        }
	        os << ";" << endl;
	    }

	    os << endl << "NODES" << endl;

	    //print all the nodes, starting with the initial node
	    for (std::list<BSDNode *>::const_iterator it = graph.graph->begin(); it != graph.graph->end(); ++it) {
	    	printOGnode(os, graph, **it);
	    }

//	    // print empty node unless we print an automaton
//	    if (not args_info.sa_given and emptyNodeReachable) {
//	        // the empty node
//	        os << "  0 : true\n";
//
//	        // empty node loops
//	        for (Label_ID l = Label::first_receive; l <= Label::last_sync; ++l) {
//	            os << "    " << Label::id2name[l]  << " -> 0\n";
//	        }
//	    }
	    return os;
}

void Output::printOGnode(std::ostream & os, BSDgraph & graph, BSDNode & node) {
	// skip U nodes
	if (node.isU) {
		return;
	}

    os << "  " << reinterpret_cast<size_t>(&node)
       << " : ";

    printFormula(os, graph, node);

    os << endl;

    // iterate through all successors of the node
    for (unsigned int id = graph.first_receive; id <= graph.last_send; ++id) {
    	// skip U node successors (not existing in MP)
    	if (node.successors[id]->isU) {
    		continue;
    	}

    	os << "    " << graph.id2name[id] << " -> " << reinterpret_cast<size_t>(node.successors[id]) << endl;
    }
}

void Output::printFormula(std::ostream & os, BSDgraph & graph, BSDNode & node) {
	if (node.formula.empty()) {
		os << "true";
	} else {
		// iterate over the disjunctions
		for (std::list<std::list<Label_ID> >::const_iterator it = node.formula.begin(); it != node.formula.end();) {
			os << "(";
			// iterate over all labels in the disjunction
			for (std::list<Label_ID>::const_iterator label = it->begin(); label != it->end();) {
				if (*label == 0) {
					os << "final";
				} else {
					os << graph.id2name[*label];
				}
				if (++label != it->end())
					os << " + ";
			}
			os << ")";
			if (++it != node.formula.end())
				os << " * ";
		}
	}
}
