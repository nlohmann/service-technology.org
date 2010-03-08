// -*- C++ -*-

/*!
 * \file    reachalyzer.cc
 *
 * \brief   Class Reachalyzer
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2009/10/21
 *
 * \date    $Date: 2010-03-01 12:00:00 +0200 (Mo, 1. Mar 2010) $
 *
 * \version $Revision: -1 $
 */

#include <vector>
#include <map>
#include <time.h>
#include <cstdio>
#include <iomanip>


#ifndef PNAPI_PNAPI_H
#include "pnapi/pnapi.h"
#endif
#include "lpwrapper.h"
#include "partialsolution.h"
#include "pathfinder.h"
#include "imatrix.h"
#include "reachalyzer.h"
#include "jobqueue.h"
#include "cmdline.h"
#include "verbose.h"

using pnapi::PetriNet;
using pnapi::Marking;
using std::vector;
using std::map;
using std::cerr;
using std::cout;
using std::endl;
using std::setw;

extern gengetopt_args_info args_info;

	/******************************************
	* Implementation of the class Reachalyzer *
	******************************************/

/** Constructor with a Petri net, an initial and a final marking to test whether
	the final marking can be reached from the initial marking.
	@param pn The Petri net.
	@param m0 The initial marking.
	@param mf The final marking
	@param verbose The level of verbosity (0-3).
*/
Reachalyzer::Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, bool verbose, int debug, bool out, int brk) 
	: error(0),m1(m0),net(pn),cols(pn.getTransitions().size()),lpwrap(cols+1),im(pn),breakafter(brk) {
	// inherit verbosity/debug level
	this->verbose = debug;
	// if output goes to a file
	this->out = out;
	im.verbose = debug;
	stateinfo = verbose;
	solved = false; // if a solution has been found
	errors = false; // if errors have occurred

	// initialize lp_solve
	map<Place*,int> cover; // we look for reachability, not coverability, so this is empty
	lpwrap.verbose = debug;
	if (lpwrap.createMEquation(m0,mf,cover,FALSE)<0) { 
		cerr << "sara: error: createMEquation (lpsolve-init) failed" << endl; 
		error=LPSOLVE_INIT_ERROR; 
		return; 
	}

	// prepare solving marking equation, initialize first partial solution and job list
	PartialSolution* init(new PartialSolution(m0));
	tps.push_back(init);
}

/** Constructor with a Petri net, an initial and a final marking to test whether
	the final marking can be covered from the initial marking.
	@param pn The Petri net.
	@param m0 The initial marking.
	@param mf The final marking.
	@param cover The set of places where the final marking need only be covered (instead of
			reached exactly).
	@param verbose The level of verbosity (0-3).
*/
Reachalyzer::Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int> cover, bool verbose, int debug, bool out, int brk) 
	: error(0),m1(m0),net(pn),cols(pn.getTransitions().size()),lpwrap(cols),im(pn),breakafter(brk) {
	// inherit verbosity/debug level
	this->verbose = debug;
	this->out = out;
	im.verbose = debug;
	stateinfo = verbose;
	solved = false;
	errors = false;

	//initialize lp_solve
	lpwrap.verbose = debug;
	if (lpwrap.createMEquation(m0,mf,cover,FALSE)<0) { 
		cerr << "sara: error: createMEquation (lpsolve-init) failed" << endl; 
		error=LPSOLVE_INIT_ERROR; 
		return; 
	}
	// prepare solving marking equation, initialize first partial solution and job list
	PartialSolution* init(new PartialSolution(m0));
	tps.push_back(init);
}

/** Destructor. */
Reachalyzer::~Reachalyzer() {}

