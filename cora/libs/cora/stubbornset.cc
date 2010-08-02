// -*- C++ -*-

/*!
 * \file    stubbornset.cc
 *
 * \brief   Class StubbornSet, Inner Class Node
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/06/01
 *
 * \date    $Date: 2010-07-30 12:00:00 +0200 (Fr, 30. Jul 2010) $
 *
 * \version $Revision: 0.3 $
 */

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "stubbornset.h"
#include "imatrix.h"
#include "cmdline.h"
#include "verbose.h"

#include <vector>
#include <map>
#include <set>
#include <string>
#include <time.h>

using pnapi::Transition;
using pnapi::Place;
using pnapi::PetriNet;
using pnapi::Node;
using std::vector;
using std::deque;
using std::map;
using std::set;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

extern gengetopt_args_info args_info;

extern vector<Place*> po;

	/***************************************
	* Implementation of inner class MyNode *
	* myNodes are used for Tarjan's algor. *
	***************************************/

/** Constructor for transition nodes for stubborn set analysis. */
StubbornSet::Node::Node() : t(NULL),index(-2),low(-1),instack(false) {}
/** Destructor. */
StubbornSet::Node::~Node() {}
/** Reset for reusability. Reaches the same state as the constructor. */
void StubbornSet::Node::reset() { index=-2; low=-1; instack=false; nodes.clear(); }

	/**************************************
	* Implementation of class StubbornSet *
	**************************************/

/** Constructor for the stubborn set methods.
	@param tlist An ordering of all transitions of the Petri net.
	@param goal The marking the stubborn set methods are aiming at for reachability testing.
	@param imx The incidence matrix created by the constructor call to IMatrix for the Petri net.
	@param cov Whether the stubborn sets are built for coverability (true) or reachability (false).
*/
StubbornSet::StubbornSet(vector<Transition*> tlist, Marking& goal, IMatrix& imx, bool cov) : aim(goal),tord(tlist),im(imx),cover(cov) {
	for(unsigned int i=0; i<tord.size(); ++i)
	{
		tton.push_back(new Node());
		tton[i]->t = tord[i];
		revtord[tord[i]]=i;
	}
}

/** Destructor. Also frees any Node objects in use.
*/
StubbornSet::~StubbornSet() {
	map<Transition*,Node*>::iterator it;
	for(unsigned int i=0; i<tton.size(); ++i)
		delete tton[i];
}

/** Set the goal marking for the stubborn set method.
	@param goal The final marking.
*/
void StubbornSet::setGoalMarking(Marking& goal) { aim = goal; }

/** Set if the goal marking for the stubborn set method should be covered or reached.
	@param cov If the goal marking should be covered.
*/
void StubbornSet::setCover(bool cov) { cover = cov; }

