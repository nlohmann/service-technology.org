#pragma once

typedef enum { DEFINITELY_TRUE, DEFINITELY_FALSE, MAYBE_TRUE, MAYBE_FALSE, MAYBE, NOT_IMPLEMENTED, ERROR } result_t;
typedef enum { t_UnknownTask, t_ReachabilityTask, t_DeadlockTask, t_DeadlockInitialTask, t_TrueTask, t_CTLTask, t_LTLTask } task_t;

#include <string>
#include <vector>
//#include <future>
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
        /// future solution of the task
        //std::future<result_t> solution;
        /// cached solution of the task
        result_t cached_solution;
        /// a function to negate results of type result_t
        static result_t negate_result(result_t);

    public:
        /// a counter for the current index
        static size_t current_property_id;
        /// a task queue to which all tasks are added
        static std::vector<Task*> queue;
        /// the Petri net for the current task family
        static pnapi::PetriNet *net;
        /// return the net
        static pnapi::PetriNet *getNet();

        /// an id to access the global list of properties
        size_t property_id;
        /// the worker for the task (depending on the used profile)
        Tool *worker;
        /// return the name of the task
        std::string getName() const;
        /// solve the task
        void solve();
        /// get the solution
        result_t getSolution();

        /// constructor
        Task(std::string, bool=false);
        /// destructor
        ~Task();
};

class UnknownTask : public Task {
    public:
        UnknownTask(std::string, bool=false);
};

class CTLTask : public Task {
    public:
        CTLTask(std::string, bool=false);
};

class LTLTask : public Task {
    public:
        LTLTask(std::string, bool=false);
};

class ReachabilityTask : public Task {
    public:
        ReachabilityTask(std::string, bool=false);
};

class DeadlockTask : public Task {
    public:
        DeadlockTask(std::string, bool=false);
};

class DeadlockInitialTask : public Task {
    public:
        DeadlockInitialTask(std::string, bool=false);
};

class TrueTask : public Task {
    public:
        TrueTask(std::string, bool=false);
};
