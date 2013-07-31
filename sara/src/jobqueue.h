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
 * \date    $Date: 2012-06-13 12:00:00 +0100 (Mi, 13. Jun 2012) $
 *
 * \version $Revision: 1.10 $
 */

#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <set>
#include <map>
#include <deque>
#include "partialsolution.h"
#include "JSON.h"

using std::set;
using std::map;
using std::deque;

namespace sara {

/*! \brief For queuing jobs (future and past), solutions, and failures

	Class for future jobs, i.e. unsolved partial solutions. Fast finding and inserting is essential,
	as the queue can become quite long. Jobs are ordered by length of the total solution, number
	of unfirable transitions, or number of constraints. Jobs with a lower position have priority
	since they are expected to run faster. Past jobs are saved for later optimisations. A JobQueue
	can also store solutions or failures and has specialised methods for handling these.
*/
class JobQueue {

public:
	/// Standard constructor for job queues, solution queues, and failure queues
	JobQueue();

	/// Constructor with initialization, i.e. a first job
	JobQueue(PartialSolution* job);

	/// Destructor
	~JobQueue();

	/// Empty a queue
	void flush();

	/// Check if a job queue is empty, i.e. has no active and future jobs
	bool empty() const;

	/// Check if a job queue contains no future jobs, an active job is possible
	bool almostEmpty();

	/// Get the number of jobs in a queue (not counting past jobs)
	int size() const;

	/// Get the active job
	PartialSolution* first();

	/// Remove the active job, possibly shifting it into the past 
	bool pop_front(bool kill);

	/// Retrieve the active job, remove it from the queue permamently
	PartialSolution* pop_front();

	/// Add job after the last job with the same (or lower) priority
	void push_back(PartialSolution* job);

	/// Put a job directly into the past of a queue
	void push_past(PartialSolution* job);

	/// Transfer the (active and future) entries of another job queue into this one 
	void transfer(JobQueue&);

	/// Find a job if it is already in the list
	int find(PartialSolution* job);

	/// Find out if a job has already been done in the past
	bool findPast(PartialSolution* job, PartialSolution* excl);

	/// Calculate the priority of a job or a failure
	int priority(PartialSolution* job) const;

	/// Check a failure queue for infeasibility of the marking equation
	bool checkMEInfeasible();

	/// Add a failure to a failure queue, checking for duplicates
	void push_fail(PartialSolution* job);

	/// Clean a failure queue from obsolete entries regarding one parikh image
	bool cleanFailure(map<Transition*,int>& p);

	/// Print a failure queue
	void printFailure(IMatrix& im, Problem& pb, int pbnr, JSON& json);

	/// Get the true size of a failure queue (excluding obsolete entries)
	int trueSize();

	/// Push a solution into the solution queue
	bool push_solved(PartialSolution* job);

	/// Print all solutions and return the maximal and sum trace length
	int printSolutions(int& sum, Problem& pb, int pbnr, JSON& json);

#ifdef SARALIB
	/// Get one solution (or a vector containing NULL pointer).
	vector<Transition*> getOneSolution();
#endif

	/// Print a Jobqueue to stderr (possibly including past and active job)
	void show(bool past);

	/// Delete solutions covered by vec from the queue
	void deleteCovered(map<Transition*,int>& vec);

	/// Append the elements of one queue to another
	void append(JobQueue& jbq);

	/// Colors the firing sequence given by tvec in the net
	void colorSequence(vector<Transition*>& tvec);

	/// Colors a given set of transitions red (or blue)
	void colorTransitions(set<Transition*>& tset, bool blue);

	/// Colors a given set of places (key-values of pmap)
	void colorPlaces(map<Place*,int>& pmap, bool blue);

	/// Removes all color from the net
	void resetColors(PetriNet& pn);

	/// Saves the colored net as number nr to a file
	void saveColoredNet(Problem& pb, int pbnr, int nr);

private:
	/// Job, solution, or failure queue (maps from priority to sublist)
	map<int,deque<PartialSolution*> > queue;

	/// The size of the queue
	int cnt;

	/// The active job, not saved in the queue itself
	PartialSolution* active;

	/// The queue of past jobs for constraint comparisons
	map<int,deque<PartialSolution*> > past;
};

}

#endif

