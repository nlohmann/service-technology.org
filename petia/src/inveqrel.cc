// -*- C++ -*-

/*!
 * \file    inveqrel.cc
 *
 * \brief   Class InvEqRel
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2011/07/13
 *
 * \date    $Date: 2011-07-13 14:47:01 +0200 (Mi, 13. Jul 2011) $
 *
 * \version $Revision: 1.0 $
 */

#include <vector>
#include <map>
#include <set>

#include "pnapi/pnapi.h"
#include "inveqrel.h"

using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Transition;
using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::endl;

	/***********************************
	* Implementation of class InvEqRel *
	***********************************/

/** Constructor for a given Petri net.
	Yields a structure to compute the equivalence classes.
	@param pn The Petri net for which the equivalence classes of places are to be determined.
	@param opt Optimize for less calls to lp_solve, induces some overhead.
*/
InvEqRel::InvEqRel(PetriNet& pn, bool opt) : start(0),net(pn),prejoin(0) {
	set<Place*> pset(pn.getPlaces());
	// the finer relation `above' separates non-equivalent places by putting them into different sets
	above.push_back(pset);
	set<Place*> empty;
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
	{
		// `toclass' point each places to the correct set in `above'
		toclass[*pit]=0;
		// the coarser relation `below' keeps equivalent places in a single set
		below[*pit]=empty;
		// a union-find algorithm is used to obtain the class where a place is in `below'
		unionfind.push_back(-1);
		pmap[*pit]=pvec.size();
		pvec.push_back(*pit);
	}
	if (opt) initPriorityChecks();
}

/** Destructor.
*/
InvEqRel::~InvEqRel() { }

/** Makes two places equivalent.
	@param p1 The first place.
	@param p2 The second place.
	@return If the operation was successful.
*/
bool InvEqRel::join(Place* p1, Place* p2) {
	// get the class of p1 and check whether it is an equivalence class at all
	p1 = findClass(p1);
	if (below.find(p1)==below.end()) return false;
	// same for p2, and check if it's in the same class as p1 already
	p2 = findClass(p2);
	if (p1==p2) return false;
	if (below.find(p2)==below.end()) return false;
	// get the position of p2 in the finer relation `above'
	unsigned int point(toclass[p2]);
	// and compare it to that of p1 -- inequality means p1 and p2 cannot be equivalent
	if (toclass[p1]!=point) return false;
	// add the class of p2 in the coarser relation `below' to that of p1
	set<Place*> pset(below[p2]);
	set<Place*>::iterator pit;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		below[p1].insert(*pit);
	// and add p2 itself to that class
	below[p1].insert(p2);
	// delete p2 and its class from `below'
	below.erase(p2);
	// and put p2 into the union-find-tree
	unionfind[pmap[p2]]=pmap[p1];
	// delete p2 from the finer relation -- we do not keep equivalent entries here
	above[point].erase(p2);
	toclass.erase(p2);
	return true;
}

/** Makes two places equivalent in the pre-lp_solve phase. Calls to this function are counted. 
	@param p1 The first place.
	@param p2 The second place.
	@return If the operation was successful.
*/
bool InvEqRel::pjoin(Place* p1, Place* p2) {
	if (join(p1,p2)) { ++prejoin; return true; }
	return false;
}

