#include <string>
#include <iostream>
#include <map>

#include "bpel2owfn.h"
#include "options.h"
#include "debug.h"

// some file names and pointers

/// Filename of input file
std::string filename = "<STDIN>";
/// Filename of input file
std::string output_filename = "";

/// pointer to input stream
std::istream * input = &std::cin;
/// pointer to output stream
std::ostream * output = &std::cout;
/// pointer to log stream
std::ostream * log_output = &std::clog;

/// Filename of log file
std::string log_filename = "";

bool createOutputFile = false;

// different modes controlled by command line

possibleModi modus;

map<possibleOptions,    bool> options;
map<possibleParameters, bool> parameters;
map<possibleFormats,    bool> formats;
// suffixes are defined in parse_command_line();
map<possibleFormats,  string> suffixes;

// long options
static struct option longopts[] =
{
  { "help",       no_argument,       NULL, 'h' },
  { "version",    no_argument,       NULL, 'v' },
  { "mode",       required_argument, NULL, 'm' },
  { "log",        optional_argument, NULL, 'l' },
  { "input",      required_argument, NULL, 'i' },
  { "inputfile",  required_argument, NULL, 'i' },
  { "output",     optional_argument, NULL, 'o' },
  { "outputfile", optional_argument, NULL, 'o' },
  { "format",     required_argument, NULL, 'f' },
  { "parameter",  required_argument, NULL, 'p' },
  { "debug",      required_argument, NULL, 'd' },
  NULL
};

const char * par_string = "hvm:li:of:p:d:";

// --------------------- functions for command line evaluation ------------------------
/**
 * Prints an overview of all commandline arguments.
 *
 */
void print_help() 
{
  // 80 chars
  //    "--------------------------------------------------------------------------------"
  trace("\n");
  trace(std::string(PACKAGE_STRING) + "\n");
  trace("\n");
  trace("Options: \n");
  trace("\n");
  trace(" -h | --help            - print these information and exit\n");
  trace(" -v | --version         - print version information and exit\n");
  trace("\n");
  trace(" -m | --mode=<modus>    - select one of the following modes:\n");
  trace("                          ast, pretty, petrinet, cfg\n");
  trace(" -p | --parameter=<par> - select additional parameters like:\n");
  trace("                          simplify, lowlevel, nointerface etc.\n");
  trace("                          (see documentation for further information)\n");
  trace("\n");
  trace(" -i | --input=<file>    - read input from <file>\n");
  trace(" -o | --output=<prefix> - write output to <prefix>.X\n");
  trace("\n");
  trace(" -f | --format          - select output formats (as far as supported for mode):\n");
  trace("                          lola, owfn, dot, pep, appn, info, pnml, txt, xml\n");
  trace("                          (note: lola,owfn,pep,appn,pnml imply modus=petrinet\n");
  trace("\n");
  trace(" -d | --debug           - set debug level: 1-4\n");
  trace(" -l | --log[=<file>]    - write additional information into file\n");
  trace("\n");
  
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.informatik.hu-berlin.de/top/tools4bpel\n");
  trace("\n");

}

/**
 * Prints some version information
 *
 * \param name commandline name of the this program
 *
 *
 */

