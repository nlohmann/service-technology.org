#include "config.h"
#include <cassert>

#include <cstdarg>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include "cmdline.h"
#include "pnapi/pnapi.h"

using std::set;
using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::ofstream;

/// the command line parameters
gengetopt_args_info args_info;

// the tuples
extern map<unsigned, vector<vector<unsigned int> > > tuples_source;
extern map<string, set<vector<unsigned int> > > tuples_target;

// the marking names
extern std::map<unsigned int, std::string> id2marking;

// statistics
extern unsigned int stat_stateCount;
extern unsigned int stat_tupleCount;
extern unsigned int stat_tupleCountNew;

// the input files
extern FILE *graph_in;
extern FILE *im_in;
extern FILE *mi_in;

// the parsers
extern int graph_parse();
extern int im_parse();
extern int mi_parse();


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
    argv[0] = (char *)PACKAGE;
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

    fprintf(stderr, "%s: migrating '%s' to '%s\n", PACKAGE, args_info.inputs[0], args_info.inputs[1]);

    // create a unique temporary file name
    char tmp[] = "/tmp/mia-XXXXXX";
    if (mkstemp(tmp) == -1) {
        abort(9, "could not create a temporary file '%s'", tmp);
    }
    string tmpname(tmp);
    string im_filename = tmpname + ".im";
    string mi_filename = tmpname + ".mi";
    string mpp_filename = tmpname + ".sa";
    string lola_filename = tmpname + ".lola";

    /*---------------------------------------------------------------.
    | 1. calculate most permissive partner and migration information |
    `---------------------------------------------------------------*/
    string wendy_command = string(BINARY_WENDY) + " " + string(args_info.inputs[0])
        + " --im=" + im_filename + " --sa=" + mpp_filename + " --mi=" + mi_filename;
    if (args_info.messagebound_given) {
        std::stringstream s;
        s << args_info.messagebound_arg;
        wendy_command += " -m" + s.str();
    }
    wendy_command += ((args_info.verbose_flag) ? " --verbose" : " 2> /dev/null");
    if (args_info.verbose_flag) {
        fprintf(stderr, "%s: executing '%s'\n", PACKAGE, wendy_command.c_str());
    }
    system(wendy_command.c_str());




    /*-------------------------------.
    | 2. parse migration information |
    `-------------------------------*/
    im_in = fopen(im_filename.c_str(), "r");
    if (!im_in) {
        abort(4, "could not read migration information");
    }
    im_parse();
    fclose(im_in);
    if (args_info.verbose_flag) {
        fprintf(stderr, "%s: parsed migration information: %d tuples\n", PACKAGE, stat_tupleCount);
    }

    /*-----------------------------.
    | 8. parse marking information |
    `-----------------------------*/
    mi_in = fopen(mi_filename.c_str(), "r");
    if (!mi_in) {
        abort(11, "could not read marking information");
    }
    mi_parse();
    fclose(mi_in);

    /*---------------------------------.
    | 3. parse most-permissive partner |
    `---------------------------------*/
    ifstream mpp_file(mpp_filename.c_str(), ifstream::in);
    if (not mpp_file) {
        abort(5, "could not read most-permissive partner");
    }
    pnapi::Automaton *mpp_sa = new pnapi::Automaton();
    mpp_file >> pnapi::io::sa >> *mpp_sa;
    mpp_file.close();
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": most-permissive partner: " << pnapi::io::stat << *mpp_sa << std::endl;
    }

    /*------------------------.
    | 5. parse target service |
    `------------------------*/
    ifstream target_file(args_info.inputs[1], ifstream::in);
    if (not target_file) {
        abort(6, "could not read target service '%s'", args_info.inputs[1]);
    }
    pnapi::PetriNet *target = new pnapi::PetriNet();
    target_file >> pnapi::io::owfn >> *target;
    target_file.close();
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": target: " << pnapi::io::stat << *target << std::endl;
    }




    /*-------------------------------------------------.
    | 4. transform most-permissive partner to open net |
    `-------------------------------------------------*/
    pnapi::PetriNet *mpp = new pnapi::PetriNet(mpp_sa->stateMachine());
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": most-permissive partner: " << pnapi::io::stat << *mpp << std::endl;
    }

    /*------------------------------------------------------.
    | 6. compose most-permissive partner and target service |
    `------------------------------------------------------*/
    // compse nets and add prefixes (if you wish to change them here, don't
    // forget to also adjust the lexer lexic_graph.ll)
    mpp->compose(*target, "mpp[1].", "target[1].");
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": composition: " << pnapi::io::stat << *mpp << std::endl;
    }

    /*-------------------------------------------------.
    | 7. generate and parse state space of composition |
    `-------------------------------------------------*/
    ofstream composition_lolafile(lola_filename.c_str(), ofstream::trunc);
    if (not composition_lolafile) {
        abort(7, "could not write composition");
    }
    composition_lolafile << pnapi::io::lola << *mpp;
    composition_lolafile.close();
    string lola_command = args_info.safe_flag ? "lola-full1" : string(BINARY_LOLA);
    lola_command += " " + lola_filename + " -M";
    if (args_info.verbose_flag) {
        fprintf(stderr, "%s: executing '%s'\n", PACKAGE, lola_command.c_str());
    } else {
        lola_command += " 2> /dev/null";
    }
    graph_in = popen(lola_command.c_str(), "r");
    if (!graph_in) {
        abort(8, "could not read state space of composition");
    }
    graph_parse();
    pclose(graph_in);
    if (args_info.verbose_flag) {
        fprintf(stderr, "%s: generated state space of composition: %d states\n", PACKAGE, stat_stateCount);
        fprintf(stderr, "%s: %d tuples for target service found\n", PACKAGE, stat_tupleCountNew);
    }

    /*---------------------------.
    | 9. find jumper transitions |
    `---------------------------*/
    FILE *outputfile = stdout;
    if (args_info.output_given) {
        outputfile = fopen(args_info.output_arg, "w");
        if (!outputfile) {
            abort(12, "could not write to file '%s'", args_info.output_arg);
        }
        if (args_info.verbose_flag) {
            fprintf(stderr, "%s: writing jumper transitions to '%s'\n", PACKAGE, args_info.output_arg);
        }
    }

    unsigned int jumperCount = 0;
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
                fprintf(outputfile, "[%s] -> [%s]\n", id2marking[q1->first].c_str(), q2->first.c_str());
                ++jumperCount;
            }
        }
    }
    if (args_info.verbose_flag) {
        fprintf(stderr, "%s: %d jumper transitions found\n", PACKAGE, jumperCount);
    }


    return EXIT_SUCCESS;
}
