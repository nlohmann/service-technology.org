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
 * \file    options.cc
 *
 * \brief   evaluation of command-line options
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2007/07/30 19:56:41 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.97 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>

#include "bpel2owfn.h"
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

/// pointer to log stream
ostream *log_output = &clog;

/// filename of log file
string log_filename = "";

/// true if output is created
bool createOutputFile = false;

/// different modes controlled by command line
possibleModi modus;

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
  { "mode",		required_argument, NULL, 'm' },
  { "log",		optional_argument, NULL, 'l' },
  { "input",		required_argument, NULL, 'i' },
  { "output",		optional_argument, NULL, 'o' },
  { "format",		required_argument, NULL, 'f' },
  { "parameter",	required_argument, NULL, 'p' },
  { "debug",		required_argument, NULL, 'd' },
  { "reduce",		required_argument, NULL, 'r' },
  { "topology",		required_argument, NULL, 't' },
  { "wsdl",		required_argument, NULL, 'w' },
  { "net",		required_argument, NULL, 'n' },
  NULL
};

/// short options (needed by GNU getopt)
const char *par_string = "hvm:li:of:p:d:r:t:w:n:";





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
  trace(" -m, --mode=MODE        use the given mode\n");
  trace(" -p, --parameter=PARAM  modify processing with given parameter\n");
  trace(" -i, --input=FILE       read a BPEL process from FILE\n");
  trace(" -o, --output[=NAME]    write output to file (NAME sets filename)\n");
  trace(" -f, --format=FORMAT    create output of the given format\n");
  trace(" -l, --log=NAME         create log file (NAME sets filename)\n");
  trace(" -d, --debug=NUMBER     set a debug level (NUMBER=0..4 or \"flex\" or \"bison\")\n");
  trace(" -r, --reduce=NUMBER    apply structural reduction level (NUMBER=0..5)\n");
  trace(" -h, --help             print this help list and exit\n");
  trace(" -v, --version          print program version and exit\n");
  trace(" -t, --topology=FILE    read a BPEL4Chor participant topology file from FILE\n");
  trace(" -w, --wsdl=FILE        read a WSDL file from FILE\n");
  trace(" -n, --net=FILE         read an OWFN or PNML file from FILE\n");
  trace("\n");
  trace("  MODE is one of the following (at most one mode permitted):\n");
  trace("    petrinet            create a Petri net model\n");
  trace("    ast                 print the abstract syntax tree\n");
  trace("    pretty              pretty-print the input BPEL process\n");
  trace("    cfg                 build a control flow graph\n");
  trace("    choreography        composes several processes to a choreography\n");
  trace("\n");
  trace("  PARAMETER is one of the following (multiple parameters permitted):\n");
  trace("    reduce              structurally simplify generated Petri net (-r5)\n");
  trace("    variables           model BPEL variables\n");
  trace("    standardfaults      model BPEL standard faults\n");
  trace("    fhfaults            model faults in fault handlers \n");
  trace("    communicationonly   only model the communicational behavior\n");
  trace("    xor                 use mutually exclusive transition condition\n");
  trace("    nointerface         do not draw an interface (for dot output)\n");
  trace("\n");
  trace("  FORMAT is one of the following (multiple formats permitted):\n");
  trace("    lola, owfn, dot, pep, apnn, ina, spin, info, pnml, txt, info\n");
  trace("\n");
  trace("Examples:\n");
  trace("  bpel2owfn -i service.bpel -m petrinet -f owfn -o\n");
  trace("  bpel2owfn -i service.bpel -m petrinet -f dot -p reduce -o\n");
  trace("\n");
  trace("Report bugs to <" + string(PACKAGE_BUGREPORT) + ">.\n");
}





/*!
 * \brief prints version information
 */
