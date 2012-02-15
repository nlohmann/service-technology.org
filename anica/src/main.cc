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
#include <cstdio>
#include <cstring>

#include <pnapi/pnapi.h>
#include <cuddObj.hh>

#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "anica-util.h"
#include "Results.h"

using namespace pnapi;
using namespace std;

typedef struct { 
  Place * goal;
  Transition * hC;
  Transition * lC;
} extended_result_t;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
string invocation;

/// a variable holding the time of the call
const clock_t start_clock = clock();

/// constants for security labeling
const int CONFIDENCE_NONE = 0;
const int CONFIDENCE_LOW = 1;
const int CONFIDENCE_HIGH = 2;
const int CONFIDENCE_DOWN = 3;

/// check if a file exists and can be opened for reading
inline bool fileExists(const string& filename) {
    ifstream tmp(filename.c_str(), ios_base::in);
    return tmp.good();
}

/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += (i == 0 ? "" : " ") + string(argv[i]);
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
            debug_output.stream() << CONFIG_LOG << flush;
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
        const string conf_generic_filename = string(PACKAGE) + ".conf";
        const string conf_filename = fileExists(conf_generic_filename) ? conf_generic_filename :
                                    (fileExists(string(SYSCONFDIR) + "/" + conf_generic_filename) ?
                                     (string(SYSCONFDIR) + "/" + conf_generic_filename) : "");

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
    // ToDo : zweite LoLA eruieren!
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
		args_info.oneActiveOnly_flag = false;
	}
	if ((args_info.task_arg != task_arg_verfification) && args_info.oneTripleOnly_flag) {
		status(_cwarning_("oneTripleOnly has no influence on this task: parameter will be ignored."));
		args_info.oneTripleOnly_flag = false;
	}
	if (args_info.dotActive_given && args_info.modus_arg == modus_arg_makefile) {
		status(_cwarning_("activePlaces requieres modus=call: no dot-file will be created."));
	}

    if (args_info.finalize_flag) {free(params);}
}

void clearColors(PetriNet & Petrinet) {
    FOREACH(node, Petrinet.getNodes()) {
        (**node).setColor("");
    }
}

void createDotFile(const string & OutputFile, const PetriNet & Petrinet, const string & InputFile) {
	ofstream outStream;

	outStream.open(string(OutputFile + ".dot").c_str(), ios_base::trunc);
	if (outStream.fail()) {
		abort(3, "file %s cannot be created", _cfilename_(OutputFile + ".dot"));
	}

	outStream << io::dot
				<< meta(io::INPUTFILE, InputFile)
                << meta(io::CREATOR, PACKAGE_STRING)
                << meta(io::INVOCATION, invocation)
				<< Petrinet;
	outStream.close();

	status("file %s created", _cfilename_(OutputFile + ".dot"));
}


bool callLoLA(const PetriNet& net, const Place * goal) {

	time_t start_time, end_time;
	
	stringstream ss;
	ss << io::lola << net;
	ss << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";

	// select LoLA binary and build LoLA command 
    #if defined(__MINGW32__) 
    // // MinGW does not understand pathnames with "/", so we use the basename 
    const string command_line = "\"" + string(args_info.lola_arg) + (args_info.witnessPath_given ? " -p " : "") + (args_info.verbose_flag ? "" : " 2> nul"); 
    #else 
    const string command_line = string(args_info.lola_arg) + (args_info.witnessPath_given ? " -p " : "") + (args_info.verbose_flag ? "" : " 2> nul"); 
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

	switch (ret)  {
	    case 0:
		    return true;
	    case 1:
		    return false;
	    default:
		    abort(9, "LoLA parse error");
	}
}

bool callWendy(const PetriNet& net) {

	time_t start_time, end_time;
	
	stringstream ss;
	ss << io::owfn << net;

	// select LoLA binary and build LoLA command 
    #if defined(__MINGW32__) 
    // // MinGW does not understand pathnames with "/", so we use the basename 
    const string command_line = "\"" + string(args_info.wendy_arg)  + (args_info.verbose_flag ? " -v " : "") + " --lola=" + string(args_info.lolaWendy_arg) + " --resultFile=-" + " | grep -q \"result = true\"";  
    #else 
    const string command_line = string(args_info.wendy_arg) + (args_info.verbose_flag ? " -v " : "") + " --lola=" + string(args_info.lolaWendy_arg) + " --resultFile=-" + " | grep -q \"result = true\"";  
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

	switch (ret)  {
	    case 0:
		    return true;
	    case 1:
		    return false;
	    default:
		    abort(9, "Wendy parse error");
	}
}

