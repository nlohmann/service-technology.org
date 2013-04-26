#include <config.h>
#include "Tool.h"
#include "verbose.h"

/// needed to evaluate exit status of system call
#define __WEXITSTATUS(status)   (((status) & 0xff00) >> 8)	

bool contains(std::string filename, std::string s) {
    std::string call_grep = std::string(TOOL_GREP) + " -q \"" + s + "\" " + filename;
    int return_value_grep = system(call_grep.c_str());
    return __WEXITSTATUS(return_value_grep);
}


LoLA::LoLA() {
    executable = std::string(TOOL_PATH) + "/lola-2.0-unreleased/bin/lola";
}

Tool_LoLA_Deadlock::Tool_LoLA_Deadlock() : LoLA() {}


result_t Tool_LoLA_Deadlock::execute() {
    status("searching deadlocks");

    std::string call_tool = executable + " --nolog --check=deadlock t/model.pnml.lola &> /tmp/foo";
    status("calling %s", call_tool.c_str());
    int return_value_tool = system(call_tool.c_str());
    return_value_tool = __WEXITSTATUS(return_value_tool);

    if (return_value_tool == 2) {
        return ERROR;
    }

    if (contains("/tmp/foo", "lola: result: yes")) {
        return DEFINITELY_TRUE;
    }
    if (contains("/tmp/foo", "lola: result: no")) {
        return DEFINITELY_FALSE;
    }

    return MAYBE;
}

result_t Tool_LoLA_Reachability::execute() {
    status("checking reachability");
    return DEFINITELY_TRUE;
}
