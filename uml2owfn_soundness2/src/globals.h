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
 * \file    globals.h
 *
 * \brief   global varialbes
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 * 
 * \since   2007/03/05
 *
 * \date    \$Date: 2007/06/28 07:38:16 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitšt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.34 $
 */





#ifndef GLOBALS_H
#define GLOBALS_H





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <set>
#include <string>
#include <map>

#include "options.h"
#include "block.h"

using std::set;
using std::string;
using std::map;


/*!
 * \brief namespace of all global variables
 */
namespace globals
{

    extern set<Block*> processes;
    extern set<Block*> tasks;
    extern set<Block*> services;
    

  extern string finalCondition;

  /// string holding the called program name of BPEL2oWFN
  extern string program_name;

  extern string invocation;

  /// filename of input file
  extern string filename;

  /// filename of output file
  extern string output_filename;
  
  /// suffix for output file name, used if multiple files are written
  extern string output_filename_suffix;

  /// parameters (set by #parse_command_line)
  extern map<possibleParameters, bool> parameters;
  
  /// analysis task flags (set by #parse_command_line)
  extern map<possibleAnalysis, bool> analysis;

  /// the last token that was displayed in an error message
  extern string last_error_token;

  /// the line of the last syntax error
  extern string last_error_line;
  
  extern unsigned int reduction_level;
  
// <Dirk.F start> 
  /// store current process name for output
  extern string currentProcessName;
// <Dirk.F end>

}

#endif
