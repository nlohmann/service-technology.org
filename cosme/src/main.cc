#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <libgen.h>
#include "Label.h"
#include "Service.h"
#include "OperatingGuideline.h"
#include "cmdline.h"
#include "Output.h"
#include <pnapi/pnapi.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;

// lexer and parser
extern int og_yyparse();
extern int graph_yyparse();
extern FILE* og_yyin;
extern FILE* graph_yyin;

// the command line parameters
gengetopt_args_info args_info;
// the invocation string
string invocation;

Label GlobalLabels;
std::map <std::string, label_id_t> TransitionLabels;
std::set<label_id_t> OGInterface;
std::set<label_id_t> ServiceInterface;
std::map<og_service_index_t, OGMarking*> OGMarkings;
std::map<og_service_index_t, ServiceMarking*> ServiceMarkings;

pnapi::PetriNet tmpNet;

// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
      invocation += string(argv[i]) + " ";
    }

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
			cerr << PACKAGE << ": ERROR: invalid command-line parameter(s)" << endl;
    }

		// check what to do
		if ((args_info.matching_flag + args_info.simulation_flag + args_info.equivalence_flag) > 1) {
			cerr << PACKAGE << ": ERROR: don't know what to do (matching, simulation or equivalence)" << endl;
		}
		if ((args_info.matching_flag + args_info.simulation_flag + args_info.equivalence_flag) == 0) {
			cerr << PACKAGE << ": ERROR: don't know what to do (matching, simulation or equivalence)" << endl;
		}

    // check whether two files are given
    if (args_info.matching_flag) {
			if (!args_info.ServiceC_given or !args_info.OGA_given) {
				cerr << PACKAGE << ": ERROR: for matching a service and an operating guideline must be given" << endl;
			}
    }
		else {
			if (!args_info.OGA_given or !args_info.OGB_given) {
				cerr << PACKAGE << ": ERROR: for simulation/equivalence two operating guidelines must be given" << endl;
			}
		}
	
    free(params);
}

OperatingGuideline* parseOG(char* file) {
	og_yyin = fopen(file, "r");
	
	if(!og_yyin) // if an error occurred
  {
    cerr << PACKAGE << ": ERROR: failed to open input file '"
         << file << "'" << endl;
		return NULL;
  }
  
  // actual parsing
  og_yyparse();
  
  // close input (output is closed by destructor)
  fclose(og_yyin);

	OperatingGuideline *tmpOG = new OperatingGuideline(OGMarkings, OGInterface);
	OGMarkings.clear();
	OGInterface.clear();

	return tmpOG;
}

inline void parseService_LabelHelper() {

	GlobalLabels.mapLabel("Tau");
	label_id_t curLabel;
	std::pair<std::map<std::string, label_id_t>::iterator, bool> ret; 

	const std::set<pnapi::Place*> outputPlaces( tmpNet.getOutputPlaces() );
  for (std::set<pnapi::Place*>::const_iterator p = outputPlaces.begin(); p != outputPlaces.end(); ++p) {
		// save current !-event
		string tmpStr("!"); tmpStr += (*p)->getName();
		curLabel = GlobalLabels.mapLabel(tmpStr);
		ServiceInterface.insert(curLabel);

    const std::set<pnapi::Node*> preset( (*p)->getPreset() );
    for (std::set<pnapi::Node*>::const_iterator t = preset.begin(); t != preset.end(); ++t) {
      // save current Transitionlabel
			ret = TransitionLabels.insert(make_pair((*t)->getName(), curLabel));
			assert(ret.second);
    }
  }
 
  const std::set<pnapi::Place*> inputPlaces( tmpNet.getInputPlaces() );
  for (std::set<pnapi::Place*>::const_iterator p = inputPlaces.begin(); p != inputPlaces.end(); ++p) {
		// save current ?-event
    string tmpStr("?"); tmpStr += (*p)->getName();
		curLabel = GlobalLabels.mapLabel(tmpStr);
		ServiceInterface.insert(curLabel);

    const std::set<pnapi::Node*> postset( (*p)->getPostset() );
    for (std::set<pnapi::Node*>::const_iterator t = postset.begin(); t != postset.end(); ++t) {
    	ret = TransitionLabels.insert(make_pair((*t)->getName(), curLabel));
			assert(ret.second);
    }
  }

}

Service* parseService(char* file) {
	std::ifstream InputStream(file);
	if (!InputStream) {
		cerr << PACKAGE << ": ERROR: failed to open input file '"
         << file << "'" << endl;
		return NULL;
	}	

	// parse OWFN
	InputStream >> pnapi::io::owfn >> tmpNet;
	InputStream.close();

	//TODO: notwendig?	
	// "fix" the net in order to avoid parse errors from LoLA (see bug #14166)
  if (tmpNet.getTransitions().empty()) {
		cerr << PACKAGE << ": ERROR: net has no transitions" << endl;
		return NULL;
  }
  // only normal nets are supported so far
  if (!tmpNet.isNormal()) {
		cerr << PACKAGE << ": ERROR: the input open net must be normal" << endl;
		return NULL;
  } 
	
	parseService_LabelHelper();
	
	//TODO: ggf. allgemeinen Constructor nutzen
#if defined(__MINGW32__)
    Output temp(basename(args_info.tmpfile_arg), "");
#else
    Output temp(args_info.tmpfile_arg, "");
#endif

  temp.stream() << pnapi::io::lola << tmpNet;

  // select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    // MinGW does not understand pathnames with "/", so we use the basename
    string command_line = string(basename(args_info.lola_arg)) + " " + temp.name() + " -M" + (args_info.verbose_flag ? "" : " 2> nul");
#else
    string command_line = string(args_info.lola_arg) + " " + temp.name() + " -M" + (args_info.verbose_flag ? "" : " 2> /dev/null");
#endif

	graph_yyin = popen(command_line.c_str(), "r");
  graph_yyparse();
  pclose(graph_yyin);	

	Service *tmpService = new Service(ServiceMarkings, ServiceInterface);
	ServiceMarkings.clear();
	ServiceInterface.clear();

	return tmpService;

}

// main function
int main(int argc, char** argv)
{
	
	evaluateParameters(argc, argv);

	if (args_info.matching_flag) {
		Service* C;
		OperatingGuideline *A;

		C = parseService(args_info.ServiceC_arg);
		if (C == NULL) return EXIT_FAILURE;
		A = parseOG(args_info.OGA_arg);
		if (A == NULL) return EXIT_FAILURE;

		C->calculateBitSets(GlobalLabels);
		A->calculateBitSets(GlobalLabels);

		if (A->isMatching(*C)) 
			cout << "\nObjective completed\n" << endl;
		else
			cout << "\nObjective failed\n" << endl;

		delete A;
		delete C;
	}
	else if ((args_info.simulation_flag) or (args_info.equivalence_flag)) {
		OperatingGuideline *A, *B;

		A = parseOG(args_info.OGA_arg);
		if (A == NULL) return EXIT_FAILURE;
		B = parseOG(args_info.OGB_arg);
		if (B == NULL) return EXIT_FAILURE;
		
		if (args_info.simulation_flag) {
			if (A->isSimulation(*B))
				cout << "\nObjective completed\n" << endl;
			else
				cout << "\nObjective failed\n" << endl;
		}
		else {
			if (A->isEquivalent(*B))
				cout << "\nObjective completed\n" << endl;
			else
				cout << "\nObjective failed\n" << endl;
		}
		
		delete A;
		delete B;
	}

  return EXIT_SUCCESS;
}

