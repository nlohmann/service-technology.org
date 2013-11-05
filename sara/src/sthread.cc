// -*- C++ -*-

/*!
 * \file    sthread.cc
 *
 * \brief   Threading
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/04/20
 *
 * \date    $Date: 2013-11-05 12:00:00 +0200 (Di, 5. Nov 2013) $
 *
 * \version $Revision: 1.14 $
 */

#include <pthread.h>
#include <iostream>
#include "sthread.h"
#include "reachalyzer.h"
#include "pathfinder.h"
#include <vector>
#include "verbose.h"

using std::vector;
using std::cerr;
using std::endl;

namespace sara {

	/*****************************************
	* Global variables for thread handling   *
	*****************************************/

// debugging flag
extern int debug;

/// the global transition ordering, from main.cc
extern vector<Transition*> transitionorder;

/// Number of threads available
unsigned int maxthreads;

/// Number of threads in initialisation (main process will wait until zero)
unsigned int cntthreads;

/// Mutex to let the main process wait until all threads are done
pthread_mutex_t main_mutex;

/// Signal for awakening the main process when the last thread goes idle
pthread_cond_t main_cond;

/// Mutex for exclusive use of the active JobQueue
pthread_mutex_t tps_mutex;

/// Mutex for exclusive use of failure queue
pthread_mutex_t fail_mutex;

/// Mutex for exclusive use of solution queue
pthread_mutex_t solv_mutex;

/// Mutex for exclusive use of fpool
pthread_mutex_t fpool_mutex;

/// Mutex for exclusive use of shortcut
pthread_mutex_t sc_mutex;

/// Mutex for exclusive use of Reachalyzer local variables
pthread_mutex_t reach_mutex;

/// Thread data for all threads
vector<SThread*> threaddata;

/// Threads themselves
vector<pthread_t> threads;

/// Available thread IDs
vector<bool> thread_id_taken;

/// List of idle threads (IDs)
set<unsigned int> idleID;

/// For printing to screen (mainly for debugging)
pthread_mutex_t print_mutex;

/// flag showing whether there are threads at all
bool multithreaded;


