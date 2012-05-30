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

#include "AnicaLib.h"

using namespace pnapi;
using namespace std;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
string invocation;

/// a variable holding the time of the call
const clock_t start_clock = clock();

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
    if ((args_info.task_arg != task_arg_verification) && (args_info.property_arg != property_arg_PBNIPLUS_)) {
		abort(7, "task and property combination is not supported");
	}
	
	if ((args_info.task_arg != task_arg_verification) && (args_info.unlabeledTransitions_arg != unlabeledTransitions_arg_none)) {
		status(_cwarning_("unlabeledTransitions makes no sense with current task: BUT will be performed."));
	}
	if ((args_info.task_arg != task_arg_verification) && args_info.oneActiveOnly_flag) {
		status(_cwarning_("oneActiveOnly has no influence on this task: parameter will be ignored."));
		args_info.oneActiveOnly_flag = false;
	}
	if ((args_info.task_arg != task_arg_verification) && args_info.oneTripleOnly_flag) {
		status(_cwarning_("oneTripleOnly has no influence on this task: parameter will be ignored."));
		args_info.oneTripleOnly_flag = false;
	}

    if (args_info.finalize_flag) {free(params);}
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

/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.3f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
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
    | 3. use library    			   |
    `---------------------------------*/
    anica::AnicaLib alib(net);
    
    // configure library
    alib.setLolaPath(string(args_info.lola_arg));
    alib.setLolaWitnessPath(args_info.witnessPath_flag);
    alib.setLolaVerbose(args_info.verbose_flag);
    alib.setRepresantiveNames(args_info.useRepresantiveNames_flag);
    alib.setOneActiveOnly(args_info.oneActiveOnly_flag);
    alib.setOneTripleOnly(args_info.oneTripleOnly_flag);
    
    switch (args_info.property_arg) {
        case property_arg_PBNIPLUS_:
            alib.setProperty(anica::PROPERTY_PBNIPLUS);
            break;
        case property_arg_PBNID:
            alib.setProperty(anica::PROPERTY_PBNID);
            break;
    }
    
    size_t result = 0;
    
    // check task parameters
    if ((args_info.property_arg == property_arg_PBNIPLUS_) && (alib.getDownLabeledTransitionsCount() != 0)) {
        abort(6, "Unknown confidence level used");
    }
    if ((args_info.task_arg != task_arg_verification) && (alib.getDownLabeledTransitionsCount() > 0)) {
		abort(800, "PBNID is not supported for your task");
    }
    
    // handle unassigned transitions
    if (alib.getUnassignedTransitionsCount() != 0) {
        switch (args_info.unlabeledTransitions_arg) {
            case unlabeledTransitions_arg_none:
	            if (args_info.task_arg == task_arg_verification) {
	                abort(6, "Unknown confidence level used");
	            }
	            break;
            case unlabeledTransitions_arg_low:
                alib.assignUnassignedTransitions(anica::CONFIDENCE_LOW);
                break;
            case unlabeledTransitions_arg_high:
                alib.assignUnassignedTransitions(anica::CONFIDENCE_HIGH);
                break;
            case unlabeledTransitions_arg_down:
                alib.assignUnassignedTransitions(anica::CONFIDENCE_DOWN);
                break;
        }
    }
    
    // perform task
    if (args_info.task_arg == task_arg_verification) {
        result = alib.isSecure();
        if (result == 0) {
            message(_cgood_("net is secure"));
        }
        else {
            message(_cbad_("net is unsecure"));
        }
    }
    else if (args_info.task_arg == task_arg_characterization) {
        char** cuddVariableNames = new char*[net.getTransitions().size()];
        BDD* cuddOutput = new BDD();
        std::map<std::string, int> cuddVariables;
        
        Cudd* myBDD = alib.getCharacterization(cuddVariableNames, cuddOutput, cuddVariables);
        
        myBDD->info();
        message("%s - nodes: %d", _ctool_("Cudd"), myBDD->nodeCount(vector<BDD>(1, *cuddOutput)));
        
        //myBDD->DumpDot(vector<BDD>(1, *cuddOutput), cuddVariableNames);
        
        if (args_info.finalize_flag) {
            for (size_t i = 0; i < net.getTransitions().size(); ++i) {
                free(cuddVariableNames[i]);
            }
            delete[] cuddVariableNames;
            delete cuddOutput;
            cuddOutput = NULL;
            delete myBDD;
            myBDD = NULL;
        }    
   }
   else if (args_info.task_arg == task_arg_controllability) {
        PetriNet* controllerNet = alib.getControllerProblem();
        if (args_info.wendyCall_flag) {
            callWendy(*controllerNet);
        }
        else {
            ofstream o;
            o.open(string(fileName + ".controler" + ".owfn").c_str(), ios_base::trunc);
            o << io::owfn << *controllerNet;
            o.close();
            status("file %s created", _cfilename_(fileName + ".controler" + ".owfn"));
        }
        if (args_info.finalize_flag) {
            delete controllerNet;
        }
   }
    
    // Dot output
    if (args_info.dotConfidence_given) {
        PetriNet* curLibraryNet;
        switch (args_info.dotConfidence_arg) {
            case dotConfidence_arg_low:
                curLibraryNet = alib.colorConfidence(anica::COLOR_CONFIDENCE_TASK_LOW);
                break;
            case dotConfidence_arg_high:
                curLibraryNet = alib.colorConfidence(anica::COLOR_CONFIDENCE_TASK_HIGH);
                break;
            case dotConfidence_arg_down:
                curLibraryNet = alib.colorConfidence(anica::COLOR_CONFIDENCE_TASK_DOWN);
                break;
            case dotConfidence_arg_all:
                curLibraryNet = alib.colorConfidence(anica::COLOR_CONFIDENCE_TASK_ALL);
                break;
        }
        createDotFile(string(fileName + ".confidence"), *curLibraryNet, fileName);
        if (args_info.finalize_flag) {
            delete curLibraryNet;
        }
    }
    
    if (args_info.dotPotential_given) {
        PetriNet* curLibraryNet;
        switch (args_info.dotPotential_arg) {
            case dotPotential_arg_conflict:
                curLibraryNet = alib.colorPotentialPlaces(anica::COLOR_PLACE_TASK_CONFLICT);
                break;
            case dotPotential_arg_causal:
                curLibraryNet = alib.colorPotentialPlaces(anica::COLOR_PLACE_TASK_CAUSAL);
                break;
            case dotPotential_arg_both:
                curLibraryNet = alib.colorPotentialPlaces(anica::COLOR_PLACE_TASK_ALL);
                break;
        }
	    createDotFile(string(fileName + ".potential"), *curLibraryNet, fileName);
	    if (args_info.finalize_flag) {
            delete curLibraryNet;
        }
    }
    
    if (args_info.dotActive_given) {
        PetriNet* curLibraryNet;
        switch (args_info.dotPotential_arg) {
            case dotPotential_arg_conflict:
                curLibraryNet = alib.colorActivePlaces(anica::COLOR_PLACE_TASK_CONFLICT);
                break;
            case dotPotential_arg_causal:
                curLibraryNet = alib.colorActivePlaces(anica::COLOR_PLACE_TASK_CAUSAL);
                break;
            case dotPotential_arg_both:
                curLibraryNet = alib.colorActivePlaces(anica::COLOR_PLACE_TASK_ALL);
                break;
        }
        createDotFile(string(fileName + ".active"), *curLibraryNet, fileName);
	    if (args_info.finalize_flag) {
            delete curLibraryNet;
        }
    }

    if (args_info.resultFile_given) {
  	    Results results(args_info.resultFile_arg ? args_info.resultFile_arg : fileName + ".results");
        
        switch (args_info.property_arg) {
            case property_arg_PBNIPLUS_:
                results.add("non-interference.property", (char*)"PBNI+");
                break;
            case property_arg_PBNID:
                results.add("non-interference.property", (char*)"PBNID");
                break;
        }
        
        if (args_info.task_arg == task_arg_verification) {
            if (result == 0) {
                results.add("non-interference.result", (char*)"PASSED");
            }
            else {
                results.add("non-interference.result", (char*)"FAILED");
            }
        }

        results.add("meta.package_name", (char*)PACKAGE_NAME);
        results.add("meta.package_version", (char*)PACKAGE_VERSION);
        results.add("meta.svn_version", (char*)VERSION_SVN);
        results.add("meta.invocation", invocation);

        results.add("net.places", (unsigned int)(net.getPlaces().size()));
        results.add("net.transitions", (unsigned int)(net.getTransitions().size()));
        results.add("net.high", (unsigned int)alib.getHighLabeledTransitionsCount());
        results.add("net.down", (unsigned int)alib.getDownLabeledTransitionsCount());
        results.add("net.low", (unsigned int)alib.getLowLabeledTransitionsCount());
        results.add("net.arcs", (unsigned int)(net.getArcs().size()));
    }

    return EXIT_SUCCESS;
}
