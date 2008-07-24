/*****************************************************************************\
  GNU BPEL2oWFN -- Translating BPEL Processes into Petri Net Models

  Copyright (C) 2006, 2007  Niels Lohmann,
                            Christian Gierds, and
                            Martin Znamirowski
  Copyright (C) 2005        Niels Lohmann and
			    Christian Gierds

  GNU BPEL2oWFN is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 3 of the License, or (at your option) any
  later version.

  GNU BPEL2oWFN is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  GNU BPEL2oWFN (see file COPYING); if not, see http://www.gnu.org/licenses
  or write to the Free Software Foundation,Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301  USA.
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
 * \date    \$Date: 2007/06/28 07:38:15 $
 *
 * \note    This file is part of the tool BPEL2oWFN and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 * \version \$Revision: 1.185 $
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

#include "uml2owfn.h"          // generated configuration file
#include "petrinet.h"           // Petri Net support
#include "debug.h"		// debugging help
#include "options.h"
#include "globals.h"
#include "helpers.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::map;





/******************************************************************************
 * External functions
 *****************************************************************************/
extern int frontend_parse();		// from Bison
extern int frontend_debug;			// from Bison
extern int frontend_nerrs;			// from Bison
extern int frontend_lineno;			// from Bison
extern int frontend__flex_debug;		// from flex
extern FILE *frontend_in;			// from flex
extern void frontend_restart(FILE*);		// from flex




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

  // Reading command line arguments and triggering appropriate behaviour.
  // In case of false parameters call command line help function and exit.
  parse_command_line(argc, argv);

}



// opening a file
void open_file(string file) 
{
  if (inputfiles.size() >= 1)
  {
    globals::filename = file;
    if (!(frontend_in = fopen(globals::filename.c_str(), "r"))) 
    {
      cerr << "Could not open file for reading: " << globals::filename.c_str() << endl;
      exit(2);
    }
  }
}


// closing a file
void close_file(string file) 
{
  if ( globals::filename != "<STDIN>" && frontend_in != NULL)
  {
    trace(TRACE_INFORMATION," + Closing input file: " + globals::filename + "\n");
    fclose(frontend_in);
    frontend_in = NULL;
  }
}


