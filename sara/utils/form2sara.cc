#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>

#include "cmdline-form2sara.h"
#include "verbose.h"

#include "BFNode.h"

using std::cerr;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::map;
using std::set;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::setw;

/// the command line parameters
gengetopt_args_info args_info;

extern FILE *form2sara_in;
extern int form2sara_parse();
extern vector<BFNode*> roots;
extern vector<bool> quant;
extern vector<string> formid;

/*! Evaluate the command line parameters.
	\param argc Number of arguments.
	\param argv Argument list.
*/
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params *params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser (argc, argv, &args_info) != 0) {
        fprintf(stderr, "       see 'form2sara --help' for more information\n");
        exit(EXIT_FAILURE);
    }

    free(params);
}


/*! Main method of Sara.
	\param argc Number of arguments.
	\param argv Argument list.
*/
int main(int argc, char** argv) {

	// evaluate command line
    evaluateParameters(argc, argv);


/******************************
* CONSTRUCT PROBLEM INSTANCES *
******************************/

	// try to open file to read problem from
	string formfile = args_info.formula_arg;
	if (formfile=="stdin") form2sara_in = stdin;
	else form2sara_in = fopen(formfile.c_str(),"r");
	if (!form2sara_in) abort(32, "could not read formula file");
	form2sara_parse();
	if (form2sara_in!=stdin) fclose(form2sara_in);

	for(unsigned int i=0; i<roots.size(); ++i)
	{
		if (!quant[i])
		{
			BFNode* top = new BFNode();
			top->setInner(BFNode::BFNOT);
			top->addSon(roots[i]);
			roots[i] = top;
		}
		roots[i]->toDNF(false);
/*
		// if the root is a NOT it is preserved, everything else is converted to DNF
		if (roots[i]->isLeaf()==BFNode::BFFORM && roots[i]->getOp()==BFNode::BFNOT) 
			(*(roots[i]->getSons().begin()))->toDNF(false);
		else roots[i]->toDNF(false);
*/
/* output: the formula tree/graph
		cout << "FORMULA " << formid[i] << " (" << (quant[i]?"reachable":"invariant") << ")" << endl;
		set<BFNode*> go;
		go.insert(roots[i]);
		while(!go.empty())
		{
			BFNode* next(*(go.begin()));
			cout << (int)(next) << ": ";
			int comp(next->isLeaf());
			int op(next->getOp());
			string place(next->getPlace());
			set<BFNode*> succ(next->getSons());
			set<BFNode*>::iterator sit;
			switch(comp) {
				case BFNode::BFFORM: 	switch(op) {
							case BFNode::BFAND: cout << "AND "; break;
							case BFNode::BFOR: cout << "OR "; break;
							case BFNode::BFNOT: cout << "NOT "; break;
						}
						for(sit=succ.begin(); sit!=succ.end(); ++sit)
						{
							cout << (int)(*sit) << " ";
							go.insert(*sit);
						}
						cout << endl;
						break;
				case BFNode::BFLE:	cout << place << " <= " << op << endl; break;
				case BFNode::BFGE:	cout << place << " >= " << op << endl; break;
				case BFNode::BFEQ:	cout << place << " == " << op << endl; break;
				case BFNode::BFNE:	cout << place << " != " << op << endl; break;
				default:	cout << "error" << endl;
			}
			go.erase(next);
		}
		cout << endl;
*/
		BFNode* top(roots[i]);
		if (top->isLeaf()==BFNode::BFFORM && top->getOp()==BFNode::BFNOT) 
			top=*(top->getSons().begin());
		set<BFNode*> orloop;
		if (top->isLeaf()==BFNode::BFFORM && top->getOp()==BFNode::BFOR) orloop=top->getSons();
		else orloop.insert(top);
		set<BFNode*>::iterator nit;
		for(nit=orloop.begin(); nit!=orloop.end(); ++nit)
		{
			cout << "PROBLEM " << formid[i] << ":" << endl << "\tGOAL ";
//			if (!quant[i] && orloop.size()>1) cout << "DUMMY;" << endl;
//			else 
			cout << "REACHABILITY;" << endl;
			cout << "\tFILE " << args_info.net_arg << " TYPE ";
			if (args_info.owfn_given) cout << "OWFN;" << endl;
			else if (args_info.lola_given) cout << "LOLA;" << endl;
			else cout << "PNML;" << endl;
			cout << "\tFINAL COVER ";
			set<BFNode*>::iterator ait;
			set<BFNode*> aset((*nit)->getSons());
			if (aset.empty()) aset.insert(*nit);
			bool comma(false);
			for(ait=aset.begin(); ait!=aset.end(); ++ait,comma=true)
			{
				if ((*ait)->isLeaf()<=BFNode::BFFORM) { cout << (comma?",":"") << "error" << endl; continue; }
				cout << (comma?",":"") << (*ait)->getPlace();
				switch ((*ait)->isLeaf()) {
					case BFNode::BFLE: cout << "<"; break;
					case BFNode::BFEQ: cout << ":"; break;
					case BFNode::BFGE: cout << ">"; break;
					default: cout << "error";
				}
				cout << (*ait)->getOp();
			}
			cout << ";" << endl;
			cout << "\tRESULT" << (quant[i]?" OR ":" ");
			if (roots[i]->isLeaf()==BFNode::BFFORM && roots[i]->getOp()==BFNode::BFNOT) 
			{ if (quant[i])	cout << "NEGATE "; }
			else if (!quant[i]) cout << "NEGATE ";
			cout << formid[i] << ";" << endl << endl;
//			if (!quant[i] && orloop.size()>1) break;
		}

		// delete the formula
		set<BFNode*> all;
		roots[i]->allNodes(all);
		for(set<BFNode*>::iterator dit=all.begin(); dit!=all.end(); ++dit)
			delete (*dit);
	}
	return EXIT_SUCCESS;
}

