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
extern std::string filename;
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


void trace(trace_level pTraceLevel, std::string message)
{
  if (pTraceLevel <= debug_level)
  {
    std::cerr << message << std::flush;
  }

}

void trace(std::string message )
{
  trace(TRACE_ALWAYS, message);
}

int yyerror(const char* msg)
{
  // display passed error message
  trace("\n");
  
  if (filename != "")
  {
    trace("Error in '" + filename + "' in line ");
    trace(intToString(yylineno));
    trace(":\n");
    trace("  token/text last read was '");
    trace(yytext);
    trace("'\n\n");

    // close input file
    fclose(yyin);
  }


  trace("-------------------------------------------------------------------------------\n");
  
  int firstShowedLine = ((yylineno-3)>0)?(yylineno-3):1;

  std::ifstream inputFile(filename.c_str());
  std::string errorLine;
  for (int i=0; i<firstShowedLine; i++)
    getline(inputFile, errorLine);
  
  // print the erroneous line (plus/minus three more)
  for (int i=firstShowedLine; i<=firstShowedLine+6; i++)
  {
    trace(intToString(i) + ": " + errorLine + "\n");
    getline(inputFile, errorLine);
    if (inputFile.eof())
      break;
  }
  
  trace("-------------------------------------------------------------------------------\n");
  
  inputFile.close();
  
  
  exit(1);
}

