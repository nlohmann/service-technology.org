/*****************************************************************************\
 Maxis -- Construct maximal services

 Copyright (C) 2010  Jarungjit Parnjai <parnjai@informatik.hu-berlin.de>

 Maxis is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Maxis is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Maxis.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <map>
#include <set>
#include <string>
#include <queue>

#include <config.h>
#include "cmdline.h"
#include "util.h"
#include "formula.h"
#include "dnf.h"
#include "verbose.h"

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using std::ostringstream;
using std::stringstream;
using std::map;
using std::multimap;
using std::pair;
using std::string;
using std::equal;
using std::set;
//using std::queue;
using std::ostream;

/// the command line parameters
gengetopt_args_info args_info;

/// lexer and parser
extern int yyparse();
extern int yylex_destroy();
extern FILE* yyin;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// output stream
std::ostream* myOut = &cout;

/****************************
 * Variables used by parser *
 ****************************/
/// non deterministic successor list of recent OG
map<unsigned int, multimap<string, unsigned int> > succ;
/// annotations of recent OG
map<unsigned int, Formula *> formulae;
/// satisfying assignments of recent OG
map<unsigned int, Choice *> dnf_choices;
/// input labels of recent OG
set<string> inputs;
/// output labels of recent OG
set<string> outputs;
/// synchronous labels of recent OG
set<string> synchronous;
/// present labels of recent OG
map<unsigned int, set<string> > presentLabels;
/// initial node ID of recent OG
unsigned int * initialID = NULL;

/***************************
 * Variables used by Maxis *
 ***************************/

// verbose and debug flag
dbg_flag verbose = off;
dbg_flag debug = off;

// the invocation string
string invocation;

// final states
set<unsigned int> finalIDs;

// global temporary buffer for each state
multimap<string, unsigned int> removeArcs;

// number of nodes/states
unsigned int number_of_nodes = 0;
unsigned int node_index = 0;

liberal liberal_flag = normal;

/// mapping from ID to state
//map<unsigned int, vector<unsigned int> > ID2state;


void dumpTest() {

	cout << PACKAGE << " : ---------" << endl;
	cout << PACKAGE << " : succ " << endl;
	for( map<unsigned int, multimap<string, unsigned int> >::iterator it=succ.begin(); it!=succ.end(); ++it) {
		cout << it->first << "::" << endl;
		multimap<string, unsigned int> element = it->second;
		for( multimap<string, unsigned int>::iterator sit=element.begin(); sit!=element.end(); ++sit) {
	       cout << sit->second << ", ";
		}
		cout << endl;
	 }

     cout << PACKAGE << " : ---------" << endl;
     cout << PACKAGE << " : Choice " << endl;
     for(map<unsigned int, Choice *>::iterator it=dnf_choices.begin(); it !=dnf_choices.end(); ++it)
     {
    	 cout << it->first << ": " << endl;
    	 set< set<string> > choices = (it->second)->get_all_choices();
    	 for(set< set<string> >::iterator cit= choices.begin(); cit!= choices.end(); ++cit)  {
    		 set<string> ch = * cit;
    		 cout << "[, ";
    		 for(set<string>::iterator sit= ch.begin(); sit!= ch.end(); ++sit) {
    			 cout << sit->data() << ", ";
    		 }
    		 cout << "], ";
    	 }
    	 cout << endl;
     }

/*cout << "for each Choice : ";
set< set<string> > choices = ch->get_all_choices();
for(set< set<string> >::iterator cit= choices.begin(); cit!= choices.end(); ++cit)  {
   set<string> ch = * cit;
   for(set<string>::iterator sit= ch.begin(); sit!= ch.end(); ++sit) {
	       cout << sit->data() << " * ";
   }
   cout <<  ", ";
}
cout << endl;
*/
}

/****************************************
 * evaluate the command line parameters *
 ****************************************/
