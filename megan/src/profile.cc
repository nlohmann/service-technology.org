#include "profile.h"

Tool *task2tool(task_t task) {
    switch (task) {
        case(t_UnknownTask): return NULL;
        case(t_ReachabilityTask): return new Tool_LoLA_Reachability();
        case(t_DeadlockTask): return new Tool_LoLA_Deadlock();
        case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock();
    }
}
