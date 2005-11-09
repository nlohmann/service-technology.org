/*
 * debug.cc
 *
 * Change log:
 * 
 * date        | author        | changes
 * ---------------------------------------------------------------------
 *  2005-11-09 | Gierds        | initial release:
 *             |               |  - simple trace methods
 *             |               |  - contains yyerror function
 *
 */

#include "debug.h"

/// debug level
int debug_level = 0;

/* variables from main.c */
extern char* filename;
extern bool mode_petri_net;
extern bool mode_simplify_petri_net;
extern bool mode_dot_petri_net;
extern bool mode_pretty_printer;
extern bool mode_ast;

/* defined by Bison */
extern int yyparse();
extern int yydebug;

/* defined by flex */
extern int yylineno;
extern char *yytext;
extern int yy_flex_debug;
extern FILE *yyin;


void trace(int trace_level, std::string message)
{
  if (trace_level <= debug_level)
  {
    std::cerr << message << std::flush;
  }

}

void trace(std::string message)
{
  trace(TRACE_ALWAYS, message);
}

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