// ToDo: gemeinsame Elemente zusammenfassen?!
extended_result_t addCausalPattern(PetriNet& extendedNet, const Place& s, const Transition& h, const Transition& l, const set<string>& downgradeTransitions, set< pair<Node*, Node*> >& newArcs, const bool insertArcs) {
    extended_result_t ret;
    
    // sprechende Namen eruieren
    Transition* lC = &extendedNet.createTransition();
    Transition* hC = &extendedNet.createTransition();
    Place* fired = &extendedNet.createPlace();
    Place* goal = &extendedNet.createPlace();

    const string triple = "(" + s.getName() + ", " + h.getName() + ", " + l.getName() + ")";

    if (args_info.useRepresantiveNames_flag) {
        lC->setName("lC_" + triple);
        hC->setName("hC_" + triple);
        fired->setName("fired_" + triple);
        goal->setName("goal_" + triple);
    }

    ret.goal = goal;
    ret.hC = hC;
    ret.lC = lC;

    FOREACH(prePlace, h.getPreset()) {
        newArcs.insert(make_pair(*prePlace, hC));
    }
    FOREACH(postPlace, h.getPostset()) {
        newArcs.insert(make_pair(hC, *postPlace));
    }

    FOREACH(prePlace, l.getPreset()) {
        newArcs.insert(make_pair(*prePlace, lC));
    }

    FOREACH(preTransition, s.getPreset()) {
        if (((**preTransition).getName() != l.getName()) && ( (**preTransition).getName() != h.getName() )) {
            Place* a = &extendedNet.createPlace("", 1);
            if (args_info.useRepresantiveNames_flag) {
                a->setName("p_" + (**preTransition).getName() + "_" + triple);
            }
            newArcs.insert(make_pair(*preTransition, a));
            newArcs.insert(make_pair(a, *preTransition));
            newArcs.insert(make_pair(a, hC));
        }
    }

    if (args_info.property_arg == property_arg_PBNID) {
        //handle downgrading transitions
        FOREACH(downTransition, downgradeTransitions) {
            Place* a = &extendedNet.createPlace("", 1);
            Transition* d = (Transition *)extendedNet.findNode(*downTransition);
            if (args_info.useRepresantiveNames_flag) {
                a->setName("d_" + d->getName() + "_" + triple);
            }
            newArcs.insert(make_pair(d, a));
            newArcs.insert(make_pair(a, d));
            newArcs.insert(make_pair(a, hC));
        }
    }

    newArcs.insert(make_pair(hC, fired));
    newArcs.insert(make_pair(fired, lC));
    newArcs.insert(make_pair(lC, goal));

    if (insertArcs) {
        FOREACH(a, newArcs) {
            extendedNet.createArc(*a->first, *a->second);
        }
        newArcs.clear();
    }

    return ret;
}

extended_result_t addConflictPattern(PetriNet& extendedNet, const Place& s, Transition& h, const Transition& l, const set<string>& downgradeTransitions, set< pair<Node*, Node*> >& newArcs, const bool insertArcs) {
    extended_result_t ret;
    
    Transition* lC = &extendedNet.createTransition();
    Transition* hC = &extendedNet.createTransition();
    Place* fired = &extendedNet.createPlace();
    Place* enabled = &extendedNet.createPlace("", 1);
    Place* goal = &extendedNet.createPlace();

    const string triple = "(" + s.getName() + ", " + h.getName() + ", " + l.getName() + ")";

    if (args_info.useRepresantiveNames_flag) {
        lC->setName("lC_" + triple);
        hC->setName("hC_" + triple);
        fired->setName("fired_" + triple);
        enabled->setName("enabled_" +triple);
        goal->setName("goal_" + triple);
    }

    ret.goal = goal;
    ret.hC = hC;
    ret.lC = lC;

    FOREACH(prePlace, h.getPreset()) {
        newArcs.insert(make_pair(*prePlace, hC));
        newArcs.insert(make_pair(hC, *prePlace));
    }

    FOREACH(prePlace, l.getPreset()) {
        newArcs.insert(make_pair(*prePlace, lC));
    }

    FOREACH(preTransition, s.getPreset()) {
        if (((**preTransition).getName() != l.getName()) && ( (**preTransition).getName() != h.getName() )) {
            Place* a = &extendedNet.createPlace("", 1);
            if (args_info.useRepresantiveNames_flag) {
                a->setName("p_" + (**preTransition).getName() + "_" + triple);
            }
            newArcs.insert(make_pair(*preTransition, a));
            newArcs.insert(make_pair(a, *preTransition));
            newArcs.insert(make_pair(a, hC));
        }
    }

    if (args_info.property_arg == property_arg_PBNID) {
        //handle downgrading transitions
        FOREACH(downTransition, downgradeTransitions) {
            Place* a = &extendedNet.createPlace("", 1);
            Transition* d = (Transition *)extendedNet.findNode(*downTransition);
            if (args_info.useRepresantiveNames_flag) {
                a->setName("d_" + d->getName() + "_" + triple);
            }
            newArcs.insert(make_pair(d, a));
            newArcs.insert(make_pair(a, d));
            newArcs.insert(make_pair(a, hC));
        }
    }
       
    newArcs.insert(make_pair(hC, fired));
    newArcs.insert(make_pair(fired, lC));
    newArcs.insert(make_pair(lC, goal)); 
    newArcs.insert(make_pair(enabled, &h));
    newArcs.insert(make_pair(&h, enabled));
    newArcs.insert(make_pair(enabled, hC));
    
    if (insertArcs) {
        FOREACH(a, newArcs) {
            extendedNet.createArc(*a->first, *a->second);
        }
        newArcs.clear();
    }
    
    return ret;
}

