// -*- C++ -*-

/*!
 * \file    problem.cc
 *
 * \brief   Class Problem
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/21
 *
 * \date    $Date: 2009-10-21 12:00:00 +0200 (Mi, 21. Okt 2009) $
 *
 * \version $Revision: -1 $
 */

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

#include "verbose.h"

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "partialsolution.h"
#include "pathfinder.h"
#include "reachalyzer.h"
#include "problem.h"
#include "lp_solve/lp_lib.h"

using std::cerr;
using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::set;
using std::stack;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::setw;
//using namespace pnapi;

extern pnapi::parser::owfn::Parser paowfn;
extern pnapi::parser::lola::Parser palola;
extern vector<Transition*> transitionorder;
extern vector<Place*> placeorder;
extern map<Transition*,int> revtorder;
extern map<Place*,int> revporder;

	/**************************************
	* Implementation of the class Problem *
	**************************************/

/** Standard constructor.
*/
Problem::Problem() : deinit(false),generalcover(false),type(REACHABLE),nettype(LOLA),pn(NULL) {}

/** Constructor. Cannot be used for coverability.
	@param net The name of the Petri net file.
	@param pntype The type of the Petri net format (LOLA/OWFN).
	@param initialmarking The initial marking (may differ from the file).
	@param requirement The final marking or the transition vector to realize.
	@param typ The problem type: REACHABILITY or REALIZABILITY.
*/
Problem::Problem(string net, int pntype, map<string,int>& initialmarking, map<string,int>& requirement, int typ) 
	: deinit(false),generalcover(false),type(typ),filename(net),initial(initialmarking),required(requirement),nettype(pntype),pn(NULL) {
	name = "";
	cover.clear();
}

/** Constructor for all variants of reachability including coverability.
	@param net The name of the Petri net file.
	@param pntype The type of the Petri net format (LOLA/OWFN).
	@param initialmarking The initial marking (may differ from the file).
	@param covermarking The final marking.
	@param coverplaces Whether token numbers on places must be covered, bounded, or reached exactly.
*/
Problem::Problem(string net, int pntype, map<string,int>& initialmarking, map<string,int>& covermarking, map<string,int>& coverplaces) 
	: deinit(false),generalcover(false),filename(net),initial(initialmarking),required(covermarking),cover(coverplaces),nettype(pntype),pn(NULL) {
	type = REACHABLE;
	name = "";
}

/** Destructor.
*/
Problem::~Problem() { clear(); }

/** Deinitialization of a problem.
*/
void Problem::clear() {
	name = "";
	deinit = false;
	pn = NULL;
	nettype = 0;
	filename = "";
	initial.clear();
	required.clear();
	cover.clear();
	type = 0;
}

/** Set the name of a problem.
	@param s The name.
*/
void Problem::setName(string s) { name = s; }

/** Set the goal of a problem.
	@param g The goal: REACHABILITY or REALIZABILITY.
*/
void Problem::setGoal(int g) { type = g; }

/** Set the filename for a problem.
	@param fn The filename of the Petri net file.
*/
void Problem::setFilename(string fn) { filename = fn; }

/** Get the filename for a problem.
	@return The filename of the Petri net file.
*/
string Problem::getFilename() { return filename; }

/** Set the type of Petri net.
	@param nt LOLA or OWFN.
*/
void Problem::setNetType(int nt) { nettype = nt; }

/** Set the initial marking for a problem.
	@param s The name of a place in the Petri net file.
	@param i The number of token initially on place s.
*/
void Problem::setInit(string s, int i) { initial[s]=i; }

/** Set the final marking or transition vector for a problem.
	@param s The name of a place or transition in the Petri net file.
	@param f The number of tokens finally on place s or the number of times the transition must fire.
*/
void Problem::setFinal(string s, int f) { required[s]=f; }

/** Set the mode for a place in the final marking.
	@param s The name of a place in the Petri net file.
	@param mode EQ (reach exactly), GE (cover), or LE (bound).
*/
void Problem::setCover(string s, int mode) { cover[s]=mode; }

/** Set the mode for all places unmentioned in the final marking.
	@param cov Whether those places should have >=0 token (true) or ==0 tokens (false)
*/
void Problem::setGeneralCover(bool cov) { generalcover = cov; }

/** Get the name of a problem.
	@return The name.
*/
string Problem::getName() { return name; }

/** Get the initial marking of a problem.
	@return The initial marking.
*/
Marking Problem::getInitialMarking() {
	if (pn==NULL) getPetriNet();
	map<const Place*,unsigned int> result;
	result.clear();
	map<string,int>::iterator it;
	for(it=initial.begin(); it!=initial.end(); ++it)
	{
		const Place* p(pn->findPlace(it->first));
		if (p) result[p]=(unsigned int)it->second;
	}
	return Marking(result,pn);
}

