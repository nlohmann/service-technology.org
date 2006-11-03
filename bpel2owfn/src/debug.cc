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
 * \date    \$Date: 2006/11/03 14:57:45 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.24 $
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

  trace("===============================================================================\n");
  trace("PARSE ERROR\n\n");

  trace("+ Problem: " + string(msg) + " in file `" + filename + "' near line " + toString(yylineno) + ".\n");
  trace("  The token/text last read was `" + string(yytext) + "'.\n\n");
  
  showLineEnvironment(yylineno);

  trace("+ Description: either the input file does not validate with the BPEL XML schema\n");
  trace("  or it uses extension elements that cannot be processed by BPEL2oWFN.\n\n");

  trace("For more information see Appendix E of the WS-BPEL 2.0 specification.\n");
  trace("===============================================================================\n\n");

  error();
  return 1;
}





/*!
 * \brief prints a line of the input file
 * 
 * Outputs the environment (i.e. four lines before and after) of a line in the
 * input file.
 *
 * \param lineNumber line number
 *
 * \ingroup debug
 */
void showLineEnvironment(int lineNumber)
{
  if (filename != "<STDIN>")
  {
    // number of lines to print before and after errorneous line
    int environment = 4;

    unsigned int firstShowedLine = ((lineNumber-environment)>0)?(lineNumber-environment):1;
  
    ifstream inputFile(filename.c_str());
    string errorLine;
    for (unsigned int i=0; i<firstShowedLine; i++)
    {
      getline(inputFile, errorLine);
    }
    // print the erroneous line (plus/minus three more)
    for (unsigned int i=firstShowedLine; i<=firstShowedLine+(2*environment); i++)
    {
      trace(TRACE_INFORMATION, "  " + toString(i) + ": " + errorLine + "\n");
      getline(inputFile, errorLine);
      if (inputFile.eof())
	break;
    }
    trace(TRACE_INFORMATION, "\n");
    inputFile.close();
  }
}






void SAerror(unsigned int code, string information, string lineNumber)
{
  SAerror(code, information, toInt(lineNumber));
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
  cerr << "error in " << filename << ":" << lineNumber << " - [SA";
  cerr << setfill('0') << setw(5) << code;
  cerr << "] ";

  switch (code)
  {
    case(23):
      {
	cerr << "<variable> `" << information << "' defined twice" << endl;
	break;
      }

    case(72):
      {
	cerr << "<link> `" << information << "' closes a control cycle" << endl;
	break;
      }

    case(77):
      {
	cerr << "<scope> `" << information << "' is not immediately enclosed to current scope" << endl;
	break;
      }

    case(78):
      {
	cerr << "`" << information << "' does not refer to a <scope> or an <invoke> activity" << endl;
	break;
      }

    case(80):
       {
	 cerr << "<faultHandler> have no <catch> or <catchAll> element" << endl;
	 break;
       }

    case(83):
       {
	 cerr << "<eventHandlers> have no <onEvent> or <onAlarm> element" << endl;
	 break;
       }

    default:
      break;
  }
}
