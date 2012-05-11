#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "verbose.h"
#include "ptask.h"
#include "outcome.h"
#include "problem.h"
#include "action.h"

/*!
	Constructor for a container object to be passed to a thread as parameter.

	\param pr A problem instance.
	\param act An action derived from a problem.
	\param out Where to put the result.
*/
PTask::PTask(Problem& pr, Action& act, Outcome& out) : problem(pr), action(act), outcome(out) {}

/*!
 Performs the given action and publishes the outcome.
 This function is used for calling Action::perform() in a new thread.

 \param arguments castable to (PTask*)
 */
void* threadedPerform(void* args) {
    PTask* ptask = (PTask*)args;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    Outcome outcome(ptask->action.perform(ptask->problem));
    ptask->outcome.publish(outcome.state, outcome.tool.c_str());

	delete ptask;
    return NULL;
}

/*!
 Runs a tool and publishes the outcome.
 This function is used for running a tool in a new thread.

 \param arguments castable to (PTask*)

 \note the action of the ptask variable has type of Run
 */
void* threadedRun(void* args) {
    PTask* ptask = (PTask*)args;

    assert(ptask->action.type == Action::Run);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    ptask->outcome.publish(ptask->action.tool->run(ptask->problem), ptask->action.tool->name);

    return NULL;
}

/*!
 Sleeps and then publishes Undefined under the name "timeout".
 This function is used for implementing timeouts.

 \param arguments castable to (PTask*)

 \note the action of the ptask variable has type of Run
 */
void* threadedSleep(void* args) {
    PTask* ptask = (PTask*)args;

    assert(ptask->action.type == Action::Run);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    usleep(ptask->action.timeout * 1000); // TODO: Replace this with nanosleep?
    ptask->outcome.publish(Outcome::Undefined, "timeout");

    return NULL;
}

/*!
 Kills the process with the given pid.

 \param argument castable to (pid_t*)
 */
void killProcess(void* arg) {
    int state;
    pid_t* pid = (pid_t*)arg;
    status("%s: killing %d", __func__, *pid);
    kill(*pid, SIGKILL);
    waitpid(*pid, &state, WNOHANG); // avoid zombies by requesting the exit status
}

/*!
 Kills a thread with the given thread id.

 \param argument castable to (pthread_t*).
 */
void killThread(void* arg) {
    pthread_t* tid = (pthread_t*)arg;
    status("%s: canceling %lu", __func__, (unsigned long)*tid);
    if (pthread_cancel(*tid)) status("%s: cancellation of %lu failed",__func__,(unsigned long)*tid);
	else { 
		void* res; 
		pthread_join(*tid,&res); 
		status("%s: joined %lu",__func__,(unsigned long)*tid); 
	}
}

/*!
 Kills threads with the given thread ids.
 Pointer arithmetic is used to obtain the thread ids.

 \param argument castable to (pthread_t*).

 \note the behaviour is unspecified if the array of thread ids is not zero-terminated.
 */
void killThreads(void* arg) {
    pthread_t* tid;

    for (tid = (pthread_t*)arg; *tid != 0; tid++) {
        status("%s: canceling %lu", __func__, (unsigned long)*tid);
    if (pthread_cancel(*tid)) status("%s: cancellation of %lu failed",__func__,(unsigned long)*tid);
	else { 
		void* res; 
		pthread_join(*tid,&res); 
		status("%s: joined %lu",__func__,(unsigned long)*tid); }
    }
}

