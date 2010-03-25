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

/*  Constructor with a Petri net, an initial and a final marking to test whether
	the final marking can be reached from the initial marking.
	@param pn The Petri net.
	@param m0 The initial marking.
	@param mf The final marking
	@param verbose The level of verbosity (0-3).
Reachalyzer::Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, Problem& pb, bool verbose, int debug, bool out, int brk) 
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
	if (lpwrap.createMEquation(m0,mf,cover,pb,FALSE)<0) { 
		cerr << "sara: error: createMEquation (lpsolve-init) failed" << endl; 
		error=LPSOLVE_INIT_ERROR; return;
	}

	// prepare solving marking equation, initialize first partial solution and job list
	PartialSolution* init(new PartialSolution(m0));
	tps.push_back(init);
}
*/

/*! Constructor with a Petri net, an initial and a final marking to test whether
	the final marking can be covered from the initial marking.
	\param pn The Petri net.
	\param m0 The initial marking.
	\param mf The final marking.
	\param cover The set of places where the final marking need only be covered (instead of
			reached exactly).
	\param pb The instance of the problem where additional (global) constraints may be found.
	\param verbose If the Reachalyzer should give additional infos.
	\param debug The level of the debug infos printed (0-3), highest == most information.
	\param out If we print to stdout.
	\param brk The value of the option --break, if set.
	\param passedon If the problem was passed on from an earlier run of the PathFinder.
*/
Reachalyzer::Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int> cover, Problem& pb, bool verbose, int debug, bool out, int brk, bool passedon) 
	: error(0),m1(m0),net(pn),cols(pn.getTransitions().size()),lpwrap(cols),im(pn),breakafter(brk) {
	// inherit verbosity/debug level
	this->verbose = debug;
	this->out = out;
	im.verbose = debug;
	stateinfo = verbose;

	this->passedon = passedon; // if a realizability problem was infeasible we are now looking for reasons etc.
	if (passedon) torealize = pb.getVectorToRealize();

	solved = false;
	errors = false;
	maxsollen = 0;
	sumsollen = 0;

	//initialize lp_solve
	lpwrap.verbose = debug;
	if (lpwrap.createMEquation(m0,mf,cover,pb,FALSE)<0) { 
		cerr << "sara: error: createMEquation (lpsolve-init) failed" << endl; 
		error=LPSOLVE_INIT_ERROR; return;
	}
	// prepare solving marking equation, initialize first partial solution and job list
	PartialSolution* init(new PartialSolution(m0));
	tps.push_back(init);
}

/*! Destructor. */
Reachalyzer::~Reachalyzer() {}

