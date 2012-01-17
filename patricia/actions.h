#ifndef ACTIONS_H
#define ACTIONS_H

#include "types.h"

enum ActionType { Parallel, Try, Run };

struct action {
    enum ActionType type;
    union {
        struct {
            struct action** actions;
            int num;
        }        parallel;
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
