/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
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
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
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
//#include "options.h"
#include "uml2owfn.h"
#include "helpers.h"	// for toInt
#include "globals.h"
#include "colorconsole.h"

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
  if (pTraceLevel <= debug_level)
    std::cout << message << flush;
}





/*!
 * \brief traces a string to the log stream
 * \overload
 * \ingroup debug
 */
void trace(string message)
{
  trace(TRACE_ALWAYS, message);
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

  cerr << colorconsole::fg_blue;
  cerr << globals::filename << ":" << frontend_lineno+1 << " - [SYNTAX]\n";
  cerr << colorconsole::fg_standard;

  if (debug_level == TRACE_ERROR)
    return 1;

  cerr << string(msg);

  if (debug_level >= TRACE_WARNINGS)
    cerr << "; last token read: `" << string(frontend_text) << "'" << endl << endl;
  else
    cerr << endl << endl;

  // remember the last token
  globals::last_error_token = string(frontend_text);
  globals::last_error_line = toString(frontend_lineno);


  return 1;

}


