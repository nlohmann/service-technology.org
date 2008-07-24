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
 * \file    globals.cc
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
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.32 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include "globals.h"
#include "block.h"





namespace globals {

  string finalCondition = "";
  
    set<Block*> processes;
    set<Block*> tasks;
    set<Block*> services;
    
  string program_name ="";

  string invocation = "";
 
  string filename = "<STDIN>";
  string output_filename = "";
  string output_filename_suffix = "";	///< suffix of the current output file (if multiple files are written)

  map<possibleParameters, bool> parameters;
  analysis_t analysis;

  string last_error_token = "";
  string last_error_line = "";

  unsigned int reduction_level = 0;
  
// <Dirk.F start> store the current process name (for output)
  string currentProcessName = "";
// <Dirk.F end>
}
