/*****************************************************************************\
 Sayo -- Service Automata Yielded from Operating guidelines

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 Sayo is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Sayo is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Sayo.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

/*!
 * \file    main.cc
 *
 * \brief   Sayo's main source
 *
 * \author  Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: \$Author$
 * 
 * \since   2009-05-10
 *
 * \date    $Date$
 *
 * \version $Revision$
 * 
 */

#include <iostream>
#include <fstream>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <ctime>

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

/// a variable holding the time of the call
clock_t start_clock = clock();

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

/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    // print statistics
    if (args_info.stats_flag) {
        fprintf(stderr, "%s: runtime: %.2f sec\n", PACKAGE, (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}

/// main function
int main(int argc, char** argv)
{
  // set the function to call on normal termination
  atexit(terminationHandler);

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

