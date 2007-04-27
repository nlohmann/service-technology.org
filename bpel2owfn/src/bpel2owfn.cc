/*****************************************************************************\
 * Copyright 2007  Niels Lohmann, Christian Gierds, Martin Znamirowski       *
 * Copyright 2005, 2006  Niels Lohmann, Christian Gierds                     *
 *                                                                           *
 * This file is part of GNU BPEL2oWFN.                                       *
 *                                                                           *
 * GNU BPEL2oWFN is free software; you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * GNU BPEL2oWFN is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  *
 * Public License for more details.                                          *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with GNU BPEL2oWFN; see file COPYING. if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA. *
\*****************************************************************************/

/*!
 * \file    bpel2owfn.cc
 *
 * \brief   BPEL2oWFN's main
 *
 * \author  Niels Lohmann <nlohmann@informatik.hu-berlin.de>,
 *          Christian Gierds <gierds@informatik.hu-berlin.de>,
 *          Martin Znamirowski <znamirow@informatik.hu-berlin.de>,
 *          last changes of: \$Author: nielslohmann $
 *
 * \since   2005/10/18
 *
 * \date    \$Date: 2007/04/27 11:17:17 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 *
 * \version \$Revision: 1.163 $
 */





/******************************************************************************
 * Headers
 *****************************************************************************/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <map>

#include "bpel2owfn.h"          // generated configuration file

#include "petrinet.h"           // Petri Net support
#include "cfg.h"		// Control Flow Graph
#include "debug.h"		// debugging help
#include "options.h"
#include "ast-config.h"
#include "ast-details.h"
#include "globals.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::map;





/******************************************************************************
 * External functions
 *****************************************************************************/

extern int frontend_parse();			// from Bison
extern int frontend_debug;			// from Bison
extern int frontend_nerrs;			// from Bison
extern int frontend_lineno;			// from Bison
extern int frontend__flex_debug;		// from flex
extern FILE *frontend_in;			// from flex





/******************************************************************************
 * Global variables
 *****************************************************************************/

/// The Petri Net
PetriNet PN = PetriNet();

/******************************************************************************
 * program parts
 *****************************************************************************/


// analyzation of the commandline
void analyze_cl(int argc, char *argv[]) 
{

  // setting globals
  globals::program_name = string(argv[0]);

  for (int i = 0; i < argc; i++)
  {
    globals::invocation += string(argv[i]);
    if (i != (argc-1))
      globals::invocation += " ";
  }

  /*
   * Reading command line arguments and triggering appropriate behaviour.
   * In case of false parameters call command line help function and exit.
   */
  parse_command_line(argc, argv);

}




// opening a file
void open_file(set< string >::iterator file) 
{
  if (inputfiles.size() >= 1)
  {
    globals::filename = *file;
    if (!(frontend_in = fopen(globals::filename.c_str(), "r"))) 
    {
      cerr << "Could not open file for reading: " << globals::filename.c_str() << endl;
      exit(1);
    }
  }
}




// closing a file
void close_file(set< string >::iterator file) 
{
  if ( globals::filename != "<STDIN>" && frontend_in != NULL)
  {
    trace(TRACE_INFORMATION," + Closing input file: " + globals::filename + "\n");
    fclose(frontend_in);
    frontend_in = NULL;
  }
}




// finishing AST
void finish_AST() 
{
  // apply first set of rewrite rules
  trace(TRACE_INFORMATION, "Rewriting...\n");
  globals::AST = globals::AST->rewrite(kc::invoke);
  globals::AST = globals::AST->rewrite(kc::implicit);
  trace(TRACE_INFORMATION, "Rewriting complete...\n");

  // postprocess and annotate the AST
  trace(TRACE_INFORMATION, "Postprocessing...\n");
  globals::AST->unparse(kc::printer, kc::postprocessing);
  trace(TRACE_INFORMATION, "Postprocessing complete...\n");

  // apply second set of rewrite rules
  trace(TRACE_INFORMATION, "Rewriting 2...\n");
  globals::AST = globals::AST->rewrite(kc::newNames);
  trace(TRACE_INFORMATION, "Rewriting 2 complete...\n");

  // an experiment      
  //      cout << "digraph G{" << endl;
  //      globals::ASTEmap[1]->output();
  //      cout << "}" << endl;
}




