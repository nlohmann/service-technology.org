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
 * \date    \$Date: 2006/11/30 11:31:39 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.64 $
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
#include <climits>

#include "helpers.h"
#include "options.h"
#include "debug.h"
#include "petrinet.h"

using namespace std;





/******************************************************************************
 * Conversion functions
 *****************************************************************************/

#ifdef USING_BPEL2OWFN
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
#endif




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
 * \brief converts a vector to a C++ string
 *
 * Convers an STL vector of unsigned ints to a C++ string representation. The
 * integers are seperated by a period.
 *
 * \param v  an STL vector of unsigned ints
 * \return   C++ string representation of v
 *
 * \ingroup conversion
 */
string toString(vector<unsigned int> &v)
{
  string result;

  for (unsigned int i = 0; i < v.size(); i++)
  {
    if (i != 0)
      result += ".";

    result += toString(v[i]);
  }

  return result;
}





/*!
 * \brief converts string to int
 *
 * Converts a C++ string to a C++ int.
 *
 * \param s C++ string
 * \return int representing s or INT_MAX if the conversion failed
 *
 * \ingroup conversion
 */
int toInt(string s)
{
  int result;
  istringstream isst;
  isst.str(s);
 
  isst >> result;

  if (isst.fail())
    return INT_MAX;

  return result;
}





/*!
 * \brief converts string to unsigned int
 *
 * Converts a C++ string to a C++ unsigned int.
 *
 * \param s C++ string
 * \return unsigned int representing s or UINT_MAX if the conversion failed
 *         (e.g. a negative value was passed)
 *
 * \ingroup conversion
 */
unsigned int toUInt(string s)
{
  unsigned int result;
  istringstream isst;
  isst.str(s);
 
  isst >> result;

  if (isst.fail())
    return UINT_MAX;

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
  extern PetriNet PN;	// defined in bpel2owfn.cc

  trace(TRACE_INFORMATION,"Cleaning up ...\n");

  if ( filename != "<STDIN>" && frontend_in != NULL)
  {
    trace(TRACE_INFORMATION," + Closing input file: " + filename + "\n");
    fclose(frontend_in);
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





/******************************************************************************
 * Other functions
 *****************************************************************************/

/*!
 * \brief increases the index vector
 *
 * The function increases the indices in the vector and propagates resulting
 * carries. For example, if the index vector [3,2] is increased and the maximal
 * bounds are [5,2] the resulting vector is [4,1].
 *
 * \param current_index  vector holding the current indices
 * \param max_index      vector holding the upper bounds of the indices
 *
 * \post Index vector is increased according to the described rules.
 *
 * \invariant Each index lies between 1 and its maximal value, i.e., 1 and the
 *            maximal value can be reached.
 */
void next_index(vector<unsigned int> &current_index, vector<unsigned int> &max_index)
{
  assert(current_index.size() == max_index.size());

  for (unsigned int i = 0; i < current_index.size(); i++)
  {
    if (current_index[i] < max_index[i])
    {
      current_index[i]++;
      break;
    }
    else
      current_index[i] = 1;
  }
}

