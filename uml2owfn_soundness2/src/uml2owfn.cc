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
 * \file    bpel2owfn.cc
 *
 * \brief   BPEL2oWFN's main
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2007/06/28 07:38:15 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.185 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/


#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <map>
#include <set>

#include "uml2owfn.h"   // generated configuration file
#include "pnapi.h"      // Petri Net support
#include "bom-process.h"	// representation of BOM processes
#include "debug.h"		// debugging help
#include "options.h"
#include "globals.h"
#include "helpers.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::map;
using std::set;



/******************************************************************************
 * External functions
 *****************************************************************************/
extern int frontend_parse();		// from Bison
extern int frontend_debug;			// from Bison
extern int frontend_nerrs;			// from Bison
extern int frontend_lineno;			// from Bison
extern int frontend__flex_debug;		// from flex
extern FILE *frontend_in;			// from flex
extern void frontend_restart(FILE*);		// from flex




/******************************************************************************
 * Global variables
 *****************************************************************************/

/// The Petri Net
ExtendedWorkflowNet PN = ExtendedWorkflowNet();
/// formula specifying the final state
FormulaState* finalStateFormula = NULL;
/// formula specifying safness of the net
FormulaState* safeStateFormula = NULL;

map<possibleAnalysis,bool> taskFileWritten;
/// string holding the contents of a script
stringstream scriptContents;
/// string holding the contents of a log file
stringstream logContents;

/******************************************************************************
 * program parts
 *****************************************************************************/

string generate_task_file_contents (analysis_t analysis);
bool write_task_to_file (analysis_t analysis);

void log_print (string text);
void log_println (string text);

/******************************************************************************
 * helper functions
 *****************************************************************************/


/*!
 * \brief	modify the process's name deleting some symbols
 * 			such that the outputfile can be easily read by 
 * 			most programs
 * 
 * \param	processName	current name of the process
 * 
 * \returns	modified name of the process
 */
