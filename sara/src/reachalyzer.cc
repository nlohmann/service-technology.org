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
 * \date    $Date: 2011-04-08 12:00:00 +0200 (Fr, 08. Apr 2011) $
 *
 * \version $Revision: 1.06 $
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
#include "verbose.h"
#include "sthread.h"

using pnapi::PetriNet;
using pnapi::Marking;
using std::vector;
using std::map;
using std::cerr;
using std::cout;
using std::endl;
using std::setw;


	/******************************************
	* Implementation of the class Reachalyzer *
	******************************************/

namespace sara {

extern bool flag_verbose, flag_show, flag_break, flag_treemaxjob, flag_witness;
extern int val_maxdepth;
extern unsigned int maxthreads;
extern vector<SThread*> threaddata;
extern vector<Transition*> transitionorder;
extern pthread_mutex_t print_mutex, tps_mutex, fail_mutex, solv_mutex, sc_mutex;

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
Reachalyzer::Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int> cover, Problem& pb, bool verbose, int debug, bool out, int brk, bool passedon, IMatrix* im0)
	: error(0),m1(m0),net(pn),cols(pn.getTransitions().size()),lpwrap(cols),im(*im0),breakafter(brk),problem(&pb),loops(0),suboptimal(false),jsum(0),jmax(1),jloop(0),jold(0),dsum(0),dmax(1),dloop(0) {
	// inherit verbosity/debug level
	this->verbose = debug;
	this->out = out;
	im.verbose = debug;
	stateinfo = verbose;
	problemcreated = false;

	this->passedon = passedon; // if a realizability problem was infeasible we are now looking for reasons etc.
	if (passedon) torealize = pb.getVectorToRealize();

	solved = false;
	errors = false;
	maxsollen = 0;
	sumsollen = 0;

	//initialize lp_solve
	lpwrap.verbose = debug;
	if (lpwrap.createMEquation(m0,mf,cover,&pb,FALSE)<0) { 
		cerr << "sara: error: createMEquation (lpsolve-init) failed" << endl; 
		error=LPSOLVE_INIT_ERROR; return;
	}
	// prepare solving marking equation, initialize first partial solution and job list
	PartialSolution* init(new PartialSolution(m0));
	tps.push_back(init);
}

#ifdef SARALIB
/*! Constructor for API-like purposes.
	\param pn The Petri net.
	\param m0 The initial marking.
	\param mf The final marking.
	\param cover The set of places where the final marking need only be covered (instead of
			reached exactly).
*/
Reachalyzer::Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int> cover)
	: error(0),m1(m0),net(pn),cols(pn.getTransitions().size()),lpwrap(cols),im(pn),breakafter(0),loops(0),suboptimal(false),jsum(0),jmax(1),jloop(0),jold(0),dsum(0),dmax(1),dloop(0) {
	this->verbose = 0;
	this->out = false;
	im.verbose = 0;
	stateinfo = false;
	this->passedon = false;
	solved = false;
	errors = false;
	maxsollen = 0;
	sumsollen = 0;

	problemcreated = true;
	problem = new Problem();
	problem->setFilename("sara_inline");
	problem->setPetriNet(pn);

	//initialize lp_solve
	lpwrap.verbose = 0;
	if (lpwrap.createMEquation(m0,mf,cover,NULL,FALSE)<0) { 
		cerr << "sara: error: createMEquation (lpsolve-init) failed" << endl; 
		error=LPSOLVE_INIT_ERROR; return;
	}

	// prepare solving marking equation, initialize first partial solution and job list
	PartialSolution* init(new PartialSolution(m0));
	tps.push_back(init);
}
#endif

/*! Destructor. */
Reachalyzer::~Reachalyzer() { 
	if (problemcreated) delete problem; 
}

