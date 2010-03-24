/*****************************************************************************\
 Safira -- Implementing a Set Algebra for Service Behavior

 Copyright (c) 2010 Kathrin Kaschner

 Safira is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Safira is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Safira.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <config.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <algorithm>
#include "Node.h"
#include "Graph.h"
#include "GraphComplement.h"
#include "GraphIntersection.h"
#include "helpers.h"
#include "cmdline.h"
#include "verbose.h"
#include "config-log.h"
#include "time.h"
#include "Output.h"
#include "Node.h"

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

map<string, int> label2id;
map<int, string> id2label;
map<int, char> inout;

int firstLabelId; //all labels including tau
int lastLabelId;

string generateFilename(int filenumber);
void parseFile(int filenumber);
void initGlobalVariables();
void out(Graph * g, string filename, string title);
void printUsedMemory();
void evaluateParameters(int argc, char** argv);


int main(int argc, char **argv) {

	evaluateParameters(argc, argv);
	Output::setTempfileTemplate(args_info.tmpfile_arg);
	Output::setKeepTempfiles(args_info.noClean_flag);

	initGlobalVariables();

	/******************
	 * Complement     *
	 ******************/
	if(args_info.complement_given){

		GraphComplement * g = new GraphComplement();
		graph = g;

		time_t parsingTime_start = time(NULL);

		parseFile(0);

		if(args_info.time_given){
			time_t parsingTime_end = time(NULL);
			cout << label2id.size()-3 << " labels" << endl;
			cout << graph->nodes.size() << " nodes in the given extended annotated automaton"  << endl;
			cout << difftime(parsingTime_end, parsingTime_start) << " s consumed for parsing the file" << endl << endl;
			printUsedMemory(); cout << "(after parsing)" << endl;
		}

		time_t buildOG_start = time(NULL);
		clock_t buildOG_start_clock = clock();

		g->complement();

		if(args_info.time_given){
			time_t buildOG_end = time(NULL);
			cout << g->nodes.size() + g->addedNodes.size() << " nodes in the complement"  << endl;
			cout << difftime(buildOG_end, buildOG_start) << " s consumed for building the complement" << endl;
			cout << (static_cast<double>(clock()) - static_cast<double>(buildOG_start_clock)) / CLOCKS_PER_SEC << " s consumed for building the complement" << endl;

			cout << Formula::getMinisatTime() << " s consumed by minisat" << endl;
			cout << GraphComplement::getTime() << " s consumed for building and using the decision tree" << endl;
			cout << "--------------------------------------" << endl;
			printUsedMemory(); cout << "(after calculation)" << endl;

		}

		string filename = generateFilename(0);
		string title = "Complement of " + filename + "    global formula: " + g->globalFormulaAsString;
		out(g, filename+"_complement",title);

		delete g;
	}


	/******************
     * Intersection   *
	 ******************/
	if(args_info.intersection_given){

		Graph * g1 = new Graph();
		graph = g1;

		time_t parsingTime_start = time(NULL);

		parseFile(0); //parse first file

		if(args_info.time_given){
			time_t parsingTime_end = time(NULL);
			cout << "--------------------------------------" << endl;
			cout << g1->nodes.size() << " nodes in the given extended annotated automaton (1st file)"  << endl;
			cout << difftime(parsingTime_end, parsingTime_start) << " s consumed for parsing 1st file" << endl << endl;
		}


		Graph * g2 = new Graph();
		graph = g2;


		parseFile(1); //parse second file

		if(args_info.time_given){
			time_t parsingTime_end = time(NULL);
			cout << g2->nodes.size() << " nodes in the given extended annotated automaton (2nd file)"  << endl;
			cout << difftime(parsingTime_end, parsingTime_start) << " s consumed for parsing 2nd file" << endl << endl;
		}

		time_t buildIntersection_start = time(NULL);

		GraphIntersection * g3 = new GraphIntersection();
		g3->intersection(g1,g2);

		if(args_info.time_given){
			time_t buildIntersection_end = time(NULL);
			cout << g3->nodepairs.size() << " nodes in the intersection"  << endl;
			cout << difftime(buildIntersection_end, buildIntersection_start) << " s consumed for building the intersection" << endl;
			cout << "--------------------------------------" << endl;
		}

		string filename = generateFilename(0) + "_" + generateFilename(1) + "_intersection";
		string title = filename + "    global formula: " + g3->globalFormulaAsString;

		out(g3, filename, title);

		delete g1;
		delete g2;
		delete g3;
	}


	/******************
	 * Union          *
	 ******************/
	if(args_info.union_given){


		// Complement of the first Graph

		GraphComplement * g1 = new GraphComplement();
		graph = g1;

		time_t parsingTime_start = time(NULL);

		parseFile(0); // parse the first file

		if(args_info.time_given){
			time_t parsingTime_end = time(NULL);
			cout << "--------------------------------------" << endl;
			cout << label2id.size()-3 << " labels" <<  endl;
			cout << graph->nodes.size() << " nodes in the given extended annotated automaton (1st file)"  << endl;
			cout << difftime(parsingTime_end, parsingTime_start) << " s consumed for parsing the file" << endl << endl;
		}

		time_t buildOG_start = time(NULL);

		g1->complement();


		// Complement of the second Graph

		GraphComplement * g2 = new GraphComplement();
		graph = g2;

		parsingTime_start = time(NULL);

		parseFile(1); // parse the first file

		if(args_info.time_given){
			time_t parsingTime_end = time(NULL);
			cout << label2id.size()-3 << " labels"  << endl;
			cout << graph->nodes.size() << " nodes in the given extended annotated automaton (2nd file)" << endl;
			cout << difftime(parsingTime_end, parsingTime_start) << " s consumed for parsing the file" << endl << endl;
		}

		buildOG_start = time(NULL);

		g2->complement();


		// Intersection of both GraphComplements

		GraphIntersection * g3 = new GraphIntersection();
		g3->intersection(g1,g2);


		// Complement of the Intersection

		GraphComplement * g4 = new GraphComplement();
		g3->convertToGraph(g4);


		/* since g4 is still total and complete it is sufficient to negate the global Formula
		 * (instead to apply the hole complement operation)
		 */
		g4->negateGlobalFormula();
		//g4->complement();


		if(args_info.time_given){
			time_t buildOG_end = time(NULL);
			cout << g4->nodes.size() << " number of nodes in the complement"  << endl;
			cout << difftime(buildOG_end, buildOG_start) << " s consumed for building the union" << endl;
			//cout << Formula::getMinisatTime() << "s consumed by minisat" << endl;
			cout << "--------------------------------------" << endl;
		}

		string filename = generateFilename(0) + "_" + generateFilename(1) + "_union";
		string title = filename + "    global formula: " + g3->globalFormulaAsString;

		out(g4, filename, title);

		//delete g1;
		//delete g2;
		//delete g3;
		//delete g4;
	}

	//printUsedMemory(); cout << "(after all deletes)" << endl;
	//cout << "--------------------------------------" << endl;

	cmdline_parser_free(&args_info);
    return EXIT_SUCCESS;
}


