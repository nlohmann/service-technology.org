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
 * \date    $Date: 2012-09-11 12:00:00 +0200 (Di, 11. Sep 2012) $
 *
 * \version $Revision: 1.00 $
 */

#include <pthread.h>
#include <iostream>
#include "sthread.h"
#include "formula.h"
#include "verbose.h"

using std::cerr;
using std::cout;
using std::endl;

	/*****************************************
	* Global variables for thread handling   *
	*****************************************/

// debugging flag
int debug(0);

// if any thread finds a satisfiable formula, this is set to true
bool fsolved(false);

// signal for all threads to terminate
bool texit(false);

// to notify the main process when a job is completed
extern set<unsigned int> tindex;

/// Number of threads available
unsigned int maxthreads;

/// Number of threads in initialisation (main process will wait until zero)
unsigned int cntthreads;

/// Mutex to let the main process wait until all threads are done
pthread_mutex_t main_mutex;

/// Signal for awakening the main process when the last thread goes idle
pthread_cond_t main_cond;

/// Mutex for the exclusive use of the SAT solver
pthread_mutex_t sat_mutex;

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

/// quit on solved formula
bool forcequit(true);

	/*****************************************
	* Implementation for the thread handling *
	*****************************************/

/** Initialise the global thread related variables.
	@param The number of additional threads.
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
	pthread_mutex_init(&sat_mutex,NULL);
	thread_id_taken.resize(maxthreads);
}

/** Start the additional threads.
*/
void startThreads() {
	// allocate thread related space for the main process
	threaddata[0] = new SThread();
	threaddata[0]->restart = false; // init
	threaddata[0]->solved = false; // init
//	pthread_mutex_init(&(threaddata[0]->mutex),NULL);
//	pthread_cond_init(&(threaddata[0]->cond),NULL);

	// debug message
	if (debug) cerr << "[M] Starting Threads ..." << endl;

	// mark all thread IDs as to be taken
	for(unsigned int i=0; i<maxthreads; ++i)
		thread_id_taken[i]=false;

	// set the thread attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

	// create all the threads
	pthread_mutex_lock(&main_mutex);
	for(unsigned int i=0; i<maxthreads; ++i)
	{
		int res = pthread_create(&(threads[i]),&attr,threadManagement,NULL);	
		if (debug) cerr << "[M] Thread " << i+1 << " create returned " << res << endl; 
	}

	if (debug) cerr << "[M] All Threads initialized" << endl;

	// release thread attribute object
	pthread_attr_destroy(&attr);

	// wait until all threads have initialised
	cntthreads = maxthreads;
	while (cntthreads>0) pthread_cond_wait(&main_cond,&main_mutex);
	pthread_mutex_unlock(&main_mutex);
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
	pthread_mutex_lock(&main_mutex);
	for(unsigned int i=0; i<maxthreads; ++i)
		if (!thread_id_taken[i]) { thread_id_taken[i]=true; tid=i+1; break; }
	pthread_mutex_unlock(&main_mutex);

	// status message
	if (debug) {
		pthread_mutex_lock(&print_mutex);
		cerr << "[" << tid << "] Thread " << tid << " started." << endl;
		pthread_mutex_unlock(&print_mutex);
	}

	// make our data available to other threads
	threaddata[tid] = &thread;

	// initialise our local data
	// have we solved the active problem?
	thread.solved = false;
	// to count the jobs done (for debug/analysis)
	thread.activity = 0;

	// critical phase of initialisation: our status
	pthread_mutex_lock(&main_mutex);
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
	pthread_mutex_unlock(&main_mutex);

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
		} else {
			// we have a new job
			++thread.activity;

			// status message
			if (debug) {
				pthread_mutex_lock(&(print_mutex));
				cout << "[" << tid << "] Starting Job " << thread.job << endl;
				pthread_mutex_unlock(&(print_mutex));
			}
			
			// do the job
			thread.solved = false;
			thread.s.clear();
			if (thread.ms) {
				for(unsigned int mID=0; mID<thread.mids->size(); ++mID)
				{
					if (texit) break;
					InterfaceID id2(0);
					int side(0);
					if (thread.sttype==Matchings::TTRAP) {
						side=-1;
						if (thread.ms->getTokenTrapSide(thread.mids->at(mID))) side=1;
						id2 = thread.ms->getTokenTrap(thread.mids->at(mID));
					}
					thread.inf1->computeWrappingSets(*(thread.ms),thread.id,thread.ms->getMatching(thread.mids->at(mID),thread.sttype),id2,side);
				}
				thread.ms = NULL;
			} else if (thread.sn) {
				Formula fo(*(thread.sn));
				if (!texit && fo.check(false)) { thread.solved = true; thread.s = fo.getSiphon(); }
				if (!thread.solved || !forcequit) {
					thread.st = new SiphonTrap(*(thread.sn));
					if (!texit) thread.st->computeBruteForce();
					if (!texit) texit = thread.result->computeComponentInfo(*thread.st);
				} else texit = true;
				thread.sn = NULL;
			} else {
				thread.m = new Matchings(*(thread.inf1),*(thread.inf2));
				Formula f(*thread.m);
				if (!texit && f.check(false)) { thread.solved = true; thread.wmat = f.getMatching(); }
				if (!texit && (!thread.solved || !forcequit)) texit = thread.result->computeComponentInfo(*thread.m);
/*
				Matchings m(*(thread.inf1),*(thread.inf2));
				Formula f(m);
				if (!texit && f.check(false)) { thread.solved = true; thread.wmat = f.getMatching(); }
				if (!texit && (!thread.solved || !forcequit)) texit = thread.result->computeComponentInfo(m);
*/
			}
		}

		// deinitialise our job data
		pthread_mutex_lock(&(thread.mutex));
		if (thread.restart) thread.solved = false;
		thread.restart = false;
		thread.idle = true;
		pthread_mutex_unlock(&(thread.mutex));

		// tell the world about our status (we're idle now)
		pthread_mutex_lock(&main_mutex);
		if (thread.solved) fsolved = true;
		tindex.insert(tid);
