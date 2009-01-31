/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2007, 2008  Dirk Fahland <dirk.fahland@service-technolog.org>,
                            Martin Znamirowski <znamirow@informatik.hu-berlin.de>

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
 * \file    options.cc
 *
 * \brief   evaluation of command-line options
 *
 * \author  Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: znamirow $
 *
 * \since   2008/05/07
 *
 * \date    \$Date: 2008/05/09 12:38:16 $
 *
 * \note    This file is part of the tool UML2OWFN and was created at the
 *          Humboldt-Universität zu Berlin. See
 *
 * \version \$Revision: 1.00 $
 */




/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <map>
#include <utility>

#include <libgen.h>     // for ::basename

#include "uml2owfn.h"
#include "uml2owfn-io.h"
#include "options.h"
#include "helpers.h"
#include "debug.h"
#include "getopt.h"	// for radon
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

/// long options (needed by GNU getopt)
static struct option longopts[] =
{
  { "help",		no_argument,       NULL, 'h' },
  { "version",		no_argument,       NULL, 'v' },
  { "supressfilter",	no_argument      , NULL, 's' },
  { "rolecut",	        no_argument      , NULL, 'R' },
  { "input",		required_argument, NULL, 'i' },
  { "output",		optional_argument, NULL, 'o' },
  { "format",		required_argument, NULL, 'f' },
  { "parameter",	required_argument, NULL, 'p' },
  { "debug",		required_argument, NULL, 'd' },
  { "reduce",		required_argument, NULL, 'r' },
  { "roleexclusive",	required_argument, NULL, 'e' },
  { "rolecontains",	required_argument, NULL, 'c' },
  NULL
};

/// short options (needed by GNU getopt)
const char *par_string = "hvsRi:o::f:p:d:r:e:c:a:";


/******************************************************************************
 * Functions for command line evaluation
 *****************************************************************************/

/*!
 * \brief prints an overview of all commandline arguments
 */
void print_help()
{
	// 80 chars
	//    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
	trace(string(PACKAGE_NAME) + " -- Translating BPEL Processes into Petri Net Models\n");
	trace("\n");
	trace("Usage: " + globals::program_name + " [OPTION]\n");
	trace("\n");
	trace("Options:\n");
	trace(" -p, --parameter=PARAM     modify processing with given parameter\n");
	trace(" -i, --input=FILE          read a BPEL process from FILE\n");
	trace(" -o, --output[=NAME]       write output to file (NAME sets filename)\n");
	trace(" -f, --format=FORMAT       create output of the given format\n");
	trace(" -d, --debug=NUMBER        set a debug level (NUMBER=0..4 or \"flex\" or \"bison\")\n");
	trace(" -r, --reduce=NUMBER       apply structural reduction level (NUMBER=0..5)\n");
	trace(" -R, --rolecut             cuts away all swimlanes in a process that\n");
	trace("                           do not contain a startnode\n");
	trace(" -e, --roleexclusive=ROLE  cuts away all swimlanes in a process that\n");
	trace("                           do not contain a startnode or don't have\n");
	trace("                           exactly the exclusive role\n");
	trace(" -c, --rolecontains=ROLE   cuts away all swimlanes in a process that\n");
	trace("                           do not contain a startnode or are one of the\n");
	trace("                           contained roles\n");
	trace(" -h, --help                print this help list and exit\n");
	trace(" -v, --version             print program version and exit\n");
	trace("\n");
  trace("  PARAMETER is one of the following (multiple parameters permitted):\n");
  trace("    filter              filter out infeasible processes from the library\n");
  trace("    keeppins            keep unconnected pins\n");
  trace("    log                 write a log file for the translation\n");
  trace("    taskfile            write analysis task to a separate file\n");
  trace("    anon                anonymize the process output\n");
  trace("    ctl                 generate CTL model checking properties for analysis\n");
  trace("                        (if applicable)");
	trace("\n");
	trace("  FORMAT is one of the following (multiple formats permitted):\n");
  trace("    lola, owfn, dot, pep, tpn, apnn, ina, spin, pnml, info\n");
	trace("\n");
  trace("  TASK is one of the following (multiple parameters permitted):\n");
  trace("    soundness           analyze for soundness\n");
  trace("    deadlocks           check for deadlocks (except in the final state),\n");
  trace("                        requires -a soundness\n");
  trace("    safe                analyze for safeness\n");
  trace("    stop                distinguish stop nodes from end nodes\n");
  trace("\n");
  trace("  the following TASK parameters determine the process termination semantics\n");
  trace("  that is used for the analysis, they are used mutually exclusive, all\n");
  trace("  parameters require '-a soundness'\n");
  trace("    noData              ignore state of data flow upon process termination,\n");
  trace("    wfNet               attempt to translate net into a workflow net,\n");
  trace("    orJoin              analyze net by assuming an implicit OR-join,\n");
  trace("\n");
	trace("Examples:\n");
	trace("  uml2owfn -i library.xml -f owfn -o\n");
	trace("  uml2owfn -i library.xml -f dot -p soundness -o\n");
	trace("\n");
	//trace("Report bugs to <" + string(PACKAGE_BUGREPORT) + ">.\n");
}


/*!
 * \brief prints version information
 */
void print_version()
{
	// 80 chars
	//    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
	trace(string(PACKAGE_STRING) + " (compiled " + string(__DATE__) + ")\n\n");
	trace("Copyright (C) 2007, 2008 Martin Znamirowski, Dirk Fahland\n");
}





/*!
 * \brief parses the command line using GNU getopt
 *
 * \param argc argument counter from #main
 * \param argv argument array from #main
 */
