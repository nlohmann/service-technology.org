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
 * \date    $Date: 2013-11-05 12:00:00 +0200 (Di, 5. Nov 2013) $
 *
 * \version $Revision: 1.14 $
 */

#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "pathfinder.h"
#include "partialsolution.h"
#include "imatrix.h"
#include "jobqueue.h"
#include "verbose.h"
#include <pthread.h>
#include <unistd.h>
#include "sthread.h"

#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
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

namespace sara {
// our order of transitions, from main.cc
extern vector<Transition*> transitionorder; 
// reverse order of the transitions
extern map<const Transition*,int> revtorder; 
// our order of places, from main.cc
extern vector<Place*> placeorder; 
// reverse order of the places
extern map<Place*,int> revporder; 

// command line switches
extern bool flag_lookup, flag_verbose, flag_output, flag_treemaxjob, flag_forceprint, flag_continue, flag_scapegoat, flag_parallel_tree, flag_incremental;
// command line values
extern int val_lookup, val_treemaxjob;

// internal thread data for all threads, from sthread.h/cc
extern vector<SThread*> threaddata;
// mutex variables for the threads, from thread.cc
extern pthread_mutex_t main_mutex,print_mutex,fail_mutex,fpool_mutex,sc_mutex,solv_mutex,tps_mutex;
// condition variables for the mutexes
extern pthread_cond_t main_cond,main_cond2;
// list of idle threads to obtain helpers from
extern set<unsigned int> idleID;
// maximal number of threads running
extern unsigned int maxthreads;
// if there is a thread beyond the main process at all
extern bool multithreaded;
}

	/***************************************
	* Implementation of inner class MyNode *
	* myNodes are used for Tarjan's algor. *
	***************************************/

namespace sara {

/** Constructor for transition nodes for stubborn set analysis. */
PathFinder::myNode::myNode() : t(NULL),index(-2),low(-1),count(0),instack(false),enabled(false),scapegoat(NULL) {}

/** Destructor. */
PathFinder::myNode::~myNode() {}

/** Reset for reusability. Reaches the same state as the constructor. */
void PathFinder::myNode::reset() { index=-2; low=-1; count=0; instack=false; scapegoat=NULL; nodes.clear(); }

/** Reinitialises the graph before reusing it. */ 
void PathFinder::myNode::reinit() { low=-1; instack=false; if (index>=0) index=-1; }

