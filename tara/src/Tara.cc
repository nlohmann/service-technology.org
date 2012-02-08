#include "Tara.h"


unsigned int Tara::highestTransitionCosts = 0;
unsigned int Tara::sumOfLocalMaxCosts = 0;
unsigned int Tara::initialState = 0;

std::map<pnapi::Transition* ,unsigned int> Tara::partialCostFunction;

pnapi::PetriNet* Tara::net = 0; 

gengetopt_args_info Tara::args_info;

std::deque<innerState *> Tara::graph;

unsigned int Tara::cost(pnapi::Transition* t) {
   std::map<pnapi::Transition*,unsigned int>::iterator cost = Tara::partialCostFunction.find(t);
   if(cost==partialCostFunction.end()) 
      return 0;

   return cost->second;
}


/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
        return tmp.good();
	}

void Tara::evaluateParameters(int argc, char** argv) {

    //invocation string
    std::string invocation;

    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &Tara::args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (Tara::args_info.bug_flag) {
        {
            Output debug_output("bug.log", "configuration information");
            debug_output.stream() << CONFIG_LOG << std::flush;
        }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (Tara::args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file(Tara::args_info.config_arg, &Tara::args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", Tara::args_info.config_arg);
        } else {
            status("using configuration file '%s'", Tara::args_info.config_arg);
        }
    } else {
        // check for configuration files
        std::string conf_generic_filename = std::string(PACKAGE) + ".conf";
        std::string conf_filename = fileExists(conf_generic_filename) ? conf_generic_filename :
                                    (fileExists(std::string(SYSCONFDIR) + "/" + conf_generic_filename) ?
                                     (std::string(SYSCONFDIR) + "/" + conf_generic_filename) : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file(const_cast<char*>(conf_filename.c_str()), &Tara::args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }


    // check whether at most one file is given
    if (Tara::args_info.inputs_num > 1) {
        abort(4, "at most one reachability graph must be given");
    }

    free(params);
}
