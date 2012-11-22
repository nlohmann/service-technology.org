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
 * \date    $Date: 2012-09-11 12:00:00 +0200 (Di, 11. Sep 2012) $
 *
 * \version $Revision: 1.00 $
 */

#ifndef STHREAD_H
#define STHREAD_H

#include <pthread.h>
#include "types.h"
#include "subnetinfo.h"
#include "subnet.h"
#include "matchings.h"

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

	/// The number of jobs done by this thread
	unsigned int activity;

	/// number of the current job
	unsigned int job;

	/// pointer to the left child
	SubNetInfo* inf1;
	/// pointer to the right child
	SubNetInfo* inf2;
	/// pointer to the resulting structure
	SubNetInfo* result;
	SubNet* sn;
	Siphon s;

	const Matchings* ms;
	InterfaceID id;
	vector<MatchingID>* mids;
	enum Matchings::STType sttype;
};

	/// Initialise basic data for all threads (mutexes etc.)
	void initThreadData(int max);

	/// Start all threads
	void startThreads();

	/// Running threads
	void* threadManagement(void* arg);

	/// Initialize a thread with an inner node of the divide-and-conquer tree
	void initThread(unsigned int threadID, unsigned int jobnr, SubNetInfo* sni1, SubNetInfo* sni2, SubNetInfo* result);

	/// Initialize a thread with a leaf node of the divide-and-conquer tree
	void initThread(unsigned int threadID, unsigned int jobnr, SubNet* sn, SubNetInfo* result);

	/// Initialize a thread for a wrapping set computation (unused)
	void initThread(unsigned int threadID, unsigned int jobnr, SubNetInfo& sni, const Matchings& ms, InterfaceID id1, vector<MatchingID>& mids, Matchings::STType sttype);

	/// Restart all threads
	void makeThreadsIdle();

	/// Wait until all threads go idle
	void waitForAllIdle();

	/// Kill all threads
	void stopThreads();

	/// Free remaining thread data
	void destroyThreadData();

#endif

