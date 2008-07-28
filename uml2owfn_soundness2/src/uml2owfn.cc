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
PetriNet PN = PetriNet();
/// formula specifying the final state
FormulaState* finalStateFormula;
bool taskFileWritten;
/// string holding the contents of a script
stringstream scriptContents;

/******************************************************************************
 * program parts
 *****************************************************************************/

string generate_task_file_contents (analysis_t analysis);
bool write_task_to_file (analysis_t analysis);

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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_OWFN]);
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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_LOLA]);
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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_PNML]);
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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_PEP]);
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
            output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_TPN]);
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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_INA]);
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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_SPIN]);
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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_APNN]);
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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_DOT]);
	  }
	  trace(TRACE_INFORMATION, "-> Printing Petri net for dot ...\n");
	  PN.set_format(FORMAT_DOT, true);
	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;

#ifdef HAVE_DOT
		  string systemcall = "dot -q -Tpng -o" + globals::output_filename + globals::output_filename_suffix + ".png " + globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_DOT];
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
		  output = openOutput(globals::output_filename + globals::output_filename_suffix + "." + suffixes[F_INFO]);
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

string generate_task_file_contents (analysis_t analysis)
{
	stringstream taskContents;
	
	if ( formats[F_LOLA] ) {
		if (analysis[A_SOUNDNESS]) {
			if (!analysis[A_DEADLOCKS] && !analysis[A_LIVELOCKS]) {
				// formula that specifies final state
				// create an AG EF model checking formula and write the .task-file
				FormulaState* ctl_AG = new FormulaState(LOGQ_CTL_ALLPATH_ALWAYS);
				FormulaState* ctl_EF = new FormulaState(LOGQ_CTL_EXPATH_EVENTUALLY);
				ctl_EF->subFormulas.insert(finalStateFormula);
				ctl_AG->subFormulas.insert(ctl_EF);
			    ctl_AG->set_format(FORMAT_LOLA_FORMULA);
			    taskContents << (*ctl_AG) << endl;
			}
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
	
    trace(TRACE_DEBUG, "-> writing analysis task to file\n");
	taskFileWritten = false;	// in this run, we did not write a task-file yet 
	
	if ( formats[F_LOLA] ) {
		if (analysis[A_SOUNDNESS]) {
			if (!analysis[A_DEADLOCKS] && !analysis[A_LIVELOCKS]) {
				// currently, we distinguish stop-nodes from end-nodes 
				// by a state-predicate, written to a .task-file 
				if (globals::output_filename != "")
				{
					output = openOutput(globals::output_filename + globals::output_filename_suffix + ".task");
				}
				  
			    (*output) << generate_task_file_contents(analysis) << endl << endl;
			    
				if (globals::output_filename != "") {
					closeOutput(output);
					output = NULL;
					taskFileWritten = true;
				}
			}
		}
	}
}

/*
 * \brief distinguish the cases where a task file has to be written to
 *        the disk rather than to be appended to the net file
 */
bool write_task_to_file (analysis_t analysis) {
	if (formats[F_LOLA] && analysis[A_SOUNDNESS] && !analysis[A_DEADLOCKS] && !analysis[A_LIVELOCKS])
	{	 
		return globals::parameters[P_TASKFILE];	// append task to net file
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

/*!
 * \brief	generates and writes a script file (if applicable) for the current
 * 			call, e.g. a script file to invoke lola for soudness checking
 * 
 * \pre		write_net_file() and write_script_file() have already been called
 * 			for the current output
 */
void extend_script_file (analysis_t analysis) {
    trace(TRACE_DEBUG, "-> extending script file\n");
	if ( formats[F_LOLA] ) {
		if ( analysis[A_SOUNDNESS] ) {
			
			string analysis_text;
			string lolaCommand;
			if (analysis[A_DEADLOCKS]) {
				analysis_text = " for deadlocks";
				lolaCommand = "lola_deadlock";
			} else if (analysis[A_LIVELOCKS]) {
				analysis_text = " for livelocks";
				lolaCommand = "lola_deadlock";
			} else {
				analysis_text = " for soundness";
				lolaCommand = "lola";
			}
			
			scriptContents << "echo ----------------------------------------------------------------" << endl;
			scriptContents << "echo  checking " << globals::output_filename + globals::output_filename_suffix << analysis_text << endl;
			scriptContents << "echo ----------------------------------------------------------------" << endl;
			

			
			if (taskFileWritten)
			{
				scriptContents << "${1}" << lolaCommand << " "
					<< globals::output_filename << globals::output_filename_suffix << "." << suffixes[F_LOLA] 
					<< " -a " << globals::output_filename + globals::output_filename_suffix << ".task"
					<< " -P" 
					<< endl;
			}
			else
			{
				scriptContents << "${1}" << lolaCommand << " "
						<< globals::output_filename << globals::output_filename_suffix << "." << suffixes[F_LOLA] 
						<< " -P" 
						<< endl;
			}
			scriptContents << endl;
		}
	}
}

/*!
 * \brief	translate the process according to the given analysis flags,
 * 			calls #write_net_file(), #write_task_file(), #extend_script_file()
 * 			accordingly
 */
bool translate_process(Block *process, analysis_t analysis, int reduction_level)
{
	trace(TRACE_DEBUG, "generating Petri net for " + process->name + "\n");	    	

    // Generates a petri net from the current process
    //PetriNet* tryPN = new PetriNet();
    BomProcess *bom = new BomProcess();
    PN = PetriNet();	// get a new net
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
    }
    
    // extend net for soundness checking
    if (analysis[A_SOUNDNESS]) {
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
    }
    
	// jump into Thomas Heidinger's part
	if (reduction_level > 0)
	{
		// structural reduction preserves livelocks and deadlocks
		//if (analysis[A_DEADLOCKS] || analysis[A_LIVELOCKS])
		{
    		trace(TRACE_INFORMATION, "-> Structurally simplifying Petri Net ...\n");
    		PN.reduce(globals::reduction_level);
		}
	}
    
	if (PN.getInternalPlaces().size() == 0) {
		cerr << process->name << " retrying (reason: empty process)." << endl;
		cerr << PN.information() << endl;
		return false;
	}
	
	// finished Petri net creation and manipulation, generate analysis files and scripts
    if (analysis[A_SOUNDNESS]) {
    	// generate formula that specifies the final state
    	/*
    	if (analysis[A_STOP_NODES])
    		finalStateFormula = bom->createFinalStatePredicate(&PN);
    	else*/ {
        	trace(TRACE_DEBUG, "-> soundness analysis: creating final statea formula\n");
    		finalStateFormula = bom->createOmegaPredicate(&PN, false);
    	}
    }
    
    // fix names of places such that they can be read by 
    // an owfn/lola parser after output
    //PN.fixPlaceNames();

    cerr << process->name << "." << endl;
    
    string analysis_suffix = "";
    if (analysis[A_DEADLOCKS])
    	analysis_suffix += "_dl";
    if (analysis[A_LIVELOCKS])
        analysis_suffix += "_ll";
    
    // generate the output for the given process
    globals::output_filename_suffix = "." 
    								+ process_name_to_file_name(process->name)
    								+ analysis_suffix;
    // if we work with the unreduced net, extend the file name 
    if (reduction_level < globals::reduction_level) {
    	globals::output_filename_suffix += ".unreduced";
    }
    
    globals::currentProcessName = process->name;
    
    
    write_net_file(analysis);		// write out current process and furthe related files
    write_task_file(analysis);		// write task file for this process (if applciable)
    extend_script_file(analysis);	// extend script file for this process (if applicable)
    
    if (finalStateFormula != NULL)
    	delete finalStateFormula;	// TODO clean up subformulas!
    
    return true;
}

bool translate_process_reconcile (Block *process, analysis_t analysis)
{
	if (!translate_process(process, analysis, globals::reduction_level)) {
		return translate_process(process, analysis, 0);
	}
	return true;
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
	    int processTranslated = 0; int processNum = 0;
	    for(set<Block*>::iterator process = globals::processes.begin(); process != globals::processes.end(); process++)
	    {
	    	bool processWritten = true;
	    	
	    	processNum++;
    		trace(TRACE_DEBUG, "translate process " + toString(processNum) + "\n");
	        // set the name variables of all blocks within this process to the parsed
	    	// values
    		trace(TRACE_DEBUG, "-> resolving references and names, creating nodes of the process\n");
	    	(*process)->transferName();
    		trace(TRACE_DEBUG, "-> resolved\n");
	        
	    	// link the information of processes, tasks and services that are referenced
	    	// by "callto" blocks into the callto blocks and then tests if there
	    	// are overlapping input and/or output sets
    		trace(TRACE_DEBUG, "-> creating links of the process\n");
	    	(*process)->linkInserts();
	    	trace(TRACE_DEBUG, "-> created\n");

	    	
	    	//if the process was filtered, dont create an output at all
	    	if (globals::parameters[P_FILTER] && (*process)->filtered)
	    	{
	    		trace(TRACE_WARNINGS, "filtering on: skipping process " + (*process)->name + "\n");
	    		continue;
	    	}
	    	
	    	// check whether all connections can find their starting and ending pins
    		trace(TRACE_DEBUG, "-> resolving links between nodes\n");
	        (*process)->linkNodes();
    		trace(TRACE_DEBUG, "-> resolved\n");
	        
	    	if (globals::parameters[P_FILTER] && (*process)->filtered)
	    	{
	    		trace(TRACE_WARNINGS, "filtering on: skipping process " + (*process)->name + " because of wrong pin multiplicities\n");
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
	        
 	    	if (globals::analysis[A_DEADLOCKS] && globals::analysis[A_LIVELOCKS]) {
	    		// cannot check for deadlock and livelocks at once,
	    		// create two dedicated nets
	    		analysis_t sub_analysis;
	    		
	    		sub_analysis = globals::analysis; sub_analysis[A_LIVELOCKS] = false;
	    		processWritten &= translate_process_reconcile(*process, sub_analysis);
	    		sub_analysis = globals::analysis; sub_analysis[A_DEADLOCKS] = false;
	    		processWritten &= translate_process_reconcile(*process, sub_analysis);
	    		
	    	} else {
	    		processWritten &= translate_process_reconcile(*process, globals::analysis);
	    	}

 	    	if (processWritten)
 	    		processTranslated++;
	    } // for all processes
	    write_script_file();				// write script file for the entire process library
	    scriptContents.flush();
    
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
