/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

/*!
 * \file helpers.cc
 *
 * \brief Helper functions (implementation)
 *
 * This file implements several small helper functions that do not belong to
 * any other file.
 * 
 * \author  
 *          - responsible: Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/11/11
 *          - last changed: \$Date: 2006/01/08 10:08:44 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.33 $
 *
 */


#include "helpers.h"

/// The Petri Net
extern PetriNet *TheNet;

/*!
 * \param a set of Petri net nodes
 * \param b set of Petri net nodes
 * \return union of the sets a and b
 */
set<Node *> setUnion(set<Node *> a, set<Node *> b)
{
  set<Node *> result;
  insert_iterator<set<Node *, less<Node *> > > res_ins(result, result.begin());
  set_union(a.begin(), a.end(), b.begin(), b.end(), res_ins);
  
  return result;
}





/*!
 * \param i standard C int
 * \return  C++ string representing i
 */
string intToString(int i)
{
  char buffer[20];
  sprintf(buffer, "%d", i);
  
  return string(buffer);
}


/*!
 * \param  file     a filename
 * \param  line     a line number
 * \param  function a function
 * \return a formatted string uniting the given information
 */
string pos(const char *file, int line, const char *function)
{
  string result = "function " + string(function) + " (" + string(file) + ":" + intToString(line) + ")\n";
  return result;
}


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
  trace("BPEL2oWFN\n");
  trace("---------\n");
  trace("Options: \n");
  trace("   -f  | --file <filename>    - read input from <filename>,\n");
  trace("                                if this parameter is omitted, input is read\n");
  trace("                                from <STDIN>\n");
  trace("   -d  | -dd | -ddd | -dddd | - set debug level\n");
  trace("         --debug [1..4]         ... some more information soon\n");
  trace("   -df | --debug-flex         - enable flex' debug mode\n");
  trace("   -dy | --debug-yacc         - enable yacc's/bison's debug mode\n");
  trace("   -h  | --help               - print this screen\n");
  trace("   -v  | --version            - prints some version information\n");
  trace("\n");
  trace("\n");
  trace("output modes (choose at most one, default is just parsing):\n");
  trace("\n");
  trace("   -a   | --ast              - output the AST\n");
  trace("   -x   | --xml              - output simple XML w/o any unnecessary\n");
  trace("                               informtion\n");
  trace("   -pn  | --petri-net        - output the Petri Net (in LoLA style?)\n");
  trace("\n");
  trace("special Petri Net modes:\n");
  trace("\n");
  trace("   -s   | --simplify         - outpus a structural simplified Petri Net\n");
  trace("                               (implies option -pn)\n");
  trace("   -ll  | --low-level        - generate an abstract low level Petri Net\n");
  trace("                               (see manual for further information)\n");
  trace("                               (implies option -pn)\n"); 
  trace("   -L   | --lola             - output LoLA input,\n");
  trace("                               should not used together with -D, -O\n");
  trace("                               (implies option -pn)\n");
  trace("   -L2F | --lola2file        - output LoLA input into file (same name as\n");
  trace("                               input file)\n");
  trace("                               (implies option -L)\n");
  /// \todo give the oWFN tool a name
  trace("   -O   | --owfn             - output oWFN input,\n");
  trace("                               should not used together with -D, -L\n");
  trace("                               (implies option -pn)\n");
  trace("   -O2F | --owfn2file        - output oWFN input into file (same name as\n");
  trace("                               input file)\n");
  trace("                               (implies option -O)\n");
  trace("   -D   | --dot              - output dot input,\n");
  trace("                               should not used together with -L, -O\n");
  trace("                               (implies option -pn)\n");
  trace("   -D2F | --dot2file         - output dot input into file (same name as\n");
  trace("                               input file)\n");
  trace("                               (implies option -D)\n");
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel\n");
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
  trace("\n");
  trace("BPEL2oWFN Version 1.0\n");
  trace("---------------------\n");
  trace("Translating BPEL Processes to Open Workflow Nets\n");
  trace("\n");
  trace("Written by Niels Lohmann, Christian Gierds and Dennis Reinert\n");
  trace("\n");
  trace("Copyright (c) 2005, 2006 Niels Lohmann, Christian Gierds and Dennis Reinert\n");
  trace("This is free software; see the source for copying conditions.  There is NO\n");
  trace("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
  trace("\n");
}