string process_name_to_file_name (string processName)
{
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

// analyzation of the commandline
void analyze_cl(int argc, char *argv[]) 
{

  // setting globals
  globals::program_name = string(argv[0]);

  for (int i = 0; i < argc; i++)
  {
    globals::invocation += string(argv[i]);
    if (i != (argc-1))
      globals::invocation += " ";
  }

  // Reading command line arguments and triggering appropriate behaviour.
  // In case of false parameters call command line help function and exit.
  parse_command_line(argc, argv);

}



// opening a file
void open_file(string file) 
{
  if (inputfiles.size() >= 1)
  {
    globals::filename = file;
    if (!(frontend_in = fopen(globals::filename.c_str(), "r"))) 
    {
      cerr << "Could not open file for reading: " << globals::filename.c_str() << endl;
      exit(2);
    }
  }
}


// closing a file
void close_file(string file) 
{
  if ( globals::filename != "<STDIN>" && frontend_in != NULL)
  {
    trace(TRACE_INFORMATION," + Closing input file: " + globals::filename + "\n");
    fclose(frontend_in);
    frontend_in = NULL;
  }
}

/******************************************************************************
 * output functions
 *****************************************************************************/

/*!
 * \brief return string of the output file name of the current process
 * \pre globals::output_filename and globals::output_filename_suffix have been set
 */ 
string getOutputFilename() {
  if (globals::output_filename == "")
    return "";
  else
    return globals::output_filename + globals::output_filename_suffix;
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
 * \brief	write resulting Petri net to output (file or output stream)
 */
void write_net_file(analysis_t analysis)
{
    trace(TRACE_DEBUG, "-> writing Petri net to file\n");
    // now the net will not change any more, thus the nodes are re-enumerated
    // and the maximal occurrences of the nodes are calculated.
// <Dirk.F start>
    //DO NOT RE-ENUMERATE PLACES
    //PN.reenumerate();
// <Dirk.F end>
    //  PN.calculate_max_occurrences();
  cerr << PN.information() << endl;
    
    
  // create oWFN output ?
  if (formats[F_OWFN])
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net(F_OWFN));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for oWFN ...\n");
	  PN.set_format(FORMAT_OWFN);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }
  }


  // create LoLA output ?
  if ( formats[F_LOLA] )
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net (F_LOLA));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for LoLA ...\n");
	  PN.set_format(FORMAT_LOLA);
	  (*output) << PN;
	  
	  if (!write_task_to_file(analysis)) {	// append verification property to net
		  trace(TRACE_DEBUG, "-> Appending specification for verification ...\n");
		  (*output) << generate_task_file_contents(analysis);
	  }
	  
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }

	  if ( analysis[A_SOUNDNESS] ) {
		  // ...
	  }
  }


  // create PNML output ?
  if ( formats[F_PNML] )
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net(F_PNML));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for PNML ...\n");
	  PN.set_format(FORMAT_PNML);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }
  }


  // create PEP output ?
  if ( formats[F_PEP] )
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net(F_PEP));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for PEP ...\n");
	  PN.set_format(FORMAT_PEP);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }
  }

    // create Woflan TPN output ?
    if ( formats[F_TPN] )
    {
        if (globals::output_filename != "")
        {
            output = openOutput(getOutputFilename_net(F_TPN));
        }
        trace(TRACE_INFORMATION, "-> Printing Petri net for Woflan (TPN) ...\n");
        PN.set_format(FORMAT_TPN);
        (*output) << PN;
        if (globals::output_filename != "")
        {
            closeOutput(output);
            output = NULL;
        }
    }
    

  // create INA output ?
  if ( formats[F_INA] )
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net(F_INA));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for INA ...\n");
	  PN.set_format(FORMAT_INA);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }
  }

  // create SPIN output ?
  if ( formats[F_SPIN] )
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net(F_SPIN));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for SPIN ...\n");
	  PN.set_format(FORMAT_SPIN);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }
  }

  // create APNN output ?
  if ( formats[F_APNN] )
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net(F_APNN));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for APNN ...\n");
	  PN.set_format(FORMAT_APNN);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }
  }


  // create dot output ?
  if ( formats[F_DOT] )
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net(F_DOT));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for dot ...\n");
	  PN.set_format(FORMAT_DOT, true);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;

#ifdef HAVE_DOT
		  string systemcall = "dot -q -Tpng -o" + getOutputFilename_net (F_NONE) + ".png " + getOutputFilename_net (F_DOT);
		  trace(TRACE_INFORMATION, "Invoking dot with the following options:\n");
		  trace(TRACE_INFORMATION, systemcall + "\n\n");
		  system(systemcall.c_str());
#endif
	  }
  }


  // create info file ?
  if ( formats[F_INFO] )
  {
	  if (globals::output_filename != "")
	  {
		  output = openOutput(getOutputFilename_net(F_INFO));
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net information ...\n");
	  PN.set_format(FORMAT_INFO);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }
  }
}

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
		output = openOutput("uml2owfn_" + globals::output_filename + ".log");
	}
	
	(*output) << logContents.str() << endl;

	if (globals::output_filename != "") {
		closeOutput(output);
		output = NULL;
		trace(TRACE_INFORMATION, "done.\n");
	}
}