void evaluate_parameters(int argc, char** argv)
{
	// initialize the parameters structure
	struct cmdline_parser_params *params = cmdline_parser_params_create();


	// call the cmdline parser
	if (cmdline_parser(argc, argv, &args_info) != 0) {
		abort(7, "invalid command-line parameter(s)");
	}

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
    }

    if(args_info.verbose_flag) { verbose = on; }
    if(args_info.debug_flag) { debug = on; }

	if(args_info.liberal_given) {
		switch (args_info.liberal_arg) {
		   case 0 :  liberal_flag = normal;
					  if(verbose == on) { message("compute a maximal partner as stated in the input file"); }
					  break;
			case 1 :  liberal_flag = minimal;
					  if(verbose == on) { message("compute a least liberal maximal partner"); }
					  break;
			case 2 :  liberal_flag = maximal;
					  if(verbose == on) {   message("compute a most liberal maximal partner"); }
					  break;
			default : liberal_flag = normal;
   			          if(verbose == on) {
   						message("no input liberal mode. Now set to the default value '0'.");
   			 		    message("compute a maximal partner as stated in the input file");
   			          }
					  break;
		}
	} else {
		liberal_flag = normal;
		if (verbose == on) {
			message("no input liberal mode. Now set to the default value '0'.");
 		    message("compute a maximal partner as stated in the input file");
		}
	}

	// if none of input OG files is given
	if(args_info.inputs_num == 0) {
		message("ERROR : an OG input file must be given");
		status("terminated with failure!");
		exit(EXIT_FAILURE);
	}

	// set output destination
	if(args_info.output_given) {
		ofstream * ofs = new ofstream(args_info.output_arg, std::ios_base::trunc);
		// if an error occurred on opening the file
		if(!ofs)  {
			message("ERROR : failed to open output file %s", args_info.output_arg);
			status("terminated with failure!");
			exit(EXIT_FAILURE);
		}

		// else link output stream to file stream
		myOut = ofs;
	}

	free(params);

//	cmdline_parser_free(&args_info);
}

/***********************
 * parse OG input file *
 ***********************/
void parse_OG()
{
    // open file and link input file pointer
    yyin = fopen(args_info.inputs[0], "r");
    if(!yyin) // if an error occurred
    {
      message("ERROR : failed to open input file %s",args_info.inputs[0]);
      status("terminated with failure!");
      exit(EXIT_FAILURE);
    }

    // parsing OG
    yyparse();
}


void dump()
{
	// check parsed output
    cout << PACKAGE << " : ---------" << endl;
    cout << PACKAGE << " : succ " << endl;
    for( map<unsigned int, multimap<string, unsigned int> >::iterator it=succ.begin(); it!=succ.end(); ++it) {
	    cout << it->first << "::" << endl;
	    multimap<string, unsigned int> element = it->second;
        for( multimap<string, unsigned int>::iterator sit=element.begin(); sit!=element.end(); ++sit) {
 	       cout << sit->second << ", ";
        }
        cout << endl;
 	 }

    cout << PACKAGE << " : ---------" << endl;
    cout << PACKAGE << " : formula " << endl;
    for( map<unsigned int,Formula * >::iterator it=formulae.begin(); it!=formulae.end(); ++it) {
 	       cout << it->first << ": " << it->second << endl;
 	 }

    cout << PACKAGE << " : ---------" << endl;
    cout << PACKAGE << " : inputs " << endl;
    for (set<string>::iterator it=inputs.begin();  it!= inputs.end(); ++it) {
      cout << it->data() << ",";
    }
    cout << endl;

    cout << PACKAGE << " : ---------" << endl;
    cout << PACKAGE << " : outputs " << endl;
    for (set<string>::iterator it=outputs.begin();  it!= outputs.end(); ++it) {
      cout << it->data() << ",";
    }
    cout << endl;

    cout << PACKAGE << " : ---------" << endl;
    cout << PACKAGE << " : synchronous " << endl;
    for (set<string>::iterator it=synchronous.begin();  it!= synchronous.end(); ++it) {
      cout << it->data() << ",";
    }
    cout << endl;

    cout << PACKAGE << " : ---------" << endl;
    cout << PACKAGE << " : presentLabels " << endl;
    for( map<unsigned int, set<string> >::iterator it=presentLabels.begin(); it!=presentLabels.end(); ++it) {
	    cout << it->first << "::" << endl;
	    set<string> element = it->second;
	    for (set<string>::iterator sit=element.begin();  sit!= element.end(); ++sit) {
 	       cout << sit->data() << ", ";
        }
        cout << endl;
 	 }
    /// initial node ID of recent OG
    cout << PACKAGE << " : ---------" << endl;
    cout << PACKAGE << " : initialID " << (*initialID) << endl;

    cout << PACKAGE << " : ---------" << endl;
}

