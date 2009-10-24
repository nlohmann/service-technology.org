#include <iostream>
#include <fstream>
#include <map>
#include <cstdio>
#include <cstdlib>

#include "cmdline.h"
#include "config.h"

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::ofstream;


/// the command line parameters
gengetopt_args_info args_info;


/// lexer and parser
extern int og_yyparse();
extern int og_yylex_destroy();
extern FILE* og_yyin;


/// output stream
std::ostream* outStream = &cout;


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) 
{
  // initialize the parameters structure
  struct cmdline_parser_params *params = cmdline_parser_params_create();

  // call the cmdline parser
  cmdline_parser(argc, argv, &args_info);

  free(params);
}


/// main function
int main(int argc, char** argv)
{
  // stream for file output
  ofstream ofs;
  
  // parse commandline
  evaluateParameters(argc, argv);
  
  // set output destination
  if(args_info.output_given) // if user set an output file
  {
    ofs.open(args_info.output_arg, std::ios_base::trunc); // open file
    if(!ofs) // if an error occurred on opening the file
    {
      cerr << PACKAGE << ": ERROR: failed to open output file '"
           << args_info.output_arg << "'" << endl;
      exit(EXIT_FAILURE);
    }
    
    // else link output stream to file stream
    outStream = &ofs;
  }
  
  // set input source
  if(args_info.input_given) // if user set an input file
  {
    // open file and link input file pointer
    og_yyin = fopen(args_info.input_arg, "r");
    if(!og_yyin) // if an error occurred
    {
      cerr << PACKAGE << ": ERROR: failed to open input file '"
           << args_info.input_arg << "'" << endl;
      exit(EXIT_FAILURE);
    }
  }
  
  /// actual parsing
  og_yyparse();

  // close input (output is closed by destructor)
  fclose(og_yyin);
  
  /// clean lexer memory
  og_yylex_destroy();

  return EXIT_SUCCESS; // finished parsing
}