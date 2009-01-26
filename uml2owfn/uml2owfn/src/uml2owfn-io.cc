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

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <libgen.h>     // for ::basename

#include "uml2owfn.h"
#include "uml2owfn-io.h"

#include "debug.h"      // debugging help
#include "helpers.h"    // toString etc.

using std::cerr;
using std::cout;
using std::endl;
using std::stringstream;


/******************************************************************************
 * Global variables
 *****************************************************************************/

/// The Petri Net
ExtendedWorkflowNet  PN = ExtendedWorkflowNet();
/// formula specifying the final state
FormulaState* finalStateFormula = NULL;
/// formula specifying safness of the net
FormulaState* safeStateFormula = NULL;

/// store for each analysis task whether a taskfile has been written
map<possibleAnalysis,bool> taskFileWritten;
/// string holding the contents of a script
stringstream scriptContents;
/// string holding the contents of a log file
stringstream logContents;

/******************************************************************************
 * file open and lcose functions
 *****************************************************************************/

// opening a file
void open_file(string file) {
    if (inputfiles.size() >= 1) {

        char* ffile = (char*)malloc(file.size() + sizeof(char));
        strcpy(ffile, file.c_str());
        globals::filename = string(::basename(ffile));
        globals::workingDirectory = string(::dirname(ffile));
        free(ffile);

        if (!(frontend_in = fopen(file.c_str(), "r"))) {
            cerr << "Could not open file for reading: " << file.c_str() << endl;
            exit(EXIT_FAILURE);
        }
    }
}

// closing a file
void close_file(string file) {
    if ( globals::filename != "<STDIN>" && frontend_in != NULL) {
        trace(TRACE_INFORMATION," + Closing input file: " + globals::filename + "\n");
        fclose(frontend_in);
        frontend_in = NULL;
    }
}

/******************************************************************************
 * file name manipulation functions
 *****************************************************************************/

/*!
 * \brief remove last extension of a file name
 */
string stripExtension(string fileName) {
    return fileName.substr(0, fileName.rfind('.',fileName.length()));
}

/*!
 * \brief return string of the output file name of the current process
 * \param prefix added in front of the file name
 * \param use_suffix append suffix to filename
 * \pre globals::output_filename and globals::output_filename_suffix have been set
 */
string getOutputFilename(string prefix = "", bool use_suffix=true) {
  if (globals::output_filename == "")
    return "";

  string dirString = (globals::output_directory == ""
      ? globals::workingDirectory
      : globals::output_directory);

  return dirString + GetSeparatorChar() + prefix + globals::output_filename + (use_suffix ? globals::output_filename_suffix : "");
}

/*!
 * \brief find the 'default' output format in the current program call
 *        if multiple output formats are provided, then the first of all
 *        output formats is returned, if no output format is set, then
 *        the standard F_NONE is returned
 */
possibleFormats getOutputDefaultFormat_net () {
    for (int f = (int)F_LOLA; f < (int)F_NONE; f++) {
        if (formats[(possibleFormats)f])
            return (possibleFormats)f;
    }
    return F_NONE;
}

/*!
 * \brief return string of the output file name of the current process
 *
 * \param format of the output file name
 *
 * \pre globals::output_filename and globals::output_filename_suffix have been set
 */
string getOutputFilename_net (possibleFormats format) {
    if (globals::output_filename == "")
        return "";
    else
    {
        string ext = "";
        if (format != F_NONE) ext += "."+suffixes[format];
        return getOutputFilename() + ext;
    }
}

/*!
 * \brief modify the process's name deleting some symbols
 *      such that the outputfile can be easily read by
 *      most programs
 *
 * \param processName current name of the process
 *
 * \returns modified name of the process
 */
string process_name_to_file_name (string processName) {
    string modifiedProcessName = "";

    for (string::iterator c = processName.begin(); c != processName.end(); c++) {
        switch (*c) {
            case '#':
                modifiedProcessName = modifiedProcessName + '_';
                break;
            case ' ':
                break;
            default:
                modifiedProcessName = modifiedProcessName + (*c);
        }
    }
    return modifiedProcessName;
}



