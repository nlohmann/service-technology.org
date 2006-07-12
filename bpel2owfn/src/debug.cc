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
 * \file debug.cc
 *
 * \brief Some debugging tools for BPEL2oWFN
 *
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/11/09
 *          - last changed: \$Date: 2006/07/12 10:00:52 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.19 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <fstream>

#include "debug.h"
#include "options.h"
#include "helpers.h"

using namespace std;





/******************************************************************************
 * Global variables
 *****************************************************************************/

/// debug level
trace_level debug_level = TRACE_ALWAYS;





/******************************************************************************
 * Functions to indicate errors, warnings or observations
 *****************************************************************************/

/**
 * Provides output to stderr using different #trace_level 
 * (in order to regulate amount of output)
 *
 * \param pTraceLevel	the #trace_level
 * \param message	the output
 *
 */
void trace(trace_level pTraceLevel, string message)
{
  if (pTraceLevel <= debug_level)
  {
    (*log_output) << message << flush;
  }
}





/**
 * Works like #trace(trace_level,string) with trace_level = TRACE_ALWAYS
 *
 * \param message the output
 *
 */
void trace(string message )
{
  trace(TRACE_ALWAYS, message);
}





/*!
 * This function is invoked by the parser and the lexer during the syntax
 * analysis. When an error occurs, it prints an accordant message and shows the
 * lines of the input files where the error occured.
 *
 * \param msg a message (mostly "Parse error") and some more information e.g.
 *            the location of the syntax error.
 * \return 1, since an error occured
 */
int yyerror(const char* msg)
{
  /* defined by flex */
  extern int yylineno;      // line number of current token
  extern char *yytext;      // text of the current token

  trace("Error while parsing!\n\n");
  trace(msg);
  trace("\n");
	
  // display passed error message
  trace("Error in '" + filename + "' in line ");
  trace(intToString(yylineno));
  trace(":\n");
  trace("  token/text last read was '");
  trace(yytext);
  trace("'\n\n");


  if (filename != "<STDIN>")
  {
    trace("-------------------------------------------------------------------------------\n");
    
    // number of lines to print before and after errorneous line
    int environment = 4;

    unsigned int firstShowedLine = ((yylineno-environment)>0)?(yylineno-environment):1;
  
    ifstream inputFile(filename.c_str());
    string errorLine;
    for (unsigned int i=0; i<firstShowedLine; i++)
    {
      getline(inputFile, errorLine);
    }
    // print the erroneous line (plus/minus three more)
    for (unsigned int i=firstShowedLine; i<=firstShowedLine+(2*environment); i++)
    {
      trace(intToString(i) + ": " + errorLine + "\n");
      getline(inputFile, errorLine);
      if (inputFile.eof())
	break;
    }
    inputFile.close();
    
    trace("-------------------------------------------------------------------------------\n");
  }


  error();
  return 1;
}
