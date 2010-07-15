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
#if defined(__MINGW32__)
  os(*(new std::ofstream(mktemp(temp = basename(strdup(tmpFileTemplate))), std::ofstream::out | std::ofstream::trunc))),
#else
  os(*(new std::ofstream(mktemp(temp = strdup(tmpFileTemplate)), std::ofstream::out | std::ofstream::trunc))),
#endif 
  filename(temp), kind("")
{
  if (not os.good() or filename == "") 
  {
    //  abort(13, "could not create to temporary file '%s'", filename.c_str());
    std::cerr << "PNAPI: could not create temporary file '" << filename << "'" << std::endl;
    exit(EXIT_FAILURE);
  }

  // status("writing to temporary file '%s'", filename.c_str());
}

/*!
 This constructor creates a file with the given filename. In case the
 filename matches "-", no file is created, but std::cout is used as output.
*/
Output::Output(std::string& str, std::string kind) :
  os((!str.compare("-")) ?
        std::cout :
        *(new std::ofstream(str.c_str(), std::ofstream::out | std::ofstream::trunc))
    ),
    filename(str), temp(NULL), kind(kind)
{
  if (not os.good()) 
  {
    //  abort(11, "could not write to file '%s'", str.c_str());
  std::cerr << "PNAPI: could not write to file '" << str << "'" << std::endl;
  exit(EXIT_FAILURE);
  }

  /*
  if (str.compare("-")) {
      status("writing %s to file '%s'", kind.c_str(), filename.c_str());
  } else {
      status("writing %s to standard output", kind.c_str());
  }
  */
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
    }
    */
    remove(filename.c_str());
    if(temp)
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
Output::operator std::ostream&() {
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