void print_version(std::string name)
{
  trace(std::string(PACKAGE_STRING) + " -- ");
  trace("Translating BPEL Processes to Open Workflow Nets\n");
  trace("\n");
  trace("Copyright (C) 2005, 2006 Niels Lohmann, Christian Gierds and Dennis Reinert\n");
  trace("This is free software; see the source for copying conditions. There is NO\n");
  trace("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}


void parse_command_line(int argc, char* argv[])
{
  suffixes[F_LOLA] = "lola"  ;
  suffixes[F_OWFN] = "owfn"  ;
  suffixes[F_DOT]  = "dot"   ;
  suffixes[F_PEP]  = "ll_net";
  suffixes[F_APPN] = "appn"  ;
  suffixes[F_INFO] = "info"  ;
  suffixes[F_PNML] = "pnml"  ;
  suffixes[F_TXT]  = "txt"   ;
  suffixes[F_XML]  = "xml"   ;

  possibleFormats format[] = { F_LOLA, F_OWFN, F_DOT, F_PEP, F_APPN, F_INFO, F_PNML, F_TXT, F_XML };

  map< pair<possibleModi,possibleFormats>, bool > validFormats;
  
  // validFormats[pair<possibleModi,possibleFormats>(M_AST,F_TXT)] = true;

  validFormats[pair<possibleModi,possibleFormats>(M_PRETTY,F_XML)] = true;

  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_LOLA)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_OWFN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_DOT )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_PEP )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_APPN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_INFO)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_PNML)] = true;
  // validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_TXT )] = true;

  validFormats[pair<possibleModi,possibleFormats>(M_CFG,F_DOT)] = true;

  string progname = string(argv[0]);

  yydebug = 0;
  yy_flex_debug = 0;

  int optc = 0;
  while ((optc = getopt_long (argc, argv, par_string, longopts, (int *) 0))
         != EOF)
  {
    // \todo call one of them argument and remove the rest
    string mode = "";
    string format = "";
    string parameter = "";
    string debug = "";
    possibleModi old_modus;
    switch (optc)
      {
      case 'h':
	      options[O_HELP] = true;
  	      break;
      case 'v':
	      options[O_VERSION] = true;
      	      break;
      case 'm':
	      old_modus = modus;
	      mode = string(optarg);
	      if (mode == "ast") {
		modus = M_AST;
	      }
	      else if (mode == "pretty") {
		modus = M_PRETTY;
	      }
	      else if (mode == "petrinet") {
		modus = M_PETRINET;
	      }
	      else if (mode == "cfg") {
		modus = M_CFG;
	      }
	      if (options[O_MODE] && modus != old_modus)
	      {
		throw Exception(OPTION_MISMATCH, 
				"Choose only one mode\n",
				"Type " + progname + " -h for more information.\n");
	      }
	      options[O_MODE] = true;
	      break;
      case 'l':
	      options[O_LOG] = true;
	      if (optarg != NULL)
	      {
	        log_filename = string(optarg);
	      }
              break;
      case 'i':
	      if (options[O_INPUT])
	      {
		trace(TRACE_WARNINGS, "Multiple input options are given, only last one is used!\n");
	      }
	      options[O_INPUT] = true;
	      filename = string(optarg);
              break;
      case 'o':
	      if (options[O_OUTPUT])
	      {
		trace(TRACE_WARNINGS, "Multiple output options are given, only last given name is used!\n");
	      }
	      options[O_OUTPUT] = true;
	      if (optarg != NULL)
	      {
		output_filename = string(optarg);
	      }
              break;
      case 'f':
	      options[O_FORMAT] = true;
	      format = string(optarg);
	      if (format == suffixes[F_LOLA])
	      {
		formats[F_LOLA] = true;
 	        if (options[O_MODE] && modus != M_PETRINET)
	        {
		  throw Exception(OPTION_MISMATCH, 
				  "Choose only one mode\n",
				  "Type " + progname + " -h for more information.\n");
	        }
	    	modus = M_PETRINET;
	        options[O_MODE] = true;
	      }
	      else if (format == suffixes[F_OWFN])
	      {
		formats[F_OWFN] = true;
 	        if (options[O_MODE] && modus != M_PETRINET)
	        {
		  throw Exception(OPTION_MISMATCH, 
				  "Choose only one mode\n",
				  "Type " + progname + " -h for more information.\n");
	        }
	    	modus = M_PETRINET;
	        options[O_MODE] = true;
	      }
	      else if (format == suffixes[F_DOT])
	      {
		formats[F_DOT] = true;
	      }
	      else if (format == "pep")
	      {
		formats[F_PEP] = true;
 	        if (options[O_MODE] && modus != M_PETRINET)
	        {
		  throw Exception(OPTION_MISMATCH, 
				  "Choose only one mode\n",
				  "Type " + progname + " -h for more information.\n");
	        }
	    	modus = M_PETRINET;
	        options[O_MODE] = true;
	      }
	      else if (format == suffixes[F_APPN])
	      {
		formats[F_APPN] = true;
 	        if (options[O_MODE] && modus != M_PETRINET)
	        {
		  throw Exception(OPTION_MISMATCH, 
				  "Choose only one mode\n",
				  "Type " + progname + " -h for more information.\n");
	        }
	    	modus = M_PETRINET;
	        options[O_MODE] = true;
	      }
	      else if (format == suffixes[F_INFO])
	      {
		formats[F_INFO] = true;
	      }
	      else if (format == suffixes[F_PNML])
	      {
		formats[F_PNML] = true;
 	        if (options[O_MODE] && modus != M_PETRINET)
	        {
		  throw Exception(OPTION_MISMATCH, 
				  "Choose only one mode\n",
				  "Type " + progname + " -h for more information.\n");
	        }
	    	modus = M_PETRINET;
	        options[O_MODE] = true;
	      }
	      else if (format == suffixes[F_TXT])
	      {
		formats[F_TXT] = true;
	      }
	      else if (format == suffixes[F_XML])
	      {
		formats[F_XML] = true;
	      }
	      break;
      case 'p':
	      options[O_PARAMETER] = true;
	      parameter = string(optarg);
	      if ( parameter == "simplify" )
	      {
	        parameters[P_SIMPLIFY] = true;
	      }
	      else if ( parameter == "lowlevel" )
	      {
	        parameters[P_LOWLEVEL] = true;
	      }
	      else if ( parameter == "nointerface" )
	      {
	        parameters[P_NOINTERFACE] = true;
	      }
	      else if ( parameter == "finalmarking" )
	      {
	        parameters[P_FINALMARKING] = true;
	      }
	      else if ( parameter == "uniquefault" )
	      {
	        parameters[P_UNIQUEFAULT] = true;
	      }
	      else if ( parameter == "finishloop" )
	      {
	        parameters[P_FINISHLOOP] = true;
	      }
	      else if ( parameter == "nostandardfaults" )
	      {
		parameters[P_NOSTANDARDFAULTS] = true;
	      }
	      break;
      case 'd':
	      options[O_DEBUG] = true;
	      debug = string(optarg);
	      if ( debug == "flex" )
	      {
		yy_flex_debug = 1;
	      }
	      else if ( debug == "bison" )
	      {
		yydebug = 1;
	      }
	      else if ( debug == "1" )
	      {
		debug_level = TRACE_WARNINGS;
	      }
	      else if ( debug == "2" )
	      {
		debug_level = TRACE_INFORMATION;
	      }
	      else if ( debug == "3" )
	      {
		debug_level = TRACE_DEBUG;
	      }
	      else if ( debug == "4" )
	      {
		debug_level = TRACE_VERY_DEBUG;
	      }
	      else
	      {
		throw Exception(OPTION_MISMATCH, 
				"Unrecognised debug mode!\n",
				"Type " + progname + " -h for more information.\n");
	      }
	      break;
      default:
	     throw Exception(OPTION_MISMATCH,
			     "Unknown option!\n",
			     "Type " + progname + " -h for more information.\n");
             break;
      }
      
  }

  // print help and exit
  if (options[O_HELP])
  {
    print_help();
    exit(0);
  }
  // print version and exit
  if (options[O_VERSION])
  {
    print_version("");
    exit(0);
  }


  // if input file is given, bind it to yyin
  if (options[O_INPUT])
  {
    if (!(yyin = fopen(filename.c_str(), "r")))
    {
      throw Exception(FILE_NOT_FOUND, "File '" + filename + "' not found.\n");
    }
  }

  if ( options[O_OUTPUT] )
  {
    createOutputFile = true;
  }

  // set output file name if non is already chosen
  if ((options[O_OUTPUT] || options[O_LOG]) && (output_filename == ""))
  {
    int pos = filename.rfind(".bpel", filename.length());
    if (pos == (filename.length() - 5))
    {
      output_filename = filename.substr(0, pos);
    }
  }
	  
  if (options[O_LOG])
  {
    if (log_filename == "")
    {
      int pos = filename.rfind(".bpel", filename.length());
      if (pos == (filename.length() - 5))
      {
        log_filename = filename.substr(0, pos) + ".log";
      }
    }
    log_output = openOutput(log_filename);
  }
  
  // check for valid formats
  if ( options[O_MODE] )
  {
    int counter = 0;

    for ( int i = 0; i < (sizeof(format) / sizeof(possibleFormats)); i++)
    {
      if ( validFormats[pair<possibleModi, possibleFormats>(modus,format[i])] && formats[format[i]] )
      {
        counter++;
      }
      else if ( formats[format[i]] )
      {
        trace(TRACE_WARNINGS, "WARNING: " + suffixes[format[i]] + " is no valid format for the chosen mode: omitting.\n");
        formats[format[i]] = false;
      }
    }
    if ((counter > 1) && !options[O_OUTPUT])
    {
      trace(TRACE_WARNINGS, "WARNING: More than one format was selected, but not the output file option.\n");
      trace(TRACE_WARNINGS, "         Therefore all ouput will be mixed on STDOUT.\n");
    }
  }

  // \todo: TODO (gierds) complete information for modus operandi
  trace(TRACE_INFORMATION, "Modus operandi:\n");
  switch ( modus )
  {
    case M_AST: 
            trace(TRACE_INFORMATION, " - generate AST\n");
	    break;
    case M_PRETTY:
	    trace(TRACE_INFORMATION, " - generate XML\n");
	    break;
    case M_CFG:
	    trace(TRACE_INFORMATION, " - generate CFG and do some analysis\n");
	    break;
    case M_PETRINET:
	    trace(TRACE_INFORMATION, " - generate Petri net\n");
	    break;
  }

  if (options[O_INPUT])
  {
    trace(TRACE_INFORMATION, " - input is read from \"" + filename + "\"\n");
  }
  if (options[O_OUTPUT])
  {
    trace(TRACE_INFORMATION, " - output files will be named \"" + output_filename + ".X\"\n");
  }
  if (options[O_LOG])
  {
    trace(TRACE_INFORMATION, " - Logging additional information to \"" + log_filename + "\"\n");
  }

}

std::ostream * openOutput(std::string name)
{
  std::ofstream * file = new std::ofstream(name.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
  if (!file->is_open())
  {
    throw Exception(FILE_NOT_OPEN, "File \"" + name + "\" could not be opened for writing access!\n");
  } 

  return file;
}

void closeOutput(std::ostream * file)
{
  if ( file != NULL )
  {
    (*file) << std::flush;
    ((std::ofstream*)file)->close();
    delete(file);
    file = NULL;
  }
}

