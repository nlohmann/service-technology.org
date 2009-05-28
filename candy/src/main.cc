//#include <cstdlib>
//#include <ctime>
#include <cassert>
#include <sstream>

// ???
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "cmdline.h"
#include "config.h"
#include "pnapi.h"


/// the command line parameters
gengetopt_args_info args_info;

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {

	// set default values
	cmdline_parser_init(&args_info);

	// initialize the parameters structure
	struct cmdline_parser_params *params = cmdline_parser_params_create();

	// call the cmdline parser
	cmdline_parser(argc, argv, &args_info);

	// check whether at most one file is given
	if (args_info.inputs_num > 1) {
		fprintf(stderr, "%s: at most one input file must be given -- aborting\n", PACKAGE);
		exit(EXIT_FAILURE);
	}
	free(params);
}


int main(int argc, char** argv) {
	time_t start_time, end_time;

	time(&start_time);
	/*--------------------------------------.
	| 0. parse the command line parameters  |
	`--------------------------------------*/

	evaluateParameters(argc, argv);

	std::cerr << PACKAGE << "%s: Processing" << args_info.inputs[0] << ".\n";

	pnapi::PetriNet* net = new pnapi::PetriNet;

	/*---------------------------------.
	| 1. parse the operating guideline |
	`---------------------------------*/

	//try {
	//	// parse either from standard input or from a given file
	//	if (args_info.inputs_num == 0) {
	//		std::cin >> pnapi::io::owfn >> *(net);
	//	} else {
	//		assert (args_info.inputs_num == 1);
	//		std::ifstream inputStream;
	//		inputStream.open(args_info.inputs[0]);
	//		inputStream >> pnapi::io::owfn >> *(net);
	//		inputStream.close();
	//	}
	//	if (args_info.verbose_given) {
	//		std::cerr << PACKAGE << ": read net " << pnapi::io::stat << *(net) << std::endl;
	//	}
	//} catch (pnapi::io::InputError error) {
	//	std::cerr << PACKAGE << error << std::endl;
	//	exit(EXIT_FAILURE);
	//}

	/*-------------------.
	| 2. parse cost file |
	`-------------------*/

	/*-----------------------------.
	| 3. compute cost-efficient OG |
	`-----------------------------*/

	/*----------------------------------.
	| 4. compute cost-efficient partner |
	`----------------------------------*/
}
