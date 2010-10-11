/*****************************************************************************\
 Stanca --  Siphon/Trap Analysis and siphon-trap property Checking using the sAtisfiability solver MINISAT
 
 Copyright (c) 2010 Olivia Oanea
 
 Stanca is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.
 
 Stanca is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.
 
 You should have received a copy of the GNU Affero General Public License
 along with Stanca.  If not, see <http://www.gnu.org/licenses/>.
 \*****************************************************************************/

#include <config.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <map>
#include <algorithm>
#include "cmdline.h"
#include "verbose.h"
#include "config-log.h"
#include "config.h"
#include "pnapi.h"
#include <fstream>
#include <string>
#include "Output.h"
#include "STFormula.h"



using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using pnapi::Arc;


using namespace std;
using namespace Minisat;
using std::set;
using std::map;
using std::deque;
using std::cout;
using std::cerr;
using std::endl;
using std::set;
using std::vector;
using std::map;
using std::string;
using std::ofstream;
using std::ifstream;
using std::stringstream;
using std::ostringstream;
using std::exception;

using pnapi::io::owfn;
using pnapi::io::lola;

using pnapi::io::dot;
using pnapi::io::meta;
using pnapi::io::nets;
//using pnapi::io::InputError;
using pnapi::io::CREATOR;
using pnapi::io::INPUTFILE;
using pnapi::io::OUTPUTFILE;


/// the command line parameters
gengetopt_args_info args_info;



string invocation;


string toPl(int id);




void initGlobalVariables();
//void evaluateParameters(int argc, char** argv);
/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    //argv[0] = basename(argv[0]);
	
    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }
	
    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();
	
    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }
	
    // debug option
    /*if (args_info.bug_flag) {
        { Output debug_output("bug.log", "configuration information");
			debug_output.stream() << CONFIG_LOG << std::flush; }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }*/
	
	
	
    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one input file must be given");
    }
	
    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
	/* [USER] Add code here */
	
    // print statistics when stanca gets smarter
    /*if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        //fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        //system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }*/
}

///prints out a net with the respective assigment
void printAssignmentToFile(STStructure sts,PetriNet pn);
///prints out net with a trap within a siphon
void printSTAssignmentToFile(STStructure siphon, STStructure trap, PetriNet net);


