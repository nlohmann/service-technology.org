#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "actions.h"
#include "tools.h"
#include "types.h"
#include "utils.h"

/*!
 \param number of actions that are executed parallel
 \param ... (actions)
 \return action executing the given actions in parallel
 */
struct action* parallelize(int num, ...) {
    struct action* action = malloc(sizeof(struct action));
    va_list ap;
    int i;

    assert(num > 0);
    action->type = Parallel;
    action->data.parallel.actions = malloc(sizeof(struct action*) * num);
    action->data.parallel.num = num;

    va_start(ap, num);
    for (i = 0; i < num; i++) {
        action->data.parallel.actions[i] = va_arg(ap, struct action*);
    }
    va_end(ap);

    return action;
}

/*!
 \param action that is tried first
 \param action that is used otherwise
 \return action that executes the second action if the first fails
 */
struct action* tryThen(struct action* try, struct action* then) {
    struct action* action = malloc(sizeof(struct action));
    action->type = Try;
    action->data.try.try = try;
    action->data.try.then = then;
    return action;
}

/*!
 \param tool
 \param timeout (in milliseconds; zero implies no timeout)
 \return action that runs the given tool with a specified timeout

 \note do not use timeouts less than zero
 */
struct action* tool2action(struct tool* tool, int timeout) {
    assert(timeout >= 0);
    struct action* action = malloc(sizeof(struct action));
    action->type = Run;
    action->data.run.tool = tool;
    action->data.run.timeout = timeout;
    return action;
}

/*!
 Depending on the type of the action this function calls:
 - performParallel (Parallel)
 - performTry (Try)
 - performRunWithTimeout (Run, timeout strictly greater than zero)
 - performRunWithoutTimeout (Run, timeout equals zero)
 
 \param problem
 \param action tree that is executed
 \return outcome
 */
struct outcome* perform(struct problem* problem, struct action* action) {
    assert(action->type == Parallel || action->type == Try || action->type == Run);

    if (action->type == Parallel) {
        debug_print("%s: next action is parallel\n", __func__);
        return performParallel(problem, action->data.parallel.actions, action->data.parallel.num);
    } else if (action->type == Try) {
        debug_print("%s: next action is try-then\n", __func__);
        return performTry(problem, action->data.try.try, action->data.try.then);
    } else if (action->type == Run) {
        if (action->data.run.timeout > 0) {
            debug_print("%s: next action is running a tool with timeout\n", __func__);
            return performRunWithTimeout(problem, action->data.run.tool, action->data.run.timeout);
        } else {
	    assert(action->data.run.timeout == 0);
            debug_print("%s: next action is running a tool without timeout\n", __func__);
            return performRunWithoutTimeout(problem, action->data.run.tool);
        }
    }

    return NULL; // shouldn't happen
}

/*!
 \param problem
 \param actions that are executed in parallel
 \param number of actions
 \return outcome
 */
struct outcome* performParallel(struct problem* problem, struct action** actions, int num) {
    int i;
    int remaining = num;
    int cancel;
    pthread_t threads[num + 1];
    struct ptask* ptask;
    struct outcome* outcome = malloc(sizeof(struct outcome));
    
    // disable cancellation
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel);
        
    // initialize outcome variable
    debug_print("%s: initializing\n", __func__);
    outcome->state = Undefined;
    outcome->tool = "none";
    pthread_mutex_init(&outcome->mutex, NULL);
    pthread_cond_init(&outcome->cond, NULL);
    pthread_mutex_lock(&outcome->mutex);
    pthread_cleanup_push(killThreads, (void*)threads);

    // create threads
    debug_print("%s: creating threads\n", __func__);
    threads[num] = (pthread_t)NULL;
    for (i = 0; i < num; i++) {
        ptask = malloc(sizeof(struct ptask));
        ptask->outcome = outcome;
        ptask->problem = problem;
        ptask->action = actions[i];
	
        pthread_create(&threads[i], NULL, threadedPerform, (void*)ptask);
    }

    // allow cancelation
    pthread_setcancelstate(cancel, NULL);

    // fetch the verification results from the tools
    debug_print("%s: waiting for results\n", __func__);
    for (remaining = num; remaining > 0; remaining--) {
        debug_print("%s: waiting for results (%d/%d)\n", __func__, num - remaining, num);
        pthread_cond_wait(&outcome->cond, &outcome->mutex);
        debug_print("%s: received %d from %s\n", __func__, outcome->state, outcome->tool);

        if (outcome->state != Undefined) {
            remaining = 0;
        }
    }

    debug_print("%s: canceling threads\n", __func__);
    pthread_cleanup_pop(1);

    // finalize outcome variable
    pthread_mutex_unlock(&outcome->mutex);
    pthread_mutex_destroy(&outcome->mutex);
    pthread_cond_destroy(&outcome->cond);

    return outcome;
}

