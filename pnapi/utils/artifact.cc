#include <config.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <libgen.h>
#include "pnapi.h"
#include "util.h"
#include "verbose.h"
#include "Output.h"
#include "artifact-cmdline.h"

using namespace pnapi;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;


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
        abort(1, "invalid command-line parameter(s)");
    }

    free(params);
}


/// remove the suffix from a filename
inline std::string stripSuffix(std::string s) {
    return (s.substr(0, s.find_first_of(".")));
}


int main(int argc, char** argv) {
    /// collect input files
    std::string inputfiles;

    /// the overall net
    PetriNet all;

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);


    /*-------------------.
    | 1. parse artifacts |
    `-------------------*/
    for (unsigned int i = 0; i < args_info.artifacts_given; ++i) {
        inputfiles += std::string(args_info.artifacts_arg[i]) + " ";
        PetriNet n;
        std::ifstream net_file(args_info.artifacts_arg[i], std::ifstream::in);
        try {
            net_file >> io::owfn >> n;
        } catch (exception::InputError error) {
            std::stringstream ss;
            ss << error;
            abort(2, "Input Error: %s", ss.str().c_str());
        }
        n.prefixNames(stripSuffix(args_info.artifacts_arg[i]) + ".");
        all.compose(n, "", "");
    }


    /*--------------.
    | 2. parse goal |
    `--------------*/
    if (args_info.goal_given) {
        inputfiles += std::string(args_info.goal_arg) + " ";
        PetriNet m;
        std::ifstream marking_file(args_info.goal_arg, std::ifstream::in);
        marking_file >> io::owfn >> m;
        all.getFinalCondition().conjunct(m.getFinalCondition(), all.guessPlaceRelation(m));
    }


    /*------------------.
    | 3. parse policies |
    `------------------*/
    for (unsigned int i = 0; i < args_info.policies_given; ++i) {
        inputfiles += std::string(args_info.policies_arg[i]) + " ";
        PetriNet policy;
        std::ifstream policy_file(args_info.policies_arg[i], std::ifstream::in);
        policy_file >> io::owfn >> policy;
        all.produce(policy, "net", stripSuffix(args_info.policies_arg[i]));
    }


    /*----------------.
    | 4. write output |
    `----------------*/
    Output o(args_info.output_arg, "result");
    o << meta(io::CREATOR, std::string(argv[0]) + " (" + PACKAGE_STRING + ")")
      << meta(io::INVOCATION, invocation) << meta(io::INPUTFILE, inputfiles)
      << io::owfn << all;


    return EXIT_SUCCESS;
}
