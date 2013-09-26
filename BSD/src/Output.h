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

#pragma once

#include <iostream>
#include <string>
#include <pnapi/pnapi.h>
#include "types.h"
#include "Label.h"
#include "BSD.h"


/*!
 \brief output file organization

 This file is a wrapper class around an STL ostream object and organizes the
 lifecycle of an output. An output can be a regular file that is created by
 the tool, a temporary file that is used to communicate with other tools,
 or a redirection to the standard output stream.

 To create a regular file, call the constructor with a filename and a
 description of the output (for verbose messages). To create a temporary
 file, call the constructor without arguments. Temporary files will be
 deleted when the object is destroyed unless function setKeepTempfiles() is
 called. To redirect to standard output, use the the filename "-".

 The quite tricky constructor was motivated from the Ubuntu Forums at
 http://ubuntuforums.org/showthread.php?p=5929211
*/
class Output {
    private: /* static members */
        /// the template for temfiles to create
        static std::string tempfileTemplate;

        /// whether tempfiles should be deleted upon termination
        static bool keepTempfiles;

    public: /* static functions */
        /// set the tempfile template
        static void setTempfileTemplate(std::string);

        /// set the whether tempfiles should be deleted upon termination
        static void setKeepTempfiles(bool);

    private: /* member functions */
        /// helper function to create temp files
        char* createTmp();

    public: /* member functions */
        /// constructor (creates temp file)
        Output();

        /// constructor (opens named stream or directs to std::cout)
        Output(const std::string&, const std::string&);

        /// destructor (closes stream)
        ~Output();

        /// implicit conversation operator to ostream
        operator std::ostream& () const;

        /// returns the filename of this object
        std::string name() const;

        /// returns the stream of this object
        std::ostream& stream() const;

        /// dot output
        static std::ostream & dotoutput(std::ostream &, BSDgraph & graph, std::string & filename, bool CSD, int bound);

        static void traverse(BSDNode * node);

        static std::string dotnodeName(BSDNode & node, BSDNode* U, BSDNode* emptyset, bool CSD);

        /*!
         * \brief write vector elements, seperated by given delimeter, to stream
         */
        template <typename T>
        static std::ostream & outputVector(std::ostream & os, const std::vector<T> & v)
        {
          std::string delim = pnapi::io::util::DelimData::data(os).delim;
          if (v.empty())
            return os;
          for(typename std::vector<T>::const_iterator it = v.begin(); it != --v.end(); ++it)
          {
            output(os, **it) << delim;
          }
          return output(os, **--v.end());
        }

        /*!
         * \brief write set elements, seperated by given delimeter, to stream
         */
        template <typename T>
        static std::ostream & outputSet(std::ostream & os, const std::set<T> & s)
        {
          // sort the elements
          std::vector<T> v;
          PNAPI_FOREACH(it, s)
          {
            v.push_back(*it);
          }
          bool (*c)(T, T) = pnapi::io::util::compareContainerElements;
          std::sort(v.begin(), v.end(), c);

          // output the sorted vector
          return outputVector(os, v);
        }

    private: /* member attributes */
        /// the stream itself
        std::ostream& os;

        /// filename of the output stream
        std::string filename;

        /// its a temp file if its not NULL
        char* temp;

        /// the kind of the file
        const std::string kind;

        static BSDNodeList* templist;
};
