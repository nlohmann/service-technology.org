/*****************************************************************************\
 Anastasia -- Analyzer for siphons & traps in arbitrary nets

 Copyright (c) 2010 Harro Wimmel

 Anastasia is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Anastasia is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Anastasia.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <pnapi/pnapi.h>
#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "formula.h"
#include "witness.h"

using namespace pnapi;
using std::vector;
using std::set;
using std::map;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::ostream;
using std::ofstream;

/// the command line parameters
gengetopt_args_info args_info;

/// verbose flag
bool flag_verbose(false);

/// the invocation string
std::string invocation;

/// a file to store a mapping from marking ids to actual Petri net markings
Output* markingoutput = NULL;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// parser supplement (anastasia) for formulae
extern FILE *anastasia_in;
extern int anastasia_parse();
extern Formula* pf;
extern vector<setVar> mainvars;
vector<string> mainvarName;
setVar generateFormula(Formula& f);

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
            abort(14, "error reading configuration file '%s'", args_info.config_arg);
        } else {
            status("using configuration file '%s'", args_info.config_arg);
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
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
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
    /* [USER] Add code here */

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        int i(system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str()));
    }
}

/// main-function
int main(int argc, char** argv) {
//    time_t start_time, end_time;

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);
	flag_verbose = args_info.verbose_given;

    /*-----------------------.
    | 1. parse the Petri net |
    `-----------------------*/
    pnapi::PetriNet net;

    try {
        // parse either from standard input or from a given file
        if (args_info.inputs_num == 0) {
            // parse the open net from standard input
            if (args_info.owfn_given) std::cin >> pnapi::io::owfn >> net;
            else if (args_info.lola_given) std::cin >> pnapi::io::lola >> net;
            else std::cin >> pnapi::io::pnml >> net;
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
			if (args_info.owfn_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::owfn >> net;
			else if (args_info.lola_given)
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::lola >> net;
			else 
	            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
    	                    >> pnapi::io::pnml >> net;
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

    /*----------------------------------------.
    | 2. Make a loop (for workflow nets)      |
    `----------------------------------------*/

	if (args_info.loopnet_given) {
		Transition& t(net.createTransition("#loop_out_to_in#"));
		const set<Place*> pset(net.getPlaces());
		set<Place*>::const_iterator pit;
		for(pit=pset.begin(); pit!=pset.end(); ++pit)
		{
			if ((*pit)->getPreset().empty()) net.createArc(t,**pit);
			if ((*pit)->getPostset().empty()) net.createArc(**pit,t);
		}
	}

    /*----------------------------------------.
    | 3. Check the Property                   |
    `----------------------------------------*/

	// first some info for the option --cover going over all the places
	vector<Place*> loop; // a vector collecting all the generating places, contains a single NULL if none
	map<set<Place*>,set<Place*> > coverinfo; // maps results to the set of generating places they were created from
	map<set<Place*>,vector<set<Place*> > > extrainfo; // other sets of interest for the same result as in coverinfo
	map<set<Place*>,set<Place*> >::iterator mit;
	set<Place*> uncovered; // the set of places for which no solutions were found, complementing coverinfo (2nd component)
	if (args_info.cover_given) { // if --cover is given, put all places in loop
		set<Place*> pset(net.getPlaces());
		set<Place*>::iterator pit;
		for(pit=pset.begin(); pit!=pset.end(); ++pit)
			loop.push_back(*pit);
	} else loop.push_back(NULL); // otherwise a single NULL (just one pass through the program)

	time_t mytime(clock()); // measure time for formula creation, evaluation, and minimaxing
	// here we start generating the formula to be checked, according to the selected property	
	Formula* gf(NULL); // general formula (for option --cover only)	
	setVar X(0); // the main variable for which the user wants so see the satisfying assignment
	if (args_info.cover_given && !args_info.nocopy_given) // formula will be needed more than once and may be copied
	{	
		gf = new Formula(net);
		X = generateFormula(*gf);
	}

	for(unsigned int pc=0; pc<loop.size(); ++pc) // one pass for each generating place
	{
		if (pc>0 && !args_info.min_given && !args_info.pmin_given) { // if we don't minimize
			for(mit=coverinfo.begin(); mit!=coverinfo.end(); ++mit) // every place appearing in a solution
				if (mit->first.find(loop[pc])!=mit->first.end())
				{ mit->second.insert(loop[pc]); break; } // generates this solution
			if (mit!=coverinfo.end()) continue; // so we don't have to work on it again if we find the current place in a solution
		}

		set<Place*> enforce,forbid; // for enforcing or forbidding place membership in a solution
		Place* p(NULL);
		for(unsigned int i=0; i<args_info.include_given; ++i) // read in the option --include
		{
			p = net.findPlace(args_info.include_arg[i]);
			if (p) enforce.insert(p);
			p = NULL;
		}	
		for(unsigned int i=0; i<args_info.exclude_given; ++i) // read in the option --exclude
		{
			p = net.findPlace(args_info.exclude_arg[i]);
			if (p) forbid.insert(p);
			p = NULL;
		}

		// here we start generating the formula to be checked, according to the selected property	
		Formula f(net);	
		if (gf) f.copyFormula(*gf); // we are allowed to copy the general formula
		else X = generateFormula(f); // the general formula does not exist, we build our formula here

		// now we add the forced and forbidden places to the formula
		for(set<Place*>::iterator pit=enforce.begin(); pit!=enforce.end(); ++pit)
			f.SetTo(f.getVar(X,**pit),true);
		for(set<Place*>::iterator pit=forbid.begin(); pit!=forbid.end(); ++pit)
			f.SetTo(f.getVar(X,**pit),false);
		// and we add the generating place (current place) if it exists
		if (loop[pc]) f.SetTo(f.getVar(X,*(loop[pc])),true);
		if (args_info.verbose_given) { 
			status("Formula constructed%s in %.2f seconds.",(loop[pc]?" for "+loop[pc]->getName():"").c_str(),(float)(clock()-mytime)/CLOCKS_PER_SEC);
			mytime = clock();
		}
		if (args_info.clauses_given) { // the user wants to see the clauses, so print them
			if (args_info.clauses_arg) { // either to the given file
				ofstream outfile(args_info.clauses_arg);
				if (!outfile.is_open()) abort(6,"error while writing clauses to file '%s'",args_info.clauses_arg);
				f.printClauses(outfile);
				outfile.close();
			} else f.printClauses(cout); // or to stdout
			mytime = clock(); // printing the clauses doesn't count for the solving time
		}
		if (!f.solve()) {
			uncovered.insert(loop[pc]); // if no solution is found, remember that
			if (args_info.verbose_given) {
				status("SAT-solving failed%s in %.2f seconds.",(loop[pc]?" for "+loop[pc]->getName():"").c_str(),(float)(clock()-mytime)/CLOCKS_PER_SEC);
				mytime = clock();
			}
		} else { // if there is a solution
			if (args_info.verbose_given) {
				status("SAT-solving successful%s in %.2f seconds.",(loop[pc]?" for "+loop[pc]->getName():"").c_str(),(float)(clock()-mytime)/CLOCKS_PER_SEC);
				mytime = clock();
			}
			if (loop[pc]) f.removeLastClause(); // remove the clause for the generating place (technical reasons)
			// and minimize or maximize once
			if (args_info.min_given || args_info.pmin_given || args_info.max_given) 
				f.MinMax(X,args_info.max_given,&enforce,&forbid,loop[pc]);
			// if minimization should be able to remove the generating place, try again
			if (loop[pc] && args_info.min_given) 
				f.MinMax(X,false,&enforce,&forbid);
			set<Place*> pset(f.getAssignment(X)); // get the satisfying assignment for X
			coverinfo[pset].insert(loop[pc]); // and remember it
			for(unsigned int k=1; k<mainvars.size(); ++k) // get the assignments for other important variables and remember them
				extrainfo[pset].push_back(f.getAssignment(mainvars[k]));
			if (args_info.verbose_given && (args_info.min_given || args_info.pmin_given || args_info.max_given)) {
				status("Minimax done%s in %.2f seconds.",(loop[pc]?" for "+loop[pc]->getName():"").c_str(),(float)(clock()-mytime)/CLOCKS_PER_SEC);
				mytime = clock();
			}
		}
	}
	delete gf; // destroy the formula

	// we are done, now we can print the result
	set<Place*>::iterator pit;
	if (coverinfo.size()==0) { // if there is no positive result ...
		if (args_info.utrap_given) cout << "No unmarked trap found." << endl;
		if (args_info.mtrap_given) cout << "No marked trap found." << endl;
		if (args_info.trap_given) cout << "No trap found." << endl;
		if (args_info.usiphon_given) cout << "No unmarked siphon found." << endl;
		if (args_info.msiphon_given) cout << "No marked siphon found." << endl;
		if (args_info.siphon_given) cout << "No siphon found." << endl;
		if (args_info.swot_given || args_info.swomt_given)
			cout << "Every siphon contains a " << (args_info.swomt_given?"marked ":"") << "trap." << endl;
		if (args_info.formula_given) cout << "The formula is unsatisfiable." << endl;
	} else if (args_info.snoopy_given && !args_info.witness_given) { // if output format snoopy was selected ...
		if (args_info.min_given) cout << " minimal";
		if (args_info.pmin_given) cout << " place-minimal";
		if (args_info.max_given) cout << " maximal";
		if (args_info.swot_given) cout << " bad siphon";
		if (args_info.swomt_given) cout << " non-ch siphon";
		if (args_info.utrap_given) cout << " unmarked trap";
		if (args_info.mtrap_given) cout << " marked trap";
		if (args_info.trap_given) cout << " trap";
		if (args_info.usiphon_given) cout << " unmarked siphon";
		if (args_info.msiphon_given) cout << " marked siphon";
		if (args_info.siphon_given) cout << " siphon";
		if (args_info.formula_given) cout << " satisifying set";
		cout << "s ( place ) = " << endl << endl;
		unsigned int nr(1);
		for(mit=coverinfo.begin(); mit!=coverinfo.end(); ++mit,++nr)
		{
			cout << nr;
			bool comma(false);
			for(pit=mit->first.begin(); pit!=mit->first.end(); ++pit, comma=true)
			{
				if (comma) cout << "," << endl;
				cout << "\t|";
				if ((*pit)->getName().find('.')!=string::npos && (*pit)->getName()[0]=='P')
					cout << (*pit)->getName().substr(1);
				else cout << (*pit)->getName();
				cout << "\t:1";
			}
			cout << endl;
		}
	} else if (!args_info.snoopy_given) { // otherwise we print the positive result(s) ...
		if (loop[0]==NULL) cout << "A"; else cout << "Covering"; // if we looped through all places
		if (args_info.min_given) cout << " minimal";
		else if (args_info.pmin_given) cout << " place-minimal";
		else if (args_info.max_given) cout << " maximal";
		else if (args_info.utrap_given|| args_info.usiphon_given) cout << (loop[0]?"":"n");
		if (args_info.swot_given) cout << " siphon" << (loop[0]?"s":"") << " without a trap";
		if (args_info.swomt_given) cout << " siphon" << (loop[0]?"s":"") << " without a marked trap";
		if (args_info.utrap_given) cout << " unmarked trap";
		if (args_info.mtrap_given) cout << " marked trap";
		if (args_info.trap_given) cout << " trap";
		if (args_info.usiphon_given) cout << " unmarked siphon";
		if (args_info.msiphon_given) cout << " marked siphon";
		if (args_info.siphon_given) cout << " siphon";
		if (args_info.formula_given) cout << " satisfying set";
		if (!args_info.swot_given && !args_info.swomt_given && loop[0]) cout << "s";
		if (args_info.formula_given) cout << " for " << mainvarName[0];
		cout << ":" << endl;
		for(mit=coverinfo.begin(); mit!=coverinfo.end(); ++mit)
		{
			if (loop[0]) { // with --cover first print which generating places were covered by this result
				cout << "Covering (";
				bool comma(false);
	 			for(pit=mit->second.begin(); pit!=mit->second.end(); ++pit,comma=true)
				{
					if (comma) cout << ",";
					cout << (*pit)->getName();
				}
				cout << "):" << endl;
			} // and the print the result
			bool something(false);
			for(pit=mit->first.begin(); pit!=mit->first.end(); ++pit,something=true)
				cout << (*pit)->getName() << " ";
			if (!something) cout << "<empty set>";
			cout << endl;
			for(unsigned int k=1; k<mainvars.size(); ++k)
			{
				something=false;
				cout << "A corresponding satisfying assignment for " << mainvarName[k] << " is" << endl;
				for(pit=extrainfo[mit->first][k-1].begin(); pit!=extrainfo[mit->first][k-1].end(); ++pit,something=true)
					cout << (*pit)->getName() << " ";
				if (!something) cout << "<empty set>";
				cout << endl;
			}
		}
		if (uncovered.size()>0) { // if there were non-coverable places, show them
			cout << "Non-coverable places: ";
			for(pit=uncovered.begin(); pit!=uncovered.end(); ++pit)
				cout << (*pit)->getName() << " ";
			cout << endl;
		}
	}

    /*----------------------------------------.
    | 3. Check for a Witness Path             |
    `----------------------------------------*/

	// we may check for a witness path if the previous algorithm provided us with
	// an fc-net and a siphon containing no marked trap
	if (args_info.witness_given && coverinfo.size()==0) 
		status("no result to forward to witness finder");
	if (args_info.witness_given && coverinfo.size()>0) {
		if (!net.isFreeChoice()) abort(20,"Petri net is not free-choice");
		if (args_info.cover_given) status("only the first found result is checked");
		if (args_info.verbose_flag) sara::setVerbose();
		mytime = clock();
		Witness w(net); // data structures for a siphon dependency graph
		w.initSWOMT(coverinfo.begin()->first); // with its initial node being the found siphon
		vector<Transition*> fseq; // the firing sequence, to be filled by computeGraph
		set<Place*> siphon(w.computeGraph(fseq)); // here the graph gets constructed and we find a result
		status("emptying path found%s in %.2f seconds.",(loop[0]?" for "+loop[0]->getName():"").c_str(),(float)(clock()-mytime)/CLOCKS_PER_SEC);
		set<Place*>::iterator pit;
		if (args_info.snoopy_given) { // output for snoopy
			cout << "deadlocking siphons ( place ) = " << endl << endl;
			unsigned int nr(1);
			for(mit=coverinfo.begin(); mit!=coverinfo.end(); ++mit,++nr)
			{
				cout << nr;
				bool comma(false);
				for(pit=siphon.begin(); pit!=siphon.end(); ++pit, comma=true)
				{
					if (comma) cout << "," << endl;
					cout << "\t|";
					if ((*pit)->getName().find('.')!=string::npos && (*pit)->getName()[0]=='P')
						cout << (*pit)->getName().substr(1);
					else cout << (*pit)->getName();
					cout << "\t:1";
				}
				cout << endl << endl;
				cout << "% Remove this and the following two lines if Snoopy rejects this output." << endl;
				cout << "% The siphon is emptied as far as possible by the firing sequence" << endl << "%";
				for(unsigned int i=0; i<fseq.size(); ++i)
					cout << " " << fseq[i]->getName();
				if (fseq.empty()) cout << "<empty>";
				cout << endl;
				break;
			}
		} else { // general output
			cout << "The firing sequence \"";
			bool comma(false);
			for(unsigned int i=0; i<fseq.size(); ++i,comma=true)
				cout << (comma?" ":"") << fseq[i]->getName();
			if (fseq.empty()) cout << "<empty>";
			cout << "\" transforms the siphon {";
			comma = false;
			for(pit=siphon.begin(); pit!=siphon.end(); ++pit,comma=true)
				cout << (comma?",":"") << (*pit)->getName();
			cout << "} to a deadlock set." << endl;
		}
	}


    return EXIT_SUCCESS;
}


setVar generateFormula(Formula& f) {
	// here we start generating the formula to be checked, according to the selected property	
	if (args_info.clauses_given) f.verbose=1;
	setVar X(0),Y(0);
	if (args_info.formula_given) { // read a user-defined formula
		pf = &f; // tell the parser where the formula has to go
		anastasia_in = fopen(args_info.formula_arg,"r");
		if (!anastasia_in) abort(3, "could not open file '%s' with user-defined formula",args_info.formula_arg);
		anastasia_parse(); // parse the formula
		fclose(anastasia_in);
		X = mainvars[0]; // the set variable the user is interested in
	} else { // use a predefined formula
		X = f.createSetVar("X"); // the main set variable
		if (args_info.swot_given || args_info.swomt_given)
			Y = f.createSetVar("Y"); // secondary set variable, only used for complex properties

		if (args_info.trap_given || args_info.utrap_given || args_info.mtrap_given) f.XTrap(X); // X should be a trap
		else f.XSiphon(X); // or a siphon
		f.XNEmpty(X); // that is not empty
		if (args_info.msiphon_given || args_info.mtrap_given)
			f.XMarked(X); // it should be marked
		if (args_info.usiphon_given || args_info.utrap_given)
			f.XNMarked(X); // it should be unmarked
		if (args_info.swot_given || args_info.swomt_given) {
			f.XMaxTrap(X,Y); // for complex properties, where Y is the maximal trap in X
			if (args_info.swot_given) f.XEmpty(Y); // Y must be empty
			else if (args_info.swomt_given) f.XNMarked(Y); // resp. unmarked
		}
		mainvars.clear();
		mainvars.push_back(X);
	}
	// collect the names of the important set variables for later use (in case the formula is forgotten)
	mainvarName.clear();
	for(unsigned int k=0; k<mainvars.size(); ++k)
		mainvarName.push_back(f.getSetVarName(mainvars[k]));
	return X;
}


/* <<-- CHANGE END -->> */
