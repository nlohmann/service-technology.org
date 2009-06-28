// -*- C++ -*-

/*!
 * \file    owfn2bool.h
 *
 * \brief   Experimental oWFN parser without API Framework
 *
 * \author  Christian Sura <christian.sura@uni-rostock.de>,
 *          last changes of: $Author$
 *
 * \since   2009-06-18
 *
 * \date    $Date$
 *
 * \version $Revision$
 * 
 * \note    Trying to implement a faster oWFN parser by avoiding
 *          the AST of the generic parser.
 */

#include "pnapi.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using namespace pnapi;

/// lexer and parser
extern int owfn_yyparse();
extern FILE* owfn_yyin;
extern PetriNet owfn_yynet;

/// main function
int main(int argc, char *argv[])
{
  /// checking for input file
  if (argc < 2) 
  {
      cerr << "No input file given. Expecting oWFN file as parameter #1." << endl;
      return EXIT_FAILURE;
  }

  string input_filename(argv[1]);
  
  if (!(input_filename.substr(input_filename.size()-5) == ".owfn")) 
  {
      cerr << "Expected file ending '.owfn' in parameter #1." << endl;
      return EXIT_FAILURE;
  }
  
  /// open input
  owfn_yyin = fopen(input_filename.c_str(), "r");
  
  /// call parser
  owfn_yyparse();
  
  /// close input 
  fclose(owfn_yyin);
  
  /// when reaching this line, parsing was successful
  return EXIT_SUCCESS;
}

