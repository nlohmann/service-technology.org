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
 * \date    $Date: 2012-06-22 12:00:00 +0200 (Fr, 22. Jun 2012) $
 *
 * \version $Revision: 1.10 $
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

extern bool flag_verbose, flag_show, flag_break, flag_treemaxjob, flag_witness, flag_parallel_jobs;
extern int val_maxdepth;
extern unsigned int maxthreads, cntthreads;
extern set<unsigned int> idleID;
extern vector<SThread*> threaddata;
extern vector<Transition*> transitionorder;
extern pthread_mutex_t print_mutex, tps_mutex, fail_mutex, solv_mutex, sc_mutex, reach_mutex, main_mutex;
extern pthread_cond_t main_cond;

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
	\param im0 The indicence matrix for the Petri net.
*/
Reachalyzer::Reachalyzer(PetriNet& pn, Marking& m0, Marking& mf, map<Place*,int> cover, Problem& pb, bool verbose, int debug, bool out, int brk, bool passedon, IMatrix* im0)
	: error(0),m1(m0),net(pn),cols(pn.getTransitions().size()),lpwrap(cols),im(*im0),breakafter(brk),problem(&pb),loops(0),suboptimal(false),jsum(0),jmax(1),jloop(0),jold(0),dsum(0),dmax(1),dloop(0),recstepsum(0) {
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

	//save pointer to reachalyzer for threads
	threaddata[0]->r = this;

	//initialize lp_solve
	lpwrap.verbose = debug;
	if (lpwrap.createMEquation(m0,mf,cover,&pb,FALSE)<0) { 
		cerr << "sara: error: createMEquation (lpsolve-init) failed" << endl; 
		error=LPSOLVE_INIT_ERROR; return;
	}
	// make LPWrapper available for copying to other threads
	threaddata[0]->lp = &lpwrap; 

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
	: error(0),m1(m0),net(pn),cols(pn.getTransitions().size()),lpwrap(cols),im(pn),breakafter(0),loops(0),suboptimal(false),jsum(0),jmax(1),jloop(0),jold(0),dsum(0),dmax(1),dloop(0),recstepsum(0) {
	this->verbose = 0;
	this->out = false;
	im.verbose = 0;
	stateinfo = false;
	this->passedon = false;
	solved = false;
	errors = false;
	maxsollen = 0;
	sumsollen = 0;

	//save pointer to reachalyzer for threads
	threaddata[0]->r = this;

	problemcreated = true;
	problem = new Problem();
	problem->setFilename("sara_inline");
	problem->setPetriNet(pn);

	//initialize lp_solve / LPWrapper
	lpwrap.verbose = 0;
	if (lpwrap.createMEquation(m0,mf,cover,NULL,FALSE)<0) { 
		cerr << "sara: error: createMEquation (lpsolve-init) failed" << endl; 
		error=LPSOLVE_INIT_ERROR; return;
	}
	// make LPWrapper available for copying to other threads
	threaddata[0]->lp = &lpwrap; 

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
	bool done(false);
	while (!solved && !done) 
		// if --continue is specified, solved will never be set
	{
		if (breakafter>0 && breakafter<=loops) break; // debug option --break
		PartialSolution* ps(getSingleJob(0));
		assignJobHelper(0);
		if (ps) doSingleJob(0,ps);

		// check if there are more jobs to do
		pthread_mutex_lock(&main_mutex);
		done = (idleID.size()==maxthreads);
		pthread_mutex_lock(&tps_mutex);
		bool done2(true);
		if (!tps.empty()) { done = false; done2 = false; }
		pthread_mutex_unlock(&tps_mutex);
		if (!solved && !done && done2) pthread_cond_wait(&main_cond,&main_mutex);
		pthread_mutex_unlock(&main_mutex);
	}
	waitForAllIdle(verbose);

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

PartialSolution* Reachalyzer::getSingleJob(unsigned int threadID) {
		if (breakafter>0 && breakafter<=loops) return NULL; // debug option --break

		// get the active job and remove it from the queue
		pthread_mutex_lock(&tps_mutex);
		unsigned int jtmp(tps.size());
		PartialSolution* ps(tps.pop_front());
		pthread_mutex_unlock(&tps_mutex);
		if (!ps) return NULL; // if there is no active job, we can't do anything

		if (stateinfo && out && isatty(fileno(stdout))) { // overwrite job info if stdout (screen) is used
			pthread_mutex_lock(&print_mutex);
			cout << setw(7) << loops << "/" << setw(7) << jtmp << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"; 
			cout.flush();
			pthread_mutex_unlock(&print_mutex);
		} 

		if (verbose>0) { // debug output, show active job
			pthread_mutex_lock(&print_mutex);
			cerr << endl;
			cerr << "sara: [" << threadID << "] Job " << (loops) << ",   To do: " << jtmp << endl;
			ps->show();
			if (verbose>1)
				cerr << "Lookup-Table-Size: " << shortcut.size() << endl; // debug info
			pthread_mutex_unlock(&print_mutex);
		} 

		// adapt avg/max number of jobs in queue and delta between steps
		pthread_mutex_lock(&reach_mutex);
		if (jtmp>jold) {
			dsum += jtmp-jold;
			if (dmax<jtmp-jold) dmax = jtmp-jold;
			++dloop;
		}
		jold = jtmp;
		if (jold>jmax) jmax=jold;
		jsum += jold;
		++jloop;
		pthread_mutex_unlock(&reach_mutex);

		return ps;
}

void Reachalyzer::doSingleJob(unsigned int threadID, PartialSolution* ps) {
		// Get a pointer to the thread's data
		SThread* thr(threaddata[threadID]);

		// Create a temporary jobqueue for new jobs evolving, put the active job into it
		JobQueue ttps(ps);

		map<Transition*,int> oldvector(ps->getFullVector()); // the previous (father) solution of lp_solve
		nextJump(ttps,new PartialSolution(*ps)); // create a new jump if one has been precomputed, before changes are made
		ps->touchConstraints(false); // mark all normal constraints so far as old
		ps->buildSimpleConstraints(im); // compute and create new constraints
		if (!thr->lp->stripConstraints()) { // remove all constraints not belonging to the marking equation
			if (out) cerr << "\r" ;
			status("error, resize_lp failed (at job %d)",loops);
			error=LPSOLVE_RESIZE_ERROR; return; 
		}
		thr->lp->addConstraints(*ps); // add the constraints to the marking equation
		int ret = thr->lp->solveSystem(); // now solve the system of marking equation + constraints
		if (verbose>1) {
			pthread_mutex_lock(&print_mutex);
			cerr << "sara: [" << threadID << "] lp_solve returned " << ret << endl; // debug info
			pthread_mutex_unlock(&print_mutex);
		}

		// in case we did not get a solution:
		if (ret!=0 && ret!=1) {
			if (ret!=2) { if (!errors) { if (out) cerr << "\r"; 
											status("warning, failure of lp_solve (at job %d)",loops); }
							errors = true; // print the warning once only
			} else if (!ps->isExtendable()) { // lp_solve denies a solution, this may be part of a counterexample
				set<Constraint>::iterator cit;
				// newly introduced jumps cannot lead to counterexamples, so check for them
				for(cit=ps->getConstraints().begin(); cit!=ps->getConstraints().end(); ++cit)
					if ((*cit).isRecent() && (*cit).isJump()) break; 
				if (cit==ps->getConstraints().end()) { // now there is not a recent jump ...
					PartialSolution* fps(new PartialSolution(*ps)); // copy the old job
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
			transferJobs(ttps,false);
			return; // do not try to realize a firing sequence or create child jobs as lp_solve gave no solution
		}

		if (ret==1) suboptimal=true; // solutions might be lost!!!
		ps->touchConstraints(true); // mark all jump constraints so far as old
		// calculate what the new constraints changed in the solution of lp_solve
		map<Transition*,int> fullvector(thr->lp->getTVector(net));

		// check if the solution is longer than the parameter -M (if given), if so, omit it
		if (checkSize(fullvector)) { 
			transferJobs(ttps,false);
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
		if (!ps->getRemains().empty()) // if the active job is not a full solution
		{
			if (verbose>1) { // debug info
				map<Transition*,int>::iterator mit;
				pthread_mutex_lock(&print_mutex);
				cerr << "[" << threadID << "] New-Old Vector: ";
				for(mit=fullvector.begin(); mit!=fullvector.end(); ++mit)
					if (mit->second>oldvector[mit->first])
						cerr << mit->first->getName() << ":" << mit->second-oldvector[mit->first] << " ";
				cerr << endl;
				pthread_mutex_unlock(&print_mutex);
			}
			if (oldvector==fullvector)
			{ // if lp_solve didn't increase the solution, we have a portion of a counterexample
				// this should only happen if no transitions are enabled at all
				PartialSolution* fps(new PartialSolution(*ps)); // copy the job
				fps->setFeasibility(true); // there was a solution, the marking equation is feasible
				if (verbose>1) { // debug info
					pthread_mutex_lock(&print_mutex);
					cerr << "sara: [" << threadID << "] Failure:" << endl;
					fps->show();
					cerr << "*** EQUAL ***" << endl;
					pthread_mutex_unlock(&print_mutex);
				}
				pthread_mutex_lock(&fail_mutex);
				failure.push_fail(fps); // record the counterexample
				pthread_mutex_unlock(&fail_mutex);
				transferJobs(ttps,false);
				return; // do not try to find a realization
			} else if (flag_verbose || flag_show) { // only if looking for counterexamples
				bool ngeq(false); // if the new solution is not greater or equal to the old one
				for(vit=oldvector.begin(); vit!=oldvector.end(); ++vit)
					if (vit->second>fullvector[vit->first]) ngeq=true;
				if (ngeq) { // unclear if the old solution can be pumped up or just "pumped" sideways
					PartialSolution* ups(new PartialSolution(*ps));
					pthread_mutex_lock(&fail_mutex);
					unknown.push_back(ups); 
					pthread_mutex_unlock(&fail_mutex);
				}
			}
		}

		// make a copy of the active job before we change it
		PartialSolution* cjps(new PartialSolution(*ps));
		ps->transformJumps(fullvector); // change jump constraints to normal ones

		if (tps.findPast(ps,NULL)) // check if the transformed job has already been done before
		{ // i.e. if there is a previous job with the same constraints
			delete cjps;
			transferJobs(ttps,true);
			return; // do not do anything with it
		}   

		if (!diff.empty()) 
		{	// if the solution has changed we may make a jump next, so create such a job
			cjps->touchConstraints(true); // mark all jump constraints so far as old
			createJumps(ttps,diff,cjps); // precompute possible jumps and create one job for these jumps
		}

		// now push the job into the global past (but we are not fully done yet)
		pthread_mutex_lock(&tps_mutex);
		PartialSolution* comp(new PartialSolution(*ps)); // we forward this to PathFinder for checks against the past of tps
		tps.push_past(comp);
		pthread_mutex_unlock(&tps_mutex);
		
		if (!solutionSeen(ttps,comp,fullvector)) // adapt known solutions (from an earlier loop) for the new constraints
		{ // no solutions known so far, calculate them by trying to realize a firing sequence
				PathFinder pf(m1,fullvector,tps,ttps,solutions,failure,im,shortcut,comp);
				pf.verbose = verbose; //((breakafter>0 && breakafter<=loops+2)?verbose+1:verbose);
				pf.setMinimize(); // do not allow repeating markings in firing sequences
				pf.passedon = passedon;
				if (passedon) pf.torealize = torealize;
				initPathFinderThread(threadID,ps,m1,fullvector,pf);
				bool issolved(pf.recurse(threadID)); // main call to realize a solution
				issolved = pf.waitForThreads(threadID,issolved); // wait for the helper threads (or cancel them in case of a solution)
				if (issolved) {
					pthread_mutex_lock(&main_mutex);
					solved = true;
					if (threadID>0) pthread_cond_signal(&main_cond); // notify the main process (if it's not us)
					pthread_mutex_unlock(&main_mutex);
				}
				adaptNumberOfJobbers(pf);
		} 
		transferJobs(ttps,true);
}

/** Print the results of the reachability analysis. 
	@param pbnr An identifying number for the problem instance for use in filenames
	@param json The JSON object to which the result will be written.
*/
void Reachalyzer::printResult(int pbnr, JSON& json) {
	// print all solutions first and find the longest solution length
	if (!solutions.almostEmpty()) 
	{
		if (passedon) cout << "sara: A shorter realizable firing sequence with the same token effect would be:" << endl;
		maxsollen = solutions.printSolutions(sumsollen,*problem,pbnr,json);
	} else if (errors) {
		cout << "sara: UNSOLVED: Result is indecisive due to failure of lp_solve." << endl;
		json[problem->getName()]["UNSOLVED"] = "failure of lp_solve";
	} else if (suboptimal) {
		cout << "sara: UNSOLVED: Result is indecisive due to non-minimal solutions by lp_solve." << endl;
		json[problem->getName()]["UNSOLVED"] = "non-minimal solutions of lp_solve";
	} else if (flag_treemaxjob || val_maxdepth>0) {
		cout << "sara: UNSOLVED: solutions may have been cut off due to command line switch -M/-T" << endl;
		json[problem->getName()]["UNSOLVED"] = "cut off due to command line option -M/-T";
	}
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
						json[problem->getName()]["INFEASIBLE"] = "marking equation has no solution";
						if (stateinfo) if (!lpwrap.findNearest())
							cout << "sara: warning: unable to suggest changes, probably a failure of lp_solve" << endl;
						return; 
					}
					// or the marking equation is feasible but still we cannot reach a solution 
					cout << "unable to borrow enough tokens via T-invariants." << endl;
					json[problem->getName()]["INFEASIBLE"] = "borrowing of tokens via T-invariants failed";
				}
				if (stateinfo) cout << "There are firing sequences that could not be extended towards the final marking." << endl;
			} else if (passedon && stateinfo) cout << "sara: at the following points the algorithm could not continue:" << endl;
			else if (stateinfo) cout << "sara: at the following points the algorithm needed to backtrack:" << endl;
			if (stateinfo || flag_show) failure.printFailure(im,*problem,pbnr,json); // then print the counterexample; the following shouldn't happen:
		} else if (solutions.almostEmpty()) {
			cout << "sara: UNSOLVED: Result is indecisive" << (flag_break?" due to a break.":", no counterexamples found.") << endl;
			json[problem->getName()]["UNSOLVED"] = (flag_break?"due to a break":"no counterexamples found");
		}
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
	\param ttps A thread-local job queue having the new partial solution as its first entry.
	\param excl A former partial solution excluded from the comparison (usually father node)
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
	\param ttps A thread-local job queue having the new partial solution as its first entry.
	\param diff The values in the new solution that are greater than the old (father) solution.
	\param ps The job on which the jumps are to be based.
*/
void Reachalyzer::createJumps(JobQueue& ttps, map<Transition*,int>& diff, PartialSolution* ps) {
	ps->setJC(diff); // make all jump alternatives available in a list for future use
	int val=0;
	Transition* t(ps->getNextJC(val)); // get the first alternative
	if (t==NULL) { delete ps; return; } // [none available]
	ps->popJC(); // and remove it from the list
	// now create the according job
	map<Transition*,int> tmp;
	tmp[t] = val;
	Constraint c(tmp,true); // build constraint to forbid the increase, so another minimal solution will be sought
	c.setRecent(true); // tag the constraint as new
	ps->setConstraint(c); // add it to the job
	pthread_mutex_lock(&tps_mutex);
	if (!tps.findPast(ps,NULL) && ttps.find(ps)>=0)
	{ // if the new job has not been done before and wasn't planned before
		pthread_mutex_unlock(&tps_mutex);
		ttps.push_back(ps); // put the new job into the list
		if (verbose>1) { // debug info
			pthread_mutex_lock(&print_mutex);
			cerr << "sara: New Partial Solution:" << endl;
			ps->show();
			cerr << "*** CREATE JUMPS ***" << endl;
			pthread_mutex_unlock(&print_mutex);
		}
	} else { // if this jump shouldn't be done
		pthread_mutex_unlock(&tps_mutex);
		nextJump(ttps,ps); // move on the next in our list
	} 
}

/*! Create a job for the next precomputed jump.
	\param ttps A thread-local job queue having the new partial solution as its first entry.
	\param ps The job for the former jump containing the precomputed jump list.
*/
void Reachalyzer::nextJump(JobQueue& ttps, PartialSolution* ps) {
	int val(0);
	Transition* t(ps->getNextJC(val)); // get the next alternative jump
	if (t==NULL) { delete ps; return; } // [none available]
	ps->popJC(); // and remove it from the list

	// first remove the old jump constraint (which is still tagged as recent)
	set<Constraint>& cs(ps->getConstraints());
	set<Constraint>::iterator cit;
	for(cit=cs.begin(); cit!=cs.end(); ++cit)
		if (cit->isRecent() && cit->isJump()) { cs.erase(cit); break; }

	// then add the new one and create the according job
	map<Transition*,int> tmp;
	tmp[t] = val;
	Constraint c(tmp,true); // build constraint to forbid the increase, so another minimal solution will be sought
	c.setRecent(true); // tag the new jump as recent
	ps->setConstraint(c); // add it to the job
	pthread_mutex_lock(&tps_mutex);
	if (!tps.findPast(ps,NULL) && ttps.find(ps)>=0)
	{ // if it hasn't been done and wasn't planned so far
		pthread_mutex_unlock(&tps_mutex);
		ttps.push_back(ps); // put the new job into the list
		if (verbose>1) { // debug info
			pthread_mutex_lock(&print_mutex);
			cerr << "sara: New Partial Solution:" << endl;
			ps->show();
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

/** Transfer the given Jobqueue to this Reachalyzers main JobQueue.
	@param ttps The Jobqueue to transfer.
	@param killactive If the active job should be killed or pushed to the past.
*/
void Reachalyzer::transferJobs(JobQueue& ttps, bool killactive) {
			pthread_mutex_lock(&tps_mutex);

			// delete the active job (true) or push it to the past (false)
			PartialSolution* ps(ttps.pop_front()); 

			// count the job
			++loops; 

			// do the transfer
//			pthread_mutex_lock(&tps_mutex);
			if (killactive) delete ps; 
			else tps.push_past(ps);
			tps.transfer(ttps);
			pthread_mutex_unlock(&tps_mutex);
}

/** Adapt the number of threads that can do PathFinder jobs only.
	The number if computed dynamically from the number of recursion
	steps in PathFinder instances of the recent past and the numbers
	of jobs already done and in queue.
	@param pf A completed PathFinder instance.
*/
void Reachalyzer::adaptNumberOfJobbers(PathFinder& pf) {
	unsigned int pfrs(pf.getRecSteps());
	unsigned int jobs((unsigned int)jold);
	unsigned int done((unsigned int)jloop);
	if (done==0) done=1;

	recstepsum += pfrs;
	unsigned int stepsperjob(recstepsum/done);

	unsigned int shift(0);
	while (stepsperjob>0) { stepsperjob >>= 1; ++shift; }
	while (jobs>0 && shift>0) { jobs >>=1; --shift; }

	if (shift>10) shift=0; else shift = 10-shift;
	cntthreads = (maxthreads*shift/10)+1;
}

} // end namespace sara

