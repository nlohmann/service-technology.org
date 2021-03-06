/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2005, 2006, 2007  Niels Lohmann, Christian Gierds,
                                 Martin Znamirowski, and Dirk Fahland

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    debug.cc
 *
 * \brief   debugging tools
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/11/09
 *
 * \date    \$Date: 2007/06/28 07:38:15 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.106 $
 *
 * \ingroup debug
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>

#include "debug.h"
#include "verbose.h"
//#include "options.h"
#include "config.h"
#include "helpers.h"	// for toInt
#include "globals.h"

using std::cerr;
using std::clog;
using std::cout;
using std::endl;
using std::setw;
using std::setfill;
using std::flush;
using std::ofstream;





/******************************************************************************
 * Global variables
 *****************************************************************************/

/*!
 * \brief debug level
 *
 * The command-line parameter "-d" can be used to set a debug level.
 *
 * \see #trace_level
 * \see #frontend__flex_debug
 * \see #frontend_debug
 *
 * \ingroup debug
 */
trace_level debug_level = TRACE_ALWAYS;





/******************************************************************************
 * Functions to indicate errors, warnings or observations
 *****************************************************************************/

/*!
 * \brief traces a string to the log stream
 *
 * Prints the string message to the output string #log_output if the passed
 * debug level #debug_level is greater or equal to the set debug level
 * #debug_level.
 *
 * \param pTraceLevel	the #trace_level
 * \param message	the output
 *
 * \ingroup debug
 */
void trace(trace_level pTraceLevel, string message)
{
  if (debug_level <= 1) {
      return;
  }
  std::cerr << PACKAGE << ": " << message;
}





/*!
 * \brief traces a string to the log stream
 * \overload
 * \ingroup debug
 */
void trace(string message)
{
  std::cerr << PACKAGE << ": " << message;
}





/*!
 * \brief signal a syntax error
 *
 * This function is invoked by the parser and the lexer during the syntax
 * analysis. When an error occurs, it prints an accordant message and shows
 * the lines of the input files where the error occured.
 *
 * \param msg a message (mostly "Parse error") and some more information e.g.
 *            the location of the syntax error.
 * \return 1, since an error occured
 *
 * \ingroup debug
 */
int frontend_error(const char *msg)
{
  // defined by flex
  extern int frontend_lineno;      // line number of current token
  extern char *frontend_text;      // text of the current token

  message ("%s: %i - [SYNTAX ERROR]", globals::filename.c_str(), frontend_lineno+1);
  message ("%s; last token read: `%s'", msg, frontend_text);

  // remember the last token
  globals::last_error_token = string(frontend_text);
  globals::last_error_line = toString(frontend_lineno);

  return 1;
}
