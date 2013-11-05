// -*- C++ -*-

/*!
 * \file    sthread.h
 *
 * \brief   Thread declarations
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2012/04/20
 *
 * \date    $Date: 2013-11-05 12:00:00 +0200 (Di, 5. Nov 2013) $
 *
 * \version $Revision: 1.14 $
 */

#ifndef STHREAD_H
#define STHREAD_H

#include <pthread.h>
#include "reachalyzer.h"
#include "pathfinder.h"

namespace sara {


/*! \brief Thread data for the parallelized execution of Sara

	This struct contains the data needed for a pthread as well as some data
	that formerly has been contained in the class PathFinder, needed by the
	method PathFinder::recurse() but contradicting thread safety.
*/
struct SThread {

	/// Mutex to be used when the thread is waiting for a new job
	pthread_mutex_t mutex;

	/// Signal the thread waits on while idle
	pthread_cond_t cond;

	/// If the thread has solved the global problem
	bool solved;

	/// If the thread is idle
	bool idle;

	/// If the thread should interrupt its work and go idle
	bool restart;

	/// If the thread should exit
	bool exit;

	/// A marking to be used by PathFinder
	Marking* m;

	/// Stack of transitions (firing sequence) for Pathfinder recursion
	vector<Transition*> fseq;

	/// The transitions to check against for diamonds while recursing Pathfinder
	vector<vector<Transition*> > stubsets;

	/// Pointer to the active element in stubsets
	unsigned int stubptr;

	/// The stubborn sets along the recursion
	vector<vector<Transition*> > tord;

	/// Pointers to the active transitions in the stubborn sets (for helper assignment)
	vector<unsigned int> tordptr;

	/// The full stubborn sets

	/// Depth to which helpers may be assigned in tord
	unsigned int tordlocsearchdepth;

	/// A transition vector (remainder) to be used by Pathfinder
	map<Transition*,int> tv;

	/// Pointers from transitions to their unique nodes in the conflict graph (Pathfinder recursion)
	vector<PathFinder::myNode*> tton;

	/// Active instance of the Reachalyzer
	Reachalyzer* r;

	/// lp_solve Wrapper structure
	LPWrapper* lp;

	/// Active Instance of the PathFinder
	PathFinder* pf;

	/// Active job
	PartialSolution* ps;

	/// Local Jobqueue
	JobQueue tps;

	/// recursive depth in PathFinder::recurse() at any given moment
	int recursedepth;

	/// ID of the root thread for which this thread acts as a helper in PathFinder
	unsigned int rootID;

	/// IDs of threads for which this thread acts as root
	set<unsigned int> root;

	/// Which helper solved the problem (-1=none)
	int solvedHelper;

	/// The number of jobs done by this thread
	unsigned int activity;

	/// The conflict & dependency graph for this thread
	vector<vector<Transition*> > cftab;

	// General purpose vector (for memory allocation)
	vector<Transition*> tseta;

	// General purpose vector (for memory allocation)
	vector<Transition*> tsetb;

	// General purpose vector (for memory allocation)
	vector<Transition*> tsets;

	// General purpose vector (for memory allocation)
	vector<int> tseti;

	// General purpose vector (for memory allocation)
	vector<bool> tbool;

	// General purpose vector (for memory allocation)
	vector<bool> tboola;

	// General purpose vector (for memory allocation)
	vector<bool> tboolb;

	// General purpose vector (for memory allocation)
	vector<int> pseta;

	// General purpose vector (for memory allocation)
	vector<int> psetb;

	// General purpose vector (for memory allocation)
	vector<Place*> psets;
};

	/// Initialise basic data for all threads (mutexes etc.)
	void initThreadData(int max);

	/// Start all threads
	void startThreads();

	/// Running threads
	void* threadManagement(void* arg);

	/// Initialize a thread with a new PathFinder job
	void initPathFinderThread(unsigned int threadID, PartialSolution* ps, Marking& m, map<Transition*,int>& tv, PathFinder& pf);

	/// Restart all threads
	void makeThreadsIdle(int debug);

	/// Wait until all threads go idle
	void waitForAllIdle(int debug);

	/// Kill all threads
	void stopThreads();

	/// Free remaining thread data
	void destroyThreadData();

	/// Allocate memory for Tarjan's algorithm
	void allocTarjan(unsigned int tid);

	/// Free memory of Tarjan's algorithm
	void freeTarjan(unsigned int tid);

}

#endif

