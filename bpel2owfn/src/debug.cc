/*!
 * \file debug.cc
 *
 * \brief Some debugging tools for BPEL2oWFN
 *
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2005/11/09
 *          - last changed: \$Date: 2005/11/16 11:05:33 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.5 $
 *          - 2005-11-09 (gierds) Initial release.
 *            Simple trace methods and new place for yyerror().
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
 
  error();
  return 1;
}