/******************************************************************************
 *
 * functions to write files/stdout
 *
 *****************************************************************************/


/******************************************************************************
 * write Petri nets
 *****************************************************************************/

/// write the current Petri net
void write_net_file(analysis_t analysis) {
    trace(TRACE_DEBUG, "-> writing Petri net to file\n");

    // now the net will not change anymore, re-enumerate nodes of the net
    // to anonymize it
    if (globals::parameters[P_ANONYMIZE]) {
      PN.reenumerate();
      PN.anonymizeNodes();
    }

    //  PN.calculate_max_occurrences();
    cerr << PN.information() << endl;

    // create oWFN output ?
    if (formats[F_OWFN]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_OWFN));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for oWFN ...\n");
        PN.set_format(FORMAT_OWFN);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }

    // create LoLA output ?
    if (formats[F_LOLA]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_LOLA));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for LoLA ...\n");
        PN.set_format(FORMAT_LOLA);
        (*output) << PN;

       if (!shall_write_task_to_file(analysis)) { // append verification property to net
           trace(TRACE_DEBUG, "-> Appending specification for verification ...\n");
           (*output) << generate_task_file_contents(analysis);
       }

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }

    // create PNML output ?
    if (formats[F_PNML]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_PNML));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for PNML ...\n");
        PN.set_format(FORMAT_PNML);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }

    // create PEP output ?
    if (formats[F_PEP]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_PEP));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for PEP ...\n");
        PN.set_format(FORMAT_PEP);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }

    // create Woflan TPN output ?
    if (formats[F_TPN]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_TPN));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for Woflan (TPN) ...\n");
        PN.set_format(FORMAT_TPN);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }

    // create INA output ?
    if (formats[F_INA]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_INA));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for INA ...\n");
        PN.set_format(FORMAT_INA);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }

    // create SPIN output ?
    if (formats[F_SPIN]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_SPIN));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for SPIN ...\n");
        PN.set_format(FORMAT_SPIN);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }

    // create APNN output ?
    if (formats[F_APNN]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_APNN));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for APNN ...\n");
        PN.set_format(FORMAT_APNN);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }

    // create dot output ?
    if (formats[F_DOT]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_DOT));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net for dot ...\n");
        PN.set_format(FORMAT_DOT, true);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;

#ifdef HAVE_DOT
            string systemcall = "dot -q -Tpng -o" + getOutputFilename_net(F_NONE)
                + ".png " + getOutputFilename_net(F_DOT);
            trace(TRACE_INFORMATION, "Invoking dot with the following options:\n");
            trace(TRACE_INFORMATION, systemcall + "\n\n");
            int dummyCatch = system(systemcall.c_str());
#endif
        }
    }

    // create info file ?
    if (formats[F_INFO]) {
        if (globals::output_filename != "") {
            output = openOutput(getOutputFilename_net(F_INFO));
        }

        trace(TRACE_INFORMATION, "-> Printing Petri net information ...\n");
        PN.set_format(FORMAT_INFO);
        (*output) << PN;

        if (globals::output_filename != "") {
            closeOutput(output);
            output = NULL;
        }
    }
}

/******************************************************************************
 * write log files
 *****************************************************************************/

void log_print (string text) {
  if (!globals::parameters[P_LOG]) return;
  logContents << text;
}

void log_println (string text) {
  if (!globals::parameters[P_LOG]) return;
  logContents << text << endl;
}

void write_log_file () {
  if (!globals::parameters[P_LOG]) return;

  // write log file
  if (globals::output_filename != "")
  {
    trace(TRACE_INFORMATION, "writing log file to uml2owfn_" + globals::output_filename + ".log ... ");
    output = openOutput(getOutputFilename("uml2owfn_", false) + ".log");
  }

  (*output) << logContents.str() << endl;

  if (globals::output_filename != "") {
    closeOutput(output);
    output = NULL;
    trace(TRACE_INFORMATION, "done.\n");
  }

  logContents.flush();
}

/******************************************************************************
 * write task files
 *****************************************************************************/

