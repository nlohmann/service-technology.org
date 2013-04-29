#include "profile.h"

Tool *task2tool(task_t task, Task *t) {
    switch (task) {
        case(t_UnknownTask):         return NULL;
        case(t_ReachabilityTask):    return new Tool_LoLA_Reachability(t);
        //case(t_ReachabilityTask):    return new Tool_Sara_Reachability(t);
        case(t_DeadlockTask):        return new Tool_LoLA_Deadlock(t);
        case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock(t);
    }
}
