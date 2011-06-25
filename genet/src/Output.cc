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

#include "Output.h"
#include <libgen.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#define FUNC std::cerr << " ... in function " << __PRETTY_FUNCTION__ << " in " << __FILE__ << "@" << __LINE__ << std::endl;

extern "C" int mkstemp(char*);
extern "C" char* strdup(const char*);

/******************
 * STATIC MEMBERS *
 ******************/

#ifdef HAVE_CONFIG
std::string Output::tempfileTemplate = std::string("/tmp/") + PACKAGE_TARNAME + "-XXXXXX";
#else
std::string Output::tempfileTemplate = "/tmp/temp-XXXXXX";
#endif
bool Output::keepTempfiles = true;


/***************
 * CONSTRUCTOR *
 ***************/

/*!
 This constructor creates a temporary file using createTmp() as helper.
*/
Output::Output() :
    os(*(new std::ofstream(createTmp(), std::ofstream::out | std::ofstream::trunc))),
    filename(temp), kind("") {
    /* empty */
}

/*!
 This constructor creates a file with the given filename. In case the
 filename matches "-", no file is created, but std::cout is used as output.
*/
Output::Output(const std::string& str, const std::string& kind) :
    os(
      * (new std::ofstream(str.c_str(), std::ofstream::out | std::ofstream::trunc))
      ),
    filename(str), temp(NULL), kind(kind) {
    if (not os.good()) {
        exit(1);
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
            
        } else {
            if (not keepTempfiles) {
                remove(filename.c_str());
            }
            // free(temp);
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
    temp = basename(const_cast<char*>(strdup(tempfileTemplate.c_str())));
    if (mktemp(temp) == NULL) {
        exit(1);
    };
#else
    temp = strdup(tempfileTemplate.c_str());
    if (mkstemp(temp) == -1) {
        exit(1);
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