/** Detects from a semipositive place invariant which places can never be equivalent.
	@param inv The semipositive place invariant.
*/
void InvEqRel::split(map<Place*,int> inv) {
	// we split up each class in the finer `above' relation into two subsets:
	// one containing all the places with zero entries in inv, the other
	// containing those with non-zero entries.
	set<Place*>::iterator pit;
	unsigned int end(above.size());
	// sets before `start' are singletons or empty, we don't need to check there
	// sets after `end' are those just created -- they already have been split
	for(unsigned int i=start; i<end; ++i)
	{
		// don't split if there is nothing to split
		if (above[i].size()<2) continue;
		set<Place*> pset;
		// get all places with non-zero entries in inv
		for(pit=above[i].begin(); pit!=above[i].end(); ++pit)
			if (inv[*pit]!=0) pset.insert(*pit);
		// if all or no places have to be split away, there is nothing to do
		if (pset.size()==above[i].size() || pset.empty()) continue;
		// remove the collected places from the active set
		for(pit=pset.begin(); pit!=pset.end(); ++pit) 
		{
			above[i].erase(*pit);
			// and also adapt the pointers to the new set (to be created) 
			toclass[*pit]=above.size();
		}
		// add the new set to the end of the vector
		above.push_back(pset);
	}
}

/** Obtain a vector containing the equivalence relation as far as it has been computed.
	@param approx This parameter decides if we get the finer (false) or the coarser 
		(true) version of the relation.
		If the computation is completed, both will coincide.
	@return A vector of equivalence classes of places.
*/
vector<set<Place*> > InvEqRel::getClasses(bool approx) {
	vector<set<Place*> > res;
	// in case we want the coarser relation ... 
	if (approx)
	{
		// we just pick up the classes in `below' of known-to-be-equivalent places
		map<Place*,set<Place*> >::iterator mit;
		for(mit=below.begin(); mit!=below.end(); ++mit)
		{
			res.push_back(mit->second);
			// we need to add the representative of the class, since
			// it isn't contained in the class itself
			res[res.size()-1].insert(mit->first);
		}
		return res;
	}
	// in case we want the finer relation, we go through `above'
	for(unsigned int i=0; i<above.size(); ++i)
	{
		// we check for empty sets (though they won't occur if the programming is correct)
		if (above[i].size()==0) continue;
		set<Place*> pset;
		set<Place*>::iterator pit;
		// we go through each set of places, but note that there are no
		// known-to-be-equivalent places in these sets
		for(pit=above[i].begin(); pit!=above[i].end(); ++pit)
		{
			// therefore we do not only need the places itself
			pset.insert(*pit);
			set<Place*>::iterator pit2;
			// but also all places known to be equivalent, as given by `below'
			for(pit2=below[*pit].begin(); pit2!=below[*pit].end(); ++pit2)
				pset.insert(*pit2);
		}
		// we picked up a set of places that are pairwise either equivalent
		// or undecided
		res.push_back(pset);
	}
	return res;
}

/** Obtain two places for which equivalence is undecided so far.
	@param p1 Empty pointer, to be filled with the first place.
	@param p2 Empty pointer, to be filled with the second place.
	@return If two places could be obtained. (Otherwise the equivalence relation has been fully computed.)
*/
bool InvEqRel::getUndecided(Place*& p1, Place*& p2) { 
	if (findPriorityCheck(p1,p2)) return true;
	// increment that start pointer to the finer relation `above' to the first interesting class
	while (start<above.size() && above[start].size()<2) ++start;
	// if there is none, we are done
	if (start>=above.size()) return false;
	// otherwise, get the first two places in this class
	set<Place*>::iterator pit(above[start].begin());
	p1 = *pit;
	++pit;
	p2 = *pit;
	return true;
}

