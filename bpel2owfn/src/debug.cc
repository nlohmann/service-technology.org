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
 *          - last changed: \$Date: 2005/11/30 14:47:07 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.9 $
 *          - 2005-11-09 (gierds) Initial release.
 *            Simple trace methods and new place for yyerror().
 *          - 2005-11-20 (nlohmann) Overworked and commented yyerror().
 *
 */

#include "debug.h"

/// debug level
int debug_level = 0;





/**
 * Provides output to stderr using different #trace_level 
 * (in order to regulate amount of output)
 *
 * \param pTraceLevel	the #trace_level
 * \param message	the output
 *
 */
void trace(trace_level pTraceLevel, std::string message)
{
  if (pTraceLevel <= debug_level)
  {
    std::cerr << message << std::flush;
  }
}


/**
 * Works like #trace(trace_level,std::string) with trace_level = TRACE_ALWAYS
 *
 * \param message the output
 *
 */
void trace(std::string message )
{
  trace(TRACE_ALWAYS, message);
}





/*!
 * This function is invoked by the parser and the lexer during the syntax
 * analysis. When an error occurs, it prints an accordant message and shows the
 * lines of the input files where the error occured.
 *
 * \param msg a message (mostly "Parse error") and some more information e.g.
 *            the location of the syntax error.
 * \return 1, since an error occured
 */
int yyerror(const char* msg)
{
  /* defined by flex */
  extern int yylineno;      ///< line number of current token
  extern char *yytext;      ///< text of the current token

  trace("Error while parsing!\n\n");
  trace(msg);
  trace("\n");
	
  // display passed error message
  trace("Error in '" + filename + "' in line ");
  trace(intToString(yylineno));
  trace(":\n");
  trace("  token/text last read was '");
  trace(yytext);
  trace("'\n\n");


  if (filename != "<STDIN>")
  {
    trace("-------------------------------------------------------------------------------\n");
    
    // number of lines to print before and after errorneous line
    int environment = 4;

    unsigned int firstShowedLine = ((yylineno-environment)>0)?(yylineno-environment):1;
  
    std::ifstream inputFile(filename.c_str());
    std::string errorLine;
    for (unsigned int i=0; i<firstShowedLine; i++)
    {
      getline(inputFile, errorLine);
    }
    // print the erroneous line (plus/minus three more)
    for (unsigned int i=firstShowedLine; i<=firstShowedLine+(2*environment); i++)
    {
      trace(intToString(i) + ": " + errorLine + "\n");
      getline(inputFile, errorLine);
      if (inputFile.eof())
	break;
    }
    inputFile.close();
    
    trace("-------------------------------------------------------------------------------\n");
  }


  error();
  return 1;
}
