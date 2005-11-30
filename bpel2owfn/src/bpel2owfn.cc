/*!
 * \file main.cc
 *
 * \brief The control component for BPEL2oWFN
 *
 * This file controls the behaviour of BPEL2oWFN and is the interface to the 
 * environment.
 * 
 * \author  
 *          - responsible: Christian Gierds <gierds@informatik.hu-berlin.de>
 *          - last changes of: \$Author: gierds $
 *          
 * \date
 *          - created: 2005/10/18
 *          - last changed: \$Date: 2005/11/30 09:00:16 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.37 $
 *          - 2005-11-15 (gierds) Moved command line evaluation to helpers.cc.
 *            Added option to created (abstracted) low level nets.
 *            Added option for LoLA output.
 *          - 2005-11-15 (nlohmann) Call Exception::info() to signal error.
 *          - 2005-11-16 (gierds) Use of error() and cleanup() as defined in 
 *            helpers.cc
 *
 */

#include "main.h"

/// The Petri Net
PetriNet *TheNet = new PetriNet();

// some file names and pointers

/// Filename of input file.
std::string filename = "<STDIN>";

/// Filename of dot output file
std::string dot_filename = "";
/// Pointer to dot output file
std::ostream * dot_output = &std::cout;

/// Filename of lola output file
std::string lola_filename = "";
/// Pointer to lola output file
std::ostream * lola_output = &std::cout;

/// Filename of PN info file
std::string info_filename = "";
/// Pointer to PN info file
std::ostream * info_output = &std::cout;

// different modes controlled by command line

/// read from file
bool mode_file = false;
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
 * \param argc	number of command line arguments
 * \param argv	array with command line arguments
 *
 * \returns Error code (0 if everything went well)
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
      
      symMan.printScope();
      
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
    // create LoLA output ?
    if (mode_lola_petri_net)
    {
      TheNet->lolaOut();
    }
    // create dot output ?
    if (mode_dot_petri_net)
    {
      TheNet->drawDot();
    }    
    // output info file ?
    if (mode_petri_net && mode_file )
    {
      TheNet->printInformation();
    }


    cleanup();  

    return error;
  }
  catch (Exception& e)
  {
    // output info file ?
    if (mode_petri_net && mode_file )
    {
      // in case of an exception, the Petri Net information might help us!
      TheNet->printInformation();
    }
    error(e);
  }

  return 0;  
}
