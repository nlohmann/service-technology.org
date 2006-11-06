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
 * \file    debug.cc
 *
 * \brief   debugging tools
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 *
 * \since   2005/11/09
 *          
 * \date    \$Date: 2006/11/06 14:17:19 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.39 $
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
#include "options.h"
#include "helpers.h"

using namespace std;





/******************************************************************************
 * Global variables
 *****************************************************************************/

/*!
 * \brief debug level
 *
 * The command-line parameter "-d" can be used to set a debug level.
 *
 * \see #trace_level
 * \see #yy_flex_debug
 * \see #yydebug
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
    (*log_output) << message << flush;
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
int yyerror(const char *msg)
{
  /* defined by flex */
  extern int yylineno;      // line number of current token
  extern char *yytext;      // text of the current token

  cerr << filename << ":" << yylineno+1 << " - " << string(msg) << "; last token read: `" << string(yytext) << "'" << endl;

  return 1;
}





/*!
 * \brief prints static analysis error messages
 *
 * Outputs error messages triggered by several static tests. The static
 * anaylsis codes are taken from the WS-BPEL 2.0 specification. For each error
 * code, a standard problem description is printed.
 *
 * \param code		code of the static analysis error
 * \param information	additional information about the error
 * \param lineNumber	a line number to locate the error
 *
 * \ingroup debug
 */
void SAerror(unsigned int code, string information, int lineNumber)
{
  cerr << filename << ":" << lineNumber << " - [SA";
  cerr << setfill('0') << setw(5) << code;
  cerr << "] ";

  switch (code)
  {
    case(6):
      { cerr << "<rethrow> activity must only be used within <catch> or <catchAll>" << endl;
	break; }

    case(7):
      { cerr << "<compensateScope> activity must only be used within an FTC-handler" << endl;
	break; }

    case(8):
      { cerr << "<compensate> activity must only be used within an FTC-handler" << endl;
	break; }
	
    case(15):
      { cerr << "<process> does not contain a start activity" << endl;
	break; }

    case(16):
      { cerr << "<partnerLink> `" << information << "' neither specifies `myRole' nor `partnerRole'" << endl;
	break; }

    case(17):
      { cerr << "<partnerLink> `" << information << "' uses `initializePartnerRole' but does not have a partner role" << endl;
	break; }

    case(18):
      { cerr << "<partnerLink> `" << information << "' already defined in this scope/process" << endl;
	break; }

    case(23):
      { cerr << "<variable> `" << information << "' already defined in this scope/process" << endl;
	break; }

    case(24):
      { cerr << "attribute `" << information << "' must be of type `BPELVariableName'" << endl;
	break; }

    case(25):
      { cerr << "<variable> `" << information << "' must either use `messageType', `type' or `element' attribute" << endl;
	break; }
	
    case(44):
      { cerr << "<correlationSet> `" << information << "' already defined in this scope/process" << endl;
	break; }

    case(51):
      { cerr << "<invoke> must not be used with an inputVariable AND <toPart> elements" << endl;
	break; }

    case(52):
      { cerr << "<invoke> must not be used with an outputVariable AND <fromPart> elements" << endl;
	break; }

    case(55):
      { cerr << "<receive> must not be used with a variable AND <fromPart> elements" << endl;
	break; }

    case(56):
      { cerr << information << " start activity must not be preceeded by another activity" << endl;
	break; }

    case(59):
      { cerr << "<reply> must not be used with a variable AND <toPart> elements" << endl;
	break; }

    case(62):
      { cerr << "<pick> start activity must only contain <onMessage> branches" << endl;
	break; }

    case(63):
      { cerr << "<onMessage> must not be used with a variable AND <fromPart> elements" << endl;
	break; }

    case(64):
      { cerr << "<link> `" << information << "' already defined in this <flow>" << endl;
	break; }
	
    case(65):
      { cerr << "<link> `" << information << "' was not defined before" << endl;
	break; }

    case(66):
      { cerr << "<link> `" << information << "' must be used exactly once as source and target" << endl;
	break; }

    case(68):
      { cerr << information << " more than once as source" << endl;
	break; }

    case(69):
      { cerr << information << " more than once as target" << endl;
	break; }

    case(72):
      { cerr << "<link> `" << information << "' closes a control cycle" << endl;
	break; }

    case(76):
      { cerr << "<variable> `" << information << "' hides enclosing <forEach>'s `counterName'" << endl;
	break; }

    case(77):
      { cerr << "<scope> `" << information << "' is not immediately enclosed to current scope" << endl;
	break; }

    case(78):
      { cerr << "`" << information << "' does not refer to a <scope> or an <invoke> activity" << endl;
	break; }

    case(80):
       { cerr << "<faultHandlers> have no <catch> or <catchAll> element" << endl;
	 break; }

    case(81):
       { cerr << "<catch> attribute " << information << endl;
	 break; }

    case(83):
       { cerr << "<eventHandlers> have no <onEvent> or <onAlarm> element" << endl;
	 break; }

    case(91):
       { cerr << "an isolated <scope> may not contain other isolated scopes" << endl;
	 break; }

    case(92):
       { cerr << "<scope> with name `" << information << "' defined twice" << endl;
	 break; }

    case(93):
       { cerr << "<catch> construct defined twice" << endl;
	 break; }

    default:
	 cerr << endl;
  }
}





/*!
 * \brief prints static analysis error messages
 * \overload
 * \ingroup debug
 */
void SAerror(unsigned int code, string information, string lineNumber)
{
  SAerror(code, information, toInt(lineNumber));
}
