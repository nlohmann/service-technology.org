/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2009        Niels Lohmann
  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
                            Christian Gierds

  GNU BPEL2oWFN is free software: you can redistribute it and/or modify it
  under the terms of the GNU Affero General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
  details.

  You should have received a copy of the GNU Affero General Public License
  along with GNU BPEL2oWFN. If not, see <http://www.gnu.org/licenses/>.
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
 * \date    \$Date: 2007/08/07 12:01:37 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.98 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>

#include "config.h"
#include "options.h"
#include "helpers.h"
#include "debug.h"
#include "globals.h"
#include "cmdline.h"

using std::cin;
using std::cout;
using std::clog;
using std::cerr;
using std::flush;
using std::pair;
using std::ofstream;



extern gengetopt_args_info args_info;


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





/******************************************************************************
 * Functions for command line evaluation
 *****************************************************************************/


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
  suffixes[F_IOLOLA] = "iolola"  ;
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
  suffixes[F_TPN]  = "tpn"   ;
  
  // this array helps us to automatically check the valid formats
  possibleFormats format[] = { F_LOLA, F_OWFN, F_DOT, F_INA, F_SPIN, F_PEP, F_APNN, F_INFO, F_PNML, F_TXT, F_XML, F_IOLOLA, F_TPN };
  
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
  validFormats[pair<possibleModi,possibleFormats>(M_PETRINET,F_TPN)] = true;

  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_IOLOLA)] = true;  
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_LOLA)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_OWFN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_DOT )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_INA )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_SPIN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_PEP )] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_APNN)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_INFO)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_PNML)] = true;
  validFormats[pair<possibleModi,possibleFormats>(M_CHOREOGRAPHY,F_TPN)] = true;
  
  validFormats[pair<possibleModi,possibleFormats>(M_CFG,F_DOT)] = true;
  
  validFormats[pair<possibleModi,possibleFormats>(M_VIS,F_DOT)] = true;
  
  // turn off debug modi of flex and bison by default
  frontend_debug = 0;
  frontend__flex_debug = 0;

  
  // copy getopt-results to variables
  
  // HELP
  if (args_info.help_given)
    options[O_HELP] = true;
  if (args_info.version_given)
    options[O_VERSION] = true;
  
  // MODE
  if (args_info.mode_given) {
    string argument(args_info.mode_arg);
    
    if (argument == "ast") {
      modus = M_AST;
    }
    if (argument == "pretty") {
      modus = M_PRETTY;
      formats[F_XML] = true;
      options[O_FORMAT] = true;
    }
    if (argument == "petrinet") {
      modus = M_PETRINET;
    }
    if (argument == "choreography") {
      modus = M_CHOREOGRAPHY;
    }
    if (argument == "cfg") {
      modus = M_CFG;
    }
    if (argument == "visualization") {
      modus = M_VIS;
    }
  }
  
  // LOG
  if (args_info.log_given) {
    options[O_LOG] = true;
    if (args_info.log_arg != NULL && args_info.log_arg != "")
      log_filename = string(args_info.log_arg);
  }
  
  // DEBUG
  if (args_info.debug_given) {
    options[O_DEBUG] = true;
    for (size_t i=0; i < args_info.debug_given; i++) {
      assert( args_info.debug_arg[i] != NULL);
      string argument(args_info.debug_arg[i]);
      
      if (argument == "flex")
      {
        frontend__flex_debug = 1;
      }
      else if (argument == "bison")
      {
        frontend_debug = 1;
      }
      else if (argument == "1")
        debug_level = TRACE_WARNINGS;
      else if (argument == "2")
        debug_level = TRACE_INFORMATION;
      else if (argument == "3")
        debug_level = TRACE_DEBUG;
      else if (argument == "4")
        debug_level = TRACE_VERY_DEBUG;
      else if (argument == "0")
        debug_level = TRACE_ERROR;
    }    
  }
  
  // INPUT
  if (args_info.input_given) {
    options[O_INPUT] = true;
    for (size_t i=0; i < args_info.input_given; i++) {
      assert( args_info.input_arg[i] != NULL);
      globals::filename = string(args_info.input_arg[i]);

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
  }

  // FORMAT
  if (args_info.format_given) {
    options[O_FORMAT] = true;
    for (size_t i=0; i < args_info.format_given; i++) {
      assert( args_info.format_arg[i] != NULL);
      string argument(args_info.format_arg[i]);
      
      if (argument == suffixes[F_LOLA])
        formats[F_LOLA] = true;
      else if (argument == suffixes[F_OWFN])
        formats[F_OWFN] = true;
      else if (argument == suffixes[F_TPN])
        formats[F_TPN] = true;
      else if (argument == suffixes[F_DOT])
        formats[F_DOT] = true;
      else if (argument == "pep")
        formats[F_PEP] = true;
      else if (argument == suffixes[F_APNN])
        formats[F_APNN] = true;
      else if (argument == suffixes[F_INFO])
        formats[F_INFO] = true;
      else if (argument == "ina")
        formats[F_INA] = true;
      else if (argument == "iolola")
        formats[F_IOLOLA] = true;
      else if (argument == "spin")
        formats[F_SPIN] = true;
      else if (argument == suffixes[F_PNML])
        formats[F_PNML] = true;
      else if (argument == suffixes[F_TXT])
        formats[F_TXT] = true;
      else if (argument == suffixes[F_XML])
        formats[F_XML] = true;
    }      
  }

  // PARAMETER
  if (args_info.parameter_given) {
    options[O_PARAMETER] = true;
    for (size_t i=0; i < args_info.parameter_given; i++) {
      assert( args_info.parameter_arg[i] != NULL);
      string parameter(args_info.parameter_arg[i]);
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
      else if (parameter == "ports")
        globals::parameters[P_PORTS] = true;
      else if(parameter == "decomp")
        globals::parameters[P_DECOMP] = true;      
    }
  }

  // REDUCTION
  if (args_info.reduce_given) {
    globals::reduction_level = args_info.reduce_arg;
  }
  
  // TOPOLOGY
  if (args_info.topology_given) {
    options[O_TOPOLOGY] = true;
    globals::choreography_filename = string(args_info.topology_arg);
    if (!globals::parameters[P_COMMUNICATIONONLY]) {
        trace(TRACE_ALWAYS, "Choreography support only works with parameter 'communicationonly'!\n");
        cleanup();
        exit(1);        
    }
  }

  // WSDL
  if (args_info.wsdl_given) {
    options[O_WSDL] = true;
    globals::wsdl_filename = string(args_info.wsdl_arg);
  }
  
  // OUTPUT
  if (args_info.output_given) {
    options[O_OUTPUT] = true;
    
    if (args_info.output_arg != NULL)
      globals::output_filename = string(args_info.output_arg);    
  }
  
  
  // collect all input files
  for (size_t i=0; i < args_info.inputs_num; i++) {
    options[O_INPUT] = true;
    
    assert( args_info.inputs[i] != NULL);
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
    
    for ( unsigned int i=0; i < (sizeof(format) / sizeof(possibleFormats)); i++)
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
  for ( unsigned int i=0; i < (sizeof(format) / sizeof(possibleFormats)); i++)
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