/** Compute some simple equivalences that can be found by looking at the local structure
	of the net, without using lp_solve.
*/
void InvEqRel::simpleEquivalences() {
	// we check for every transition ...
	set<Transition*> tset(net.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		set<pnapi::Arc*> arcs1((*tit)->getPresetArcs());
		set<pnapi::Arc*> arcs2((*tit)->getPostsetArcs());
		set<pnapi::Arc*>::iterator ait;
		// if it has an empty preset
		if (arcs1.size()==0)
		{
			// the places in the postset have weight zero in every invariant
			ait = arcs2.begin();
			// so, select a place from the postset as class representative
			Place* p(&((*ait)->getPlace()));
			// and put all postset places into that class, as they are eqivalent
			for(++ait; ait!=arcs2.end(); ++ait)
				pjoin(p,&((*ait)->getPlace()));
		} else if (arcs2.size()==0) {
			// if it has an empty postset, do the same for the preset
			ait = arcs1.begin();
			Place* p(&((*ait)->getPlace()));
			for(++ait; ait!=arcs1.end(); ++ait)
				pjoin(p,&((*ait)->getPlace()));
		} else if (arcs1.size()==1 && arcs2.size()==1) {
			// if the transition has singleton pre- and postsets,
			// the two places occur in all invariant with the same weight,
			// so make them equivalent
			Place* p1(&((*(arcs1.begin()))->getPlace()));
			Place* p2(&((*(arcs2.begin()))->getPlace()));
			pjoin(p1,p2);
		}
	}
}

/** Find the representative of the class containing a given place, in the coarser relation `below'.
	@param p The place for which the representative is sought.
	@return The represetative (or NULL in case of an error).
*/
Place* InvEqRel::findClass(Place* p) {
	int j(findClassNum(p));
	return (j<0 ? NULL : pvec[j]);
}

/** Compute the ID of the class containing a given place, in the coarser relation `below'.
	@param p The place for which the representative is sought.
	@return The number appointed to the class.
*/
int InvEqRel::findClassNum(Place* p) {
	map<Place*,int>::iterator hit(pmap.find(p));
	if (hit==pmap.end()) return -1;
	int j(hit->second);
	while (unionfind[j]>=0) j=unionfind[j];
	return j;
}

/** Get the number of equivalences found prior to using lp_solve.
	@return The number of equivalences found.
*/
unsigned int InvEqRel::preJoinsDone() const { return prejoin; }

/** Initialize the list of priority checks. These checks may reduce
	the number of calls to lp_solve.
*/
void InvEqRel::initPriorityChecks() {
	set<Transition*> tset(net.getTransitions());
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		set<pnapi::Arc*> prearcs((*tit)->getPresetArcs());
		set<pnapi::Arc*> postarcs((*tit)->getPostsetArcs());
		bool which(prearcs.size()==1);
		if ((which) ^ (postarcs.size()!=1)) continue;
		set<Place*> pset;
		set<pnapi::Arc*>::iterator ait;
		for(ait=prearcs.begin(); ait!=prearcs.end(); ++ait)
			if (which) ppplace.push_back(&((*ait)->getPlace()));
			else pset.insert(&((*ait)->getPlace()));
		for(ait=postarcs.begin(); ait!=postarcs.end(); ++ait)
			if (!which) ppplace.push_back(&((*ait)->getPlace()));
			else pset.insert(&((*ait)->getPlace()));
		ppcheck.push_back(pset);
	}
}

/** If there are priority checks to be done, get one of them.
	@param p1 The first place of the pair to be checked. 
	@param p2 The second place of the pair to be checked.
	@return If there are checks to be done. If not, p1 and p2 remain unchanged.
*/
bool InvEqRel::findPriorityCheck(Place*& p1, Place*& p2) {
	while (!ppcheck.empty())
	{
		set<Place*> pset(ppcheck.back());
		set<Place*>::iterator pit(pset.begin());
		Place* eqclass1(findClass(*pit));
		Place* eqclass2;
		unsigned int clss(toclass[eqclass1]);
		bool destroy(false);
		for(++pit; pit!=pset.end(); ++pit)
		{
			eqclass2 = findClass(*pit);
			if (clss!=toclass[eqclass2]) { destroy=true; break; }
			if (eqclass1!=eqclass2) break; 
			ppcheck.back().erase(*pit);
		}
		if (ppcheck.back().size()<2 || destroy)
		{
			if (!destroy) pjoin(eqclass1,ppplace.back());
			ppcheck.pop_back();
			ppplace.pop_back();
			continue;
		}
		p1 = eqclass1;
		p2 = eqclass2;
		return true;
	}
	return false;
}