/*!
 \param problem
 \param action that is tried first
 \param action that is executed if the first fails
 \return outcome
 */
struct outcome* performTry(struct problem* problem, struct action* try, struct action* then) {
    struct outcome* result;

    debug_print("%s: trying action\n", __func__);
    result = perform(problem, try);
    if (result->state == Undefined) {
        debug_print("%s: action failed, trying next action\n", __func__);
        result = perform(problem, then);
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
struct outcome* performRunWithTimeout(struct problem* problem, struct tool* tool, unsigned int timeout) {
    int cancel;
    struct outcome* outcome = malloc(sizeof(struct outcome));
    struct ptask* ptask = malloc(sizeof(struct ptask));
    pthread_t worker;
    pthread_t guard;
    
    // disable cancellation
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel);

    // initialize variables
    debug_print("%s: initializing\n", __func__);
    ptask->problem = problem;
    ptask->outcome = outcome;
    ptask->action = tool2action(tool, timeout);
    outcome->tool = "none";
    outcome->state = Undefined;

    // initialize mutex und condition variable
    pthread_mutex_init(&outcome->mutex, NULL);
    pthread_cond_init(&outcome->cond, NULL);
    pthread_mutex_lock(&outcome->mutex);

    // run worker
    debug_print("%s: creating worker thread\n", __func__);
    pthread_cleanup_push(killThread, (void*)&worker);
    pthread_create(&worker, NULL, threadedRun, (void*)ptask);

    // run guard
    debug_print("%s: creating guard thread\n", __func__);
    pthread_cleanup_push(killThread, (void*)&guard);
    pthread_create(&guard, NULL, threadedSleep, (void*)ptask);

    // enable cancellation
    pthread_setcancelstate(cancel, NULL);

    // wait for signal
    debug_print("%s: waiting for signal\n", __func__);
    pthread_cond_wait(&outcome->cond, &outcome->mutex);
    debug_print("%s: received %d from %s\n", __func__, outcome->state, outcome->tool);
    pthread_cleanup_pop(1); // cancel guard
    pthread_cleanup_pop(1); // cancel worker
    pthread_mutex_unlock(&outcome->mutex);

    // finalize outcome variable
    pthread_mutex_destroy(&outcome->mutex);
    pthread_cond_destroy(&outcome->cond);

    return outcome;
}

/*!
 \param problem
 \param tool solving the problem
 \return outcome
 */
struct outcome* performRunWithoutTimeout(struct problem* problem, struct tool* tool) {
    struct outcome* outcome = malloc(sizeof(struct outcome));
    outcome->tool = tool->name;
    outcome->state = run(problem, tool);
    return outcome;
}
/*!
 Performs the given action and publishes the outcome.
 This function is used for calling perform in a new thread.
 
 \param arguments castable to (struct ptask*)
 */
void* threadedPerform(void* args) {
    struct ptask* ptask = (struct ptask*)args;
    struct outcome* outcome;
    
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    outcome = perform(ptask->problem, ptask->action);
    publish(ptask->outcome, outcome->state, outcome->tool);
    
    return NULL;
}

/*!
 Runs a tool and publishes the outcome.
 This function is used for running a tool in a new thread.
 
 \param arguments castable to (struct ptask*)
 
 \note the action of the ptask variable has type of Run
 */
void* threadedRun(void* args) {
    struct ptask* ptask = (struct ptask*)args;
    
    assert(ptask->action->type == Run);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    publish(ptask->outcome, run(ptask->problem, ptask->action->data.run.tool), ptask->action->data.run.tool->name);
    
    return NULL;
}

/*!
 Sleeps and then publishes Undefined under the name "timeout".
 This function is used for implementing timeouts.
 
 \param arguments castable to (struct ptask*)
 
 \note the action of the ptask variable has type of Run
 */
void* threadedSleep(void* args) {
    struct ptask* ptask = (struct ptask*)args;
    
    assert(ptask->action->type == Run);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    usleep(ptask->action->data.run.timeout * 1000); // TODO: Replace this with nanosleep?
    publish(ptask->outcome, Undefined, "timeout");
    
    return NULL;
}
