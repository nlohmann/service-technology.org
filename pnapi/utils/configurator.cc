#include <config.h>
#include <cstdlib>
#include <string>
#include <set>
#include <fstream>
#include <sstream>
#include <libgen.h>
#include "pnapi.h"
#include "util.h"
#include "verbose.h"
#include "Output.h"
#include "configurator-cmdline.h"

using std::string;
using std::ifstream;
using namespace pnapi;

/// the command line parameters
gengetopt_args_info args_info;

/// the net
PetriNet net;

/// name of the input file
string filename;

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
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(1, "invalid command-line parameter(s)");
    }

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }
    
    free(params);
}

int main(int argc, char** argv) {
    evaluateParameters(argc, argv);

    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            std::cin >> meta(io::INVOCATION, invocation) >> pnapi::io::owfn >> net;
            filename = "net";
        } else {
                // strip suffix from input filename
            filename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

            std::ifstream inputStream(args_info.inputs[0]);
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                >> meta(io::INVOCATION, invocation)
                >> pnapi::io::owfn >> net;
        }
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    } catch(pnapi::io::InputError error) {
        std::ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }

    std::set<Transition *> originalTransitions = net.getTransitions();

    // STEP 1: ADD START TRANSITION
    Place &pstart = net.createPlace("config_p0");

    std::set<std::string> labels;
    labels.insert("start");
    net.addSynchronousLabels(labels);
    Transition& start = net.createTransition("config_start", labels);
    net.createArc(pstart, start);
    
    PNAPI_FOREACH(std::set<Place*>, net.getInternalPlaces(), p) {
        if ((*p)->getTokenCount() > 0) {
            net.createArc(start, **p, (*p)->getTokenCount());
            (*p)->setTokenCount(0);
        }
    }
    
    pstart.setTokenCount(1);

    if (args_info.allowByDefault_given) {
        PNAPI_FOREACH(std::set<Transition*>, originalTransitions, t) {
            Place &pt = net.createPlace("config_p_" + (*t)->getName(), Node::INTERNAL, 1);
            net.createArc(pt, **t);
            net.createArc(**t, pt);
            
            std::set<std::string> temp_labels;
            temp_labels.insert("block_" + (*t)->getName());
            Transition &bt = net.createTransition("config_b_" + (*t)->getName(), temp_labels);
            net.addSynchronousLabels(temp_labels);
            net.createArc(pt, bt);
            net.createArc(pstart, bt);
            net.createArc(bt, pstart);
        }
    }

    if (args_info.blockByDefault_given) {
        PNAPI_FOREACH(std::set<Transition*>, originalTransitions, t) {
            Place &pt = net.createPlace("config_p_" + (*t)->getName());
            net.createArc(pt, **t);
            net.createArc(**t, pt);

            Place &pbt = net.createPlace("config_pb_" + (*t)->getName(), Node::INTERNAL, 1);

            std::set<std::string> temp_labels;
            temp_labels.insert("allow_" + (*t)->getName());
            Transition &at = net.createTransition("config_a_" + (*t)->getName(), temp_labels);
            net.addSynchronousLabels(temp_labels);
            net.createArc(at, pt);
            net.createArc(pbt, at);
            net.createArc(pstart, at);
            net.createArc(at, pstart);
        }
    }

    std::cout << pnapi::io::owfn << net;

    return EXIT_SUCCESS;
}