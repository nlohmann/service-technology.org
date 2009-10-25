/*****************************************************************************\
 Mia -- calculating migration information

 Copyright (C) 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>

 Mia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Mia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Mia.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

#include "config.h"
#include <cassert>

#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include "cmdline.h"
#include "config-log.h"
#include "pnapi/pnapi.h"
#include "verbose.h"

using std::set;
using std::string;
using std::map;
using std::vector;
using std::ifstream;
using std::ofstream;

/// the command line parameters
gengetopt_args_info args_info;

/// a variable holding the time of the call
clock_t start_clock = clock();

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


bool fileExists(std::string filename) {
    FILE *tmp = fopen(filename.c_str(), "r");
    if (tmp) {
        fclose(tmp);
        return true;
    } else {
        return false;
    }
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // set default values
    argv[0] = (char *)PACKAGE;

    // debug option
    if (argc > 0 and std::string(argv[1]) == "--bug") {
        FILE *debug_output = fopen("bug.log", "w");
        fprintf(debug_output, "%s\n", CONFIG_LOG);
        fclose(debug_output);
        fprintf(stderr, "Please send file 'bug.log' to %s.\n", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file (args_info.config_arg, &args_info, params) != 0) {
            abort(12, "error reading configuration file '%s'", args_info.config_arg);
        } else {
            status("using configuration file '%s'", args_info.config_arg);
        }
    } else {
        // check for configuration files
        string conf_filename = fileExists("mia.conf") ? "mia.conf" :
                               (fileExists(string(SYSCONFDIR) + "/mia.conf") ?
                               (string(SYSCONFDIR) + "/mia.conf") : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file ((char*)conf_filename.c_str(), &args_info, params) != 0) {
                abort(12, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }


    if (args_info.inputs_num != 2) {
        abort(1, "exactly two services have to be given");
    }

    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    // release memory (used to detect memory leaks)
    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}


int main(int argc, char** argv) {
    time_t start_time, end_time;

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);

    status("migrating '%s' to '%s", args_info.inputs[0], args_info.inputs[1]);

    // create a unique temporary file name
    char tmp[] = "/tmp/mia-XXXXXX";
#ifdef HAVE_MKSTEMP
    if (mkstemp(tmp) == -1) {
        abort(9, "could not create a temporary file '%s'", tmp);
    }
#endif
    string tmpname(tmp);
    string im_filename = tmpname + ".im";
    string mi_filename = tmpname + ".mi";
    string mpp_filename = tmpname + ".sa";
    string lola_filename = tmpname + ".lola";

    /*---------------------------------------------------------------.
    | 1. calculate most permissive partner and migration information |
    `---------------------------------------------------------------*/
    string wendy_command = string(args_info.wendy_arg) + " " + args_info.inputs[0]
        + " --im=" + im_filename + " --sa=" + mpp_filename + " --mi=" + mi_filename;
    if (args_info.messagebound_given) {
        std::stringstream s;
        s << args_info.messagebound_arg;
        wendy_command += " -m" + s.str();
    }
    wendy_command += ((args_info.verbose_flag) ? " --verbose" : " 2> /dev/null");
    time(&start_time);
    status("executing '%s'", wendy_command.c_str());
    system(wendy_command.c_str());
    time(&end_time);
    status("Wendy done [%.0f sec]", difftime(end_time, start_time));

    /*-------------------------------.
    | 2. parse migration information |
    `-------------------------------*/
    im_in = fopen(im_filename.c_str(), "r");
    if (!im_in) {
        abort(4, "could not read migration information");
    }
    im_parse();
    fclose(im_in);
    status("parsed migration information: %d tuples", stat_tupleCount);

    /*-----------------------------.
    | 3. parse marking information |
    `-----------------------------*/
    mi_in = fopen(mi_filename.c_str(), "r");
    if (!mi_in) {
        abort(11, "could not read marking information");
    }
    mi_parse();
    fclose(mi_in);

    /*---------------------------------.
    | 4. parse most-permissive partner |
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
    | 6. transform most-permissive partner to open net |
    `-------------------------------------------------*/
    time(&start_time);
    pnapi::PetriNet *mpp = new pnapi::PetriNet(mpp_sa->stateMachine());
    time(&end_time);
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": most-permissive partner: " << pnapi::io::stat << *mpp << std::endl;
    }
    status("converting most-permissive partner done [%.0f sec]", difftime(end_time, start_time));

    /*------------------------------------------------------.
    | 7. compose most-permissive partner and target service |
    `------------------------------------------------------*/
    // compse nets and add prefixes (if you wish to change them here, don't
    // forget to also adjust the lexer lexic_graph.ll)
    time(&start_time);
    mpp->compose(*target, "mpp[1].", "target[1].");
    time(&end_time);
    if (args_info.verbose_flag) {
        std::cerr << PACKAGE << ": composition: " << pnapi::io::stat << *mpp << std::endl;
    }
    status("composition done [%.0f sec]", difftime(end_time, start_time));

    /*-------------------------------------------------.
    | 8. generate and parse state space of composition |
    `-------------------------------------------------*/
    ofstream composition_lolafile(lola_filename.c_str(), ofstream::trunc);
    if (not composition_lolafile) {
        abort(7, "could not write composition");
    }
    composition_lolafile << pnapi::io::lola << *mpp;
    composition_lolafile.close();
    string lola_command = args_info.safe_flag ? args_info.lola_full1_arg : args_info.lola_full_arg;
    lola_command += " " + lola_filename + " -M";    
    if (!args_info.verbose_flag) {
        lola_command += " 2> /dev/null";
    }

    status("executing '%s'", lola_command.c_str());
    time(&start_time);
    graph_in = popen(lola_command.c_str(), "r");
    if (!graph_in) {
        abort(8, "could not read state space of composition");
    }
    graph_parse();
    pclose(graph_in);
    time(&end_time);

    status("LoLA done [%.0f sec]", difftime(end_time, start_time));
    status("generated state space of composition: %d states", stat_stateCount);
    status("%d tuples for target service found", stat_tupleCountNew);

    /*---------------------------.
    | 9. find jumper transitions |
    `---------------------------*/
    FILE *outputfile = stdout;
    if (args_info.output_given) {
        outputfile = fopen(args_info.output_arg, "w");
        if (!outputfile) {
            abort(12, "could not write to file '%s'", args_info.output_arg);
        }
        status("writing jumper transitions to '%s'", args_info.output_arg);
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

    status("%d jumper transitions found", jumperCount);

    fclose(outputfile);

    return EXIT_SUCCESS;
}
