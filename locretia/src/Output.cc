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
#include "pnapi/util.h"
#include <pnapi/pnapi.h>

using std::endl;
using std::set;
using std::vector;


/******************
 * STATIC MEMBERS *
 ******************/

#ifdef HAVE_CONFIG
std::string Output::tempfileTemplate = std::string("/tmp/") + PACKAGE_TARNAME + "-XXXXXX";
#else
std::string Output::tempfileTemplate = "/tmp/temp-XXXXXX";
#endif
bool Output::keepTempfiles = true;
std::string Output::placeID_B = "";


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
        abort(11, "could not write to file '%s'", _cfilename_(filename));
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
        abort(13, "could not create temporary file '%s'", _cfilename_(temp));
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
 ***** PNML output
 *************************************************************************/

/*!
 * \brief special petri net output in pnml for use in Prom
 */
std::ostream & Output::output(std::ostream & os, const pnapi::PetriNet & net, std::string & filename)
{
	// initialize placeID_B
	placeID_B = "";

	os //< output everything to this stream

	   << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\n"
	   << "<!--\n"
	   << "  generator:   " << "Locretia 1.0-unreleased" << endl //net.getMetaInformation(os, pnapi::io::CREATOR, PACKAGE_STRING) << endl
	   << "  input file:  " << filename << endl
	   //  << "  invocation:  " << net.getMetaInformation(os, pnapi::io::INVOCATION) << endl
	   << "  net size:    " << pnapi::io::stat << net << pnapi::io::pnml
	   << "\n-->\n\n"

	   << "<pnml>\n"

	   << "  <module>\n";

	output(os, net.getInterface());

	os << "\n    <net id=\"n1\" type=\"PTNet\">\n"

	   << "    <name>\n"
	   << "      <text>" << filename << ".owfn</text>\n"
	   << "    </name>\n";

	outputSet(os, net.getPlaces());

	outputSet(os, net.getTransitions());

	outputSet(os, net.getArcs());

	os << "    </net>\n"

	   << "    <finalmarkings>\n"
	   << "      <marking>\n";

	if (placeID_B == "")
		net.getFinalCondition().getFormula().output(os);
	else
		outputSpecialFinalMarking(os, net.getPlaces());

	os << "      </marking>\n"
	   << "    </finalmarkings>\n"

	   << "  </module>\n"

	   << "</pnml>\n";

	return os << endl;
}

/*!
 * \brief place output
 */
std::ostream & Output::output(std::ostream & os, const pnapi::Place & p)
{
	os << "      <place id=\"" << p.getName() << "\"";

	if (p.getTokenCount())
	{
		os << ">\n"
		   << "        <initialMarking>\n"
		   << "          <text>" << p.getTokenCount() << "</text>\n"
		   << "        </initialMarking>\n"
		   << "      </place>\n";
	} else {
		os << " />\n";
	}

	return os;
}

/*!
 * \brief transition output
 */
std::ostream & Output::output(std::ostream & os, const pnapi::Transition & t)
{
	os << "      <transition id=\"" << t.getName() << "\">\n"
	   << "        <name><text>";

	// name-tag either empty or containing the corresponding label
	if(t.getLabels().empty()) {
		os << "";
	} else {
		os << t.getLabels().begin()->first->getName();
	}

	os << "</text></name>\n";

	return (os << "      </transition>\n");
}

/*!
 * \brief arc output
 */
std::ostream & Output::output(std::ostream & os, const pnapi::Arc & arc)
{
	static pnapi::PetriNet * net = NULL;
	static unsigned int arcId = 0;

	// check for different nets
	if(net != &arc.getPetriNet())
	{
		net = &arc.getPetriNet();
		arcId = 0;
	}

	os << "      <arc id=\"arcId" << (++arcId)
       << "\" source=\"" << arc.getSourceNode().getName()
       << "\" target=\"" << arc.getTargetNode().getName()
       << "\"";

	// the place following the B-transition is the final place
	if (arc.getSourceNode().getName() == "B\\n") {
		placeID_B = arc.getTargetNode().getName();
	}

	if (arc.getWeight() > 1)
	{
		os << ">\n"
		   << "        <inscription>\n"
		   << "          <text>" << arc.getWeight() << "</text>\n"
		   << "        </inscription>\n"
		   << "      </arc>\n";
	}
	else
	{
		os << " />\n";
	}

	return os;
}

/*!
 * \brief interface output
 */
std::ostream & Output::output(std::ostream & os, const pnapi::Interface & interface)
{
	//  os << "    <ports>\n";
	//
	//  PNAPI_FOREACH(port, interface.getPorts())
	//  {
	//    os << "      <port id=\"" << port->first << "\">\n";
	//
	//    output(os, *port->second);
	//
	//    os << "      </port>\n";
	//  }
	//
	//  return (os << "    </ports>\n");
	return os;
}

/*!
 * \brief port output
 */
std::ostream & Output::output(std::ostream & os, const pnapi::Port & port)
{
	os << pnapi::io::util::delim("\n");
	outputSet(os, port.getAllLabels());
	os << pnapi::io::util::delim("") << "\n";

	return os;
}

/*!
 * \brief label output
 */
std::ostream & Output::output(std::ostream & os, const pnapi::Label & l)
{
	os << "        <";

	switch(l.getType())
	{
	case pnapi::Label::INPUT: os << "input"; break;
	case pnapi::Label::OUTPUT: os << "output"; break;
	case pnapi::Label::SYNCHRONOUS: os << "synchronous"; break;
	default: break;
	}

	return (os << " id=\"" << l.getName() << "\" />");
}

/*!
 * \brief special final marking output (1 token on the place after transition "B\n")
 */
std::ostream & Output::outputSpecialFinalMarking(std::ostream & os, const std::set<pnapi::Place *> & places)
{
	// sort the elements
	std::vector<pnapi::Place *> v;
	PNAPI_FOREACH(it, places)
	{
		v.push_back(*it);
	}
	bool (*c)(pnapi::Place *, pnapi::Place *) = pnapi::io::util::compareContainerElements;
	std::sort(v.begin(), v.end(), c);

	if (v.empty())
		return os;

	for(typename std::vector<pnapi::Place *>::const_iterator it = v.begin(); it != --v.end(); ++it)
	{
        os << "        <place idref=\"" << (*it)->getName() << "\">\n";
        if ((*it)->getName() == placeID_B) {
        	os << "          <text>1</text>\n";
        } else {
        	os << "          <text>0</text>\n";
        }
        os << "        </place>\n";

	}

	os << "        <place idref=\"" << (*--v.end())->getName() << "\">\n";
	        if ((*--v.end())->getName() == placeID_B) {
	        	os << "          <text>1</text>\n";
	        } else {
	        	os << "          <text>0</text>\n";
	        }
	        os << "        </place>\n";

	return os;
}