/*! This method is called to analyze the given reachability problem. */
void Reachalyzer::start() {
	//start counting time here
	starttime = clock();

	solved = false; // not solved yet
	errors = false; // no errors yet
	int loops = 0; // counter for number of loops (jobs)
	if (stateinfo && out) cout << "JOBS(done/open):";
	while (!solved && !tps.empty()) { // go through the job list as long as there are jobs in it and we have no solution
		// if --continue is specified, solved will never be set
		if (breakafter>0 && breakafter<=loops) break; // debug option --break
		if (stateinfo && out) { // overwrite job info if stdout (screen) is used
			cout << setw(7) << loops << "/" << setw(7) << tps.size() << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"; 
			cout.flush();
		} 
		if (verbose>0) { // debug output, show active job
			cerr << endl;
			cerr << "sara: Job " << (loops) << ",   To do: " << tps.size() << endl;
			tps.first()->show();
		} 
		if (verbose>1) cerr << "Lookup-Table-Size: " << shortcut.size() << endl; // debug info
		map<Transition*,int> oldvector(tps.first()->getFullVector()); // the previous (father) solution of lp_solve
		// make a copy of the active job before we change it
		PartialSolution ps(*(tps.first()));
		nextJump(*(tps.first())); // create a new jump if one has been precomputed, before changes are made
		tps.first()->touchConstraints(false); // mark all normal constraints so far as old
//		tps.first()->calcCircleConstraints(im,m1); // old version of constraint builder, not used anymore
		tps.first()->buildSimpleConstraints(im); // compute and create new constraints
		if (!lpwrap.stripConstraints()) { // remove all constraints not belonging to the marking equation
			cerr << "sara: error: resize_lp failed" << endl; 
			error=LPSOLVE_RESIZE_ERROR; return; 
		}
		lpwrap.addConstraints(*(tps.first())); // add the constraints to the marking equation
		int ret = lpwrap.solveSystem(); // now solve the system of marking equation + constraints
		if (verbose>1) cerr << "sara: lp_solve returned " << ret << endl; // debug info
		if (ret!=0 && ret!=1) { // in case we don't get a solution ...
			if (ret!=2) { errors = true; cerr << "sara: warning: failure of lp_solve (" << loops << ")" << endl; }
			else { // lp_solve denies a solution, this may be part of a counterexample
				set<Constraint>::iterator cit;
				for(cit=tps.first()->getConstraints().begin(); cit!=tps.first()->getConstraints().end(); ++cit)
					if ((*cit).isRecent() && (*cit).isJump()) break; // newly introduced jumps cannot lead to counterexamples
				if (cit==tps.first()->getConstraints().end()) { // now it's not a recent jump ...
					PartialSolution* fps(new PartialSolution(*(tps.first()))); // copy the old job
					fps->setFeasibility(false); // state that we have no solution and thus the marking equation might be infeasible
					if (verbose>1) { // debug info
						cerr << "sara: Failure:" << endl;
						fps->show();
						cerr << "*** NOSOLUTION ***" << endl;
					}
					failure.push_fail(fps); // record this part of a counterexample
				}
			}
			tps.pop_front(false); // delete this job
			++loops; // count it, but ...
			continue; // do not try to realize a firing sequence or create child jobs as lp_solve gave no solution
		}
		tps.first()->touchConstraints(true); // mark all jump constraints so far as old
		// calculate what the new constraints changed in the solution of lp_solve
		map<Transition*,int> fullvector(lpwrap.getTVector(net));
		map<Transition*,int> diff; // this contains changes from the old to the new solution
		map<Transition*,int>::iterator vit;
		for(vit=fullvector.begin(); vit!=fullvector.end(); ++vit)
			if (vit->second>oldvector[vit->first]) // if the solution has grown here
				diff[vit->first] = vit->second; // note the new value (not the difference!)
		if (!tps.first()->getRemains().empty()) // if the active job is not a full solution
		{
			if (verbose>1) { // debug info
				map<Transition*,int>::iterator mit;
				cerr << "New-Old Vector: ";
				for(mit=fullvector.begin(); mit!=fullvector.end(); ++mit)
					if (mit->second>oldvector[mit->first])
						cerr << mit->first->getName() << ":" << mit->second-oldvector[mit->first] << " ";
				cerr << endl;
			}
			if (oldvector==fullvector)
			{ // if lp_solve didn't increase the solution, we have a portion of a counterexample
				// this should only happen if no transitions are enabled at all
				PartialSolution* fps(new PartialSolution(*(tps.first()))); // copy the job
				fps->setFeasibility(true); // there was a solution, the marking equation is feasible
				if (verbose>1) { // debug info
					cerr << "sara: Failure:" << endl;
					fps->show();
					cerr << "*** EQUAL ***" << endl;
				}
				failure.push_fail(fps); // record the counterexample
				tps.pop_front(false); // and delete the job
				if (verbose>1) cerr << endl;
				++loops; // count the job
				continue; // do not try to find a realization
			}
		}
		tps.first()->transformJumps(fullvector); // change jump constraints to normal ones
		if (tps.findPast(tps.first())) // check if the transformed job has already been done before
		{ // i.e. if there is a previous job with the same constraints
			tps.pop_front(true); // pop_front, but don't put it in past list (it's there already)
			++loops; // count the job
			continue; // do not do anything with it
		}
		if (ret<2 && !diff.empty()) 
		{	// if the solution has changed we may make a jump next, so create such a job
			ps.touchConstraints(true); // mark all jump constraints so far as old
			createJumps(diff,ps); // precompute possible jumps and create one job for these jumps
		}
		if (!solutionSeen(fullvector)) // adapt known solutions (from an earlier loop) for the new constraints
		{ // no solutions known so far, calculate them by trying to realize a firing sequence
//			if (!passedon || isSmaller(fullvector,torealize)) {
				PathFinder pf(m1,fullvector,cols,tps,solutions,failure,im,shortcut);
				pf.verbose = ((breakafter>0 && breakafter<=loops+2)?3:verbose);
				pf.setMinimize(); // do not allow repeating markings in firing sequences
				pf.passedon = passedon;
				if (passedon) pf.torealize = torealize;
				solved = pf.recurse(); // main call to realize a solution
/*			} else { // going past the original problem vector, this is a failure 
				PartialSolution* fps(new PartialSolution(*(tps.first()))); // copy the job
				fps->setFeasibility(true); // there was a solution, the marking equation is feasible
				if (verbose>1) { // debug info
					cerr << "sara: Failure:" << endl;
					fps->show();
					cerr << "*** BEYOND PASSED ON ***" << endl;
				}
				failure.push_fail(fps); // record the counterexample
			}
*/
		} 
		tps.pop_front(false); // we are through with this job, move it to the past jobs list
		if (verbose>0) cerr << endl;
		++loops; // count the job
	}
	if (stateinfo && out) cout << "\r"; // if on screen
	if (args_info.break_given && stateinfo) // debug output if option --break was used
	{ 
		cerr << endl << endl << "****** JobQueue ******" << endl;
		tps.show(true); 
		cerr << "****** SolutionQueue ******" << endl;
		solutions.show(false); 
		cerr << "****** FailureQueue ******" << endl;
		failure.show(false); 
		cerr << "****** End of Queues ******" << endl << endl;
	} // debug option --break
	if (stateinfo && !passedon) // counting the jobs etc. if --verbose was given
		cout << "sara: " << loops << " job" << (loops!=1?"s":"") << " done, " 
			<< tps.size() << " in queue, " << failure.trueSize() <<  " failure" 
			<< (failure.trueSize()!=1?"s":"") << ", " 
			<< solutions.trueSize() << " solution" << (solutions.trueSize()!=1?"s":"") << "." << endl;
	//stop counting time here
	endtime = clock();
}

