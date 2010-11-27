/*****************************************************************************\
 			 _____                  _             
			|  __ \                (_)            
			| |  | | ___  _ __ ___  _ _ __   ___  
			| |  | |/ _ \| '_ ` _ \| | '_ \ / _ \ 
			| |__| | (_) | | | | | | | | | | (_) |
			|_____/ \___/|_| |_| |_|_|_| |_|\___/ 		                          
			DecOMposition of busINess wOrkflows (into services)
			http://service-technology.org/domino

 Copyright (c) 2010 Andreas Lehmann

 Domino is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Domino is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Domino.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

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
        int ret = system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
		if (ret == 12071983) {
			status("compiler satisfied");
		}
    }
}

void createDotFile(const string & OutputFile, pnapi::PetriNet & Petrinet, const string InputFile) {
	if (fileExists(string(OutputFile + ".dot")) && args_info.noOverride_flag) {
		abort(5, "file %s already exists", _cfilename_(OutputFile + ".dot"));
	}

	std::ofstream outStream;

	outStream.open(string(OutputFile + ".dot").c_str(), std::ios_base::trunc);
	if (outStream.fail()) {
		abort(3, "file %s cannot be opened", _cfilename_(OutputFile + ".dot"));
	}

	outStream << pnapi::io::dot
				<< meta(pnapi::io::INPUTFILE, InputFile)
                << meta(pnapi::io::CREATOR, PACKAGE_STRING)
                << meta(pnapi::io::INVOCATION, invocation)
				<< Petrinet;
	outStream.close();

	status("file %s created", _cfilename_(OutputFile + ".dot"));
}

void createOWFNFile(const string & OutputFile, pnapi::PetriNet & Petrinet, const string InputFile) {
	if (fileExists(string(OutputFile + ".owfn")) && args_info.noOverride_flag) {
		abort(5, "file %s already exists", _cfilename_(OutputFile + ".owfn"));
	}

	std::ofstream outStream;

	outStream.open(string(OutputFile + ".owfn").c_str(), std::ios_base::trunc);
	if (outStream.fail()) {
		abort(3, "file %s cannot be opened", _cfilename_(OutputFile + ".owfn"));
	}

	outStream << pnapi::io::owfn
				<< meta(pnapi::io::INPUTFILE, InputFile)
                << meta(pnapi::io::CREATOR, PACKAGE_STRING)
                << meta(pnapi::io::INVOCATION, invocation)
				<< Petrinet;
	outStream.close();

	status("file %s created", _cfilename_(OutputFile + ".owfn"));
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

    /*----------------------------------------.
    | 2. specific							  |
    `----------------------------------------*/
	string fileName;	
	
	if (args_info.inputs_num == 0) {
		fileName = "STDIN";
	}
	else {
		fileName = string(args_info.inputs[0]);
	}

	if (args_info.canonicalNames_flag) {
		net.canonicalNames();
	}

	if (net.getRoles().size() < 2) {
		message(_cimportant_("workflow contains less than 2 roles"));
		message(_cgood_("workflow decomposition successfull"));
		return EXIT_SUCCESS;
	}
	
	string outputFileName;
	bool retOK;
	validStatus_e fragmentStatus;
	
	retOK = true;

	Fragmentation f(net);

	if (!f.buildRoleFragments()) {
		message(_cbad_("worklfow decomposition failed"));
		retOK = false;
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
	
	if (retOK) {
		if (!f.processUnassignedFragments()) {
			message(_cbad_("worklfow decomposition failed"));
			retOK = false;
		}		
	}

	if (retOK) {
		fragmentStatus = f.isFragmentationValid(true);
		assert(fragmentStatus != VALID_TODO);
		if (fragmentStatus == VALID_BAD) {
			message(_cbad_("worklfow decomposition failed"));
			retOK = false;
		}
	}

	if (retOK) {
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

		retOK = f.buildServices();

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

		fragmentStatus = f.isFragmentationValid(false);
		assert(fragmentStatus != VALID_TODO);
		if (fragmentStatus == VALID_BAD) {
			message(_cbad_("worklfow decomposition failed"));
			retOK = false;
		}
		else {
			message(_cgood_("workflow decomposition successfull"));
		}

		if (args_info.output_given && retOK) {
			roleName2RoleID_t  roles = f.getRoles();
			for (roleName2RoleID_t::const_iterator curRole=roles.begin(); curRole!=roles.end(); ++curRole) {
				if (f.isRoleEmpty(curRole->second)) {
					status("%s %s %s", _cwarning_("role"), _cwarning_(curRole->first), _cwarning_("is empty"));
				}
				else {
					pnapi::PetriNet roleNet = f.createPetrinetByRoleID(curRole->second);			
					if (!args_info.noNormalization_flag) {
						status("normalize service %s...", curRole->first.c_str());
						roleNet.normalize();
						status("..done");
					}
					if (args_info.output_arg != NULL) {
						outputFileName = string(args_info.output_arg + curRole->first);
					}
					else {
						outputFileName = string(fileName + "_Service-" + curRole->first);
					}
					createOWFNFile(outputFileName, roleNet, fileName);
					if (args_info.dotServices_flag) {
						createDotFile(outputFileName, roleNet, fileName);
					}
				}
			}
		}

	}

	if (args_info.resultFile_given) {
        std::string results_filename = args_info.resultFile_arg ? args_info.resultFile_arg : fileName + ".results";
		if (fileExists(results_filename)  && args_info.noOverride_flag) {
			abort(5, "file %s already exists", _cfilename_(results_filename));
		}

        Results results(results_filename);

		results.add("decomposition.success", retOK);
		results.add("decomposition.interface_corrected_places", (unsigned int)f.getInterfaceCorrections());
		results.add("decomposition.bounded_transitions", (unsigned int)f.getBoundnessCorrections());
		results.add("decomposition.or_bounded_transitions", (unsigned int)f.getBoundnessOrConnections());
		results.add("decomposition.and_bounded_transitions", (unsigned int)f.getBoundnessAndConnections());
		results.add("decomposition.mergings", (unsigned int)f.getMergings());
		results.add("decomposition.fragment_connections", (unsigned int)f.getFragmentConnections());
		results.add("decomposition.arcweight_increased", (unsigned int)f.getArcweightCorrections());
		results.add("decomposition.initial_marked_places", (unsigned int)f.getInitialMarkings());
		results.add("decomposition.selfreactivating_transitions", (unsigned int)f.getSelfreactivatings());
		results.add("decomposition.places_inserted", (unsigned int)f.getPlacesInsert());
		results.add("decomposition.transitions_inserted", (unsigned int)f.getTransitionsInsert());
		results.add("decomposition.arcs_inserted", (unsigned int)f.getArcsInsert());
		results.add("decomposition.places_deleted", (unsigned int)f.getPlacesDelete());
		results.add("decomposition.transitions_deleted", (unsigned int)f.getTransitionsDelete());
		results.add("decomposition.arcs_deleted", (unsigned int)f.getArcsDelete());	
		results.add("decomposition.roles_annotated", (unsigned int)f.getRolesAnnotated());
		results.add("decomposition.concatenation_necessary", f.getConcatenateAnnotationNecessary());	
		results.add("decomposition.diane_calls", (unsigned int)f.getDianeCalls());
		results.add("decomposition.not_annotated_transitions", (unsigned int)f.getNotAnnotatedTransitions());
		results.add("decomposition.annotations_forced", (unsigned int)f.getDianeForces());
		results.add("decomposition.annotation_alternatives", (unsigned int)f.getDianeAlternatives());
		results.add("decomposition.lola_called", f.getLolaCalled());

		results.add("meta.package_name", (char*)PACKAGE_NAME);
        results.add("meta.package_version", (char*)PACKAGE_VERSION);
        results.add("meta.svn_version", (char*)VERSION_SVN);
        results.add("meta.invocation", invocation);

		results.add("workflow.roles", (unsigned int)net.getRoles().size());
		results.add("workflow.places", (unsigned int)(net.getPlaces().size() - f.getPlacesInsert() + f.getPlacesDelete()));
		results.add("workflow.transitions", (unsigned int)(net.getTransitions().size() - f.getTransitionsInsert() + f.getTransitionsDelete()));
		results.add("workflow.arcs", (unsigned int)(net.getArcs().size() - f.getArcsInsert() + f.getArcsDelete()));
		results.add("workflow.isFreeChoice", f.isFreeChoice());
		results.add("workflow.hasCycles", f.hasCycles());

    }
	
    return EXIT_SUCCESS;
}
