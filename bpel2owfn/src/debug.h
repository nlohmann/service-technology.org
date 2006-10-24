/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

/*!
 * \file debug.h
 *
 * \brief   debugging tools
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 * 
 * \since   2005/11/09
 *
 * \date    \$Date: 2006/10/24 18:18:34 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.22 $
 */





#ifndef DEBUG_H
#define DEBUG_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <string>

using namespace std;





/******************************************************************************
 * Data structures
 *****************************************************************************/

/*!
 * \brief trace levels
 *
 * The trace levels that are used by the #trace function. A standard trace
 * level can be set using the command-line parameter "-d".
 *
 * \todo Provide a detailled documentation of what the several debug levels
 * actually show.
 *
 * \see #yy_flex_debug
 * \see #yydebug
 */
typedef enum
{
  TRACE_ALWAYS,		///< trace level for errors
  TRACE_WARNINGS,       ///< trace level for warnings 
  TRACE_INFORMATION,	///< trace level some more (useful) information
  TRACE_DEBUG,          ///< trace level for detailed debug information
  TRACE_VERY_DEBUG	///< trace level for everything
} trace_level;





/******************************************************************************
 * External variables
 *****************************************************************************/

extern trace_level debug_level; // defined in debug.h





/******************************************************************************
 * Functions
 *****************************************************************************/

/// Provides output to stderr using different #trace_level 
void trace(trace_level pTraceLevel, string message);

/// Works like #trace(trace_level, string) with trace_level = TRACE_ALWAYS
void trace(string message);

/// Signalling syntax errors.
int yyerror(const char* msg);

/// Signal an error with a SA (static analysis) code
void SAerror(unsigned int code, string information = "", int lineNumber = 0);

/// Outputs the environment (i.e. four lines before and after) of a line.
void showLineEnvironment(int lineNumber);





/******************************************************************************
 * Macros
 *****************************************************************************/

/*!
 * \brief prints the function name
 *
 * A preprocessor directive to print the name of the called function together
 * with its file position. The \a prefix is used to add a short abbreviation
 * of the module of BPEL2oWFN to which the function belongs to, e.g. "[PN]"
 * for the Petri net class, etc. The macro #ENTER also adds a "+" before the
 * output.
 *
 * \param prefix additional string to print
*/
#define ENTER(prefix) \
  trace(TRACE_VERY_DEBUG, string(prefix) + "\t+" + string(__FUNCTION__) + \
    "() [" + string(__FILE__) + ":" + toString(__LINE__) + "]\n")


/*!
 * \brief prints the function name
 *
 * A preprocessor directive to print the name of the called function together
 * with its file position. The \a prefix is used to add a short abbreviation
 * of the module of BPEL2oWFN to which the function belongs to, e.g. "[PN]"
 * for the Petri net class, etc. The macro #LEAVE also adds a "-" before the
 * output.
 *
 * \param prefix additional string to print
 */
#define LEAVE(prefix) \
  trace(TRACE_VERY_DEBUG, string(prefix) + "\t-" + string(__FUNCTION__) + \
    "() [" + string(__FILE__) + ":" + toString(__LINE__) + "]\n")





#endif
