// -*- C++ -*-

/*!
 * \file    pathfinder.cc
 *
 * \brief   Class Pathfinder, Class MyNode
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/14
 *
 * \date    $Date: 2010-03-01 12:00:00 +0200 (Mo, 1. Mar 2010) $
 *
 * \version $Revision: -1 $
 */

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "pathfinder.h"
#include "partialsolution.h"
#include "imatrix.h"
#include "jobqueue.h"
#include "cmdline.h"

#include <vector>
#include <map>
#include <set>
#include <string>
#include <time.h>

using pnapi::Transition;
using pnapi::Place;
using pnapi::PetriNet;
//using pnapi::Arc;
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
extern vector<Transition*> transitionorder;
extern map<Transition*,int> revtorder;
extern map<Place*,int> revporder;

	/***************************************
	* Implementation of inner class MyNode *
	* myNodes are used for Tarjan's algor. *
	***************************************/

/** Constructor */
PathFinder::myNode::myNode() : t(NULL),index(-2),low(-1),instack(false) {}
/** Destructor */
PathFinder::myNode::~myNode() {}
/** Reset for reusability. Reaches the same state as the constructor. */
void PathFinder::myNode::reset() { index=-2; low=-1; instack=false; nodes.clear(); }

	/*************************************
	* Implementation of class PathFinder *
	*************************************/

/** Constructor.
	@param m The marking where the firing sequence should start off.
	@param tv The transition vector that should be realized.
	@param col The number of columns (transition) in the marking equation
	@param tps A JobQueue of partial solutions. At the start it should contain only one partial solution
		which should be initialized with just the start marking of the problem.
	@param im The incidence matrix created by the constructor call to IMatrix for the Petri net.
	@param lookup The lookup table for solutions of lp_solve
*/
PathFinder::PathFinder(Marking& m, map<Transition*,int>& tv, int col, JobQueue& itps, IMatrix& imx, map<map<Transition*,int>,vector<PartialSolution> >& lookup) 
	: m0(m),fulltvector(tv),rec_tv(tv),cols(col),tps(itps),im(imx),shortcut(lookup),recsteps(0) {
	for(unsigned int i=0; i<transitionorder.size(); ++i)
	{
		tton.push_back(new myNode());
		tton[i]->t = transitionorder[i];
	}
	pos=0;
	verbose=0;
	shortcutmax=1000;
	if (args_info.lookup_given) shortcutmax=(unsigned int)(args_info.lookup_arg);
}

/** Destructor. Also frees any myNode objects in use.
*/
PathFinder::~PathFinder() {
	map<Transition*,myNode*>::iterator it;
	for(unsigned int i=0; i<tton.size(); ++i)
		delete tton[i];
}

