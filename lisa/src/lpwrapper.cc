#include <cstdio>
#include <cstdlib>
#include <set>
#include <map>
#include <algorithm>
#include <vector>
#include <typeinfo>

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
using pnapi::Marking;
using pnapi::Node;

extern vector<Transition*> transitionorder;
extern vector<Place*> placeorder;
extern map<Transition*,int> revtorder;
extern map<Place*,int> revporder;
extern set<pnapi::Arc*> arcs;


	/*************************************
	* Class LPWrapper method definitions *
	*************************************/

	/** Constructor for systems with a given number of variables (transitions).
		@param cols The number of transitions in the net (equals the numbers
			of columns in the linear system.
	*/
	LPWrapper::LPWrapper(unsigned int columns, PetriNet* net) : cols(columns), _net(net) {
		lp = make_lp(0,cols);
		if (!lp) abort(12,"error: could not create LP model");

	}

	/** Destructor
	*/
	LPWrapper::~LPWrapper() {
		delete_lp(lp);
	}

/** Checks if a given marking might be reachable. If there is no solution, the marking is definitely not reachable.
	@param m1 Initial marking.
	@param m2 Final marking.
	@param verbose If TRUE prints information on cout.
	@return The number of equations on success, -1 otherwise.
*/
int LPWrapper::checkReachability(Marking& m1, Marking& m2, bool verbose) {

	setupT();

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
		int mark = m2[*(placeorder[k])]-m1[*(placeorder[k])]; // calculate right hand side

		//initialize the rows
		if (!add_constraintex(lp,cols,mat,colpoint,EQ,mark)) return -1;
	}

	//allow only nonnegative solutions
	REAL r = 1;
	for(int y=1; y<=(int)(cols); ++y)
		if (!add_constraintex(lp,1,&r,&y,GE,0)) return -1;

	set_add_rowmode(lp,FALSE);	
	if (verbose) write_LP(lp,stdout);
	else set_verbose(lp,CRITICAL);
	basicrows = placeorder.size()+cols;

	int ret = solve(lp);

	if(ret != 0)
	 message("marking is not reachable");
	else{
         message("marking might be reachable");
         message("following is the parikh vector possibly realizing the marking");
	 get_variables(lp, mat);
         for(int j = 0; j < cols; j++)
           fprintf(stderr, "%s: %f\n", get_col_name(lp, j + 1), mat[j]);
	}

	return cleanup();
}

/** Creates equations for calculating t-invariants for lp_solve.
	@param verbose If TRUE prints information on cout.
	@return The number of equations on success, -1 otherwise.
*/
int LPWrapper::calcTInvariant(bool verbose) {

	setupT();
	
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

	if(solveLP(verbose) != 2){
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
	
	return cleanup();
}

/** Creates equations for calculating p-invariants for lp_solve.
	@param verbose If TRUE prints information on cout.
	@return The number of equations on success, -1 otherwise.
*/
int LPWrapper::calcPInvariant(bool verbose) {
	
	setupP();
	
	//create incidence matrix by adding rows to lp_solve
	for(unsigned int k=0; k<transitionorder.size(); ++k)
	{
		for(unsigned int y=0; y<cols; ++y) mat[y]=0;
		set<pnapi::Arc*>::iterator ait;
		set<pnapi::Arc*> arcs = transitionorder[k]->getPresetArcs(); 
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Place* t = &((*ait)->getPlace());
			mat[ppos[t]] += (*ait)->getWeight();
		}
		arcs = transitionorder[k]->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		{
			Place* t = &((*ait)->getPlace());
			mat[ppos[t]] -= (*ait)->getWeight();
		}
		int mark = 0; // right hand side equals zero as we want to calculate invariants
		//initialize the rows
		if (!add_constraintex(lp,cols,mat,colpoint,EQ,mark)) return -1;
	}

	if(solveLP(verbose) != 2){
	  message("Found p-invariant");

	  bool isNonNegative = true;
	  bool isPositive = true;
          get_variables(lp, mat);
          for(int j = 0; j < cols; j++){
            fprintf(stderr, "%s: %f\n", get_col_name(lp, j + 1), mat[j]);
	    //Check if p-invariant is non-negative
	    if(mat[j]<0){
	      isNonNegative = false;
              isPositive = false;
	    }
	    else{
	      //Check if p-invariant is positive, i.e. all components are greather than zero
	      if(mat[j]==0)
                isPositive = false;
	    }
	  }
          if(isNonNegative){
	    message("p-invariant is non-negative");	
	    message("petri net is covered by p-invariants");
	  }
	  if(isPositive)
            message("petri net is bounded");
	}
	else{
	  message("No p-invariant could be found");
	}
	
	return cleanup();
}

/** Creates equations for calculating a trap that is minimal, i.e. it 
    it contains the smallest set of places among all traps.
    Implementation follows "Verification of Safety Properties Using Integer
    Programming: Beyond the State Equation" by Javier Esparza and Stephan Melzer
	@param verbose If TRUE prints information on cout.
	@return The number of equations on success, -1 otherwise.
*/

