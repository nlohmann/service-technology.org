/*****************************************************************************\
 Candy -- Synthesizing cost efficient partners for services

 Copyright (c) 2010 Richard MŸller

 Candy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Candy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Candy.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/* *****************************************************
 * this header includes all globally used headers
 * *****************************************************/

// if you need assertions, always include these headers in _this_ order
// header from configure and cassert library
#include "config.h"
#include <cassert>

// C Standard General Utilities Library: EXIT_SUCCESS, atoi(), etc.
#include <cstdlib>
// C Header for UINT_MAX, etc.
#include <climits>

// standard header for declaring objects that control reading from
// and writing to the standard streams
#include <iostream>
#include <fstream>
// C++ strings library
#include <string>

// map container
#include <map>
// list container
#include <list>

// header from gengetopt
#include "cmdline.h"

// used namespaces
using std::cerr;
using std::cout;
using std::endl;
using std::string;

using std::pair;
using std::map;
using std::list;
