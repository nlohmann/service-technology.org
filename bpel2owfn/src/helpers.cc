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
 * \file    helpers.cc
 *
 * \brief   helper functions
 *
 * \author  responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nlohmann $
 * 
 * \since   2005/11/11
 *
 * \date    \$Date: 2006/10/25 06:53:38 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.57 $
 *
 * \ingroup conversion
 * \ingroup debug
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>

#include "helpers.h"
#include "options.h"
#include "debug.h"
#include "petrinet.h"

using namespace std;





/******************************************************************************
 * External variables
 *****************************************************************************/

// The Petri Net
extern PetriNet *TheNet;	// defined in bpel2owfn.cc





/******************************************************************************
 * Conversion functions
 *****************************************************************************/

/*!
 * \brief converts integer to string
 *
 * Converts a Kimwitu++ kc::integer to a C++ string.
 *
 * \param i Kimwitu++ integer
 * \return  C++ string representing i
 *
 * \ingroup conversion
 */
string toString(kc::integer i)
{
  return toString(i->value);
}





/*!
 * \brief converts int to string
 *
 * Converts a C++ int to a C++ string.
 *
 * \param i standard C int
 * \return  C++ string representing i
 *
 * \ingroup conversion
 */
string toString(int i)
{
  char buffer[20];
  sprintf(buffer, "%d", i);
  
  return string(buffer);
}





/*!
 * \brief converts string to int
 *
 * Converts a C++ int to a C++ int.
 *
 * \param s C++ string
 * \return integer representing s
 *
 * \ingroup conversion
 */
int toInt(string s)
{
  int result;
  istringstream isst;
  isst.str(s);
  isst >> result;

  return result;
}





/******************************************************************************
 * Error handling functions
 *****************************************************************************/

/*!
 * \brief calls #cleanup() then exits
 *
 * Some output in case an error has occured.
 *
 * \post all goals from #cleanup
 * \post programm terminated
 *
 * \ingroup debug
 */
void error()
{
  trace("\nAn error has occured while parsing \"" + filename + "\"!\n\n");
  trace(TRACE_WARNINGS, "-> Any output file might be in an undefinded state.\n");

  cleanup();

  if (log_filename != "")
  {
    trace("\nProgramme aborted due to error.\n\n");
    trace("Please check logfile for detailed information!\n\n");
  }

  exit(1);
}





/*!
 * \brief closes all open files and delete all pointers
 *
 * Cleans up. Afterwards we should have an almost defined state.
 *
 * \post input file closed
 * \post current output file closed
 * \post log file closed
 * \post Petri net (pointer #TheNet) deleted
 *
 * \ingroup debug
 */
void cleanup()
{
  trace(TRACE_INFORMATION,"Cleaning up ...\n");

  if ( modus == M_PETRINET ) 
  {
    trace(TRACE_INFORMATION," + Deleting Petri Net pointer\n");
    delete(TheNet);
    TheNet = NULL;
  }

  if ( filename != "<STDIN>" && yyin != NULL)
  {
    trace(TRACE_INFORMATION," + Closing input file: " + filename + "\n");
    fclose(yyin);
  }
 
  if ( output != &cout && output != &clog && output != log_output && output != NULL )
  {
    trace(TRACE_INFORMATION," + Closing output file: " + output_filename + ".X\n");
    (*output) << flush;
    ((ofstream*)output)->close();
    delete(output);
    output = NULL;
  }
 
  if ( log_output != &clog )
  {
    trace(TRACE_INFORMATION," + Closing log file: " + log_filename + "\n");
    (*log_output) << flush;
    ((ofstream*)log_output)->close();
    delete(log_output);
    log_output = &cerr;
  }
}