/** Recursive algorithm for finding a firing sequence given a vector of
	transitions.
	@return If a firing sequence has been found.
*/
bool PathFinder::recurse() {
	// on screen progress indicator
	if (args_info.verbose_given && !args_info.output_given)
	{
		switch (++recsteps&0x1FFF) {
			case 0x0000: cerr << "|\b"; cerr.flush(); break;
			case 0x0800: cerr << "/\b"; cerr.flush(); break;
			case 0x1000: cerr << "-\b"; cerr.flush(); break;
			case 0x1800: cerr << "\\\b"; cerr.flush(); break;
		}
	}
	{ // begin scope to reduce visibility of local variables
	// if the user has specified a maximal number of new jobs to be created by a tree
	// and the number is reached, stop the recursion
	if (args_info.treemaxjob_given && (int)(fpool.size())>=args_info.treemaxjob_arg) return false;

	// obtain a first node/transition (one that is activated and must fire eventually)
	map<Transition*,int>::iterator recit;
	Transition* tstart = NULL;
	bool terminate = true; // so far, we might have a full solution
	// check whether more transition are to fire and if there is an enabled one.
	for(recit=rec_tv.begin(); recit!=rec_tv.end(); ++recit)
	{
		if (recit->second>0) { // there are still transitions that need to fire
			terminate = false; // so we don't have a full solution
			tstart = recit->first; // check whether such a transition
			if (m0.activates(*tstart)) break; // is activated, if yes, we found a candidate
		}
	}
	// if no more transitions can fire:
	if (recit==rec_tv.end()) {
		// progress indication
		if (verbose>0) {
			if ((++pos)%1000==0) 
				{ cerr << "sara: PATH: ";
				for(unsigned int l=0; l<fseq.size(); ++l) cerr << fseq[l]->getName() << " ";
				cerr << " (" << pos << ")" << endl; }
		}
		// add path to pool for complex diamond check (optimization)
		fpool.push_back(fseq);
		if (verbose>0) cerr << "sara: FPOOL size: " << fpool.size() << endl;

		// create a partial solution with all the information ...
		PartialSolution newps(fseq,m0,rec_tv);
		newps.setConstraints(tps.first()->getConstraints());
		newps.setFullVector(fulltvector);
		if (tps.find(&newps)>=0) { // job isn't already in the queue
		if (tps.almostEmpty() || !tps.first()->betterSequenceThan(fseq,m0,rec_tv))
		{
			// but only save it for later use (=adaption of constraints) if it is new or better than the things we already have
			if (terminate) newps.setSolved(); // no more transitions to fire, so we have a solution
			tps.push_back(new PartialSolution(newps)); // put the job into the queue
			// try to add this partial solution to the lookup table
			if (shortcut.size()<shortcutmax) shortcut[fulltvector].push_back(newps);
			else if (args_info.verbose_given && shortcut.size()==shortcutmax) 
			{ 
				cerr << "sara: warning: lookup table too large (" << shortcutmax << ")" << endl; 
				shortcut[fulltvector].push_back(newps); 
			}
			if (verbose>1) {
				cerr << "sara: New Partial Solution:" << endl;
				newps.show();
				cerr << "*** PF ***" << endl;
			}
		} else if (verbose>2) cerr << "sara: OldJobBetterThanNew-Hit" << endl;
		} else if (verbose>2) cerr << "sara: CheckAgainstQueue-Hit" << endl;
		// go up one level in the recursion, if there were nonfirable transitions left over,
		// but terminate the recursion altogether if this partial solution is a full solution.
		return terminate; 
	}

	// Initialise tton(entries for conflict graph) and todo-list with the first transition
	set<int> todo;
	todo.insert(revtorder[tstart]);
	tton[revtorder[tstart]]->index = -1;
	if (verbose>2) cerr << "TStart: " << tstart->getName() << endl;

	// calculate all dynamic conflicts and dependencies with resp. to marking m0
	conflictTable(tstart);
	Cftit cftit;
	if (verbose>2) {
		cerr << "sara: Conflicts:";
		for(cftit=cftab.begin(); cftit!=cftab.end(); cftit++)
		{
			set<Transition*>::iterator tit;
			cerr << cftit->first->getName() << "[";
			for(tit=cftit->second.begin(); tit!=cftit->second.end(); tit++)
				cerr << (*tit)->getName() << ",";
			cerr << "] ";
		}
		cerr << endl;
	}

	// iterate through the todo-list of non-visited transitions (new ones may be added meanwhile)
	Transition* next; // the next transition to work on ...
	int npos; // and its number in our total ordering
	while (!todo.empty()) { // go on until the list becomes empty
		npos = *(todo.begin());
		next = transitionorder[npos];

		// create stubborn set conflict&dependency graph for "next" transition
		cftit = cftab.find(next);
		if (verbose>2) cerr << "Working on " << next->getName() << endl;
		if (cftit!=cftab.end()) {
			set<Transition*>::iterator cit;
			for(cit=cftit->second.begin(); cit!=cftit->second.end(); cit++)
			{
				int cpos(revtorder[*cit]);
				// new depending or conflicting transitions are added to the todo-list recursively
				if (tton[cpos]->index==-2) todo.insert(cpos);
				if (verbose>2) cerr << "Index of " << (*cit)->getName() << " is " << tton[cpos]->index << endl;
				// the graph is built (arcs)
				tton[npos]->nodes.insert(cpos);
				// the transition is marked as "has been in the todo-list", so it won't be added twice 
				tton[cpos]->index = -1;
			}
		}
		todo.erase(npos); // we are done with this transition
	}
	if (verbose>2) {
		cerr << "tton: ";
		for(unsigned int i=0; i<tton.size(); ++i)
		{
			cerr << transitionorder[i]->getName() << ":[";
			set<int>::iterator xt;
			for(xt=tton[i]->nodes.begin(); xt!=tton[i]->nodes.end(); ++xt)
				cerr << tton[(*xt)]->t->getName() << ",";
			cerr << "] ";
		}
		cerr << endl;
	}
	} // end scope

	// put an empty entry for the diamond check (no transitions yet) on the stack	
	set<Transition*> tres;
	stubsets.push_back(tres); 

	// put the local marking on the stack (also used for the diamond check)
	mv.push_back(m0);

	// find strongly connected component(s) with activated transition in the stubborn set graph
	tres = getSZK(); // get the stubborn set of activated transitions

	// clear tton (conflict graph) for use in the next recursion step
	for(unsigned int i=0; i<tton.size(); ++i)
		tton[i]->reset();

	if (verbose>2) {
		// print the stubborn set
		set<Transition*>::iterator tresit;
		cerr << "sara: SZK:";
		for(tresit=tres.begin(); tresit!=tres.end(); tresit++)
		{
			cerr << " " << (*tresit)->getName();
		}
		cerr << endl;
	}

	// check recursively if one of the transitions in the stubborn set allows for 
	// the firing of the remaining transition vector, use the global transition ordering 
	vector<Transition*> tord;
	for(unsigned int o=0; o<transitionorder.size(); ++o)
		if (tres.find(transitionorder[o])!=tres.end()) tord.push_back(transitionorder[o]);
	for(unsigned int o=0; o<tord.size(); ++o)
	{
		if (verbose>2) {
			// debug output
			cerr << "sara: PATH: ";
			for(unsigned int l=0; l<fseq.size(); ++l) cerr << fseq[l]->getName() << " ";
			cerr << endl;
			cerr << "sara: StubSet at (" << stubsets.size()-1 << "):";
			set<Transition*>::iterator xx;
			for(xx=tres.begin(); xx!=tres.end(); ++xx)
				cerr << (*xx)->getName() << " ";
			cerr << endl << "sara: Work done at (" << stubsets.size()-1 << "):";
			for(xx=stubsets.back().begin(); xx!=stubsets.back().end(); ++xx)
				cerr << (*xx)->getName() << " ";
			map<const Place*,unsigned int>::const_iterator mit;
			cerr << endl << "sara: Marking: ";
			for(mit=m0.begin(); mit!=m0.end(); ++mit)
				cerr << mit->first->getName() << ":" << mit->second << " "; 
			cerr << endl;
			cerr << "sara: Active: " << tord[o]->getName() << endl;
		}
		im.successor(m0,*(tord[o])); // calc successor marking
		--rec_tv[tord[o]]; // one instance less of this transition to fire
		fseq.push_back(tord[o]); // add to partial firing sequence
		if (!checkForDiamonds()) // do not recurse if a diamond has just been completed
			if (recurse()) return true; // check if successor marking leads to goal
		// the transition we just tried does not lead to a success, revert all side effects:
		im.predecessor(m0,*(tord[o])); // calc predeccessor marking
		++rec_tv[tord[o]]; // add the instance of the transition to the remaining transitions
		fseq.pop_back(); // remove it from the firing sequence
		// activate the diamond check for this transition for the next recursion loop
		stubsets.back().insert(tord[o]); 
	}
	// print debug info
	if (verbose>2) {
		cerr << "sara: StubSet finished at (" << stubsets.size()-1 << "):";
		set<Transition*>::iterator xx;
		for(xx=stubsets.back().begin(); xx!=stubsets.back().end(); ++xx)
			cerr << (*xx)->getName() << " ";
		cerr << endl;
	}
	// remove the set for the diamond check and the local marking from the stack, as we are done with it
	stubsets.pop_back();
	mv.pop_back();
	// we couldn't prolong the firing sequence we were called with, so go back one recursion level
	return false;
}


