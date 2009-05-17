// -*- C++ -*-

/*!
 * \file    main.cc
 *
 * \brief   Jane Doe's main source
 *
 * \author  Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: \$Author: cas $
 * 
 * \since   2009-05-10
 *
 * \date    $Date: 2009-02-16 08:39:17 +0100 (Mo, 16 Feb 2009) $
 *
 * \version $Revision: 3882 $
 * 
 */

#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>

#include "cmdline.h"
#include "config.h"

using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::map;
using std::ofstream;


/// the command line parameters
gengetopt_args_info args_info;


/// lexer and parser
extern int ognew_yyparse();
extern FILE* ognew_yyin;


/// output stream
std::ostream* myOut = &cout;


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) 
{
  // set default values
  cmdline_parser_init(&args_info);

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
  evaluateParameters(argc,argv);
  
  // set output destination
  if(args_info.output_given)
  {
    ofs.open(args_info.output_arg);
    if(ofs.bad())
    {
      cerr << "ERROR: Can't open output file." << endl << flush;
      exit(EXIT_FAILURE);
    }
    
    myOut = &ofs;
  }
  
  // set input source
  bool useFile = false;
  if(args_info.input_given)
  {
    ognew_yyin = fopen(args_info.input_arg, "r");
    
    if(ognew_yyin == 0)
    {
      cerr << "ERROR: Can't open input file." << endl << flush;
      exit(EXIT_FAILURE);
    }
    
    useFile = true;
  }
  
  /// actual parsing
  ognew_yyparse();
  
  /// close input
  if(useFile)
    fclose(ognew_yyin);
  
  /// close output
  if(myOut == &ofs)
    ofs.close();
  
  exit(EXIT_SUCCESS);
}