/** This method is called to analyze the given reachability problem. */
void Reachalyzer::start() {
	//start counting time here
	starttime = clock();

	solved = false; // not solved yet
	errors = false; // no errors yet
	int loops = 0; // counter for number of loops
	if (stateinfo && out) cout << "JOBS(done/open):";
	while (!solved && !tps.empty()) { // go through the job list as long as there are jobs in it and we have no solution
		if (breakafter>0 && breakafter<=loops) break; // debug option -break
		if (stateinfo && out) { 
			cout << setw(7) << loops << "/" << setw(7) << tps.size() << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"; 
			cout.flush();
		} 
		if (verbose>0) { // debug output, show active job
			cerr << endl;
			cerr << "sara: Round=" << (loops) << "   # Partial Solutions=" << tps.size() << endl;
			tps.first()->show();
		} 
		if (verbose>1) cerr << "Lookup-Table-Size: " << shortcut.size() << endl;
		map<Transition*,int> oldvector;
		// make a copy of the active job before we change it
		PartialSolution ps(tps.first()->getSequence(),tps.first()->getMarking(),tps.first()->getRemains());
		ps.setConstraints(tps.first()->getConstraints());
		if (!tps.first()->getRemains().empty()) // active job is no solution, modify it
		{
			oldvector = tps.first()->getFullVector();
			{ 
				tps.first()->touchConstraints(); // mark all constraints so far as old
//				tps.first()->calcCircleConstraints(im,m1); // old version of constraint builder
				tps.first()->buildSimpleConstraints(im); // add new constraints
			}
		}
		if (!lpwrap.stripConstraints()) { // remove all constraints not belonging to the marking equation
			cerr << "sara: error: resize_lp failed" << endl; 
			error=LPSOLVE_RESIZE_ERROR; 
			return; 
		}
		if (!lpwrap.addConstraints(*(tps.first()))) { // add the new constraints
			cerr << "sara: error: addConstraints failed" << endl; 
			error=LPSOLVE_UPDATE_ERROR; 
			return; 
		}
		int ret = lpwrap.solveSystem(); // now solve the system of marking equation + constraints
		if (verbose>1) cerr << "sara: lp_solve returned " << ret << endl;
		if (ret!=0 && ret!=1) { // in case we don't get a solution ...
			if (ret!=2) { errors = true; cerr << "sara: warning: failure of lp_solve (" << loops << ")" << endl; }
			else { // lp_solve denies a solution, this may be part of a counterexample
				set<Constraint>::iterator cit;
				for(cit=tps.first()->getConstraints().begin(); cit!=tps.first()->getConstraints().end(); ++cit)
					if ((*cit).isRecent() && (*cit).isJump()) break; // unchecked jumps cannot lead to failures
				if (cit==tps.first()->getConstraints().end()) {
					PartialSolution* fps(new PartialSolution(*(tps.first())));
					fps->setFeasibility(false);
					if (verbose>1) {
						cerr << "sara: Failure:" << endl;
						fps->show();
						cerr << "*** NOSOLUTION ***" << endl;
					}
					failure.push_fail(fps); // record this part of a counterexample
				}
			}
			tps.pop_front(); // delete this job
			++loops; // count it, but ...
			continue; // do not try to realize a firing sequence or create child jobs as lp_solve gave no solution
		}
		// calculate what the new constraints changed in the solution of lp_solve
		map<Transition*,int> fullvector(lpwrap.getTVector(net));
		map<Transition*,int> diff; // this is the difference of the old and new solution
		map<Transition*,int>::iterator vit;
		for(vit=fullvector.begin(); vit!=fullvector.end(); ++vit)
			if (vit->second>oldvector[vit->first])
				diff[vit->first] = vit->second;
		for(vit=oldvector.begin(); vit!=oldvector.end(); ++vit)
			if (vit->second>fullvector[vit->first]) break; // have we increased the solution or jumped to another minimal one?
		if (vit==oldvector.end() && ret<2 && !diff.empty()) 
		{	// if we have made a non-jump leading to a higher solution before we may make a jump next, so create such a job
			Constraint c(diff); // build constraint to forbid the increase, so another minimal solution will be sought
			c.setRecent(true);
			ps.setConstraint(c);
			ps.setFullVector(oldvector); // a jump must compare to the former solution, not to the new one
			PartialSolution* nps(new PartialSolution(ps));
			tps.push_back(nps); // put the new job into the list
			if (verbose>1) {
				cerr << "sara: New Partial Solution:" << endl;
				nps->show();
				cerr << "*** JUMP ***" << endl;
			}
		}
		if (!tps.first()->getRemains().empty()) // the active job is not a full solution
		{
			if (verbose>1) {
				map<Transition*,int>::iterator mit;
				cerr << "New-Old Vector: ";
				for(mit=fullvector.begin(); mit!=fullvector.end(); ++mit)
					if (mit->second>oldvector[mit->first])
						cerr << mit->first->getName() << ":" << mit->second-oldvector[mit->first] << " ";
				cerr << endl;
			}
			if (oldvector==fullvector)
			{ // if lp_solve didn't increase the solution, we have a portion of a counterexample
				// this can only happen if no transitions are enabled
				PartialSolution* fps(new PartialSolution(*(tps.first())));
				fps->setFeasibility(true);
				if (verbose>1) {
					cerr << "sara: Failure:" << endl;
					fps->show();
					cerr << "*** EQUAL ***" << endl;
				}
				failure.push_fail(fps); // record the counterexample
				tps.pop_front(); // and delete the job
				if (verbose>1) cerr << endl;
				++loops;
				continue; // do not try to find a realization
			}
		}
		if (!solutionSeen(fullvector)) // adapt known solutions (from a earlier loop) for the new constraints
		{ // no solutions known so far, calculate them by trying to realize a firing sequence
			PathFinder pf(m1,fullvector,cols,tps,solutions,im,shortcut);
			pf.verbose = verbose;
			solved = pf.recurse(); // main call to realize a solution
		} 
		tps.pop_front(); // we are through with this job
		if (verbose>0) cerr << endl;
		++loops;
	}
	if (stateinfo && out) cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
	if (stateinfo)
		cout << "sara: " << loops << " job" << (loops!=1?"s":"") << " done, " 
			<< tps.size() << " in queue, " << failure.trueSize() <<  " failure" 
			<< (failure.trueSize()!=1?"s":"") << ", " 
			<< solutions.trueSize() << " solution" << (solutions.trueSize()!=1?"s":"") << "." << endl;
	//stop counting time here
	endtime = clock();
}