int main(int argc, char **argv) {
	time_t start_time;
	
    // set the function to call on normal termination
    atexit(terminationHandler);
	
    /*--------------------------------------.
	 | 0. parse the command line parameters  |
	 `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);
	if(!args_info.inputs_num){
		exit(EXIT_SUCCESS);
	}
	
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
                abort(1, "could not open file '%s'", args_info.inputs[0]);
            }
			
            // parse the open net from the input file stream
            inputStream >> meta(pnapi::io::INPUTFILE, args_info.inputs[0])
			>> pnapi::io::owfn >> net;
        }
        // additional output if verbose-flag is set
        if (args_info.verbose_flag) {
            std::ostringstream s;
            s << pnapi::io::stat << net;
			// status("read net: %s", s.str().c_str());
        }
	} catch (pnapi::exception::InputError e) {
        abort(2, "could not parse file '%s': %s", args_info.inputs[0], e.message.c_str());
    }
	status("reading file '%s'", args_info.inputs[0]);
    //} catch(pnapi::io::InputError error) {
	//   std::ostringstream s;
	//  s << error;
	// abort(2, "\b%s", s.str().c_str());
    //}
	
	/*--------------------------------.
	 | 2. parse partial siphons/traps |
	 `-------------------------------*/	
	
	set<std::string> setsiphon;	
	if (args_info.semisiphon_arg){
		// set of strings involved
		
		for (unsigned i = 0; i < args_info.semisiphon_given; ++i){
 			std::string s=args_info.semisiphon_arg[i];
			setsiphon.insert(s);
 		}
		if (!args_info.semisiphon_given) {//this will never happen because we have a multi string
			//printout all solutions
			status("print out all siphons");
		}
	}
	
	
	set<std::string> settrap;
	if (args_info.semitrap_arg){
		// set of strings involved
		
		for (unsigned i = 0; i < args_info.semitrap_given; ++i){
 			std::string s=args_info.semitrap_arg[i];
			settrap.insert(s);
 		}
		if (!args_info.semitrap_given) {
			status("print out all traps");
			
		}
		
	}
	
	/*-------------------.
	 | 3. build formulas |
	 `-------------------*/		
	
	vector<bool> rs;//in case a formula is satisfiable, a satisfying assignment is returned
	vector<int> conflict;//in case a formula is unsatisfiable, a conflict clause is returned
	
	//some statistics no of places and max id
	
	start_time=clock();
	map<string, vector<unsigned int> > mp;///m(p)(1) is the id of a place
    // build formula
	
	if(args_info.siphonbasis_flag){
		cout << "siphon basis"<<endl;
		MinSiphonFormula tf(&net);
		tf.setFormula();
		if(args_info.verbose_flag) tf.printFormula();
		if(!tf.evaluateNontriviallyFormula()){
			cout << "gata"<<endl;//print minimal siphon
		}
	}
	else if ( (args_info.trap_flag || args_info.semitrap_arg) && !args_info.siphon_flag ){
		//traps need to be computed 
		cout << "trap"<<endl;
		TrapFormula tf(&net);
		tf.setFormula();
		if (args_info.semitrap_arg){
			//add  conjunction where all involved variables are 1
			for (set<string>::iterator it=settrap.begin(); it!= settrap.end(); ++it) {
				//find place and add clause
				tf.addUnitClause(*it);
			}
		}
		
		if(args_info.verbose_flag) tf.printFormula();
		if(tf.evaluateFormula(conflict)){
			tf.printSatAssignment();
			if (args_info.output_given) {
				printAssignmentToFile(*tf.getAssign(),net);
			}
			
			exit(0);
		}
		else {
			cout<<"The net does not contain any trap"<<endl;exit(0);
		}
		
		
	}
	else if ( (args_info.siphon_flag || args_info.semisiphon_arg ) &&  !args_info.maxtrap_flag &&  !args_info.trap_flag && !args_info.markedtrap_flag){
		cout << "siphon"<<endl;
		SiphonFormula tf(&net);
		tf.setFormula();		
		//siphons have to be computed 
		if (args_info.semisiphon_arg){
			//add  conjunction where all involved variables are 1
			for (set<string>::iterator it=setsiphon.begin(); it!= setsiphon.end(); ++it) {
				//find place and add clause
				tf.addUnitClause(*it);
			}
		}
		if(args_info.verbose_flag) tf.printFormula();
		if(tf.evaluateFormula(conflict)){
			tf.printSatAssignment();
			
			if (args_info.output_given) {
				printAssignmentToFile(*tf.getAssign(),net);
			}
			exit(0);
		}
		else {
			cout<<"The net does not contain any siphon"<<endl;
			exit(0);
		}
	}
	else if ( args_info.trap_flag && args_info.siphon_flag){
		cout << "siphon with non-empty trap inside check: "<<endl;
		SwTFormula tf(&net);
		tf.setFormula();
		tf.setNonemptyTrap();
		if(args_info.verbose_flag) tf.printFormula();
		if(tf.evaluateFormula(conflict)){
			tf.printSatAssignment();
			if (args_info.output_given) {
				printSTAssignmentToFile(tf.getSiphon(), tf.getTrap(),net);
			}
			exit(0);
			}
		else {
			cout<<"The net does not contain any siphon with a non-empty trap"<<endl;
			exit(0);
		}
	
	}
	else if ( args_info.markedtrap_flag && args_info.siphon_flag){
		cout << "siphon with non-empty marked trap inside check: "<<endl;
		SwTFormula tf(&net);
		tf.setFormula();
		tf.setMarkedTrap();
		if(args_info.verbose_flag) tf.printFormula();
		if(tf.evaluateFormula(conflict)){
			tf.printSatAssignment();
			if (args_info.output_given) {
				printSTAssignmentToFile(tf.getSiphon(), tf.getTrap(),net);
			}
			exit(0);
		}
		else {
			cout<<"The net does not contain any siphon with a non-empty marked trap inside"<<endl;
			exit(0);
		}
		
	}	
	else if(args_info.maxtrap_flag||args_info.maxsiphon_flag){
		PFormula *tf;
		if(args_info.maxtrap_flag && !args_info.siphon_flag){
			cout << "max trap "<<endl;
			tf=new MaxTrapFormula(&net);
		}
		else if(args_info.maxtrap_flag && args_info.siphon_flag){
			cout << "siphon with max trap check "<<endl;
			tf=new SwMTFormula(&net);
		}
		
		else if(args_info.maxsiphon_flag){
			cout << "max siphon "<<endl;
			tf=new MaxSiphonFormula(&net);
		}
		else {
			status("invalid option");
			exit(0);
		}
		tf->setFormula();
		if(args_info.verbose_flag) tf->printFormula();
		if(tf->evaluateFormula(conflict)){
			if(args_info.maxtrap_flag && args_info.siphon_flag)
				tf->printSatAssignment(false);
			else tf->printSatAssignment(false);
			
			if (args_info.output_given) {
				printAssignmentToFile(*(*tf).getAssign(),net);
			}
			//exit(0);
		}
		else {
			if(args_info.maxtrap_flag){
				cout<<"The net does not contain any trap"<<endl;
			}
			else{
				cout<<"The net does not contain any siphon"<<endl;
			}
			//exit(0);
		}
	
	}
	else{
		//in this case we have the general formula
		cout << "ST property"<<endl;
		STFormula tf(&net);
		//cout << "initialization"<<endl;
		tf.setFormula();
		//cout << "printing"<<endl;
		if(args_info.verbose_flag) tf.printFormula();
		if(tf.evaluateFormula(conflict)){
		   //get a counterexample
			tf.setNonemptyTrap();
			if(!tf.evaluateFormula(conflict)){
				//display empty siphon
				
			}	
			else {
				// display solution siphon with unmarked trap
			}

			//tf.evaluateNontriviallyFormula(rs, conflict);
		}
		
		//else cout<<"ST property holds"<<endl;
		//exit(0);
	//}
	}

	// time

	std::cerr << endl << "Stanca : runtime: " << ((double(clock()) - double(start_time)) / CLOCKS_PER_SEC) << " sec\n";
	//printf("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_time)) / CLOCKS_PER_SEC);
	cout << endl;
	exit(0);
}