string generateFilename(int filenumber){
	assert(args_info.inputs != NULL);
	assert(args_info.inputs[filenumber] != NULL);

	string filename = args_info.inputs[filenumber];
	filename = filename.substr(0, filename.find_last_of(".og")-2);
	filename = filename.substr(filename.find_last_of("/")+1, filename.length());

	return filename;
}


void parseFile(int filenumber){
	assert(args_info.inputs != NULL);
	assert(args_info.inputs[filenumber] != NULL);

	og_yyin = fopen(args_info.inputs[filenumber], "r");

	if (og_yyin == NULL){
		abort(1, "File %s not found", args_info.inputs[filenumber]);
	}

	//parse
	if ( og_yyparse() != 0) {cout << PACKAGE << "\nparse error\n" << endl; exit(1);}

	fclose(og_yyin);
	og_yylex_destroy();
}


void initGlobalVariables(){
    label2id.clear();
    id2label.clear();
    inout.clear();
 //   Node::maxId = 0;

    lastLabelId = -1;

    addLabel("", ' '); //0 has a special meaning in minisat, therefore 0 cannot use as label ID
    addLabel("final", ' '); //Pos. 1
    addLabel("tau", ' '); // Pos. 2

    //	currentIdPos = 1;  //if there is no tau in the OG
	// currentIdPos = 2;
	firstLabelId = 2;

}


void out(Graph * g, string filename, string title){
	bool printToStdout = true;

	for (unsigned int j = 0; j<args_info.output_given; ++j){
		switch(args_info.output_arg[j]) {

		// create output using Graphviz dot
		case (output_arg_png):
		case (output_arg_eps):
		case (output_arg_pdf): {
			printToStdout = false;
			if (strcmp(CONFIG_DOT, "not found") == 0) {
				cerr << PACKAGE << ": Graphviz dot was not found by configure script; see README" << endl;
				cerr << "       necessary for option '--output'" << endl;
				exit(EXIT_FAILURE);
			}

			string call = string(CONFIG_DOT) + " -T" + args_info.output_orig[j] + " -q -o " + filename + "." + args_info.output_orig[j];
			FILE *s = popen(call.c_str(), "w");
			assert(s);

			g->toDot(s, title);
			assert(!ferror(s));

			pclose(s);
			break;
		}
		case (output_arg_dot): {
			printToStdout = false;
			FILE *s = fopen((filename+".dot").c_str(), "w");
			stringstream o;

			g->toDot(s, title);
			assert(!ferror(s));
			fclose(s);
			break;
		}

		case (output_arg_eaa): {
			printToStdout = false;
			Output o(filename+".eaa", "");
			g->print(o);
			break;
		}
		case (output_arg_none): {
			printToStdout = false;
			break;
		}
		}
	}

	if(printToStdout){
		Output o("-", "");
		g->print(o);
	}
}


void printUsedMemory(){

	if(args_info.time_given){
		std::string call = std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }'";
		FILE* ps = popen(call.c_str(), "r");
		unsigned int memory = 0;
		int res = fscanf(ps, "%u", &memory);
		assert(res != EOF);
		pclose(ps);
		printf( "%u KB memory used", memory);
	}
}


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

    // checks the correct number of input files
    if (args_info.inputs_num != 1 && args_info.complement_given) {
        abort(4, "Wrong number of input files. Exactly one file must be given if complement is chosen.");
    }

    if (args_info.inputs_num != 2 && args_info.intersection_given) {
        abort(4, "Wrong number of input files. Exactly two files must be given if intersection is chosen.");
    }

    if (args_info.inputs_num != 2 && args_info.union_given) {
        abort(4, "Wrong number of input files. Exactly two files must be given if union is chosen.");
    }

    free(params);
}
