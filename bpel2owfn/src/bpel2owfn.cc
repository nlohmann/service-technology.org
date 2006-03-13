/*****************************************************************************\
 * Copyright 2005, 2006 Niels Lohmann, Christian Gierds, Dennis Reinert      *
 *                                                                           *
 * This file is part of BPEL2oWFN.                                           *
 *                                                                           *
 * BPEL2oWFN is free software; you can redistribute it and/or modify it      *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT  *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with BPEL2oWFN; if not, write to the Free Software Foundation, Inc., 51   *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
\****************************************************************************/

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
 *          - last changes of: \$Author: nlohmann $
 *          
 * \date
 *          - created: 2005/10/18
 *          - last changed: \$Date: 2006/03/13 09:36:59 $
 * 
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.61 $
 *          - 2005-11-15 (gierds) Moved command line evaluation to helpers.cc.
 *            Added option to created (abstracted) low level nets.
 *            Added option for LoLA output.
 *          - 2005-11-15 (nlohmann) Call Exception::info() to signal error.
 *          - 2005-11-16 (gierds) Use of error() and cleanup() as defined in 
 *            helpers.cc
 *          - 2005-12-13 (gierds) Added variables in order to handle creation
 *            of oWFNs.  
 *
 */

#include "main.h"
#include "options.h"

/// The Petri Net
PetriNet *TheNet = new PetriNet();

/// The CFG
CFGBlock * TheCFG = NULL;


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
      if (modus == M_AST)
      {
	trace(TRACE_INFORMATION, "-> Printing AST ...\n");
        TheProcess->print();
      }
        
      if (modus == M_PRETTY)
      {
	if (formats[F_XML])
	{
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + "." + suffixes[F_XML]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing \"pretty\" XML ...\n");
          TheProcess->unparse(kc::printer, kc::xml);
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
	}
      }
     
      // create CFG
      if (modus == M_CFG || modus == M_PETRINET)
      {
        trace(TRACE_INFORMATION, "-> Unparsing AST to CFG ...\n");
        TheProcess->unparse(kc::pseudoPrinter, kc::cfg);
	// do some business with CFG
      }

      if (modus == M_CFG)
      {
	if (formats[F_DOT])
	{
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + ".cfg." + suffixes[F_DOT]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing CFG in dot ...\n");
          // output CFG;
  	  cfgDot(TheCFG);
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
	}
      }

      // delete CFG
      if (modus == M_CFG || modus == M_PETRINET)
      {
	delete(TheCFG);
      }

      if (modus == M_PETRINET)
      {
        trace(TRACE_INFORMATION, "-> Unparsing AST to Petri net ...\n");
        TheProcess->unparse(kc::pseudoPrinter, kc::petrinet);

	// remove isolated nodes
	TheNet->removeIsolatedNodes();
	
        // remove variables?
 	if ( parameters[P_NOVARIABLES] )
        {
 	  trace(TRACE_INFORMATION, "-> Remove variable places from Petri Net ...\n");
          TheNet->removeVariables();
        }    
	// simplify net ?
 	if ( parameters[P_SIMPLIFY] )
        {
 	  trace(TRACE_INFORMATION, "-> Structurally simplifying Petri Net ...\n");
          TheNet->simplify();
        }    
        // create LoLA output ?
        if ( formats[F_LOLA] )
        {
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + "." + suffixes[F_LOLA]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing Petri net for LoLA ...\n");
          TheNet->lolaOut();
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
        }
        // create PNML output ?
        if ( formats[F_PNML] )
        {
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + "." + suffixes[F_PNML]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing Petri net for PNML ...\n");
          TheNet->pnmlOut();
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
        }
        // create PEP output ?
        if ( formats[F_PEP] )
        {
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + "." + suffixes[F_PEP]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing Petri net for PEP ...\n");
          TheNet->pepOut();
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
        }
        // create APNN output ?
        if ( formats[F_APNN] )
        {
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + "." + suffixes[F_APNN]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing Petri net for APNN ...\n");
          TheNet->apnnOut();
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
        }
        // create oWFN output ?
        if ( formats[F_OWFN] )
        {
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + "." + suffixes[F_OWFN]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing Petri net for oWFN ...\n");
          TheNet->owfnOut();
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
        }
        // create dot output ?
        if ( formats[F_DOT] )
        {
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + "." + suffixes[F_DOT]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing Petri net for dot ...\n");
          TheNet->dotOut();
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
        }
        // create info file ?
        if ( formats[F_INFO] )
        {	
	  if (output_filename != "")
	  {
 	    output = openOutput(output_filename + "." + suffixes[F_INFO]);
	  }
 	  trace(TRACE_INFORMATION, "-> Printing Petri net information ...\n");
          TheNet->printInformation();
	  if (output_filename != "")
	  {
	    closeOutput(output);
	    output = NULL;
	  }
        }

      }

    }

    cleanup();  

    return error;
  }
  catch (Exception& e)
  {
    // output info file ?
    if ( formats[F_INFO] )
    {
      output = log_output;
      // just print to std::out
      TheNet->printInformation();
      output = NULL;
    }
    error(e);
  }

  return 0;  
}
