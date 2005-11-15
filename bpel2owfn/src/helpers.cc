/*!
 * \file helpers.cc
 *
 * \brief Helper functions (implementation)
 *
 * This file implements several small helper functions that do not belong to
 * any other file.
 * 
 * \author  
 *          - Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *          
 * \date    2005-11-11
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version
 *          - 2005-11-11 (nlohmann) Initial version.
 *          - 2005-11-15 (gierds) Moved commandline evaluation functions from main.cc to here.
 */


#include "helpers.h"


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

// --------------------- functions for command line evaluation --------------------------------

void print_help() 
{
  // 80 chars
  //    "--------------------------------------------------------------------------------"
  trace("\n");
  trace("BPEL2oWFN\n");
  trace("---------\n");
  trace("Options: \n");
  trace("   -f | --file <filename>    - read input from <filename>,\n");
  trace("                               if this parameter is omitted, input is read from\n");
  trace("                               STDIN\n");
  trace("   -d | -dd | -ddd | -dddd | - set debug level\n");
  trace("        --debug [1..4]         ... some more information soon\n");
  trace("   -h | --help               - print this screen\n");
  trace("\n");
  trace("\n");
  trace("output modes (choose at maximum one, default is just parsing):\n");
  trace("\n");
  trace("   -a  | --ast               - output the AST\n");
  trace("   -x  | --xml               - output simple XML w/o any unnecessary\n");
  trace("                               informtion\n");
  trace("   -pn | --petri-net         - output the Petri Net (in LoLA style?)\n");
  trace("\n");
  trace("special Petri Net modes:\n");
  trace("\n");
  trace("   -s   | --simplify         - outout a structural simplified Petri Net\n");
  trace("                               (implies option -pn)\n");
  trace("   -D   | --dot              - output dot input\n");
  trace("                               (implies option -pn)\n");
  trace("   -D2F | --dot2file         - output dot input into file (same name as\n");
  trace("                               input file\n");
  trace("                               (implies option -D)\n");
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel/\n");
  trace("\n");

}

void parse_command_line(int argc, char* argv[])
{
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
          if (!(yyin = fopen(filename.c_str(), "r"))) 
	  {
            // fprintf(stderr, "  File '%s' not found.\n", filename);
            trace("  File '");
	    trace(filename);
	    trace("' not found.\n");
            exit(2);
          }
	}
	else 
	{
          print_help();
	  exit(1);
	}
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

      // help
      else if (! strcmp(argument_string, "-h") || ! strcmp(argument_string, "-?") || ! strcmp(argument_string, "--help")) 
      {
        print_help();
        exit(1);
      }
      else
      {
	trace("Unknown option: " + ((std::string) argument_string) + "\n");
	print_help();
	exit(1);
      }

    }
    // take care of the output modes
    if ((mode_petri_net && mode_ast) || (mode_petri_net && mode_pretty_printer) || (mode_ast && mode_pretty_printer))
    {
      print_help();
      exit(1);
    }
  }

  if ( filename != "")
  {
    trace(TRACE_INFORMATION, "Reading BPEL from file ");
    trace(TRACE_INFORMATION, (filename));
    trace(TRACE_INFORMATION, "\n");

    // if wanted, create a dot output file
    if ( mode_dot_2_file )
    {
      trace(TRACE_INFORMATION, "Creating file for dot output\n");
      std::string dotti_file = filename;
      if ( dotti_file.rfind(".bpel", 0) >= (dotti_file.length() - 8) )
      {
        dotti_file = dotti_file.replace( (dotti_file.length() - 5), 5, ".dot");
      }
      else
      {
        dotti_file += ".dot";
      }
      
      dot_filename = dotti_file.c_str();
      dot_output = new std::ofstream(dotti_file.c_str(), std::ofstream::out | std::ofstream::trunc);
    }
  }
      
  // don't show debug messages from flex and Bison, unless highest debug mode is requested
  if (debug_level == TRACE_VERY_DEBUG) 
  {
    yydebug = 1;
    yy_flex_debug = 1;
  }
  else 
  {
    yydebug = 0;
    yy_flex_debug = 0;
  }

}

