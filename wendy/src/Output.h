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


#ifndef _OUTPUT_H
#define _OUTPUT_H

#include <iostream>
#include <string>

/* http://ubuntuforums.org/showthread.php?p=5929211 */
class Output{
    private:
        /// if this name is given as filename, std::cout will be used as output
        static std::string stdout_filename;

    public:
        /// constructor (creates temp file)
        Output();

        /// constructor (opens named stream or directs to std::cout)
        Output(std::string &, std::string);

        /// destructor (closes stream)
        ~Output();

        /// the stream itself
        std::ostream &os; 

        /// filename of the output stream
        std::string filename;

    private:
        /// its a temp file if its not NULL
        char *temp;

        /// the kind of the file
        std::string kind;
};

#endif