string generate_task_file_contents_finalState (analysis_t analysis);
string generate_task_file_contents_safeState (analysis_t analysis);
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
string generate_task_file_contents (analysis_t analysis)
{
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
				FormulaState ctl_AG = FormulaState(LOGQ_CTL_ALLPATH_ALWAYS);
				FormulaState ctl_EF = FormulaState(LOGQ_CTL_EXPATH_EVENTUALLY);
				ctl_EF.subFormulas.insert(finalStateFormula);
				ctl_AG.subFormulas.insert(&ctl_EF);
			  ctl_AG.set_format(FORMAT_LOLA_FORMULA);
			  taskContents << ctl_AG << endl;
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
 * \brief	write additional files, e.g. task files for LoLA
 * 
 * \post	taskFileWritten is set to true iff a file was written to the disk
 */
void write_task_file (analysis_t analysis)
{
	if (!write_task_to_file(analysis))
		return;	// skip
	
	// in this run, we did not write a task-file yet
	for (int i=0;i<(int)A_MAX;i++)
	  taskFileWritten[(possibleAnalysis)i] = false;
	
	if ( formats[F_LOLA] ) {
		if (analysis[A_SOUNDNESS]) {
			if (!analysis[A_DEADLOCKS]) {
			  trace(TRACE_DEBUG, "-> writing soundness analysis task to file\n");
				// currently, we distinguish stop-nodes from end-nodes 
				// by a state-predicate, written to a .task-file 
				if (globals::output_filename != "")	{
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
bool write_task_to_file (analysis_t analysis) {
	if (formats[F_LOLA])
	{
	  // check cases where a task file is generated
	  if (analysis[A_SOUNDNESS] && !analysis[A_DEADLOCKS])
	    return globals::parameters[P_TASKFILE];  // take global setting
	  if (analysis[A_SAFE])
	     return globals::parameters[P_TASKFILE]; // take global setting
	}
	return true;	// in general yes, whether task file is written depends on
					// write_task_file()
}

/*!
 * \brief	generates and writes a script file (if applicable) for the current
 * 			call, e.g. a script file to invoke lola for soudness checking
 */
void write_script_file () {
    trace(TRACE_DEBUG, "writing script file...\n");
	if ( formats[F_LOLA] ) {
		if ( globals::analysis[A_SOUNDNESS] )
		{
			// write script file for soundness analysis
			if (globals::output_filename != "")
			{
				output = openOutput("check_" + globals::output_filename + ".sh");
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
    lolaCommand = "lola-state";
    taskFile_suffix = ".safe"; // safeness of the net
    isTaskFileAnalysis = true;
    
  } else if (an == A_SOUNDNESS) {
    analysis_text = " for soundness";
    lolaCommand = "lola-mc";
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
 * \brief	generates and writes a script file (if applicable) for the current
 * 			call, e.g. a script file to invoke lola for soudness checking
 * 
 * \pre		write_net_file() and write_script_file() have already been called
 * 			for the current output
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

// translation results for handling errors during the translation
typedef enum {
	RES_OK,
	RES_EMPTY_PROCESS,
	RES_NOT_FREECHOICE,
	RES_NO_WF_STRUCTURE
} translationResult_t;

/*!
 * \brief	translate the process according to the given analysis flags,
 * 			calls #write_net_file(), #write_task_file(), #extend_script_file()
 * 			accordingly
 */
translationResult_t translate_process(Block *process, analysis_t analysis, unsigned int reduction_level)
{
	translationResult_t res = RES_OK;
	
	trace(TRACE_WARNINGS, "\ngenerating Petri net for " + process->name + "\n");	    	

    // Generates a petri net from the current process
    //PetriNet* tryPN = new PetriNet();
    BomProcess *bom = new BomProcess();
    PN = ExtendedWorkflowNet();	// get a new net
    process->returnNet(&PN, bom);
    trace(TRACE_DEBUG, "-> done\n");

    // DECOMPOSITION
    // Creating an end state for the decomposed oWFN
    //(*process)->createEndState(tryPN);

    // Delete places in the net that represent unused pins
    // of tasks in the business object model
    //tryPN->fixIBMNet();
    if (not(analysis[A_KEEP_UNCONN_PINS])) {
        trace(TRACE_DEBUG, "-> removing unconnected pins\n");
    	bom->removeUnconnectedPins(&PN);
    	//bom->removeEmptyOutputPinSets(&PN);
    }
    
    // extend net for soundness checking
    if (analysis[A_SOUNDNESS]) {
        
      // check net structure: free choice?
      trace(TRACE_DEBUG, "-> checking whether net is free-choice\n");
      set<Node *> nonFC = PN.isFreeChoice();
      if (!nonFC.empty()) {
      	res = RES_NOT_FREECHOICE;
      	string witnessString = "";
      	for (set<Node*>::iterator n=nonFC.begin(); n!=nonFC.end(); n++) {
      		witnessString += " '"+(*n)->nodeFullName()+"'";
      	}
      	trace(TRACE_WARNINGS, " [WARNING] process is not free-choice: transitions"+witnessString+" are not free-choice.\n"); 
      }
      
      trace(TRACE_DEBUG, "-> soundness analysis: creating alpha places\n");
    	// create proper initial marking
    	bom->soundness_initialPlaces(&PN);
    	
    	if (analysis[A_DEADLOCKS]) {
        // create proper livelocks on the net to check for deadlocks
        trace(TRACE_DEBUG, "-> soundness analysis: creating omega places for deadlock analysis\n");
    		bom->soundness_terminalPlaces(&PN, true, analysis[A_STOP_NODES]);
    	}
    	else
    	{
    		// close net without adding livelocks
        trace(TRACE_DEBUG, "-> soundness analysis: creating omega places for general soundness analysis\n");
    		bom->soundness_terminalPlaces(&PN, true, analysis[A_STOP_NODES]);
    	}
  	
    	// check net structure: alpha to omega?
      trace(TRACE_DEBUG, "-> checking whether each node is on a path from alpha to omega\n");
      Node* ll_node = NULL;
      if (res == RES_OK) ll_node = PN.isPathCovered();
      if (ll_node != NULL){
      	res = RES_NO_WF_STRUCTURE;
      	trace(TRACE_WARNINGS, " [WARNING] process has no workflow-structure: node " + ll_node->nodeFullName() + " is not on alpha-omega path\n.");
      }
      
      if (res != RES_OK) {
      	// the net either is not a free-choice net or has no WF-structure
      	// cannot use deadlock analysis to decide soundness
      	analysis[A_DEADLOCKS] = false;
      }
    }
    
	// jump into Thomas Heidinger's part
	if (reduction_level > 0) {
		// structural reduction preserves livelocks and deadlocks
		//if (analysis[A_DEADLOCKS] || analysis[A_LIVELOCKS])
		{
    		trace(TRACE_INFORMATION, "-> Structurally simplifying Petri Net ...\n");
    		PN.reduce(globals::reduction_level);
		}
		
	    // check whether net reduction created an empty net 
		if (PN.getInternalPlaces().size() == 0) {
			cerr << process->name << " retrying (reason: empty process)." << endl;
			cerr << PN.information() << endl;
			return RES_EMPTY_PROCESS;
		}
	}
	
	// finished Petri net creation and manipulation, generate analysis files and scripts
  if (analysis[A_SOUNDNESS] && !analysis[A_DEADLOCKS]) {
    // generate formula that specifies the final state
    /*
    if (analysis[A_STOP_NODES])
     finalStateFormula = bom->createFinalStatePredicate(&PN);
    else*/ {
      trace(TRACE_DEBUG, "-> soundness analysis: creating final state formula\n");
    	finalStateFormula = bom->createOmegaPredicate(&PN, false);
    }
  }
  
  if (analysis[A_SAFE]) {
    safeStateFormula = bom->createSafeStatePredicate(&PN);
    if (safeStateFormula == NULL || safeStateFormula->size() == 0)
      // no formula needed to check safeness, net is safe
      analysis[A_SAFE] = false;
  }
    
  // fix names of places such that they can be read by 
  // an owfn/lola parser after output
  //PN.fixPlaceNames();

  cerr << process->name << "." << endl;

  // extend output file name suffix if necessary
  // if we work with the unreduced net, extend the file name 
  if (reduction_level < globals::reduction_level) {
  	globals::output_filename_suffix += ".unreduced";
  }

  // generate the output for the given process
  globals::currentProcessName = process->name;	// for comments about origin in generated net files
  write_net_file(analysis);		// write out current process and further related files
  write_task_file(analysis);		// write task file for this process (if applciable)
  extend_script_file(analysis);	// extend script file for this process (if applicable)
  
  if (finalStateFormula != NULL) {
  	delete finalStateFormula;	// TODO clean up subformulas!
  	finalStateFormula = NULL;
  }
  if (safeStateFormula != NULL) {
    delete safeStateFormula;  // TODO clean up subformulas!
    safeStateFormula = NULL;
  }
  
  return res;
}

translationResult_t translate_process_reconcile (Block *process, analysis_t analysis)
{
	translationResult_t result = translate_process(process, analysis, globals::reduction_level);
	switch (result)
	{
		case RES_EMPTY_PROCESS:
			return translate_process(process, analysis, 0);
		default:
			break;
	}
	return result;
}

/******************************************************************************
 * main() function
 *****************************************************************************/

/*!
 * \brief entry point of UML2OWFN
 *
 * Controls the behaviour of input and output.
 *
 * \param argc	number of command line arguments
 * \param argv	array with command line arguments
 *
 * \returns 0 if everything went well
 * \returns 1 if an error occurred
 *
 * \todo The opening and closing of files is awkward.
 *
 * \bug  Piping does not always work...
 */
int main( int argc, char *argv[])
{


  // analyzation of the commandline
  analyze_cl(argc,argv);


  // parsing all inputfiles
  set< string >::iterator file = inputfiles.begin();
  do
  {
    if (file != inputfiles.end())
    {
      open_file(*file);

      // reset the parser
      frontend_restart(frontend_in);      
    } else {
        return 0;
    }

    // invoke BPEL Bison parser
    trace(TRACE_ALWAYS, "Parsing " + globals::filename + " ...\n");
    int parse_result = frontend_parse();
    trace(TRACE_INFORMATION, "Parsing of " + globals::filename + " complete.\n");
    
    
    if (file != inputfiles.end())
      close_file(*file);
    
    if (frontend_nerrs == 0)
    {
    	//log_println("NET;CORRECT SYNTAX;SYNTAX ERROR");
	    int processTranslated = 0; int processNum = 0;
	    for(set<Block*>::iterator process = globals::processes.begin(); process != globals::processes.end(); process++)
	    {
	    	processNum++;
    		trace(TRACE_DEBUG, "\ntranslate process " + toString(processNum) + "\n");
	        // set the name variables of all blocks within this process to the parsed
	    	// values
    		trace(TRACE_DEBUG, "-> resolving references and names, creating nodes of the process\n");
	    	(*process)->transferName();
    		trace(TRACE_DEBUG, "-> resolved\n");
    		
    		// set suffix for this process, so we have an output file name
    	    globals::output_filename_suffix =  "." + process_name_to_file_name((*process)->name);
    	    // write output file name of this process to the log
    	    log_print(getOutputFilename_net(getOutputDefaultFormat_net()));
    	    
	    	//if the process was filtered, dont create an output at all
	    	if ((*process)->syntaxError) {
	    		trace(TRACE_WARNINGS, "translation error: skipping process " + (*process)->name + "\n");
	    		log_println(";false;translation error");
	    		continue;
	    	}
	    	
	    	if ((*process)->isComplexEmpty()) {
	    		trace(TRACE_WARNINGS, "[ERROR] process has no contents, skipping process " + (*process)->name + "\n");
	    		log_println(";false;empty process");
	    		continue;
	    	}
	        
	    	// link the information of processes, tasks and services that are referenced
	    	// by "callto" blocks into the callto blocks and then tests if there
	    	// are overlapping input and/or output sets
    		trace(TRACE_DEBUG, "-> creating links of the process\n");
	    	(*process)->linkInserts();
	    	trace(TRACE_DEBUG, "-> created\n");

	    	//if the process was filtered, dont create an output at all
	    	if (globals::parameters[P_FILTER] && (*process)->filtered) {
	    		trace(TRACE_WARNINGS, "filtering on: skipping process " + (*process)->name + "\n");
	    		log_println(";false;overlapping");
	    		continue;
	    	}
	    	
	    	// check whether all connections can find their starting and ending pins
    		trace(TRACE_DEBUG, "-> resolving links between nodes\n");
	      (*process)->linkNodes();
    		trace(TRACE_DEBUG, "-> resolved\n");
	        
	    	if (globals::parameters[P_FILTER] && (*process)->filtered)
	    	{
	    		trace(TRACE_WARNINGS, "filtering on: skipping process " + (*process)->name + " because of wrong pin multiplicities\n");
	    		log_println(";false;multiplicities");
	    		continue;
	    	}
	
	        // DECOMPOSITION
	        // Decomposition is currently not in use, due to
	        // a missing algorithm that would allow to turn a
	        // structurally decomposed net into a meaningful process
	
	        // Testroles for cutting
	        // set<string> testStrings;
	        // testStrings.insert("Hmn32Rls##Customer");        
	
	        // Functions to spread roles, cut the net and
	        // turn it into a working process (incomplete)
	        // (*process)->adjustRoles();
	        // (*process)->cutNet(testStrings, 2);
	        // (*process)->disableStart();        
	        // (*process)->disableEnd();
	    	
	    	translationResult_t res;
	    	/*
 	    	if (globals::analysis[A_DEADLOCKS] && globals::analysis[A_LIVELOCKS]) {
	    		// cannot check for deadlock and livelocks at once,
	    		// create two dedicated nets
 	    		globals::multi_analysis = true; // start multi analysis
 	    		
	    		analysis_t sub_analysis;
	    		
	    		sub_analysis = globals::analysis; sub_analysis[A_LIVELOCKS] = false;
	    		res = translate_process_reconcile(*process, sub_analysis);

	    		if (res == RES_OK) {
	    			sub_analysis = globals::analysis; sub_analysis[A_DEADLOCKS] = false;
	    			res = translate_process_reconcile(*process, sub_analysis);
	    		}
	    		
	    		globals::multi_analysis = false; // done
	    		
	    	} else*/ {
	    		res = translate_process_reconcile(*process, globals::analysis);
	    	}
 	    	
 	    	// evaluate translation result for this process
 	    	switch (res) {
 	    		case RES_OK:
 	 	    		processTranslated++;
 	 	    		log_println(";true;---");
 	 	    		break;
 	    		case RES_EMPTY_PROCESS:
 	    			log_println(";false;empty process");
 	    			break;
 	    		case RES_NOT_FREECHOICE:
 	    		  processTranslated++;
 	    			log_println(";true;not free-choice");
 	    			break;
 	    		case RES_NO_WF_STRUCTURE:
 	    		  processTranslated++;
 	    			log_println(";true;no workflow structure");
 	    			break;
 	    	}

	    } // for all processes
	    write_script_file();		// write script file for the entire process library
	    scriptContents.flush();
	    write_log_file();			// write translation log for the entire library
	    logContents.flush();
    
	    if (processTranslated > 1)
	    	trace(TRACE_ALWAYS, toString(processTranslated)+" of "+toString(processNum)+" processes have been translated.\n");
	    else
	    	trace(TRACE_ALWAYS, toString(processTranslated)+" of "+toString(processNum)+" processes has been translated.\n");
    } else {
    	trace(TRACE_ERROR, "No processes translated, "+toString(frontend_nerrs)+" parse error(s).\n");
    }

    // clear all variables and prepare for input of the next xml file
    globals::processes.clear();      
    globals::tasks.clear();      
    globals::services.clear();      
	
	file++;
  } while (file != inputfiles.end());

  trace(TRACE_INFORMATION, "All files have been parsed.\n");

  // everything went fine
  return 0;
}

/*!
 * \defgroup frontend Front End
 * \defgroup patterns Petri Net Patterns
 */
