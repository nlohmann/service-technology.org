/*****************************************************************************\
 Hello -- <<-- Hello World -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Hello is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Hello is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/* <<-- CHANGE START (main program) -->> */
// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>

#include <pnapi/pnapi.h>
#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"

#include "TaraHelpers.h"
#include "ReachabilityGraph.h"
#include "Dimension.h"
#include "Condition.h"
#include "DFA.h"
#include "CostGraph.h"
#include "Multiset.h"
#include "KnowledgeGraph.h"
#include "Policy.h"

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::ofstream;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

// lexer and parser
extern int rg_yyparse();
extern int rg_yylex_destroy();
extern FILE* rg_yyin;

/// output stream
std::stringstream outStream;

/// Reachability graph coming from LoLA
ReachabilityGraph rg;

// Name for dot output file
std::string filename;

/// a variable holding the time of the call
clock_t start_clock = clock();


/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (args_info.bug_flag) {
        {
            Output debug_output("bug.log", "configuration information");
            debug_output.stream() << CONFIG_LOG << std::flush;
        }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file(args_info.config_arg, &args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", args_info.config_arg);
        } else {
            status("using configuration file '%s'", args_info.config_arg);
        }
    } else {
        // check for configuration files
        std::string conf_generic_filename = std::string(PACKAGE) + ".conf";
        std::string conf_filename = fileExists(conf_generic_filename) ? conf_generic_filename :
                                    (fileExists(std::string(SYSCONFDIR) + "/" + conf_generic_filename) ?
                                     (std::string(SYSCONFDIR) + "/" + conf_generic_filename) : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file(const_cast<char*>(conf_filename.c_str()), &args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }


    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one reachability graph must be given");
    }

    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    /* [USER] Add code here */

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        int i = system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}


/// main-function
int main(int argc, char** argv) {
    
    time_t start_time, end_time;

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);


    /*--------------------------------------.
    | 1. parse the reachability graph       |
    `--------------------------------------*/

      
  // set input source
  if(args_info.inputs_num > 0) // if user set an input file
  {
    // open file and link input file pointer
    rg_yyin = fopen(args_info.inputs[0], "r");
    if(!rg_yyin) // if an error occurred
    {
      cerr << PACKAGE << ": ERROR: failed to open input file '"
           << args_info.inputs[0] << "'" << endl;
      exit(EXIT_FAILURE);
    }
    filename = args_info.inputs[0];
  }
  else {
    filename = "stdin";
  }
 
  /// actual parsing
  rg_yyparse();
  
  // close input (output is closed by destructor)
  fclose(rg_yyin);

  /// clean lexer memory
  rg_yylex_destroy();    
      
  rg.print();
  
  
/*    
  Condition c;
  for (int i = rg.places.size()-3; i >= 0;--i)
    c.constraints[i] = std::pair<int,int>(0,0);
  cerr << rg.places[1] << c.satisfies(19,rg) << endl;
  rg.removeIndifferents(c);
  rg.print();
*/
  
  for (int i = 0; i < args_info.dfa_given; ++i) {  
    int last = TaraHelpers::insertDFA(new DFA(std::string(args_info.dfa_arg[i]))); 
  }
  std::vector<std::pair<int,int> > bounds;
  
  for (int i = 0; i < args_info.costfunction_given; ++i) {  
    int last = TaraHelpers::insertCostFunction(new StandardCostFunction(std::string(args_info.costfunction_arg[i])));
    bounds.push_back(std::pair<int,int>(-60,60)); 
  }
  
  
  
  
  
  
  // Parsing the open net, using the PNAPI
		status("Processing %s", args_info.net_arg);
		pnapi::PetriNet* net = new pnapi::PetriNet;

		try {

			// parse either from standard input or from a given file
				
				std::ifstream inputStream;
				inputStream.open(args_info.net_arg);
				inputStream >> pnapi::io::owfn >> *(net);
				inputStream.close();

			std::stringstream pnstats;
			pnstats << pnapi::io::stat << *(net);

			message("read net %s", pnstats.str().c_str());
		} catch (pnapi::exception::InputError error) {
			std::stringstream inputerror;
			inputerror << error;
			abort(3, "pnapi error %i", inputerror.str().c_str());
		}	

   
     
  // Insert labels and assign them to transitions
  for (std::set<pnapi::Transition*>::iterator it = net->getTransitions().begin(); it !=   net->getTransitions().end(); ++it) {
  
    int curTransition = TaraHelpers::insertTransition((*it)->getName());
    std::set<pnapi::Label*> inLabels = (*it)->getInputLabels();
    std::set<pnapi::Label*> outLabels = (*it)->getOutputLabels();
    for (std::set<pnapi::Label*>::iterator it2 = inLabels.begin(); it2 != inLabels.end(); ++it2) {
      int curLabel = TaraHelpers::insertLabel("?" + (*it2)->getName());
      TaraHelpers::setLabel(curTransition, curLabel);
    }
    for (std::set<pnapi::Label*>::iterator it2 = outLabels.begin(); it2 != outLabels.end(); ++it2) {
      int curLabel = TaraHelpers::insertLabel("!" + (*it2)->getName());
      TaraHelpers::setLabel(curTransition, curLabel);
    }
  } 
  
  

  std::vector<Policy*> policies;

  for (int i = 0; i < args_info.dfa_given; ++i) {  
	policies.push_back(new SimplePolicy(args_info.pol_arg[i], &rg)); 
 }
 
  CostGraph cg = CostGraph(rg, policies);
  cg.print();
  /*Situation s;
  s.state = 0;
  std::set<Situation> S;
  S.insert(s);
  cg.closure(S);*/
  KnowledgeGraph kg = KnowledgeGraph(&cg);
  kg.print();
  kg.reflag();
  kg.printToDot();
/*
  // set output destination
  if(args_info.output_given){
     //Append format suffix to file name
     filename = filename + "." + args_info.output_arg;

  } else {
    
    // todo: print to command line
  
  }
*/
    return EXIT_SUCCESS;
}
/* <<-- CHANGE END -->> */
