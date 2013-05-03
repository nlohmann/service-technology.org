#include "cmdline.h"
#include "profile.h"

extern gengetopt_args_info args_info;

Tool *task2tool(task_t task, Task *t) {
    switch (args_info.profile_arg) {
        case (profile_arg_lola): {
            switch (task) {
                case(t_UnknownTask):         return NULL;
                case(t_ReachabilityTask):    return new Tool_LoLA_Reachability(t);
                case(t_DeadlockTask):        return new Tool_LoLA_Deadlock(t);
                case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock(t);
                case(t_TrueTask) :           return new Tool_Megan_True(t);
            }
        }

        case (profile_arg_sara): {
            switch (task) {
                case(t_UnknownTask):         return NULL;
                case(t_ReachabilityTask):    return new Tool_Sara_Reachability(t);
                case(t_DeadlockTask):        return NULL;
                case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock(t);
                case(t_TrueTask) :           return new Tool_Megan_True(t);
            }
        }
    }
}
