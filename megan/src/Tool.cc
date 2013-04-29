#include <config.h>
#include <fstream>
#include <pnapi/pnapi.h>
#include "Tool.h"
#include "verbose.h"
#include "cmdline.h"

extern gengetopt_args_info args_info;

/// needed to evaluate exit status of system call
#define __WEXITSTATUS(status)   (((status) & 0xff00) >> 8)	

bool contains(std::string filename, std::string s) {
    std::string call_grep = std::string(TOOL_GREP) + " -q \"" + s + "\" " + filename;
    int return_value_grep = system(call_grep.c_str());
    return __WEXITSTATUS(return_value_grep);
}


/********
 * LoLA *
 ********/

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


/*********
 * Megan *
 *********/

Megan::Megan() {}

Tool_Megan_InitialDeadlock::Tool_Megan_InitialDeadlock() : Megan() {}

result_t Tool_Megan_InitialDeadlock::execute() {
    // check if net file parameter is given
    if (not args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(args_info.net_arg);

    // open net file
    std::ifstream netfile(args_info.net_arg);
    if (!netfile) {
        abort(11, "could not read Petri net from file %s", _cfilename_(args_info.net_arg));
    }

    // read net from file
    pnapi::PetriNet net;
    netfile >> meta(pnapi::io::INPUTFILE, args_info.net_arg)
                >> (args_info.pnml_flag ? pnapi::io::pnml : pnapi::io::owfn)
                >> net;

    // foreach transition t:
    // if transition is enabled: return DEFINITELY_FALSE
    // endforeach
    // return DEFINITELY_TRUE

    return MAYBE;
}


/********
 * Sara *
 ********/
