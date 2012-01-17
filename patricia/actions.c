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

struct action* parallelize(int num, ...) {
  struct action *action = malloc(sizeof(struct action));
  va_list ap;
  int i;
  
  assert(num > 0);
  action->type = Parallel;
  action->data.parallel.actions = malloc(sizeof(struct action*) * num);
  action->data.parallel.num = num;

  va_start(ap, num);
  for(i = 0; i < num; i++) {
    action->data.parallel.actions[i] = va_arg(ap, struct action*);
  }
  va_end(ap);
  
  return action;
}

struct action* tryThen(struct action *try, struct action *then) {
  struct action *action = malloc(sizeof(struct action));
  action->type = Try;
  action->data.try.try = try;
  action->data.try.then = then;
  return action;
}

struct action* tool2action(struct tool *tool, int timeout) {
  struct action *action = malloc(sizeof(struct action));
  action->type = Run;
  action->data.run.tool = tool;
  action->data.run.timeout = timeout;
  return action;
}

struct outcome *performParallel(struct problem *problem, struct action **actions, int num) {
  int i;
  int remaining = num;
  pthread_t threads[num+1];
  struct ptask *ptask;
  struct outcome *outcome = malloc(sizeof(struct outcome));
  
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
  for(i = 0; i < num; i++) {
    ptask = malloc(sizeof(struct ptask));
    ptask->outcome = outcome;
    ptask->problem = problem;
    ptask->action = actions[i];

    pthread_create(&threads[i], NULL, threadedPerform, (void*)ptask);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  }

  // fetch the verification results from the tools
  debug_print("%s: waiting for results\n", __func__);
  for(remaining = num; remaining > 0; remaining--) {
    debug_print("%s: waiting for results (%d/%d)\n", __func__, num-remaining, num);
    pthread_cond_wait(&outcome->cond, &outcome->mutex);
    debug_print("%s: received %d from %s\n", __func__, outcome->state, outcome->tool);

    if(outcome->state != Undefined) {
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

struct outcome *performTry(struct problem *problem, struct action *try, struct action *then) {
  struct outcome *result;
  
  debug_print("%s: trying action\n", __func__);
  result = perform(problem, try);
  if(result->state == Undefined) {
    debug_print("%s: action failed, trying next action\n", __func__);
    result = perform(problem, then);
  }
  
  return result;
}

void *threadedRun(void *args) {
  struct ptask *ptask = (struct ptask*)args;
  assert(ptask->action->type == Run);
  publish(ptask->outcome, run(ptask->problem, ptask->action->data.run.tool), ptask->action->data.run.tool->name);
  return NULL;
}

void *threadedSleep(void *args) {
  struct ptask *ptask = (struct ptask*)args;
  assert(ptask->action->type == Run);
  usleep(ptask->action->data.run.timeout * 1000); // TODO: Replace this with nanosleep?
  publish(ptask->outcome, Undefined, "timeout");
  return NULL;
}

struct outcome *performRunWithTimeout(struct problem *problem, struct tool *tool, unsigned int timeout) {
  struct outcome *outcome = malloc(sizeof(struct outcome));
  struct ptask *ptask = malloc(sizeof(struct ptask));
  pthread_t worker;
  pthread_t guard;
  
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
  pthread_create(&worker, NULL, threadedRun, (void*)ptask);
  pthread_cleanup_push(killThread, (void*)&worker);
  
  // run guard
  debug_print("%s: creating guard thread\n", __func__);
  pthread_create(&guard, NULL, threadedSleep, (void*)ptask);
  pthread_cleanup_push(killThread, (void*)&guard);

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

struct outcome *performRunWithoutTimeout(struct problem *problem, struct tool *tool) {
  struct outcome *outcome = malloc(sizeof(struct outcome));
  outcome->tool = tool->name;
  outcome->state = run(problem, tool);
  return outcome;
}

struct outcome *perform(struct problem *problem, struct action *action) {
  assert(action->type == Parallel || action->type == Try || action->type == Run);
  
  if(action->type == Parallel) {
    debug_print("%s: next action is parallel\n", __func__);
    return performParallel(problem, action->data.parallel.actions, action->data.parallel.num);
  } else if(action->type == Try) {
    debug_print("%s: next action is try-then\n", __func__);
    return performTry(problem, action->data.try.try, action->data.try.then);
  } else if(action->type == Run) {
    if(action->data.run.timeout > 0) {
      debug_print("%s: next action is running a tool with timeout\n", __func__);
      return performRunWithTimeout(problem, action->data.run.tool, action->data.run.timeout);
    } else {
      debug_print("%s: next action is running a tool without timeout\n", __func__);
      return performRunWithoutTimeout(problem, action->data.run.tool);
    }
  }
  
  return NULL; // shouldn't happen
}

void *threadedPerform(void *args) {
  struct ptask *ptask = (struct ptask*)args;
  struct outcome *outcome;
  outcome = perform(ptask->problem, ptask->action);
  publish(ptask->outcome, outcome->state, outcome->tool);
  return NULL;
}