void parse_command_line(int argc, char* argv[])
{
  yydebug = 0;
  yy_flex_debug = 0;

  if (argc > 1)
  {
    int argument_counter = 1;
    while (argument_counter < argc) 
    {
      char * argument_string = argv[argument_counter++];
      
      // check for input other than stdin
      if (! strcmp(argument_string, "-f") || ! strcmp(argument_string, "--file")) 
      {
	if (argument_counter < argc) 
	{
          filename = (std::string) argv[argument_counter++];
	}
	else 
	{
          print_help();
	  exit(1);
	}
        mode_file = true;
      }

      else if (! strcmp(argument_string, "-v") || ! strcmp(argument_string, "--version")) {
	print_version(string(argv[0]));
	exit(0);
      }

      // select output, default is just parsing
      else if (! strcmp(argument_string, "-pn") || ! strcmp(argument_string, "--petri-net")) {
	// output a Petri Net
        mode_petri_net = true;
      }
      else if (! strcmp(argument_string, "-x") || ! strcmp(argument_string, "--xml")) {
	// output a pretty xml
        mode_pretty_printer = true;
      }
      else if (! strcmp(argument_string, "-a") || ! strcmp(argument_string, "--ast")) {
	// output the AST, but why anyone should want to?
        mode_ast = true;
      }
      
      // simplyfy?
      else if (! strcmp(argument_string, "-s") || ! strcmp(argument_string, "--simplify")) 
      {
        mode_simplify_petri_net = true;
	mode_petri_net = true;
      }

      // low level?
      else if (! strcmp(argument_string, "-ll") || ! strcmp(argument_string, "--low-level")) 
      {
        mode_low_level_petri_net = true;
	mode_petri_net = true;
      }

      // generate lola output
      else if (! strcmp(argument_string, "-L") || ! strcmp(argument_string, "--lola")) 
      {
        mode_lola_petri_net = true;
	mode_petri_net = true;
      }
      
      // generate lola output and write it into a file
      else if (! strcmp(argument_string, "-L2F") || ! strcmp(argument_string, "--lola2file")) 
      {
        mode_lola_petri_net = true;
	mode_lola_2_file = true;
	mode_petri_net = true;
      }
      
      // generate owfn output
      else if (! strcmp(argument_string, "-O") || ! strcmp(argument_string, "--owfn")) 
      {
        mode_owfn_petri_net = true;
	mode_petri_net = true;
      }
      
      // generate owfn output and write it into a file
      else if (! strcmp(argument_string, "-O2F") || ! strcmp(argument_string, "--owfn2file")) 
      {
        mode_owfn_petri_net = true;
	mode_owfn_2_file = true;
	mode_petri_net = true;
      }
      
      // generate dot output
      else if (! strcmp(argument_string, "-D") || ! strcmp(argument_string, "--dot")) 
      {
        mode_dot_petri_net = true;
	mode_petri_net = true;
      }
      
      // generate dot output and write it into a file
      else if (! strcmp(argument_string, "-D2F") || ! strcmp(argument_string, "--dot2file")) 
      {
        mode_dot_petri_net = true;
	mode_dot_2_file = true;
	mode_petri_net = true;
      }
      
      // debug
      else if (! strcmp(argument_string, "-d")) 
      {
        debug_level = TRACE_WARNINGS;
      }
      else if (! strcmp(argument_string, "-dd")) 
      {
        debug_level = TRACE_INFORMATION;
      }
      else if (! strcmp(argument_string, "-ddd")) 
      {
        debug_level = TRACE_DEBUG;
      }
      else if (! strcmp(argument_string, "-dddd")) 
      {
        debug_level = TRACE_VERY_DEBUG;
      }
      else if (! strcmp(argument_string, "--debug")) 
      {
        if (argument_counter < argc) 
	{
          argument_string = argv[argument_counter++];
	  // instead of atoi we may use the stringstream class?
          debug_level = atoi(argument_string);
	  if (debug_level < 1 || debug_level > 4) {
            print_help();
	    exit(1);
	  }
	}
	else 
	{
          print_help();
	  exit(1);
	}
      }

      // debug flex
      else if (! strcmp(argument_string, "-df") || ! strcmp(argument_string, "--debug-flex")) 
      {
        yy_flex_debug = 1;
      }
      // debug yacc/bison
      else if (! strcmp(argument_string, "-dy") || ! strcmp(argument_string, "--debug-yacc")) 
      {
        yydebug = 1;
      }

      // help
      else if (! strcmp(argument_string, "-h") || ! strcmp(argument_string, "-?") || ! strcmp(argument_string, "--help")) 
      {
        print_help();
        exit(0);
      }

      // unknown parameter
      else
      {
	trace("Unknown option: " + ((std::string) argument_string) + "\n");
	print_help();
	exit(1);
      }

    }

    // open input file
    if (mode_file)
    {
      if (!(yyin = fopen(filename.c_str(), "r"))) 
      {
	mode_file = false;
        throw Exception(FILE_NOT_FOUND, "File '" + filename + "' not found.\n");
      }

    }
    
    // take care of the output modes
    if ((mode_petri_net && mode_ast) || (mode_petri_net && mode_pretty_printer) || (mode_ast && mode_pretty_printer))
    {
    throw Exception(OPTION_MISMATCH, 
		    "Chosen parameters cannot work together (see parameter -h).\n",
		    "Chose only one of mode Petri Net, AST or XML!\n");
    }
  }

  // LoLA and dot on stdout are very confusing ! so abort 
  if ((mode_lola_petri_net && mode_dot_petri_net) && !(mode_lola_2_file || mode_dot_2_file))
  {
    throw Exception(OPTION_MISMATCH, 
		    "Chosen parameters cannot work together (see parameter -h).\n",
		    "LoLA and dot output on stdout are confusing, chose one!\n");
  }
  // LoLA and oWFN on stdout are very confusing ! so abort 
  if ((mode_lola_petri_net && mode_owfn_petri_net) && !(mode_lola_2_file || mode_owfn_2_file))
  {
    throw Exception(OPTION_MISMATCH, 
		    "Chosen parameters cannot work together (see parameter -h).\n",
		    "LoLA and oWFN output on stdout are confusing, chose one!\n");
  }
  // oWFN and dot on stdout are very confusing ! so abort 
  if ((mode_owfn_petri_net && mode_dot_petri_net) && !(mode_owfn_2_file || mode_dot_2_file))
  {
    throw Exception(OPTION_MISMATCH, 
		    "Chosen parameters cannot work together (see parameter -h).\n",
		    "oWFN and dot output on stdout are confusing, chose one!\n");
    // exit(1);
  }
  
  // trace and check for some files to be created
  if ( mode_file )
  {
    trace(TRACE_INFORMATION, "Reading BPEL from file ");
    trace(TRACE_INFORMATION, (filename));
    trace(TRACE_INFORMATION, "\n");

    // if wanted, create a dot output file
    if ( mode_dot_2_file )
    {
      trace(TRACE_INFORMATION, "Creating file for dot output\n");
      std::string dotti_file = filename;
      
      // try to replace .bpel through .dot
      if ( dotti_file.rfind(".bpel", 0) >= (dotti_file.length() - 6) )
      {
        dotti_file = dotti_file.replace( (dotti_file.length() - 5), 5, ".dot");
      }
      else
      {
        dotti_file += ".dot";
      }
      
      /// set dot filename
      dot_filename = dotti_file.c_str();
      /// create dot file and point to it
      dot_output = new std::ofstream(dotti_file.c_str(), std::ofstream::out | std::ofstream::trunc);
    }

    // if wanted, create a LoLA output file
    if ( mode_lola_2_file )
    {
      trace(TRACE_INFORMATION, "Creating file for LoLA output\n");
      std::string lola_file = filename;
      
      // try to replace .bpel through .lola
      if ( lola_file.rfind(".bpel", 0) >= (lola_file.length() - 6) )
      {
        lola_file = lola_file.replace( (lola_file.length() - 5), 5, ".lola");
      }
      else
      {
        lola_file += ".lola";
      }
      
      /// set lola filename
      lola_filename = lola_file.c_str();
      /// create lola file and point to it
      lola_output = new std::ofstream(lola_file.c_str(), std::ofstream::out | std::ofstream::trunc);
    }

    // if wanted, create a oWFN output file
    if ( mode_owfn_2_file )
    {
      trace(TRACE_INFORMATION, "Creating file for oWFN output\n");
      std::string owfn_file = filename;
      
      // try to replace .bpel through .owfn
      if ( owfn_file.rfind(".bpel", 0) >= (owfn_file.length() - 6) )
      {
        owfn_file = owfn_file.replace( (owfn_file.length() - 5), 5, ".owfn");
      }
      else
      {
        owfn_file += ".owfn";
      }
      
      /// set owfn filename
      owfn_filename = owfn_file.c_str();
      /// create owfn file and point to it
      owfn_output = new std::ofstream(owfn_file.c_str(), std::ofstream::out | std::ofstream::trunc);
    }

    // create info file for Petri Net
    if ( mode_petri_net )
    {
      trace ( TRACE_INFORMATION, "Creating file for Petri Net information\n");
      std::string info_file = filename;
      
      // try to replace .bpel through .info
      if ( info_file.rfind(".bpel", 0) >= (info_file.length() - 6) )
      {
        info_file = info_file.replace( (info_file.length() - 5), 5, ".info");
      }
      else
      {
        info_file += ".info";
      }

      /// set info filename
      info_filename = info_file.c_str();
      /// create info file and point to it
      info_output = new std::ofstream(info_file.c_str(), std::ofstream::out | std::ofstream::trunc);
    }
  }
  else
  {
    mode_lola_2_file = false;
    mode_owfn_2_file = false;
    mode_dot_2_file = false;
  }  
 
  // trace information about current mode
  trace(TRACE_INFORMATION, "\nModus operandi:\n");
  if (mode_simplify_petri_net)
    trace(TRACE_INFORMATION, " - create structural simlified Petri Net\n");
  else if (mode_petri_net)
    trace(TRACE_INFORMATION, " - create Petri Net\n");

  if (mode_low_level_petri_net)
    trace(TRACE_INFORMATION, "   --> abstract to low level\n");

  if (mode_petri_net && mode_file) 
    trace(TRACE_INFORMATION, " - output information about the Petri Net to file " + info_filename + "\n");

  if (mode_lola_2_file)
    trace(TRACE_INFORMATION, " - output LoLA input of the Petri Net to file " + lola_filename + "\n");
  else if (mode_lola_petri_net)
    trace(TRACE_INFORMATION, " - output LoLA input of the Petri Net\n");

  if (mode_owfn_2_file)
    trace(TRACE_INFORMATION, " - output oWFN input of the Petri Net to file " + owfn_filename + "\n");
  else if (mode_owfn_petri_net)
    trace(TRACE_INFORMATION, " - output oWFN input of the Petri Net\n");

  if (mode_dot_2_file)
    trace(TRACE_INFORMATION, " - output dot representation of the Petri Net to file " + dot_filename + "\n");
  else if (mode_dot_petri_net)
    trace(TRACE_INFORMATION, " - output dot representation of the Petri Net\n");
  
  if (mode_pretty_printer)
    trace(TRACE_INFORMATION, " - output \"pretty\" XML\n");

  if (mode_ast)
    trace(TRACE_INFORMATION, " - output AST\n");

  if (debug_level > 0)
    trace(TRACE_INFORMATION, " - debug level is set to " + intToString(debug_level) + "\n");
 
  if (yy_flex_debug > 0)
    trace(TRACE_INFORMATION, " - special debug mode for flex is enabled\n");

  if (yydebug > 0)
    trace(TRACE_INFORMATION, " - special debug mode for yacc/bison is enabled\n");
  
  trace(TRACE_INFORMATION, "\n");
}