/*! This method is called to analyze the given reachability problem. */
void Reachalyzer::start() {
	//start counting time here
	starttime = clock();

	solved = false; // not solved yet
	errors = false; // no errors yet
	loops = 0; // counter for number of loops (jobs)
	if (stateinfo && out) cout << "JOBS(done/open):";

	// main loop, go through the job list until emptied or a solution is found
	while (!solved && !tps.empty()) // if --continue is specified, solved will never be set
	{
		if (breakafter>0 && breakafter<=loops) break; // debug option --break
		doSingleJob(0);
	}

	if (flag_verbose || flag_show)
		failure.append(unknown); // the unknown are known now: they belong to the counterexample

	// the following paragraph prints all kinds of status information
	if (stateinfo && out) cout << "\r"; // if on screen
	if (flag_break && stateinfo) // debug output if option --break was used
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
	if (stateinfo && !passedon) // print statistics on jobqueue
		cout << "sara: JobQueue entries " << jmax << " max, " << jsum/jloop << " avg,"
		<< " positive Delta " << dmax << " max, " << dsum/dloop << " avg." << endl;

	//stop counting time here
	endtime = clock();
}

void Reachalyzer::doSingleJob(unsigned int threadID) {
		if (breakafter>0 && breakafter<=loops) return; // debug option --break

		if (stateinfo && out) { // overwrite job info if stdout (screen) is used
			pthread_mutex_lock(&print_mutex);
			cout << setw(7) << loops << "/" << setw(7) << tps.size() << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"; 
			cout.flush();
			pthread_mutex_unlock(&print_mutex);
		} 

		if (verbose>0) { // debug output, show active job
			pthread_mutex_lock(&print_mutex);
			cerr << endl;
			cerr << "sara: [" << threadID << "] Job " << (loops) << ",   To do: " << tps.size() << endl;
			pthread_mutex_lock(&tps_mutex);
			tps.first()->show();
			pthread_mutex_unlock(&tps_mutex);
			pthread_mutex_unlock(&print_mutex);
		} 

		if (verbose>1) {
			pthread_mutex_lock(&print_mutex);
			cerr << "Lookup-Table-Size: " << shortcut.size() << endl; // debug info
			pthread_mutex_unlock(&print_mutex);
		}

		// get the active job and remove it from the queue
		pthread_mutex_lock(&tps_mutex);
		unsigned int jtmp(tps.size());
		PartialSolution* saveps(new PartialSolution(*(tps.first())));
		tps.pop_front(true); // delete the job

		// adapt avg/max number of jobs in queue and delta between steps
		pthread_mutex_unlock(&tps_mutex);
		if (jtmp>jold) {
			dsum += jtmp-jold;
			if (dmax<jtmp-jold) dmax = jtmp-jold;
			++dloop;
		}
		jold = jtmp;
		if (jold>jmax) jmax=jold;
		jsum += jold;
		++jloop;

		// Create a temporary jobqueue for new jobs evolving, put the active job into it
		JobQueue ttps(saveps);

		map<Transition*,int> oldvector(ttps.first()->getFullVector()); // the previous (father) solution of lp_solve
		nextJump(ttps,*(ttps.first())); // create a new jump if one has been precomputed, before changes are made
		ttps.first()->touchConstraints(false); // mark all normal constraints so far as old
		ttps.first()->buildSimpleConstraints(im); // compute and create new constraints
		if (!lpwrap.stripConstraints()) { // remove all constraints not belonging to the marking equation
			if (out) cerr << "\r" ;
			status("error, resize_lp failed (at job %d)",loops);
			error=LPSOLVE_RESIZE_ERROR; return; 
		}
		lpwrap.addConstraints(*(ttps.first())); // add the constraints to the marking equation
		int ret = lpwrap.solveSystem(); // now solve the system of marking equation + constraints
		if (verbose>1) cerr << "sara: lp_solve returned " << ret << endl; // debug info
		if (ret!=0 && ret!=1) { // in case we don't get a solution ...
			if (ret!=2) { if (!errors) { if (out) cerr << "\r"; 
											status("warning, failure of lp_solve (at job %d)",loops); }
							errors = true; 
			} else { // lp_solve denies a solution, this may be part of a counterexample
				set<Constraint>::iterator cit;
				for(cit=ttps.first()->getConstraints().begin(); cit!=ttps.first()->getConstraints().end(); ++cit)
					if ((*cit).isRecent() && (*cit).isJump()) break; // newly introduced jumps cannot lead to counterexamples
				if (cit==ttps.first()->getConstraints().end()) { // now it's not a recent jump ...
					PartialSolution* fps(new PartialSolution(*(ttps.first()))); // copy the old job
					fps->setFeasibility(false); // state that we have no solution and thus the marking equation might be infeasible
					if (verbose>1) { // debug info
						pthread_mutex_lock(&print_mutex);
						cerr << "sara: [" << threadID << "] Failure:" << endl;
						fps->show();
						cerr << "*** NOSOLUTION ***" << endl;
						pthread_mutex_unlock(&print_mutex);
					}
					pthread_mutex_lock(&fail_mutex);
					failure.push_fail(fps); // record this part of a counterexample
					pthread_mutex_unlock(&fail_mutex);
				}
			}
			ttps.pop_front(false); // delete this job
			++loops; // count it, but ...
			pthread_mutex_lock(&tps_mutex);
			tps.transfer(ttps);
			pthread_mutex_unlock(&tps_mutex);
			return; // do not try to realize a firing sequence or create child jobs as lp_solve gave no solution
		}
		if (ret==1) suboptimal=true; // solutions might be lost!!!
		ttps.first()->touchConstraints(true); // mark all jump constraints so far as old
		// calculate what the new constraints changed in the solution of lp_solve
		map<Transition*,int> fullvector(lpwrap.getTVector(net));
		// check if the solution is longer than the parameter -M (if given), if so, omit it
		if (checkSize(fullvector)) { 
			ttps.pop_front(false); 
			++loops; 
			pthread_mutex_lock(&tps_mutex);
			tps.transfer(ttps);
			pthread_mutex_unlock(&tps_mutex);
			return;
		}
		// delete all "unknown" jobs covered by this job if we are interested in a counterexample
		if (flag_verbose || flag_show) {
			pthread_mutex_lock(&fail_mutex);
			unknown.deleteCovered(fullvector); 
			pthread_mutex_unlock(&fail_mutex);
		}
		map<Transition*,int> diff; // this contains changes from the old to the new solution
		map<Transition*,int>::iterator vit;
		for(vit=fullvector.begin(); vit!=fullvector.end(); ++vit)
			if (vit->second>oldvector[vit->first]) // if the solution has grown here
				diff[vit->first] = vit->second; // note the new value (not the difference!)
		if (!ttps.first()->getRemains().empty()) // if the active job is not a full solution
		{
			if (verbose>1) { // debug info
				map<Transition*,int>::iterator mit;
				pthread_mutex_lock(&print_mutex);
				cerr << "New-Old Vector: ";
				for(mit=fullvector.begin(); mit!=fullvector.end(); ++mit)
					if (mit->second>oldvector[mit->first])
						cerr << mit->first->getName() << ":" << mit->second-oldvector[mit->first] << " ";
				cerr << endl;
				pthread_mutex_unlock(&print_mutex);
			}
			if (oldvector==fullvector)
			{ // if lp_solve didn't increase the solution, we have a portion of a counterexample
				// this should only happen if no transitions are enabled at all
				PartialSolution* fps(new PartialSolution(*(ttps.first()))); // copy the job
				fps->setFeasibility(true); // there was a solution, the marking equation is feasible
				if (verbose>1) { // debug info
					pthread_mutex_lock(&print_mutex);
					cerr << "sara: Failure:" << endl;
					fps->show();
					cerr << "*** EQUAL ***" << endl;
					pthread_mutex_unlock(&print_mutex);
				}
				pthread_mutex_lock(&fail_mutex);
				failure.push_fail(fps); // record the counterexample
				pthread_mutex_unlock(&fail_mutex);
				ttps.pop_front(false); // and delete the job
				++loops; // count the job
				pthread_mutex_lock(&tps_mutex);
				tps.transfer(ttps);
				pthread_mutex_unlock(&tps_mutex);
				return; // do not try to find a realization
			} else if (flag_verbose || flag_show) { // only if looking for counterexamples
				bool ngeq(false); // if the new solution is not greater or equal to the old one
				for(vit=oldvector.begin(); vit!=oldvector.end(); ++vit)
					if (vit->second>fullvector[vit->first]) ngeq=true;
				if (ngeq) { // unclear if the old solution can be pumped up or just "pumped" sideways
					PartialSolution* ups(new PartialSolution(*(ttps.first())));
					pthread_mutex_lock(&fail_mutex);
					unknown.push_back(ups); 
					pthread_mutex_unlock(&fail_mutex);
				}
			}
		}

		// make a copy of the active job before we change it
		PartialSolution ps(*(ttps.first()));
		ttps.first()->transformJumps(fullvector); // change jump constraints to normal ones
		PartialSolution* comp(NULL);

		if (tps.findPast(ttps.first(),NULL)) // check if the transformed job has already been done before
		{ // i.e. if there is a previous job with the same constraints
			ttps.pop_front(true); // pop_front, but don't put it in past list (it's there already)
			++loops; // count the job
			pthread_mutex_lock(&tps_mutex);
			tps.transfer(ttps);
			pthread_mutex_unlock(&tps_mutex);
			return; // do not do anything with it
		}   

		if (ret<2 && !diff.empty()) 
		{	// if the solution has changed we may make a jump next, so create such a job
			ps.touchConstraints(true); // mark all jump constraints so far as old
			createJumps(ttps,diff,ps); // precompute possible jumps and create one job for these jumps
		}

		// now push the job into the global past (but we are not fully done yet)
		pthread_mutex_lock(&tps_mutex);
		comp = new PartialSolution(*(ttps.first())); // we forward this to PathFinder for checks against the past of tps
		tps.push_past(comp);
		pthread_mutex_unlock(&tps_mutex);
		
		if (!solutionSeen(ttps,comp,fullvector)) // adapt known solutions (from an earlier loop) for the new constraints
		{ // no solutions known so far, calculate them by trying to realize a firing sequence
				PathFinder pf(m1,fullvector,tps,ttps,solutions,failure,im,shortcut,comp);
				pf.verbose = verbose; //((breakafter>0 && breakafter<=loops+2)?verbose+1:verbose);
				pf.setMinimize(); // do not allow repeating markings in firing sequences
				pf.passedon = passedon;
				if (passedon) pf.torealize = torealize;
				int depth(0);
				for(map<Transition*,int>::iterator mit=fullvector.begin(); mit!=fullvector.end(); ++mit) depth+=mit->second;
//				pf.maxforkdepth = depth*(maxthreads+1)/500; // 600 is a guess
//				pf.limitdepth = pf.maxforkdepth+1;
				initThread(0,ttps.first(),m1,fullvector,pf);
				solved = pf.recurse(0); // main call to realize a solution
				solved = pf.waitForThreads(0,solved); // wait for the helper threads (or cancel them in case of a solution)
		} 
		ttps.pop_front(false); // we are through with this job, move it to the past jobs list
		++loops; // count the job
		pthread_mutex_lock(&tps_mutex);
		tps.transfer(ttps);
		pthread_mutex_unlock(&tps_mutex);
}

