#ifndef PTASK_H
#define PTASK_H

#include "problem.h"
#include "action.h"
#include "outcome.h"

/*!
 Class containing a problem, an action and an outcome variable.
 It is used for performing actions using threads.
 */
class PTask {
public:

	/// Constructor for a container object to pass to a thread
	PTask(Problem&, Action&, Outcome&);

	Problem& problem;
	Action& action;
	Outcome& outcome;
};

// helper functions for dealing with parallel execution using a PTask object as parameter 
void* threadedRun(void* args);
void* threadedSleep(void* args);
void* threadedPerform(void* args);

// helper functions for thread/process killing
void killProcess(void* arg);
void killThread(void* arg);
void killThreads(void* arg);

#endif
