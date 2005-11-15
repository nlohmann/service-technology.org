/*!
 * \file main.cc
 *
 * \brief The control component for BPEL2oWFN
 *
 * This file controls the behaviour of BPEL2oWFN and is the interface to the environment.
 * 
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2005/10/18
 *          - last changed: \$Date: 2005/11/15 15:47:24 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.25 $
 *          - 2005-11-15 (gierds) Moved command line evaluation to helpers.cc.
 *            Added option to created (abstracted) low level nets.
 *            Added option for LoLA output.
 *
 */

#include "main.h"

/// The Petri Net
PetriNet *TheNet = new PetriNet();

// some file names and pointers

/// Filename of input file.
std::string filename = "";

/// Filename of dot output file
std::string dot_filename = "";
/// Pointer to dot output file
std::ostream * dot_output = &std::cout;

/// Filename of lola output file
std::string lola_filename = "";
/// Pointer to lola output file
std::ostream * lola_output = &std::cout;

// different modes controlled by command line

/// print the Petri Net
bool mode_petri_net = false;
/// simplify Petri Net
bool mode_simplify_petri_net = false;
/// paint Petri Net with lola
bool mode_lola_petri_net = false;
/// paint Petri Net with lola and output to file
bool mode_lola_2_file = false;
/// paint Petri Net with dot
bool mode_dot_petri_net = false;
/// paint Petri Net with dot and output to file
bool mode_dot_2_file = false;
/// pretty printer
bool mode_pretty_printer = false;
/// print AST
bool mode_ast = false;
/// print the "low level" Petri Net
bool mode_low_level_petri_net = false;


/**
 * Entry point for BPEL2oWFN.
 * Controls the behaviour of input and output.
 *
 * \parameter argc	number of command line arguments
 * \parameter argv	array with command line arguments
 *
 * \returns Error code (0 if everything went well)
 *
 * \todo
 * 	- outsource error handling and big switch for exception_id 
 */
int main( int argc, char *argv[])
{
	
  try
  {
    /***
     * Reading command line arguments and triggering appropriate behaviour.
     * In case of false parameters call command line help function and exit.
     */
    parse_command_line(argc, argv);
  	
    trace(TRACE_INFORMATION, "Parsing ...\n");
  
    // invoke Bison parser
    int error = yyparse();

    if (!error)
    {
      trace(TRACE_INFORMATION, "Parsing complete.\n");
      if (mode_ast)
      {
        TheProcess->print();
      }
    
      trace(TRACE_INFORMATION, "Rewriting BPEL...\n");
      TheProcess = TheProcess->rewrite(kc::implicit);
      trace(TRACE_INFORMATION, "Rewriting complete.\n");
    
      if (mode_pretty_printer)
      {
        TheProcess->unparse(kc::printer, kc::xml);
      }
      if (mode_petri_net)
      {
        TheProcess->unparse(kc::pseudoPrinter, kc::petrinet);
      }
    }

    // make low level ?
    if (mode_low_level_petri_net)
    {
      TheNet->makeLowLevel();
    }
    // simplify net ?
    if (mode_simplify_petri_net)
    {
      TheNet->simplify();
    }
    // create dot output ?
    if (mode_lola_petri_net)
    {
      TheNet->lolaOut();
    }
    // create dot output ?
    if (mode_dot_petri_net)
    {
      TheNet->drawDot();
    }

  
    if (lola_filename != "")
    {
      trace(TRACE_INFORMATION,"Closing LoLA output file: " + lola_filename + "\n");
      (*lola_output) << std::flush;
      ((std::ofstream*)lola_output)->close();
    }
  
    if (dot_filename != "")
    {
      trace(TRACE_INFORMATION,"Closing dot output file: " + dot_filename + "\n");
      (*dot_output) << std::flush;
      ((std::ofstream*)dot_output)->close();
    }

    return error;
  }
  catch (Exception e)
  {

    trace("\nException occured!\n\n");	    

    trace("Number:\n    ");
    trace(""+ (int) e.id);
    trace("\n");

    trace("Error: \n    " + e.error + "\n");
    if (e.information != "")
    {
      trace("Addtional information:\n    " + e.information + "\n");
    }

    exit(e.id);
  }

  return 0;  
}
