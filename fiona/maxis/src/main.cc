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
using std::queue;
using std::ostream;

/// the command line parameters
gengetopt_args_info args_info;

/// lexer and parser
extern int yyparse();
extern int yylex_destroy();
extern FILE* yyin;

/// output stream
std::ostream* myOut = &cout;

/****************************
 * Variables used by parser *
 ****************************/
/// non deterministic successor list of recent OG
map<unsigned int, multimap<string, unsigned int> > succ;
/// annotations of recent OG
map<unsigned int, Formula *> formulae;
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
/// mapping from ID to state
//map<unsigned int, vector<unsigned int> > ID2state;

/// final states
set<unsigned int> finalIDs;

/// global temporary buffer for each state
multimap<string, unsigned int> removeArcs;

/// number of nodes/states
unsigned int number_of_nodes = 0;
unsigned int node_index = 0;

liberal liberal_flag = normal;

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
	if (args_info.verbose_flag) {
		for (int idx = 0; idx < argc; ++idx) {
			message("arg[%d] is %s ", idx, argv[idx]);
		}
	}

	if(args_info.liberal_given) {
		switch (args_info.liberal_arg) {
			case 0 :  liberal_flag = normal;
					  if(args_info.verbose_flag)
						  message("the liberal flag is not set");
					  break;
			case 1 :  liberal_flag = minimal;
					  if(args_info.verbose_flag)
						  message("the liberal flag is set to minimal");
					  break;
			case 2 :  message("the liberal flag is set to maximal");
/*					  message("sorry, the most liberal maximal partner is currently under construction!");
					  status("terminated with failure!");
					  exit(EXIT_FAILURE);
*/
					  liberal_flag = maximal;
					  if(args_info.verbose_flag)
						  message("the liberal flag is set to maximal");
					  break;
			default : liberal_flag = normal;
					  break;
		}
	}

	// if none of input OG files is given
	if(args_info.inputs_num == 0 ) {
		message("ERROR : an OG input file must be given");
		status("terminated with failure!");
		exit(EXIT_FAILURE);
	}

	// if none of input OG files is given
	if(args_info.inputs_num > 1 )  {
		message("ERROR : exactly one OG input file must be given");
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

    /// actual parsing
    yyparse();
}


void dump()
{
	// check parsed output
    cout << PACKAGE << " : succ " << endl;
    for( map<unsigned int, multimap<string, unsigned int> >::iterator it=succ.begin(); it!=succ.end(); ++it) {
	    cout << it->first << "::" << endl;
	    multimap<string, unsigned int> element = it->second;
        for( multimap<string, unsigned int>::iterator sit=element.begin(); sit!=element.end(); ++sit) {
 	       cout << it->first << ": " << sit->second << ", ";
        }
        cout << endl;
 	 }

    cout << PACKAGE << " : formula " << endl;
    for( map<unsigned int,Formula * >::iterator it=formulae.begin(); it!=formulae.end(); ++it) {
 	       cout << it->first << ": " << it->second << endl;
 	 }

    cout << PACKAGE << " : inputs " << endl;
    for (set<string>::iterator it=inputs.begin();  it!= inputs.end(); ++it) {
      cout << it->data() << ",";
    }
    cout << endl;

    cout << PACKAGE << " : outputs " << endl;
    for (set<string>::iterator it=outputs.begin();  it!= outputs.end(); ++it) {
      cout << it->data() << ",";
    }
    cout << endl;

    cout << PACKAGE << " : synchronous " << endl;
    for (set<string>::iterator it=synchronous.begin();  it!= synchronous.end(); ++it) {
      cout << it->data() << ",";
    }
    cout << endl;

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
    cout << PACKAGE << " : initialID " << (*initialID) << endl;

}

/********************************
 * convert each formulae to DNF *
 ********************************
void convertFormulaToDNF()
{
	for( map<unsigned int,Formula * >::iterator it=formulae.begin(); it!=formulae.end(); ++it)
	{
		Formula * f = it->second;
        Formula * formula = f->clone();
        Formula * formula_ = formula->dnf();
        delete f;
        delete formula;
        formulae[it->first] = formula_;
	 }
}
*/

/* ******************************************************************************
 * generic routine to add an internal tau choice                                *
 * for each formula,                                                            *
 *    add an internal tau choice of all outgoing arcs described by the formulas *
 ********************************************************************************/
