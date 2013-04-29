#pragma once

typedef enum { DEFINITELY_TRUE, DEFINITELY_FALSE, MAYBE_TRUE, MAYBE_FALSE, MAYBE, NOT_IMPLEMENTED, ERROR } result_t;
typedef enum { t_UnknownTask, t_ReachabilityTask, t_DeadlockTask, t_DeadlockInitialTask } task_t;

#include <string>
#include <vector>
#include <pnapi/pnapi.h>
#include "ast-system-k.h"
#include "Tool.h"
#include "profile.h"

class Tool;

class Task {
    private:
        /// whether to negate the final result
        bool negate;
        /// the name of the task
        std::string name;
        /// a function to negate results of type result_t
        static result_t negate_result(result_t);
    public:
        /// a task queue to which all tasks are added
        static std::vector<Task*> queue;
        /// the worker for the task (depending on the used profile)
        Tool *worker;
        /// the Petri net for the current task family
        static pnapi::PetriNet *net;
        /// return the net
        static pnapi::PetriNet *getNet();
        /// return the name of the task
        std::string getName() const;
        /// solve the task
        result_t solve() const;
        /// constructor
        Task(std::string, bool=false);
};

class UnknownTask : public Task {
    public:
        /// the formula to solve
        kc::formula f;
        UnknownTask(std::string, kc::formula, bool=false);
};

class ReachabilityTask : public Task {
    public:
        /// the formula to solve
        kc::formula f;
        ReachabilityTask(std::string, kc::formula, bool=false);
};

class DeadlockTask : public Task {
    public:
        DeadlockTask(std::string, bool=false);
};

class DeadlockInitialTask : public Task {
    public:
        DeadlockInitialTask(std::string, bool=false);
};
