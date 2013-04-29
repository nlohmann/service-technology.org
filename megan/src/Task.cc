#include "Task.h"
#include "verbose.h"

std::vector<Task*> Task::queue;
const char* result_t_names[] = { "true", "false", "true?", "false?", "?", "not implemented", "error" };

Task::Task(std::string name, bool negate) : negate(negate), name(name), worker(NULL) {
}

result_t Task::negate_result(result_t r) {
    switch (r) {
        case(DEFINITELY_TRUE): return DEFINITELY_FALSE;
        case(DEFINITELY_FALSE): return DEFINITELY_TRUE;
        case(MAYBE_TRUE): return MAYBE_FALSE;
        case(MAYBE_FALSE): return MAYBE_TRUE;
        default: return r;
    }
}

result_t Task::solve() const {
    status("solving task %s", _coutput_(name));
    result_t result = NOT_IMPLEMENTED;

    // if we assigned a worker, execute it
    if (worker) {
        result = worker->execute();
    }

    // negate result if necessary
    if (negate) {
        result = negate_result(result);
    }

    message("result of task %s: %s", _coutput_(name), _cimportant_(result_t_names[result]));
    return result;
}

std::string Task::getName() const {
    return name;
}

UnknownTask::UnknownTask(std::string name, kc::formula f, bool negate) : Task(name, negate), f(f) {
    queue.push_back(this);
    worker = task2tool(t_UnknownTask, this);
    status("created %sunknown task %s", (negate ? "negated " : ""), _coutput_(name));
}

ReachabilityTask::ReachabilityTask(std::string name, kc::formula f, bool negate) : Task(name, negate), f(f) {
    queue.push_back(this);
    worker = task2tool(t_ReachabilityTask, this);
    status("created %sreachability task %s", (negate ? "negated " : ""), _coutput_(name));
}

DeadlockTask::DeadlockTask(std::string name, bool negate) : Task(name, negate) {
    queue.push_back(this);
    worker = task2tool(t_DeadlockTask, this);
    status("created %sdeadlock task %s", (negate ? "negated " : ""), _coutput_(name));
}

DeadlockInitialTask::DeadlockInitialTask(std::string name, bool negate) : Task(name, negate) {
    queue.push_back(this);
    worker = task2tool(t_DeadlockInitialTask, this);
    status("created %sinitial deadlock task %s", (negate ? "negated " : ""), _coutput_(name));
}