/**Prints out a net with the respective trap/siphon place assignment in a dot file
 @param sts the place structure
 @param net the net to which it belongs
 **/
void printAssignmentToFile(STStructure sts, PetriNet net){
	if(!args_info.output_given) return;
	for (unsigned int i=0; i<sts.set.size(); ++i) {
		if(sts.set.at(i)) {
			pnapi::Place *p=net.findPlace(sts.st[i]);
			if(sts.szs==tr) p->setColor("red");
			else if (sts.szs==sf) p->setColor("blue");
		}
	}
	std::string outputParam;
    std::string fileName;
    if (args_info.output_given and args_info.output_arg) {
        fileName = args_info.output_arg;
    } else {
		string initial=args_info.inputs[0];
		fileName = initial+".dot";
    }

    // add the output filename
	ofstream fout(fileName.c_str());
	if(!fout.is_open()) abort(2,"error while writing to file %", fileName.c_str());
	fout << pnapi::io::dot << net;
	fout.close();
	cerr << "wrote .dot file '" << fileName << "':\t" << pnapi::io::stat << net << endl;
}

/**Prints out a net with a trap & siphon place assignment in a dot file
 @param siphon the siphon structure
 @param trap the trap structure
 @param net the net to which they belong
 **/
void printSTAssignmentToFile(STStructure siphon, STStructure trap, PetriNet net){
	if(!args_info.output_given) return;
	for (unsigned int i=0; i<siphon.set.size(); ++i) {
		if(siphon.set.at(i) && trap.set.at(i)) {
			pnapi::Place *p=net.findPlace(siphon.st[i]);
			p->setColor("violet");
		}
		else if(siphon.set.at(i) && !trap.set.at(i)){
			pnapi::Place *p=net.findPlace(siphon.st[i]);
			p->setColor("blue");
		}
		else if(!siphon.set.at(i) && trap.set.at(i)){
			pnapi::Place *p=net.findPlace(trap.st[i]);
			p->setColor("red");
		}		
	}
	std::string outputParam;
    std::string fileName;
    if (args_info.output_given and args_info.output_arg) {
        fileName = args_info.output_arg;
    } else {
		string initial=args_info.inputs[0];
		fileName = initial+".dot";
    }
	
    // add the output filename
	ofstream fout(fileName.c_str());
	if(!fout.is_open()) abort(2,"error while writing to file %", fileName.c_str());
	fout << pnapi::io::dot << net;
	fout.close();
	cerr << endl << "wrote .dot file '" << fileName << "':\t" << pnapi::io::stat << net << endl;
}



