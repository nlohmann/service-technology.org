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
 * \brief Some debugging tools for BPEL2oWFN
 *
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/11/09
 *          - last changed: \$Date: 2006/10/20 19:31:54 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.21 $
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

typedef enum
{
  TRACE_ALWAYS,		/// trace level for errors
  TRACE_WARNINGS,       /// trace level for warnings 
  TRACE_INFORMATION,	/// trace level some more (useful) information
  TRACE_DEBUG,          /// trace level for detailed debug information
  TRACE_VERY_DEBUG	/// trace level for everything (? information collapse)
} trace_level;





/******************************************************************************
 * External variables
 *****************************************************************************/

extern trace_level debug_level;





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

// a preprocessor directive to print the called function and its position
#define ENTER(prefix) \
  trace(TRACE_VERY_DEBUG, string(prefix) + "\t+" + string(__FUNCTION__) + "() [" + string(__FILE__) + ":" + toString(__LINE__) + "]\n")

#define LEAVE(prefix) \
  trace(TRACE_VERY_DEBUG, string(prefix) + "\t-" + string(__FUNCTION__) + "() [" + string(__FILE__) + ":" + toString(__LINE__) + "]\n")




#endif
