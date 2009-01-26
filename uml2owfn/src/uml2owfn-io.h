/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2008  Dirk Fahland <dirk.fahland@service-technolog.org>

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


#ifndef UML2OWFNIO_H_
#define UML2OWFNIO_H_

#include <string.h>

#include "pnapi.h"      // Petri Net support
#include "globals.h"

using std::string;

// global variables

/// The Petri Net
extern ExtendedWorkflowNet  PN;
/// formula specifying the final state
extern FormulaState* finalStateFormula;
/// formula specifying safness of the net
extern FormulaState* safeStateFormula;

// define path separator
#ifdef WIN32
#define GetSeparatorChar() '\\'
#else
#define GetSeparatorChar() '/'
#endif

// file open/close functions
void open_file(string file);
void close_file(string file);

// file name manipulation functions
string stripExtension(string fileName);
string getOutputFilename(string prefix, bool use_suffix);
possibleFormats getOutputDefaultFormat_net ();
string getOutputFilename_net (possibleFormats format);
string process_name_to_file_name (string processName);

// file contents create/write functions
void write_net_file(analysis_t analysis);

void write_log_file ();
void log_print (string text);
void log_println (string text);

void write_task_file (analysis_t analysis);
bool shall_write_task_to_file (analysis_t analysis);
string generate_task_file_contents (analysis_t analysis);
string generate_task_file_contents_finalState (analysis_t analysis);
string generate_task_file_contents_safeState (analysis_t analysis);

void write_script_file ();
void extend_script_file_subAnalysis_lola (possibleAnalysis an);
void extend_script_file (analysis_t analysis);

#endif /* UML2OWFNIO_H_ */