/** Print the results of the reachability analysis. 
	@param pbnr An identifying number for the problem instance for use in filenames
*/
void Reachalyzer::printResult(int pbnr) {
	// print all solutions first and find the longest solution length
	if (!solutions.almostEmpty()) 
	{
		if (passedon) cout << "sara: A shorter realizable firing sequence with the same token effect would be:" << endl;
		maxsollen = solutions.printSolutions(sumsollen,*problem,pbnr);
	}
	else if (errors) cout << "sara: UNSOLVED: Result is indecisive due to failure of lp_solve." << endl;
	else if (suboptimal) cout << "sara: UNSOLVED: Result is indecisive due to non-minimal solutions by lp_solve." << endl;
	else if (flag_treemaxjob || val_maxdepth>0) cout << "sara: UNSOLVED: solutions may have been cut off due to command line switch -M/-T" << endl;
	if (!errors && !flag_treemaxjob && val_maxdepth==0 && (flag_witness || solutions.almostEmpty()))
	{ // if we have no solution or witnesses are sought anyway
		if (suboptimal) cout << "sara: The following information might therefore not be relevant." << endl;
		if (failure.trueSize()>0 && !flag_break)
		{ // if there are witnesses and --break was not specified
			if (solutions.almostEmpty() && !passedon) {
				if (!suboptimal) {
					cout << "sara: INFEASIBLE: "; // check why there is no solution:
					if (failure.checkMEInfeasible() && solutions.almostEmpty() && loops<=1)
					{ // it might be that the initial marking equation has no solution
						cout << "the marking equation is infeasible." << endl; 
						if (stateinfo) if (!lpwrap.findNearest())
							cout << "sara: warning: unable to suggest changes, probably a failure of lp_solve" << endl;
						return; 
					}
					// or the marking equation is feasible but still we cannot reach a solution 
					cout << "unable to borrow enough tokens via T-invariants." << endl;
				}
				if (stateinfo) cout << "There are firing sequences that could not be extended towards the final marking." << endl;
			} else if (passedon && stateinfo) cout << "sara: at the following points the algorithm could not continue:" << endl;
			else if (stateinfo) cout << "sara: at the following points the algorithm needed to backtrack:" << endl;
			if (stateinfo || flag_show) failure.printFailure(im,*problem,pbnr); // then print the counterexample; the following shouldn't happen:
		} else if (solutions.almostEmpty())
			cout << "sara: UNSOLVED: Result is indecisive" << (flag_break?" due to a break.":", no counterexamples found.") << endl;
	}
}