void createLolaTask(const PetriNet& extendedNet, const string& name, set<string>& allNets) {
    ofstream o;
    o.open(name.c_str(), ios_base::trunc);
    o << io::lola << extendedNet;
    o.close();
    //status("........%s created", _cfilename_(curFileName));
    allNets.insert(name);
}

void createWendyTask(const PetriNet& extendedNet, const string& name, set<string>& allNets) {
    ofstream o;
    o.open(name.c_str(), ios_base::trunc);
    o << io::owfn << extendedNet;
    o.close();
    //status("........%s created", _cfilename_(curFileName));
    allNets.insert(name);
}

/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
		int ret = system((string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }

	if (args_info.finalize_flag) {cmdline_parser_free(&args_info);}
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
    PetriNet net;

    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            // parse the open net from standard input
            cin >> io::lola >> net;
        } else {
            // strip suffix from input filename (if necessary: uncomment next line for future use)
            //inputFilename = string(args_info.inputs[0]).substr(0, string(args_info.inputs[0]).find_last_of("."));

            // open input file as an input file stream
            ifstream inputStream(args_info.inputs[0]);
            // ... and abort, if an error occurs
            if (!inputStream) {
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }

            // parse the open net from the input file stream
            inputStream >> meta(io::INPUTFILE, args_info.inputs[0])
                        >> io::lola >> net;
        }
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            ostringstream s;
            s << io::stat << net;
            status("read net: %s", s.str().c_str());
        }
    } catch (const pnapi::exception::InputError& error) {
        ostringstream s;
        s << error;
        abort(2, "\b%s", s.str().c_str());
    }

    /*---------------------------------.
    | 2. set parameters      		   |
    `---------------------------------*/

	string fileName;

	if (args_info.inputs_num == 0) {
		fileName = "STDIN";
	}
	else {
		fileName = string(args_info.inputs[0]);
	}    

	/*---------------------------------.
    | 3. handle confidence			   |
    `---------------------------------*/

    unsigned int downTransitions = 0;
	unsigned int highTransitions = 0;
	unsigned int lowTransitions = 0;

    // names of all downgrading transitions (names because we need to check in different Petri nets)
    set<string> downgradeTransitions;

	FOREACH(t, net.getTransitions()) {
		switch ((**t).getConfidence()) {
		    case CONFIDENCE_NONE:
			    // unlabeled transition
			    switch (args_info.unlabeledTransitions_arg) {
			        case unlabeledTransitions_arg_none:
				        if (args_info.task_arg == task_arg_verfification) {
				            abort(6, "%s has unknown confidence level", (**t).getName().c_str());
				        }
				        break;
			        case unlabeledTransitions_arg_low:
			            (**t).setConfidence(CONFIDENCE_LOW);
			            lowTransitions++;
			            break;
			        case unlabeledTransitions_arg_high:
			            (**t).setConfidence(CONFIDENCE_HIGH);
			            highTransitions++;
			            break;
			        case unlabeledTransitions_arg_down:
			            (**t).setConfidence(CONFIDENCE_DOWN);
			            downTransitions++;
			            downgradeTransitions.insert((**t).getName());
			            break;
			    }
			    break;
		    case CONFIDENCE_LOW:
		        lowTransitions++;
		        break;
		    case CONFIDENCE_HIGH: 
		        highTransitions++;
		        break;
		    case CONFIDENCE_DOWN:
		        downTransitions++;
		        downgradeTransitions.insert((**t).getName());
		        break;
		    default:
		        abort(6, "%s has unknown confidence level", (**t).getName().c_str());
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
		FOREACH(t, net.getTransitions()) {
		    switch ((**t).getConfidence()) {
			    case CONFIDENCE_LOW:
			        if ((args_info.dotConfidence_arg == dotConfidence_arg_low) || (args_info.dotConfidence_arg = dotConfidence_arg_all)) {
				        (**t).setColor("green");
				    }
				    break;
			    case CONFIDENCE_HIGH:
			        if ((args_info.dotConfidence_arg == dotConfidence_arg_high) || (args_info.dotConfidence_arg = dotConfidence_arg_all)) {
				        (**t).setColor("red");
				    }
				    break;
			    case CONFIDENCE_DOWN:
			        if ((args_info.dotConfidence_arg == dotConfidence_arg_down) || (args_info.dotConfidence_arg = dotConfidence_arg_all)) {
				        (**t).setColor("yellow");
				    }
				    break;
		    }
		}
		createDotFile(string(fileName + ".confidence"), net, fileName);
	}

	/*---------------------------------.
    | 4. gather potential places	   |
    `---------------------------------*/
	 
	const bool checkPotentialCausal = (args_info.task_arg != task_arg_verfification) ? true : (args_info.potentialPlaces_arg != potentialPlaces_arg_conflict);
    const bool checkPotentialConflict = (args_info.task_arg != task_arg_verfification) ? true : (args_info.potentialPlaces_arg != potentialPlaces_arg_causal);
    
    set<Place *> potentialCausal;
    set<Place *> potentialConflict;
        
    multimap<Place *, Transition *> highPre;
    multimap<Place *, Transition *> lowPost;
    multimap<Place *, Transition *> highPost;

    status("check potential places");
    FOREACH(p, net.getPlaces()) {
	    status("..%s", (**p).getName().c_str());
	
	    set<Transition *> lHighPre;
	    set<Transition *> lHighPost;
	    set<Transition *> lLowPost;
	
	    if (checkPotentialCausal) {
		    FOREACH(t, (**p).getPreset()) {
		        const unsigned int confidence = ((Transition *)(*t))->getConfidence();
			    if (confidence == CONFIDENCE_HIGH) {
				    lHighPre.insert((Transition *)*t);
			    }
			    if ((args_info.task_arg != task_arg_verfification) && (confidence == CONFIDENCE_NONE)) {
			        lHighPre.insert((Transition *)*t);
			    }
		    }
	    }

	    FOREACH(t, (**p).getPostset()) {
	        switch (((Transition *)(*t))->getConfidence()) {
		        case CONFIDENCE_HIGH:
			        lHighPost.insert((Transition *)*t);
			        break;
		        case CONFIDENCE_LOW:
			        lLowPost.insert((Transition *)*t);
			        break;
		        case CONFIDENCE_NONE:
		            if (args_info.task_arg != task_arg_verfification) {
		                // handle (later!) the case of same transition
		                lHighPost.insert((Transition *)*t);
		                lLowPost.insert((Transition *)*t);
		            }
		            break;
		    }
	    }
        
	    bool lowPostDone = false;

	    if (checkPotentialCausal) {
		    if (!lHighPre.empty() && !lLowPost.empty()) {
			    status(_cwarning_("....potential causal"));
			    potentialCausal.insert(*p);
			    FOREACH(t, lHighPre) {
				    highPre.insert(make_pair(*p, *t));
			    }
			    FOREACH(t, lLowPost) {
				    lowPost.insert(make_pair(*p, *t));
			    }
			    lowPostDone = true;
		    }
	    }

	    if (checkPotentialConflict) {
		    if (!lHighPost.empty() && !lLowPost.empty()) {
			    status(_cwarning_("....potential conflict"));
			    potentialConflict.insert(*p);
			    FOREACH(t, lHighPost) {
				    highPost.insert(make_pair(*p, *t));
			    }
			    if (!lowPostDone) {
				    FOREACH(t, lLowPost) {
					    lowPost.insert(make_pair(*p, *t));
				    }
			    }
		    }
	    }

    }

    if (args_info.dotPotential_given) {
        if (args_info.dotConfidence_given) {clearColors(net);}
	    FOREACH(p, net.getPlaces()) {
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
	    createDotFile(string(fileName + ".potential"), net, fileName);
    }

	/*---------------------------------.
    | 5. check potential places		   |
    `---------------------------------*/

    const bool checkActiveCausal = (args_info.task_arg != task_arg_verfification) ? true : ((args_info.activePlaces_arg == activePlaces_arg_causal) || (args_info.activePlaces_arg == activePlaces_arg_both));
    const bool checkActiveConflict = (args_info.task_arg != task_arg_verfification) ? true : ((args_info.activePlaces_arg == activePlaces_arg_conflict) || (args_info.activePlaces_arg == activePlaces_arg_both));
    
    set<Place *> activeCausal;
    set<Place *> activeConflict;

    multimap<Place *, pair<Transition *, Transition *> > potentialCausalTriple;
    multimap<Place *, pair<Transition *, Transition *> > potentialConflictTriple;
    multimap<Place *, pair<Transition *, Transition *> > activeCausalTriple;
    multimap<Place *, pair<Transition *, Transition *> > activeConflictTriple;

    set<string> allNets;

    if ((checkActiveCausal || checkActiveConflict) && (!potentialCausal.empty() || !potentialConflict.empty())) {
        // check active places
	    status("check active places");
	    
        const string HIGH = "_high";
        const string LOW = "_low";
	    
	    PetriNet* extendedNet;
	    Place* s;
		Transition* h;
		Transition* l;
		extended_result_t extResult;
		
		Cudd* cuddManager;
		map<string, BDD> cuddVariables;
		char** cuddVariableNames;
		BDD cuddOutput;
		
	    set< pair<Node*, Node*> > newArcs;
	    
	    unsigned int i;
	    
	    // initial stuff before considering the specific net
	    switch (args_info.task_arg) {
	        case task_arg_verfification:
	            break;
	        case task_arg_characterization:
	            cuddManager = new Cudd();
	            cuddManager->AutodynEnable(CUDD_REORDER_GROUP_SIFT_CONV);
	            cuddOutput = cuddManager->bddOne();
	            cuddVariableNames = new char*[net.getTransitions().size()];
	            i = 0;
	            FOREACH(t, net.getTransitions()) {
	                cuddVariables[(**t).getName()] = cuddManager->bddVar();
	                cuddVariableNames[i++] = strdup((**t).getName().c_str());
	            }
	            break;
	        case task_arg_controllability:
	            extendedNet = new PetriNet(net);
	            Transition* startTransition = &extendedNet->createTransition("tStart");
                startTransition->addLabel(extendedNet->getInterface().addSynchronousLabel("start"));
                Place* preStart = &extendedNet->createPlace("preStart", 1);
                extendedNet->getFinalCondition().addProposition(*preStart == 0, true);
                newArcs.insert(make_pair(preStart, startTransition));
                // update initial marking
                FOREACH(p, net.getPlaces()) {
                    if ((**p).getTokenCount() > 0) {
                        assert((**p).getTokenCount() == 1);
                        newArcs.insert(make_pair(startTransition, extendedNet->findNode((**p).getName())));
                        extendedNet->findPlace((**p).getName())->setTokenCount(0);
                    }
                }
                // controller structure for all transitions
                FOREACH(t, net.getTransitions()) {
                    status("handle transition %s", (**t).getName().c_str());
                    switch ((**t).getConfidence()) {
                        case CONFIDENCE_LOW:
                            // low labeled
                            extendedNet->createPlace(string((**t).getName() + HIGH), 0);
                            extendedNet->createPlace(string((**t).getName() + LOW), 1);
                            break;
                        case CONFIDENCE_HIGH:
                            // high labeled
                            extendedNet->createPlace(string((**t).getName() + HIGH), 1);
                            extendedNet->createPlace(string((**t).getName() + LOW), 0);
                            break;
                        case CONFIDENCE_NONE:
                            // unlabeled transition
                            /*
                            Place* controllerActivate = &extendedNet->createPlace(string("activate_make_" + (**t).getName()), 1);
                            Place* controllerConfigure = &extendedNet->createPlace(string("configure_" + (**t).getName()), 1);
                            Place* controllerLow = &extendedNet->createPlace(string((**t).getName() + LOW), 1);
                            Place* controllerHigh = &extendedNet->createPlace(string((**t).getName() + HIGH), 0);
                            Transition* controllerMakeHigh = &extendedNet->createTransition(string("make_high_" + (**t).getName()));
                            controllerMakeHigh->addLabel(extendedNet->getInterface().addSynchronousLabel(string("make_" + (**t).getName() + "_high")));
                            newArcs.insert(make_pair(controllerActivate, controllerMakeHigh));
                            newArcs.insert(make_pair(controllerConfigure, controllerMakeHigh));
                            newArcs.insert(make_pair(controllerMakeHigh, controllerConfigure));
                            newArcs.insert(make_pair(controllerConfigure, startTransition));
                            newArcs.insert(make_pair(controllerLow, controllerMakeHigh));
                            newArcs.insert(make_pair(controllerMakeHigh, controllerHigh));
                            */
                            Place* controllerConfigure = &extendedNet->createPlace(string("configure_" + (**t).getName()), 1);
                            Transition* controllerMakeHigh = &extendedNet->createTransition(string("make_high_" + (**t).getName()));
                            controllerMakeHigh->addLabel(extendedNet->getInterface().addSynchronousLabel(string("make_" + (**t).getName() + "_high")));
                            newArcs.insert(make_pair(&extendedNet->createPlace(string("activate_make_" + (**t).getName()), 1), controllerMakeHigh));
                            newArcs.insert(make_pair(controllerConfigure, controllerMakeHigh));
                            newArcs.insert(make_pair(controllerMakeHigh, controllerConfigure));
                            newArcs.insert(make_pair(controllerConfigure, startTransition));
                            newArcs.insert(make_pair(&extendedNet->createPlace(string((**t).getName() + LOW), 1), controllerMakeHigh));
                            newArcs.insert(make_pair(controllerMakeHigh, &extendedNet->createPlace(string((**t).getName() + HIGH), 0)));
                            break;
                    }
                }
	            break;
	    }

	    multimap<Place *, Transition *>::iterator itHigh;
      	pair<multimap<Place *, Transition *>::iterator,multimap<Place *, Transition *>::iterator> retHigh;
	    multimap<Place *, Transition *>::iterator itLow;
      	pair<multimap<Place *, Transition *>::iterator,multimap<Place *, Transition *>::iterator> retLow;

        bool isActive;

	    if (checkActiveCausal) {
	    status("..potential causal places");
		    FOREACH(p, potentialCausal) {

                // we have a potential causal place
			    if (args_info.oneActiveOnly_flag && (!activeCausal.empty() || !activeConflict.empty())) {break;}
                isActive = false;

			    status("....%s (%d combination(s))", (**p).getName().c_str(), highPre.count(*p) * lowPost.count(*p));

			    retHigh = highPre.equal_range(*p);
			    for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {
                    if (isActive && args_info.oneTripleOnly_flag) {break;}
                
				    retLow = lowPost.equal_range(*p);
				    for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {
				        if (isActive && args_info.oneTripleOnly_flag) {break;}
				        
				        // we have a potential causal triple
					    status("......%s, %s", (*itHigh).second->getName().c_str(), (*itLow).second->getName().c_str());
					    potentialCausalTriple.insert(make_pair(*p, make_pair((*itHigh).second, (*itLow).second)));
					    
					    if (args_info.task_arg == task_arg_verfification || args_info.task_arg == task_arg_characterization) {
					        extendedNet = new PetriNet(net);
					    }
					    s = extendedNet->findPlace((**p).getName());
			            h = extendedNet->findTransition((*itHigh).second->getName());
			            l = extendedNet->findTransition((*itLow).second->getName());
					    
					    extResult = addCausalPattern(*extendedNet, *s, *h, *l, downgradeTransitions, newArcs, (args_info.task_arg == task_arg_controllability) ? false : true);
			            extendedNet->getFinalCondition().addProposition(*extResult.goal == 0, true);
					    
					    switch (args_info.task_arg) {
					        case task_arg_verfification:
					            switch (args_info.modus_arg) {
			                        case modus_arg_makefile:
			                            createLolaTask(*extendedNet, fileName + ".causal." + (*s).getName() + "." + (*h).getName() + "." + (*l).getName() + ".lola", allNets);
					                    break;

					                case modus_arg_call:
						                // call LoLA
						                if (callLoLA(*extendedNet, extResult.goal)) {
							                status(_cbad_("........active"));
							                activeCausalTriple.insert(make_pair(*p, make_pair((*itHigh).second, (*itLow).second)));
							                isActive = true;
						                }
						                else {
							                status(_cgood_("........not active"));
						                }
						                break;
					            }
					            if (args_info.finalize_flag) {delete extendedNet;}
					            break;
					            
					        case task_arg_characterization: // ToDo: mit task_arg_verfification zusammenfassen...
					            switch (args_info.modus_arg) {
			                        case modus_arg_makefile:
			                            createLolaTask(*extendedNet, fileName + ".causal." + (*s).getName() + "." + (*h).getName() + "." + (*l).getName() + ".lola", allNets);
					                    break;

					                case modus_arg_call:
						                // call LoLA
						                if (callLoLA(*extendedNet, extResult.goal)) {
							                status(_cbad_("........active"));
							                activeCausalTriple.insert(make_pair(*p, make_pair((*itHigh).second, (*itLow).second)));
                                            cuddOutput *= !(cuddVariables[(*h).getName()] * !cuddVariables[(*l).getName()]);					                
							                isActive = true;
						                }
						                else {
							                status(_cgood_("........not active"));
						                }
						                break;
					            }
					            if (args_info.finalize_flag) {delete extendedNet;}
					            break;
					            
					        case task_arg_controllability:
					            // ToDo: mit oben abgleichen
                                Place * lController = extendedNet->findPlace(string(l->getName() + LOW));
                                Place * hController = extendedNet->findPlace(string(h->getName() + HIGH));
                                newArcs.insert(make_pair(hController, extResult.hC));
                                newArcs.insert(make_pair(extResult.hC, hController));
                                newArcs.insert(make_pair(lController, extResult.lC));
                                newArcs.insert(make_pair(extResult.lC, lController));
					            break;
					            
					    }

				    } // l's

			    } // h's
			
			    // activation of place is now decided
			    switch (args_info.task_arg) {
			        case task_arg_verfification:
			            if (isActive) {
			                status(_cbad_("......active"));
			                activeCausal.insert(*p);
			            }
			            else {
			                status(_cgood_("......not active"));
			            }
			            break;
			        case task_arg_characterization:
			            if (isActive) {
			                status(_cbad_("......active"));
			                activeCausal.insert(*p);
			            }
			            else {
			                status(_cgood_("......not active"));
			            }
			            break;
			        case task_arg_controllability:
			            break;
			    }

		    } // s's

	    } // activeCausal

	    if (checkActiveConflict) {
		    status("..potential conflict places");
		    FOREACH(p, potentialConflict) {

                // we have a potential conflict place
			    if (args_info.oneActiveOnly_flag && (!activeCausal.empty() || !activeConflict.empty())) {break;}
			    isActive = false;

			    status("....%s (%d combination(s))", (**p).getName().c_str(), highPost.count(*p) * lowPost.count(*p));
	
			    retHigh = highPost.equal_range(*p);
			    for (itHigh=retHigh.first; itHigh!=retHigh.second; ++itHigh) {
			        if (args_info.oneTripleOnly_flag && isActive) {break;}
			        
				    retLow = lowPost.equal_range(*p);
				    for (itLow=retLow.first; itLow!=retLow.second; ++itLow) {
				        if ((*itHigh).second == (*itLow).second) {continue;}
				        if (args_info.oneTripleOnly_flag && isActive) {break;}
				        
				        // we have a potential conflict triple
					    status("......%s, %s", (*itHigh).second->getName().c_str(), (*itLow).second->getName().c_str());
                        potentialConflictTriple.insert(make_pair(*p, make_pair((*itHigh).second, (*itLow).second)));

                        if (args_info.task_arg == task_arg_verfification || args_info.task_arg == task_arg_characterization) {
					        extendedNet = new PetriNet(net);
					    }
					    s = extendedNet->findPlace((**p).getName());
			            h = extendedNet->findTransition((*itHigh).second->getName());
			            l = extendedNet->findTransition((*itLow).second->getName());

                        extResult = addConflictPattern(*extendedNet, *s, *h, *l, downgradeTransitions, newArcs, (args_info.task_arg == task_arg_controllability) ? false : true);
					    extendedNet->getFinalCondition().addProposition(*extResult.goal == 0, true);

                        switch (args_info.task_arg) {
					        case task_arg_verfification:
					            switch (args_info.modus_arg) {
			                        case modus_arg_makefile:
			                            createLolaTask(*extendedNet, fileName + ".conflict." + (*s).getName() + "." + (*h).getName() + "." + (*l).getName() + ".lola", allNets);
					                    break;

					                case modus_arg_call:
						                // call LoLA
						                if (callLoLA(*extendedNet, extResult.goal)) {
							                status(_cbad_("........active"));
							                activeConflictTriple.insert(make_pair(*p, make_pair((*itHigh).second, (*itLow).second)));
							                isActive = true;
						                }
						                else {
							                status(_cgood_("........not active"));
						                }
						                break;
					            }
					            if (args_info.finalize_flag) {delete extendedNet;}
					            break;
					            
					        case task_arg_characterization: // ToDo: mit task_arg_verfification zusammenfassen...
					            switch (args_info.modus_arg) {
			                        case modus_arg_makefile:
			                            createLolaTask(*extendedNet, fileName + ".conflict." + (*s).getName() + "." + (*h).getName() + "." + (*l).getName() + ".lola", allNets);
					                    break;

					                case modus_arg_call:
						                // call LoLA
						                if (callLoLA(*extendedNet, extResult.goal)) {
							                status(_cbad_("........active"));
							                activeConflictTriple.insert(make_pair(*p, make_pair((*itHigh).second, (*itLow).second)));
							                cuddOutput *= !(cuddVariables[(*h).getName()] * !cuddVariables[(*l).getName()]);
							                isActive = true;
						                }
						                else {
							                status(_cgood_("........not active"));
						                }
						                break;
					            }
					            if (args_info.finalize_flag) {delete extendedNet;}
					            break;
					            
					        case task_arg_controllability:
					            // ToDo: mit oben abgleichen
                                Place * lController = extendedNet->findPlace(string(l->getName() + LOW));
                                Place * hController = extendedNet->findPlace(string(h->getName() + HIGH));
                                newArcs.insert(make_pair(hController, extResult.hC));
                                newArcs.insert(make_pair(extResult.hC, hController));
                                newArcs.insert(make_pair(lController, extResult.lC));
                                newArcs.insert(make_pair(extResult.lC, lController));
					            break;
					            
					    }

				    } // l's

			    } // h's
			
			    // activation of place is now decided
			    switch (args_info.task_arg) {
			        case task_arg_verfification:
			            if (isActive) {
			                status(_cbad_("......active"));
			                activeConflict.insert(*p);
			            }
			            else {
			                status(_cgood_("......not active"));
			            }
			            break;
			        case task_arg_characterization:
			            if (isActive) {
			                status(_cbad_("......active"));
			                activeCausal.insert(*p);
			            }
			            else {
			                status(_cgood_("......not active"));
			            }
			            break;
			        case task_arg_controllability:
			            break;
			    }

		    } // s's

	    } // activeConflict
	    switch (args_info.task_arg) {
	        case task_arg_verfification:
	            break;
	        case task_arg_characterization:
	            //ToDo: Ausgabe in DOT-File pipen
	            cuddManager->DumpDot(vector<BDD>(1, cuddOutput), cuddVariableNames);
	            //message("nodeCount: %d", cuddManager->nodeCount(outputs));
	            if (args_info.finalize_flag) {
	                for (unsigned int i = 0; i < net.getTransitions().size(); i++) {
	                    free(cuddVariableNames[i]);
	                }
	                delete[] cuddVariableNames;
	            }
	            
	            break;
	        case task_arg_controllability:
	            // add collected arcs
                FOREACH(a, newArcs) {
		            extendedNet->createArc(*a->first, *a->second);
	            }
	            switch (args_info.modus_arg) {
                    case modus_arg_makefile:
                        createWendyTask(*extendedNet, fileName + ".controler" + ".owfn", allNets);
                        // ToDo: create makefile
	                    break;

	                case modus_arg_call:
		                // call Wendy
                        if (callWendy(*extendedNet)) {
	                        status(_cgood_("Controllable"));
                        }
                        else {
	                        status(_cbad_("Uncontrollable"));
                        }
                        break;
	            }
	            if (args_info.finalize_flag) {delete extendedNet;}
	            break;
	    }

    } // if (checkActiveCausal || checkActiveConflict) {
    
    if (args_info.modus_arg == modus_arg_makefile) {
        ofstream o;
        
        switch (args_info.task_arg) {
            case task_arg_verfification:
                // create makefile for verification
                o.open(string(fileName + ".makefile").c_str(), ios_base::trunc);

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

                message("%s %s %s", _cgood_("makefile"), _cfilename_(string(fileName + ".makefile")), _cgood_("created"));
                break;

            case task_arg_characterization:
                // ToDo: create makefile for characterization
                break;
            case task_arg_controllability:
                // ToDo: create makefile for controllability
                break;
                
        }
    }
    
    if (activeCausal.size() + activeConflict.size() > 0) {
        if (args_info.dotActive_given) {
            if (args_info.dotConfidence_given || args_info.dotPotential_given) {clearColors(net);}
	        FOREACH(p, net.getPlaces()) {
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
	        createDotFile(string(fileName + ".active"), net, fileName);
        }
    }

	/*---------------------------------.
    | 6. finishing					   |
    `---------------------------------*/
    
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
          	    pair<multimap<Place *, pair<Transition *, Transition *> >::iterator,multimap<Place *, pair<Transition *, Transition *> >::iterator> retTriple;

                Results results(args_info.resultFile_arg ? args_info.resultFile_arg : fileName + ".results");

                switch (result) {
                    case 0:
                        results.add("result.non-interference", (char*)"FAILED");
                        break;
                    case 1:
                        results.add("result.non-interference", (char*)"POTENTIAL");
                        break;
                    case 2: 
                        results.add("result.non-interference", (char*)"PASSED");
                        break;
                }

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
                    const string placeName((**p).getName());
                    results.add(string(placeName + ".causal_candidates").c_str(), (unsigned int)(highPre.count(*p) * lowPost.count(*p)));
                    results.add(string(placeName + ".causal_isActive").c_str(), activeCausal.find(*p) != activeCausal.end());
                    results.add(string(placeName + ".causal_checkedTriples").c_str(), (unsigned int)(potentialCausalTriple.count(*p)));
                    results.add(string(placeName + ".causal_activeTriples").c_str(), (unsigned int)(activeCausalTriple.count(*p)));
                    
                    retTriple = potentialCausalTriple.equal_range(*p);
                    for (multimap<Place *, pair<Transition *, Transition *> >::iterator itTriple=retTriple.first; itTriple!=retTriple.second; ++itTriple) {
                        results.add(string(placeName + ".causal_triple(" + placeName + ", " + (*itTriple).second.first->getName() + ", " + (*itTriple).second.second->getName() + ")").c_str(), false);
                    }
                    retTriple = activeCausalTriple.equal_range(*p);
                    for (multimap<Place *, pair<Transition *, Transition *> >::iterator itTriple=retTriple.first; itTriple!=retTriple.second; ++itTriple) {
                        results.add(string(placeName + ".causal_triple(" + placeName + ", " + (*itTriple).second.first->getName() + ", " + (*itTriple).second.second->getName() + ")").c_str(), true);
                    }
                }
                FOREACH(p, potentialConflict) {
                    const string placeName((**p).getName());
                    results.add(string(placeName + ".conflict_candidates").c_str(), (unsigned int)(highPost.count(*p) * lowPost.count(*p)));
                    results.add(string(placeName + ".conflict_isActive").c_str(), activeConflict.find(*p) != activeConflict.end());
                    results.add(string(placeName + ".conflict_checkedTriples").c_str(), (unsigned int)(potentialConflictTriple.count(*p)));
                    results.add(string(placeName + ".conflict_activeTriples").c_str(), (unsigned int)(activeConflictTriple.count(*p)));
                    
                    retTriple = potentialConflictTriple.equal_range(*p);
                    for (multimap<Place *, pair<Transition *, Transition *> >::iterator itTriple=retTriple.first; itTriple!=retTriple.second; ++itTriple) {
                        results.add(string(placeName + ".conflict_triple(" + placeName + ", " + (*itTriple).second.first->getName() + ", " + (*itTriple).second.second->getName() + ")").c_str(), false);
                    }
                    retTriple = activeConflictTriple.equal_range(*p);
                    for (multimap<Place *, pair<Transition *, Transition *> >::iterator itTriple=retTriple.first; itTriple!=retTriple.second; ++itTriple) {
                        results.add(string(placeName + ".conflict_triple(" + placeName + ", " + (*itTriple).second.first->getName() + ", " + (*itTriple).second.second->getName() + ")").c_str(), true);
                    }
                }
            }

            switch (result) {
                case 0:
                    message(_cbad_("Non-Interference: FAILED"));
                    break;
                case 1:
                    message(_cwarning_("Non-Interference: POTENTIAL"));
                    break;
                case 2: 
                    message(_cgood_("Non-Interference: PASSED"));
                    break;
            }

        }
    }
        

    return EXIT_SUCCESS;
}