/** Compute a stubborn set for a given extended marking.
	@param m The extended marking (i.e. a set of markings) for which a small set of necessary
		successors is to be found.
	@return The stubborn set, ordered by a greedy algorithm (earlier transitions have better
		chances to lead to the goal).
*/
vector<Transition*> StubbornSet::compute(ExtMarking& m) {
	// memorize the marking while we are computing (avoiding passing it to other methods :-()
	m0 = m;

	// select a place where m0 differs from the aim, use this as starting point for the stubborn set method
	Place* pstart(m0.distinguish(aim,false));
	if (!pstart) // we are at a goal node, all activated transitions must be tested
	{
		set<Transition*> tset(m0.firableTransitions(aim.getPetriNet(),im));
		return greedySort(tset); // sort by distance
	}
//cout << "Place: " << pstart->getName() << endl;

	// get the transitions diminishing the distance to the goal marking on place pstart
	set<Transition*> upset(changesMarkingOn(pstart,m0.lessThanOn(aim,*pstart)));
	if (upset.empty()) { return greedySort(upset); } // no transition will lead to the goal

	// compute the stubborn set for each upset transition and collect the transitions
	set<Transition*> stubset; // for collecting the transitions
	set<Transition*>::iterator uit;
//cout << "Upset: ";
//for(uit=upset.begin(); uit!=upset.end(); ++uit)
//cout << (*uit)->getName() << ",";
//cout << endl;
	for(uit=upset.begin(); uit!=upset.end(); ++uit) // walk through the upset
	{
		if (stubset.find(*uit)!=stubset.end()) continue; // we already have this one
		
		// Initialise tton(entries for conflict graph) and todo-list with an upset transition
		set<int> todo;
		todo.insert(revtord[*uit]);
		tton[revtord[*uit]]->index = -1;

		// calculate all dynamic conflicts and dependencies with resp. to marking m0
		conflictTable(*uit);
		Cftit cftit;

		// iterate through the todo-list of non-visited transitions (new ones may be added meanwhile)
		Transition* next; // the next transition to work on ...
		int npos; // and its number in our total ordering
		while (!todo.empty()) { // go on until the list becomes empty
			npos = *(todo.begin());
			next = tord[npos];

			// create stubborn set conflict&dependency graph for "next" transition
			cftit = cftab.find(next);
			if (cftit!=cftab.end()) {
				set<Transition*>::iterator cit;
				for(cit=cftit->second.begin(); cit!=cftit->second.end(); ++cit)
				{
					int cpos(revtord[*cit]); // the conflicting transition's number
					// new depending or conflicting transitions are added to the todo-list recursively
					if (tton[cpos]->index==-2) todo.insert(cpos);
					// the graph is built (arcs)
					tton[npos]->nodes.insert(cpos);
//cout << "Arc: " << next->getName() << " -> " << (*cit)->getName() << endl;
					// the transition is marked as "has been in the todo-list", so it won't be added twice 
					tton[cpos]->index = -1;
				}
			}
			todo.erase(npos); // we are done with this transition
		}

		// find strongly connected component(s) with activated transition in the stubborn set graph
		set<Transition*> tres(getSZK()); // get the stubborn set of activated transitions

		// add the new stubborn set transitions to the already collected ones
		set<Transition*>::iterator xit;
//cout << "Stubset for " << (*uit)->getName() << ":";
//for(xit=tres.begin(); xit!=tres.end(); ++xit)
//cout << (*xit)->getName() << ",";
//cout << endl;
		for(xit=tres.begin(); xit!=tres.end(); ++xit)
			stubset.insert(*xit);

		// clear tton (conflict graph) for use in the next loop or next call of compute()
		for(unsigned int i=0; i<tton.size(); ++i)
			tton[i]->reset();
	}

	uit = stubset.begin();
	while (uit!=stubset.end())
	{
		if (m0.isDisabled((**uit),im)) stubset.erase(uit++);
		else ++uit;
	}

	// order the resulting set by distance change w.r. to the goal marking
	return greedySort(stubset);
}

/** Order the transitions by the distance of the node they lead to from the active
	marking to the final marking.
	@param tset A set of transitions enabled under the active marking.
	@return The same set of transitions, sorted by the distance, closest first.
*/
vector<Transition*> StubbornSet::greedySort(set<Transition*> tset) {
	vector<Transition*> result;
	map<int,set<Transition*> > ordtres;
	set<Transition*>::iterator xit;
	for(xit=tset.begin(); xit!=tset.end(); ++xit) // group the transitions by distance
		ordtres[m0.distanceTo(**xit,aim,im)].insert(*xit);
	map<int,set<Transition*> >::iterator oit;
	for(oit=ordtres.begin(); oit!=ordtres.end(); ++oit) // and read them into a vector
		for(xit=oit->second.begin(); xit!=oit->second.end(); ++xit)
			result.push_back(*xit);
	return result;
}