//		idleID.insert(tid);
		pthread_cond_signal(&main_cond);
		pthread_mutex_unlock(&main_mutex);

		if (debug) {
			pthread_mutex_lock(&(print_mutex));
			cout << "[" << tid << "] Idle" << endl;
			pthread_mutex_unlock(&(print_mutex));
		}

	}

	// terminate, free some thread data
	pthread_mutex_destroy(&(thread.mutex));
	pthread_cond_destroy(&(thread.cond));
	pthread_exit((void *)tid);
}

/** Initialise a root thread with a new job.
	@param rtnr The thread's ID.
	@param ps The problem to solve.
	@param m The initial marking.
	@param tv The transition vector to realize.
	@param pf An instance of the problem solver.
*/
void initThread(unsigned int rtnr, unsigned int jobnr, SubNetInfo* sni1, SubNetInfo* sni2, SubNetInfo* result) {
	SThread* me(threaddata[rtnr]);
	pthread_mutex_lock(&(me->mutex));

	me->ms = NULL;
	me->inf1 = sni1;
	me->inf2 = sni2;
	me->result = result;
	me->job = jobnr;

	// we're not idle anymore
	me->idle = false;

	pthread_cond_signal(&(me->cond));
	pthread_mutex_unlock(&(me->mutex));
}

void initThread(unsigned int rtnr, unsigned int jobnr, SubNet* sn, SubNetInfo* result) {
	SThread* me(threaddata[rtnr]);
	pthread_mutex_lock(&(me->mutex));

	me->ms = NULL;
	me->sn = sn;
	me->result = result;
	me->job = jobnr;

	// we're not idle anymore
	me->idle = false;

	pthread_cond_signal(&(me->cond));
	pthread_mutex_unlock(&(me->mutex));
}

void initThread(unsigned int rtnr, unsigned int jobnr, SubNetInfo& sni, const Matchings& ms, InterfaceID id, vector<MatchingID>& mids, Matchings::STType sttype) {
	SThread* me(threaddata[rtnr]);
	pthread_mutex_lock(&(me->mutex));

	me->inf1 = &sni;
	me->ms = &ms;
	me->id = id;
	me->mids = &mids;
	me->sttype = sttype;
	me->job = jobnr;

	// we're not idle anymore
	me->idle = false;

	pthread_cond_signal(&(me->cond));
	pthread_mutex_unlock(&(me->mutex));
}

/** Signal depending threads to restart.
	@param debug Flag for status messages.
*/
void makeThreadsIdle() {
	// signal all threads (including the main process) to restart
	pthread_mutex_lock(&main_mutex);
	for(unsigned int i=1; i<=maxthreads; ++i) {
		pthread_mutex_lock(&(threaddata[i]->mutex));
		idleID.erase(i);
		threaddata[i]->restart = true;
		pthread_cond_signal(&(threaddata[i]->cond));
		pthread_mutex_unlock(&(threaddata[i]->mutex));
	}
	texit = true;
	pthread_mutex_unlock(&main_mutex);
}

/** Wait for all threads to go idle.
	@param debug flag for status messages.
*/
void waitForAllIdle() {
	// this function must be called by the main process only
	// wait until all threads are idle
	pthread_mutex_lock(&main_mutex);
	while (idleID.size()+tindex.size()<maxthreads)
		pthread_cond_wait(&main_cond,&main_mutex);
	pthread_mutex_unlock(&main_mutex);

//	makeThreadsIdle();
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
		if (debug) { cerr << i+1; cerr.flush(); }
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
		if (debug) { cerr << i+1; cerr.flush(); }
		pthread_join(threads[i],(void **)x);
	}
	if (debug) cerr << endl;
}

/** Destroy all thread related data.
*/
void destroyThreadData() {
	pthread_mutex_destroy(&main_mutex);
	pthread_cond_destroy(&main_cond);
	pthread_mutex_destroy(&print_mutex);
	pthread_mutex_destroy(&sat_mutex);
	delete threaddata[0];
}