/** Get the final marking of a problem.
	@return The final marking.
*/
Marking Problem::getFinalMarking() {
	if (pn==NULL) getPetriNet();
	map<const Place*,unsigned int> result;
	result.clear();
	map<string,int>::iterator it;
	for(it=required.begin(); it!=required.end(); ++it)
	{
		Place* p(pn->findPlace(it->first));
		if (p) result[p]=(unsigned int)it->second;
	}
	return Marking(result,pn);
}

/** Get the modes of all places for the final marking of a problem.
	@return A map from places to modes (EQ,GE,LE).
*/
map<Place*,int> Problem::getCoverRequirement() {
	if (pn==NULL) getPetriNet();
	map<Place*,int> result;
	result.clear();
	map<string,int>::iterator it;
	for(it=cover.begin(); it!=cover.end(); ++it)
	{
		Place* p(pn->findPlace(it->first));
		if (p) result[p]=it->second; 
	}
	if (generalcover)
	{
		set<Place*> pset(pn->getPlaces());
		set<Place*>::iterator pit;
		for(pit=pset.begin(); pit!=pset.end(); ++pit)
			if (required.find((*pit)->getName())==required.end())
				result[*pit]=GE;
	}
	return result;
}

/** Get the transition vector to realize for this problem.
	@return The vector.
*/
map<Transition*,int> Problem::getVectorToRealize() {
	if (pn==NULL) getPetriNet();
	map<Transition*,int> result;
	result.clear();
	map<string,int>::iterator it;
	for(it=required.begin(); it!=required.end(); ++it)
	{
		Transition* t(pn->findTransition(it->first));
		if (t) result[t]=it->second;
	}
	return result;
}

/** Get the Petri net of a problem and calculate the global ordering of transitions and places.
	@return The Petri net.
*/
PetriNet* Problem::getPetriNet() {
	if (pn) return pn;

	// read from file only!
//    string invocation = "";

	// try to open file
	ifstream infile(filename.c_str(), ifstream::in);
	if (!infile.is_open()) {
	    cerr << "sara: error: could not read from file '" << filename << "'" << endl;
	    exit(EXIT_FAILURE);
	}

	// try to parse net
	try {
	    switch (nettype) {
	        case(OWFN): {
				paowfn.clean();
				pn = &(const_cast<PetriNet&>(paowfn.parse(infile)));
	            break;
	        }
	        case(LOLA): {
				palola.clean();
				pn = &(const_cast<PetriNet&>(palola.parse(infile)));
	            break;
	        }
	    }
	} catch (pnapi::io::InputError error) {
	    cerr << "sara: " << error << endl;
	    infile.close();
	    exit(EXIT_FAILURE);
	}

	infile.close();
	deinit = true;
	calcPTOrder();
	return pn;
}

/** Get the goal of a problem.
	@return REACHABILITY or REALIZABILITY.
*/
int Problem::getGoal() {
	return type;
}

/** Checks whether this problem needs the same Petri net as the parameter problem.
	If so, advances the Petri net to this problem without reloading.
	@param p A problem with a loaded Petri net.
*/
void Problem::checkForNetReference(Problem& p) {
	if (filename==p.filename) { pn = p.pn; p.deinit = false; deinit = true; }
}

/** Calculate the global ordering of transitions and places for this problem.
*/
void Problem::calcPTOrder() {
	set<Transition*> tset(pn->getTransitions());
	set<Place*> pset(pn->getPlaces());
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
	for(int i=0; i<5; ++i)
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
		for(tmit=ttmp.begin(),min=0; tmit!=ttmp.end(); ++tmit,++min)
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
					val -= tval[&((*ait)->getTransition())]*(*ait)->getWeight();
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
		for(pmit=ptmp.begin(),min=0; pmit!=ptmp.end(); ++pmit,++min)
		{
			pord[min]=pmit->second;
			for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
				pval[(*pit)] = min;
		}
		ptmp.clear();
	}

	// push the result into the global vectors
	transitionorder.clear();
	for(tmit=tord.begin(); tmit!=tord.end(); ++tmit)
	{
		for(tit=tmit->second.begin(); tit!=tmit->second.end(); ++tit)
			transitionorder.push_back(*tit);
	}
	placeorder.clear();
	for(pmit=pord.begin(); pmit!=pord.end(); ++pmit)
	{
		for(pit=pmit->second.begin(); pit!=pmit->second.end(); ++pit)
			placeorder.push_back(*pit);
	}
	revtorder.clear();
	for(unsigned int i=0; i<transitionorder.size(); ++i)
		revtorder[transitionorder[i]]=i;
	revporder.clear();
	for(unsigned int i=0; i<placeorder.size(); ++i)
		revporder[placeorder[i]]=i;
}

