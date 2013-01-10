/*****************************************************************************\
 Locretia -- generating logs...

 Copyright (c) 2012 Simon Heiden

 Locretia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Locretia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Locretia.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#pragma once

#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <vector>
#include "Output.h"

class Results {
    public:
        /// constructor
        explicit Results(std::string);
        /// destructor
        ~Results();

        /// implicit conversation operator to ostream
        operator std::ostream& () const;

        void add(std::string,              std::stringstream&);
        void add(std::string, std::string, std::stringstream&);
        void add(std::string,              std::string);
        void add(std::string, std::string, std::string);
        void add(std::string,              const char*);
        void add(std::string, std::string, const char*);
        void add(std::string,              float);
        void add(std::string, std::string, float);
        void add(std::string,              unsigned int);
        void add(std::string, std::string, unsigned int);
        void add(std::string,              int);
        void add(std::string, std::string, int);
        void add(std::string,              bool);
        void add(std::string, std::string, bool);

    private:
        /// the output filename
        std::string filename;

        /// the output object
        Output* output;

        /// the stored values
        std::map<std::string, std::map<std::string, std::string> > values;

        /// the stored values (lists)
        std::map<std::string, std::map<std::string, std::string> > lists;
};
