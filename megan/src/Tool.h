#pragma once

#include <string>
#include "Task.h"
#include "ast-system-k.h"

class Task;
class ReachabilityTask;

class Tool {
    public:
        std::string basedir;
        virtual result_t execute() = 0;
        Task* t;
        Tool(Task *);
};



class LoLA : public Tool {
    public:
        LoLA(Task *t);
};

class Tool_LoLA_Reachability : public LoLA {
    public:
        Tool_LoLA_Reachability(Task*);
        result_t execute();
};

class Tool_LoLA_Deadlock : public LoLA {
    public:
        Tool_LoLA_Deadlock(Task *);
        result_t execute();
};



class Megan : public Tool {
    public:
        Megan(Task *t);
};

class Tool_Megan_InitialDeadlock : public Megan {
    public:
        Tool_Megan_InitialDeadlock(Task*);
        result_t execute();
};

class Tool_Megan_True : public Megan {
    public:
        Tool_Megan_True(Task*);
        result_t execute();
};


class Sara : public Tool {
    public:
        Sara(Task *t);
};

class Tool_Sara_Reachability : public Sara {
    public:
        Tool_Sara_Reachability(Task*);
        result_t execute();
};