void parse_command_line(int argc, char* argv[])
{
	// suffixes stores the file suffixes for the various supported file types
	suffixes[F_LOLA] = "lola";
	suffixes[F_OWFN] = "owfn";
	suffixes[F_DOT]  = "dot";
	suffixes[F_TPN]  = "tpn";
	suffixes[F_INA]  = "pnt";
	suffixes[F_SPIN]  = "spin";
	suffixes[F_PEP]  = "ll_net";
	suffixes[F_APNN] = "apnn"  ;
	suffixes[F_INFO] = "info"  ;
	suffixes[F_PNML] = "pnml"  ;

	// the programme's name on the commandline
	string progname = string(argv[0]);

	// turn off debug modi of flex and bison by default
	frontend_debug = 0;
	frontend__flex_debug = 0;


	// use GNU getopt to parse the command-line arguments
	int optc = 0;
	while ((optc = getopt_long (argc, argv, par_string, longopts, static_cast<int *>(0)))
			!= EOF)
	{
		/// \todo call one of them argument and remove the rest
		string parameter = "";
		switch (optc)
		{
		case 'h': options[O_HELP] = true; break;

		case 'v': options[O_VERSION] = true; break;

		case 'i':
		{
			options[O_INPUT] = true;
			globals::filename = string(optarg);
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
			break;
		}

		case 'o':
		{
			if (options[O_OUTPUT]) {
				trace(TRACE_ALWAYS, "Multiple output options are given, please choose only one!\n");
				exit(EXIT_FAILURE);
			}

      options[O_OUTPUT] = true;

      if (optarg != NULL) {
        globals::output_filename = string(optarg);
        trace(TRACE_INFORMATION, "output file name is: "+string(optarg)+"\n");
      } else
        trace(TRACE_INFORMATION, "no output file name given\n");

      break;
		}

		case 'f':
		{
			options[O_FORMAT] = true;
			parameter = string(optarg);

			if (parameter == suffixes[F_LOLA])
				formats[F_LOLA] = true;
			else if (parameter == suffixes[F_OWFN])
				formats[F_OWFN] = true;
			else if (parameter == suffixes[F_DOT])
				formats[F_DOT] = true;
			else if (parameter == "pep")
				formats[F_PEP] = true;
			else if (parameter == suffixes[F_APNN])
				formats[F_APNN] = true;
      else if (parameter == suffixes[F_TPN])
        formats[F_TPN] = true;
			else if (parameter == suffixes[F_INFO])
				formats[F_INFO] = true;
			else if (parameter == "ina")
				formats[F_INA] = true;
			else if (parameter == "spin")
				formats[F_SPIN] = true;
			else if (parameter == suffixes[F_PNML])
				formats[F_PNML] = true;
			else {
				trace(TRACE_ALWAYS, "Unknown format \"" + parameter +"\".\n");
				trace(TRACE_ALWAYS, "Use -h to get a list of valid formats.\n");
				exit(EXIT_FAILURE);
			}

			break;
		}

		case 'p':
		{
			options[O_PARAMETER] = true;
			parameter = string(optarg);

	    if (parameter == "filter")
	      globals::parameters[P_FILTER] = true;
      else if (parameter == "taskfile")
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
				exit(EXIT_FAILURE);
			}

			break;
		}

    case 'a':
    {
      options[O_ANALYSIS] = true;
      parameter = string(optarg);

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
        exit(1);
      }

      break;
    }

		case 'd':
		{
			options[O_DEBUG] = true;
			parameter = string(optarg);

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
				exit(EXIT_FAILURE);
			}
			break;
		}

		case 'r':
		{
			globals::reduction_level = toUInt(string(optarg));
			if (globals::reduction_level == UINT_MAX)
			{
				trace(TRACE_ALWAYS, "Unrecognised reduction mode: \"" + string(optarg) +"\"!\n");
				trace(TRACE_ALWAYS, "Define a number between 0 and 6.\n");
				exit(EXIT_FAILURE);
			}

			break;
		}

		case 'R':
		{
      options[O_ROLECUT] = true;
			break;
		}

		case 'e':
		{
      options[O_ROLECUT] = true;
			parameter = string(optarg);
      globals::exclusiveRoles.insert(string(parameter));
			break;
		}

		case 'c':
		{
      options[O_ROLECUT] = true;
			parameter = string(optarg);
      globals::keepRoles.insert(string(parameter));
			break;
		}

		default:
		{
			trace("Unknown option!\n");
			trace(TRACE_ALWAYS, "Use -h to get a list of valid options.\n");
			exit(EXIT_FAILURE);
			break;
		}
		}
	}

	for ( ; optind < argc; ++optind)
	{
		options[O_INPUT] = true;
		globals::filename = string(argv[optind]);
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

	// print help and exit
	if (options[O_HELP])
	{
		print_help();
		exit(EXIT_SUCCESS);
	}


	// print version and exit
	if (options[O_VERSION])
	{
		print_version();
		exit(EXIT_SUCCESS);
	}


	// check whether all input files are available
	if (options[O_INPUT])
	{
	  for (set< string >::const_iterator file = inputfiles.begin(); file != inputfiles.end(); file++) {
	    FILE * fin = NULL;
	    if (!(fin = fopen(file->c_str(), "r")))
	    {
	      trace(TRACE_ALWAYS, "File '" + *file + "' not found.\n");
	      exit(EXIT_FAILURE);
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
      exit(EXIT_FAILURE);
    }

  }

  if (options[O_PARAMETER]) {
    if (globals::parameters[P_FILTER])
      trace(TRACE_INFORMATION, "filter processes for syntactical compliance\n");
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
 * \post out file addressed by output file stream #file closed
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
