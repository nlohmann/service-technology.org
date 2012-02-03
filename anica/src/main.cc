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
const clock_t start_clock = clock();

/// constants for security labeling
const int CONFIDENCE_NONE = 0;
const int CONFIDENCE_LOW = 1;
const int CONFIDENCE_HIGH = 2;
const int CONFIDENCE_DOWN = 3;

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

	// set LoLA if it is present in the environment and not set otherwise
    if (getenv("LOLA") != NULL and not args_info.lola_given) {
        status("using environment variable '%s' set to '%s'",
            _cfilename_("LOLA"), _cfilename_(getenv("LOLA")));
        free(args_info.lola_arg);
        args_info.lola_arg = strdup(getenv("LOLA"));
    }
    
    // set Wendy if it is present in the environment and not set otherwise
    if (getenv("WENDY") != NULL and not args_info.wendy_given) {
        status("using environment variable '%s' set to '%s'",
            _cfilename_("WENDY"), _cfilename_(getenv("WENDY")));
        free(args_info.lola_arg);
        args_info.lola_arg = strdup(getenv("WENDY"));
    }

    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }

	// check dependencies
    if ((args_info.task_arg != task_arg_verfification) && (args_info.property_arg != property_arg_PBNIPLUS_)) {
		abort(7, "task and property combination is not supported");
	}

	const bool checkPotentialCausal = (args_info.potentialPlaces_arg != potentialPlaces_arg_conflict);
	const bool checkPotentialConflict = (args_info.potentialPlaces_arg != potentialPlaces_arg_causal);
	const bool checkActiveCausal = ((args_info.activePlaces_arg == activePlaces_arg_causal) || (args_info.activePlaces_arg == activePlaces_arg_both));
	const bool checkActiveConflict = ((args_info.activePlaces_arg == activePlaces_arg_conflict) || (args_info.activePlaces_arg == activePlaces_arg_both));

	if (checkActiveCausal && !checkPotentialCausal) {
		abort(7, "activeCausal requires potentialCausal");
	}
	if (checkActiveConflict && !checkPotentialConflict) {
		abort(7, "activeConflict requires potentialConflict");
	}
	
	//if ((args_info.task_arg == task_arg_characterization) && (args_info.modus_arg == mdous_arg_call)) {
	//	status(_cwarning_("modus=call makes no sense with task characterization: BUT will be performed."));
	//}
	if ((args_info.task_arg != task_arg_verfification) && (args_info.unlabeledTransitions_arg != unlabeledTransitions_arg_none)) {
		status(_cwarning_("unlabeledTransitions makes no sense with current task: BUT will be performed."));
	}
	if ((args_info.task_arg != task_arg_verfification) && (checkActiveCausal || checkActiveConflict)) {
		status(_cwarning_("activePlaces have no influence on this task: parameter will be ignored."));
	}
	if ((args_info.task_arg != task_arg_verfification) && args_info.oneActiveOnly_flag) {
		status(_cwarning_("oneActiveOnly has no influence on this task: parameter will be ignored."));
	}
	if ((args_info.task_arg != task_arg_verfification) && args_info.oneTripleOnly_flag) {
		status(_cwarning_("oneTripleOnly has no influence on this task: parameter will be ignored."));
	}
	if (args_info.dotActive_given && args_info.modus_arg == modus_arg_makefile) {
		status(_cwarning_("activePlaces requieres modus=call: no dot-file will be created."));
	}

    free(params);
}

void clearColors(pnapi::PetriNet & Petrinet) {
    PNAPI_FOREACH(node, Petrinet.getNodes()) {
        (**node).setColor("");
    }
}

void createDotFile(const std::string & OutputFile, const pnapi::PetriNet & Petrinet, const std::string & InputFile) {
	std::ofstream outStream;

	outStream.open(std::string(OutputFile + ".dot").c_str(), std::ios_base::trunc);
	if (outStream.fail()) {
		abort(3, "file %s cannot be created", _cfilename_(OutputFile + ".dot"));
	}

	outStream << pnapi::io::dot
				<< meta(pnapi::io::INPUTFILE, InputFile)
                << meta(pnapi::io::CREATOR, PACKAGE_STRING)
                << meta(pnapi::io::INVOCATION, invocation)
				<< Petrinet;
	outStream.close();

	status("file %s created", _cfilename_(OutputFile + ".dot"));
}


inline bool callLoLA(const pnapi::PetriNet& net, const pnapi::Place * goal) {

	time_t start_time, end_time;
	
	std::stringstream ss;
	ss << pnapi::io::lola << net;
	ss << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";

	// select LoLA binary and build LoLA command 
    #if defined(__MINGW32__) 
    // // MinGW does not understand pathnames with "/", so we use the basename 
    const std::string command_line = "\"" + std::string(args_info.lola_arg) + (args_info.witnessPath_given ? " -p " : "") + (args_info.verbose_flag ? "" : " 2> nul"); 
    #else 
    const std::string command_line = std::string(args_info.lola_arg) + (args_info.witnessPath_given ? " -p " : "") + (args_info.verbose_flag ? "" : " 2> nul"); 
    #endif 

    status("........calling %s: '%s'", _ctool_("LoLA"), command_line.c_str());
    time(&start_time);
	FILE *fp = popen(command_line.c_str(), "w");
	if (fp == NULL) {
	    abort(700, "pipe cannot be created");
	}
	fprintf (fp, "%s", ss.str().c_str());
	const int ret = pclose(fp) / 256;
    time(&end_time);
    status("........%s is done [%.0f sec]", _ctool_("LoLA"), difftime(end_time, start_time));

	if (ret == 0) {
		return true;
	}
	else if (ret == 1) {
		return false;
	}
	else {
		abort(9, "LoLA parse error");
	}
	return true;
}

