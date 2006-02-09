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
 * \file helpers.cc
 *
 * \brief Helper functions (implementation)
 *
 * This file implements several small helper functions that do not belong to
 * any other file.
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/11/11
 *          - last changed: \$Date: 2006/02/09 19:09:06 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.43 $
 */





#include "helpers.h"





/******************************************************************************
 * External variables
 *****************************************************************************/

/// The Petri Net
extern PetriNet *TheNet;





/*!
 * \param a set of Petri net nodes
 * \param b set of Petri net nodes
 * \return union of the sets a and b
 */
set<Node *> setUnion(set<Node *> a, set<Node *> b)
{
  set<Node *> result;
  insert_iterator<set<Node *, less<Node *> > > res_ins(result, result.begin());
  set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);
  
  return result;
}





/*!
 * \param i standard C int
 * \return  C++ string representing i
 */
string intToString(int i)
{
  char buffer[20];
  sprintf(buffer, "%d", i);
  
  return string(buffer);
}





/*!
 * \param  file     a filename
 * \param  line     a line number
 * \param  function a function
 * \return a formatted string uniting the given information
 */
string pos(const char *file, int line, const char *function)
{
  string result = "function " + string(function) + " (" + string(file) + ":" + intToString(line) + ")\n";
  return result;
}





/**
 * Some output in case an error has occured.
 * 
 */
void error()
{
  trace("\nAn error has occured while parsing \"" + filename + "\"!\n\n");
  trace(TRACE_WARNINGS, "-> Any output file might be in an undefinded state.\n");
  // call #cleanup()
  cleanup();
  if (log_filename != "")
  {
    trace("\nProgramme aborted due to error.\n\n");
    trace("Please check logfile for detailed information!\n\n");
  }
  exit(1);

}





/**
 * Some output in case an error has occured.
 * Prints out the exception's information.
 *
 * \param e The exception that triggered this function call. 
 * 
 */
void error(Exception e)
{
  trace("\nAn error has occured while parsing \"" + filename + "\"!\n\n");
  // output information about the exception
  e.info();
  trace(TRACE_WARNINGS, "-> Any output file might be in an undefinded state.\n");
  // call #cleanup()
  cleanup();
  if (log_filename != "")
  {
    trace("\nProgramme aborted due to error.\n\n");
    trace("Please check logfile for detailed information!\n\n");
  }
  // stop execution
  exit(e.id);
}





/**
 * Cleans up.
 * Afterwards we should have an almost defined state.
 *
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
 
  if ( output != &std::cout && output != NULL )
  {
    trace(TRACE_INFORMATION," + Closing output file: " + output_filename + ".X\n");
    (*output) << std::flush;
    ((std::ofstream*)output)->close();
    delete(output);
    output = NULL;
  }
 
  if ( log_output != &std::clog )
  {
    trace(TRACE_INFORMATION," + Closing log file: " + log_filename + "\n");
    (*log_output) << std::flush;
    ((std::ofstream*)log_output)->close();
    delete(log_output);
    log_output = &std::cerr;
  }
}
