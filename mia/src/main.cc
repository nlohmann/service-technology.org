#include <cstdlib>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include "config.h"
#include "cmdline.h"
#include "pnapi/pnapi.h"

using std::set;
using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::ofstream;
using pnapi::Automaton;
using pnapi::PetriNet;

/// the command line parameters
gengetopt_args_info args_info;

// the tuples
extern map<unsigned, vector<vector<unsigned int> > > tuples_source;
extern map<string, set<vector<unsigned int> > > tuples_target;

// the input files
extern FILE *graph_in;
extern FILE *im_in;

// the parsers
extern int im_parse();
extern int graph_parse();


/*!
 \brief abort with an error message and an error code
 
 The codes are documented in Wendy's manual.
 
 \param code    the error code
 \param format  the error message formatted as printf string
*/
void abort(unsigned int code, const char* format, ...) {
    fprintf(stderr, "%s: ", PACKAGE);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end (args);

    fprintf(stderr, " -- aborting [#%02d]\n", code);

    if (args_info.verbose_flag) {
        fprintf(stderr, "%s: see manual for a documentation of this error\n", PACKAGE);
    }

    exit(EXIT_FAILURE);
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    argv[0] = PACKAGE;
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    if (args_info.inputs_num != 2) {
        abort(1, "exactly two services have to be given");
    }

    free(params);
}


int main(int argc, char** argv) {
    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);

    fprintf(stderr, "%s: source service is '%s'\n", PACKAGE, args_info.inputs[0]);
    fprintf(stderr, "%s: target service is '%s'\n", PACKAGE, args_info.inputs[1]);

    /*---------------------------------------------------------------.
    | 1. calculate most permissive partner and migration information |
    `---------------------------------------------------------------*/
    string wendy_command = "wendy " + string(args_info.inputs[0]) + " --im=tmp.im --sa=tmp.sa";
    wendy_command += (args_info.verbose_flag) ? " --verbose" : " &> /dev/null";
    system(wendy_command.c_str());

    /*-------------------------------.
    | 2. parse migration information |
    `-------------------------------*/
    im_in = fopen("tmp.im", "r");
    im_parse();
    fclose(im_in);

    /*---------------------------------.
    | 3. parse most-permissive partner |
    `---------------------------------*/
    ifstream mpp_file("tmp.sa", ifstream::in);
    Automaton *mpp_sa = new Automaton();
    mpp_file >> pnapi::io::sa >> *mpp_sa;
    mpp_file.close();

    /*-------------------------------------------------.
    | 4. transform most-permissive partner to open net |
    `-------------------------------------------------*/
    PetriNet mpp = mpp_sa->stateMachine();
    delete mpp_sa;
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": most-permissive partner: " << pnapi::io::stat << mpp << std::endl;
    }

    /*------------------------.
    | 5. parse target service |
    `------------------------*/
    ifstream target_file(args_info.inputs[1], ifstream::in);
    PetriNet target;
    target_file >> pnapi::io::owfn >> target;
    target_file.close();
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": target: " << pnapi::io::stat << target << std::endl;
    }

    /*------------------------------------------------------.
    | 6. compose most-permissive partner and target service |
    `------------------------------------------------------*/
    PetriNet composition = mpp;
    composition.compose(target, "mpp", "target");
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": composition: " << pnapi::io::stat << composition << std::endl;
    }

    /*-------------------------------------------------.
    | 7. generate and parse state space of composition |
    `-------------------------------------------------*/
    ofstream composition_lolafile("tmp.lola", ofstream::trunc);
    composition_lolafile << pnapi::io::lola << composition;
    composition_lolafile.close();
    string lola_command = "lola-full tmp.lola -M 2> /dev/null";
    graph_in = popen(lola_command.c_str(), "r");
    graph_parse();
    pclose(graph_in);

    /*-------------------------.
    | 8. find migration states |
    `-------------------------*/
    for (map<unsigned, vector<vector<unsigned int> > >::iterator q1 = tuples_source.begin(); q1 != tuples_source.end(); ++q1) {
        for (map<string, set<vector<unsigned int> > >::iterator q2 = tuples_target.begin(); q2 != tuples_target.end(); ++q2) {
            bool pos = true;
            for (unsigned int i = 0; i < q1->second.size(); ++i) {
                if (q2->second.find(q1->second[i]) == q2->second.end()) {
                    pos = false;
                    break;
                }
            }
            if (pos) {
                fprintf(stderr, "m%d -> [%s]\n", q1->first, q2->first.c_str());
            }
        }
    }

    /*-----------.
    | 9. tidy up |
    `-----------*/
    if (not args_info.debug_flag) {
        remove("tmp.im");
        remove("tmp.sa");
        remove("tmp.lola");
    }

    return EXIT_SUCCESS;
}
