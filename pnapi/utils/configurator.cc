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

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);


    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
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
    } catch(pnapi::exception::InputError error) {
        std::ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }


    /*--------------------------.
    | 2. check role informaiton |
    `--------------------------*/
    if (args_info.roles_flag and net.getRoles().empty()) {
        abort(6, "net has no roles, but parameter '--roles' is given");
    }
    status("the net has %d roles", (unsigned int)net.getRoles().size());

    // preserve original transition set
    std::set<Transition *> originalTransitions = net.getTransitions();


    /*----------------------------.
    | 3. organize initial marking |
    `----------------------------*/
    Place &pstart = net.createPlace("config_p0");

    Label & label = net.getInterface().addSynchronousLabel("start");
    Transition & start = net.createTransition("config_start");
    start.addLabel(label);
    net.createArc(pstart, start);

    PNAPI_FOREACH(p, net.getPlaces()) {
        if ((*p)->getTokenCount() > 0) {
            net.createArc(start, **p, (*p)->getTokenCount());
            (*p)->setTokenCount(0);
        }
    }

    pstart.setTokenCount(1);


    /*--------------------------------------------.
    | 4. proceed with the configuration interface |
    `--------------------------------------------*/
    if (args_info.allowByDefault_given) {
        PNAPI_FOREACH(t, originalTransitions) {
            // only process transitions with role information if required
            if (args_info.roles_flag and (*t)->getRoles().empty()) {
                continue;
            }

            // create a "guard" place for each transition and mark it
            Place & pt = net.createPlace("config_pa_" + (*t)->getName(), 1);
            net.createArc(pt, **t);
            net.createArc(**t, pt);
            
            if (args_info.roles_flag) {
                // create "block" transition for the role if not already present
                std::set<std::string> roles = (*t)->getRoles();
                if (roles.size() > 1) {
                    abort(5, "transition %s has more than one label", (*t)->getName().c_str());
                }
                std::string role = *(roles.begin());
                Transition *bt = net.findTransition("config_b_" + role);
                if (bt == NULL) {
                    Label & temp_label = net.getInterface().addSynchronousLabel("block_" + role);
                    bt = &(net.createTransition("config_b_" + role));
                    bt->addLabel(temp_label);

                    // connect "block" transition
                    net.createArc(pstart, *bt);
                    net.createArc(*bt, pstart);
                }
                net.createArc(pt, *bt);
            } else {
                // create a "block" transition for each transition
                Label & temp_label = net.getInterface().addSynchronousLabel("block_" + (*t)->getName());
                Transition & bt = net.createTransition("config_b_" + (*t)->getName());
                bt.addLabel(temp_label);

                // connect "block" transition
                net.createArc(pt, bt);
                net.createArc(pstart, bt);
                net.createArc(bt, pstart);
            }
        }
    }

    if (args_info.blockByDefault_given) {
        PNAPI_FOREACH(t, originalTransitions) {
            // only process transitions with role information if required
            if (args_info.roles_flag and (*t)->getRoles().empty()) {
                continue;
            }

            // create a "guard" place for each transition
            Place &pt = net.createPlace("config_pa_" + (*t)->getName());
            net.createArc(pt, **t);
            net.createArc(**t, pt);

            if (args_info.roles_flag) {
                // create "allow" transition if not already present
                std::set<std::string> roles = (*t)->getRoles();
                if (roles.size() > 1) {
                    abort(5, "transition %s has more than one label", (*t)->getName().c_str());
                }
                std::string role = *(roles.begin());
                Transition *at = net.findTransition("config_a_" + role);
                if (at == NULL) {
                    Label & temp_label = net.getInterface().addSynchronousLabel("allow_" + role);
                    at = &(net.createTransition("config_a_" + role));
                    at->addLabel(temp_label);

                    // connect "allow" transition
                    net.createArc(pstart, *at);
                    net.createArc(*at, pstart);

                    // create a "guard" place to make net bounded
                    Place &pbt = net.createPlace("config_pb_" + role, 1);
                    net.createArc(pbt, *at);
                }
                net.createArc(*at, pt);
            } else {
                // create an "allow" transition for each transition
                Label & temp_label = net.getInterface().addSynchronousLabel("allow_" + (*t)->getName());
                Transition & at = net.createTransition("config_a_" + (*t)->getName());
                at.addLabel(temp_label);

                // connect "allow" transition
                net.createArc(at, pt);
                net.createArc(pstart, at);
                net.createArc(at, pstart);

                // create a "guard" place to make net bounded
                Place &pbt = net.createPlace("config_pb_" + (*t)->getName(), 1);
                net.createArc(pbt, at);
            }
        }
    }


    /*----------.
    | 5. output |
    `----------*/
    if (args_info.output_given) {
        std::string mode_name = args_info.allowByDefault_given ? "allow" : "block"; 
        std::string output_filename = args_info.output_arg ? args_info.output_arg : filename + "_" + mode_name + ".owfn";
        Output output(output_filename, "net with configuration interface");
        output.stream() << pnapi::io::owfn << net;
    }


    return EXIT_SUCCESS;
}