/*! Prints the results of the reachability analysis to stderr. */
void Reachalyzer::printResult() {
	// print all solutions first and find the longest solution length
	if (!solutions.almostEmpty()) 
	{
		if (passedon) cout << "sara: A shorter realizable firing sequence with the same token effect would be:" << endl;
		maxsollen = solutions.printSolutions(sumsollen);
	}
	else if (errors) cout << "sara: UNSOLVED: Result is indecisive due to failure of lp_solve." << endl;
	else if (args_info.treemaxjob_given) cout << "sara: UNSOLVED: solution may have been cut off due to command line switch -T" << endl;
	if (!errors && !args_info.treemaxjob_given && (args_info.witness_given || solutions.almostEmpty()))
	{ // if we have no solution or witnesses are sought anyway
			if (failure.trueSize()>0 && !args_info.break_given)
			{ // if there are witnesses and --break was not specified
				if (solutions.almostEmpty() && !passedon) {
					cout << "sara: INFEASIBLE: "; // check why there is no solution:
					if (failure.checkMEInfeasible() && solutions.almostEmpty())
					{ // it might be that the initial marking equation has no solution
						cout << "the marking equation is infeasible." << endl; 
						if (stateinfo) if (!lpwrap.findNearest())
							cout << "sara: warning: unable to suggest changes, probably a failure of lp_solve" << endl;
						return; 
					}
					// or the marking equation is feasible but still we cannot reach a solution 
					cout << "unable to borrow enough tokens via T-invariants." << endl;
					cout << "There are firing sequences that could not be extended towards the final marking." << endl;
				} else if (passedon && stateinfo) cout << "sara: at the following points the algorithm could not continue:" << endl;
				else if (stateinfo) cout << "sara: at the following points the algorithm needed to backtrack:" << endl;
				if (stateinfo) failure.printFailure(im); // then print the counterexample; the following shouldn't happen:
			} else if (solutions.almostEmpty())
				cout << "sara: UNSOLVED: Result is indecisive" << (args_info.break_given?" due to a break.":", no counterexamples found.") << endl;
	}
}

/*! Gives the amount of CPU time used for solving the problem.
	\return The CPU time used.
*/
clock_t Reachalyzer::getTime() { return (endtime-starttime); }

/*! Find out about the status of the problem. Directly after calling the constructor, the status
	should be checked for LPSOLVE_INIT_ERROR as in that case calling start() will lead to
	undetermined behaviour. After the call to start the status will determine if a solution or
	counterexample was found or the problem remains unsolved.
	\return The status of the problem.
*/
int Reachalyzer::getStatus() {
	if (!solutions.almostEmpty()) return SOLUTION_FOUND;
	if (error>0) return error;
	if (errors) return LPSOLVE_RUNTIME_ERROR;
	if (!failure.almostEmpty()) return COUNTEREXAMPLE_FOUND;
	return UNSOLVED;	
}

/*  Gets either a solution or one possible counterexample, depending on if one was found.
	\return A solution if status is SOLUTION_FOUND or a counterexample if status is COUNTEREXAMPLE_FOUND,
		null otherwise.
PartialSolution* Reachalyzer::getSolution() {
	if (solved && !tps.empty()) return (tps.findSolution());
	if (!errors && !failure.empty()) return (failure.first());
	return NULL;
}
*/

/*  Gets all possible counterexamples. The result is valid only if status is COUNTEREXAMPLE_FOUND.
	@return A list of possible counterexamples.
JobQueue& Reachalyzer::getFailureReasons() {
	return failure;
}
*/

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

