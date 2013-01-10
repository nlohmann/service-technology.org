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

#include <pnapi/pnapi.h>
#include <vector>
#include <map>
#include <set>
#include "types.h"


/*!
 \brief ...
*/
class generateLog {

    public: /* static functions */

        /// create the XES log
        static void createLog(std::ostream& file, std::string & filename,
        										   const bool isOWFN,
        										   const int trace_count ,
				  	  	  	  	  	  	  	  	   const int trace_min_length,
				  	  	  	  	  	  	  	  	   const int trace_max_length,
				  	  	  	  	  	  	  	  	   bool finalEnd);

    private: /* static functions */
            /// creates the header for the output file
            static void fileHeader(std::ostream&, std::string& filename);

            /// creates the footer for the output file
            static void fileFooter(std::ostream&);

            /// creates a trace and writes it to the output file
            static void create_trace(std::ostream& file, bool isOWFN, const int trace_number,
            								const int trace_max_length, const bool finalEnd);

};

#define TRACE_CLASSIFIER "Trace Name"
#define TRACE_KEY_NAME "concept:name"
#define TRACE_KEY_LENGTH "length"
#define EVENT_CLASSIFIER "Event Name"
#define EVENT_KEY_LABEL "concept:name"
#define EVENT_KEY_NUMBER "number"