#ifdef SARALIB
/** Get the solution of the reachability analysis.
	@return A firing sequence. If none is found (no solution), a vector containing a single NULL pointer is returned. 
*/
vector<Transition*> Reachalyzer::getOneSolution() {
	return solutions.getOneSolution();
}
#endif

/*! Calculate the amount of CPU time used for solving the problem.
	\return The CPU time used.
*/
clock_t Reachalyzer::getTime() const { return (endtime-starttime); }

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
	if (suboptimal) return SOLUTIONS_LOST;
	if (!failure.almostEmpty()) return COUNTEREXAMPLE_FOUND;
	return UNSOLVED;	
}

/*! Check whether a solution of lp_solve has been found earlier. If this happened, use that
	former solution to create the necessary new jobs without again trying to find realizations
	of the solution in form of firing sequences, using a lookup table.
	\param tv The new? solution.
	\return If the solution has been calculated before.
*/
bool Reachalyzer::solutionSeen(JobQueue& ttps, PartialSolution* excl, map<Transition*,int>& tv) {
	pthread_mutex_lock(&sc_mutex);
	if (shortcut.find(tv)==shortcut.end()) {
		pthread_mutex_unlock(&sc_mutex);
		return false; // it's not in the lookup table
	}
	vector<PartialSolution>& vp(shortcut[tv]); // job list from the lookup table
	for(unsigned int i=0; i<vp.size(); ++i) // go through the jobs
	{
		vp[i].setConstraints(ttps.first()->getConstraints()); // set the constraints to match our new problem
		pthread_mutex_lock(&tps_mutex);
		if (tps.findPast(&(vp[i]),excl)) {
			pthread_mutex_unlock(&tps_mutex);
			continue;
		}
		pthread_mutex_unlock(&tps_mutex);
		
		if (ttps.find(&(vp[i]))>=0)
		{
			PartialSolution* cp(new PartialSolution(vp[i])); // make a copy of the job
			ttps.push_back(cp); // and insert it into the job list
			if (verbose>1) { // debug info
				pthread_mutex_lock(&print_mutex);
				cerr << "sara: Seen Partial Solution:" << endl;
				cp->show();
				cerr << "*** LOOKUP ***" << endl;
				pthread_mutex_unlock(&print_mutex);
			}
		}
	}
	pthread_mutex_unlock(&sc_mutex);
	return true; // the solution is in the lookup table (but new jobs are not necessarily created)
}

