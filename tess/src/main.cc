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
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <algorithm>
#include <time.h>
#include "cmdline.h"
#include "config-log.h"
#include "verbose.h"
#include "Output.h"
#include "helpers.h"
#include "Formula.h"
#include "Node.h"
#include "Graph.h"


using namespace std;

/// the command line parameters
gengetopt_args_info args_info;
string invocation;

// lexer and parser
extern int og_yyparse();
extern FILE* og_yyin;
extern int og_yylex_destroy();

Graph * graph;

map<string, int> label2id;
map<int, string> id2label;
map<int, char> inout;

void evaluateParameters(int argc, char** argv);
void initGlobalVariables();
void parseFile(int filenumber = 0);
void printUsedMemory();
void out(Graph * g, string filename);


int main(int argc, char **argv) {

	evaluateParameters(argc, argv);
	//TODO: muss das hier stehen?
	//Output::setTempfileTemplate(args_info.tmpfile_arg);
	//Output::setKeepTempfiles(args_info.noClean_flag);

	initGlobalVariables();

	Graph * g = new Graph();
	graph = g;

	time_t parsingTime_start = time(NULL);

	parseFile();

	if(args_info.time_given){
		time_t parsingTime_end = time(NULL);
		cout << difftime(parsingTime_end, parsingTime_start) << " s consumed for parsing the file" << endl << endl;
		printUsedMemory(); cout << "(after parsing)" << endl;
	}

	g->generateTestCases();

	cout << "Anzahl der Testfälle: " << g->testCases.size() << endl;
	vector<TestCase*>::const_iterator i = g->testCases.begin();
	vector<TestCase*>::const_iterator vorgaenger = i;
	cout << "firstID: " << (*i)->getId() << endl;
	for(vector<TestCase*>::const_iterator iter = ++i; iter != g->testCases.end(); ++iter){
		assert(((*iter)->getId() - (*vorgaenger)->getId()) == 1);
		vorgaenger = iter;
	}

	string filename = args_info.inputs[0];
	filename = filename.substr(0,filename.find_last_of(".og")-2);

	out(g,filename);
	//delete g;

	//cmdline_parser_free(&args_info);
    cout << "successful\n";

    return EXIT_SUCCESS;
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


void out(Graph * g, string filename){
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

			//TODO: fopen oder popen
			/*print the TestOG of the graph*/
			string call = string(CONFIG_DOT) + " -T" + args_info.output_orig[j] + " -q -o " + filename + "_TestOG" + "." + args_info.output_orig[j];
			FILE *s = popen(call.c_str(), "w");
			g->toDot(s, filename + "_TestOG");
			pclose(s);

			/*print the test cases of the graph*/
			string call_tc = string(CONFIG_DOT) + " -T" + args_info.output_orig[j] + " -q -o " + filename + "_TestCases" + "." + args_info.output_orig[j];
			FILE *out = popen(call_tc.c_str(), "w");
			g->testCasesToDot(out, filename + "_TestCases");
			pclose(out);

			break;
		}
		case (output_arg_dot): {
			printToStdout = false;

			//TODO: fopen oder popen
			/*print the TestOG of the graph*/
			FILE *s = fopen((filename + "_TestOG" + ".dot").c_str(), "w");
			g->toDot(s, filename + "_TestOG");
			assert(!ferror(s));
			fclose(s);

			/*print the test cases of the graph*/
			FILE *s_tc = fopen((filename + "_TestCases" + ".dot").c_str(), "w");
			g->testCasesToDot(s_tc, filename + "_TestCases");
			fclose(s_tc);

			break;
		}

		case (output_arg_eaa): {
			printToStdout = false;

			/*print the TestOG of the graph*/
			Output o(filename + "_TestOG" +".eaa", "");
			g->toEaa(o);

			/*print the test cases of the graph*/
			Output o_tc(filename + "_TestCases" + ".eaa", "");
			g->toEaa_TestCases(o_tc);

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
		g->toEaa(o);
		g->toEaa_TestCases(o);
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


    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    free(params);
}
