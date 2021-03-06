/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.

 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file	debug.h
 *
 * \brief	Some debugging tools for Fiona
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"

/// In a release output is  restricted to TRACE_0, so write_trace will never be called
#ifdef NDEBUG
#define TRACE(A,B)
#else
#define TRACE(A,B) debug_trace(A,B)
#endif

#include "mynew.h"
#include <string>
#include <iostream>
#include <fstream>
#include <exception>

extern std::string ogfileToParse;
extern std::string owfnfileToParse;
extern std::string covfileToParse;

typedef enum {
    TRACE_0, /*!< error messages and some important status messages
                  like "is controllable" and size of IG/OG
             */
    TRACE_1, //!< notification with coarse granularity what Fiona is doing ("parsing file", "calculating IG/OG", etc.)
    TRACE_2, //!<
    TRACE_3, //!<
    TRACE_4, //!<
    TRACE_5  //!< really everything
} trace_level_fiona;

extern trace_level_fiona debug_level;

/// Provides output to stderr using different #trace_level_fiona
/// (in order to regulate amount of output)
void debug_trace(trace_level_fiona pTraceLevel, std::string message);

/// Works like #trace(trace_level,std::string) with trace_level_fiona = TRACE_0
void trace(std::string message);

/// turns an integer into its string representation
std::string intToString(int);

/// turns a double value into its string representation
std::string doubleToString(double);

/// checks whether a string shows a nonnegative integer
bool isNonNegativeInteger(const std::string& s);

/// returns the upper case of the given string
std::string toUpper(const std::string& s);

/// returns the lower case of the given string
std::string toLower(const std::string& s);

/// Signalling syntax errors.
int yyerror(const char* msg);

/// Prints an error that has been encountered while parsing an OG file. The
/// program exits after printing the error.
int og_yyerror(const char* msg);

/// Prints an error that has been encountered while parsing a ConstraintOG file. The
/// program exits after printing the error.
int covog_yyerror(const char* msg);

/// Prints an error that has been encountered while parsing an file containing a set
/// of oWFN nodes to be covered. The program exits after printing the error.
int cov_yyerror(const char* msg);


/// Needed for atexit-function
extern short unsigned int globalExitCode;

#ifdef LOG_NEW
	/// Prints the newlogger report
	void newloggerAtExit(void);
#endif

#endif