/**
 * Some output in case an error has occured.
 * 
 */
void error()
{
  trace("\nAn error has occured while parsing \"" + filename + "\"!\n\n");
  // call #cleanup()
  cleanup();
  trace(TRACE_WARNINGS, "-> Any output file might be in an undefinded state.\n");
  exit(1);

}





/**
 * Some output in case an error has occured.
 * Prints out the exception's information.
 *
 * \param e The exception that triggered this function call. 
 * 
 */
void error(Exception e)
{
  trace("\nAn error has occured while parsing \"" + filename + "\"!\n\n");
  // output information about the exception
  e.info();
  // call #cleanup()
  cleanup();
  trace(TRACE_WARNINGS, "-> Any output file might be in an undefinded state.\n");
  // stop execution
  exit(e.id);
}





/**
 * Cleans up.
 * Afterwards we should have an almost defined state.
 *
 */
void cleanup()
{
  trace(TRACE_INFORMATION,"Cleaning up ...\n");
 
  if ( mode_file )
  {
    trace(TRACE_INFORMATION," + Closing input file: " + filename + "\n");
    fclose(yyin);
  }

  // close info file
  if ( mode_file && mode_petri_net )
  {
    trace(TRACE_INFORMATION," + Closing Petri Net info file: " + info_filename + "\n");
    (*info_output) << std::flush;
    ((std::ofstream*)info_output)->close();
    delete(info_output);
    info_output = NULL;
  }

  if ( mode_file )
  {
    if ( mode_lola_2_file )
    {
      trace(TRACE_INFORMATION," + Closing LoLA output file: " + lola_filename + "\n");
      (*lola_output) << std::flush;
      ((std::ofstream*)lola_output)->close();
      delete(lola_output);
      lola_output = NULL;
    }

    if ( mode_owfn_2_file )
    {
      trace(TRACE_INFORMATION," + Closing oWFN output file: " + owfn_filename + "\n");
      (*owfn_output) << std::flush;
      ((std::ofstream*)owfn_output)->close();
      delete(owfn_output);
      owfn_output = NULL;
    }

    if (mode_dot_2_file)
    {
      trace(TRACE_INFORMATION," + Closing dot output file: " + dot_filename + "\n");
      (*dot_output) << std::flush;
      ((std::ofstream*)dot_output)->close();
      delete(dot_output);
      dot_output = NULL;
    }
  }
    
  if (mode_petri_net) 
  {
    trace(TRACE_INFORMATION," + Deleting Petri Net pointer\n");
    delete(TheNet);
    TheNet = NULL;
  }

}