/*!
 * \brief creates a string that is the contents of task or analysis file
 *        regardin the given analysis setting, the method concatenates all
 *        analysis tasks. It is a convenience method used in #write_net_file()
 *        to append a single task to the file. If the analysis requires
 *        several task files to be written, this method gives incorrect results.
 *        For multiple task files, use the dedicated methods
 *           #generate_mc_task_file_contents()
 *           #generate_state_task_file_contents()
 *        and/or
 *           #write_task_file()
 *
 * \param analysis the analysis setting for which the task file shall be
 *        written
 *
 * \return string to be written to a file
 */
string generate_task_file_contents (analysis_t analysis) {
  // append all task file contents
  string contents = "";
  contents += generate_task_file_contents_finalState(analysis)+"\n\n";
  contents += generate_task_file_contents_safeState(analysis)+"\n\n";
  return contents;
}

/*!
 * \brief generate a task file string for checking the reachability
 *        of a final state
 * \param analysis the analysis setting for which the task file shall be
 *        written
 * \pre #finalStateFormula != NULL
 * \return string to be written to a file
 */
string generate_task_file_contents_finalState (analysis_t analysis)
{
  stringstream taskContents;

  if ( formats[F_LOLA] ) {
    if (analysis[A_SOUNDNESS]) {
      if (!analysis[A_DEADLOCKS]) {
        // formula that specifies final state
        // create an AG EF model checking formula and write the .task-file
        if (globals::parameters[P_CTL]) {
          // generate a CTL-formula
          FormulaState ctl_AG = FormulaState(LOGQ_CTL_ALLPATH_ALWAYS);
          FormulaState ctl_EF = FormulaState(LOGQ_CTL_EXPATH_EVENTUALLY);
          ctl_EF.subFormulas.insert(finalStateFormula);
          ctl_AG.subFormulas.insert(&ctl_EF);
          ctl_AG.set_format(FORMAT_LOLA_FORMULA);
          // and write to output stream
          taskContents << ctl_AG << endl;
        } else {
          // generate a state predicate and write to output stream
          finalStateFormula->set_format(FORMAT_LOLA_STATEPREDICATE);
          taskContents << *finalStateFormula << endl;
        }
      }
    }
  }

  return taskContents.str();
}

/*!
 * \brief generate a task file string for checking the safeness of the net
 * \param analysis the analysis setting for which the task file shall be
 *        written
 * \pre #safeStateFormula != NULL
 * \return string to be written to a file
 */
string generate_task_file_contents_safeState (analysis_t analysis)
{
  stringstream taskContents;

  if ( formats[F_LOLA] ) {
    if (analysis[A_SAFE]) {
      safeStateFormula->set_format(FORMAT_LOLA_STATEPREDICATE);
      taskContents << (*safeStateFormula) << endl;
    }
  }

  return taskContents.str();
}

/*!
 * \brief write additional files, e.g. task files for LoLA
 *
 * \post  taskFileWritten is set to true iff a file was written to the disk
 */
void write_task_file (analysis_t analysis) {
  if (!shall_write_task_to_file(analysis))
    return; // skip

  // in this run, we did not write a task-file yet
  for (int i=0;i<(int)A_MAX;i++)
    taskFileWritten[(possibleAnalysis)i] = false;

  if ( formats[F_LOLA] ) {
    if (analysis[A_SOUNDNESS]) {
      if (!analysis[A_DEADLOCKS]) {
        trace(TRACE_DEBUG, "-> writing soundness analysis task to file\n");
        // currently, we distinguish stop-nodes from end-nodes
        // by a state-predicate, written to a .task-file
        if (globals::output_filename != "") {
          output = openOutput(getOutputFilename_net(F_LOLA) + ".fin.task");
        }

        (*output) << generate_task_file_contents_finalState(analysis) << endl << endl;

        if (globals::output_filename != "") {
          closeOutput(output);
          output = NULL;
          taskFileWritten[A_SOUNDNESS] = true;
        }
      }
    }

    if (analysis[A_SAFE]) {
      trace(TRACE_DEBUG, "-> writing safeness analysis task to file\n");
      if (globals::output_filename != "") {
        output = openOutput(getOutputFilename_net(F_LOLA) + ".safe.task");
      }

      (*output) << generate_task_file_contents_safeState(analysis) << endl << endl;

      if (globals::output_filename != "") {
        closeOutput(output);
        output = NULL;
        taskFileWritten[A_SAFE] = true;
      }
    }
  }
}