/** Calculates which transitions a given transition may depend on
	(stubborn set) recursively, beginning at a transition tstart.
	@param tstart An arbitrary transition as first element of the stubborn set.
*/
void StubbornSet::conflictTable(Transition* tstart) {
	// remove all remains
	cftab.clear();
	PetriNet& pn(aim.getPetriNet());
	// marker for "has already been done"
	set<Transition*> marker;
	// the set of transitions to work on
	set<Transition*> tset;
	tset.insert(tstart); // initialized with the start transition
	set<Transition*>::iterator it;
	while (!tset.empty()) // go through the set until it becomes empty
	{
		it = tset.begin();
		// mark this transition as "done before" (so it won't be scheduled again)
		marker.insert(*it);
		// if the transition is enabled, look for conflicting transitions
		if (!m0.isDisabled(**it,im)) {
			// put all conflicting transitions into the stubborn set
			const set<pnapi::Arc*> tpre = (*it)->getPresetArcs();
			set<pnapi::Arc*>::iterator prearc;
			for(prearc=tpre.begin(); prearc!=tpre.end(); ++prearc)
			{ // go through the preset
				Place* p = &((*prearc)->getPlace());
				if (m0.isOpen(*p)) continue; // if m0 is unlimited at p, there can be no conflicts
				pnapi::Arc* a = pn.findArc(**it,*p);
				// places on which we effectively produce tokens do not lead to conflicts where the other transition must fire first
				if (a) if ((*prearc)->getWeight()<=a->getWeight()) continue;
//				int itcons((*prearc)->getWeight());
				int itprod(a?a->getWeight():0);
				const set<pnapi::Arc*> ppost = p->getPostsetArcs();
				set<pnapi::Arc*>::iterator postarc; // now check the postset of p for conflicting transitions
				for(postarc=ppost.begin(); postarc!=ppost.end(); ++postarc)
				{
					// found a conflicting transition
					Transition* cft = &((*postarc)->getTransition());
					// if it is the same, we don't need it
					if (cft==*it) continue;
					pnapi::Arc* a2 = pn.findArc(*cft,*p);
					int cftprod(a2?a2->getWeight():0);
					int cftcons((*postarc)->getWeight());
					// check if there is a conflict at all
					if (itprod>=cftcons && cftprod-cftcons<=0) continue; // each produces enough to let the other one live
					// mark it as conflicting, i.e. it possibly has priority
					cftab[*it].insert(cft);
//					cftab[cft].insert(*it);
//cout << "Cft Conflict: " << (*it)->getName() << " -> " << cft->getName() << endl;
					// if we haven't worked on it yet, insert it into the todo-list
					if (marker.find(cft)==marker.end()) tset.insert(cft);
				}
			}
		// if it is not enabled, look for predecessor transitions that could enable it
		} else {
			// put the pre-transitions of one place with not enough tokens into the stubborn set
			Place* hp(hinderingPlace(**it));
			set<Transition*> reqt(requiredTransitions(*hp));
			set<Transition*>::iterator rtit;
			for(rtit=reqt.begin(); rtit!=reqt.end(); ++rtit)
			{
				// do not put the same transition into the list of token-producing transitions, it cannot fire anyway
				if (*rtit==*it) continue;
				// mark it as having priority, as it produces a necessary token
				cftab[*it].insert(*rtit);
//cout << "Cft Dependency: " << (*it)->getName() << " -> " << (*rtit)->getName() << endl;
				// if we haven't worked on it yet, insert it into the todo-list
				if (marker.find(*rtit)==marker.end()) tset.insert(*rtit);
			}
		}
		tset.erase(it); // we are done with this transition
	}
}

/** Calculates a place with insufficient tokens to activate a given transition.
	@param t The transition to check
	@return Pointer to an insufficiently marked place. Program exit on failure.
*/
Place* StubbornSet::hinderingPlace(Transition& t) {
	set<pnapi::Arc*> aset = t.getPresetArcs();
	set<pnapi::Arc*>::iterator ait;
	set<Place*> pset;
	set<Place*>::iterator pit;
	for(ait=aset.begin(); ait!=aset.end(); ++ait)
	{
		// check if this place has enough tokens, record it as candidate if not
		if (m0.isOpen((*ait)->getPlace())) continue; // it has an unlimited number, that's enough
		if ((*ait)->getWeight()>(unsigned int)(m0.getLowerBound((*ait)->getPlace()))) 
			pset.insert(&((*ait)->getPlace()));
	}
	// catch error of missing scapegoat
	if (pset.empty()) abort(12,"internal error, no scapegoat for stubborn set method");
	// if the user opts for a random scapegoat, select one by time
//	if (args_info.scapegoat_given) 
//	{
		int rnd = clock()%(pset.size());
		pit = pset.begin();
		for(int i=0; i<rnd; ++i,++pit) ;
		Place *p = *pit;
		return p;
//	}
	// otherwise find the first place according to the global ordering placeorder/revporder
//	Place* p(NULL);
//	int pordnr = -1;
//	for(pit=pset.begin(); pit!=pset.end(); ++pit)
//		if (pordnr<0 || revpord[*pit]>pordnr)
//		{ p=*pit; pordnr=revpord[p]; }
//	// return the place
//	return p;
}

/** Calculate a set of transitions that increments the number of tokens on a place p.
	@param p The place where the token increment must occur.
	@return All transitions fulfilling the requirements.
*/
set<Transition*> StubbornSet::requiredTransitions(Place& p) {
	set<Transition*> tset;
	set<pnapi::Arc*> prearc = p.getPresetArcs();
	set<pnapi::Arc*>::iterator ait;
	for(ait=prearc.begin(); ait!=prearc.end(); ++ait)
	{
		// find transitions that produce tokens on p
		Transition* t = &((*ait)->getTransition());
		pnapi::Arc *a = aim.getPetriNet().findArc(p,*t);
		// add them to the result list if they produce more than they consume
		if (a==0) { tset.insert(t); continue; }
		if (a->getWeight()<(*ait)->getWeight()) tset.insert(t);
	}	
	return tset;
}