void print_version()
{
  // 80 chars
  //    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
  trace(string(PACKAGE_STRING) + " (compiled " + string(__DATE__) + ")\n\n");
  trace("Copyright (C) 2006, 2007 Niels Lohmann, Christian Gierds and Martin Znamirowski\n");
  trace("Copyright (C) 2005 Niels Lohmann and Christian Gierds\n\n");
  trace("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n");
  trace(string(PACKAGE_NAME) + " is free software: you are free to change and redistribute it.\n");
  trace("There is NO WARRANTY, to the extent permitted by law.\n\n");  
  trace("See http://www.gnu.org/software/bpel2owfn for more information\n");
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
  suffixes[F_LOLA] = "lola"  ;
  suffixes[F_OWFN] = "owfn"  ;
  suffixes[F_DOT]  = "dot"   ;
  suffixes[F_INA]  = "pnt";
  suffixes[F_SPIN]  = "spin";
  suffixes[F_PEP]  = "ll_net";
  suffixes[F_APNN] = "apnn"  ;
  suffixes[F_INFO] = "info"  ;
  suffixes[F_PNML] = "pnml"  ;
  suffixes[F_TXT]  = "txt"   ;
  suffixes[F_XML]  = "xml"   ;
  
  // this array helps us to automatically check the valid formats
  possibleFormats format[] = { F_LOLA, F_OWFN, F_DOT, F_INA, F_SPIN, F_PEP, F_APNN, F_INFO, F_PNML, F_TXT, F_XML };
  
  // this map indicates, whether a certain format is valid for a mode
  map< pair<possibleModi,possibleFormats>, bool > validFormats;
  
  validFormats[pair<possibleModi,possibleFormats>(M_AST,F_DOT)] = true;
  
  validFormats[pair<possibleModi,possibleFormats>(M_PRETTY,F_XML)] = true;
  
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_LOLA)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_OWFN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_DOT )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_INA )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_SPIN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_PEP )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_APNN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_INFO)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_PNML)] = true;
  
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_LOLA)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_OWFN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_DOT )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_INA )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_SPIN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_PEP )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_APNN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_INFO)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_PNML)] = true;
  
  validFormats[pair<possibleModi,possibleFormats>(M_CFG,F_DOT)] = true;
  
  validFormats[pair<possibleModi,possibleFormats>(M_VIS,F_DOT)] = true;
  
  // the programme's name on the commandline
  string progname = string(argv[0]);
  
  
  // turn off debug modi of flex and bison by default
  frontend_debug = 0;
  frontend__flex_debug = 0;
  frontend_owfn_debug = 0;
  frontend_owfn__flex_debug = 0;
  frontend_pnml_debug = 0;
  
  
  // use GNU getopt to parse the command-line arguments
  int optc = 0;
  while ((optc = getopt_long (argc, argv, par_string, longopts, static_cast<int *>(0)))
         != EOF)
  {
    /// \todo call one of them argument and remove the rest
    string parameter = "";
    possibleModi old_modus;
    switch (optc)
    {
      case 'h': options[O_HELP] = true; break;
        
      case 'v': options[O_VERSION] = true; break;
        
      case 'm':
      {
        old_modus = modus;
        parameter = string(optarg);
        
        if (parameter == "ast")
          modus = M_AST;
        else if (parameter == "pretty")
        {
          modus = M_PRETTY;
          formats[F_XML] = true;
          options[O_FORMAT] = true;
        }
        else if (parameter == "petrinet" || parameter == "pn")
          modus = M_PETRINET;
        else if (parameter == "choreography" || parameter == "chor")
          modus = M_CHOREOGRAPHY;
        else if (parameter == "cfg")
          modus = M_CFG;
        else if (parameter == "visualization" || parameter == "vis")
          modus = M_VIS;
        else  {
          trace(TRACE_ALWAYS, "Unknown mode \"" + parameter+ "\".\n");
          trace(TRACE_ALWAYS, "Use -h to get a list of valid modes.\n");
          cleanup();
          exit(1);
        }
        
        if (options[O_MODE] && modus != old_modus) {
          trace(TRACE_ALWAYS, "Multiple modes are given, please choose only one!\n");
          cleanup();
          exit(1);
        }
        
        options[O_MODE] = true;
        
        break;
      }
        
      case 'l':
      {
        options[O_LOG] = true;
        if (optarg != NULL)
          log_filename = string(optarg);
        break;
      }
        
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
          cleanup();
          exit(1);
        }
        
        options[O_OUTPUT] = true;
        
        if (optarg != NULL)
          globals::output_filename = string(optarg);
        
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
        else if (parameter == suffixes[F_INFO])
          formats[F_INFO] = true;
        else if (parameter == "ina")
          formats[F_INA] = true;
        else if (parameter == "spin")
          formats[F_SPIN] = true;
        else if (parameter == suffixes[F_PNML])
          formats[F_PNML] = true;
        else if (parameter == suffixes[F_TXT])
          formats[F_TXT] = true;
        else if (parameter == suffixes[F_XML])
          formats[F_XML] = true;
        else {
          trace(TRACE_ALWAYS, "Unknown format \"" + parameter +"\".\n");
          trace(TRACE_ALWAYS, "Use -h to get a list of valid formats.\n");
          cleanup();
          exit(1);
        }
        
        break;
      }
        
      case 'p':
      {
        options[O_PARAMETER] = true;
        parameter = string(optarg);
        
        if ( parameter == "reduce")
          globals::reduction_level = 5;
        else if (parameter == "communicationonly" | parameter == "small") // communicationonly is deprecated!
          globals::parameters[P_COMMUNICATIONONLY] = true;
        else if (parameter == "standardfaults")
          globals::parameters[P_STANDARDFAULTS] = true;
        else if (parameter == "fhfaults")
          globals::parameters[P_FHFAULTS] = true;
        else if (parameter == "variables")
          globals::parameters[P_VARIABLES] = true;
        else if (parameter == "xor")
          globals::parameters[P_XOR] = true;
        else if (parameter == "loopcount")
          globals::parameters[P_LOOPCOUNT] = true;
        else if (parameter == "loopcontrol")
          globals::parameters[P_LOOPCONTROL] = true;
        else if (parameter == "nointerface")
          globals::parameters[P_NOINTERFACE] = true;
        else if (parameter == "deadlocktest")
          globals::parameters[P_DEADLOCKTEST] = true;
        else if (parameter == "data")
          globals::parameters[P_DATA] = true;
        else if (parameter == "ports")
          globals::parameters[P_PORTS] = true;
        else {
          trace(TRACE_ALWAYS, "Unknown parameter \"" + parameter +"\".\n");
          trace(TRACE_ALWAYS, "Use -h to get a list of valid parameters.\n");
          cleanup();
          exit(1);
        }
        
        break;
      }
        
      case 'd':
      {
        options[O_DEBUG] = true;
        parameter = string(optarg);
        
        if (parameter == "flex")
        {
          frontend__flex_debug = 1;
          frontend_owfn__flex_debug = 1;
        }
        else if (parameter == "bison")
        {
          frontend_debug = 1;
          frontend_owfn_debug = 1;
          frontend_pnml_debug = 1;
        }
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
          cleanup();
          exit(1);
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
          cleanup();
          exit(1);
        }
        
        break;
      }
        
      case 't':
      {
        options[O_TOPOLOGY] = true;
        globals::choreography_filename = string(optarg);
        break;
      }
        
      case 'w':
      {
        options[O_WSDL] = true;
        globals::wsdl_filename = string(optarg);
        break;
      }

      case 'n':
      {
        options[O_NET] = true;
        
        // set the mode to Petri net
        modus = M_PETRINET;
        string suffix;
        suffix = string(optarg).substr(string(optarg).length()-4);
        if (suffix == "owfn")
        {
          globals::net_filename = string(optarg);
          globals::net_mode = OWFN;
        }
        else if (suffix == "pnml")
        {
          globals::net_filename = string(optarg);
          globals::net_mode = PNML;
        }
        else
        {
        globals::net_mode = NONE;
        trace(string(optarg) + " does not have a valid filetype!\n");
        trace(TRACE_ALWAYS, "Use -h to get a list of valid filetypes.\n");
        }
        break;
      }
        
      default:
      {
        trace("Unknown option!\n");
        trace(TRACE_ALWAYS, "Use -h to get a list of valid options.\n");
        cleanup();
        exit(1);
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
    exit(0);
  }
  
  
  // print version and exit
  if (options[O_VERSION])
  {
    print_version();
    exit(0);
  }
  
  
  // if input file is given, bind it to frontend_in
  if (options[O_INPUT])
  {
    if (modus != M_CHOREOGRAPHY && inputfiles.size() > 1)
    {
      trace(TRACE_ALWAYS, "Multiple input options are given, please choose only one!\n");
      cleanup();
      exit(1);
    }
    set< string >::iterator file = inputfiles.begin();
    FILE * fin = NULL;
    if (!(fin = fopen(file->c_str(), "r")))
    {
	    trace(TRACE_ALWAYS, "File '" + *file + "' not found.\n");
      exit(1);
    }
    fclose(fin);
    file++;
    while(modus == M_CHOREOGRAPHY && file != inputfiles.end())
    {
      if (!(fin = fopen(file->c_str(), "r")))
      {
        trace(TRACE_ALWAYS, "File '" + *file + "' not found.\n");
        exit(1);
      }
      else
      {
        fclose( fin );
      }
      file++;
    }
  }
  
  if ( options[O_OUTPUT] )
  {
    createOutputFile = true;
  }
  
  // set output file name if non is already chosen
  if ((options[O_OUTPUT] || options[O_LOG]) && (globals::output_filename == ""))
  {
    // set output file name to a standard output filename in case of no inputfiles
    if ( not(options[O_INPUT]) )
    {
      if (options[O_NET])
      {
        unsigned int pos = globals::net_filename.length() - 5;
        globals::output_filename = globals::net_filename.substr(0, pos);
      }
      else
      {
        globals::output_filename = "stdof"; 
        trace(TRACE_ALWAYS, "Output filename set to standard: stdof\n");
      }
    } 
    else 
    {
      set< string >::iterator file = inputfiles.begin();
      unsigned int pos = file->rfind(".bpel", file->length());
      if (pos == (file->length() - 5))
      {
        globals::output_filename = file->substr(0, pos);
      }
      else
      {
        globals::output_filename = *file;
      }
      file++;
      while(modus == M_CHOREOGRAPHY && file != inputfiles.end())
      {
        unsigned int pos = file->rfind(".bpel", file->length());
        unsigned int pos2 = file->rfind("/", file->length());
        if (pos == (file->length() - 5))
        {
          globals::output_filename += "_" + file->substr(pos2 + 1, pos - pos2 - 1);
        }
        file++;
      }
      
    }
  }
  
  if (options[O_LOG])
  {
    if (log_filename == "")
    {
      log_filename = globals::output_filename + ".log";
    }
    log_output = openOutput(log_filename);
  }
  
  // check for valid formats
  if ( options[O_MODE] )
  {
    int counter = 0;
    
    for ( unsigned int i = 0; i < (sizeof(format) / sizeof(possibleFormats)); i++)
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
  
  /// \todo: TODO (gierds) complete information for modus operandi
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
    case M_VIS:
	    trace(TRACE_INFORMATION, " - generating activity diagram\n");
	    break;
    case M_PETRINET:
	    trace(TRACE_INFORMATION, " - generate Petri net\n");
	    break;
    case M_CHOREOGRAPHY:
	    trace(TRACE_INFORMATION, " - choreography mode\n");
	    break;
  }
  
  if (options[O_INPUT])
  {
    trace(TRACE_INFORMATION, " - input is read from \"" + globals::filename + "\"\n");
  }
  if (options[O_OUTPUT])
  {
    trace(TRACE_INFORMATION, " - output files will be named \"" + globals::output_filename + ".X\"\n");
  }
  for ( unsigned int i = 0; i < (sizeof(format) / sizeof(possibleFormats)); i++)
  {
    if ( formats[format[i]] )
    {
      trace(TRACE_INFORMATION, "   + output will be written in " + suffixes[format[i]] + " style\n");
    }
  }
  
  if (options[O_LOG])
    trace(TRACE_INFORMATION, " - Logging additional information to \"" + log_filename + "\"\n");
  
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