void add_an_internal_choice(set<Formula *> childrenFormula,
		unsigned int sourceNode,  // TODO : warning, unused parameter
		multimap<string, unsigned int> * arcMap)
{
	node_index++;
	multimap<string, unsigned int> oldArcs;

	// there are possibly too many literals described in  arcMap
	// get only arc that describe by all literals in the formula
	for( set<Formula *>::iterator it=childrenFormula.begin(); it!=childrenFormula.end(); ++it) {
    	Formula * f = *it;
    	Literal * literal = dynamic_cast<Literal *> (f);
		if (literal != NULL) {

			if (literal->getString() == "final") {
				finalIDs.insert(node_index);
			}
			else {
				string label = literal->getString();
	 		    pair< multimap<string, unsigned int>::iterator, multimap<string, unsigned int>::iterator> range
						= arcMap->equal_range(label);
	 		    for (multimap<string, unsigned int>::iterator mit=range.first; mit!=range.second; ++mit) {
					oldArcs.insert(multimap<string, unsigned int>::value_type(mit->first, mit->second));
					removeArcs.insert(multimap<string, unsigned int>::value_type(mit->first, mit->second));
	 		    }
 		    }
		}
		// else it is not literal
	}

	// create a newNode;
    succ[node_index] = oldArcs;

	// add a TAU arc from a sourceNode to a newNode
   	arcMap->insert( multimap<string, unsigned int>::value_type("TAU", node_index) );
}

void add_literal_node(unsigned int sourceNode, Literal * literal)
{
	// read all outgoing arc label from a source node;
	multimap<string, unsigned int> arcMap = succ[sourceNode];

	//	if literal string is "true"
	// #TODO : then add all (least/most liberal) possible combinations according to liberal flag
	/* #TODO : case insensitive string comparison */
 	if (literal->operator==("true")) {
		for( multimap<string, unsigned int>::iterator it=arcMap.begin(); it!=arcMap.end(); ++it) {
			Literal aLiteral(it->first);
			set<Formula *> childrenFormula;
			childrenFormula.insert(& aLiteral);
			add_an_internal_choice(childrenFormula, sourceNode, & succ[sourceNode]);
		}
	} else {
		//	if literal string is not "false"
		if (!literal->operator==("false")) {
			set<Formula *> childrenFormula;
			childrenFormula.insert(dynamic_cast<Formula *> (literal));
			add_an_internal_choice(childrenFormula, sourceNode, & succ[sourceNode]);
		}
	}	// in case literal string is "false", do nothing
}

void add_conjunction_node(unsigned int sourceNode, Formula * formula)
{
	set<Formula *> elements = formula->getElements();
	set<string> ss;
	for(set<Formula *>::iterator it = elements.begin(); it != elements.end(); ++it) {
		Literal * l = dynamic_cast<Literal *>(*it);
		if (l != NULL)
			ss.insert(l->getString());
	}

	// do nothing if a conjunction contains "false"
	if (!formula->sat(ss))
		return;

	// add an transition
	add_an_internal_choice(formula->getElements(), sourceNode, & succ[sourceNode]);
}

void add_disjunction_node(unsigned int sourceNode, Formula * formula)
{
	// assume formula is dnf; a disjunction of either literal or conjunction
	set<Formula *> dnfElements = formula->getElements();
	for (set<Formula *>::iterator it=dnfElements.begin(); it!=dnfElements.end(); ++it)	{
		Literal * l = dynamic_cast<Literal *> (formula);
		if (l != NULL)
			add_literal_node(sourceNode, l);
		else
			add_conjunction_node(sourceNode, *it);
	}
}

/****************************************
 * construct a maximal partner from OGs *
 ****************************************/