// output of every single processed file
void single_output(set< string >::iterator file, PetriNet PN2) 
{
  // print the AST?
  if (modus == M_AST)
  {
    trace(TRACE_INFORMATION, "-> Printing AST ...\n");
    if (formats[F_DOT])
    {
      string dot_filename = globals::output_filename + "." + suffixes[F_DOT];
      FILE *dotfile = fopen(dot_filename.c_str(), "w+");
      globals::AST->fprintdot(dotfile, "", "", "", true, true, true);
      fclose(dotfile);
      #ifdef HAVE_DOT
      string systemcall = "dot -q -Tpng -o" + globals::output_filename + ".png " + globals::output_filename + "." + suffixes[F_DOT];
      trace(TRACE_INFORMATION, "Invoking dot with the following options:\n");
      trace(TRACE_INFORMATION, systemcall + "\n\n");
      system(systemcall.c_str());
      #endif
    }
    else
    globals::AST->print();
  }

  // print the Visualization?
  if (modus == M_VIS)
  {
    trace(TRACE_INFORMATION, "-> Printing the process visualization ...\n");
    if (formats[F_DOT])
    {
      if (globals::output_filename != "")
      {
        output = openOutput(globals::output_filename + "." + suffixes[F_DOT]);
      }
      trace(TRACE_INFORMATION, "-> Printing it in dot format ...\n");
      globals::AST->unparse(kc::printer, kc::visualization);
      if (globals::output_filename != "")
      {
        closeOutput(output);
        output = NULL;
      }
      #ifdef HAVE_DOT
      string systemcall = "dot -q -Tpng -o" + globals::output_filename + ".png " + globals::output_filename + "." + suffixes[F_DOT];
      trace(TRACE_INFORMATION, "Invoking dot with the following options:\n");
      trace(TRACE_INFORMATION, systemcall + "\n\n");
      system(systemcall.c_str());
      #endif
    }
    else
    globals::AST->print();
  }

  if (modus == M_PRETTY)
  {
    if (formats[F_XML])
    {
      if (globals::output_filename != "")
      {
        output = openOutput(globals::output_filename + "." + suffixes[F_XML]);
      }
      trace(TRACE_INFORMATION, "-> Printing \"pretty\" XML ...\n");
      globals::AST->unparse(kc::printer, kc::xml);
      if (globals::output_filename != "")
      {
        closeOutput(output);
        output = NULL;
      }
    }
  }

  // generate and process the control flow graph?
  if (modus == M_CFG)
  processCFG();

  // print information about the process
  show_process_information();

  // generate a Petri net?
  if (modus == M_PETRINET || modus == M_CONSISTENCY)
  {
    trace(TRACE_INFORMATION, "-> Unparsing AST to Petri net ...\n");
    // choose Petri net patterns
    if (globals::parameters[P_COMMUNICATIONONLY] == true)
    globals::AST->unparse(kc::pseudoPrinter, kc::petrinetsmall);
    else
    globals::AST->unparse(kc::pseudoPrinter, kc::petrinetnew);

    // calculate maximum occurences
    PN.calculate_max_occurrences();
    if (modus == M_CONSISTENCY)
    {
      unsigned int pos = file->rfind(".bpel", file->length());
      unsigned int pos2 = file->rfind("/", file->length());
      string prefix = "";
      if (pos == (file->length() - 5))
      {
        prefix = file->substr(pos2 + 1, pos - pos2 - 1) + "_";
      }

      if (globals::reduction_level > 0)
      {
	trace(TRACE_INFORMATION, "-> Structurally simplifying Petri Net ...\n");
	PN.reduce(globals::reduction_level);
      }

      PN.addPrefix(prefix);
      PN2.compose(PN);
      PN = PetriNet();

      globals::AST = NULL;
      }
    }
}