/*
 * \brief distinguish the cases where a task file has to be written to
 *        the disk rather than to be appended to the net file
 */
bool shall_write_task_to_file (analysis_t analysis) {
  if (formats[F_LOLA])
  {
    // check cases where a task file is generated
    if (analysis[A_SOUNDNESS] && !analysis[A_DEADLOCKS])
      return globals::parameters[P_TASKFILE];  // take global setting
    if (analysis[A_SAFE])
       return globals::parameters[P_TASKFILE]; // take global setting
  }
  return true;  // in general yes, whether task file is really written
                // depends on write_task_file()
}

/******************************************************************************
 * write script files
 *****************************************************************************/

/*!
 * \brief generates and writes a script file (if applicable) for the current
 *      call, e.g. a script file to invoke lola for soudness checking
 */
void write_script_file () {
    trace(TRACE_DEBUG, "writing script file...\n");
  if ( formats[F_LOLA] ) {
    if ( globals::analysis[A_SOUNDNESS] )
    {
      // write script file for soundness analysis
      if (globals::output_filename != "")
      {
        output = openOutput(getOutputFilename("check_", false) + ".sh");
      }

      (*output) << "# script for checking the processes for soundness" << endl;
      (*output) << scriptContents.str();

      if (globals::output_filename != "") {
        closeOutput(output);
        output = NULL;
      }
    }
  }
}

void extend_script_file_subAnalysis_lola (possibleAnalysis an) {

  string analysis_text;
  string taskFile_suffix;
  string lolaCommand;
  bool isTaskFileAnalysis = false;

  if (an == A_DEADLOCKS) {
    analysis_text = " for deadlocks";
    lolaCommand = "lola-dl";
    taskFile_suffix = "";

  } else if (an == A_SAFE) {
    analysis_text = " for safeness";
    lolaCommand = "lola-sp";
    taskFile_suffix = ".safe"; // safeness of the net
    isTaskFileAnalysis = true;

  } else if (an == A_SOUNDNESS) {
    analysis_text = " for soundness";
    if (globals::parameters[P_CTL])
      lolaCommand = "lola-mc";
    else
      lolaCommand = "lola-lp";
    taskFile_suffix = ".fin"; // reachability of final state
    isTaskFileAnalysis = true;

  } else {
    trace(TRACE_ERROR, " [ERROR] unknown analysis task , cannot generate script file\n");
    return;
  }

  scriptContents << "echo ----------------------------------------------------------------" << endl;
  scriptContents << "echo  checking " << getOutputFilename() << analysis_text << endl;
  scriptContents << "echo ----------------------------------------------------------------" << endl;

  if (isTaskFileAnalysis && taskFileWritten[an]) {
    scriptContents << "${1}" << lolaCommand << " "
      << getOutputFilename_net(F_LOLA)
      << " -a " << getOutputFilename_net(F_LOLA) << taskFile_suffix << ".task"
      << " -P"
      << endl;
  } else {
    scriptContents << "${1}" << lolaCommand << " "
      << getOutputFilename_net(F_LOLA)
      << " -P"
      << endl;
  }
  scriptContents << endl;
}

/*!
 * \brief generates and writes a script file (if applicable) for the current
 *      call, e.g. a script file to invoke lola for soudness checking
 *
 * \pre   write_net_file() and write_script_file() have already been called
 *      for the current output
 */
void extend_script_file (analysis_t analysis) {
    trace(TRACE_DEBUG, "-> extending script file...");
  if ( formats[F_LOLA] ) {
    if (analysis[A_SAFE])
       extend_script_file_subAnalysis_lola(A_SAFE);

    if ( analysis[A_SOUNDNESS] ) {

      if (analysis[A_DEADLOCKS])
        extend_script_file_subAnalysis_lola(A_DEADLOCKS);
      else
        extend_script_file_subAnalysis_lola(A_SOUNDNESS);
    }
  }
  trace(TRACE_DEBUG, "done\n");
}
