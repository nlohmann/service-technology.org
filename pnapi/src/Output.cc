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

/*!
 * \file  Output.cc
 */

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libgen.h>
#include <unistd.h>
#include "config.h"
#include "Output.h"

/*!
 * \todo  Fix verbose output and error handling.
 */

namespace pnapi
{
namespace util
{
const char* tmpFileTemplate = "/tmp/pnapi-XXXXXX";
}
}

using namespace pnapi::util;

/***************
 * CONSTRUCTOR *
 ***************/

/*!
 This constructor creates a temporary file using mktemp().
 In case of MinGW compilations, the
 basename has to be used to avoid problems with path names.
 */
Output::Output() :
  os(*(new std::ofstream(createTmp(), std::ofstream::out | std::ofstream::trunc))),
  filename(temp), kind("")
{
  // status("writing to temporary file '%s'", _cfilename_(filename));
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
      // abort(11, "could not write to file '%s'", _cfilename_(filename));
      std::cerr << "PNAPI: could not write to file '" << filename << "'" << std::endl;
      exit(EXIT_FAILURE);
  }
  
  /*
  if (str.compare("-")) {
      status("writing %s to file '%s'", _coutput_(kind), _cfilename_(filename));
  } else {
      status("writing %s to standard output", _coutput_(kind));
  } //*/
}


/**************
 * DESTRUCTOR *
 **************/

/*!
 This destructor closes the associated file. 
Temporary files are deleted after closing.
 */
Output::~Output() 
{
  if (&os != &std::cout) 
  {
    delete(&os);
    /*
    if (!temp) 
    {
      status("closed file '%s'", filename.c_str());
    } 
    else 
    {
      if (remove(filename.c_str()) == 0) 
      {
          status("closed and deleted temporary file '%s'", filename.c_str());
      } 
      else 
      {
          status("closed, but could not delete temporary file '%s'", filename.c_str());
      }
    } //*/
    remove(filename.c_str());
    free(temp);
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
char * Output::createTmp()
{
#ifdef __MINGW32__
    temp = strdup(basename(tmpFileTemplate));
    if (mktemp(temp) == NULL)
    {
      // abort(13, "could not create to temporary file '%s'", basename(args_info.tmpfile_arg));
      std::cerr << "could not create to temporary file '" << basename(tmpFileTemplate) << "'\n";
      exit(EXIT_FAILURE);
    };
#else
    temp = strdup(tmpFileTemplate);
    if (mkstemp(temp) == -1) {
      // abort(13, "could not create to temporary file '%s'", temp);
      std::cerr << "could not create to temporary file '" << temp << "'\n";
      exit(EXIT_FAILURE);
    };
#endif
    return temp;
}