/** Calculates which transitions a given transition may depend on
	(stubborn set) recursively, beginning at a transition tstart.
	@param tstart An arbitrary transition as first element of the stubborn set.
*/
void PathFinder::conflictTable(Transition* tstart) {
	// remove all remains
	cftab.clear();
	PetriNet& pn(m0.getPetriNet());
	// create a todo-list
	set<Transition*> tset;
	// begin with the start transition
	tset.insert(tstart);
	// marker for "has been done"
	set<Transition*> marker;
	set<Transition*>::iterator it;
	while (!tset.empty())
	{
		it = tset.begin();
		// mark this transition as done (a bit early, but nevertheless)
		marker.insert(*it);
		// if the transition is enabled, look for conflicting transitions
		if (m0.activates(**it)) {
			// put all conflicting transitions into the stubborn set
			const set<pnapi::Arc*> tpre = (*it)->getPresetArcs();
			set<pnapi::Arc*>::iterator prearc;
			for(prearc=tpre.begin(); prearc!=tpre.end(); ++prearc)
			{ // go through the preset
				Place* p = &((*prearc)->getPlace());
				pnapi::Arc* a = pn.findArc(**it,*p);
				// places on which we effectively produce tokens do not lead to conflicts where the other transition must fire first
				if (a) if ((*prearc)->getWeight()<=a->getWeight()) continue;
				const set<pnapi::Arc*> ppost = p->getPostsetArcs();
				set<pnapi::Arc*>::iterator postarc; // now check the postset of p for conflicting transitions
				for(postarc=ppost.begin(); postarc!=ppost.end(); ++postarc)
				{
					// found a conflicting transition
					Transition* cft = &((*postarc)->getTransition());
					// if it is the same or shouldn't fire anyway, we don't need it
					if (cft==*it || rec_tv[cft]==0) continue;
					// mark it as conflicting, i.e. it possibly has priority
					cftab[*it].insert(cft);
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
Place* PathFinder::hinderingPlace(Transition& t) {
	set<pnapi::Arc*> aset = t.getPresetArcs();
	set<pnapi::Arc*>::iterator ait;
	set<Place*> pset;
	set<Place*>::iterator pit;
	for(ait=aset.begin(); ait!=aset.end(); ait++)
	{
		// check if this place has enough tokens, record it as candidate if not
		if ((*ait)->getWeight()>m0[(*ait)->getPlace()]) 
			pset.insert(&((*ait)->getPlace()));
	}
	// catch error of missing scapegoat
	if (pset.empty()) { cerr << "sara: error: no scapegoat for stubborn set method" << endl; exit(EXIT_FAILURE); }
	// if the user opts for a random scapegoat, select one by time
	if (args_info.scapegoat_given) 
	{
		int rnd = clock()%(pset.size());
		pit = pset.begin();
		for(int i=0; i<rnd; ++i,++pit) ;
		Place *p = *pit;
		return p;
	}
	// otherwise find the first place according to the global ordering placeorder/revporder
	Place* p(NULL);
	int pord = -1;
	for(pit=pset.begin(); pit!=pset.end(); ++pit)
		if (pord<0||revporder[*pit]>pord)
		{ p=*pit; pord=revporder[p]; }
	// return the place
	return p;
}

/** Calculates a set of transitions that increments
	the number of tokens on a place p and is contained in a
	to-be-fired list.
	@param p The place where the token increment must occur.
	@return All transitions fulfilling the requirements.
*/
set<Transition*> PathFinder::requiredTransitions(Place& p) {
	set<Transition*> tset;
	set<pnapi::Arc*> prearc = p.getPresetArcs();
	set<pnapi::Arc*>::iterator ait;
	for(ait=prearc.begin(); ait!=prearc.end(); ait++)
	{
		// find transitions that produce tokens on p
		Transition* t = &((*ait)->getTransition());
		// if they are required to fire at all ...
		if (rec_tv[t]>0)
		{
			pnapi::Arc *a = m0.getPetriNet().findArc(p,*t);
			// add them to the result list if they produce more than they consume
			if (a==0) { tset.insert(t); continue; }
			if (a->getWeight()<(*ait)->getWeight())
				tset.insert(t);
		}
	}	
	return tset;
}

/** Modification of Tarjans algorithm for finding strongly connected components.
	We look for small strongly connected components without outgoing arcs and 
	containing at least one activated transition. Only activated transitions
	are returned.
	@param start The root of the graph.
	@param result Return parameter that will contain the transitions
		constituting some strongly connected components.
	@param maxdfs Recursive parameter for the depth in the depth-first-search, should be zero initially.
	@return Whether a suitable set of strongly connected components has been found.
*/
bool PathFinder::doTarjan(myNode* start, set<Transition*>& result, int& maxdfs) {
	start->index = maxdfs;
	start->low = maxdfs;
	++maxdfs;
	// Tarjans stack, begin with the start node(transition)
	st.push_back(start->t);
	// marker for "is contained in the stack"
	start->instack = true;
	set<int>::iterator it; // int corresponds to Transition* via transitionorder/revtorder, but our choice of ordering
	for(it=start->nodes.begin(); it!=start->nodes.end(); it++)
	{
		myNode* mn(tton[*it]);
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
			tton[revtorder[go]]->instack = false;
			st.pop_back();
			// stop looking for strongly connected components once we have found one with an enabled and required transition
			if (rec_tv[go]>0 && m0.activates(*go)) 
			{
				ok=true;
				result.insert(go);
			}
		} while (start->t!=go); // until the component ends
		return ok; // signal whether the search must go on (false) or can stop (true)
	}
	return false;
}

/** Outer loop for Tarjans algorithm, calls the inner algorithm for each
	connected component.
	@return Contains the activated transitions in the suitable strongly connected components.
*/
set<Transition*> PathFinder::getSZK() {
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

/** Checks for diamonds in the reachability graph that are about to be closed during the recursion.
	Also checks if a marking is reached twice, as then the active partial solution can't be optimal
	(and therefore, if we can reach a solution, we will also reach a smaller one at some other point).
	@return If a diamond is about to be closed or the marking is repeated.
*/
bool PathFinder::checkForDiamonds()
{
	Transition* t_active = fseq.back();
	// check the active transition against all stubborn sets on an earlier level in the recursion
	if (fseq.size()>1)
		for(int i=fseq.size()-2; i>=0; --i) // don't check the transition against itself! (-> -2)
		{
			if (mv[i]==m0) return true; // if we reach a marking again, there would be a smaller solution, so dismiss this one
			// if it isn't in some set, it either hasn't been done at that point or it can't fire
			// in any case: there is no diamond
			if (stubsets[i].find(t_active)==stubsets[i].end()) continue;
			// now create a new firing sequence beginning at recursion level i by switching
			// the transition on level i with the active transition.
			Transition* t_check = fseq[i];
			vector<Transition*> v;
			v.assign(fseq.begin()+i,fseq.end());
			v.front() = t_active;
			v.back() = t_check;
			// if that sequence can fire under the marking from level i, it has all been done
			// and the diamond is complete.
			set<Place*> pset(im.compareOutput(*t_active,*t_check));
			if (im.checkRestrictedActivation(mv[i],v,pset)) return true;
		} 
	// if the checks against all recursion levels go wrong, no diamond is completed
	// Now do the complex check against all remembered firing sequences, hope the pool remains small
	for(unsigned int i=0; i<fpool.size(); ++i)
	{
		map<Transition*,int> tmp;
		if (fseq.size()<=fpool[i].size())
		{
			for(unsigned int j=0; j<fseq.size(); ++j)
			{
				++tmp[fseq[j]];
				--tmp[fpool[i][j]];
			}
			map<Transition*,int>::iterator mit;
			for(mit=tmp.begin(); mit!=tmp.end(); ++mit)
				if (mit->second!=0) break;
			if (mit==tmp.end()) return true;
		}
	}
	return false;
}

/** Print a full solution if there is one in the job list.
*/
void PathFinder::printSolution() {
	PartialSolution* ps(tps.findSolution());
	if (ps==NULL) { cout << "sara: UNSOLVED: solution is missing" << endl; return; }
	vector<Transition*> solution = ps->getSequence();
	cout << "sara: SOLUTION: ";
	for(unsigned int j=0; j<solution.size(); ++j)
		cout << solution[j]->getName() << " ";
	cout << endl;
}

/*
vector<Transition*> PathFinder::calcOrder(set<Transition*> tset) {
	vector<Transition*> result;
	set<Transition*> active,marked;
	map<Place*,int> ch(im.getChange(rec_tv));
	map<Place*,int>::iterator mit;
	for(mit=ch.begin(); mit!=ch.end(); ++mit)
	if (mit->second!=0)
	{
		set<Arc*>::iterator ait;
		set<Arc*> arcs(mit->first->getPresetArcs());
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			active.insert(&((*ait)->getTransition()));
		arcs = mit->first->getPostsetArcs();
		for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			active.insert(&((*ait)->getTransition()));
	}
	while (!tset.empty())
	{
		set<Place*> ack;
		set<Transition*>::iterator tit;
		for(tit=active.begin(); tit!=active.end(); ++tit)
		{
			marked.insert(*tit);
			if (tset.find(*tit)!=tset.end()) 
			{
				result.push_back(*tit);
				tset.erase(*tit);
			}
			map<Place*,int>& col(im.getColumn(**tit));
			map<Place*,int>& loop(im.getLoopColumn(**tit));
			map<Place*,int>::iterator cmit;
			for(cmit=col.begin(); cmit!=col.end(); ++cmit) ack.insert(cmit->first);
			map<Place*,int>::iterator lmit;
			for(lmit=loop.begin(); lmit!=loop.end(); ++lmit) ack.insert(lmit->first);
		}
		active.clear();
		set<Place*>::iterator pit;
		for(pit=ack.begin(); pit!=ack.end(); ++pit)
		{
			set<Arc*>::iterator ait;
			set<Arc*> arcs((*pit)->getPresetArcs());
			for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			{
				Transition* t(&((*ait)->getTransition()));
				if (marked.find(t)==marked.end()) active.insert(t);
			}
			arcs = (*pit)->getPostsetArcs();
			for(ait=arcs.begin(); ait!=arcs.end(); ++ait)
			{
				Transition* t(&((*ait)->getTransition()));
				if (marked.find(t)==marked.end()) active.insert(t);
			}
		}
	}
//	cerr << "TV: ";
//	for(int i=0; i<result.size(); ++i)
//		cerr << result[i]->getName() << " ";
//	cerr << endl;
	return result;
}
*/