/* ********************************************************************
 * generic routine to add an internal tau choice                      *
 * for each choice, add an intermediate internal tau edge to  choices *
 **********************************************************************/
void add_an_internal_choice(std::set<std::string> choice,
		unsigned int sourceNode,
		multimap<string, unsigned int> * arcMap)
{
	node_index++;
	multimap<string, unsigned int> oldArcs;

	if (debug == on) { cout << "starting with index : " << sourceNode << endl; }
	// there are possibly too many literals described in  arcMap
	// get only arc that describe by all literals in the formula => only for a lest liberal mode #TODO#
	for( std::set<std::string>::iterator it=choice.begin(); it!=choice.end(); ++it) {
		std::string ch  = *it;
    	if (ch == "false") {   // #TODO : check for case sensitive
    		break;   // skip, do nothing
    	}
    	if (ch == "final") {  // #TODO : check for case sensitive
			finalIDs.insert(node_index);
	    	if (debug == on) { cout << "    for each : final " << endl; }
			continue;    // check for the cases that contain final in the conjunction, e.g., final * a * b
    	}
    	if (ch == "true") { // #TODO : (1) check for case sensitive, and (2) check liberal mode
	    	if (debug == on) { cout << "    for each : true " << endl; }
			continue;    // check for the cases that contain final in the conjunction, e.g., final * a * b
    	}
    	if (debug == on) { cout << "    for each : " << ch  << endl; }

        // mark arcs labeled with ch from souceNode as removing arcs
    	pair< multimap<string, unsigned int>::iterator, multimap<string, unsigned int>::iterator> range = arcMap->equal_range(ch);
 		for (multimap<string, unsigned int>::iterator mit=range.first; mit!=range.second; ++mit) {

		    oldArcs.insert(multimap<string, unsigned int>::value_type(mit->first, mit->second));
			removeArcs.insert(multimap<string, unsigned int>::value_type(mit->first, mit->second));
 		}

	}

	arcMap->insert( multimap<string, unsigned int>::value_type("TAU ", node_index) );
 	succ[node_index] = oldArcs;

 	if (debug == on) {
		cout << "reading oldArcs : ";
		for (multimap<string, unsigned int>::iterator it=oldArcs.begin(); it!=oldArcs.end(); ++it) {
			cout << "(" << it->first << "," << it->second << "), ";
		}
		cout <<  endl;
		cout << "adding tau Arcs to node : " << node_index << endl << endl;
	}
}

/* ********************************************************************
 * generic routine to add an internal tau choice                      *
 * for each choice, add an intermediate internal tau edge to  choices *
 **********************************************************************/
void add_extra_choices(unsigned int sourceNode, multimap<string, unsigned int> * arcMap)
{
	multimap<string, unsigned int> oldArcs;

 	// construct a TAU arc to a new divergent node
   	oldArcs.clear();
	arcMap->insert( multimap<string, unsigned int>::value_type("TAU ", ++node_index) );
	oldArcs.insert(multimap<string, unsigned int>::value_type("TAU ",node_index));
	succ[node_index] = oldArcs;

  	// construct a TAU arc from a sourceNode to a new deadlock Node
	arcMap->insert( multimap<string, unsigned int>::value_type("TAU ", ++node_index) );
	succ[node_index] = oldArcs;

	// construct a TAU arc from a sourceNode to a new final Node
	Choice * choice = dnf_choices[sourceNode];
	std::set<std::string> ch;
	ch.insert("final");
	if (!choice->contains(ch)) { // check if there exists already a TAU arc to a final node
		oldArcs.clear();
		arcMap->insert( multimap<string, unsigned int>::value_type("TAU ", ++node_index) );
		succ[node_index] = oldArcs;
		finalIDs.insert(node_index);
	}
}

/****************************************
 * construct a maximal partner from OGs *
 ***************************************/