/** Prints the results of the reachability analysis to stderr. */
void Reachalyzer::printResult() {
	if (!solutions.almostEmpty()) solutions.printSolutions();
/*
	if (solved && !tps.empty()) // in case we have a solution:
	{
		PartialSolution* ps(tps.findSolution()); // find it
		if (ps)
		{ // and print it
			vector<Transition*> solution = ps->getSequence();
			cout << "sara: SOLUTION: ";
			for(unsigned int j=0; j<solution.size(); ++j)
				cout << solution[j]->getName() << " ";
			cout << endl;
		} else cerr << "sara: error: solved, but no solution found" << endl; // this should never happen
	} // now check for errors (and other things) that disallow a decision
*/
	else if (errors) cout << "sara: UNSOLVED: Result is indecisive due to failure of lp_solve." << endl;
	else if (args_info.treemaxjob_given) cout << "sara: UNSOLVED: solution may have been cut off due to command line switch -T" << endl;
	if (!errors && !args_info.treemaxjob_given && (args_info.all_given || solutions.almostEmpty()))
	{ // if we have a counterexample or the all flag is set
			if (failure.trueSize()>0)
			{
				if (solutions.almostEmpty()) {
					cout << "sara: INFEASIBLE: ";
					if (failure.checkMEInfeasible() && solutions.almostEmpty())
					{ // it might be that the initial marking equation has no solution
						cout << "the marking equation is infeasible." << endl; 
						if (stateinfo) if (!lpwrap.findNearest())
							cout << "sara: warning: unable to suggest changes, probably a failure of lp_solve" << endl;
						return; 
					}
					// or the marking equation is feasible but still we cannot reach a solution 
					cout << "unable to borrow enough tokens via T-invariants." << endl;
				} else if (stateinfo) cout << "sara: at the following points the algorithm got stuck:" << endl;
				if (stateinfo) failure.printFailure(im); // then print the counterexample; the following shouldn't happen:
			} else if (solutions.almostEmpty()) 
				cout << "sara: UNSOLVED: Result is indecisive, no counterexamples found." << endl;
	}
}

/** Gives the amount of CPU time used for solving the problem.
	@return The CPU time used.
*/
clock_t Reachalyzer::getTime() { return (endtime-starttime); }

/** Find out about the status of the problem. Directly after calling the constructor, the status
	should be checked for LPSOLVE_INIT_ERROR as in that case calling start() will lead to
	undetermined behaviour. After the call to start the status will determine if a solution or
	counterexample was found or the problem remains unsolved.
	@return The status of the problem.
*/
int Reachalyzer::getStatus() {
	if (solved && !tps.empty()) return SOLUTION_FOUND;
	if (error>0) return error;
	if (errors) return LPSOLVE_RUNTIME_ERROR;
	if (!failure.empty()) return COUNTEREXAMPLE_FOUND;
	return UNSOLVED;	
}

/** Gets either a solution or one possible counterexample, depending on if one was found.
	@return A solution if status is SOLUTION_FOUND or a counterexample if status is COUNTEREXAMPLE_FOUND,
		null otherwise.
*/
/*
PartialSolution* Reachalyzer::getSolution() {
	if (solved && !tps.empty()) return (tps.findSolution());
	if (!errors && !failure.empty()) return (failure.first());
	return NULL;
}
*/

/** Gets all possible counterexamples. The result is valid only if status is COUNTEREXAMPLE_FOUND.
	@return A list of possible counterexamples.
*/
JobQueue& Reachalyzer::getFailureReasons() {
	return failure;
}

/*
void Reachalyzer::extendTransitionOrder(map<Transition*,int> fullv, vector<Transition*>& ord, vector<Transition*> fseq) {
	for(int i=0; i<ord.size(); ++i)
		fullv[ord[i]]=0;
	for(int i=0; i<fseq.size(); ++i)
	{
		if (fullv[fseq[i]]>0) ord.push_back(fseq[i]);
		fullv[fseq[i]]=0;
	}
	map<Transition*,int>::iterator mit;
	for(mit=fullv.begin(); mit!=fullv.end(); ++mit)
		if (mit->second>0) ord.push_back(mit->first);
}
*/

/** Check whether a solution of lp_solve has been found earlier. If this happened, use that
	former solution to create the necessary new jobs without again trying to find realizations
	of the solution in form of firing sequences, using a lookup table.
	@return If the solution has been calculated before.
*/
bool Reachalyzer::solutionSeen(map<Transition*,int>& tv) {
	if (shortcut.find(tv)==shortcut.end()) return false;
	vector<PartialSolution>& vp(shortcut[tv]);
	for(unsigned int i=0; i<vp.size(); ++i)
	{
		vp[i].setConstraints(tps.first()->getConstraints());
		PartialSolution* cp(new PartialSolution(vp[i]));
		tps.push_back(cp);
		if (verbose>1) {
			cerr << "sara: Seen Partial Solution:" << endl;
			cp->show();
			cerr << "*** LOOKUP ***" << endl;
		}
	}
	return true;
}

