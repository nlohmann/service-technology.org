/*****************************************************************************\
 Anica -- Automated Non-Interference Check Assistent

 Copyright (c) 2011 Andreas Lehmann

 Anica is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Anica is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Anica.  If not, see <http://www.gnu.org/licenses/>.
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
#include "util.h"
#include "Results.h"

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// a variable holding the time of the call
clock_t start_clock = clock();


/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
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
            abort(10, "error reading configuration file '%s'", args_info.config_arg);
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
                abort(10, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", _cfilename_(conf_filename));
            }
        } else {
            status("not using a configuration file");
        }
    }


    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }

	// check dependencies
	bool checkPotentialCausal = (args_info.potentialPlaces_arg != potentialPlaces_arg_conflict);
	bool checkPotentialConflict = (args_info.potentialPlaces_arg != potentialPlaces_arg_causal);
	bool checkActiveCausal = ((args_info.activePlaces_arg == activePlaces_arg_causal) || (args_info.activePlaces_arg == activePlaces_arg_both));
	bool checkActiveConflict = ((args_info.activePlaces_arg == activePlaces_arg_conflict) || (args_info.activePlaces_arg == activePlaces_arg_both));

	if (checkActiveCausal && !checkPotentialCausal) {
		abort(7, "activeCausal requires potentialCausal");
	}
	if (checkActiveConflict && !checkPotentialConflict) {
		abort(7, "activeConflict requires potentialConflict");
	}
	if (args_info.dotActive_given && args_info.modus_arg == modus_arg_makefile) {
		status(_cwarning_("activePlaces requieres modus=lola: no dot-file will be created."));
	}

    free(params);
}

void createDotFile(const std::string & OutputFile, pnapi::PetriNet & Petrinet, const std::string & InputFile) {
	std::ofstream outStream;

	outStream.open(std::string(OutputFile + ".dot").c_str(), std::ios_base::trunc);
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


bool callLoLA(std::string netFile) {

	time_t start_time, end_time;
	std::string outputParam;
	std::string fileName;
	std::string buffer;

	buffer = std::string(args_info.tmpfile_arg);

	// for piping LoLA's output, we use " 2> " to read from std::cerr
    outputParam += " 2> ";
    // create a temporary file
#if defined(__MINGW32__)
    fileName = mktemp(basename(args_info.tmpfile_arg));
#else
    fileName = mktemp(args_info.tmpfile_arg);
#endif
	strcpy(args_info.tmpfile_arg, buffer.c_str());

    // add the output filename
    outputParam += fileName;


    // select LoLA binary and build LoLA command
#if defined(__MINGW32__)
    // MinGW does not understand pathnames with "/", so we use the basename
    std::string command_line = "\"" + std::string(args_info.lola_arg);
#else
    std::string command_line(args_info.lola_arg);
#endif

	command_line += " " + netFile;
	if (args_info.witnessPath_given) {
		command_line += " -p";
	}
    command_line += outputParam;

    status("........calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    time(&start_time);
	FILE *fp = popen(command_line.c_str(), "r");
	pclose(fp);
    time(&end_time);
    status("........%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));

	std::fstream f;
    f.open(fileName.c_str(), std::ios::in);
	std::string curLine;
	size_t foundState = std::string::npos;
	size_t foundNoState = std::string::npos;
    while (!f.eof() && (foundState == std::string::npos && foundNoState == std::string::npos)) {
		getline(f, curLine);
		foundState = curLine.find("state found!");
		foundNoState = curLine.find("state not found!");
    }
	f.close();

	if (!args_info.noClean_flag) {
		if (remove(fileName.c_str()) != 0) {
			abort(5, "file %s cannot be deleted", _cfilename_(fileName));
		}
	}

	if (foundState != std::string::npos && foundNoState == std::string::npos) {
		return true;
	}
	else if (foundNoState != std::string::npos && foundState == std::string::npos) {
		return false;
	}
	else {
		abort(9, "LoLA parse error");
	}
}

/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
		int ret = system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
		if (ret == 12071983) {
			status("compiler satisfied");
		}
    }

	cmdline_parser_free(&args_info);
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
            std::cin >> pnapi::io::lola >> net;
        } else {
            // strip suffix from input filename (if necessary: uncomment next line for future use)
            //inputFilename = std::string(args_info.inputs[0]).substr(0, std::string(args_info.inputs[0]).find_last_of("."));

            // open input file as an input file stream
            std::ifstream inputStream(args_info.inputs[0]);
            // ... and abort, if an error occurs
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }

            // parse the open net from the input file stream
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                        >> pnapi::io::lola >> net;
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
        abort(2, "\b%s", s.str().c_str());
    }

    /*---------------------------------.
    | 2. set parameters      		   |
    `---------------------------------*/

	std::string fileName;

	if (args_info.inputs_num == 0) {
		fileName = "STDIN";
	}
	else {
		fileName = std::string(args_info.inputs[0]);
	}

	bool checkPotentialCausal = (args_info.potentialPlaces_arg != potentialPlaces_arg_conflict);
	bool checkPotentialConflict = (args_info.potentialPlaces_arg != potentialPlaces_arg_causal);
	bool checkActiveCausal = ((args_info.activePlaces_arg == activePlaces_arg_causal) || (args_info.activePlaces_arg == activePlaces_arg_both));
	bool checkActiveConflict = ((args_info.activePlaces_arg == activePlaces_arg_conflict) || (args_info.activePlaces_arg == activePlaces_arg_both));

	std::set<pnapi::Place *> potentialCausal;
	std::set<pnapi::Place *> potentialConflict;
	std::set<pnapi::Place *> activeCausal;
	std::set<pnapi::Place *> activeConflict;

	/*---------------------------------.
    | 3. handle confidence			   |
    `---------------------------------*/

	unsigned int highTransitions = 0;
	unsigned int lowTransitions = 0;
	unsigned int curConfidence;

	PNAPI_FOREACH(t, net.getTransitions()) {
		curConfidence = (**t).getConfidence();
		if (curConfidence == 0) {
			// unlabeled transition
			if (args_info.unlabeledTransitions_arg == unlabeledTransitions_arg_none) {
				abort(6, "%s has unknown confidence level", (**t).getName().c_str());
			}
			else if (args_info.unlabeledTransitions_arg == unlabeledTransitions_arg_low) {
				(**t).setConfidence(1);
				lowTransitions++;
			}
			else if (args_info.unlabeledTransitions_arg == unlabeledTransitions_arg_high) {
				(**t).setConfidence(2);
				highTransitions++;
			}
		}
		else if (curConfidence == 1) {lowTransitions++;}
		else if (curConfidence == 2) {highTransitions++;}
	}

	if (lowTransitions == 0 || highTransitions == 0) {
		status("only one confidence level used");
		message(_cgood_("Non-Interference: PASSED"));
		return EXIT_SUCCESS;
	}

	if (args_info.dotConfidence_given) {
		PNAPI_FOREACH(t, net.getTransitions()) {
			(**t).setColor("");
			if ((**t).getConfidence() == 1 && args_info.dotConfidence_arg != dotConfidence_arg_high) {
				(**t).setColor("green");
			}
			if ((**t).getConfidence() == 2 && args_info.dotConfidence_arg != dotConfidence_arg_low) {
				(**t).setColor("blue");
			}
		}
		createDotFile(std::string(fileName + ".confidence"), net, fileName);
	}

	/*---------------------------------.
    | 4. gather potential places	   |
    `---------------------------------*/

	std::multimap<pnapi::Place *, pnapi::Transition *> highPre;
	std::multimap<pnapi::Place *, pnapi::Transition *> lowPost;
	std::multimap<pnapi::Place *, pnapi::Transition *> highPost;

	status("check potential places");
	PNAPI_FOREACH(p, net.getPlaces()) {
		status("..%s", (**p).getName().c_str());
		
		std::set<pnapi::Transition *> lHighPre;
		std::set<pnapi::Transition *> lHighPost;
		std::set<pnapi::Transition *> lLowPost;
		
		if (checkPotentialCausal) {
			PNAPI_FOREACH(t, (**p).getPreset()) {
				//status("....%s", (**t).getName().c_str());
				if (((pnapi::Transition *)(*t))->getConfidence() == 2) {
					lHighPre.insert((pnapi::Transition *)*t);
					//status("......high preset");
				}
			}
		}

		PNAPI_FOREACH(t, (**p).getPostset()) {
			//status("....%s", (**t).getName().c_str());
			if (((pnapi::Transition *)(*t))->getConfidence() == 2) {
				lHighPost.insert((pnapi::Transition *)*t);
				//status("......high postset");
			}
			else {
				lLowPost.insert((pnapi::Transition *)*t);
				//status("......low postset");
			}
		}

		bool lowPostDone = false;

		if (checkPotentialCausal) {
			if (!lHighPre.empty() && !lLowPost.empty()) {
				status(_cwarning_("....potential causal"));
				potentialCausal.insert(*p);
				FOREACH(t, lHighPre) {
					highPre.insert(std::make_pair(*p, *t));
				}
				FOREACH(t, lLowPost) {
					lowPost.insert(std::make_pair(*p, *t));
				}
				lowPostDone = true;
			}
		}

		if (checkPotentialConflict) {
			if (!lHighPost.empty() && !lLowPost.empty()) {
				status(_cwarning_("....potential conflict"));
				potentialConflict.insert(*p);
				FOREACH(t, lHighPost) {
					highPost.insert(std::make_pair(*p, *t));
				}
				if (!lowPostDone) {
					FOREACH(t, lLowPost) {
						lowPost.insert(std::make_pair(*p, *t));
					}
				}
			}
		}

	}

	if (args_info.dotPotential_given) {
		PNAPI_FOREACH(p, net.getPlaces()) {
			(**p).setColor("");
			if (args_info.dotPotential_arg != dotPotential_arg_conflict) {
				// causal interesting
				if (potentialCausal.find(*p) != potentialCausal.end()) {
					// causal confirmed
					(**p).setColor("yellow");
					if (args_info.dotPotential_arg == dotPotential_arg_both) {
						if (potentialConflict.find(*p) != potentialConflict.end()) {
							(**p).setColor("orange");
						}
					}
				}
			}
			else {
				if (potentialConflict.find(*p) != potentialConflict.end()) {
					(**p).setColor("red");
				}
			}
		}
		createDotFile(std::string(fileName + ".potential"), net, fileName);
	}

	/*---------------------------------.
    | 5. check potential places		   |
    `---------------------------------*/

	std::set<std::string> allNets;

	if ((checkActiveCausal || checkActiveConflict) && (!potentialCausal.empty() || !potentialConflict.empty())) {

		status("check active places");

		std::multimap<pnapi::Place *, pnapi::Transition *>::iterator itHigh;
	  	std::pair<std::multimap<pnapi::Place *, pnapi::Transition *>::iterator,std::multimap<pnapi::Place *, pnapi::Transition *>::iterator> retHigh;
		std::multimap<pnapi::Place *, pnapi::Transition *>::iterator itLow;
	  	std::pair<std::multimap<pnapi::Place *, pnapi::Transition *>::iterator,std::multimap<pnapi::Place *, pnapi::Transition *>::iterator> retLow;

		if (checkActiveCausal) {
		status("..potential causal places");
			FOREACH(p, potentialCausal) {

				if (args_info.oneActiveOnly_flag && (!activeCausal.empty() || !activeConflict.empty())) {break;}

				status("....%s (%d combination(s))", (**p).getName().c_str(), highPre.count(*p) * lowPost.count(*p));

				retHigh = highPre.equal_range(*p);
				for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {

					retLow = lowPost.equal_range(*p);
					for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {
						status("......%s, %s", (*itHigh).second->getName().c_str(), (*itLow).second->getName().c_str());

						pnapi::PetriNet cNet(net);
						pnapi::Transition * lowCopy = &cNet.createTransition();
						pnapi::Transition * highCopy = &cNet.createTransition();
						pnapi::Place * preHigh = &cNet.createPlace("", 1);
						pnapi::Place * postHigh = &cNet.createPlace();
						pnapi::Place * goal = &cNet.createPlace();

						pnapi::Place * curPlace = cNet.findPlace((**p).getName());
						pnapi::Transition * curHigh = cNet.findTransition((*itHigh).second->getName());
						pnapi::Transition * curLow = cNet.findTransition((*itLow).second->getName());

				
						PNAPI_FOREACH(prePlace, curLow->getPreset()) {
							cNet.createArc(**prePlace, *lowCopy);
						}

						cNet.createArc(*preHigh, *curHigh);
						cNet.createArc(*curHigh, *postHigh);
						cNet.createArc(*highCopy, *preHigh);
						cNet.createArc(*curPlace, *highCopy);
						cNet.createArc(*postHigh, *highCopy);
						cNet.createArc(*postHigh, *lowCopy);
						cNet.createArc(*lowCopy, *goal);
				
						std::string curFileName;
						curFileName = fileName + ".causal." + (*curPlace).getName() + "." + (*curHigh).getName() + "." + (*curLow).getName() + ".lola";

						std::ofstream o;
						o.open(curFileName.c_str(), std::ios_base::trunc);
						o << pnapi::io::lola << cNet;
						o << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";
						o.close();

						allNets.insert(curFileName);

						if (args_info.modus_arg == modus_arg_lola) {
							// call LoLA
							if (callLoLA(curFileName)) {
								status(_cbad_("........active"));
								activeCausal.insert(*p);
							}
							else {
								status(_cgood_("........not active"));
							}
						}

					} // for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {

				} // for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {

			} // FOREACH(p, potentialCausal) {

		} // activeCausal

		if (checkActiveConflict) {
			status("..potential conflict places");
			FOREACH(p, potentialConflict) {

				if (args_info.oneActiveOnly_flag && (!activeCausal.empty() || !activeConflict.empty())) {break;}

				status("....%s (%d combination(s))", (**p).getName().c_str(), highPost.count(*p) * lowPost.count(*p));
		
				retHigh = highPost.equal_range(*p);
				for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {

					retLow = lowPost.equal_range(*p);
					for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {
						status("......%s, %s", (*itHigh).second->getName().c_str(), (*itLow).second->getName().c_str());

						pnapi::PetriNet cNet(net);
						pnapi::Transition * lowCopy = &cNet.createTransition();
						pnapi::Transition * highCopy = &cNet.createTransition();
						pnapi::Place * proHigh = &cNet.createPlace();
						pnapi::Place * notHigh = &cNet.createPlace("", 1);
						pnapi::Place * goal = &cNet.createPlace();

						pnapi::Place * curPlace = cNet.findPlace((**p).getName());
						pnapi::Transition * curHigh = cNet.findTransition((*itHigh).second->getName());
						pnapi::Transition * curLow = cNet.findTransition((*itLow).second->getName());

						std::set< std::pair<pnapi::Node *, pnapi::Node *> > newArcs;

						PNAPI_FOREACH(postTransition, curPlace->getPostset()) {
							newArcs.insert(std::make_pair(*postTransition, notHigh));
							newArcs.insert(std::make_pair(notHigh, *postTransition));
						}

						PNAPI_FOREACH(prePlace, curLow->getPreset()) {
							newArcs.insert(std::make_pair(*prePlace, lowCopy));
						}

						PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
							newArcs.insert(std::make_pair(*prePlace, highCopy));
							newArcs.insert(std::make_pair(highCopy, *prePlace));
						}

						FOREACH(a, newArcs) {
							cNet.createArc(*a->first, *a->second);
						}

						cNet.createArc(*highCopy, *proHigh);
						cNet.createArc(*proHigh, *lowCopy);
						cNet.createArc(*notHigh, *highCopy);
						cNet.createArc(*lowCopy, *goal);

						std::string curFileName;
						curFileName = fileName + ".conflict." + (*curPlace).getName() + "." + (*curHigh).getName() + "." + (*curLow).getName() + ".lola";

						std::ofstream o;
						o.open(curFileName.c_str(), std::ios_base::trunc);
						o << pnapi::io::lola << cNet;
						o << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";
						o.close();

						allNets.insert(curFileName);

						if (args_info.modus_arg == modus_arg_lola) {
							// call LoLA
							if (callLoLA(curFileName)) {
								status(_cbad_("........active"));
								activeConflict.insert(*p);
							}
							else {
								status(_cgood_("........not active"));
							}
						}

					} // for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {

				} // for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {

			} // FOREACH(p, potentialConflict) {

		} // activeCausal

		if (args_info.modus_arg == modus_arg_makefile) {
			// create makefile
			std::ofstream o;
			o.open(std::string(fileName + ".makefile").c_str(), std::ios_base::trunc);

			o << ".PHONY : clean veryclean\n\n";
    		o << "SED = sed\n\n";
    		o << "nets = ";
    		FOREACH(f, allNets) {
				o << basename(const_cast<char *>((*f).c_str())) << " ";
			}
			o << "\n\n";

			// display help
			o << "all: activeplaces\n";

			// active places using statepredicate
			o << "\n##############################################################################\n";
			o << "# check for active places by checking whether a marking is reachable\n";
			o << "##############################################################################\n";
			o << "activeplaces: $(nets:%=%.result)\n";
			o << "%.result:\n";
			if (args_info.witnessPath_given) {
				o << "\t@echo \"ANALYSE NET $(@:%.result=%) -p\"\n";
				o << "\t-@" << args_info.lola_arg << " $(@:%.result=%) -p 2> $@ ; echo \"RESULT: $$?\" >> $@\n\n";
			}
			else {
				o << "\t@echo \"ANALYSE NET $(@:%.result=%)\"\n";
				o << "\t-@" << args_info.lola_arg << " $(@:%.result=%) 2> $@ ; echo \"RESULT: $$?\" >> $@\n\n";
			}
			
			// clean up
			o << "\n##############################################################################\n";
			o << "# clean up\n";
			o << "##############################################################################\n";
			o << "clean:\n";
			o << "\nveryclean: clean\n";
			//o << "\t@$(RM) $(net) $(MAKEFILE_LIST)\n";
			//o << "\t@$(RM) $(net:.lola=).*.lola\n";

			o.close();

			message("%s %s %s", _cgood_("makefile"), _cfilename_(std::string(fileName + ".makefile")), _cgood_("created"));

		} // (args_info.modus_arg == modus_arg_makefile) {
		else {
			if (args_info.dotActive_given) {
				PNAPI_FOREACH(p, net.getPlaces()) {
					(**p).setColor("");
					if (args_info.dotActive_arg != dotActive_arg_conflict) {
						// causal interesting
						if (activeCausal.find(*p) != activeCausal.end()) {
							// causal confirmed
							(**p).setColor("yellow");
							if (args_info.dotActive_arg == dotActive_arg_both) {
								if (activeConflict.find(*p) != activeConflict.end()) {
									(**p).setColor("orange");
								}
							}
						}
					}
					else {
						if (activeConflict.find(*p) != activeConflict.end()) {
							(**p).setColor("red");
						}
					}
				}
				createDotFile(std::string(fileName + ".active"), net, fileName);
			}
		}

	} // if (checkActiveCausal || checkActiveConflict) {

	/*---------------------------------.
    | 6. finishing					   |
    `---------------------------------*/

	if (args_info.modus_arg == modus_arg_lola) {

		// clean up after active places
		if (!args_info.noClean_flag) {
			FOREACH(f, allNets) {
				if (remove((*f).c_str()) != 0) {
					abort(5, "file %s cannot be deleted", _cfilename_(*f));
				}
			}
		}

		int result = 2;
		if (checkActiveCausal || checkActiveConflict) {
			if (!activeCausal.empty() || !activeConflict.empty()) {result = 0;}
			else {
				if (!checkActiveCausal && !potentialCausal.empty()) {result = 1;}
				if (!checkActiveConflict && !potentialConflict.empty()) {result = 1;}
			}
		}
		else {
			if (!potentialCausal.empty() || !potentialConflict.empty()) {result = 1;}
		}
		
		if (args_info.resultFile_given) {
		    std::string results_filename = args_info.resultFile_arg ? args_info.resultFile_arg : fileName + ".results";

		    Results results(results_filename);

			if (result == 0) {results.add("result.non-interference", (char*)"FAILED");}
			if (result == 1) {results.add("result.non-interference", (char*)"POTENTIAL");}
			if (result == 2) {results.add("result.non-interference", (char*)"PASSED");}

			results.add("places.potential_causal", (unsigned int)potentialCausal.size());
			results.add("places.potential_conflict", (unsigned int)potentialConflict.size());
			results.add("places.active_causal", (unsigned int)activeCausal.size());
			results.add("places.active_conflict", (unsigned int)activeConflict.size());

			results.add("meta.package_name", (char*)PACKAGE_NAME);
		    results.add("meta.package_version", (char*)PACKAGE_VERSION);
		    results.add("meta.svn_version", (char*)VERSION_SVN);
		    results.add("meta.invocation", invocation);

			results.add("net.places", (unsigned int)(net.getPlaces().size()));
			results.add("net.transitions", (unsigned int)(net.getTransitions().size()));
			results.add("net.high", highTransitions);
			results.add("net.low", lowTransitions);
			results.add("net.arcs", (unsigned int)(net.getArcs().size()));
		}

		if (result == 0) {message(_cbad_("Non-Interference: FAILED"));}
		if (result == 1) {message(_cwarning_("Non-Interference: POTENTIAL"));}
		if (result == 2) {message(_cgood_("Non-Interference: PASSED"));}

	}

    return EXIT_SUCCESS;
}
