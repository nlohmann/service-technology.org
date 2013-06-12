#include <fstream>
#include <future>
#include "Task.h"
#include "verbose.h"
#include "Runtime.h"

std::vector<Task*> Task::queue;
pnapi::PetriNet * Task::net = nullptr;
const char* result_t_names[] = { "true", "false", "true?", "false?", "?", "not implemented", "error" };
size_t Task::current_property_id = 0;

pnapi::PetriNet *Task::getNet() {
    if (Task::net == nullptr) {
        Task::net = new pnapi::PetriNet();

        // open net file
        std::ifstream netfile(Runtime::args_info.net_arg);
        if (!netfile) {
            abort(11, "could not read Petri net from file %s", _cfilename_(Runtime::args_info.net_arg));
        }

        // read net from file
        status("reading net from file %s", _cfilename_(Runtime::args_info.net_arg));
        netfile >> meta(pnapi::io::INPUTFILE, Runtime::args_info.net_arg)
                >> (Runtime::args_info.pnml_flag ? pnapi::io::pnml : pnapi::io::lola)
                >> *(Task::net);
    }
    
    return Task::net;
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

void Task::solve() {
    status("solving task %s", _coutput_(name));
    //auto result = NOT_IMPLEMENTED;

    // if we assigned a worker, execute it
    if (worker) {
        // call worker->execute()
        solution = std::async(&Tool::execute, worker);
        message("worker started");
    }
}

std::string Task::getName() const {
    return name;
}

result_t Task::getSolution() {
    // take old/default solution
    auto result = cached_solution;

    // if solution not is not yet finished, force to determine value
    if (solution.valid()) {
        result = solution.get();

        // negate result if necessary
        if (negate) {
            result = negate_result(result);
        }

        // cache value
        cached_solution = result;
    }

    return result;
}

Task::Task(std::string name, bool negate) : negate(negate), name(name), property_id(current_property_id++) {
}

Task::~Task() {
    status("destroyed task %s", name.c_str());
}


/****************
 * CONSTRUCTORS *
 ****************/

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

TrueTask::TrueTask(std::string name, bool negate) : Task(name, negate) {
    queue.push_back(this);
    worker = task2tool(t_DeadlockInitialTask, this);
    status("created %strue task %s", (negate ? "negated " : ""), _coutput_(name));
}
