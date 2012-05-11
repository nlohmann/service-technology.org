#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <iostream>

#include "verbose.h"
#include "action.h"
#include "tools.h"
#include "outcome.h"
#include "problem.h"
#include "ptask.h"

using std::vector;
//using std::cerr;
//using std::endl;

/*!
	Constructor yielding a container for the given actions.

 \param par Whether the given actions are to be executed in parallel.
 \param va A vector of actions to be put into a container.
 */
Action::Action(bool par, vector<Action*>& va) : type(par ? Action::Parallel : Action::Sequence), actions(va), tool(NULL), timeout(0) {}

/*!
	Constructor for a container for two actions.

 \param par Whether the given actions should be executed in parallel.
 \param act1 The first action
 \param act2 The second action
 */
Action::Action(bool par, Action* act1, Action* act2) : type(par ? Action::Parallel : Action::Sequence), tool(NULL), timeout(0) {
	actions.push_back(act1);
	actions.push_back(act2);
}

/*!
	Constructor for a single action container.

 \param t The tool to be executed.
 \param time A timeout (in milliseconds; zero implies no timeout)

 \note do not use timeouts less than zero
 */
Action::Action(Tool* t, int time) : type(Action::Run), tool(t), timeout(time) {
	assert(timeout >= 0);
} 

/*!
	Destructor (deletes the tool!).
*/
Action::~Action() {
	delete tool;
	for(unsigned int i=0; i<actions.size(); ++i)
		delete actions[i];
}

/*!
 Depending on the type of the action this function calls:
 - performParallel (Parallel)
 - performSequence (Sequence)
 - performRunWithTimeout (Run, timeout strictly greater than zero)
 - performRunWithoutTimeout (Run, timeout equals zero)

 \param problem
 \return outcome
 */
Outcome Action::perform(Problem& problem) {
    assert(type == Parallel || type == Sequence || type == Run);

	switch (type) {
		case Parallel:
	        status("%s: next action is parallel", __func__);
    	    return performParallel(problem);
		case Sequence:
	        status("%s: next action is try-then", __func__);
    	    return performSequence(problem);
		case Run:
	        if (timeout > 0) {
    	        status("%s: next action is running a tool with timeout", __func__);
        	    return performRunWithTimeout(problem);
	        } else {
    	        assert(timeout == 0);
        	    status("%s: next action is running a tool without timeout", __func__);
            	return performRunWithoutTimeout(problem);
        	}
    }

	Outcome outcome("");
    return outcome; // shouldn't happen
}

/*!
 \param problem
 \param actions that are executed in parallel
 \param number of actions
 \return outcome
 */
Outcome Action::performParallel(Problem& problem) {
    unsigned int num = actions.size();
    int cancel;
    pthread_t threads[num + 1];

    // disable cancellation
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel);

    // initialize outcome variable
    status("%s: initializing", __func__);
    Outcome outcome("none");
    pthread_mutex_lock(&outcome.mutex);
    pthread_cleanup_push(killThreads, (void*)threads);

    // create threads
    status("%s: creating threads", __func__);
    threads[num] = (pthread_t)NULL;
    for (unsigned int i = 0; i < num; ++i) {
		PTask* ptask(new PTask(problem,*(actions[i]),outcome));
        pthread_create(&threads[i], NULL, threadedPerform, (void*)ptask);
    }

    // allow cancellation
    pthread_setcancelstate(cancel, NULL);

    // fetch the verification results from the tools
    status("%s: waiting for results", __func__);
    for (unsigned int remaining = num; remaining > 0; --remaining) {
        status("%s: waiting for results (%d/%d)", __func__, num - remaining, num);
        pthread_cond_wait(&outcome.cond, &outcome.mutex);
        status("%s: received %d from %s", __func__, outcome.state, outcome.tool.c_str());

        if (outcome.state != Outcome::Undefined) break;
    }

    status("%s: canceling threads", __func__);
    pthread_cleanup_pop(1);

    // finalize outcome variable
    pthread_mutex_unlock(&outcome.mutex);

    return outcome;
}

/*!
 \param problem
 \param action that is tried first
 \param action that is executed if the first fails
 \return outcome
 */
Outcome Action::performSequence(Problem& problem) {
	unsigned int i(0);
    status("%s: trying action", __func__);
    Outcome result(actions[i]->perform(problem));
    while (result.state == Outcome::Undefined && ++i<actions.size()) {
        status("%s: action failed, trying next action", __func__);
        result = actions[i]->perform(problem);
    }

    return result;
}

/*!
 \param problem
 \param tool solving the problem
 \param timeout in milliseconds
 \return outcome

 \note when the timeout expires, the outcome's state is Undefined and the name is "timeout".
 */
Outcome Action::performRunWithTimeout(Problem& problem) {
    int cancel;
    pthread_t worker;
    pthread_t guard;

    // disable cancellation
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel);

    // initialize variables
    status("%s: initializing", __func__);
	Outcome outcome("none");
	Action act(tool,timeout);
	PTask ptask(problem,act,outcome);
    pthread_mutex_lock(&outcome.mutex);

    // run worker
    status("%s: creating worker thread", __func__);
    pthread_cleanup_push(killThread, (void*)&worker);
    pthread_create(&worker, NULL, threadedRun, (void*)(&ptask));

    // run guard
    status("%s: creating guard thread", __func__);
    pthread_cleanup_push(killThread, (void*)&guard);
    pthread_create(&guard, NULL, threadedSleep, (void*)(&ptask));

    // enable cancellation
    pthread_setcancelstate(cancel, NULL);

    // wait for signal
    status("%s: waiting for signal", __func__);
    pthread_cond_wait(&outcome.cond, &outcome.mutex);
    status("%s: received %d from %s", __func__, outcome.state, outcome.tool.c_str());
    pthread_cleanup_pop(1); // cancel guard
    pthread_cleanup_pop(1); // cancel worker
    pthread_mutex_unlock(&outcome.mutex);

	act.tool = NULL; // prevent deletion of tool on deconstruction of act
    return outcome;
}

/*!
 \param problem
 \param tool solving the problem
 \return outcome
 */
Outcome Action::performRunWithoutTimeout(Problem& problem) {
    Outcome outcome(tool->name);
    outcome.state = tool->run(problem);
    return outcome;
}

/*
void Action::show(int indent) {
	for(int i=0; i<indent; ++i) cerr << "-";
	cerr << " ";
	if (tool) cerr << "RUN Tool: " << tool->name << " Timeout: " << timeout << endl;
	else {
		switch (type) {
			case Parallel: cerr << "PAR "; break;
			case Sequence: cerr << "SEQ "; break;
		}
		cerr << "Actions: " << actions.size() << endl;
		for(int i=0; i<actions.size(); ++i) actions[i]->show(indent+1);
	}
}
*/