void construct_maximal_partner()
{
	if (debug == on) {
		cout << "++++++++++++++++ before +++++++++++++++++++++"<< endl;
		dumpTest();
	}

    number_of_nodes = succ.size();

     for( map<unsigned int, Choice *>::iterator it=dnf_choices.begin(); it!=dnf_choices.end(); ++it)
     {
	    unsigned int node = it->first;    // node id
    	Choice * chFormula = it->second;  // maximal partner choices


	    std::set<std::set<std::string> > choices = chFormula->get_all_choices();
		for(std::set<std::set<std::string> > ::iterator cit= choices.begin();  cit!= choices.end(); ++cit) {
			std::set<std::string> ch = * cit;
			// for each choice, e.g., x * y
			add_an_internal_choice(ch, node, & succ[node]);
		}
		if (chFormula->is_always_satisfied()) {
			add_extra_choices(node, & succ[node]);
		}


	    // remove all arcs from the list of destination node of sourceNode
    	// i.e., remove all outgoing arc with labels described in the removeArcs
		multimap<string, unsigned int> * arcMap = & succ[node];
		multimap<string, unsigned int>::iterator foundit;
    	for ( multimap<string, unsigned int>::iterator rit=removeArcs.begin(); rit!=removeArcs.end(); ++rit)
    	{
    		string label = rit->first;
			foundit = arcMap->find(label);
			if (foundit != arcMap->end()) {
			    if (debug == on) {
			 		cout << "      removing : " << label << ", " << rit->second << endl;
			    }
			 	arcMap->erase( arcMap->lower_bound(rit->first), arcMap->upper_bound(rit->first) );
			}
 		}
        removeArcs.clear();
     }

     if (debug == on) {
 		cout << "++++++++++++++++ after +++++++++++++++++++++" << endl;
 		dumpTest();
 	}

}

/* *************************************
 * write output as a service automaton *
 ***************************************/
void write_output_SA()
{
	bool initialState = false;

	  (*myOut) << "INTERFACE" << endl;
	  if(!inputs.empty()) {
	    (*myOut) << "  INPUT\n    " << *inputs.begin();
	    for(set<string>::iterator it = ++(inputs.begin()); it != inputs.end(); ++it) {
	      (*myOut) << ", " << *it;
	    }
	    (*myOut) << ";\n";
	  }

	  if(!outputs.empty()) {
	    (*myOut) << "  OUTPUT\n    " << *outputs.begin();
	    for(set<string>::iterator it = ++(outputs.begin()); it != outputs.end(); ++it) {
	      (*myOut) << ", " << *it;
	    }
	    (*myOut) << ";\n";
	  }

	  if(!synchronous.empty()) {
	    (*myOut) << "  SYNCHRONOUS\n    " << *synchronous.begin();
	    for(set<string>::iterator it = ++(synchronous.begin()); it != synchronous.end(); ++it) {
	      (*myOut) << ", " << *it;
	    }
	    (*myOut) << ";\n";
	  }

	  (*myOut) << "\nNODES\n";

	  for( map<unsigned int, multimap<string, unsigned int> >::iterator it=succ.begin(); it!=succ.end(); ++it)  {

//	    if(args_info.show_states_given)
//	    {
//	      (*myOut) << "  { ";
//	      for(unsigned int j = 0; j < ID2state[i->first].size(); ++j)
//	      {
//	        (*myOut) << ID2state[i->first][j] << " ";
//	      }
//	      (*myOut) << "}\n";
//	    }

	    (*myOut) << "  " << it->first; // << " : "; //it->second << "\n";

	    // read a node, write corresponding node
	    if ((*initialID) == (it->first)) {
	      (*myOut) << " : INITIAL";
	      initialState = true;
	    }


	    set<unsigned int>::iterator finalFound = finalIDs.find(it->first);
	    if (finalFound != finalIDs.end()) {
	    	if ((*initialID) == (it->first))
	    		(*myOut) << ", ";

	    	(*myOut) << " : FINAL";
	    }


	    (*myOut) << "\n";

	    for(multimap<string, unsigned int>::iterator sit = succ[it->first].begin(); sit != succ[it->first].end(); ++sit) {
	    	(*myOut) << "    " << sit->first << " -> " << sit->second << "\n";
	    }
	  }

	  (*myOut) << endl << std::flush;
}