	/*************************************
	* Implementation of class PathFinder *
	*************************************/

/** Constructor for a single job/partial solution.
	@param m The marking where the firing sequence should start off.
	@param tv The transition vector that should be realized.
	@param pst A JobQueue containing all the past jobs done so far for comparisons.
	@param itps A JobQueue of partial solutions. At the start it should contain only one partial solution
		which should be initialized with just the start marking of the problem.
	@param sol A JobQueue for solutions, initially empty.
	@param fail A JobQueue for failures, initially empty.
	@param imx The incidence matrix created by the constructor call to IMatrix for the Petri net.
	@param lookup The lookup table for solutions of lp_solve.
	@param ex A job to be excluded from comparisons with past jobs (typically the active job or its father).
*/
PathFinder::PathFinder(Marking& m, map<Transition*,int>& tv, JobQueue& pst, JobQueue& itps, JobQueue& sol, JobQueue& fail, IMatrix& imx, map<map<Transition*,int>,vector<PartialSolution> >& lookup, PartialSolution* ex) 
	: m0(m),fulltvector(tv),past(pst),tps(itps),solutions(sol),failure(fail),im(imx),shortcut(lookup),recsteps(0),solved(false),tvsize(0),excl(ex) {

	// for printing status info
	verbose=0;

	// maximal size of the shortcut list, so we won't insert too many solutions
	shortcutmax=-1;
	if (flag_lookup) shortcutmax=val_lookup;

	// additional options for a second run after a realisability problem could not be solved
	minimize = false;
	passedon = false;

	// compute the size of the sought solution
	map<Transition*,int>::iterator mit;
	for(mit=tv.begin(); mit!=tv.end(); ++mit) tvsize += mit->second;
}

/** Destructor.
*/
PathFinder::~PathFinder() {}

/** Recursive algorithm for finding a firing sequence realizing a vector of
	transitions.
	@param tid ID of the thread calling this function (0 for main process)
	@return If a firing sequence has been found and the overall algorithm should stop.
*/
bool PathFinder::recurse(unsigned int tid) {
	// Get the data fields of this thread
	SThread* thr(threaddata[tid]);

	// check if some other thread has already solved the problem
	if (solved || thr->restart) return true;

	// on screen progress indicator
	++recsteps;
	if (flag_verbose && !flag_output && isatty(fileno(stderr)))
	{
		switch (recsteps&0xFFFF) {
			case 0x0000: cerr << "|\b"; cerr.flush(); break;
			case 0x4000: cerr << "/\b"; cerr.flush(); break;
			case 0x8000: cerr << "-\b"; cerr.flush(); break;
			case 0xC000: cerr << "\\\b"; cerr.flush(); break;
		}
	}

	{ // begin scope to reduce visibility of local variables

	// if the user has specified a maximal number of new jobs to be created by a tree
	// and the number is reached, stop the recursion
	if (flag_treemaxjob && (int)(fpool.size())>=val_treemaxjob) return false;

	// a first node/transition (one that is activated and must fire eventually)
	Transition* tstart(NULL); // not initialized yet

	// flag indicating if we might have a full solution (so far, yes)
	bool terminate = true;

	// check whether there are transitions still to fire and if there is an enabled one.
	map<Transition*,int>::iterator recit;
	for(recit=thr->tv.begin(); recit!=thr->tv.end(); ++recit)
	{ // go through the vector tv that we want to realise
		if (recit->second>0) { // if there are still transitions that need to fire
			terminate = false; // we don't have a full solution
			tstart = recit->first; // check whether such a transition
			if (thr->m->activates(*tstart)) break; // is activated, if yes, we have a candidate
		}
	}

	// if no more transitions can fire:
	if (recit==thr->tv.end()) {
/*
		// optimisation left out (not useful at the moment)
		// add path to pool for complex diamond check (optimization)
		pthread_mutex_lock(&fpool_mutex);
		fpool.push_back(thr->fseq);
		pthread_mutex_unlock(&fpool_mutex);
		if (verbose>0) cerr << "sara: FPOOL size: " << fpool.size() << endl;
*/
		// create a partial solution with all the information (firing sequence, marking reached, rest vector
		PartialSolution newps(thr->fseq,*(thr->m),thr->tv);
		// copy the constraints from the active job
		newps.setConstraints(thr->tps.first()->getConstraints());
		// and set the total transition vector (for comparisons)
		newps.setFullVector(fulltvector);

		// realisability only: check if the sought solution would be smaller than the one we're looking for right now
		if (passedon && !isSmaller(fulltvector,torealize)) {
			pthread_mutex_lock(&fail_mutex);
			failure.push_fail(new PartialSolution(*(thr->tps.first()))); // going beyond the maximal firing sequence
			pthread_mutex_unlock(&fail_mutex);
		} else if (terminate) 
		{ // no more transitions should fire, we have a solution
			pthread_mutex_lock(&solv_mutex);
			if (solutions.push_solved(new PartialSolution(newps))) // push it to the solution queue
				if (flag_forceprint) printSolution(&newps); // should we print it immediately?
			pthread_mutex_unlock(&solv_mutex);
		} else {
			// no solution so far, so check if we came closer to it than the father job
			if (thr->tps.first()->compareSequence(thr->fseq,thr->tv) && !thr->tps.first()->getRemains().empty()) {
				if (!thr->tps.first()->isExtendable()) {
					pthread_mutex_lock(&fail_mutex);
					failure.push_fail(new PartialSolution(newps)); // sequence was not extended, this is part of a counterexample
					pthread_mutex_unlock(&fail_mutex);
				}
			} else { // no solution, no failure, so we have a new job to do
				if (thr->tps.find(&newps)>=0) { // check if the job has been done before

					// check if invariants go us closer, otherwise expand them 
					newps.checkAddedInvariants(im,m0,thr->tps.first());
					thr->tps.push_back(new PartialSolution(newps));

					// try to add this partial solution to the lookup table
					pthread_mutex_lock(&(threaddata[thr->rootID]->mutex));
					sctmp.push_back(newps);
					pthread_mutex_unlock(&(threaddata[thr->rootID]->mutex));
				}
			}
		}

		// status messages, print the new job and to which queue it was added (if any)
		if (verbose>1) {
			pthread_mutex_lock(&print_mutex);
			cerr << "[" << tid << "] ";
			if (passedon && !isSmaller(fulltvector,torealize)) cerr << "Beyond Passed on TVector:" << endl;
			else if (terminate) cerr << "Full Solution found:" << endl; 
			else if (thr->tps.first()->compareSequence(thr->fseq,thr->tv) && !thr->tps.first()->getRemains().empty()) 
				cerr << "Failure:" << endl;
			else cerr << "Partial Solution" << (newps.isExtendable() ? " (ext)":"") << ":" << endl;
			newps.show();
			cerr << "*** PF ***" << endl;
			pthread_mutex_unlock(&print_mutex);
		}

		// tell other threads if we have solved the problem
		if (!flag_continue && terminate) solved = true;

		// go up one level in the recursion, if there were nonfirable transitions left over,
		// but terminate the recursion altogether if this partial solution is a full solution.
		return (flag_continue?false:terminate); // surpress termination if -C flag is given
	}

	if (flag_incremental) adaptConflictGraph(thr,tstart);
	else {
		// clear tton (conflict graph) for stubborn set construction
		for(unsigned int i=0; i<thr->tton.size(); ++i)
			thr->tton[i]->reset();

		// Initialise tton(entries for conflict graph) and todo-list with the first transition
		thr->tton[revtorder[tstart]]->index = -1;

		// calculate all dynamic conflicts and dependencies with resp. 
		// to the active marking and the chosen transition
		conflictTable(thr,tstart);

		// iterate through a todo-list of non-visited transitions (new ones may be added meanwhile)
		vector<int>& todo(thr->tseti); // get a "new" vector (already allocated)
		todo.clear(); // and empty it 
		todo.push_back(revtorder[tstart]); // tstart is the first element that needs to be "done"
		unsigned int todoptr(0); // pointer to the active element in todo

		// now go through the list until all elements in it are done
		while (todoptr<todo.size()) {
			int npos(todo[todoptr]); // get the number of the transition to work upon
	
			// create stubborn set conflict&dependency graph for "npos" transition
			if (!thr->cftab[npos].empty()) { // check if there are conflicts/dependencies at all
				for(unsigned int i=0; i<thr->cftab[npos].size(); ++i) // then walk the list if not empty
				{
					// get the number of a transition in conflict with or dependent on "npos"
					int cpos(revtorder[thr->cftab[npos][i]]);
	
					// new depending or conflicting transitions are added to the todo-list recursively (but once only)
					if (thr->tton[cpos]->index==-2) todo.push_back(cpos);
	
					// the c&d graph is built (arcs)
					thr->tton[npos]->nodes.push_back(cpos);
	
					// the new transition is marked as "has been in the todo-list", so it won't be added twice 
					thr->tton[cpos]->index = -1;
				}
			}
			++todoptr; // we are done with this transition, go to the next one
		}
	}
	} // end scope


	// find strongly connected component(s) with an activated transition in the stubborn set graph
	vector<Transition*>& tordtmp(getSZK(thr)); // get a stubborn set of activated transitions
	vector<Transition*> tord;
	tord.reserve(tordtmp.size());

	// bring the stubborn set into an order according to our global transition ordering
	vector<bool>& bucket(thr->tbool); // vector for bucket sort, already allocated
	bucket.assign(im.numTransitions(),false); // clear the vector
	// sort the stubborn set
	for(unsigned int j=0; j<tordtmp.size(); ++j)
		bucket[revtorder[tordtmp[j]]]=true;

	// and reinsert the stubborn set in the right order
	for(unsigned int o=0; o<im.numTransitions(); ++o)
		if (bucket[o]) tord.push_back(transitionorder[o]); 
	// finally, put the stubborn set into the thread data,
	// so it can be accessed when we are deeper in the recursion
	thr->tord.push_back(tord);

	// for the diamond check we need the part of the stubborn set we have
	// already worked through (i.e. empty at the beginning)
	// if we are as deep as ever in the recursion, we first need
	// to make space for this stubborn set, otherwise just clear
	// the space already reserved	
	if (thr->stubsets.size()==thr->stubptr) {
		thr->stubsets.push_back(vector<Transition*> ());
		thr->stubsets.back().reserve(8); // educated guess, stubborn sets usually aren't that large
	} else thr->stubsets[thr->stubptr].clear();
//	thr->stubsets[thr->stubptr].reserve(tord.size());
	// adapt the pointer to the space where the next stubborn set goes
	++(thr->stubptr);

	// now we must check recursively if one of the transitions in the stubborn set allows for 
	// the firing of the remaining transition vector
	for(thr->tordptr.push_back(0); thr->tordptr.back()<tord.size(); ++(thr->tordptr.back()))
	{
		// tordptr points to the next element in the stubborn set to work upon
		// this may change (non-locally) if an element is assign to another thread
		// therefore, we make a local copy of tordptr:
		unsigned int o(thr->tordptr.back());

		// fire the transition (change the marking, subtract it from the remaining transitions,
		// add it to the firing sequence obtained so far) 
		im.successor(*(thr->m),*(tord[o])); // calc successor marking
		--(thr->tv[tord[o]]); // one instance less of this transition to fire
		thr->fseq.push_back(tord[o]); // add to partial firing sequence
//cerr << ">" << tord[o]->getName() << " ";

		// check if a diamond has just been completed, no need to traverse the same
		// subtree twice
		if (!checkForDiamonds(thr)) { // do not recurse if a diamond has just been completed
				// if a helper thread is available, assign one to a job of our choice
				if (!idleID.empty()) assignPathFinderHelper(tid);
//				assignPathFinderHelper(tid);

				// then go deeper into the recursion
				++(thr->recursedepth); // recursion depth is used to prevent forking deep in the recursion
				if (recurse(tid)) return true; // check if the successor marking leads to the goal
				--(thr->recursedepth);
		} 

//cerr << "<" << tord[o]->getName() << " ";
		// the transition we just tried does not lead to a success, revert all side effects:
		im.predecessor(*(thr->m),*(tord[o])); // calculate predecessor marking
		++(thr->tv[tord[o]]); // add the instance of the transition to the remaining transitions
		if (flag_incremental) adaptConflictGraph(thr,NULL); // revert changes to the conflict graph
		thr->fseq.pop_back(); // remove it from the firing sequence

		// activate the diamond check for this transition for the next recursion loop
		thr->stubsets[thr->stubptr-1].push_back(tord[o]);

		// in case helpers were assigned, jump over the transitions they have worked on
		// so that we don't do anything twice
		// at the same time, update the partial stubborn set (transitions done)
		while (o++<thr->tordptr.back()) {
			thr->stubsets[thr->stubptr-1].push_back(tord[o]);
		}

	}

	// we are done on this recursion level and need to go up
	// everything relating to the stubborn sets should be reverted to the previous state
	--(thr->stubptr);
	thr->tord.pop_back();
	thr->tordptr.pop_back();

	// we couldn't prolong the firing sequence we were called with, so go back one recursion level
	return false;
}


/** Calculates which transitions a given transition may depend on
	or be in conflict with, recursively.
	@param thr The thread calculating this c&d relation.
	@param tstart An arbitrary transition as first element of the stubborn set.
*/
void PathFinder::conflictTable(SThread* thr, Transition* tstart) {
	// empty the conflict table
	for(unsigned int i=0; i<thr->cftab.size(); ++i)
		thr->cftab[i].clear();

	// create a todo-list (memory was allocated previously, just clear it)
	vector<Transition*>& tset(thr->tsetb);
	tset.clear();

	// begin with the start transition
	tset.push_back(tstart);

	// and point to it (the active element)
	unsigned int tsetptr(0);

	// obtain a marker for "has been done" and clear it
	vector<bool>& marker(thr->tbool);
	marker.assign(im.numTransitions(),false);

	// as long as there is a new transition in tset ...
	while (tsetptr<tset.size())
	{
		// get such a transition and its number
		Transition* t(tset[tsetptr]);
		unsigned int tnr(revtorder[t]);

		// mark the transition as done (a bit early, but nevertheless)
		marker[tnr]=true;

		// if the transition is enabled, look for conflicting transitions
		if (thr->m->activates(*t)) {
			// put all conflicting transitions (via preset) into the stubborn set
			map<Place*,int>& tpre(im.getPreset(*t));
			map<Place*,int>& tpost(im.getPostset(*t));
			for(map<Place*,int>::iterator ppit=tpre.begin(); ppit!=tpre.end(); ++ppit)
			{ // go through the preset
				Place* p(ppit->first); // a place in the preset
				
				// check if the place has a loop with our transition
				map<Place*,int>::iterator ppit2(tpost.find(p));
				int itprod((ppit2==tpost.end() ? 0 : ppit2->second));

				// places on which we effectively produce tokens do not lead to conflicts 
				// where the other transition must fire first
				if (ppit->second<=itprod) continue;

				// get the place's postset and run through its transitions
				map<Transition*,int>& ppost(im.getPostset(*p));
				map<Transition*,int>& ppre(im.getPreset(*p));
				for(map<Transition*,int>::iterator tpit=ppost.begin(); tpit!=ppost.end(); ++tpit)
				{
					// found a conflicting transition
					Transition* cft(tpit->first);

					// if it is the same transition or shouldn't fire anyway, we don't need it
					if (cft==t || thr->tv[cft]==0) continue;

					// check if the conflicting transition has a loop with p
					map<Transition*,int>::iterator tpit2(ppre.find(cft));
					int cftprod((tpit2==ppre.end() ? 0 : tpit2->second));
					int cftcons(tpit->second);
					// check if there is a conflict at all
					// if we produce enough to let t live, and t cannot enable us, we are not in conflict
					if (itprod>=cftcons+(thr->tv[cft]-1)*(cftcons-cftprod) && cftprod-cftcons<=0) continue; 

					// mark cft as conflicting, i.e. cft possibly has priority
					thr->cftab[tnr].push_back(cft);

					// if we haven't worked on it yet, insert it into the todo-list
					if (!marker[revtorder[cft]]) tset.push_back(cft);
				}
			}
		// if t is not enabled, look for predecessor transitions that could enable it
		} else {
			// get a place with not enough tokens to fire t
			Place* hp(hinderingPlace(thr,*t));

			// get the transitions that produce tokens on that place
			requiredTransitions(thr,*hp); // side effect: result vector is tsets

			// walk through these transitions
			for(unsigned int rtit=0; rtit<thr->tsets.size(); ++rtit)
			{
				// get one of the transitions
				Transition* rt(thr->tsets[rtit]);

				// do not put t into the list of token-producing transitions, it cannot fire anyway
				if (rt==t) continue;

				// mark it as having priority, as it produces necessary tokens
				thr->cftab[tnr].push_back(rt);

				// if we haven't worked on it yet, insert it into the todo-list
				if (!marker[revtorder[rt]]) tset.push_back(rt);
			}
		}
		++tsetptr; // we are done with this transition
	}
}

/** Calculates which transitions a given transition may depend on
	or be in conflict with, recursively.
	@param thr The thread calculating this c&d relation.
	@param tstart An arbitrary transition as first element of the stubborn set.
*/
void PathFinder::adaptConflictGraph(SThread* thr, Transition* tstart) {

	unsigned int tstnr(tstart==NULL ? 0 : revtorder[tstart]);

	// reinit tton (conflict graph) for stubborn set construction
	for(unsigned int i=0; i<thr->tton.size(); ++i)
		thr->tton[i]->reinit();

	// obtain a marker for "has been done" and clear it
	vector<bool>& marker(thr->tbool);
	marker.assign(im.numTransitions(),false);

	// create a todo-list (memory was allocated previously, just clear it)
	vector<Transition*>& tset(thr->tsetb);
	tset.clear();

	// begin with the start transitions (tstart or the previously fired transition)
	if (!thr->fseq.empty()) {
		tset.push_back(thr->fseq.back());
		marker[revtorder[thr->fseq.back()]] = true;

		map<Place*,int>& opost(im.getPostset(*(thr->fseq.back())));
		for(map<Place*,int>::iterator oit=opost.begin(); oit!=opost.end(); ++oit)
		{
			map<Transition*,int>& ppost(im.getPostset(*(oit->first)));
			for(map<Transition*,int>::iterator qit=ppost.begin(); qit!=ppost.end(); ++qit)
				if (!marker[revtorder[qit->first]]) {
					tset.push_back(qit->first);
					marker[revtorder[qit->first]] = true;
				}
		}
	}
	if (tstart && thr->tton[tstnr]->index==-2) {
		if (!marker[tstnr]) {
			tset.push_back(tstart);
			marker[tstnr] = true;
		}
		thr->tton[tstnr]->index = -1;
	}

	// nodes for which a test for zero successors should be made (recursion start)
	vector<bool>& ntbool(thr->tboolb);
	ntbool.assign(im.numTransitions(),false);
	vector<int>& nulltest(thr->tseti);
	nulltest.clear();

	// point to the active element in tset
	unsigned int tsetptr(0);

	// as long as there is a new transition in tset ...
	while (tsetptr<tset.size())
	{
		// get such a transition and its number
		Transition* t(tset[tsetptr]);
		unsigned int tnr(revtorder[t]);
		myNode* node(thr->tton[tnr]);
		if (!node->count && !ntbool[tnr]) { nulltest.push_back(tnr); ntbool[tnr]=true; }

		// if the transition is newly enabled, look for conflicting transitions
		if (thr->m->activates(*t) && !node->enabled) {
			// remove all enabling transitions
			for(int j=0; j<node->nodes.size(); ++j)
				if (!(--thr->tton[node->nodes[j]]->count) && !ntbool[node->nodes[j]])
				{	
					nulltest.push_back(node->nodes[j]);
					ntbool[node->nodes[j]] = true;
				}
			node->nodes.clear();
			node->enabled = true;
			// so we know which transitions have already been added as conflicting:
			vector<bool>& added(thr->tboola);
			added.assign(im.numTransitions(),false);

			// put all conflicting transitions (via preset) into the stubborn set
			map<Place*,int>& tpre(im.getPreset(*t));
			map<Place*,int>& tpost(im.getPostset(*t));
			for(map<Place*,int>::iterator ppit=tpre.begin(); ppit!=tpre.end(); ++ppit)
			{ // go through the preset
				Place* p(ppit->first); // a place in the preset
				
				// check if the place has a loop with our transition
				map<Place*,int>::iterator ppit2(tpost.find(p));
				int itprod((ppit2==tpost.end() ? 0 : ppit2->second));

				// places on which we effectively produce tokens do not lead to conflicts 
				// where the other transition must fire first
				if (ppit->second<=itprod) continue;

				// get the place's postset and run through its transitions
				map<Transition*,int>& ppost(im.getPostset(*p));
				map<Transition*,int>& ppre(im.getPreset(*p));
				for(map<Transition*,int>::iterator tpit=ppost.begin(); tpit!=ppost.end(); ++tpit)
				{
					// found a conflicting transition
					Transition* cft(tpit->first);

					// if it is the same transition or shouldn't fire anyway, we don't need it
					if (cft==t || thr->tv[cft]==0) continue;

					// check if the conflicting transition has a loop with p
					map<Transition*,int>::iterator tpit2(ppre.find(cft));
					int cftprod((tpit2==ppre.end() ? 0 : tpit2->second));
					int cftcons(tpit->second);
					// check if there is a conflict at all
					// if we produce enough to let t live, and t cannot enable us, we are not in conflict
					if (itprod>=cftcons+(thr->tv[cft]-1)*(cftcons-cftprod) && cftprod-cftcons<=0) continue; 

					// mark cft as conflicting, i.e. cft possibly has priority
					if (!added[revtorder[cft]]) {
						node->nodes.push_back(revtorder[cft]);
						thr->tton[revtorder[cft]]->count++;
						added[revtorder[cft]] = true;
					}

					// if we haven't worked on it yet, insert it into the todo-list
					if (!marker[revtorder[cft]]) { tset.push_back(cft); marker[revtorder[cft]]=true; }
				}
			}
		// if t is newly disabled, look for predecessor transitions that could enable it
		// if t remains disabled, check if the scapegoat is still allowed
		} else if (!thr->m->activates(*t)) {
			bool goon(true);

			// first the case where the transition remains disabled
			if (!node->enabled && node->index!=-2) {
				pnapi::Arc* a(t->getPetriNet().findArc(*(node->scapegoat),*t));
				if (!a) abort(11,"error: no scapegoat for stubborn set method");
				// check if scapegoat is still available
				if (a->getWeight() > (*(thr->m))[*(node->scapegoat)]) goon = false;
			}

			// if the transition is newly disabled or the scapegoat must change
			if (goon) {
				// remove all conflicting transitions
				for(int j=0; j<node->nodes.size(); ++j)
				{
					if (!marker[node->nodes[j]]) {
						tset.push_back(transitionorder[node->nodes[j]]);
						marker[node->nodes[j]] = true;
					}
					if (!(--thr->tton[node->nodes[j]]->count) && !ntbool[node->nodes[j]])
					{
						nulltest.push_back(node->nodes[j]);
						ntbool[node->nodes[j]] = true;
					}
				}
				node->nodes.clear();
				node->enabled = false;
	
				// get a place with not enough tokens to fire t
				node->scapegoat = hinderingPlace(thr,*t);
	
				// get the transitions that produce tokens on that place
				requiredTransitions(thr,*(node->scapegoat)); // side effect: result vector is tsets
	
				// walk through these transitions
				for(unsigned int rtit=0; rtit<thr->tsets.size(); ++rtit)
				{
					// get one of the transitions
					Transition* rt(thr->tsets[rtit]);
	
					// do not put t into the list of token-producing transitions, it cannot fire anyway
					if (rt==t) continue;
	
					// mark it as having priority, as it produces necessary tokens
					node->nodes.push_back(revtorder[rt]);
					thr->tton[revtorder[rt]]->count++;
	
					// if we haven't worked on it yet, insert it into the todo-list
					if (!marker[revtorder[rt]]) { tset.push_back(rt); marker[revtorder[rt]]=true; }
				}
			}
		} else if (!thr->fseq.empty() && tsetptr==0) {
			// check all conflicting transitions too
			for(int j=0; j<node->nodes.size(); ++j)
			{
				if (!marker[node->nodes[j]]) {
					tset.push_back(transitionorder[node->nodes[j]]);				
					marker[node->nodes[j]] = true;
				}
			}
		}

		node->index = -1;
		++tsetptr; // we are done with this transition
	}

	// make recursive nulltests (remove nodes without predecessors except tstart)
	if (tstart) ++thr->tton[tstnr]->count;
	tsetptr = 0;
	while (tsetptr < nulltest.size()) {
		int tnr(nulltest[tsetptr++]);
		myNode* node(thr->tton[tnr]);
		if (node->count>0) continue;
		node->index = -2;
		node->enabled = false;
		for(int j=0; j<node->nodes.size(); ++j)
			if (!(--thr->tton[node->nodes[j]]->count) && !ntbool[node->nodes[j]])
			{
				nulltest.push_back(node->nodes[j]);
				ntbool[node->nodes[j]] = true;
			}
		node->nodes.clear();
	}
	if (tstart) --thr->tton[tstnr]->count;

//	printConflictGraph(thr,tstart);
}

/** Calculates a place with insufficient tokens to activate a given transition.
	@param thr The thread calling this function.
	@param t The transition to check.
	@return Pointer to an insufficiently marked place. Program exit on failure.
*/
Place* PathFinder::hinderingPlace(SThread* thr, Transition& t) {
	map<Place*,int>& pre(im.getPreset(t));
	vector<Place*> pset; // a temporary vector if -S was given on the command line

	// check if the preset places have enough tokens, record them as candidates if not
	for(map<Place*,int>::iterator ait=pre.begin(); ait!=pre.end(); ++ait)
		if (ait->second > (*(thr->m))[*(ait->first)])
		{
			if (!flag_scapegoat) return ait->first; // found a place, jump out
			pset.push_back(ait->first);
		}

	// catch error of missing scapegoat
	if (pset.empty()) abort(11,"error: no scapegoat for stubborn set method");

	// if the user opts for a random scapegoat, select one by time
	int rnd = clock()%(pset.size());
	return pset[rnd];
}

/** Calculate a set of transitions that increments
	the number of tokens on a place p and is contained in a
	to-be-fired list.
	@param thr The thread calling this function.
	@param p The place where the token increment must occur.
*/
void PathFinder::requiredTransitions(SThread* thr, Place& p) {
	// get reserved memory for the result vector
	vector<Transition*>& tset(thr->tsets);
	tset.clear();

	// traverse the preset of p
	map<Transition*,int>& pre(im.getPreset(p));
	map<Transition*,int>& post(im.getPostset(p));
	for(map<Transition*,int>::iterator tit=pre.begin(); tit!=pre.end(); ++tit)
	{
		// find transitions that produce tokens on p
		Transition* t(tit->first);
		// if they are required to fire at all ...
		if (thr->tv[t]>0) { 
			map<Transition*,int>::iterator tit2(post.find(t));
			// add them to the result list if they produce more than they consume
			if (tit2==post.end() || tit2->second<tit->second) tset.push_back(t);
		}
	}
}

/** Modification of Tarjans algorithm for finding strongly connected components.
	We look for small strongly connected components without outgoing arcs (sinks) and 
	containing at least one activated transition. Only activated transitions
	are returned.
	@param thr The thread calling this function.
	@param start The root of the graph.
	@param result Return parameter that will contain the transitions
		constituting some strongly connected components.
	@param maxdfs Recursive parameter for the depth in the depth-first-search, should be zero initially.
	@param st Stack for the algorithm.
	@return Whether a suitable strongly connected component has been found.
*/
bool PathFinder::doTarjan(SThread* thr, myNode* start, vector<Transition*>& result, int& maxdfs, vector<Transition*>& st) {
	// look up Tarjan's algorithm for details of this algorithm
	start->index = maxdfs;
	start->low = maxdfs;
	++maxdfs;
	// Tarjans stack, begin with the start node(transition)
	st.push_back(start->t);
	// marker for "is contained in the stack"
	start->instack = true;

	// go through the list of successors recursively
	for(unsigned int it=0; it<start->nodes.size(); ++it)
	{
		myNode* mn(thr->tton[start->nodes[it]]);
		if (mn->index<0) {
			if (doTarjan(thr,mn,result,maxdfs,st)) return true;
			if (start->low>mn->low) start->low = mn->low;
		} else if (mn->instack)
			if (start->low>mn->index) start->low = mn->index;
	}

	// if we have found a component, recover it
	if (start->low == start->index)
	{
		Transition* go(NULL);
		bool ok(false);
		do {	
			// pull the member transitions from the stack
			go = st.back();
			thr->tton[revtorder[go]]->instack = false;
			st.pop_back();

			// stop looking for strongly connected components once we have found one with an enabled and required transition
			// any transition in tv forms an up set; if such a transition is enabled, it ensures progress, thus we can stop here
			if (thr->tv[go]>0 && thr->m->activates(*go))
			{
				ok=true; // stop looking for other components
				result.push_back(go);
			}
		} while (start->t!=go); // go on until the component ends
		return ok; // signal whether the search must go on (false) or can stop (true)
	}
	return false;
}

/** Outer loop for Tarjans algorithm, calls the inner algorithm for each
	connected component.
	@param thr The thread calling this function.
	@return Contains the activated transitions in the suitable strongly connected component.
*/
vector<Transition*>& PathFinder::getSZK(SThread* thr) {
	int maxdfs = 0;

	// get an empty stack for Tarjan
	vector<Transition*>& st(thr->tsetb);
	st.clear();

	// get reserved memory for the result
	vector<Transition*>& result(thr->tseta);
	result.clear();

	// make sure all nodes are looked upon once
	for(unsigned int i=0; i<thr->tton.size(); ++i)
	{
		// call Tarjans algorithm for all nodes that have not yet been visited.
		if (thr->tton[i]->index==-1) 
			if (doTarjan(thr, thr->tton[i], result, maxdfs, st)) 
				return result;
	}
	return result; // never reached (graph is never empty)
}

/** Declare that the diamond checker should exclude paths that contain the same marking twice. */
void PathFinder::setMinimize() { minimize=true; }

/** Check for diamonds in the reachability graph that are about to be closed during the recursion.
	Also check if a marking is reached twice, as then the active partial solution can't be optimal
	(and therefore, if we can reach a solution, we will also reach a smaller one at some other point).
	@param thr The thread calling the diamond check.
	@return If a diamond is about to be closed or the marking is repeated.
*/
bool PathFinder::checkForDiamonds(SThread* thr)
{
	// get the active transition (just fired)
	Transition* t_active = thr->fseq.back();

	// no need to check anything if this is the only transition in the firing sequence
	if (thr->fseq.size()>1)
	{
		// get a vector to calculate the difference between this and previous markings
		vector<int>& diff(thr->psetb);
		diff.assign(im.numPlaces(),0); // no difference to the active marking (initially)
		map<Place*,int>::iterator dit;
		int pnr,diffcnt(0); // we count the places where the markings differ

		// check the active transition against all transitions and stubborn sets on an earlier level in the recursion
		for(int i=thr->fseq.size()-2; i>=0; --i) // don't check the transition against itself! (-> -2)
		{
			// get the effect of the latest transition
			map<Place*,int>& change(im.getColumn(*(thr->fseq[i]))); 

			// and subtract it from the marking difference (reverse firing)
			for(dit=change.begin(); dit!=change.end(); ++dit) 
			{
				pnr = revporder[dit->first]; // place number
				if (diff[pnr]==0) ++diffcnt; // the markings will differ on this place from now on
				diff[pnr] -= dit->second; // add the difference (reverse firing, so subtract)
				if (diff[pnr]==0) --diffcnt; // the markings coincide on this place now
			}
			// if we reach a marking again, there would be a smaller solution, so dismiss this one
			// (unless we came closer to activating a new transition on the way)
			if (minimize && diffcnt==0) if (!checkIfClosingIn(thr,i)) return true; 

			// if a transition isn't in some stubborn set, it either hasn't been done at that point or it can't fire
			// in any case: there is no diamond
			unsigned int j(0);
			for(; j<thr->stubsets[i].size(); ++j)
				if (thr->stubsets[i][j]==t_active) break; // found the active transition in the stubborn set
			if (j==thr->stubsets[i].size()) continue; // didn't find it, so no diamond possible

			// now create a new firing sequence beginning at recursion level i by switching
			// the transition on level i with the active transition.
			Transition* t_check = thr->fseq[i];
			if (t_active==t_check) continue; // but don't do it if it's the same transition

			vector<Transition*>& v(thr->tsetb);
			v.assign(thr->fseq.begin()+i,thr->fseq.end());
			v.front() = t_active;
			v.back() = t_check;

			// if that sequence can fire under the marking from level i, it has all been done before
			// and the diamond is complete. First test backward firability of t_check:
			map<Place*,int>& post(im.getPostset(*t_check));
			map<Place*,int>::iterator mit;
			for(mit=post.begin(); mit!=post.end(); ++mit)
				if (mit->second>(int)((*(thr->m))[*(mit->first)])) break;
			if (mit==post.end()) { // t_check is firable as a last step, now check the rest
				// get the places where t_active and t_check have a different effect
				vector<Place*>& pset(im.compareOutput(thr,*t_active,*t_check));
				// and check activation regarding only these places
				if (im.checkRestrictedActivation(*(thr->m),v,pset)) return true;
			}
		} 
	}

/*	this optimisation is left out at the moment, it seems to be not useful
	
	// if the checks against all recursion levels go wrong, no diamond is completed
	// Now do the complex check against all remembered firing sequences, hope the pool remains small
	pthread_mutex_lock(&fpool_mutex);
	for(index_t i=0; i<fpool.size(); ++i)
	{
		map<Transition*,int> tmp;
		if (thr->fseq.size()<=fpool[i].size())
		{
			for(unsigned int j=0; j<thr->fseq.size(); ++j)
			{
				++tmp[thr->fseq[j]];
				--tmp[fpool[i][j]];
			}
			map<Transition*,int>::iterator mit;
			for(mit=tmp.begin(); mit!=tmp.end(); ++mit)
				if (mit->second!=0) break;
			if (verbose>2 && mit==tmp.end()) cerr << "DC: FPool Hit (Entry " << i << ")" << endl;
			if (mit==tmp.end()) return true;
		}
	}
	pthread_mutex_unlock(&fpool_mutex);
*/
	return false;
}

/** Check whether one of the transitions that are expected to fire but have not so far
	comes closer to enabledness during the firing of a T-invariant than before that.
	@param thr The thread calling this check.
	@param start The starting point of the T-invariant in the active firing sequence.
		The end of the T-invariant must coincide with the end of the firing sequence.
	@return If the T-invariant has the desired positive effect.
*/
bool PathFinder::checkIfClosingIn(SThread* thr, int start) {
	// space to put the marking difference
	vector<int>& m1(thr->pseta);

	// walk through the remainder of transitions that need to fire
	map<Transition*,int>::iterator recit; 
	for(recit=thr->tv.begin(); recit!=thr->tv.end(); ++recit)
		if (recit->second>0) { // the transition should fire in the future
			map<Place*,int>& pre(im.getPreset(*(recit->first))); // get its preset

			// set the marking (only for places in the preset)
			map<Place*,int>::iterator mit;
			for(mit=pre.begin(); mit!=pre.end(); ++mit) m1[revporder[mit->first]] = m0[*(mit->first)];

			int need(-1); // the minimal token need of the transition, -1=uninitialized
			
			// walk through the markings in the firing sequence
			for(unsigned int i=0; i<thr->fseq.size(); ++i)
			{
				int ineed(0); // token need against this marking
				for(mit=pre.begin(); mit!=pre.end(); ++mit) // go through the preset
				{	// and add up all tokens necessary (additionally) for firing
					if (mit->second>(int)(m1[revporder[mit->first]])) ineed+=mit->second-m1[revporder[mit->first]];
					// get the successor marking (only on the preset)
					m1[revporder[mit->first]] += im.getEntry(*(thr->fseq[i]),*(mit->first));
				}
				// check if the need is lower than the value so far
				// if we are after the start of the T-invariant and just lowered the value,
				// the invariant pushes the transition towards enabledness, and we may return
				if (ineed<need || need<0) { need=ineed; if (i>(unsigned int)(start)) return true; }
			}
		}
	return false;
}

/** Print the given job as a full solution.
	@param ps The job containing the full solution.
*/
void PathFinder::printSolution(PartialSolution* ps) {
	cout << "\r";
	if (ps==NULL) { cout << "sara: UNSOLVED: solution is missing -- this should not happen" << endl; return; }
	vector<Transition*> solution = ps->getSequence();
	cout << "sara: SOLUTION: ";
	for(unsigned int j=0; j<solution.size(); ++j)
		cout << solution[j]->getName() << " ";
	if (solution.size()<6)
		for(unsigned int j=solution.size(); j<6; ++j) cout << "   ";
	cout << endl;
}

/** Componentwise comparison for m1<=m2.
	@param m1 A transition vector.
	@param m2 A transition vector.
	@return True if m1<=m2.
*/
bool PathFinder::isSmaller(map<Transition*,int>& m1, map<Transition*,int>& m2) {
	// the comparison uses the fact that transitions have the same ordering in both maps
	map<Transition*,int>::iterator mit1,mit2;
	for(mit2=m2.begin(),mit1=m1.begin(); mit2!=m2.end(); ++mit2)
	{
		while (mit1!=m1.end() && mit1->second==0) ++mit1; 
		if (mit1==m1.end()) return true;
		if (mit2->first!=mit1->first) continue;
		if (mit2->second<mit1->second) break;
		++mit1;
	}
	while (mit1!=m1.end() && mit1->second==0) ++mit1; 
	return (mit1==m1.end());
}

/** The main thread for a job waits for the other threads to finish their jobs in this function.
	@param rtnr The root thread number, i.e. the thread waiting.
	@param solution If the root thread has solved the problem.
	@return If a solution has been found by any thread.
*/
bool PathFinder::waitForThreads(unsigned int rtnr, bool solution) {
	// flag for "all threads are done"
	bool done(false);

	// the thread that solved the problem (-1 = none)
	int solver(-1);

	// wait until either a solution is found or all threads are done
	if (!solution) while (solver==-1 && !done) {
		pthread_mutex_lock(&(threaddata[rtnr]->mutex));
		solver = threaddata[rtnr]->solvedHelper;
		done = threaddata[rtnr]->root.empty();
		if (solver==-1 && !done) 
			pthread_cond_wait(&(threaddata[rtnr]->cond),&(threaddata[rtnr]->mutex));
		pthread_mutex_unlock(&(threaddata[rtnr]->mutex));
	}

	// status message if a solution was found
	if (solver >= 0 && verbose) {
		pthread_mutex_lock(&(print_mutex));
		cerr << "[M] Thread " << solver << " found a solution." << endl;
		pthread_mutex_unlock(&(print_mutex));
	}

	// tell all threads to quit their momentary job if we have a solution
	if (solver>=0 || solution) {
		makeThreadsIdle(verbose);
		if (verbose) {
			pthread_mutex_lock(&(print_mutex));
			cerr << "[M] All Threads signalled to restart." << endl;
			pthread_mutex_unlock(&(print_mutex));
		}
		pthread_mutex_lock(&(threaddata[rtnr]->mutex));
		while (!threaddata[rtnr]->root.empty())
			pthread_cond_wait(&(threaddata[rtnr]->cond),&(threaddata[rtnr]->mutex));
		pthread_mutex_unlock(&(threaddata[rtnr]->mutex));
	}

	if (!threaddata[rtnr]->root.empty()) abort(57,"WaitForThreads: non-empty thread dir");

	// transfer all new jobs found to the global job queue
	threaddata[rtnr]->tps.pop_front(true);
	tps.transfer(threaddata[rtnr]->tps);

	if (shortcutmax<0 || (int)(shortcut.size())<=shortcutmax) 
	{ // if the lookup table isn't full
		pthread_mutex_lock(&sc_mutex);
		shortcut[fulltvector] = sctmp; // put it into the lookup table for our lp_solve solution
		pthread_mutex_unlock(&sc_mutex);
	} 
	if (flag_verbose && (int)(shortcut.size())==shortcutmax) 
	{ // if the lookup table is full, print a warning
		cerr << "\r";
		status("warning: lookup table too large (%d elements)",shortcutmax); 
	}

	// reinit the helper list
	threaddata[rtnr]->solvedHelper = -1;
	delete threaddata[rtnr]->m;
	threaddata[rtnr]->m = NULL;

	return (solver>=0 ? true : solution);
}

/** Get the number of recursion steps done in this PathFinder.
	All Threads are counted.
	@return The number of recursion steps.
*/
unsigned int PathFinder::getRecSteps() { return recsteps; }

} // end namespace sara
