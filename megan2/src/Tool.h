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

/********
 * LoLA *
 ********/

class LoLA : public Tool {
    public:
        LoLA(Task *t);
};

// reachability

class Tool_LoLA_Reachability : public LoLA {
    public:
        Tool_LoLA_Reachability(Task*);
        result_t execute();
};

class Tool_LoLA_Reachability_optimistic : public LoLA {
    public:
        Tool_LoLA_Reachability_optimistic(Task*);
        result_t execute();
};

class Tool_LoLA_Reachability_optimistic_incomplete : public LoLA {
    public:
        Tool_LoLA_Reachability_optimistic_incomplete(Task*);
        result_t execute();
};

class Tool_LoLA_Reachability_pessimistic : public LoLA {
    public:
        Tool_LoLA_Reachability_pessimistic(Task*);
        result_t execute();
};

// deadlocks

class Tool_LoLA_Deadlock : public LoLA {
    public:
        Tool_LoLA_Deadlock(Task *);
        result_t execute();
};

class Tool_LoLA_Deadlock_optimistic : public LoLA {
    public:
        Tool_LoLA_Deadlock_optimistic(Task *);
        result_t execute();
};

class Tool_LoLA_Deadlock_optimistic_incomplete : public LoLA {
    public:
        Tool_LoLA_Deadlock_optimistic_incomplete(Task *);
        result_t execute();
};

class Tool_LoLA_Deadlock_pessimistic : public LoLA {
    public:
        Tool_LoLA_Deadlock_pessimistic(Task *);
        result_t execute();
};


/*********
 * Megan *
 *********/

class Megan : public Tool {
    public:
        Megan(Task *t);
};

// deadlocking initial state

class Tool_Megan_InitialDeadlock : public Megan {
    public:
        Tool_Megan_InitialDeadlock(Task*);
        result_t execute();
};

// constant result

class Tool_Megan_True : public Megan {
    public:
        Tool_Megan_True(Task*);
        result_t execute();
};


/********
 * Sara *
 ********/

class Sara : public Tool {
    public:
        Sara(Task *t);
};

// reachability

class Tool_Sara_Reachability : public Sara {
    public:
        Tool_Sara_Reachability(Task*);
        result_t execute();
};