/**************************
 * generate DNF structure *
 *************************/
void generate_DNF_structure() {

    for( map<unsigned int, Formula *>::iterator it=formulae.begin(); it!=formulae.end(); ++it) {

       unsigned int node = it->first;
       node_index = (node > node_index) ?  node :  node_index;

	   Formula * f = formulae[node];
	   Formula * dnf = f->dnf();
	   formulae[node] = dnf;

	   delete f;

   	   std::set<std::string> allow;
	   std::multimap<std::string, unsigned int> element = succ[node];
       for( std::multimap<std::string, unsigned int>::iterator sit=element.begin(); sit!=element.end(); ++sit) {
    	   string str = sit->first;
   		   allow.insert(sit->first);
       }


	   std::set<std::string> all = util::setUnion(inputs, outputs);
	   Choice * ch = new Choice( dnf, allow, liberal_flag,  cout, debug );

       switch (liberal_flag) {
		   case minimal : ch->minimize(cout);   break;
		   case maximal : ch->maximize(cout);  break;
		   default : break;
   	   }

   	   dnf_choices[node] = ch;
    }

}

void terminationHandler() {
    if (verbose == on) {
        message("runtime: %s%.2f sec%s.", _bold_, (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC, _c_);
        std::string call = std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }'";
        FILE* ps = popen(call.c_str(), "r");
        unsigned int memory;
        int res = fscanf(ps, "%u", &memory);
        assert(res != EOF);
        pclose(ps);
        message("memory consumption: %s%u KB. %s", _bold_, memory, _c_);

    }
}

void finalize()
{
   	if (verbose == on ) {
    		message("%d nodes in OG input file.", number_of_nodes);
    		message("%d output nodes [%d final nodes] are generated.", succ.size(), finalIDs.size());
     }
    /*
        if(initialID != NULL) ...

        if (finalIDs.empty())
        	if (verbose == on ) { message("there is no final state."); }
        else {
        	ostringstream oss;
        	string delim = "";
        	for (set<unsigned int>::iterator it=finalIDs.begin(); it!=finalIDs.end(); ++it) {
        		oss << delim << *it;
        		delim = ", ";
        	}
        	if (verbose == on ) { message("final states = [%s].", oss.str().c_str()); }
        }
    }
    */
    /// clear maps
    succ.clear();
    formulae.clear();
    inputs.clear();
    outputs.clear();
    synchronous.clear();
    presentLabels.clear();
    removeArcs.clear();
    finalIDs.clear();
    delete initialID;
    initialID = NULL;
    
    /// close input
    fclose(yyin);
  
    /// clean lexer memory
    yylex_destroy();
}

/*****************
 * main function *
 *****************/
int main(int argc, char** argv)
{

	// set the function to call on normal termination
    atexit(terminationHandler);

    // parse the command line parameters
    if (verbose == on) { message("evaluating command line parameters... "); }
    evaluate_parameters(argc, argv);

    // parse either from standard input or from a given file
    if (args_info.inputs_num == 0) {

    	// TODO : status("reading from stdin...");
    	 message("ERROR : an OG input file must be given");
 		 status("terminated with failure!");
   		 exit(EXIT_FAILURE);
    }

    // parse OG input file
    if (verbose == on) { message("parsing an OG input file... "); }
    parse_OG();

    // generate complete DNF structure
    if (verbose == on) { message("generating DNF structure..."); }
    generate_DNF_structure();

    if (debug == on) {
    	// dump all parsed variables to standard output
    	message("dumping all variables ... ");
    	//	dump();
    	dumpTest();
    }

    // construct a maximal partner
    if (verbose == on) { message("constructing a maximal partner... "); }
    construct_maximal_partner();

	// write output
    if (verbose == on) { message("writing an output service automaton..."); }
    write_output_SA();

    // summarizing information...
    if (verbose == on) { message("post-processing..."); }
    finalize();

    if (verbose == on) { message("successfully terminated."); }
    exit(EXIT_SUCCESS); // finished parsing
}

