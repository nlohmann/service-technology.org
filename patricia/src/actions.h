#ifndef ACTIONS_H
#define ACTIONS_H

#include "types.h"

/*!
 There are three types of actions:
 - Parallel: Actions are executed in parallel until one gives a meaningful verification result;
             namely Positive or Negative.
 - Try: The first action is always executed, whereas the second one is utilized only when the first fails.
 - Run: Executing a tool with or without a timeout.
 */
enum ActionType { Parallel, Try, Run };

/*!
 Tagged union that contains the necessary information for the different kinds of actions.
 Depending on the type the struct contains following information:
 - Parallel: actions and the number of actions
 - Try: two actions named "try" and "then"
 - Run: a tool and a timeout
 */
struct action {
    enum ActionType type;
    union {
        struct {
            struct action** actions;
            int num;
        } parallel;
        struct {
            struct action* try;
            struct action* then;
        } try;
        struct {
            struct tool* tool;
            unsigned int timeout;
        } run;
    } data;
};

/*!
 Struct containing a problem, an action and an outcome variable.
 It is used for performing actions using threads.
 */
struct ptask {
    struct problem* problem;
    struct action* action;
    struct outcome* outcome;
};


// functions for constructing the action tree
struct action* parallelize(int num, ...);
struct action* tryThen(struct action* try, struct action* then);
struct action* tool2action(struct tool* tool, int timeout);

// functions that process the action tree
struct outcome* perform(struct problem* problem, struct action* action);
struct outcome* performParallel(struct problem* problem, struct action** actions, int num);
struct outcome* performTry(struct problem* problem, struct action* try, struct action* then);
struct outcome* performRunWithTimeout(struct problem* problem, struct tool* tool, unsigned int timeout);
struct outcome* performRunWithoutTimeout(struct problem* problem, struct tool* tool);

// helper functions for dealing with parallel execution
void* threadedRun(void* args);
void* threadedSleep(void* args);
void* threadedPerform(void* args);

#endif
