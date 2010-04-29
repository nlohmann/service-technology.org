#include <cstdio>
#include <cstdlib>
#include <set>
#include <map>
#include <algorithm>
#include <vector>

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "lp_solve/lp_lib.h"
#include "verbose.h"

#ifndef LPWRAPPER_H
#include "lpwrapper.h"
#endif

using std::cerr;
using std::cout;
using std::endl;
using std::set;
using std::map;
using std::vector;
using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
//using pnapi::Arc;
using pnapi::Marking;

extern vector<Transition*> transitionorder;
extern vector<Place*> placeorder;
extern map<Transition*,int> revtorder;


	/*************************************
	* Class LPWrapper method definitions *
	*************************************/

	/** Constructor for systems with a given number of variables (transitions).
		@param cols The number of transitions in the net (equals the numbers
			of columns in the linear system.
	*/
	LPWrapper::LPWrapper(unsigned int columns) : cols(columns) {
		lp = make_lp(0,cols);
		if (!lp) abort(12,"error: could not create LP model");

	}

	/** Destructor
	*/
	LPWrapper::~LPWrapper() {
		delete_lp(lp);
	}

/** Creates the marking equation for lp_solve.
	@param verbose If TRUE prints information on cout.
	@return The number of equations on success, -1 otherwise.
*/
int LPWrapper::calcTInvariant(bool verbose) {
	tpos.clear(); // probably not necessary
	tvector.clear();

	// set the column variables in lp_solve according to the global transition ordering
	int colnr=0;
	for(unsigned int o=0; o<transitionorder.size(); ++o,++colnr)
	{
		Transition* t(transitionorder[o]); // get the transitions according to the global ordering 
		set_col_name(lp,colnr+1,const_cast<char*>(t->getName().c_str()));
		set_int(lp,colnr+1,1); // declare all variables to be integer
		tpos[t] = colnr; // remember the ordering of the transitions
	}
	if (colnr!=(int)(cols)) cerr << "lisa: LPWrapper error: column number mismatch" << endl;

	//lp_solve objective: minimum firing sequence length
	int *colpoint = new int[cols];
	REAL *mat = new REAL[cols];
	for(unsigned int y=0; y<cols; y++)
	{
		mat[y]=1;
		colpoint[y]=y+1;
	}
	set_add_rowmode(lp,TRUE); // go to rowmode (faster)
	if (!set_obj_fnex(lp,cols,mat,colpoint)) return -1;
	
	set_minim(lp);
	
	//create incidence matrix by adding rows to lp_solve
	for(unsigned int k=0; k<placeorder.size(); ++k)
	{
		for(unsigned int y=0; y<cols; ++y) mat[y]=0;
		set<pnapi::Arc*>::iterator ait;
		set<pnapi::Arc*> arcs = placeorder[k]->getPresetArcs(); 
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Transition* t = &((*ait)->getTransition());
			mat[tpos[t]] += (*ait)->getWeight();
		}
		arcs = placeorder[k]->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Transition* t = &((*ait)->getTransition());
			mat[tpos[t]] -= (*ait)->getWeight();
		}
		int mark = 0; // right hand side equals zero as we want to calculate invariants
		//initialize the rows
		if (!add_constraintex(lp,cols,mat,colpoint,EQ,mark)) return -1;
	}

	//allow only nonnegative solutions
	REAL r = 1;

	for(int y=1; y<=(int)(cols); ++y)
		if (!add_constraintex(lp,1,&r,&y,GE,0)) return -1;

	//exclude trivial solution	for(unsigned int y=0; y<cols; y++){
		mat[y]=1;
		colpoint[y]=y+1;
	}
	if (!add_constraintex(lp,cols,mat,colpoint,GE,1)) return -1;
	

	set_add_rowmode(lp,FALSE);	
	if (verbose) write_LP(lp,stdout);
	else set_verbose(lp,CRITICAL);
	basicrows = placeorder.size()+cols;

	int ret = solve(lp);

	if(ret != 2){
	  message("Found t-invariant");

	  bool isNonNegative = true;
          get_variables(lp, mat);
          for(int j = 0; j < cols; j++){
            fprintf(stderr, "%s: %f\n", get_col_name(lp, j + 1), mat[j]);
	    //Check if t-invariant is non-negative
	    if(mat[j]<0)
	      isNonNegative = false;
	  }
          if(isNonNegative)
	    message("t-invariant is non-negative");	
	    message("petri net is covered by t-invariants");
	}
	else{
	  message("No t-invariant could be found");
	}
	
	delete[] colpoint;
	delete[] mat;
	return (int)(basicrows);
}

