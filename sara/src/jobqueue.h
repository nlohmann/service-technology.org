// -*- C++ -*-

/*!
 * \file    jobqueue.h
 *
 * \brief   Class JobQueue
 *
 * \author  Harro Wimmel <harro.wimmel@uni-rostock.de>
 *
 * \since   2010/02/26
 *
 * \date    $Date: 2010-03-02 12:00:00 +0100 (Di, 2. Mar 2010) $
 *
 * \version $Revision: -1 $
 */

#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <set>
#include <map>
#include <deque>
#include "partialsolution.h"

using std::set;
using std::map;
using std::deque;

/** Class for future jobs, i.e. unsolved partial solutions. Fast finding and inserting is essential,
	as the queue can become quite long. Jobs are ordered by length of the total solution, number
	of unfirable transitions and number of constraints. Jobs with a lower position have priority
	since they are expected to run faster.
*/
class JobQueue {

public:
	/// Standard constructor for job queues and failure queues
	JobQueue();

	/// Constructor with initialization
	JobQueue(PartialSolution* job);

	/// Destructor
	~JobQueue();

	/// Check if a job queue is empty
	bool empty();

	/// Check if a job queue contains only the active job
	bool almostEmpty();

	/// Get the number of jobs in a queue
	int size();

	/// Get first job
	PartialSolution* first();

	/// Remove first job
	bool pop_front();

	/// Add job before all other jobs with the same (or higher) priority
	void push_front(PartialSolution* job);

	/// Add job after the last job with the same (or lower) priority
	void push_back(PartialSolution* job);

	/// Add job after the last job with the same (or lower) priority, but only if there is no equivalent job in the queue
	bool insert(PartialSolution* job);

	/// Find a job, if it is already in the list
	int find(PartialSolution* job);

	/// Calculate the priority of a job or a failure
	int priority(PartialSolution* job) const;

	/// Search the job list for a full solution and return it if found
	PartialSolution* findSolution();

	/// Check a failure queue for infeasibility of the marking equation
	bool checkMEInfeasible();

	/// Add a failure to a failure queue, checking for duplicates
	void push_fail(PartialSolution* job);

	/// Cleans a failure queue from obsolete entries regarding one parikh image
	bool cleanFailure(map<Transition*,int>& p);

	/// Print a failure queue
	void printFailure(IMatrix& im);

	/// Gets the true size of a failure queue (excluding obsolete entries)
	int trueSize();

private:
	/// job or failure queue (from priority to sublist)
	map<int,deque<PartialSolution*> > queue;

	/// size of the queue
	int cnt;

	/// active job, not saved in the queue
	PartialSolution* active;
};

#endif