// Final Output of one result
void final_output()
{

  // jump into Thomas Heidinger's part  
  if (globals::reduction_level > 0)
  {
    trace(TRACE_INFORMATION, "-> Structurally simplifying Petri Net ...\n");
    PN.reduce(globals::reduction_level);
  }
       
// <Dirk.F start> -- moved from output with F_LOLA
  // prepare the LoLA output to suit soundness-checking
  if (globals::parameters[P_SOUNDNESS])
  {
  	// transform input interface places to initially marked places
    PN.fix_lola_output();
    // add a post-transition to the final places and the output interface places
    PN.mergeFinalStates();	
  }
// <Dirk.F end>
    
    // now the net will not change any more, thus the nodes are re-enumerated
    // and the maximal occurrences of the nodes are calculated.
// <Dirk.F start>
    //DO NOT RE-ENUMERATE PLACES
    //PN.reenumerate();
// <Dirk.F end>
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
	  trace(TRACE_INFORMATION, "-> Printing Petri net for LoLA ...\n");
	  PN.set_format(FORMAT_LOLA);

	  (*output) << PN;
	  if (globals::output_filename != "")
	  {
		  closeOutput(output);
		  output = NULL;
	  }

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
	  PN.set_format(FORMAT_DOT, true);
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



/******************************************************************************
 * main() function
 *****************************************************************************/

/*!
 * \brief entry point of UML2OWFN
 *
 * Controls the behaviour of input and output.
 *
 * \param argc	number of command line arguments
 * \param argv	array with command line arguments
 *
 * \returns 0 if everything went well
 * \returns 1 if an error occurred
 *
 * \todo The opening and closing of files is awkward.
 *
 * \bug  Piping does not always work...
 */
int main( int argc, char *argv[])
{


  // analyzation of the commandline
  analyze_cl(argc,argv);


  // parsing all inputfiles
  set< string >::iterator file = inputfiles.begin();
  do
  {
    if (file != inputfiles.end())
    {
      open_file(*file);

      // reset the parser
      frontend_restart(frontend_in);      
    } else {
        return 0;
    }

    // invoke BPEL Bison parser
    trace(TRACE_INFORMATION, "Parsing " + globals::filename + " ...\n");
    int parse_result = frontend_parse();
    trace(TRACE_INFORMATION, "Parsing of " + globals::filename + " complete.\n");
    
    
    if (file != inputfiles.end())
      close_file(*file);
    if (frontend_nerrs == 0)
    {
    int i = 0;
    cerr << "Es existieren nach dem parsen noch " << globals::processes.size() << " Prozesse!\n\n";
    for(set<Block*>::iterator process = globals::processes.begin(); process != globals::processes.end(); process++)
    {
        // set the name variables of all blocks within this process to the parsed
    	// values
    	(*process)->transferName();
        
    	// link the information of processes, tasks and services that are referenced
    	// by "callto" blocks into the callto blocks and then tests if there
    	// are overlapping input and/or output sets
    	(*process)->linkInserts();
    	
    	//if the process was filtered, dont create an output at all
    	if ((*process)->filtered)
    	{
    		continue;
    	}
    	
    	// check whether all connections can find their starting and ending pins
        (*process)->linkNodes();
        
    	if ((*process)->filtered)
    	{
    		continue;
    	}

        // DECOMPOSITION
        // Decomposition is currently not in use, due to
        // a missing algorithm that would allow to turn a
        // structurally decomposed net into a meaningful process

        // Testroles for cutting
        // set<string> testStrings;
        // testStrings.insert("Hmn32Rls##Customer");        

        // Functions to spread roles, cut the net and
        // turn it into a working process (incomplete)
        // (*process)->adjustRoles();
        // (*process)->cutNet(testStrings, 2);
        // (*process)->disableStart();        
        // (*process)->disableEnd();        
        
        // Generates a petri net from the current process
        PetriNet* tryPN = new PetriNet();
        (*process)->returnNet(tryPN);

        // DECOMPOSITION
        // Creating an end state for the decomposed oWFN
        //(*process)->createEndState(tryPN);

        // Delete places in the net that represent unused pins
        // of tasks in the business object model
        tryPN->fixIBMNet();

    	// fix names of places such that they can be read by 
        // an owfn/lola parser after output
        tryPN->fixPlaceNames();


        // if the process was successfully transformed into a petrinet
        if (tryPN != NULL)
        {
            PN = (*tryPN);

            // Modify the process's name deleting some symbols
            // such that the outputfile
            // can be easily read by most programs.
            string modifiedProcessName;
            modifiedProcessName.append((*process)->name);
            while ( modifiedProcessName.find("#",0) != string::npos)
            {
                int deletePos;
                deletePos = modifiedProcessName.find_first_of("#",0);
                modifiedProcessName.erase(deletePos,1);
            }

            while ( modifiedProcessName.find(" ",0) != string::npos)
            {
                int deletePos;
                deletePos = modifiedProcessName.find_first_of(" ",0);
                modifiedProcessName.erase(deletePos,1);
            }

            cerr << "\n\"" << (*process)->name << "\" wurde übersetzt und wird nun ausgegegeben!\n"; 
            
            // generate the output for the given process
            globals::output_filename = globals::filename + "." + modifiedProcessName;
            globals::currentProcessName = (*process)->name;
            final_output();
            i++;
        }
        
    }
      
    }

    // clear all variables and prepare for input of the next xml file
    globals::processes.clear();      
    globals::tasks.clear();      
    globals::services.clear();      

    file++;

  } while (file != inputfiles.end());

  trace(TRACE_INFORMATION, "All files have been parsed.\n");

  // everything went fine
  return 0;

}





/*!
 * \defgroup frontend Front End
 * \defgroup patterns Petri Net Patterns
 */
