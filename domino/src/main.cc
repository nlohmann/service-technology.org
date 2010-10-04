/*****************************************************************************\
 Hello -- <<-- Hello World -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Hello is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Hello is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/* <<-- CHANGE START (main program) -->> */
// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>

#include <pnapi/pnapi.h>
#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "Results.h"

#include "Tarjan.h"
#include "Fragmentation.h"

map<place_t, unsigned int> Place2MaxTokens;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
Output* markingoutput = NULL;

/// a variable holding the time of the call
clock_t start_clock = clock();

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
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (args_info.bug_flag) {
        {
            Output debug_output("bug.log", "configuration information");
            debug_output.stream() << CONFIG_LOG << std::flush;
        }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file(args_info.config_arg, &args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", _cfilename_(args_info.config_arg));
        } else {
            status("using configuration file '%s'", _cfilename_(args_info.config_arg));
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
            if (cmdline_parser_config_file(const_cast<char*>(conf_filename.c_str()), &args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", _cfilename_(conf_filename));
            } else {
               status("using configuration file '%s'", _cfilename_(conf_filename));
            }
        } else {
            status("not using a configuration file");
        }
    }

	// check dot-options
	if (args_info.dot_roles_given || args_info.dot_fragments_given) {
		if (!(args_info.dotInitial_flag || args_info.dotFragmented_flag)) {
			message(_cwarning_("no dot files will be generated (%s %s %s %s"), _cparameter_("--dotInitial"), _cwarning_("or"), _cparameter_("--dotFragmented"), _cwarning_("missing)"));
		}
	}

	if (args_info.dotInitial_flag || args_info.dotFragmented_flag) {
		if (!(args_info.dot_roles_given || args_info.dot_fragments_given)) {
			message(_cwarning_("no dot files will be generated (%s %s %s %s"), _cparameter_("--dot-roles"), _cwarning_("or"), _cparameter_("--dot-fragments"), _cwarning_("missing)"));
		}
	}

	if (args_info.dotColorUnassigned_flag) {
		if (!(args_info.dot_roles_given || args_info.dot_fragments_given)) {
			message(_cwarning_("no dot files will be generated (%s %s %s %s"), _cparameter_("--dot-roles"), _cwarning_("or"), _cparameter_("--dot-fragments"), _cwarning_("missing)"));
		}
	}
	if (args_info.dotServices_flag && !args_info.output_given) {
		message(_cwarning_("dot files for services will not be generated, %s is missing"), _cparameter_("--output"));
	}

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }

    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
	cmdline_parser_free(&args_info);

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", _ctool_(PACKAGE));
        system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}

void createDotFile(const string & OutputFile, pnapi::PetriNet & Petrinet, const string InputFile) {
	if (fileExists(string(OutputFile + ".dot"))) {
		abort(2, "file %s already exists", _cfilename_(OutputFile + ".dot"));
	}

	std::ofstream outStream;

	outStream.open(string(OutputFile + ".dot").c_str(), std::ios_base::trunc);
	if (outStream.fail()) {
		abort(2, "file %s cannot be opened", _cfilename_(OutputFile + ".dot"));
	}

	outStream << pnapi::io::dot
				<< meta(pnapi::io::INPUTFILE, InputFile)
                << meta(pnapi::io::CREATOR, PACKAGE_STRING)
                << meta(pnapi::io::INVOCATION, invocation)
				<< Petrinet;
	outStream.close();	

}

void createOWFNFile(const string & OutputFile, pnapi::PetriNet & Petrinet, const string InputFile) {
	if (fileExists(string(OutputFile + ".dot"))) {
		abort(2, "file %s already exists", _cfilename_(OutputFile + ".owfn"));
	}

	std::ofstream outStream;

	outStream.open(string(OutputFile + ".owfn").c_str(), std::ios_base::trunc);
	if (outStream.fail()) {
		abort(2, "file %s cannot be opened", _cfilename_(OutputFile + ".owfn"));
	}

	outStream << pnapi::io::owfn
				<< meta(pnapi::io::INPUTFILE, InputFile)
                << meta(pnapi::io::CREATOR, PACKAGE_STRING)
                << meta(pnapi::io::INVOCATION, invocation)
				<< Petrinet;
	outStream.close();
}