/* Modification of Tarjans algorithm for finding strongly connected components.
	We look for strongly connected components containing at least one activated 
	transition. Only activated transitions are returned.
	@param start The root of the graph.
	@param result Return parameter that will contain the transitions
		constituting some strongly connected components.
	@param maxdfs Recursive parameter for the depth in the depth-first-search, should be zero initially.
	@return Whether a suitable set of strongly connected components has been found.

bool StubbornSet::doTarjan(Node* start, set<Transition*>& result, int& maxdfs) {
	start->index = maxdfs;
	start->low = maxdfs;
	++maxdfs;
	// Tarjans stack, begin with the start node(transition)
	st.push_back(start->t);
	// marker for "is contained in the stack"
	start->instack = true;
	set<int>::iterator it; // int corresponds to Transition* via transitionorder/revtorder, but our choice of ordering
	for(it=start->nodes.begin(); it!=start->nodes.end(); ++it)
	{
		Node* mn(tton[*it]);
		if (mn->index<0) {
			if (doTarjan(mn,result,maxdfs)) return true;
			if (start->low>mn->low) start->low = mn->low;
		} else if (mn->instack)
			if (start->low>mn->index) start->low = mn->index;
	}
	if (start->low == start->index) {	// we found a SCC
		Transition* go = NULL;
		bool ok = false;
		do {	// pull the member transitions from the stack
			go = st.back();
			tton[revtord[go]]->instack = false;
			st.pop_back();
			// stop looking for strongly connected components once we have found one with an enabled transition
			if (!m0.isDisabled(*go,im)) 
			{
				ok=true;
				result.insert(go);
			}
		} while (start->t!=go); // until the component ends
		return ok; // signal whether the search must go on (false) or can stop (true)
	}
	return false;
}
*/

/** Outer loop for Tarjans algorithm, calls the inner algorithm for each
	connected component.
	@return Contains the activated transitions in the suitable strongly connected component(s).
*/
/*
set<Transition*> StubbornSet::getSZK() {
	int maxdfs = 0;
	// empty the stack for Tarjan
	st.clear();
	set<Transition*> result;
	for(unsigned int i=0; i<tton.size(); ++i)
	{
		// call Tarjans algorithm for all nodes that have not been visited.
		if (tton[i]->index==-1) 
			if (doTarjan(tton[i], result, maxdfs)) 
				return result;
	}
	return result; // never reached (graph is never empty)
}
*/
set<Transition*> StubbornSet::getSZK() {
	set<Transition*> result;
	for(unsigned int i=0; i<tton.size(); ++i)
		if (tton[i]->index==-1) result.insert(tton[i]->t);
	return result;
}

/** Obtain the starting set of transitions for the stubborn set method.
	@param p A place where actual marking and goal marking differ.
	@param lower If the actual marking is lower on that place.
	@return All transitions changing the marking on p in the right direction.
*/
set<Transition*> StubbornSet::changesMarkingOn(Place* p, bool lower) {
	set<Transition*> result;
	if (!p) return result; // check if the place is given
	set<pnapi::Arc*> arcs = (lower ? p->getPresetArcs() : p->getPostsetArcs()); // get the arcs doing the right thing on p
	set<pnapi::Arc*>::iterator ait;
	for(ait=arcs.begin(); ait!=arcs.end(); ++ait) // iterate through the arcs
	{
		// find the transition connected to p
		Transition* t = &((*ait)->getTransition());
		// if t changes the marking in the right direction, put it into the start set
//		if (lower ? (im.getEntry(*t,*p)>0) : (im.getEntry(*t,*p)<0)) result.insert(t);
		if (lower) {
			if (im.getEntry(*t,*p)<=0) continue; // if m0<aim on p, must produce on p
			if (im.getLoops(*t,*p)>m0.getUpperBound(*p)) continue; // and need at most than m0[p] to fire
		} else {
			if (im.getEntry(*t,*p)>=0) continue; // if m0>aim on p, must consume on p
			int aimtok(0);
			if (aim.getMap().find(p)!=aim.getMap().end()) aimtok=aim[*p];
			if (im.getLoops(*t,*p)>aimtok) continue; // and leave at most aim[p] on p
		}
		result.insert(t);
	}	
	return result;	
}

/** Get the goal marking for the stubborn set method.
	@return The goal marking.
*/
Marking& StubbornSet::getGoal() { return aim; }

