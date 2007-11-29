/*****************************************************************************
 * Copyright 2005, 2006, 2007 Niels Lohmann, Peter Massuthe, Daniela Weinberg*
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file	debug.h
 *
 * \brief	Some debugging tools for Fiona
 * 
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "mynew.h"
#include <string>
#include <iostream>
#include <fstream>
#include <exception>

extern std::string ogfileToParse;
extern std::string owfnfileToParse;

typedef enum {
    TRACE_0, //!< no messages
    TRACE_1, //!< keep track of nodes
    TRACE_2, //!< edges
    TRACE_3, //!< number of states: global, local
    TRACE_4, //!< to be defined
    TRACE_5 //!< all
} trace_level_fiona;

extern trace_level_fiona debug_level;

/// Provides output to stderr using different #trace_level_fiona 
/// (in order to regulate amount of output)
void trace(trace_level_fiona pTraceLevel, std::string message);

/// Works like #trace(trace_level,std::string) with trace_level_fiona = TRACE_ALWAYS
void trace(std::string message);

/// turns an integer into its string representation
std::string intToString(int);

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

#endif
