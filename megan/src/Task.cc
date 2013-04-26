#include "Task.h"
#include "verbose.h"

std::vector<Task*> Task::queue;

Task::Task(std::string name, bool negate) : negate(negate), name(name), worker(NULL) {
}

result_t Task::solve() {
    status("solving task %s", _coutput_(name));
    if (worker) {
        // TODO: care about negation!
        return worker->execute();
    } else {
        return NOT_IMPLEMENTED;
    }
}

UnknownTask::UnknownTask(std::string name, kc::formula f, bool negate) : Task(name, negate), f(f) {
    queue.push_back(this);
    status("created %sunknown task %s", (negate ? "negated " : ""), _coutput_(name));
}

ReachabilityTask::ReachabilityTask(std::string name, kc::formula f, bool negate) : Task(name, negate), f(f) {
    queue.push_back(this);
    status("created %sreachability task %s", (negate ? "negated " : ""), _coutput_(name));
}

DeadlockTask::DeadlockTask(std::string name, bool negate) : Task(name, negate) {
    queue.push_back(this);
    status("created %sdeadlock task %s", (negate ? "negated " : ""), _coutput_(name));
}

DeadlockInitialTask::DeadlockInitialTask(std::string name, bool negate) : Task(name, negate) {
    queue.push_back(this);
    status("created %sinitial deadlock task %s", (negate ? "negated " : ""), _coutput_(name));
}