/*
bool InvEqRel::areEqual(Place* p1, Place* p2) {
	map<Place*,int>::iterator pit1(pmap.find(p1));
	if (pit1==pmap.end()) return false;
	map<Place*,int>::iterator pit2(pmap.find(p2));
	if (pit2==pmap.end()) return false;
	int j1(pit1->second);
	while (unionfind[j1]>=0) j1=unionfind[j1];
	int j2(pit2->second);
	while (unionfind[j2]>=0) j2=unionfind[j2];
	return (j1==j2);
}


void InvEqRel::setupInheritance(set<Transition*> tset) {
	set<Transition*>::iterator tit;
	for(tit=tset.begin(); tit!=tset.end(); ++tit)
	{
		if (tdone.find(*tit)!=tdone.end()) continue;
		set<pnapi::Arc*> arcs1((*tit)->getPresetArcs());
		set<pnapi::Arc*> arcs2((*tit)->getPostsetArcs());
		set<pnapi::Arc*>::iterator ait;
		int myclass(-1);
		for(ait=arcs1.begin(); ait!=arcs1.end(); ++ait)
		{
			int j(findClassNum(&((*ait)->getPlace())));
			if (myclass==-1) myclass=j;
			else if (myclass!=j) myclass=-2;		
		}
		if (myclass>=0)
		{
cerr << "Inheritance found for " << (*tit)->getName() << endl;
			tdone[*tit]=pvec[myclass];
			for(ait=arcs2.begin(); ait!=arcs2.end(); ++ait)
				toopt[*tit].insert(&((*ait)->getPlace()));
			continue;
		}
		myclass=-1;
		for(ait=arcs2.begin(); ait!=arcs2.end(); ++ait)
		{
			int j(findClassNum(&((*ait)->getPlace())));
			if (myclass==-1) myclass=j;
			else if (myclass!=j) myclass=-2;		
		}
		if (myclass>=0)
		{
cerr << "Inheritance found for " << (*tit)->getName() << endl;
			tdone[*tit]=pvec[myclass];
			for(ait=arcs1.begin(); ait!=arcs1.end(); ++ait)
				toopt[*tit].insert(&((*ait)->getPlace()));
		}
	}
}

void InvEqRel::adaptInheritance() {
	map<Transition*,set<Place*> >::iterator mit;
	for(mit=toopt.begin(); mit!=toopt.end(); ++mit)
	{
		Place* rep(tdone[mit->first]);
cerr << "For " << mit->first->getName() << " from " << rep->getName() << " to ";
		set<Place*> todelete;
		set<Place*>::iterator pit;
		for(pit=mit->second.begin(); pit!=mit->second.end(); ++pit)
		{
cerr << (*pit)->getName() << " ";
			if (toclass[rep]!=toclass[*pit]) todelete.insert(*pit);
			set<Place*>::iterator pit2(pit);
			while (++pit2!=mit->second.end())
				if (areEqual(*pit,*pit2)) 
				{ todelete.insert(*pit); break; }
		}
cerr << "(removed: ";
		for(pit=todelete.begin(); pit!=todelete.end(); ++pit)
		{
			mit->second.erase(*pit);
cerr << (*pit)->getName() << " ";
		}
cerr << ")";
		if (mit->second.size()==1) 
		{
			join(rep,*(mit->second.begin()));
cerr << ", join done";
		}
cerr << endl;
	}
}

void InvEqRel::addtoInheritance(Place* p) {
	if (!p) return;
	set<pnapi::Arc*> arcs(p->getPresetArcs());
	set<pnapi::Arc*>::iterator ait;
	set<Transition*> tset;
	for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		tset.insert(&((*ait)->getTransition()));
	arcs = p->getPostsetArcs();
	for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
		tset.insert(&((*ait)->getTransition()));
	setupInheritance(tset);
}
*/
