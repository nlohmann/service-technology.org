/*****************************************************************************\
 UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets

 Copyright (C) 2007, 2008, 2009  Dirk Fahland and Martin Znamirowski

 UML2oWFN is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 UML2oWFN is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with UML2oWFN.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/*!
 * command-line options handling
 */

/******************************************************************************
 * Headers
 *****************************************************************************/

#include <config.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <map>
#include <utility>

#include <libgen.h>     // for ::basename

#include "uml2owfn-io.h"
#include "options.h"
#include "cmdline.h"
#include "helpers.h"
#include "debug.h"
#include "verbose.h"
#include "globals.h"

using std::cin;
using std::cout;
using std::clog;
using std::cerr;
using std::flush;
using std::pair;
using std::ofstream;


/******************************************************************************
 * Data structures
 *****************************************************************************/

/// list of input files
set<string> inputfiles;

/// pointer to input stream
istream *input = &cin;

/// pointer to output stream
ostream *output = &cout;

/// true if output is created
bool createOutputFile = false;

/// options (set by #parse_command_line)
map<possibleOptions, bool> options;

/// output file formats (set by #parse_command_line)
map<possibleFormats, bool> formats;

/// suffixes are defined in parse_command_line();
map<possibleFormats, string> suffixes;

/// the command line parameters
gengetopt_args_info args_info;


/******************************************************************************
 * Functions for command line evaluation
 *****************************************************************************/

/*!
 * \brief parses the command line using GNU gengetopt
 *
 * \param argc argument counter from #main
 * \param argv argument array from #main
 */
