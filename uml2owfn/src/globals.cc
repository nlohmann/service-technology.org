/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2008 Dirk Fahland <dirk.fahland@service-technolog.org>,
                     Martin Znamirowski <znamirow@informatik.hu-berlin.de>
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    globals.cc
 *
 * \brief   global variables
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2007/03/05
 *
 * \date    \$Date: 2007/06/28 07:38:16 $
 *
 * \note    This file was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.32 $
 */


/******************************************************************************
 * Headers
 *****************************************************************************/

#include "globals.h"

namespace globals {

  string finalCondition = "";

  ASTNode* rootNode;

  string program_name ="";

  string invocation = "";

  // set standard values for directories and file names
  string workingDirectory = "";
  string filename = "<STDIN>";
  string output_directory = ".";      // write to current directory by default
  string output_filename = "";
  string output_filename_suffix = ""; // suffix of the current output file (if multiple files are written)
  bool getOutputFileNameFromInput = false;

  map<possibleParameters, bool> parameters;
  analysis_t analysis;

  string last_error_token = "";
  string last_error_line = "";

  unsigned int reduction_level = 0;

// <Dirk.F start>
  string currentProcessName = "";
// <Dirk.F end>

  // Variables for cutting nets by roles
  set<string>         keepRoles;
  set< set<string> >  keepRolesExact;
  set<string>         exclusiveRoles;
  bool deriveRolesToCut = false;
}