	/*****************************************
	* Implementation for the thread handling *
	*****************************************/

/** Initialise the global thread related variables.
	@param max The number of additional threads.
*/
void initThreadData(int max) {

	multithreaded = (max>0);
	maxthreads=(unsigned int)(multithreaded?max:0);

	// initialise global variables
	threaddata.resize(maxthreads+1);
	threads.resize(maxthreads);
	pthread_mutex_init(&main_mutex,NULL);
	pthread_cond_init(&main_cond,NULL);
	pthread_mutex_init(&print_mutex,NULL);
	pthread_mutex_init(&tps_mutex,NULL);
	pthread_mutex_init(&fail_mutex,NULL);
	pthread_mutex_init(&solv_mutex,NULL);
	pthread_mutex_init(&fpool_mutex,NULL);
	pthread_mutex_init(&sc_mutex,NULL);
	pthread_mutex_init(&reach_mutex,NULL);
	thread_id_taken.resize(maxthreads);
}

/** Start the additional threads.
*/
void startThreads() {
	// allocate thread related space for the main process
	threaddata[0] = new SThread();
	threaddata[0]->restart = false; // init
	threaddata[0]->solved = false; // init

	// debug message
	if (debug) cerr << "[M] Starting Threads ..." << endl;

	// mark all thread IDs as to be taken
	for(unsigned int i=0; i<maxthreads; ++i)
		thread_id_taken[i]=false;

	// set the thread attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
//	pthread_attr_setstacksize(&attr,1000000);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

	// create all the threads
	pthread_mutex_lock(&main_mutex);
	for(unsigned int i=0; i<maxthreads; ++i)
	{
		int res = pthread_create(&(threads[i]),&attr,threadManagement,NULL);	
		if (debug) cerr << "[M] Thread " << i << " create returned " << res << endl; 
	}

	if (debug) cerr << "[M] All Threads initialized" << endl;

	// release thread attribute object
	pthread_attr_destroy(&attr);

	// wait until all threads have initialised
	cntthreads = maxthreads;
	while (cntthreads>0) pthread_cond_wait(&main_cond,&main_mutex);
	pthread_mutex_unlock(&main_mutex);

	// from thread no. cntthreads on the threads are reserved for PathFinder jobs
	cntthreads = maxthreads+1;
}


/** The main routine for all threads
	@param arg NULL pointer.
	@return The (internal) thread ID.
*/ 
void* threadManagement(void* arg) {
	// the thread ID
	unsigned int tid;

	// all data needed by the thread
	SThread thread;

	// local mutex and condition for locking the local data
	pthread_mutex_init(&(thread.mutex),NULL);
	pthread_cond_init(&(thread.cond),NULL);

	// get our thread ID
	pthread_mutex_lock(&(main_mutex));
	for(unsigned int i=0; i<maxthreads; ++i)
		if (!thread_id_taken[i]) { thread_id_taken[i]=true; tid=i+1; break; }
	pthread_mutex_unlock(&(main_mutex));

	// status message
	if (debug) {
		pthread_mutex_lock(&(print_mutex));
		cerr << "[" << tid << "] Thread " << tid << " started." << endl;
		pthread_mutex_unlock(&(print_mutex));
	}

	// make our data available to other threads
	threaddata[tid] = &thread;

	// initialise our local data
	// have we solved the active problem?
	thread.solved = false;
	// the inital marking
	thread.m = NULL;
	// the ID of the thread we are helping
	thread.rootID = tid;
	// which of our helpers has solved the problem
	thread.solvedHelper = -1;
	// to count the jobs done (for debug/analysis)
	thread.activity = 0;

	thread.r = NULL;
	thread.lp = NULL;
	thread.pf = NULL;
	thread.ps = NULL;

	// critical phase of initialisation: our status
	pthread_mutex_lock(&(main_mutex));
	// restart on demand?
	thread.restart = false;
	// terminate?
	thread.exit = false;
	// are we idle?
	thread.idle = true;
	// insert our ID into the array of idle threads
	idleID.insert(tid);

	// tell the main process we are done with initialisation
	--cntthreads;
	pthread_cond_signal(&main_cond);
	pthread_mutex_unlock(&(main_mutex));

	// a flag for a solved problem
	bool solved(false);

	// do forever (until we get an exit flag)
	while (true) {

		// wait until we're not idle anymore (a job, a restart, or exit)
		pthread_mutex_lock(&(thread.mutex));
		while (thread.idle && !thread.restart && !thread.exit)
			pthread_cond_wait(&(thread.cond),&(thread.mutex));
		pthread_mutex_unlock(&(thread.mutex));

		// terminate on exit signal
		if (thread.exit) break;

		// no problem solved (by us) on restart signal, no new job waiting
		if (thread.restart) {
			solved = false;
			if (thread.r) delete thread.ps;
			thread.r = NULL;
			delete thread.lp;
			thread.lp = NULL;
		} else if (thread.r) {
			// we have a full job to do
			while (thread.ps && !thread.restart && !thread.exit) {
				thread.r->assignJobHelper(tid);
				thread.r->doSingleJob(tid,thread.ps);
				thread.ps = NULL;
				if (tid>=cntthreads) break;
				thread.ps = thread.r->getSingleJob(tid);
			};
			delete thread.ps;
			thread.r = NULL;
		} else {
			// lets act as a PathFinder helper
			++thread.activity;

			// status message
			if (debug) {
				pthread_mutex_lock(&(print_mutex));
				cerr << "[" << tid << "] Starting Job " << thread.activity << endl;
				pthread_mutex_unlock(&(print_mutex));
			}

			// make sure we have enough nodes for our Tarjan graph (at least number of transitions in the net)
			allocTarjan(tid);

			// form a new job queue, with only the problem to solve in it
			thread.tps.flush();
			thread.tps.push_back(new PartialSolution(*(thread.ps)));
			
			// optimisation: the depth in the search tree downto which we may assign a helper
			// the helper may not start below this depth
			thread.tordlocsearchdepth = 0;

			// start the recursion through the search tree unless the diamond check fails immediately
			if (!thread.pf->checkForDiamonds(&thread)) // do not recurse if a diamond has just been completed
				solved = thread.pf->recurse(tid);	

			// transfer the newly found jobs to the global job queue
			thread.tps.pop_front(true);
			if (!thread.tps.empty()) {
				pthread_mutex_lock(&(threaddata[thread.rootID]->mutex));
				thread.pf->tps.transfer(thread.tps);
				pthread_mutex_unlock(&(threaddata[thread.rootID]->mutex));
				thread.tps.flush();
			}
		}

		if (thread.rootID != tid) {
			// tell our root thread that we are done helping him (and if we solved the problem)
			pthread_mutex_lock(&(threaddata[thread.rootID]->mutex));
			if (solved) threaddata[thread.rootID]->solvedHelper = tid;
			threaddata[thread.rootID]->root.erase(tid);

			// signal the root thread we are done (in case it's waiting in PathFinder::waitForThreads()) 
			pthread_cond_signal(&threaddata[thread.rootID]->cond);
			pthread_mutex_unlock(&(threaddata[thread.rootID]->mutex));
		}

		// deinitialise our job data
		pthread_mutex_lock(&(thread.mutex));
		thread.pf = NULL;
		delete thread.m;
		thread.m = NULL;
		thread.ps = NULL;
		thread.fseq.clear();
		thread.stubsets.clear();
		thread.tv.clear();
		thread.rootID = tid;
		thread.solved = (thread.restart ? false : solved);
		thread.restart = false;
		thread.idle = true;
		pthread_mutex_unlock(&(thread.mutex));

		// tell the world about our status (we're idle now)
		pthread_mutex_lock(&main_mutex);
		idleID.insert(tid);
		pthread_cond_signal(&main_cond);
		pthread_mutex_unlock(&main_mutex);

		if (debug) {
			pthread_mutex_lock(&(print_mutex));
			cerr << "[" << tid << "] Idle" << endl;
			pthread_mutex_unlock(&(print_mutex));
		}

	}

	// terminate, free some thread data
	pthread_mutex_destroy(&(thread.mutex));
	pthread_cond_destroy(&(thread.cond));
	delete thread.m;
	delete thread.lp;
	freeTarjan(tid);
	pthread_exit((void *)tid);
}

/** Initialise a root thread with a new job.
	@param rtnr The thread's ID.
	@param ps The problem to solve.
	@param m The initial marking.
	@param tv The transition vector to realize.
	@param pf An instance of the problem solver.
*/
void initPathFinderThread(unsigned int rtnr, PartialSolution* ps, Marking& m, map<Transition*,int>& tv, PathFinder& pf) {
	SThread* me(threaddata[rtnr]);
	pthread_mutex_lock(&(threaddata[rtnr]->mutex));

	// copy the initial marking
	threaddata[rtnr]->m = new Marking(m);

	// empty firing sequence so far
	threaddata[rtnr]->fseq.clear();

	// partial stubborn sets are empty at the beginning
	threaddata[rtnr]->stubptr = 0;
	threaddata[rtnr]->stubsets.clear();
	threaddata[rtnr]->stubsets.reserve(pf.tvsize+1);

	// save pointers to the other parameters
	threaddata[rtnr]->tv = tv;
	threaddata[rtnr]->ps = ps;
	threaddata[rtnr]->pf = &pf;

	// initialize the job queue with a copy of the problem
	threaddata[rtnr]->tps.flush();
	threaddata[rtnr]->tps.push_back(new PartialSolution(*ps));

	// our recursion starts at depth zero
	threaddata[rtnr]->recursedepth = 0;

	// helpers can be assigned any time
	threaddata[rtnr]->tordlocsearchdepth = pf.tvsize;

	// clear the helper structures
	for(unsigned int k=0; k<me->tord.size(); ++k) me->tord[k].clear();
	me->tord.clear();
	me->tordptr.clear();

	// we are the root thread
	threaddata[rtnr]->rootID = rtnr;
	threaddata[rtnr]->solvedHelper = -1;
	if (!threaddata[rtnr]->root.empty()) abort(56,"InitPathFinderThread: non-empty thread dir");
//	threaddata[rtnr]->root.clear();

	// reserve memory for some vectors used in the recursion
	threaddata[rtnr]->tseta.reserve(pf.im.numTransitions());
	threaddata[rtnr]->tsetb.reserve(pf.im.numTransitions());
	threaddata[rtnr]->tsets.reserve(pf.im.pMaxPreset());
	threaddata[rtnr]->tseti.reserve(pf.im.numTransitions());
	threaddata[rtnr]->tbool.reserve(pf.im.numTransitions());
	threaddata[rtnr]->tboolb.reserve(pf.im.numTransitions());
	threaddata[rtnr]->tboola.reserve(pf.im.numTransitions());
	threaddata[rtnr]->pseta.reserve(pf.im.numPlaces());
	threaddata[rtnr]->psetb.reserve(pf.im.numPlaces());
	threaddata[rtnr]->psets.reserve(pf.im.numPlaces());

	// make sure we have enough nodes in our graph for Tarjan and in the c&d table
	allocTarjan(rtnr);
	threaddata[rtnr]->cftab.resize(transitionorder.size());
	for(unsigned int i=0; i<threaddata[rtnr]->cftab.size(); ++i)
		threaddata[rtnr]->cftab[i].reserve(pf.tvsize);

	// we're not idle anymore
	threaddata[rtnr]->idle = false;

	pthread_mutex_unlock(&(threaddata[rtnr]->mutex));
}

/** Find a thread to help with this thread's job.
	@param tid The thread ID of the thread looking for help.
	@return If help was found.
*/
bool PathFinder::assignPathFinderHelper(unsigned int tid) {
	// check if there is an idle thread
	pthread_mutex_lock(&main_mutex);
	if (idleID.empty()) {
		pthread_mutex_unlock(&main_mutex);
		return false;
	}

	// shortcut to our own thread data
	SThread* me(threaddata[tid]);

	// find a recursion level at which not all transitions of the stubborn set have been done;
	// the closer to the root the better (because the helper job will run longer)
	unsigned int tordloc(0);
	for(; tordloc<me->tord.size(); ++tordloc)
		if (me->tordptr[tordloc]+1 < me->tord[tordloc].size()) break;
	// if we are too deep don't assign a helper
	if (tordloc==me->tord.size() || tordloc>me->tordlocsearchdepth || tordloc>me->recursedepth/3) {
		pthread_mutex_unlock(&main_mutex);
		me->tordlocsearchdepth += 2;
		return false;
	}	

	// we found a helper, mark it as non-idle, and get a pointer to its data
	unsigned int helper(*(idleID.rbegin())); // use helpers reserved for the PathFinder first
	idleID.erase(helper);
	pthread_mutex_unlock(&main_mutex);
	SThread* help(threaddata[helper]);

	// we tell the helper who its working for (our root thread, need not be identical with us)
	pthread_mutex_lock(&(threaddata[me->rootID]->mutex));
	threaddata[me->rootID]->root.insert(helper);

	// set the helpers root thread, solved and idle status
	help->rootID = me->rootID;
	help->solvedHelper = -1;
	pthread_mutex_unlock(&(threaddata[me->rootID]->mutex));

	// now copy our data to the helper, from the root of the search tree to the
	// depth calculated above in tordloc. First copy the firing sequence.
	pthread_mutex_lock(&(help->mutex));
	help->fseq.reserve(me->pf->tvsize+1);
	help->fseq.assign(me->fseq.begin(),me->fseq.begin()+tordloc);
	// We choose the next transition from the stubborn set at depth tordloc to start with
	Transition* tofire(me->tord[tordloc][me->tordptr[tordloc]+1]);
	help->fseq.push_back(tofire);

	// copy the initial marking and proceed through the firing sequence
	help->m = new Marking(me->pf->m0);
	for(unsigned int i=0; i<help->fseq.size(); ++i)
		me->pf->im.successor(*(help->m),*(help->fseq[i]));

	// copy the partial stubborn sets for the diamond check
	help->stubsets.reserve(me->pf->tvsize+1);
	help->stubsets.assign(me->stubsets.begin(),me->stubsets.begin()+tordloc+1);
	for(unsigned int i=help->stubsets.back().size(); i<me->tordptr[tordloc]+1; ++i)
		help->stubsets.back().push_back(me->tord[tordloc][i]);
	help->stubptr = tordloc+1;

	// copy the full stubborn sets so our helper can assign helpers itself
	help->tord.reserve(me->pf->tvsize+1);
	help->tord.assign(me->tord.begin(),me->tord.begin()+tordloc+1);
	while (help->tord.back().back()!=tofire) help->tord.back().pop_back();

	help->tordptr.reserve(me->pf->tvsize+1);
	help->tordptr.assign(me->tordptr.begin(),me->tordptr.begin()+tordloc+1);
	help->tordptr.back() = help->tord.back().size()-1;
	++me->tordptr[tordloc];

	// calculate the remaining transition vector of our helper
	help->tv = me->pf->fulltvector;
	for(unsigned int i=0; i<help->fseq.size(); ++i) --(help->tv[help->fseq[i]]);

	// resize the c&d table
	help->cftab.resize(transitionorder.size());
	for(unsigned int i=0; i<help->cftab.size(); ++i)
		help->cftab[i].reserve(me->pf->tvsize);

	// copy additional data needed
	help->ps = me->ps;
	help->pf = me->pf;
	help->recursedepth = help->fseq.size()-1;

	// reserve memory for some vectors in the recursion if necessary
	help->tseta.reserve(me->pf->im.numTransitions());
	help->tsetb.reserve(me->pf->im.numTransitions());
	help->tsets.reserve(me->pf->im.pMaxPreset());
	help->tseti.reserve(me->pf->im.numTransitions());
	help->tbool.reserve(me->pf->im.numTransitions());
	help->tboola.reserve(me->pf->im.numTransitions());
	help->tboolb.reserve(me->pf->im.numTransitions());
	help->pseta.reserve(me->pf->im.numPlaces());
	help->psetb.reserve(me->pf->im.numPlaces());
	help->psets.reserve(me->pf->im.numPlaces());

	// signal the helper to start its job
	help->idle = false;
	pthread_cond_signal(&(help->cond));
	pthread_mutex_unlock(&(help->mutex));
	return true;
}

bool Reachalyzer::assignJobHelper(unsigned int tid) {
	// check if there are jobs at all
	SThread* me(threaddata[tid]);
	if (me->r->tps.empty()) return false;

	// check if there is an idle thread
	pthread_mutex_lock(&main_mutex);
	if (idleID.empty()) {
		pthread_mutex_unlock(&main_mutex);
		return false;
	}

	// if we find a job helper, mark it as non-idle, and get a pointer to its data
	unsigned int helper(*(idleID.begin()));
	if (helper>=cntthreads) {
		pthread_mutex_unlock(&main_mutex);
		return false;
	}
	idleID.erase(helper);
	pthread_mutex_unlock(&main_mutex);
	SThread* help(threaddata[helper]);

	PartialSolution* job(getSingleJob(helper));
	if (!job) {
		pthread_mutex_lock(&main_mutex);
		idleID.insert(helper);
		pthread_cond_signal(&main_cond);
		pthread_mutex_unlock(&main_mutex);
		return false;
	}

	// set job helper data
	pthread_mutex_lock(&(help->mutex));
	help->r = me->r;
	help->ps = job;
	if (!help->lp) help->lp = new LPWrapper(*(me->lp));

	// set the helpers root thread, solved and idle status
	help->rootID = helper;
	help->solvedHelper = -1;
	help->idle = false;

	// signal the helper to start its job
	pthread_cond_signal(&(help->cond));
	pthread_mutex_unlock(&(help->mutex));
	return true;
}

/** Signal depending threads to restart.
	@param debug Flag for status messages.
*/
void makeThreadsIdle(int debug) {
	// signal all threads (including the main process) to restart
	pthread_mutex_lock(&main_mutex);
	for(unsigned int i=0; i<=maxthreads; ++i) {
		pthread_mutex_lock(&(threaddata[i]->mutex));
		idleID.erase(i);
		threaddata[i]->restart = true;
		pthread_cond_signal(&(threaddata[i]->cond));
		pthread_mutex_unlock(&(threaddata[i]->mutex));
	}
	pthread_mutex_unlock(&main_mutex);
}


/** Wait for all threads to go idle.
	@param debug flag for status messages.
*/
void waitForAllIdle(int debug) {
	// this function must be called by the main process only

	// wait until all threads are idle
	pthread_mutex_lock(&main_mutex);
	while (idleID.size()<maxthreads)
		pthread_cond_wait(&main_cond,&main_mutex);
	pthread_mutex_unlock(&main_mutex);

	makeThreadsIdle(debug);
	threaddata[0]->restart = false;
}

/** Kill all threads.
*/
void stopThreads() {
	int **x; // dummy
	if (debug) {
		pthread_mutex_lock(&(print_mutex));
		cerr << "[M] Cancelling Thread ";
		pthread_mutex_unlock(&(print_mutex));
	}
	pthread_mutex_lock(&main_mutex);
	for(unsigned int i=0; i<maxthreads; ++i)
	{
		if (debug) { cerr << i; cerr.flush(); }
		pthread_mutex_lock(&(threaddata[i+1]->mutex));
		threaddata[i+1]->restart = true;
		threaddata[i+1]->exit = true;
		pthread_cond_signal(&(threaddata[i+1]->cond));
		pthread_mutex_unlock(&(threaddata[i+1]->mutex));
	}
	pthread_mutex_unlock(&main_mutex);
	if (debug) {
		pthread_mutex_lock(&(print_mutex));
		cerr << endl;
		cerr << "[M] Joining Thread ";
		pthread_mutex_unlock(&(print_mutex));
	}
	for(unsigned int i=0; i<maxthreads; ++i)
	{
		if (debug) { cerr << i; cerr.flush(); }
		pthread_join(threads[i],(void **)x);
	}
	cerr << endl;
}

/** Destroy all thread related data.
*/
void destroyThreadData() {
	pthread_mutex_destroy(&main_mutex);
	pthread_cond_destroy(&main_cond);
	pthread_mutex_destroy(&print_mutex);
	pthread_mutex_destroy(&tps_mutex);
	pthread_mutex_destroy(&fail_mutex);
	pthread_mutex_destroy(&solv_mutex);
	pthread_mutex_destroy(&fpool_mutex);
	pthread_mutex_destroy(&sc_mutex);
	pthread_mutex_destroy(&reach_mutex);
	freeTarjan(0);
	delete threaddata[0];
}

/** Allocate memory to hold enough nodes for a transition graph (one per transition).
	@param tid The thread allocating the memory.
*/ 
void allocTarjan(unsigned int tid) {
	// initialize tton for PathFinder
	unsigned int k(threaddata[tid]->tton.size());
	if (k<transitionorder.size())
	{
		threaddata[tid]->tton.resize(transitionorder.size());
		for(unsigned int i=k; i<transitionorder.size(); ++i)
		{
			threaddata[tid]->tton[i] = new PathFinder::myNode();
			threaddata[tid]->tton[i]->nodes.reserve(8);
		}
	}
	for(unsigned int i=0; i<transitionorder.size(); ++i)
		threaddata[tid]->tton[i]->t = transitionorder[i];
}

/** Free the memory needed by the transition graph.
	@param tid The thread freeing its memory.
*/
void freeTarjan(unsigned int tid) {
	for(unsigned int i=0; i<threaddata[tid]->tton.size(); ++i)
		delete threaddata[tid]->tton[i];
	threaddata[tid]->tton.clear();
}

} // end namespace sara