void construct_maximal_partner()
{
	number_of_nodes = succ.size();

    for( map<unsigned int, Formula *>::iterator it=formulae.begin(); it!=formulae.end(); ++it)
    {
    	unsigned int node = it->first;

    	Formula * dnf = it->second;
	    set<Formula *> fwrapper = dnf->getElements();

		set<Formula *>::iterator it = fwrapper.begin();
		while ( it!=fwrapper.end())	{
	 		Literal * l = dynamic_cast<Literal *>(*it);
			if (l != NULL) {
				if (l->operator==("true")) {
					formulae[node] = static_cast<Formula *>(new Literal("true"));
					fwrapper = formulae[node]->getElements();
					break;
				}
			}
			it++;
		}

	    for( set<Formula *>::iterator fwit=fwrapper.begin(); fwit!=fwrapper.end(); ++fwit)
	    {
	    	// add internal tau choices
	    	Formula * formula = *fwit;
	    	Literal * l = dynamic_cast<Literal *> (formula);
	    	if (l != NULL)  {
	    		/* #TODO : case insensitive string comparison */
	    		if (l->getString().compare("false")!=0)
	    			add_literal_node(node, l);
	    	} else {
	    		// formula must be an operator
	    		if ( dynamic_cast<Conjunction *> (formula) != NULL)
	    			add_conjunction_node(node, formula);
	    		else
	    			add_disjunction_node(node, formula);
	    	}
	    }

        multimap<string, unsigned int> element = succ[node];

        // remove all arcs from the list of destination node of sourceNode
    	// i.e., remove all outgoing arc with labels described in the removeArcs
		multimap<string, unsigned int> * arcMap = & succ[node];
		multimap<string, unsigned int>::iterator foundit;
    	for ( multimap<string, unsigned int>::iterator rit=removeArcs.begin(); rit!=removeArcs.end(); ++rit)
    	{
    		string label = rit->first;
			foundit = arcMap->find(label);
			if (foundit != arcMap->end()) {
			    arcMap->erase( arcMap->lower_bound(rit->first), arcMap->upper_bound(rit->first) );
			}
 		}
        removeArcs.clear();
		element = succ[node];
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

	    (*myOut) << "  " << it->first << " : "; //it->second << "\n";

	    // read a node, write corresponding node
	    if ((*initialID) == (it->first)) {
	      (*myOut) << "INITIAL";
	      initialState = true;
	    }


	    set<unsigned int>::iterator finalFound = finalIDs.find(it->first);
	    if (finalFound != finalIDs.end()) {
	    	if ((*initialID) == (it->first))
	    		(*myOut) << ", ";

	    	(*myOut) << "FINAL";
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
 **************************/
void generate_DNF_structure() {
    for( map<unsigned int, Formula *>::iterator it=formulae.begin(); it!=formulae.end(); ++it) {

    	cout << "======================" << endl;

    	unsigned int node = it->first;
	   if (node > node_index)
		   node_index = node;

	   Formula * f = formulae[node];
	   Formula * dnf = f->dnf();
	   delete f;

	   formulae[node] = dnf;

	   if (args_info.verbose_flag) {
		   cout << PACKAGE << ": creating DNF structure..." << endl;
	   }

	   set<string> tmpset = util::setUnion(inputs, outputs);
	   DNF * aDNF = new DNF( formulae[node], tmpset, cout );

	   // construct a complete DNF, not yet implemented
	   // aDNF->maximize();

	   Disjunction * aFormula = aDNF->get_DNF();
   	   formulae[node] = static_cast<Formula *>(aFormula);

   	   switch (liberal_flag) {
		   case minimal :  if (args_info.verbose_flag) {
							   cout << PACKAGE << ": DNF = ";
							   aDNF->out(cout);
							   cout << PACKAGE << ": minimizing DNF structure..." << endl;
							}

						   aDNF->minimize(cout);
						   {
							   Disjunction * minimizedFormula = aDNF->get_DNF();
							   formulae[node] = static_cast<Formula *>(minimizedFormula);
						   }
						   break;

		   case maximal : if (args_info.verbose_flag) {
							   cout << PACKAGE << ": DNF = ";
							   aDNF->out(cout);
							   cout << PACKAGE << ": maximizing DNF structure..." << endl;
							}

						   aDNF->maximize(cout);
						   {
							   Disjunction * maximizedFormula = aDNF->get_DNF();
							   formulae[node] = static_cast<Formula *>(maximizedFormula);
						   }
						   break;
		   default : break;
   	   }

   	   if (args_info.verbose_flag) {
 		   cout << PACKAGE << ": calculated DNF = ";
 		   aDNF->out(cout);
   		   cout << PACKAGE << ": deleting DNF structure..." << endl;
   	   }

	   delete(aDNF);
   }
}

void finalize()
{
    if(initialID != NULL)
    {
    	message("initialID = [%d] ", (*initialID));
    	message("number of nodes before = %d ", number_of_nodes);
    	message("number of nodes after  = %d ", succ.size());
        if (finalIDs.empty())
			message("there is no final state.");
        else {
        	ostringstream oss;
        	string delim = "";
        	for (set<unsigned int>::iterator it=finalIDs.begin(); it!=finalIDs.end(); ++it) {
        		oss << delim << *it;
        		delim = ", ";
        	}
        	message("final states = [%s].", oss.str().c_str());
        }
    }
    
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
    // parse the command line parameters
    message("evaluating command line parameters... ");
    evaluate_parameters(argc, argv);

    // parse OG input file
    message("parsing an OG input file... ");
    parse_OG();

    // dump all parsed variables to standard output
    // message("dumping all variables to standard output... ");
	// dump();

    // generate a partial order DNF structure
    message("generating DNF structure...");
    generate_DNF_structure();

    // construct a maximal partner
    message("constructing a maximal partner... ");
    construct_maximal_partner();

    // write output
    message("writing an output service automaton...");
    write_output_SA();

    // summarizing information...
    message("post-processing...");
    finalize();

    message("successfully terminated.");
    exit(EXIT_SUCCESS); // finished parsing
}