bool callWendy(const pnapi::PetriNet& net) {

	time_t start_time, end_time;
	
	std::stringstream ss;
	ss << pnapi::io::owfn << net;

	// select LoLA binary and build LoLA command 
    #if defined(__MINGW32__) 
    // // MinGW does not understand pathnames with "/", so we use the basename 
    const std::string command_line = "\"" + std::string(args_info.wendy_arg)  + (args_info.verbose_flag ? " -v " : "") + " --lola=" + std::string(args_info.lolaWendy_arg) + " --resultFile=-" + " | grep -q \"result = true\"";  
    #else 
    const std::string command_line = std::string(args_info.wendy_arg) + (args_info.verbose_flag ? " -v " : "") + " --lola=" + std::string(args_info.lolaWendy_arg) + " --resultFile=-" + " | grep -q \"result = true\"";  
    #endif 

    status("........calling %s: '%s'", _ctool_("Wendy"), command_line.c_str());
    time(&start_time);
	FILE *fp = popen(command_line.c_str(), "w");
	if (fp == NULL) {
	    abort(700, "pipe cannot be created");
	}
	fprintf (fp, "%s", ss.str().c_str());
	const int ret = pclose(fp) / 256;
    time(&end_time);
    status("........%s is done [%.0f sec]", _ctool_("Wendy"), difftime(end_time, start_time));

	if (ret == 0) {
		return true;
	}
	else if (ret == 1) {
		return false;
	}
	else {
		abort(9, "Wendy parse error");
	}
	return true;
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
		int ret = system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
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


    const bool checkPotentialCausal = (args_info.potentialPlaces_arg != potentialPlaces_arg_conflict);
    const bool checkPotentialConflict = (args_info.potentialPlaces_arg != potentialPlaces_arg_causal);
    const bool checkActiveCausal = ((args_info.activePlaces_arg == activePlaces_arg_causal) || (args_info.activePlaces_arg == activePlaces_arg_both));
    const bool checkActiveConflict = ((args_info.activePlaces_arg == activePlaces_arg_conflict) || (args_info.activePlaces_arg == activePlaces_arg_both));

    std::set<std::string> downgradeTransitions;
    std::set<pnapi::Place *> potentialCausal;
    std::set<pnapi::Place *> potentialConflict;
    std::set<pnapi::Place *> activeCausal;
    std::set<pnapi::Place *> activeConflict;

    std::multimap<pnapi::Place *, std::pair<pnapi::Transition *, pnapi::Transition *> > potentialCausalTriple;
    std::multimap<pnapi::Place *, std::pair<pnapi::Transition *, pnapi::Transition *> > potentialConflictTriple;
    std::multimap<pnapi::Place *, std::pair<pnapi::Transition *, pnapi::Transition *> > activeCausalTriple;
    std::multimap<pnapi::Place *, std::pair<pnapi::Transition *, pnapi::Transition *> > activeConflictTriple;
    

	/*---------------------------------.
    | 3. handle confidence			   |
    `---------------------------------*/

    unsigned int downTransitions = 0;
	unsigned int highTransitions = 0;
	unsigned int lowTransitions = 0;
	unsigned int curConfidence;

	PNAPI_FOREACH(t, net.getTransitions()) {
		curConfidence = (**t).getConfidence();
		if (curConfidence == CONFIDENCE_NONE) {
			// unlabeled transition
			if (args_info.unlabeledTransitions_arg == unlabeledTransitions_arg_none) {
				if (args_info.task_arg == task_arg_verfification) {
				    abort(6, "%s has unknown confidence level", (**t).getName().c_str());
				}
			}
			else if (args_info.unlabeledTransitions_arg == unlabeledTransitions_arg_low) {
			    (**t).setConfidence(CONFIDENCE_LOW);
			    lowTransitions++;
			}
			else if (args_info.unlabeledTransitions_arg == unlabeledTransitions_arg_high) {
			    (**t).setConfidence(CONFIDENCE_HIGH);
			    highTransitions++;
			}
			else if (args_info.unlabeledTransitions_arg == unlabeledTransitions_arg_down) {
			    (**t).setConfidence(CONFIDENCE_DOWN);
			    downTransitions++;
			    downgradeTransitions.insert((**t).getName());
			}
		}
		else if (curConfidence == CONFIDENCE_LOW) {lowTransitions++;}
		else if (curConfidence == CONFIDENCE_HIGH) {highTransitions++;}
		else if (curConfidence == CONFIDENCE_DOWN) {
		    downTransitions++;
		    downgradeTransitions.insert((**t).getName());
		}
	}

    if (args_info.task_arg == task_arg_verfification) {
	    if (lowTransitions == 0 || highTransitions == 0) {
	        status("trivial confidence level used");
	        message(_cgood_("Non-Interference: PASSED"));
	        // ToDo: Result?!
	        return EXIT_SUCCESS;
	    }
	}

    if ((args_info.task_arg != task_arg_verfification) && (downTransitions > 0)) {
		abort(800, "PBNID is not supported for your task");
    }
    
	if (args_info.dotConfidence_given) {
		PNAPI_FOREACH(t, net.getTransitions()) {
			if ((**t).getConfidence() == CONFIDENCE_LOW && ((args_info.dotConfidence_arg == dotConfidence_arg_low) || (args_info.dotConfidence_arg = dotConfidence_arg_all)) ) {
				(**t).setColor("green");
			}
			if ((**t).getConfidence() == CONFIDENCE_HIGH && ((args_info.dotConfidence_arg == dotConfidence_arg_high) || (args_info.dotConfidence_arg = dotConfidence_arg_all)) ) {
				(**t).setColor("red");
			}
			if ((**t).getConfidence() == CONFIDENCE_DOWN && ((args_info.dotConfidence_arg == dotConfidence_arg_down) || (args_info.dotConfidence_arg = dotConfidence_arg_all)) ) {
				(**t).setColor("yellow");
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

    // ToDo: einheitlich verwenden
    if (args_info.task_arg == task_arg_verfification) {

	    status("check potential places");
	    PNAPI_FOREACH(p, net.getPlaces()) {
		    status("..%s", (**p).getName().c_str());
		
		    std::set<pnapi::Transition *> lHighPre;
		    std::set<pnapi::Transition *> lHighPost;
		    std::set<pnapi::Transition *> lLowPost;
		
		    if (checkPotentialCausal) {
			    PNAPI_FOREACH(t, (**p).getPreset()) {
				    if (((pnapi::Transition *)(*t))->getConfidence() == CONFIDENCE_HIGH) {
					    lHighPre.insert((pnapi::Transition *)*t);
				    }
			    }
		    }

		    PNAPI_FOREACH(t, (**p).getPostset()) {
			    if (((pnapi::Transition *)(*t))->getConfidence() == CONFIDENCE_HIGH) {
				    lHighPost.insert((pnapi::Transition *)*t);
			    }
			    else if (((pnapi::Transition *)(*t))->getConfidence() == CONFIDENCE_LOW) {
				    lLowPost.insert((pnapi::Transition *)*t);
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
	        if (args_info.dotConfidence_given) {clearColors(net);}
		    PNAPI_FOREACH(p, net.getPlaces()) {
			    if (args_info.dotPotential_arg != dotPotential_arg_conflict) {
				    // causal interesting
				    if (potentialCausal.find(*p) != potentialCausal.end()) {
					    // causal confirmed
					    (**p).setColor("blue");
					    if (args_info.dotPotential_arg == dotPotential_arg_both) {
						    if (potentialConflict.find(*p) != potentialConflict.end()) {
							    (**p).setColor("brown");
						    }
					    }
				    }
			    }
			    else {
				    if (potentialConflict.find(*p) != potentialConflict.end()) {
					    (**p).setColor("orange");
				    }
			    }
		    }
		    createDotFile(std::string(fileName + ".potential"), net, fileName);
	    }
	    
	} // if (!args_info.controller_flag) {

	/*---------------------------------.
    | 5. check potential places		   |
    `---------------------------------*/

    std::set<std::string> allNets;

    if (args_info.task_arg == task_arg_verfification) {

	    if ((checkActiveCausal || checkActiveConflict) && (!potentialCausal.empty() || !potentialConflict.empty())) {

		    status("check active places");
		    // ToDo: representative Namen berücksichtigen

		    std::multimap<pnapi::Place *, pnapi::Transition *>::iterator itHigh;
	      	std::pair<std::multimap<pnapi::Place *, pnapi::Transition *>::iterator,std::multimap<pnapi::Place *, pnapi::Transition *>::iterator> retHigh;
		    std::multimap<pnapi::Place *, pnapi::Transition *>::iterator itLow;
	      	std::pair<std::multimap<pnapi::Place *, pnapi::Transition *>::iterator,std::multimap<pnapi::Place *, pnapi::Transition *>::iterator> retLow;

            bool isActive;

		    if (checkActiveCausal) {
		    status("..potential causal places");
			    FOREACH(p, potentialCausal) {

				    if (args_info.oneActiveOnly_flag && (!activeCausal.empty() || !activeConflict.empty())) {break;}
                    isActive = false;

				    status("....%s (%d combination(s))", (**p).getName().c_str(), highPre.count(*p) * lowPost.count(*p));

				    retHigh = highPre.equal_range(*p);
				    for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {
                        if (args_info.oneTripleOnly_flag && isActive) {break;}
                    
					    retLow = lowPost.equal_range(*p);
					    for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {
					        if (args_info.oneTripleOnly_flag && isActive) {break;}
					
						    status("......%s, %s", (*itHigh).second->getName().c_str(), (*itLow).second->getName().c_str());
						    potentialCausalTriple.insert(std::make_pair(*p, std::make_pair((*itHigh).second, (*itLow).second)));

                            pnapi::PetriNet cNet(net);
                            
                            pnapi::Place * curPlace = cNet.findPlace((**p).getName());
						    pnapi::Transition * curHigh = cNet.findTransition((*itHigh).second->getName());
						    pnapi::Transition * curLow = cNet.findTransition((*itLow).second->getName());

						    pnapi::Transition * lowCopy = &cNet.createTransition();
						    pnapi::Transition * highCopy = &cNet.createTransition();
						    pnapi::Place * highFired = &cNet.createPlace();
						    pnapi::Place * goal = &cNet.createPlace();

                            std::set< std::pair<pnapi::Node *, pnapi::Node *> > newArcs;

                            PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
                                newArcs.insert(std::make_pair(*prePlace, highCopy));
                            }
                            PNAPI_FOREACH(postPlace, curHigh->getPostset()) {
                                newArcs.insert(std::make_pair(highCopy, *postPlace));
                            }
                            
                            PNAPI_FOREACH(prePlace, curLow->getPreset()) {
                                newArcs.insert(std::make_pair(*prePlace, lowCopy));
                            }

						    PNAPI_FOREACH(preTransition, curPlace->getPreset()) {
						        if (((**preTransition).getName() != (*curLow).getName()) && ( (**preTransition).getName() != (*curHigh).getName() )) {
						            pnapi::Place * cPlace = &cNet.createPlace("", 1);
						            newArcs.insert(std::make_pair(*preTransition, cPlace));
							        newArcs.insert(std::make_pair(cPlace, *preTransition));
							        newArcs.insert(std::make_pair(cPlace, highCopy));
							    }
						    }

                            //handle downgrading transitions
                            PNAPI_FOREACH(downTransition, downgradeTransitions) {
					            pnapi::Place * cPlace = &cNet.createPlace("", 1);
					            pnapi::Transition * cDown = (pnapi::Transition *)cNet.findNode(*downTransition);
					            newArcs.insert(std::make_pair(cDown, cPlace));
						        newArcs.insert(std::make_pair(cPlace, cDown));
						        newArcs.insert(std::make_pair(cPlace, highCopy));
						    }

                            FOREACH(a, newArcs) {
							    cNet.createArc(*a->first, *a->second);
						    }

						    cNet.createArc(*highCopy, *highFired);
						    cNet.createArc(*highFired, *lowCopy);
						    cNet.createArc(*lowCopy, *goal);
				
				            // ToDo: case Anweisung verwenden
				            if (args_info.modus_arg == modus_arg_makefile) {
						        std::string curFileName;
						        curFileName = fileName + ".causal." + (*curPlace).getName() + "." + (*curHigh).getName() + "." + (*curLow).getName() + ".lola";

						        std::ofstream o;
						        o.open(curFileName.c_str(), std::ios_base::trunc);
						        o << pnapi::io::lola << cNet;
						        o << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";
						        o.close();
						
                                status("........%s created", _cfilename_(curFileName));

						        allNets.insert(curFileName);
						    }

						    if (args_info.modus_arg == modus_arg_call) {
							    // call LoLA
							    if (callLoLA(cNet, goal)) {
								    status(_cbad_("........active"));
								    activeCausalTriple.insert(std::make_pair(*p, std::make_pair((*itHigh).second, (*itLow).second)));
								    isActive = true;
							    }
							    else {
								    status(_cgood_("........not active"));
							    }
						    }

					    } // for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {

				    } // for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {
				
				    if (isActive) {
				        status(_cbad_("......active"));
				        activeCausal.insert(*p);
				    }
				    else {
				        status(_cgood_("......not active"));
				    }

			    } // FOREACH(p, potentialCausal) {

		    } // activeCausal

		    if (checkActiveConflict) {
			    status("..potential conflict places");
			    FOREACH(p, potentialConflict) {

				    if (args_info.oneActiveOnly_flag && (!activeCausal.empty() || !activeConflict.empty())) {break;}
				    isActive = false;

				    status("....%s (%d combination(s))", (**p).getName().c_str(), highPost.count(*p) * lowPost.count(*p));
		
				    retHigh = highPost.equal_range(*p);
				    for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {
				        if (args_info.oneTripleOnly_flag && isActive) {break;}
				        
					    retLow = lowPost.equal_range(*p);
					    for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {
					        if (args_info.oneTripleOnly_flag && isActive) {break;}
					        
						    status("......%s, %s", (*itHigh).second->getName().c_str(), (*itLow).second->getName().c_str());
                            potentialConflictTriple.insert(std::make_pair(*p, std::make_pair((*itHigh).second, (*itLow).second)));

						    pnapi::PetriNet cNet(net);
                            
                            pnapi::Place * curPlace = cNet.findPlace((**p).getName());
						    pnapi::Transition * curHigh = cNet.findTransition((*itHigh).second->getName());
						    pnapi::Transition * curLow = cNet.findTransition((*itLow).second->getName());

						    pnapi::Transition * lowCopy = &cNet.createTransition();
						    pnapi::Transition * highCopy = &cNet.createTransition();
						    pnapi::Place * highFired = &cNet.createPlace();
						    pnapi::Place * highActivated = &cNet.createPlace("", 1);
						    pnapi::Place * goal = &cNet.createPlace();

                            std::set< std::pair<pnapi::Node *, pnapi::Node *> > newArcs;

                            PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
                                newArcs.insert(std::make_pair(*prePlace, highCopy));
                                newArcs.insert(std::make_pair(highCopy, *prePlace));
                            }
                            
                            PNAPI_FOREACH(prePlace, curLow->getPreset()) {
                                newArcs.insert(std::make_pair(*prePlace, lowCopy));
                            }

						    PNAPI_FOREACH(preTransition, curPlace->getPreset()) {
						        if (((**preTransition).getName() != (*curLow).getName()) && ( (**preTransition).getName() != (*curHigh).getName() )) {
						            pnapi::Place * cPlace = &cNet.createPlace("", 1);
						            newArcs.insert(std::make_pair(*preTransition, cPlace));
							        newArcs.insert(std::make_pair(cPlace, *preTransition));
							        newArcs.insert(std::make_pair(cPlace, highCopy));
							    }
						    }

                            //handle downgrading transitions
                            PNAPI_FOREACH(downTransition, downgradeTransitions) {
					            pnapi::Place * cPlace = &cNet.createPlace("", 1);
					            pnapi::Transition * cDown = (pnapi::Transition *)cNet.findNode(*downTransition);
					            newArcs.insert(std::make_pair(cDown, cPlace));
						        newArcs.insert(std::make_pair(cPlace, cDown));
						        newArcs.insert(std::make_pair(cPlace, highCopy));
						    }

                            FOREACH(a, newArcs) {
							    cNet.createArc(*a->first, *a->second);
						    }

						    cNet.createArc(*highCopy, *highFired);
						    cNet.createArc(*highFired, *lowCopy);
						    cNet.createArc(*lowCopy, *goal);
						    cNet.createArc(*highActivated, *curHigh);
						    cNet.createArc(*curHigh, *highActivated);
						    cNet.createArc(*highActivated, *highCopy);


                            // ToDo: case Anweisung verwenden
				            if (args_info.modus_arg == modus_arg_makefile) {
						        std::string curFileName;
						        curFileName = fileName + ".conflict." + (*curPlace).getName() + "." + (*curHigh).getName() + "." + (*curLow).getName() + ".lola";

						        std::ofstream o;
						        o.open(curFileName.c_str(), std::ios_base::trunc);
						        o << pnapi::io::lola << cNet;
						        o << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";
						        o.close();
						        status("........%s created", _cfilename_(curFileName));

						        allNets.insert(curFileName);
						    }

						    if (args_info.modus_arg == modus_arg_call) {
							    // call LoLA
							    if (callLoLA(cNet, goal)) {
								    status(_cbad_("........active"));
								    activeConflictTriple.insert(std::make_pair(*p, std::make_pair((*itHigh).second, (*itLow).second)));
								    isActive = true;
							    }
							    else {
								    status(_cgood_("........not active"));
							    }
						    }

					    } // for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {

				    } // for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {
				
				    if (isActive) {
				        status(_cbad_("......active"));
				        activeConflict.insert(*p);
				    }
				    else {
				        status(_cgood_("......not active"));
				    }

			    } // FOREACH(p, potentialConflict) {

		    } // activeCausal

            // ToDo: verschieben/anpassen
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
			        if (args_info.dotConfidence_given || args_info.dotPotential_given) {clearColors(net);}
				    PNAPI_FOREACH(p, net.getPlaces()) {
					    if (args_info.dotActive_arg != dotActive_arg_conflict) {
						    // causal interesting
						    if (activeCausal.find(*p) != activeCausal.end()) {
							    // causal confirmed
							    (**p).setColor("blue");
							    if (args_info.dotActive_arg == dotActive_arg_both) {
								    if (activeConflict.find(*p) != activeConflict.end()) {
									    (**p).setColor("brown");
								    }
							    }
						    }
					    }
					    else {
						    if (activeConflict.find(*p) != activeConflict.end()) {
							    (**p).setColor("orange");
						    }
					    }
				    }
				    createDotFile(std::string(fileName + ".active"), net, fileName);
			    }
		    }

	    } // if (checkActiveCausal || checkActiveConflict) {
	    
	} // if (!args_info.controller_flag) {

	/*---------------------------------.
    | 6. finishing					   |
    `---------------------------------*/

    if (args_info.task_arg == task_arg_characterization) {
        // falls representative Namen
        const std::string HIGH = "_high";
        const std::string LOW = "_low";
        const std::string GOAL = "goal_";

        unsigned int causalStructures = 0;
        unsigned int conflictStructures = 0;

        std::set< std::pair<pnapi::Node *, pnapi::Node *> > newArcs;
        std::string curFileName;
        std::ofstream o;
    
        // handle possible causal places
        PNAPI_FOREACH(p, net.getPlaces()) {
            status("handle place %s", (**p).getName().c_str());
            PNAPI_FOREACH(tv, (**p).getPreset()) {
                PNAPI_FOREACH(tn, (**p).getPostset()) {
                    status("..handle causal triple (%s, %s, %s)", (**p).getName().c_str(), (**tv).getName().c_str(), (**tn).getName().c_str());
                    // add currect causal case structure (p, tv, tn)
                    newArcs.clear();
                    // create new PetriNet
                    pnapi::PetriNet cNet(net);
                    causalStructures++;
			        pnapi::Place * curPlace = cNet.findPlace((**p).getName());
				    pnapi::Transition * curHigh = cNet.findTransition((**tv).getName());
				    pnapi::Transition * curLow = cNet.findTransition((**tn).getName());
				    pnapi::Transition * lowCopy = &cNet.createTransition();
				    pnapi::Transition * highCopy = &cNet.createTransition();
				    pnapi::Place * highFired = &cNet.createPlace();
				    pnapi::Place * goal = &cNet.createPlace(std::string(GOAL + (**p).getName() + "_" + (**tv).getName() + "_" + (**tn).getName()));
                    PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
                        newArcs.insert(std::make_pair(*prePlace, highCopy));
                    }
                    PNAPI_FOREACH(postPlace, curHigh->getPostset()) {
                        newArcs.insert(std::make_pair(highCopy, *postPlace));
                    }
                    PNAPI_FOREACH(prePlace, curLow->getPreset()) {
                        newArcs.insert(std::make_pair(*prePlace, lowCopy));
                    }
				    PNAPI_FOREACH(preTransition, curPlace->getPreset()) {
				        if (((**preTransition).getName() != (*curLow).getName()) && ( (**preTransition).getName() != (*curHigh).getName() )) {
				            pnapi::Place * cPlace = &cNet.createPlace("", 1);
				            newArcs.insert(std::make_pair(*preTransition, cPlace));
					        newArcs.insert(std::make_pair(cPlace, *preTransition));
					        newArcs.insert(std::make_pair(cPlace, highCopy));
					    }
				    }
				    newArcs.insert(std::make_pair(highCopy, highFired));
				    newArcs.insert(std::make_pair(highFired, lowCopy));
				    newArcs.insert(std::make_pair(lowCopy, goal));
				    
				    // add collected arcs
                    FOREACH(a, newArcs) {
		                cNet.createArc(*a->first, *a->second);
	                }
				    
				    // ToDo: nur falls != statistics
				    curFileName = fileName + ".causal." + (**p).getName() + "." + (*curHigh).getName() + "." + (*curLow).getName() + ".lola";

				    o.open(curFileName.c_str(), std::ios_base::trunc);
				    o << pnapi::io::lola << cNet;
				    o << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";
				    o.close();
				    status("........%s created", _cfilename_(curFileName));
				    
			    }
		    }
        }
    
    
        // handle possible conflict places
        std::set< std::pair<pnapi::Node *, pnapi::Node *> > done;
        PNAPI_FOREACH(p, net.getPlaces()) {
            status("handle place %s", (**p).getName().c_str());
            PNAPI_FOREACH(t1, (**p).getPostset()) {
                PNAPI_FOREACH(t2, (**p).getPostset()) {
                    if ((t1 != t2) && (done.find(std::make_pair(*t1, *t2)) == done.end())) {
                        done.insert(std::make_pair(*t1, *t2));
                        newArcs.clear();
                        status("..handle conflict triple (%s, %s, %s)", (**p).getName().c_str(), (**t1).getName().c_str(), (**t2).getName().c_str());
                        // create new PetriNet
                        pnapi::PetriNet cNet2(net);
                        conflictStructures++;
                        // add currect conflict case structure (p, t1, t2)
                        pnapi::Place * curPlace = cNet2.findPlace((**p).getName());
				        pnapi::Transition * curHigh = cNet2.findTransition((**t1).getName());
				        pnapi::Transition * curLow = cNet2.findTransition((**t2).getName());
				        pnapi::Transition * lowCopy = &cNet2.createTransition();
				        pnapi::Transition * highCopy = &cNet2.createTransition();
				        pnapi::Place * highFired = &cNet2.createPlace();
				        pnapi::Place * highActivated = &cNet2.createPlace("", 1);
				        pnapi::Place * goal = &cNet2.createPlace(std::string(GOAL + (**p).getName() + "_" + (**t1).getName() + "_" + (**t2).getName()));
                        PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
                            newArcs.insert(std::make_pair(*prePlace, highCopy));
                            newArcs.insert(std::make_pair(highCopy, *prePlace));
                        }
                        PNAPI_FOREACH(prePlace, curLow->getPreset()) {
                            newArcs.insert(std::make_pair(*prePlace, lowCopy));
                        }
				        PNAPI_FOREACH(preTransition, curPlace->getPreset()) {
				            if (((**preTransition).getName() != (*curLow).getName()) && ( (**preTransition).getName() != (*curHigh).getName() )) {
				                pnapi::Place * cPlace = &cNet2.createPlace("", 1);
				                newArcs.insert(std::make_pair(*preTransition, cPlace));
					            newArcs.insert(std::make_pair(cPlace, *preTransition));
					            newArcs.insert(std::make_pair(cPlace, highCopy));
					        }
				        }
				        cNet2.createArc(*highCopy, *highFired);
				        cNet2.createArc(*highFired, *lowCopy);
				        cNet2.createArc(*lowCopy, *goal);
				        cNet2.createArc(*highActivated, *curHigh);
				        cNet2.createArc(*curHigh, *highActivated);
				        cNet2.createArc(*highActivated, *highCopy);
				        
				        // add collected arcs
                        FOREACH(a, newArcs) {
		                    cNet2.createArc(*a->first, *a->second);
	                    }
				        
				        // ToDo: nur falls != statistics
				        curFileName = fileName + ".conflict." + (**p).getName() + "." + (*curHigh).getName() + "." + (*curLow).getName() + ".lola";

				        o.open(curFileName.c_str(), std::ios_base::trunc);
				        o << pnapi::io::lola << cNet2;
				        o << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";
				        o.close();
				        status("........%s created", _cfilename_(curFileName));
		                
		                done.insert(std::make_pair(*t2, *t1));
		                status("..handle conflict triple (%s, %s, %s)", (**p).getName().c_str(), (**t2).getName().c_str(), (**t1).getName().c_str());
				        newArcs.clear();
				        // create new PetriNet
                        pnapi::PetriNet cNet3(net);
				        conflictStructures++;
				        // add currect conflict case structure (p, t2, t1)
                        curPlace = cNet3.findPlace((**p).getName());
				        curHigh = cNet3.findTransition((**t2).getName());
				        curLow = cNet3.findTransition((**t1).getName());
				        lowCopy = &cNet3.createTransition();
				        highCopy = &cNet3.createTransition();
				        highFired = &cNet3.createPlace();
				        highActivated = &cNet3.createPlace("", 1);
				        goal = &cNet3.createPlace(std::string(GOAL + (**p).getName() + "_" + (**t2).getName() + "_" + (**t1).getName()));
                        PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
                            newArcs.insert(std::make_pair(*prePlace, highCopy));
                            newArcs.insert(std::make_pair(highCopy, *prePlace));
                        }
                        PNAPI_FOREACH(prePlace, curLow->getPreset()) {
                            newArcs.insert(std::make_pair(*prePlace, lowCopy));
                        }
				        PNAPI_FOREACH(preTransition, curPlace->getPreset()) {
				            if (((**preTransition).getName() != (*curLow).getName()) && ( (**preTransition).getName() != (*curHigh).getName() )) {
				                pnapi::Place * cPlace = &cNet3.createPlace("", 1);
				                newArcs.insert(std::make_pair(*preTransition, cPlace));
					            newArcs.insert(std::make_pair(cPlace, *preTransition));
					            newArcs.insert(std::make_pair(cPlace, highCopy));
					        }
				        }
				        cNet3.createArc(*highCopy, *highFired);
				        cNet3.createArc(*highFired, *lowCopy);
				        cNet3.createArc(*lowCopy, *goal);
				        cNet3.createArc(*highActivated, *curHigh);
				        cNet3.createArc(*curHigh, *highActivated);
				        cNet3.createArc(*highActivated, *highCopy);
				        
				        // add collected arcs
                        FOREACH(a, newArcs) {
		                    cNet3.createArc(*a->first, *a->second);
	                    }
				        
				        // ToDo: nur falls != statistics
				        curFileName = fileName + ".conflict." + (**p).getName() + "." + (*curHigh).getName() + "." + (*curLow).getName() + ".lola";

				        o.open(curFileName.c_str(), std::ios_base::trunc);
				        o << pnapi::io::lola << cNet3;
				        o << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";
				        o.close();
				        status("........%s created", _cfilename_(curFileName));
				        
		            }
                }
            }
        }
    
    
    }

    if (args_info.task_arg == task_arg_controllability) {
        // falls representative Namen
        const std::string PRESTART = "preStart";
        const std::string START = "tStart";
        const std::string MAKE_HIGH = "make_high_";
        const std::string ACTIVATE_MAKE = "activate_make_";
        const std::string CONFIGURE = "configure_";
        const std::string HIGH = "_high";
        const std::string LOW = "_low";
        const std::string GOAL = "goal_";

        unsigned int causalStructures = 0;
        unsigned int conflictStructures = 0;

        std::set< std::pair<pnapi::Node *, pnapi::Node *> > newArcs;
    
        // create new PetriNet
        pnapi::PetriNet cNet(net);
        // prepare final condition
        // cNet.getFinalCondition() = false;
        // create controller structure
	    pnapi::Transition * startTransition = &cNet.createTransition(START);
	    pnapi::Label & label = cNet.getInterface().addSynchronousLabel("start");
        startTransition->addLabel(label);
	    pnapi::Place * preStart = &cNet.createPlace(PRESTART, 1);
        newArcs.insert(std::make_pair(preStart, startTransition));
        // update initial marking
        PNAPI_FOREACH(p, net.getPlaces()) {
		    if ((**p).getTokenCount() > 0) {
		        assert((**p).getTokenCount() == 1);
		        newArcs.insert(std::make_pair(startTransition, cNet.findNode((**p).getName())));
		        cNet.findPlace((**p).getName())->setTokenCount(0);
		    }
		}
		// controller structure for all transitions
        PNAPI_FOREACH(t, net.getTransitions()) {
            status("handle transition %s", (**t).getName().c_str());
            unsigned int curConfidence = (**t).getConfidence();
		    if (curConfidence == 0) {
			    // unlabeled transition
			    pnapi::Place * a = &cNet.createPlace(std::string(ACTIVATE_MAKE + (**t).getName()), 1);
			    pnapi::Place * c = &cNet.createPlace(std::string(CONFIGURE + (**t).getName()), 1);
		        pnapi::Place * l = &cNet.createPlace(std::string((**t).getName() + LOW), 1);
		        pnapi::Place * h = &cNet.createPlace(std::string((**t).getName() + HIGH), 0);
		        pnapi::Transition * mt = &cNet.createTransition(std::string(MAKE_HIGH + (**t).getName()));
		        pnapi::Label & label = cNet.getInterface().addSynchronousLabel(std::string("make_" + (**t).getName() + "_high"));
                mt->addLabel(label);
                newArcs.insert(std::make_pair(a, mt));
                newArcs.insert(std::make_pair(c, mt));
                newArcs.insert(std::make_pair(mt, c));
                newArcs.insert(std::make_pair(c, startTransition));
                newArcs.insert(std::make_pair(l, mt));
                newArcs.insert(std::make_pair(mt, h));
		    }
		    else if (curConfidence == 1) {
		        // low labeled
		        cNet.createPlace(std::string((**t).getName() + HIGH), 0);
		        cNet.createPlace(std::string((**t).getName() + LOW), 1);
		    }
		    else if (curConfidence == 2) {
		        // high labeled
		        cNet.createPlace(std::string((**t).getName() + HIGH), 1);
		        cNet.createPlace(std::string((**t).getName() + LOW), 0);
		    }
        }
        
        // handle possible causal places
        PNAPI_FOREACH(p, net.getPlaces()) {
            status("handle place %s", (**p).getName().c_str());
            PNAPI_FOREACH(tv, (**p).getPreset()) {
                PNAPI_FOREACH(tn, (**p).getPostset()) {
                    status("..handle causal triple (%s, %s, %s)", (**p).getName().c_str(), (**tv).getName().c_str(), (**tn).getName().c_str());
                    // add currect causal case structure (p, tv, tn)
                    causalStructures++;
			        pnapi::Place * curPlace = cNet.findPlace((**p).getName());
				    pnapi::Transition * curHigh = cNet.findTransition((**tv).getName());
				    pnapi::Transition * curLow = cNet.findTransition((**tn).getName());
				    pnapi::Transition * lowCopy = &cNet.createTransition();
				    pnapi::Transition * highCopy = &cNet.createTransition();
				    pnapi::Place * highFired = &cNet.createPlace();
				    pnapi::Place * goal = &cNet.createPlace(std::string(GOAL + (**p).getName() + "_" + (**tv).getName() + "_" + (**tn).getName()));
				    // set goal place in final condition
				    cNet.getFinalCondition().addProposition(*goal == 0, true);
                    PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
                        newArcs.insert(std::make_pair(*prePlace, highCopy));
                    }
                    PNAPI_FOREACH(postPlace, curHigh->getPostset()) {
                        newArcs.insert(std::make_pair(highCopy, *postPlace));
                    }
                    PNAPI_FOREACH(prePlace, curLow->getPreset()) {
                        newArcs.insert(std::make_pair(*prePlace, lowCopy));
                    }
				    PNAPI_FOREACH(preTransition, curPlace->getPreset()) {
				        if (((**preTransition).getName() != (*curLow).getName()) && ( (**preTransition).getName() != (*curHigh).getName() )) {
				            pnapi::Place * cPlace = &cNet.createPlace("", 1);
				            newArcs.insert(std::make_pair(*preTransition, cPlace));
					        newArcs.insert(std::make_pair(cPlace, *preTransition));
					        newArcs.insert(std::make_pair(cPlace, highCopy));
					    }
				    }
				    newArcs.insert(std::make_pair(highCopy, highFired));
				    newArcs.insert(std::make_pair(highFired, lowCopy));
				    newArcs.insert(std::make_pair(lowCopy, goal));
				    // add connection to controller
				    pnapi::Place * l = cNet.findPlace(std::string(curLow->getName() + LOW));
		            pnapi::Place * h = cNet.findPlace(std::string(curHigh->getName() + HIGH));
		            newArcs.insert(std::make_pair(h, highCopy));
		            newArcs.insert(std::make_pair(highCopy, h));
		            newArcs.insert(std::make_pair(l, lowCopy));
		            newArcs.insert(std::make_pair(lowCopy, l));
			    }
		    }
        }
        
        // handle possible conflict places
        std::set< std::pair<pnapi::Node *, pnapi::Node *> > done;
        PNAPI_FOREACH(p, net.getPlaces()) {
            status("handle place %s", (**p).getName().c_str());
            PNAPI_FOREACH(t1, (**p).getPostset()) {
                PNAPI_FOREACH(t2, (**p).getPostset()) {
                    if ((t1 != t2) && (done.find(std::make_pair(*t1, *t2)) == done.end())) {
                        done.insert(std::make_pair(*t1, *t2));
                        status("..handle conflict triple (%s, %s, %s)", (**p).getName().c_str(), (**t1).getName().c_str(), (**t2).getName().c_str());
                        conflictStructures++;
                        // add currect conflict case structure (p, t1, t2)
                        pnapi::Place * curPlace = cNet.findPlace((**p).getName());
				        pnapi::Transition * curHigh = cNet.findTransition((**t1).getName());
				        pnapi::Transition * curLow = cNet.findTransition((**t2).getName());
				        pnapi::Transition * lowCopy = &cNet.createTransition();
				        pnapi::Transition * highCopy = &cNet.createTransition();
				        pnapi::Place * highFired = &cNet.createPlace();
				        pnapi::Place * highActivated = &cNet.createPlace("", 1);
				        pnapi::Place * goal = &cNet.createPlace(std::string(GOAL + (**p).getName() + "_" + (**t1).getName() + "_" + (**t2).getName()));
				        // set goal place in final condition
				        cNet.getFinalCondition().addProposition(*goal == 0, true);
                        PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
                            newArcs.insert(std::make_pair(*prePlace, highCopy));
                            newArcs.insert(std::make_pair(highCopy, *prePlace));
                        }
                        PNAPI_FOREACH(prePlace, curLow->getPreset()) {
                            newArcs.insert(std::make_pair(*prePlace, lowCopy));
                        }
				        PNAPI_FOREACH(preTransition, curPlace->getPreset()) {
				            if (((**preTransition).getName() != (*curLow).getName()) && ( (**preTransition).getName() != (*curHigh).getName() )) {
				                pnapi::Place * cPlace = &cNet.createPlace("", 1);
				                newArcs.insert(std::make_pair(*preTransition, cPlace));
					            newArcs.insert(std::make_pair(cPlace, *preTransition));
					            newArcs.insert(std::make_pair(cPlace, highCopy));
					        }
				        }
				        cNet.createArc(*highCopy, *highFired);
				        cNet.createArc(*highFired, *lowCopy);
				        cNet.createArc(*lowCopy, *goal);
				        cNet.createArc(*highActivated, *curHigh);
				        cNet.createArc(*curHigh, *highActivated);
				        cNet.createArc(*highActivated, *highCopy);
				        // add connection to controller
				        pnapi::Place * l = cNet.findPlace(std::string(curLow->getName() + LOW));
		                pnapi::Place * h = cNet.findPlace(std::string(curHigh->getName() + HIGH));
		                newArcs.insert(std::make_pair(h, highCopy));
		                newArcs.insert(std::make_pair(highCopy, h));
		                newArcs.insert(std::make_pair(l, lowCopy));
		                newArcs.insert(std::make_pair(lowCopy, l));
		                
		                done.insert(std::make_pair(*t2, *t1));
		                status("..handle conflict triple (%s, %s, %s)", (**p).getName().c_str(), (**t2).getName().c_str(), (**t1).getName().c_str());
				        conflictStructures++;
				        // add currect conflict case structure (p, t2, t1)
                        curPlace = cNet.findPlace((**p).getName());
				        curHigh = cNet.findTransition((**t2).getName());
				        curLow = cNet.findTransition((**t1).getName());
				        lowCopy = &cNet.createTransition();
				        highCopy = &cNet.createTransition();
				        highFired = &cNet.createPlace();
				        highActivated = &cNet.createPlace("", 1);
				        goal = &cNet.createPlace(std::string(GOAL + (**p).getName() + "_" + (**t2).getName() + "_" + (**t1).getName()));
				        // set goal place in final condition
				        cNet.getFinalCondition().addProposition(*goal == 0, true);
                        PNAPI_FOREACH(prePlace, curHigh->getPreset()) {
                            newArcs.insert(std::make_pair(*prePlace, highCopy));
                            newArcs.insert(std::make_pair(highCopy, *prePlace));
                        }
                        PNAPI_FOREACH(prePlace, curLow->getPreset()) {
                            newArcs.insert(std::make_pair(*prePlace, lowCopy));
                        }
				        PNAPI_FOREACH(preTransition, curPlace->getPreset()) {
				            if (((**preTransition).getName() != (*curLow).getName()) && ( (**preTransition).getName() != (*curHigh).getName() )) {
				                pnapi::Place * cPlace = &cNet.createPlace("", 1);
				                newArcs.insert(std::make_pair(*preTransition, cPlace));
					            newArcs.insert(std::make_pair(cPlace, *preTransition));
					            newArcs.insert(std::make_pair(cPlace, highCopy));
					        }
				        }
				        cNet.createArc(*highCopy, *highFired);
				        cNet.createArc(*highFired, *lowCopy);
				        cNet.createArc(*lowCopy, *goal);
				        cNet.createArc(*highActivated, *curHigh);
				        cNet.createArc(*curHigh, *highActivated);
				        cNet.createArc(*highActivated, *highCopy);
				        // add connection to controller
				        l = cNet.findPlace(std::string(curLow->getName() + LOW));
		                h = cNet.findPlace(std::string(curHigh->getName() + HIGH));
		                newArcs.insert(std::make_pair(h, highCopy));
		                newArcs.insert(std::make_pair(highCopy, h));
		                newArcs.insert(std::make_pair(l, lowCopy));
		                newArcs.insert(std::make_pair(lowCopy, l));
		            }
                }
            }
        }
        
        // add preStart to final condition
        cNet.getFinalCondition().addProposition(*preStart == 0, true);
        
        // add collected arcs
        FOREACH(a, newArcs) {
		    cNet.createArc(*a->first, *a->second);
	    }
	    
	    // ToDo: case Anweisung verwenden
        if (args_info.modus_arg == modus_arg_makefile) {
	        std::string curFileName;
	        curFileName = fileName + ".controler" + ".owfn";

	        std::ofstream o;
	        o.open(curFileName.c_str(), std::ios_base::trunc);
	        o << pnapi::io::owfn << cNet;
	        o.close();
	
            status("........%s created", _cfilename_(curFileName));

	        allNets.insert(curFileName);
	    }

	    if (args_info.modus_arg == modus_arg_call) {
		    // call Wendy
            if (callWendy(cNet)) {
	            status(_cgood_("........controllable"));
            }
            else {
	            status(_cbad_("........uncontrollable"));
            }
	    }
    
        message("transitions %d, causal: %d, conflict: %d", net.getTransitions().size(), causalStructures, conflictStructures);
    
    }

    
    if (args_info.task_arg == task_arg_verfification) {
        if (args_info.modus_arg == modus_arg_call) {

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
                std::multimap<pnapi::Place *, std::pair<pnapi::Transition *, pnapi::Transition *> >::iterator itTriple;
          	    std::pair<std::multimap<pnapi::Place *, std::pair<pnapi::Transition *, pnapi::Transition *> >::iterator,std::multimap<pnapi::Place *, std::pair<pnapi::Transition *, pnapi::Transition *> >::iterator> retTriple;
                
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
                results.add("net.down", downTransitions);
                results.add("net.low", lowTransitions);
                results.add("net.arcs", (unsigned int)(net.getArcs().size()));

                FOREACH(p, potentialCausal) {
                    std::string placeName((**p).getName());
                    results.add(std::string(placeName + ".causal_candidates").c_str(), (unsigned int)(highPre.count(*p) * lowPost.count(*p)));
                    results.add(std::string(placeName + ".causal_isActive").c_str(), activeCausal.find(*p) != activeCausal.end());
                    results.add(std::string(placeName + ".causal_checkedTriples").c_str(), (unsigned int)(potentialCausalTriple.count(*p)));
                    results.add(std::string(placeName + ".causal_activeTriples").c_str(), (unsigned int)(activeCausalTriple.count(*p)));
                    
                    retTriple = potentialCausalTriple.equal_range(*p);
                    for (itTriple=retTriple.first; itTriple!=retTriple.second; ++itTriple) {
                        results.add(std::string(placeName + ".causal_triple(" + placeName + ", " + (*itTriple).second.first->getName() + ", " + (*itTriple).second.second->getName() + ")").c_str(), false);
                    }
                    retTriple = activeCausalTriple.equal_range(*p);
                    for (itTriple=retTriple.first; itTriple!=retTriple.second; ++itTriple) {
                        results.add(std::string(placeName + ".causal_triple(" + placeName + ", " + (*itTriple).second.first->getName() + ", " + (*itTriple).second.second->getName() + ")").c_str(), true);
                    }
                }
                FOREACH(p, potentialConflict) {
                    std::string placeName((**p).getName());
                    results.add(std::string(placeName + ".conflict_candidates").c_str(), (unsigned int)(highPost.count(*p) * lowPost.count(*p)));
                    results.add(std::string(placeName + ".conflict_isActive").c_str(), activeConflict.find(*p) != activeConflict.end());
                    results.add(std::string(placeName + ".conflict_checkedTriples").c_str(), (unsigned int)(potentialConflictTriple.count(*p)));
                    results.add(std::string(placeName + ".conflict_activeTriples").c_str(), (unsigned int)(activeConflictTriple.count(*p)));
                    
                    retTriple = potentialConflictTriple.equal_range(*p);
                    for (itTriple=retTriple.first; itTriple!=retTriple.second; ++itTriple) {
                        results.add(std::string(placeName + ".conflict_triple(" + placeName + ", " + (*itTriple).second.first->getName() + ", " + (*itTriple).second.second->getName() + ")").c_str(), false);
                    }
                    retTriple = activeConflictTriple.equal_range(*p);
                    for (itTriple=retTriple.first; itTriple!=retTriple.second; ++itTriple) {
                        results.add(std::string(placeName + ".conflict_triple(" + placeName + ", " + (*itTriple).second.first->getName() + ", " + (*itTriple).second.second->getName() + ")").c_str(), true);
                    }
                }
            }

            if (result == 0) {message(_cbad_("Non-Interference: FAILED"));}
            if (result == 1) {message(_cwarning_("Non-Interference: POTENTIAL"));}
            if (result == 2) {message(_cgood_("Non-Interference: PASSED"));}

        }
    }
        

    return EXIT_SUCCESS;
}
