#include <config.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <map>
#include <algorithm>
#include "Node.h"
#include "Graph.h"
#include "helpers.h"
#include "cmdline.h"
#include "verbose.h"
#include "config-log.h"
#include "time.h"
#include "Output.h"

#include "Formula.h"

/// the command line parameters
gengetopt_args_info args_info;


using namespace std;

string invocation;

// lexer and parser
extern int og_yyparse();
extern FILE* og_yyin;
extern int og_yylex_destroy();

extern Graph * graph;

extern map<string, int> label2id;
extern map<int, string> id2label;

extern int currentIdPos;
extern int firstLabelId; //all labels including tau
extern int firstInputId; //input labels
extern int firstOutputId;//output labels

extern int lastLabelId;
extern int lastInputId;
extern int lastOutputId;

void initGlobalVariables();
void evaluateParameters(int argc, char** argv);
bool fileExists(string filename);

int main(int argc, char **argv) {

	evaluateParameters(argc, argv);

	if (args_info.inputs_num > 1) {
		//fehler
	}

	string filename = "stdin";

	if (args_info.inputs_num == 1){
		assert(args_info.inputs != NULL);
		assert(args_info.inputs[0] != NULL);

		filename = args_info.inputs[0];
		filename = filename.substr(0,filename.find_last_of(".og")-2);
		og_yyin = fopen(args_info.inputs[0], "r");

	    if (og_yyin == NULL){
	    	abort(1, "File %s not found", args_info.inputs[0]);
	    }

	}

	Graph * g = new Graph();
	graph = g;

	initGlobalVariables();

	time_t parsingTime_start = time(NULL);

    //parse
    if ( og_yyparse() != 0) {cout << PACKAGE << "\nparse error\n" << endl; exit(1);}

    fclose(og_yyin);
    og_yylex_destroy();



    if(args_info.time_given){
    	time_t parsingTime_end = time(NULL);
    	cout << "number of labels: " << label2id.size()-3 << endl;
    	cout << "number of nodes in the given extended annotated automaton: " << graph->nodes.size() << endl;
    	cout << difftime(parsingTime_end, parsingTime_start) << " s consumed for parsing the file" << endl;
    }

    time_t buildOG_start = time(NULL);

	g->complement();

	if(args_info.time_given){
		time_t buildOG_end = time(NULL);
		cout << "number of nodes in the complement: " << graph->nodes.size() + graph->getSizeOfAddedNodes() << endl;
		cout << difftime(buildOG_end, buildOG_start) << " s consumed for building the complement" << endl;
		//cout << Formula::getMinisatTime() << "s consumed by minisat" << endl;
	}

	bool printToStdout = true;
	for (unsigned int j = 0; j<args_info.output_given; ++j){
		switch(args_info.output_arg[j]) {

			// create output using Graphviz dot
			case (output_arg_png):
			case (output_arg_eps):
			case (output_arg_pdf): {
				printToStdout = false;
				if (CONFIG_DOT == "not found") {
					cerr << PACKAGE << ": Graphviz dot was not found by configure script; see README" << endl;
					cerr << "       necessary for option '--output'" << endl;
					exit(EXIT_FAILURE);
				}

				string call = string(CONFIG_DOT) + " -T" + args_info.output_orig[j] + " -q -o " + filename + "_complement." + args_info.output_orig[j];
				FILE *s = popen(call.c_str(), "w");
				assert(s);

				stringstream o;
				g->getGlobalFormulaForComplement(o);

				string title = "Complement of " + filename + "    global formula: " + string(o.str());
				g->toDot(s, title);
				assert(!ferror(s));

				pclose(s);
				break;
			}
			case (output_arg_dot): {
				printToStdout = false;
				FILE *s = fopen((filename+"_complement.dot").c_str(), "w");
				stringstream o;
				g->getGlobalFormulaForComplement(o);

				string title = "Complement of " + filename + "    global formula: " + string(o.str());
				g->toDot(s, title);
				assert(!ferror(s));
				fclose(s);
				break;
			}

			case (output_arg_eaa): {
				printToStdout = false;
				Output o(filename+"_complement.eaa", "complement OG");
				g->printComplement(o);
				break;
			}
			case (output_arg_none): {
				printToStdout = false;
				break;
			}
		}
	}

	if(printToStdout){
		Output o("-", "complement OG");
		g->printComplement(o);
	}

	delete g;

//	free(args_info.minisat_arg);
	cmdline_parser_free(&args_info);
    return EXIT_SUCCESS;
}

void initGlobalVariables(){
    label2id.clear();
    id2label.clear();

    addLabel("", 0); //0 has a special meaning in minisat, therefore 0 cannot use as label ID
    addLabel("final", 1);
    addLabel("tau", 2);

    //	currentIdPos = 1;  //if there is no tau in the OG
	currentIdPos = 2;
	firstLabelId = 2;

}


//bool fileExists(std::string filename) {
//    FILE *tmp = fopen(filename.c_str(), "r");
//    if (tmp) {
//        fclose(tmp);
//        return true;
//    } else {
//        return false;
//    }
//}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {

/*for gengetopt: begin*/
    // overwrite invokation for consistent error messages
    argv[0] = (char*)PACKAGE;

/*
    // debug option
    if (argc > 0 and std::string(argv[1]) == "--bug") {
        FILE *debug_output = fopen("bug.log", "w");
        fprintf(debug_output, "%s\n", CONFIG_LOG);
        fclose(debug_output);
        fprintf(stderr, "Please send file 'bug.log' to %s.\n", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }
*/

    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }

    // set default values
    //cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

//    StoredKnowledge::reportFrequency = args_info.reportFrequency_arg;

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }


    // check the message bound
//    if ((args_info.messagebound_arg < 1) or (args_info.messagebound_arg > UINT8_MAX)) {
//        abort(9, "message bound must be between 1 and %d", UINT8_MAX);
//    }

    free(params);
}