int LPWrapper::calcTrap(bool verbose) {
	
	setupP();

	set<pnapi::Arc*>::iterator ait;
	for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
	{
		for(unsigned int y=0; y<cols; ++y) mat[y]=0;

		try{
	   	  Place& p = dynamic_cast<Place&>((*ait)->getSourceNode());
 		  Transition& t = dynamic_cast<Transition&>((*ait)->getTargetNode());
	 
		  for(unsigned int j=0; j<placeorder.size(); ++j){
		    if(placeorder[j]->getName() == p.getName() && t.getPostset().find((Node*) placeorder[j]) == t.getPostset().end()){
			mat[ppos[placeorder[j]]] = -1;
		    }
		    else{
		      if(placeorder[j]->getName() != p.getName() && t.getPostset().find((Node*) placeorder[j]) != t.getPostset().end()){
		        mat[ppos[placeorder[j]]] = 1;
		      }
		      else{
			mat[ppos[placeorder[j]]] = 0;
		      }
		    }
		  }
		  int mark = 0; // right hand side equals zero as we want to calculate invariants
		  //initialize the rows
		  if (!add_constraintex(lp,cols,mat,colpoint,GE,mark)) return -1;
		
   		}
		catch(std::bad_cast & b){

		}

	}
	
	if(solveLP(verbose) != 2){
	  message("Found trap containing the following places");

	  bool isNonNegative = true;
          get_variables(lp, mat);
	  fprintf(stderr, "Places:\n");
          for(int j = 0; j < cols; j++){
	    if(mat[j] > 0)
              fprintf(stderr, "%s\n", get_col_name(lp, j + 1));   
	  }
	}
	else
	  message("No trap could be found");

	return cleanup();
}

/** Creates equations for calculating a siphon that is minimal, i.e. it 
    it contains the smallest set of places among all siphons.
    Implementation follows "Verification of Safety Properties Using Integer
    Programming: Beyond the State Equation" by Javier Esparza and Stephan Melzer
	@param verbose If TRUE prints information on cout.
	@return The number of equations on success, -1 otherwise.
*/

int LPWrapper::calcSiphon(bool verbose) {
	setupP();
	
	set<pnapi::Arc*>::iterator ait;
	for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
	{
		for(unsigned int y=0; y<cols; ++y) mat[y]=0;

		try{
	   	  Place& p = dynamic_cast<Place&>((*ait)->getTargetNode());
 		  Transition& t = dynamic_cast<Transition&>((*ait)->getSourceNode());
	 
		  for(unsigned int j=0; j<placeorder.size(); ++j){
		    if(placeorder[j]->getName() == p.getName() && t.getPreset().find((Node*) placeorder[j]) == t.getPreset().end()){
			mat[ppos[placeorder[j]]] = -1;
		    }
		    else{
		      if(placeorder[j]->getName() != p.getName() && t.getPreset().find((Node*) placeorder[j]) != t.getPreset().end()){
		        mat[ppos[placeorder[j]]] = 1;
		      }
		      else{
			mat[ppos[placeorder[j]]] = 0;
		      }
		    }
		  }
		  int mark = 0; // right hand side equals zero as we want to calculate invariants
		  //initialize the rows
		  if (!add_constraintex(lp,cols,mat,colpoint,GE,mark)) return -1;
		
   		}
		catch(std::bad_cast & b){

		}

	}

	if(solveLP(verbose) != 2){
	  message("Found siphon containing the following places");

	  bool isNonNegative = true;
          get_variables(lp, mat);
	  fprintf(stderr, "Places:\n");
          for(int j = 0; j < cols; j++){
	    if(mat[j] > 0)
              fprintf(stderr, "%s\n", get_col_name(lp, j + 1));
	  }
	}
	else
	  message("No siphon could be found");

	return cleanup();
}

int LPWrapper::setupP(){
	ppos.clear(); // probably not necessary
	pvector.clear();

	// set the column variables in lp_solve according to the global place ordering
	colnr=0;
	for(unsigned int o=0; o<placeorder.size(); ++o,++colnr)
	{
		Place* t(placeorder[o]); // get the places according to the global ordering 
		set_col_name(lp,colnr+1,const_cast<char*>(t->getName().c_str()));
		set_int(lp,colnr+1,1); // declare all variables to be integer
		ppos[t] = colnr; // remember the ordering of the place
	}
	if (colnr!=(int)(cols)) cerr << "lisa: LPWrapper error: column number mismatch" << endl;

	//lp_solve objective: minimum firing sequence length
	colpoint = new int[cols];
	mat = new REAL[cols];
	for(unsigned int y=0; y<cols; y++)
	{
		mat[y]=1;
		colpoint[y]=y+1;
	}
	set_add_rowmode(lp,TRUE); // go to rowmode (faster)
	if (!set_obj_fnex(lp,cols,mat,colpoint)) return -1;
	
	set_minim(lp);

}