// Final Output of the result
void final_output( PetriNet PN2) 
{
  if (modus == M_CONSISTENCY)
    PN = PN2;

  if (modus == M_PETRINET || modus == M_CONSISTENCY)
  {
    if (globals::reduction_level > 0)
    {
      trace(TRACE_INFORMATION, "-> Structurally simplifying Petri Net ...\n");
      PN.reduce(globals::reduction_level);
    }

    // now the net will not change any more, thus the nodes are re-enumerated
    // and the maximal occurrences of the nodes are calculated.
    PN.reenumerate();
//  PN.calculate_max_occurrences();
    cerr << PN.information() << endl;


    // create oWFN output ?
    if (formats[F_OWFN])
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_OWFN]);
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net for oWFN ...\n");
      PN.set_format(FORMAT_OWFN);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;
      }
    }


    // create LoLA output ?
    if ( formats[F_LOLA] )
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_LOLA]);
      }
      if (modus == M_CONSISTENCY)
      {
	PN.makeChannelsInternal();
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net for LoLA ...\n");
      PN.set_format(FORMAT_LOLA);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;
      }

      if (modus == M_CONSISTENCY || modus == M_PETRINET)
      {
	if (globals::output_filename != "")
	{
	  output = openOutput(globals::output_filename + ".task");
	}
	string comment = "{ AG EF (";
	string formula = "FORMULA\n  ALLPATH ALWAYS EXPATH EVENTUALLY (";
	string andStr = "";

        set< Place * > finalPlaces = PN.getFinalPlaces();
	for (set< Place * >::iterator place = finalPlaces.begin(); place != finalPlaces.end(); place++)
	{
	  comment += andStr + (*place)->nodeFullName();
	  formula += andStr + (*place)->nodeShortName() + " > 0";
       	  andStr = " AND ";
	}
	comment += ") }";
	formula += ")";
	(*output) << comment << endl << endl;
	(*output) << formula << endl << endl;
	if (globals::output_filename != "")
	{
	  closeOutput(output);
	  output = NULL;
	}
      }
    }


    // create PNML output ?
    if ( formats[F_PNML] )
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_PNML]);
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net for PNML ...\n");
      PN.set_format(FORMAT_PNML);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;
      }
    }


    // create PEP output ?
    if ( formats[F_PEP] )
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_PEP]);
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net for PEP ...\n");
      PN.set_format(FORMAT_PEP);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;
      }
    }


    // create INA output ?
    if ( formats[F_INA] )
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_INA]);
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net for INA ...\n");
      PN.set_format(FORMAT_INA);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;
      }
    }

    // create SPIN output ?
    if ( formats[F_SPIN] )
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_SPIN]);
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net for SPIN ...\n");
      PN.set_format(FORMAT_SPIN);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;
      }
    }

    // create APNN output ?
    if ( formats[F_APNN] )
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_APNN]);
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net for APNN ...\n");
      PN.set_format(FORMAT_APNN);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;
      }
    }


    // create dot output ?
    if ( formats[F_DOT] )
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_DOT]);
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net for dot ...\n");
      // if parameter "nointerface" is set, set dot format with parameter "false"
      // not to draw an interface
      PN.set_format(FORMAT_DOT, !globals::parameters[P_NOINTERFACE]);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;

#ifdef HAVE_DOT
	string systemcall = "dot -q -Tpng -o" + globals::output_filename + ".png " + globals::output_filename + "." + suffixes[F_DOT];
	trace(TRACE_INFORMATION, "Invoking dot with the following options:\n");
	trace(TRACE_INFORMATION, systemcall + "\n\n");
	system(systemcall.c_str());
#endif
      }
    }


    // create info file ?
    if ( formats[F_INFO] )
    {
      if (globals::output_filename != "")
      {
	output = openOutput(globals::output_filename + "." + suffixes[F_INFO]);
      }
      trace(TRACE_INFORMATION, "-> Printing Petri net information ...\n");
      PN.set_format(FORMAT_INFO);
      (*output) << PN;
      if (globals::output_filename != "")
      {
	closeOutput(output);
	output = NULL;
      }
    }
  }
}


/******************************************************************************
 * main() function
 *****************************************************************************/

/*!
 * \brief entry point of BPEL2oWFN
 *
 * Controls the behaviour of input and output.
 *
 * \param argc	number of command line arguments
 * \param argv	array with command line arguments
 *
 * \returns 0 if everything went well
 * \returns 1 if an error occurred
 *
 * \todo This function is definitly too long. It should be partitioned!
 */
int main( int argc, char *argv[])
{
  // initilization of variables
  PetriNet PN2 = PetriNet();

  // analyzation of the commandline
  analyze_cl(argc,argv);

  // parsing all inputfiles
  set< string >::iterator file = inputfiles.begin();
  do
  {
    open_file(file);

    show_process_information_header();

    // invoke Bison parser
    trace(TRACE_INFORMATION, "Parsing " + globals::filename + " ...\n");
    frontend_parse();
    trace(TRACE_INFORMATION, "Parsing of " + globals::filename + " complete.\n");

    globals::parsing = false;
    close_file(file);

    if (frontend_nerrs == 0)
    {
      finish_AST();
      single_output(file, PN2);
    }
    else /* parse error */
    {
      if (globals::AST == NULL)
      {
	genericError(104, "", toString(frontend_lineno), ERRORLEVEL_CRITICAL);

	cleanup();
    	return 1;
      }
      else
      {
  	genericError(105, "", toString(frontend_lineno), ERRORLEVEL_NOTICE);

	finish_AST();
	single_output(file, PN2);
      }
    }

    file++;

  } while (modus == M_CONSISTENCY && file != inputfiles.end());

  trace(TRACE_INFORMATION, "All files have been parsed.\n");

  final_output(PN2);

  if (debug_level != TRACE_ERROR)
    cerr << "==============================================================================" << endl << endl;

  // everything went fine
  return 0;
}





/*!
 * \defgroup frontend Front End
 * \defgroup patterns Petri Net Patterns
 */
