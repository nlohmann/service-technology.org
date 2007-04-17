/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds                      *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    debug.h
 *
 * \brief   debugging tools
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2005/11/09
 *
 * \date    \$Date: 2007/04/17 15:55:28 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.33 $
 *
 * \ingroup debug
 */





#ifndef DEBUG_H
#define DEBUG_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <string>

using std::string;





/******************************************************************************
 * Data structures
 *****************************************************************************/

/*!
 * \brief trace levels
 *
 * The trace levels that are used by the #trace function. A standard trace
 * level #debug_level can be set using the command-line parameter "-d".
 *
 * \todo Provide a detailled documentation of what the several debug levels
 * actually show.
 *
 * \see #debug_level
 * \see #frontend__flex_debug
 * \see #frontend_debug
 *
 * \ingroup debug
 */
typedef enum
{
  TRACE_ERROR,		///< only trace error messages
  TRACE_ALWAYS,		///< trace level for errors
  TRACE_WARNINGS,       ///< trace level for warnings 
  TRACE_INFORMATION,	///< trace level some more (useful) information
  TRACE_DEBUG,          ///< trace level for detailed debug information
  TRACE_VERY_DEBUG	///< trace level for everything
} trace_level;





/******************************************************************************
 * External variables
 *****************************************************************************/

extern trace_level debug_level; // defined in debug.cc





/******************************************************************************
 * Functions
 *****************************************************************************/

void trace(trace_level pTraceLevel, string message);
void trace(string message);
void show_process_information();
int frontend_error(const char *msg);
void SAerror(unsigned int code, string information = "", int lineNumber = 0);
void SAerror(unsigned int code, string information, string lineNumber);
void genericError(string information, string line, bool error = false);

/// calls #cleanup() then exits
void error();
/// closes all open files and delete all pointers
void cleanup();



/******************************************************************************
 * Macros
 *****************************************************************************/

/*!
 * \def ENTER
 * \brief prints the function name
 *
 * A preprocessor directive to print the name of the called function together
 * with its file position. The \a prefix is used to add a short abbreviation
 * of the module of BPEL2oWFN to which the function belongs to, e.g. "[PN]"
 * for the Petri net class, etc. The macro #ENTER also adds a "+" before the
 * output.
 *
 * \param prefix additional string to print
 *
 * \ingroup debug
*/
#ifndef NDEBUG
#define ENTER(prefix) \
  trace(TRACE_VERY_DEBUG, string(prefix) + "\t+" + string(__FUNCTION__) + \
    "() [" + string(__FILE__) + ":" + toString(__LINE__) + "]\n")
#else
#define ENTER(prefix)
#endif





/*!
 * \def LEAVE
 * \brief prints the function name
 *
 * A preprocessor directive to print the name of the called function together
 * with its file position. The \a prefix is used to add a short abbreviation
 * of the module of BPEL2oWFN to which the function belongs to, e.g. "[PN]"
 * for the Petri net class, etc. The macro #LEAVE also adds a "-" before the
 * output.
 *
 * \param prefix additional string to print
 *
 * \ingroup debug
 */
#ifndef NDEBUG
#define LEAVE(prefix) \
  trace(TRACE_VERY_DEBUG, string(prefix) + "\t-" + string(__FUNCTION__) + \
    "() [" + string(__FILE__) + ":" + toString(__LINE__) + "]\n")
#else
#define LEAVE(prefix)
#endif





#endif



/**
 * \defgroup debug Debug Module
 */
