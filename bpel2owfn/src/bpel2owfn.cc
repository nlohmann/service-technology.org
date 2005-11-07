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
/// debug level, will soon be outsourced to debug file, so don't use yet!
int mode_debug_level = 0;

int yyerror(const char* msg)
{
  // display passed error message
  fprintf(stderr, "Error in '%s' in line %d:\n", filename, yylineno);
  fprintf(stderr, "  token/text last read was '%s'\n\n", yytext);

  // close input file
  fclose(yyin);


  fprintf(stderr, "-------------------------------------------------------------------------------\n");
  
  int firstShowedLine = ((yylineno-3)>0)?(yylineno-3):1;

  std::ifstream inputFile(filename);
  std::string errorLine;
  for (int i=0; i<firstShowedLine; i++)
    getline(inputFile, errorLine);
  
  // print the erroneous line (plus/minus three more)
  for (int i=firstShowedLine; i<=firstShowedLine+6; i++)
  {
    fprintf(stderr, "%d: %s\n", i, errorLine.c_str());
    getline(inputFile, errorLine);
    if (inputFile.eof())
      break;
  }
  
  fprintf(stderr, "-------------------------------------------------------------------------------\n");
  
  inputFile.close();
  
  
  exit(1);
}

void print_help() 
{
  fprintf(stderr, "\n");
  fprintf(stderr, "BPEL2oWFN\n");
  fprintf(stderr, "---------\n");
  fprintf(stderr, "Options: \n");
  fprintf(stderr, "   -f | --file <filename>  - read input from <filename>,\n");
  fprintf(stderr, "                             if this parameter is omitted, input is read from STDIN\n");
  fprintf(stderr, "   -d | -dd | -ddd |\n");
  fprintf(stderr, "        --debug [1..3]     - set debug level\n");
  fprintf(stderr, "                             ... some more information soon\n");
  fprintf(stderr, "   -h | --help             - print this screen\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "output modes (choose at maximum one, default is just parsing):\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   -a  | --ast             - output the AST\n");
  fprintf(stderr, "   -x  | --xml             - output simple XML w/o any unnecessary informtion\n");
  fprintf(stderr, "   -pn | --petri-net       - output the Petri Net (in LoLA style?)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "special Petri Net modes:\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "   -s  | --simplify        - outout a structural simplified Petri Net\n");
  fprintf(stderr, "                             (implies option -pn)\n");
  fprintf(stderr, "   -D  | --dot             - output dot input\n");
  fprintf(stderr, "                             (implies option -pn)\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "For more information see:\n");
  fprintf(stderr, "  http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel/\n");
  fprintf(stderr, "\n");

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
      
      /// check for input other than stdin
      if (! strcmp(argument_string, "-f") || ! strcmp(argument_string, "--file")) 
      {
        if (argument_counter < argc) 
	{
          filename = argv[argument_counter];
	  // trace(DEBUG_DETAILS, "Reading BPEL from file %s\n", filename);
          if (!(yyin = fopen(filename, "r"))) 
	  {
            fprintf(stderr, "  File '%s' not found.\n", filename);
            // trace(DEBUG_ALWAYS, "  File '%s' not found.\n", filename);
            exit(2);
          }
	}
	else 
	{
          print_help();
	  exit(1);
	}
      }

      /// select output, default is just parsing
      else if (! strcmp(argument_string, "-pn") || ! strcmp(argument_string, "--petri-net")) {
	/// output a Petri Net
        mode_petri_net = true;
      }
      else if (! strcmp(argument_string, "-x") || ! strcmp(argument_string, "--xml")) {
	/// output a pretty xml
        mode_pretty_printer = true;
      }
      else if (! strcmp(argument_string, "-a") || ! strcmp(argument_string, "--ast")) {
	/// output the AST, but why anyone should want to?
        mode_ast = true;
      }
      
      /// simplyfy?
      else if (! strcmp(argument_string, "-s") || ! strcmp(argument_string, "--simplify")) 
      {
        mode_simplify_petri_net = true;
	mode_petri_net = true;
      }

      /// generate dot output
      else if (! strcmp(argument_string, "-D") || ! strcmp(argument_string, "--dot")) 
      {
        mode_dot_petri_net = true;
	mode_petri_net = true;
      }
      
      /// debug
      else if (! strcmp(argument_string, "-d")) 
      {
        mode_debug_level = 1;
      }
      else if (! strcmp(argument_string, "-dd")) 
      {
        mode_debug_level = 2;
      }
      else if (! strcmp(argument_string, "-ddd")) 
      {
        mode_debug_level = 3;
      }
      else if (! strcmp(argument_string, "--debug")) 
      {
        if (argument_counter < argc) 
	{
          argument_string = argv[argument_counter];
	  // instead of atoi we may use the stringstream class?
          mode_debug_level = atoi(argument_string);
	  if (mode_debug_level < 1 || mode_debug_level > 3) {
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

      /// help
      if (! strcmp(argument_string, "-h") || ! strcmp(argument_string, "--help")) 
      {
        print_help();
        exit(1);
      }

      // fprintf(stderr,"Argument %d: %s\n",argument_counter - 1, argument_string);

    }
    /// take care of the output modes
    if ((mode_petri_net && mode_ast) || (mode_petri_net && mode_pretty_printer) || (mode_ast && mode_pretty_printer))
    {
      print_help();
      exit(1);
    }
  }

      
  // don't show debug messages from flex and Bison, unless highest debug mode is requested
  if (mode_debug_level == 3) 
  {
    yydebug = 1;
    yy_flex_debug = 1;
  }
  else 
  {
    yydebug = 0;
    yy_flex_debug = 0;
  }

  fprintf(stderr, "Parsing ...\n");
  
  // invoke Bison parser
  int error = yyparse();

  if (!error)
  {
    fprintf(stderr, "Parsing complete.\n");
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