void parse_command_line(int argc, char* argv[])
{

	// initialize the parameters structure
  	struct cmdline_parser_params *params = cmdline_parser_params_create();

	// call the cmdline parser
  	cmdline_parser(argc, argv, &args_info);

	// suffixes stores the file suffixes for the various supported file types
	suffixes[F_LOLA] = "lola";
	suffixes[F_OWFN] = "owfn";
	suffixes[F_DOT]  = "dot";
	suffixes[F_TPN]  = "tpn";

	// the programme's name on the commandline
	string progname = string(argv[0]);

	// turn off debug modi of flex and bison by default
	frontend_debug = 0;
	frontend__flex_debug = 0;

	string parameter = "";
	
	if(args_info.input_given)
	{
		options[O_INPUT] = true;
		globals::filename = args_info.input_arg;
		if (inputfiles.find(globals::filename) == inputfiles.end())
		{
			inputfiles.insert(globals::filename);
		}
		else
		{
			trace(TRACE_WARNINGS, "WARNING: It makes no sense reading \n");
			trace(TRACE_WARNINGS, "             "+ globals::filename + "\n");
			trace(TRACE_WARNINGS, "         more than once.\n");
			trace(TRACE_WARNINGS, "         So file will only be read once.\n");
		}	

	}
	
	if(args_info.output_given)
	{	if (options[O_OUTPUT]) {
			abort(4, "Multiple output options are given, please choose only one!");
		}

    	        options[O_OUTPUT] = true;

      		if (args_info.output_arg != NULL) {
        		globals::output_filename = args_info.output_arg;
        		trace(TRACE_INFORMATION, "output file name is: "+string(args_info.output_arg)+"\n");
      		} else
        		trace(TRACE_INFORMATION, "no output file name given\n");

		}

	if(args_info.format_given)
	{

		options[O_FORMAT] = true;
		parameter = args_info.format_arg;
		if (parameter == suffixes[F_LOLA])
			formats[F_LOLA] = true;
		else if (parameter == suffixes[F_OWFN])
			formats[F_OWFN] = true;
		else if (parameter == suffixes[F_DOT])
			formats[F_DOT] = true;
     		else if (parameter == suffixes[F_TPN])
        		formats[F_TPN] = true;
		else {
			trace(TRACE_ALWAYS, "Unknown format \"" + parameter +"\".\n");
			trace(TRACE_ALWAYS, "Use -h to get a list of valid formats.\n");
			abort(4, "Parameter error");
		}
	
	}

	for (int i = 0; i < args_info.parameter_given; ++i)
	{
			options[O_PARAMETER] = true;
			parameter = args_info.parameter_arg[i];
	    		if (parameter == "filter") {
	      			options[O_SKIP_BY_FILTER] = true;
	      			globals::filterCharacteristics[PC_EMPTY] = true;
	      			//globals::filterCharacteristics[PC_PIN_MULTI_NONMATCH] = true;
	      			globals::filterCharacteristics[PC_OVERLAPPING] = true;
	    		} else if (parameter == "taskfile")
        			globals::parameters[P_TASKFILE] = true;
	    		else if (parameter == "keeppins")
        			globals::parameters[P_KEEP_UNCONN_PINS] = true;
      			else if (parameter == "log")
        			globals::parameters[P_LOG] = true;
      			else if (parameter == "anon")
        			globals::parameters[P_ANONYMIZE] = true;
      			else if (parameter == "ctl")
        			globals::parameters[P_CTL] = true;
			else {
				trace(TRACE_ALWAYS, "Unknown parameter \"" + parameter +"\".\n");
				trace(TRACE_ALWAYS, "Use -h to get a list of valid parameters.\n");
        			abort(4, "Parameter error");
			}

	}
	
	for (int i = 0; i < args_info.skip_given; ++i)
	{
		options[O_SKIP_BY_FILTER] = true;
		parameter = args_info.skip_arg[i];
		if (parameter == "empty")
        		globals::filterCharacteristics[PC_EMPTY] = true;
     		else if (parameter == "multi")
        		globals::filterCharacteristics[PC_PIN_MULTI] = true;
      		else if (parameter == "multiNonMatching")
        		globals::filterCharacteristics[PC_PIN_MULTI_NONMATCH] = true;
      		else if (parameter == "overlappingPins")
        		globals::filterCharacteristics[PC_OVERLAPPING] = true;
     		else if (parameter == "trivialInterface")
        		globals::filterCharacteristics[PC_TRIVIAL_INTERFACE] = true;
      		else {
        		trace(TRACE_ALWAYS, "Unknown filtering process characteristic \"" + parameter +"\".\n");
        		trace(TRACE_ALWAYS, "Use -h to get a list of valid process characteristics.\n");
        		abort(4, "Parameter error");
      		}

	}

	for (int i = 0; i < args_info.analysis_given; ++i)
	{
		 options[O_ANALYSIS] = true;
     		 parameter = args_info.analysis_arg[i];

      		 if (parameter == "soundness")
        	 	globals::analysis[A_SOUNDNESS] = true;
      		else if (parameter == "stop")
        		globals::analysis[A_STOP_NODES] = true;
      		else if (parameter == "deadlocks")
        		globals::analysis[A_DEADLOCKS] = true;
      		else if (parameter == "safe")
        		globals::analysis[A_SAFE] = true;
      		else if (parameter == "noData")
        		globals::analysis[A_TERM_IGNORE_DATA] = true;
      		else if (parameter == "wfNet")
        		globals::analysis[A_TERM_WF_NET] = true;
      		else if (parameter == "orJoin")
       	 		globals::analysis[A_TERM_ORJOIN] = true;
      		else {
        		trace(TRACE_ALWAYS, "Unknown analysis task \"" + parameter +"\".\n");
        		trace(TRACE_ALWAYS, "Use -h to get a list of valid analysis tasks.\n");
        		abort(4, "Parameter error");
      		}

	}

	if(args_info.debug_given)
	{
		options[O_DEBUG] = true;
		parameter = args_info.debug_arg;
		if (parameter == "flex")
			frontend__flex_debug = 1;
		else if (parameter == "bison")
			frontend_debug = 1;
		else if (parameter == "1")
			debug_level = TRACE_WARNINGS;
		else if (parameter == "2")
			debug_level = TRACE_INFORMATION;
		else if (parameter == "3")
			debug_level = TRACE_DEBUG;
		else if (parameter == "4")
			debug_level = TRACE_VERY_DEBUG;
		else if (parameter == "0")
			debug_level = TRACE_ERROR;
		else {
			trace(TRACE_ALWAYS, "Unrecognised debug mode: \"" + parameter +"\"!\n");
			trace(TRACE_ALWAYS, "Use -h to get a list of valid debug modes.\n");
       			abort(4, "Parameter error");
		}
	
	}

	if(args_info.reduce_given)
	{
		globals::reduction_level = toUInt(args_info.reduce_arg);
		if (globals::reduction_level == UINT_MAX)
		{
			trace(TRACE_ALWAYS, "Unrecognised reduction mode: \"" + string(args_info.reduce_arg) +"\"!\n");
			trace(TRACE_ALWAYS, "Define a number between 0 and 6.\n");
        		abort(4, "Parameter error");
		}

	}
	if(args_info.rolecut_given)
		options[O_ROLECUT] = true;

	if(args_info.roleexclusive_given)
	{
	        options[O_ROLECUT] = true;
	        parameter = args_info.roleexclusive_arg;
                globals::exclusiveRoles.insert(string(parameter));
	}

	if(args_info.rolecontains_given)
	{
		options[O_ROLECUT] = true;
		parameter = string(args_info.rolecontains_arg);
      		globals::keepRoles.insert(string(parameter));

	}

	for(int i=0;i<args_info.inputs_num;i++)
	{
		options[O_INPUT] = true;
		globals::filename = string(args_info.inputs[i]);
		if (inputfiles.find(globals::filename) == inputfiles.end())
		{
			inputfiles.insert(globals::filename);
		}
		else
		{
			trace(TRACE_WARNINGS, "WARNING: It makes no sense reading \n");
			trace(TRACE_WARNINGS, "             "+ globals::filename + "\n");
			trace(TRACE_WARNINGS, "         more than once.\n");
			trace(TRACE_WARNINGS, "         So file will only be read once.\n");
		}

	}

	// check whether all input files are available
	if (options[O_INPUT])
	{
	  for (set< string >::const_iterator file = inputfiles.begin(); file != inputfiles.end(); file++) {
	    FILE * fin = NULL;
	    if (!(fin = fopen(file->c_str(), "r")))
	    {
	      abort(5, "File `%s' not found.", file->c_str());
	    }
	    fclose(fin);
	  }
	}

	// set output file names consistent to parameters and input file names
  	if ( options[O_OUTPUT] )
  	{
    	  createOutputFile = true;

    	if (globals::output_filename == "")
        {
      	  // set output file name to a standard output filename in case of no inputfiles
          if ( not(options[O_INPUT]) )
      	  {
            globals::output_filename = "stdof";
            trace(TRACE_ALWAYS, "Output file name set to standard: stdof\n");
          }
          else
          {
           set< string >::iterator file = inputfiles.begin();
           globals::getOutputFileNameFromInput = true;
           trace(TRACE_INFORMATION, "Generating output file name from input file name\n");
          }
        } else {
      	   // separate output file name into path and file name
           char* ffile = (char*)malloc(globals::output_filename.size() + sizeof(char));
           strcpy(ffile, globals::output_filename.c_str());

           if (ffile[globals::output_filename.size()-1] != GetSeparatorChar()) {
           // output file name does not end with a directory-slash -> interpret as filename
           globals::output_directory = string(::dirname(ffile));
           globals::output_filename = string(::basename(ffile));
           } else {
            // output file name does end with a directory-slash -> interpret as directory name for output directory
            globals::output_directory = string(::dirname(ffile))+GetSeparatorChar()+string(::basename(ffile));
            globals::output_filename = "";
            globals::getOutputFileNameFromInput = true;
           }
           free(ffile);
         }
        }

	  if (options[O_INPUT]) {
	    trace(TRACE_INFORMATION, " - input is read from \"" + globals::filename + "\"\n");
	  }
	  if (options[O_OUTPUT]) {
	    string outFileName = (globals::output_filename == "") ? "<in>" : globals::output_filename;
	    trace(TRACE_INFORMATION, " - output files will be named \"" + globals::output_directory+GetSeparatorChar()+outFileName + ".<ext>\"\n");
	  }

	  if (options[O_ROLECUT]) {
	    if (globals::keepRoles.empty() && globals::exclusiveRoles.empty()) {
	      trace(TRACE_INFORMATION, "cutting processes, removing all roles not containing a start node\n");
	      globals::deriveRolesToCut = true;
	    } else {
	      if (!globals::keepRoles.empty())
		trace(TRACE_INFORMATION, "cutting processes, preserving roles by containtment\n");
	      if (!globals::exclusiveRoles.empty())
		trace(TRACE_INFORMATION, "cutting processes, preserving roles by exact match\n");
	    }
	  }

	  if (options[O_ANALYSIS]) {

	    if (globals::analysis[A_SOUNDNESS])
	      trace(TRACE_INFORMATION, "generating nets to analyze soundness\n");
	    if (globals::analysis[A_STOP_NODES]) {
	      trace(TRACE_INFORMATION, "distinguishing stop nodes from end nodes\n");
	      if (!globals::analysis[A_SOUNDNESS]) {
		trace(TRACE_INFORMATION, "  - note: only reasonable if also analyzing soundness\n");
	      }
	    }
	    if (globals::analysis[A_DEADLOCKS])
	      trace(TRACE_INFORMATION, "checking for absence of deadlocks\n");
	    if (globals::analysis[A_SAFE])
	      trace(TRACE_INFORMATION, "checking for safeness\n");
	    if (globals::analysis[A_SAFE] && globals::analysis[A_SOUNDNESS]) {
	      trace(TRACE_INFORMATION, "multiple model-checking analysis tasks: forcing analysis task files\n");
	      options[O_PARAMETER] = true;
	      globals::parameters[P_TASKFILE] = true;
	    }

	    // count how many termination semantics shall be applied
	    int terminationFlags = 0;
	    if (globals::analysis[A_TERM_IGNORE_DATA]) {
	      trace(TRACE_INFORMATION, "removing output pinsets\n");
	      if (!globals::analysis[A_SOUNDNESS]) {
		trace(TRACE_INFORMATION, "  - note: only reasonable if also analyzing soundness\n");
	      }
	      terminationFlags++;
	    }
	    if (globals::analysis[A_TERM_WF_NET]) {
	      trace(TRACE_INFORMATION, "creating a workflow net with a final AND-join\n");
	      if (!globals::analysis[A_SOUNDNESS]) {
		trace(TRACE_INFORMATION, "  - note: only reasonable if also analyzing soundness\n");
	      }
	      terminationFlags++;
	    }
	    if (globals::analysis[A_TERM_ORJOIN]) {
	      trace(TRACE_INFORMATION, "creating a workflow net with a final implicit OR-join\n");
	      if (!globals::analysis[A_SOUNDNESS]) {
		trace(TRACE_INFORMATION, "  - note: only reasonable if also analyzing soundness\n");
	      }
	      terminationFlags++;
	    }

	    if (terminationFlags > 1) {
	      trace(TRACE_ALWAYS, "ERROR: more than one termination semantics selected, please choose at most one.\n");
	      abort(4, "Parameter error");
	    }

	  }

	  if (options[O_SKIP_BY_FILTER]) {
	    if (globals::filterCharacteristics[PC_EMPTY])
	      trace(TRACE_INFORMATION, "skipping empty processes\n");
	    if (globals::filterCharacteristics[PC_PIN_MULTI])
	      trace(TRACE_INFORMATION, "skipping processes with pin multiplicities\n");
	    if (globals::filterCharacteristics[PC_PIN_MULTI_NONMATCH])
	      trace(TRACE_INFORMATION, "skipping processes with non-matching pin multiplicities\n");
	    if (globals::filterCharacteristics[PC_OVERLAPPING])
	      trace(TRACE_INFORMATION, "skipping processes with overlapping pinsets\n");
	    if (globals::filterCharacteristics[PC_TRIVIAL_INTERFACE]) {
	      trace(TRACE_INFORMATION, "skipping processes with trivial interface\n");
	      if (!options[O_ROLECUT])
		trace(TRACE_INFORMATION, "  - note: effective only when cutting rules (option '-R')\n");
	    }
	  }

	  if (options[O_PARAMETER]) {
	    if (globals::parameters[P_TASKFILE])
	      trace(TRACE_INFORMATION, "write analysis task in separate files\n");
	    if (globals::parameters[P_LOG])
	      trace(TRACE_INFORMATION, "write log file\n");
	    if (globals::parameters[P_ANONYMIZE])
	      trace(TRACE_INFORMATION, "anonymize output\n");
	    if (globals::parameters[P_KEEP_UNCONN_PINS])
	      trace(TRACE_INFORMATION, "keep unconnected pins\n");
	  }

}


/*!
 * \brief open output file
 *
 * \param name filename of the output file
 * \return output file stream to the output file
 *
 * \post output file with filename name opened and bound to stream file
 */
ostream *openOutput(string name)
{
	ofstream *file = new ofstream(name.c_str(), ofstream::out | ofstream::trunc | ofstream::binary);

	if (!file->is_open())
		trace(TRACE_ALWAYS, "File \"" + name + "\" could not be opened for writing access!\n");

	return file;
}


/*!
 * \brief close output file
 *
 * \param file file stream of the output file
 *
 * \post out file addressed by output file stream closed
 */
void closeOutput(ostream *file)
{
	if ( file != NULL )
	{
		(*file) << flush;
		(static_cast<ofstream*>(file))->close();
		delete(file);
		file = NULL;
	}
}