/*! Check whether a solution of lp_solve has been found earlier. If this happened, use that
	former solution to create the necessary new jobs without again trying to find realizations
	of the solution in form of firing sequences, using a lookup table.
	\return If the solution has been calculated before.
*/
bool Reachalyzer::solutionSeen(map<Transition*,int>& tv) {
	if (shortcut.find(tv)==shortcut.end()) return false; // it's not in the lookup table
	vector<PartialSolution>& vp(shortcut[tv]); // job list from the lookup table
	for(unsigned int i=0; i<vp.size(); ++i) // go through the jobs
	{
		vp[i].setConstraints(tps.first()->getConstraints()); // set the constraints to match our new problem
		if (!tps.findPast(&(vp[i])) && tps.find(&(vp[i]))>=0)
		{
			PartialSolution* cp(new PartialSolution(vp[i])); // make a copy of the job
			tps.push_back(cp); // and insert it into the job list
			if (verbose>1) { // debug info
				cerr << "sara: Seen Partial Solution:" << endl;
				cp->show();
				cerr << "*** LOOKUP ***" << endl;
			}
		}
	}
	return true; // the solution is in the lookup table (but new jobs are not necessarily created)
}

/*! Precompute a list of jumps to be done. Create a job for the first one.
	\param diff The values in the new solution that are greater than the old (father) solution.
	\param ps The job on which the jumps are to be based.
*/
void Reachalyzer::createJumps(map<Transition*,int>& diff, PartialSolution& ps) {
	PartialSolution* nps(new PartialSolution(ps)); // make a copy of the job
	nps->setJC(diff); // make all jump alternatives available in a list for future use
	int val=0;
	Transition* t(nps->getNextJC(val)); // get the first alternative
	nps->popJC(); // and remove it from the list
	// now create the according job
	map<Transition*,int> tmp;
	tmp[t] = val;
	Constraint c(tmp,true); // build constraint to forbid the increase, so another minimal solution will be sought
	c.setRecent(true); // tag the constraint as new
	nps->setConstraint(c); // add it to the job
	if (!tps.findPast(nps) && tps.find(nps)>=0)
	{ // if the new job has not been done before and wasn't planned before
		tps.push_back(nps); // put the new job into the list
		if (verbose>1) { // debug info
			cerr << "sara: New Partial Solution:" << endl;
			nps->show();
			cerr << "*** CREATE JUMPS ***" << endl;
		}
	} else { // if this jump shouldn't be done
		nextJump(*nps); // move on the next in our list
		delete nps; // and delete this one
	} 
}

/*! Create a job for the next precomputed jump.
	\param ps The job for the former jump containing the precomputed jump list.
*/
void Reachalyzer::nextJump(PartialSolution& ps) {
	int val=0;
	Transition* t(ps.getNextJC(val)); // get the next alternative jump
	if (t==NULL) return; // [none available]
	ps.popJC(); // and remove it from the list
	PartialSolution* nps(new PartialSolution(ps)); // copy the job

	// first remove the old jump constraint (which is still tagged as recent)
	set<Constraint>& cs(nps->getConstraints());
	set<Constraint>::iterator cit;
	for(cit=cs.begin(); cit!=cs.end(); ++cit)
		if (cit->isRecent() && cit->isJump()) { cs.erase(cit); break; }

	// then add the new one and create the according job
	map<Transition*,int> tmp;
	tmp[t] = val;
	Constraint c(tmp,true); // build constraint to forbid the increase, so another minimal solution will be sought
	c.setRecent(true); // tag the new jump as recent
	nps->setConstraint(c); // add it to the job
	if (!tps.findPast(nps) && tps.find(nps)>=0)
	{ // if it hasn't been done and wasn't planned so far
		tps.push_back(nps); // put the new job into the list
		if (verbose>1) { // debug info
			cerr << "sara: New Partial Solution:" << endl;
			nps->show();
			cerr << "*** NEXT JUMP ***" << endl;
		}
	} else { // otherwise:
		delete nps; // delete this job
		nextJump(ps); // and move on to the next one in the list
	} 
}

/*! Get the recorded length of the longest solving firing sequence, if one has been found.
*/
int Reachalyzer::getMaxTraceLength() { return maxsollen; }

/*! Get the recorded length of the longest solving firing sequence, if one has been found.
*/
int Reachalyzer::getSumTraceLength() { return sumsollen; }

/** Get the number of solutions in the solutions queue.
	@return Number of solutions.
*/
int Reachalyzer::numberOfSolutions() { return solutions.size(); }


