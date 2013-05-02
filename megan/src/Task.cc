#include <fstream>
#include "Task.h"
#include "verbose.h"
#include "cmdline.h"

extern gengetopt_args_info args_info;

std::vector<Task*> Task::queue;
pnapi::PetriNet * Task::net = NULL;
const char* result_t_names[] = { "true", "false", "true?", "false?", "?", "not implemented", "error" };
size_t Task::current_property_id = 0;

pnapi::PetriNet *getNet() {
    if (Task::net == NULL) {
        Task::net = new pnapi::PetriNet();

        // open net file
        std::ifstream netfile(args_info.net_arg);
        if (!netfile) {
            abort(11, "could not read Petri net from file %s", _cfilename_(args_info.net_arg));
        }

        // read net from file
        status("reading net from file %s", _cfilename_(args_info.net_arg));
        netfile >> meta(pnapi::io::INPUTFILE, args_info.net_arg)
                >> (args_info.pnml_flag ? pnapi::io::pnml : pnapi::io::lola)
                >> *(Task::net);
    }
    
    return Task::net;
}

Task::Task(std::string name, bool negate) : negate(negate), name(name), property_id(current_property_id++), worker(NULL) {
}

Task::~Task() {
    status("destroyed task %s", name.c_str());
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

UnknownTask::UnknownTask(std::string name, bool negate) : Task(name, negate) {
    queue.push_back(this);
    worker = task2tool(t_UnknownTask, this);
    status("created %sunknown task %s", (negate ? "negated " : ""), _coutput_(name));
}

ReachabilityTask::ReachabilityTask(std::string name, bool negate) : Task(name, negate) {
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
