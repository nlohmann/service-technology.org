#include "profile.h"
#include "Runtime.h"

Tool *task2tool(task_t task, Task *t) {
    switch (Runtime::args_info.profile_arg) {
        case (profile_arg_lola): {
            switch (task) {
                case(t_UnknownTask):         return nullptr;
                case(t_ReachabilityTask):    return new Tool_LoLA_Reachability(t);
                case(t_DeadlockTask):        return new Tool_LoLA_Deadlock(t);
                case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock(t);
                case(t_TrueTask) :           return new Tool_Megan_True(t);
            }
        }

        case (profile_arg_lola_optimistic): {
            switch (task) {
                case(t_UnknownTask):         return nullptr;
                case(t_ReachabilityTask):    return new Tool_LoLA_Reachability_optimistic(t);
                case(t_DeadlockTask):        return new Tool_LoLA_Deadlock_optimistic(t);
                case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock(t);
                case(t_TrueTask) :           return new Tool_Megan_True(t);
            }
        }

        case (profile_arg_lola_optimistic_incomplete): {
            switch (task) {
                case(t_UnknownTask):         return nullptr;
                case(t_ReachabilityTask):    return new Tool_LoLA_Reachability_optimistic_incomplete(t);
                case(t_DeadlockTask):        return new Tool_LoLA_Deadlock_optimistic_incomplete(t);
                case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock(t);
                case(t_TrueTask) :           return new Tool_Megan_True(t);
            }
        }

        case (profile_arg_lola_pessimistic): {
            switch (task) {
                case(t_UnknownTask):         return nullptr;
                case(t_ReachabilityTask):    return new Tool_LoLA_Reachability_pessimistic(t);
                case(t_DeadlockTask):        return new Tool_LoLA_Deadlock_pessimistic(t);
                case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock(t);
                case(t_TrueTask) :           return new Tool_Megan_True(t);
            }
        }

        case (profile_arg_sara): {
            switch (task) {
                case(t_UnknownTask):         return nullptr;
                case(t_ReachabilityTask):    return new Tool_Sara_Reachability(t);
                case(t_DeadlockTask):        return nullptr;
                case(t_DeadlockInitialTask): return new Tool_Megan_InitialDeadlock(t);
                case(t_TrueTask) :           return new Tool_Megan_True(t);
            }
        }
        
        default: {
            return nullptr;
        }
    }
}
