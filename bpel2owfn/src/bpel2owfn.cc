#include "main.h"

/// Filename of input file.
char* filename = NULL;


/// The Petri Net
PetriNet *TheNet = new PetriNet();

// different modes controlled by command line

/// print the Petri Net
bool mode_petri_net = false;
/// simplify Petri Net
bool mode_simplify_petri_net = false;
/// paint Petri Net with dot
bool mode_dot_petri_net = false;
/// pretty printer
bool mode_pretty_printer = false;
/// print AST
bool mode_ast = false;


void print_help() 
{
  trace("\n");
  trace("BPEL2oWFN\n");
  trace("---------\n");
  trace("Options: \n");
  trace("   -f | --file <filename>    - read input from <filename>,\n");
  trace("                               if this parameter is omitted, input is read from STDIN\n");
  trace("   -d | -dd | -ddd | -dddd | - set debug level\n");
  trace("        --debug [1..4]         ... some more information soon\n");
  trace("   -h | --help               - print this screen\n");
  trace("\n");
  trace("\n");
  trace("output modes (choose at maximum one, default is just parsing):\n");
  trace("\n");
  trace("   -a  | --ast               - output the AST\n");
  trace("   -x  | --xml               - output simple XML w/o any unnecessary informtion\n");
  trace("   -pn | --petri-net         - output the Petri Net (in LoLA style?)\n");
  trace("\n");
  trace("special Petri Net modes:\n");
  trace("\n");
  trace("   -s  | --simplify          - outout a structural simplified Petri Net\n");
  trace("                               (implies option -pn)\n");
  trace("   -D  | --dot               - output dot input\n");
  trace("                               (implies option -pn)\n");
  trace("\n");
  trace("For more information see:\n");
  trace("  http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel/\n");
  trace("\n");

}

int main( int argc, char *argv[])
{
  /***
   * Reading command line arguments and triggering appropriate behaviour.
   * In case of false parameters call command line help function and exit.
   */

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
          filename = argv[argument_counter];
          if (!(yyin = fopen(filename, "r"))) 
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
          argument_string = argv[argument_counter];
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
      if (! strcmp(argument_string, "-h") || ! strcmp(argument_string, "--help")) 
      {
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

  if ( filename != NULL)
  {
    trace(TRACE_INFORMATION, "Reading BPEL from file ");
    trace(TRACE_INFORMATION, (filename));
    trace(TRACE_INFORMATION, "\n");
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

  trace(TRACE_INFORMATION, "Parsing ...\n");
  
  // invoke Bison parser
  int error = yyparse();

  if (!error)
  {
    trace(TRACE_INFORMATION, "Parsing complete.\n");
    if (mode_ast)
    {
      TheProcess->print();
    }
    TheProcess = TheProcess->rewrite(kc::implicit);
    if (mode_pretty_printer)
    {
      TheProcess->unparse(kc::printer, kc::xml);
    }
    if (mode_petri_net)
    {
      TheProcess->unparse(kc::pseudoPrinter, kc::petrinet);
    }
  }

  if (mode_simplify_petri_net)
  {
    TheNet->simplify();
  }
  if (mode_dot_petri_net)
  {
    TheNet->drawDot();
  }

  //std::cout << std::endl;

  //TheNet->information();
  
  return error;
}