/*! Precompute a list of jumps to be done. Create a job for the first one.
	\param diff The values in the new solution that are greater than the old (father) solution.
	\param ps The job on which the jumps are to be based.
*/
void Reachalyzer::createJumps(JobQueue& ttps, map<Transition*,int>& diff, PartialSolution& ps) {
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
	pthread_mutex_lock(&tps_mutex);
	if (!tps.findPast(nps,NULL) && ttps.find(nps)>=0)
	{ // if the new job has not been done before and wasn't planned before
		pthread_mutex_unlock(&tps_mutex);
		ttps.push_back(nps); // put the new job into the list
		if (verbose>1) { // debug info
			pthread_mutex_lock(&print_mutex);
			cerr << "sara: New Partial Solution:" << endl;
			nps->show();
			cerr << "*** CREATE JUMPS ***" << endl;
			pthread_mutex_unlock(&print_mutex);
		}
	} else { // if this jump shouldn't be done
		pthread_mutex_unlock(&tps_mutex);
		nextJump(ttps,*nps); // move on the next in our list
		delete nps; // and delete this one
	} 
}

/*! Create a job for the next precomputed jump.
	\param ps The job for the former jump containing the precomputed jump list.
*/
void Reachalyzer::nextJump(JobQueue& ttps, PartialSolution ps) {
	int val=0;
	Transition* t(ps.getNextJC(val)); // get the next alternative jump
	if (t==NULL) return; // [none available]
	ps.popJC(); // and remove it from the list

	// first remove the old jump constraint (which is still tagged as recent)
	set<Constraint>& cs(ps.getConstraints());
	set<Constraint>::iterator cit;
	for(cit=cs.begin(); cit!=cs.end(); ++cit)
		if (cit->isRecent() && cit->isJump()) { cs.erase(cit); break; }

	// then add the new one and create the according job
	map<Transition*,int> tmp;
	tmp[t] = val;
	Constraint c(tmp,true); // build constraint to forbid the increase, so another minimal solution will be sought
	c.setRecent(true); // tag the new jump as recent
	ps.setConstraint(c); // add it to the job
	pthread_mutex_lock(&tps_mutex);
	if (!tps.findPast(&ps,NULL) && ttps.find(&ps)>=0)
	{ // if it hasn't been done and wasn't planned so far
		pthread_mutex_unlock(&tps_mutex);
		ttps.push_back(new PartialSolution(ps)); // put the new job into the list
		if (verbose>1) { // debug info
			pthread_mutex_lock(&print_mutex);
			cerr << "sara: New Partial Solution:" << endl;
			ps.show();
			cerr << "*** NEXT JUMP ***" << endl;
			pthread_mutex_unlock(&print_mutex);
		}
	} else { // otherwise:
		pthread_mutex_unlock(&tps_mutex);
		nextJump(ttps,ps); // and move on to the next one in the list
	} 
}

/*! Get the recorded length of the longest solving firing sequence, if one has been found.
	\return The length.
*/
int Reachalyzer::getMaxTraceLength() const { return maxsollen; }

/*! Get the recorded sum of the lengths of the solving firing sequences.
	\return The sum.
*/
int Reachalyzer::getSumTraceLength() const { return sumsollen; }

/** Get the number of solutions in the solutions queue.
	@return Number of solutions.
*/
int Reachalyzer::numberOfSolutions() { return solutions.size(); }

bool Reachalyzer::checkSize(map<Transition*,int>& solution) {
	if (val_maxdepth==0) return false;
	map<Transition*,int>::iterator mit;
	int val(0);
	for(mit=solution.begin(); mit!=solution.end(); ++mit)
		val += mit->second;
	return (val_maxdepth<val);
}

} // end namespace sara

