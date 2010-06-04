#include <config.h>
#include <libgen.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "verbose.h"
#include "Output.h"

#ifdef HAVE_CONFIG
#include <config.h>
#endif


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
    filename(temp), kind("")
{
    status("writing to temporary file '%s'", _cfilename_(filename));
}

/*!
 This constructor creates a file with the given filename. In case the
 filename matches "-", no file is created, but std::cout is used as output.
*/
Output::Output(const std::string& str, const std::string& kind) :
    os((!str.compare("-")) ?
        std::cout :
        *(new std::ofstream(str.c_str(), std::ofstream::out | std::ofstream::trunc))
    ),
    filename(str), temp(NULL), kind(kind)
{
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
Output::operator std::ostream&() const {
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
        abort(13, "could not create to temporary file '%s'", basename(const_cast<char*>(tempfileTemplate.c_str())));
    };
#else
    temp = strdup(tempfileTemplate.c_str());
    if (mkstemp(temp) == -1) {
        abort(13, "could not create to temporary file '%s'", temp);
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