/// main-function
int main(int argc, char** argv) {

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);


    /*----------------------.
    | 1. parse the open net |
    `----------------------*/
    pnapi::PetriNet net;

    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            // parse the open net from standard input
            std::cin >> pnapi::io::owfn >> net;
        } else {
            // strip suffix from input filename (if necessary: uncomment next line for future use)
            //inputFilename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

            // open input file as an input file stream
            std::ifstream inputStream(args_info.inputs[0]);
            // ... and abort, if an error occurs
            if (!inputStream) {
                abort(1, "could not open file '%s'", _cfilename_(args_info.inputs[0]));
            }

            // parse the open net from the input file stream
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                        >> pnapi::io::owfn >> net;
        }
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    } catch (pnapi::exception::InputError error) {
        std::ostringstream s;
        s << error;
        abort(2, "%s", s.str().c_str());
    }

	if (args_info.canonicalNames_flag) {
		//ToDO: ggf. Mapping speichern
		net.canonicalNames();
	}

    /*----------------------------------------.
    | 2. specific							  |
    `----------------------------------------*/

	string fileName;
	string outputFileName;
	validStatus_e processStatus;
	set<std::string> roles;

	Fragmentation f(net);

	f.buildRoleFragments();

	if (args_info.inputs_num == 0) {
		fileName = "STDIN";
	}
	else {
		fileName = string(args_info.inputs[0]);
	}

	if (args_info.dotInitial_flag) {
		if (args_info.dot_roles_given) {
			f.colorFragmentsByRoleID(args_info.dotColorUnassigned_flag);
			if (args_info.dot_roles_arg != NULL) {
				outputFileName = args_info.dot_roles_arg;
			}
			else {
				outputFileName = string(fileName + "_Initial-Roles");
			}
			createDotFile(outputFileName, net, fileName);
		}
		if (args_info.dot_fragments_given) {
			f.colorFragmentsByFragID(args_info.dotColorUnassigned_flag);
			if (args_info.dot_fragments_arg != NULL) {
				outputFileName = args_info.dot_fragments_arg;
			}
			else {
				outputFileName = string(fileName + "_Initial-Fragments");
			}
			createDotFile(outputFileName, net, fileName);
		}
	}

	f.processUnassignedFragments();

	if (args_info.dotFragmented_flag) {
		if (args_info.dot_roles_given) {
			f.colorFragmentsByRoleID(args_info.dotColorUnassigned_flag);
			if (args_info.dot_roles_arg != NULL) {
				outputFileName = args_info.dot_roles_arg;
			}
			else {
				outputFileName = string(fileName + "_Fragmented-Roles");
			}
			createDotFile(outputFileName, net, fileName);
		}
		if (args_info.dot_fragments_given) {
			f.colorFragmentsByFragID(args_info.dotColorUnassigned_flag);
			if (args_info.dot_fragments_arg != NULL) {
				outputFileName = args_info.dot_fragments_arg;
			}
			else {
				outputFileName = string(fileName + "_Fragmented-Fragments");
			}
			createDotFile(outputFileName, net, fileName);
		}
	}

	if (f.isProcessValid(true) == VALID_BAD) {
		message(_cbad_("worklfow demposition FAILED"));
		return EXIT_FAILURE;
	}

	f.buildServices();

	if (args_info.dot_serviceOverview_given) {
		f.colorFragmentsByRoleID();
		if (args_info.dot_serviceOverview_arg != NULL) {
			outputFileName = args_info.dot_serviceOverview_arg;
		}
		else {
			outputFileName = string(fileName + "_Services");
		}
		createDotFile(outputFileName, net, fileName);
	}

	processStatus = f.isProcessValid(false);
	if (processStatus == VALID_BAD) {
		message(_cbad_("worklfow demposition FAILED"));
		return EXIT_FAILURE;
	}
	else if (processStatus == VALID_TODO) {
		message(_cbad_("workflow decomposition UNCOMPLETED"));
		return EXIT_FAILURE;
	}
	else {
		message(_cgood_("workflow decomposition successfull"));
	}

	if (args_info.output_given) {
		roles = net.getRoles();
		FOREACH(r, roles) {
			pnapi::PetriNet roleNet = f.createPetrinetByRoleID(f.getRoleID((*r)));
			if (args_info.output_arg != NULL) {
				outputFileName = string(args_info.output_arg + (*r));
			}
			else {
				outputFileName = string(fileName + "_Service-" + (*r));
			}
			createOWFNFile(outputFileName, roleNet, fileName);
			if (args_info.dotServices_flag) {
				createDotFile(outputFileName, roleNet, fileName);
			}
		}
	}

    return EXIT_SUCCESS;
}
