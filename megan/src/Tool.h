#pragma once

#include <string>
#include "Task.h"
#include "ast-system-k.h"

class ReachabilityTask;

class Tool {
    public:
        std::string executable;
        virtual result_t execute() = 0;
};

class LoLA : public Tool {
    public:
        LoLA();
};

class Tool_LoLA_Reachability : public LoLA {
    public:
        Tool_LoLA_Reachability();
        result_t execute();
        ReachabilityTask *t;
};

class Tool_LoLA_Deadlock : public LoLA {
    public:
        Tool_LoLA_Deadlock();
        result_t execute();
};
