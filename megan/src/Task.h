#pragma once

typedef enum { DEFINITELY_TRUE, DEFINITELY_FALSE, MAYBE_TRUE, MAYBE_FALSE, MAYBE, NOT_IMPLEMENTED, ERROR } result_t;

#include <string>
#include <vector>
#include "ast-system-k.h"
#include "Tool.h"

class Tool;

class Task {
    private:
        bool negate;
        std::string name;
    public:
        static std::vector<Task*> queue;
        Tool *worker;
        result_t solve();
        Task(std::string, bool=false);
};

class UnknownTask : public Task {
    public:
        kc::formula f;
        UnknownTask(std::string, kc::formula, bool=false);
};

class ReachabilityTask : public Task {
    public:
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
