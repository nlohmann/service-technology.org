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

		<< "/*\n"
		<< "  generator:   " << PACKAGE_STRING << endl //net.getMetaInformation(os, pnapi::io::CREATOR, PACKAGE_STRING) << endl
		<< "  input file:  " << filename << ".owfn" << endl
		<< "  bound:             " << bound << endl
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

	os << "  #nodes:            " << (unsigned int)templist->size() << endl;

	if (CSD) {
		os << "  computation time:  " << (graph.BSD_comp_time + graph.CSD_comp_time) << " s" << endl
		   << "  memory consumed:   " << graph.CSD_memory << " KB" << endl;
	} else {
		os << "  computation time:  " << (graph.BSD_comp_time) << " s" << endl
		   << "  memory consumed:   " << graph.BSD_memory << " KB" << endl;
	}

	os << "*/\n\n"
	   << "digraph {\n";

	// define a fake initial node... (is there a better way?) \todo
	os << "\tinitialNode [shape=point,label=\"\",style=invis,weight=100];" << endl;
	// an arrow to the 'real' initial node
	os << "\tinitialNode -> " << dotnodeName(**(templist->begin()), graph.U, graph.emptyset, CSD) << ";" << endl;

	for (std::list<BSDNode *>::const_iterator it = templist->begin(); it != templist->end(); ++it) {
		if ((**it).lambda == 1) {
			os << "\t" << dotnodeName(**it, graph.U, graph.emptyset, CSD) << " [style=dashed];" << endl;
		} else if (*it == graph.U) {
			os << "\t" << dotnodeName(**it, graph.U, graph.emptyset, CSD) << " [shape=plaintext];" << endl;
		} else if (*it == graph.emptyset) {
			os << "\t" << dotnodeName(**it, graph.U, graph.emptyset, CSD) << " [shape=plaintext];" << endl;
		}

		if (*it == graph.U || *it == graph.emptyset) {
			if ((unsigned int)graph.last_receive - (unsigned int)graph.first_receive >= 0) {
				os << "\t" << dotnodeName(**it, graph.U, graph.emptyset, CSD) << " -> " << dotnodeName(**it, graph.U, graph.emptyset, CSD)
											<< " [label=\"";
				for (unsigned int id = graph.first_receive; id < graph.last_receive; ++id) {
					os << graph.id2name[id] << ",";
				}
				os << graph.id2name[(unsigned int)graph.last_receive] << "\",color=red];" << endl;
			}

			if ((unsigned int)graph.last_send - (unsigned int)graph.first_send >= 0) {
				os << "\t" << dotnodeName(**it, graph.U, graph.emptyset, CSD) << " -> " << dotnodeName(**it, graph.U, graph.emptyset, CSD)
																<< " [label=\"";
				for (unsigned int id = graph.first_send; id < graph.last_send; ++id) {
					os << graph.id2name[id] << ",";
				}
				os << graph.id2name[(unsigned int)graph.last_send] << "\",color=green];" << endl;
			}
		} else {
			for (unsigned int id = 2; id <= graph.events; ++id) {
				os << "\t" << dotnodeName(**it, graph.U, graph.emptyset, CSD) << " -> " << dotnodeName(*((*it)->pointer[id]), graph.U, graph.emptyset, CSD)
					   << " [label=\"" << graph.id2name[id] << "\"";
				if (id >= graph.first_receive && id <= graph.last_receive) {
					os << ",color=red";
				} else {
					os << ",color=green";
				}
				os << "];" << endl;
			}
		}
	}

	if (CSD)
		delete templist;

	os << "\tfootnote [shape=box,label=\"red: sending labels\\ngreen: receiving labels\"];" << endl;

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
		traverse(node->pointer[id]);
	}
}

std::string Output::dotnodeName(BSDNode & node, BSDNode* U, BSDNode* emptyset, bool CSD) {
	std::stringstream temp (std::stringstream::in | std::stringstream::out);
	if (&node == U || (CSD && node.isU)) {
		temp << "\"U";
	} else if (&node == emptyset) {
		temp << "\"empty";
	} else {
		temp << "\"(";
		for (MarkingList::const_iterator itlist = node.list.begin(); itlist != node.list.end();) {
			temp << *itlist;
			if (++itlist != node.list.end()) {
				temp << ",";
			}
		}
		temp << ")";
	}

	if (CSD && node.isU) {
		temp << "." << (unsigned int)U->lambda << "\"";
	} else {
		temp << "." << (unsigned int)node.lambda << "\"";
	}
	return temp.str();
}
