/*****************************************************************************\
 Compilers -- Compiling several service-technology formats in each other

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 Compilers are free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Compilers are distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Compilers.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#include "cmdline.h"
#include <config.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;


/// the command line parameters
gengetopt_args_info args_info;


/// lexer and parser
extern int yyparse();
extern int yylex_destroy();
extern FILE* yyin;


/// output stream
std::ostream* myOut = &cout;


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
    myOut = &ofs;
  }
  
  // set input source
  if(args_info.input_given) // if user set an input file
  {
    // open file and link input file pointer
    yyin = fopen(args_info.input_arg, "r");
    if(!yyin) // if an error occurred
    {
      cerr << PACKAGE << ": ERROR: failed to open input file '"
           << args_info.input_arg << "'" << endl;
      exit(EXIT_FAILURE);
    }
  }
  
  /// actual parsing
  yyparse();

  // close input (output is closed by destructor)
  fclose(yyin);
  
  /// clean lexer memory
  yylex_destroy();

  exit(EXIT_SUCCESS); // finished parsing
}