int LPWrapper::setupT(){
	tpos.clear(); // probably not necessary
	tvector.clear();

	// set the column variables in lp_solve according to the global transition ordering
	colnr=0;
	for(unsigned int o=0; o<transitionorder.size(); ++o,++colnr)
	{
		Transition* t(transitionorder[o]); // get the transitions according to the global ordering 
		set_col_name(lp,colnr+1,const_cast<char*>(t->getName().c_str()));
		set_int(lp,colnr+1,1); // declare all variables to be integer
		tpos[t] = colnr; // remember the ordering of the transitions
	}
	if (colnr!=(int)(cols)) cerr << "lisa: LPWrapper error: column number mismatch" << endl;

	//lp_solve objective: minimum firing sequence length
	colpoint = new int[cols];
	mat = new REAL[cols];
	for(unsigned int y=0; y<cols; y++)
	{
		mat[y]=1;
		colpoint[y]=y+1;
	}
	set_add_rowmode(lp,TRUE); // go to rowmode (faster)
	if (!set_obj_fnex(lp,cols,mat,colpoint)) return -1;
	
	set_minim(lp);

}

int LPWrapper::cleanup(){

	delete[] colpoint;
	delete[] mat;
	return (int)(basicrows);

}

int LPWrapper::solveLP(bool verbose){
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

	return solve(lp);

}

/** Calculate the global ordering of transitions and places for this problem.
	@author Harro Wimmel
	@return If the ordering is deterministic.

*/
bool LPWrapper::calcPTOrder() {
	set<Transition*> tset(_net->getTransitions());
	set<Place*> pset(_net->getPlaces());
	set<Transition*>::iterator tit;
	set<Place*>::iterator pit;
	map<Transition*,int> tval;
	map<Place*,int> pval;
	map<int,set<Transition*> > tord,ttmp;
	map<int,set<Place*> > pord,ptmp;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
		tval[(*tit)] = 1;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		pval[(*pit)] = 1;
	tord[0] = tset;
	pord[0] = pset;
	map<int,set<pnapi::Transition*> >::iterator tmit;
	map<int,set<pnapi::Place*> >::iterator pmit;
	set<pnapi::Arc*>::iterator ait;
	int min;
	for(int i=0; i<6; ++i) // Probably five rounds are enough to make the order deterministic, but we can't be sure!
	{
		min = 0;
		for(tmit=tord.begin(); tmit!=tord.end(); ++tmit)
		{
			int minval = 0;
			for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
			{
				int val = 0;
				for(ait=(*tit)->getPresetArcs().begin(); ait!=(*tit)->getPresetArcs().end(); ++ait)
					val += 2*pval[&((*ait)->getPlace())]*(*ait)->getWeight();
				for(ait=(*tit)->getPostsetArcs().begin(); ait!=(*tit)->getPostsetArcs().end(); ++ait)
					val -= pval[&((*ait)->getPlace())]*(*ait)->getWeight();
				tval[(*tit)] = val;
				if (val<minval) minval=val;
			}
			for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
			{
				tval[(*tit)] -= minval;
				ttmp[min+tval[(*tit)]].insert(*tit);
			}
			min = ttmp.rbegin()->first + 1;	
		}
		tord.clear();
		for(tmit=ttmp.begin(),min=1; tmit!=ttmp.end(); ++tmit,++min)
		{
			tord[min]=tmit->second;
			for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
				tval[(*tit)]=min;
		}
		ttmp.clear();
		min = 0;
		for(pmit=pord.begin(); pmit!=pord.end(); ++pmit)
		{
			int minval = 0;
			for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
			{
				int val = 0;
				for(ait=(*pit)->getPresetArcs().begin(); ait!=(*pit)->getPresetArcs().end(); ++ait)
					val += tval[&((*ait)->getTransition())]*(*ait)->getWeight();
				for(ait=(*pit)->getPostsetArcs().begin(); ait!=(*pit)->getPostsetArcs().end(); ++ait)
					val -= 2*tval[&((*ait)->getTransition())]*(*ait)->getWeight();
				pval[(*pit)] = val;
				if (val<minval) minval=val;
			}
			for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
			{
				pval[(*pit)] -= minval;
				ptmp[min+pval[(*pit)]].insert(*pit);
			}
			min = ptmp.rbegin()->first + 1;	
		}
		pord.clear();
		for(pmit=ptmp.begin(),min=1; pmit!=ptmp.end(); ++pmit,++min)
		{
			pord[min]=pmit->second;
			for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
				pval[(*pit)] = min;
		}
		ptmp.clear();
	}

	// push the result into the global vectors
	bool deterministic(true);
	transitionorder.clear();
	for(tmit=tord.begin(); tmit!=tord.end(); ++tmit)
	{
		if (tmit->second.size()>1) deterministic=false;
		for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
			transitionorder.push_back(*tit);
	}
	placeorder.clear();
	for(pmit=pord.begin(); pmit!=pord.end(); ++pmit)
	{
		if (pmit->second.size()>1) deterministic=false;
		for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
			placeorder.push_back(*pit); 
	}
	revtorder.clear();
	for(unsigned int i=0; i<transitionorder.size(); ++i)
		revtorder[transitionorder[i]]=i;
	revporder.clear();
	for(unsigned int i=0; i<placeorder.size(); ++i)
		revporder[placeorder[i]]=i;

	arcs = _net->getArcs();
	
	return deterministic;
}


