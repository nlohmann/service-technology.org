#include <config.h>
#include <fstream>
#include <cstdio>
#include <pnapi/pnapi.h>
#include "Tool.h"
#include "Runtime.h"
#include "verbose.h"
#include "ast-system-unpk.h"       /* for unparsers */

extern FILE *outfile;
extern std::vector<kc::property> properties;
extern void printer(const char *s, kc::uview v);

/// needed to evaluate exit status of system call
#define __WEXITSTATUS(status)   (((status) & 0xff00) >> 8)	

/// returns true iff the given file contains the given string
bool contains(std::string filename, std::string s) {
    auto call_grep = std::string(TOOL_GREP) + " -q \"" + s + "\" " + filename;
    auto return_value_grep = system(call_grep.c_str());

    // grep returns 0 iff string was found
    return (__WEXITSTATUS(return_value_grep) == 0);
}

std::string getTmpName(std::string suffix="") {
    auto tmp = tmpnam(NULL);
    return std::string(tmp) + suffix;
}

Tool::Tool(Task* t) : t(t) {}


int Tool::call_tool(std::string &callstring) {
    status("calling %s", callstring.c_str());
    const auto return_value_tool = system(callstring.c_str());
    return __WEXITSTATUS(return_value_tool);
}


/********
 * LoLA *
 ********/

LoLA::LoLA(Task *t) : Tool(t) {
    basedir = std::string(TOOL_PATH) + "/lola-2.0-unreleased";
}

Tool_LoLA_Deadlock::Tool_LoLA_Deadlock(Task *t) : LoLA(t) {}
Tool_LoLA_Deadlock_optimistic::Tool_LoLA_Deadlock_optimistic(Task *t) : LoLA(t) {}
Tool_LoLA_Deadlock_optimistic_incomplete::Tool_LoLA_Deadlock_optimistic_incomplete(Task *t) : LoLA(t) {}
Tool_LoLA_Deadlock_pessimistic::Tool_LoLA_Deadlock_pessimistic(Task *t) : LoLA(t) {}

result_t Tool_LoLA_Deadlock::execute() {
    status("searching for deadlocks");

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/lola --verbose --check=deadlock " + Runtime::args_info.net_arg + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // LoLA exists normally with codes 0 and 1
    if (return_value_tool > 1) {
        return ERROR;
    }

    if (contains(filename_output, "lola: result: yes")) {
        return DEFINITELY_TRUE;
    }
    if (contains(filename_output, "lola: result: no")) {
        return DEFINITELY_FALSE;
    }

    return MAYBE;
}

result_t Tool_LoLA_Deadlock_optimistic::execute() {
    status("searching for deadlocks");

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/lola --verbose --check=deadlock --encoder=copy --store=prefix " + Runtime::args_info.net_arg + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // LoLA exists normally with codes 0 and 1
    if (return_value_tool > 1) {
        return ERROR;
    }

    if (contains(filename_output, "lola: result: yes")) {
        return DEFINITELY_TRUE;
    }
    if (contains(filename_output, "lola: result: no")) {
        return DEFINITELY_FALSE;
    }

    return MAYBE;
}

result_t Tool_LoLA_Deadlock_optimistic_incomplete::execute() {
    status("searching for deadlocks");

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/lola --verbose --check=deadlock --store=bloom " + Runtime::args_info.net_arg + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // LoLA exists normally with codes 0 and 1
    if (return_value_tool > 1) {
        return ERROR;
    }

    if (contains(filename_output, "lola: result: yes")) {
        return DEFINITELY_TRUE;
    } else {
        return MAYBE_FALSE;
    }

    // this LoLA is incomplete: if we do not find a deadlock, we cannot be sure there is none

    return MAYBE;
}

result_t Tool_LoLA_Deadlock_pessimistic::execute() {
    status("searching for deadlocks");

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/lola --verbose --check=deadlock --store=prefix --encode=copy --stubborn=deletion " + Runtime::args_info.net_arg + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // LoLA exists normally with codes 0 and 1
    if (return_value_tool > 1) {
        return ERROR;
    }

    if (contains(filename_output, "lola: result: yes")) {
        return DEFINITELY_TRUE;
    }
    if (contains(filename_output, "lola: result: no")) {
        return DEFINITELY_FALSE;
    }

    return MAYBE;
}

Tool_LoLA_Reachability::Tool_LoLA_Reachability(Task *t) : LoLA(t) {}
Tool_LoLA_Reachability_optimistic::Tool_LoLA_Reachability_optimistic(Task *t) : LoLA(t) {}
Tool_LoLA_Reachability_optimistic_incomplete::Tool_LoLA_Reachability_optimistic_incomplete(Task *t) : LoLA(t) {}
Tool_LoLA_Reachability_pessimistic::Tool_LoLA_Reachability_pessimistic(Task *t) : LoLA(t) {}

result_t Tool_LoLA_Reachability::execute() {
    status("checking reachability");

    // creating formula file
    auto filename_formula = getTmpName(".formula");
    outfile = fopen(filename_formula.c_str(), "w");
    assert(outfile);
    fprintf(outfile, "FORMULA REACHABLE (");
    auto tmp = properties[t->property_id];
    assert(tmp);
    tmp->unparse(printer, kc::lola);
    fprintf(outfile, ");\n");
    fclose(outfile);
    status("created formula file %s", _cfilename_(filename_formula));

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/lola --verbose --check=modelchecking " + Runtime::args_info.net_arg + " --formula=" + filename_formula + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // LoLA exists normally with codes 0 and 1
    if (return_value_tool > 1) {
        return ERROR;
    }

    if (contains(filename_output, "lola: result: yes")) {
        return DEFINITELY_TRUE;
    }
    if (contains(filename_output, "lola: result: no")) {
        return DEFINITELY_FALSE;
    }

    return MAYBE;
}

result_t Tool_LoLA_Reachability_optimistic::execute() {
    status("checking reachability");

    // creating formula file
    auto filename_formula = getTmpName(".formula");
    outfile = fopen(filename_formula.c_str(), "w");
    assert(outfile);
    fprintf(outfile, "FORMULA REACHABLE (");
    auto tmp = properties[t->property_id];
    assert(tmp);
    tmp->unparse(printer, kc::lola);
    fprintf(outfile, ");\n");
    fclose(outfile);
    status("created formula file %s", _cfilename_(filename_formula));

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/lola --verbose --check=modelchecking --encoder=copy --store=prefix " + Runtime::args_info.net_arg + " --formula=" + filename_formula + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // LoLA exists normally with codes 0 and 1
    if (return_value_tool > 1) {
        return ERROR;
    }

    if (contains(filename_output, "lola: result: yes")) {
        return DEFINITELY_TRUE;
    }
    if (contains(filename_output, "lola: result: no")) {
        return DEFINITELY_FALSE;
    }

    return MAYBE;
}

result_t Tool_LoLA_Reachability_optimistic_incomplete::execute() {
    status("checking reachability");

    // creating formula file
    auto filename_formula = getTmpName(".formula");
    outfile = fopen(filename_formula.c_str(), "w");
    assert(outfile);
    fprintf(outfile, "FORMULA REACHABLE (");
    auto tmp = properties[t->property_id];
    assert(tmp);
    tmp->unparse(printer, kc::lola);
    fprintf(outfile, ");\n");
    fclose(outfile);
    status("created formula file %s", _cfilename_(filename_formula));

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/lola --verbose --check=modelchecking --store=bloom " + Runtime::args_info.net_arg + " --formula=" + filename_formula + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // LoLA exists normally with codes 0 and 1
    if (return_value_tool > 1) {
        return ERROR;
    }

    if (contains(filename_output, "lola: result: yes")) {
        return DEFINITELY_TRUE;
    } else {
        return MAYBE_FALSE;
    }

    // this LoLA is incomplete: if we do not find a state satisfying the formula, we cannot be sure there is none

    return MAYBE;
}

result_t Tool_LoLA_Reachability_pessimistic::execute() {
    status("checking reachability");

    // creating formula file
    auto filename_formula = getTmpName(".formula");
    outfile = fopen(filename_formula.c_str(), "w");
    assert(outfile);
    fprintf(outfile, "FORMULA REACHABLE (");
    kc::property tmp = properties[t->property_id];
    assert(tmp);
    tmp->unparse(printer, kc::lola);
    fprintf(outfile, ");\n");
    fclose(outfile);
    status("created formula file %s", _cfilename_(filename_formula));

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/lola --verbose --check=modelchecking --store=prefix --encode=copy --stubborn=deletion " + Runtime::args_info.net_arg + " --formula=" + filename_formula + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // LoLA exists normally with codes 0 and 1
    if (return_value_tool > 1) {
        return ERROR;
    }

    if (contains(filename_output, "lola: result: yes")) {
        return DEFINITELY_TRUE;
    }
    if (contains(filename_output, "lola: result: no")) {
        return DEFINITELY_FALSE;
    }

    return MAYBE;
}


/*********
 * Megan *
 *********/

Megan::Megan(Task *t) : Tool(t) {}

Tool_Megan_InitialDeadlock::Tool_Megan_InitialDeadlock(Task *t) : Megan(t) {}

result_t Tool_Megan_InitialDeadlock::execute() {
    status("checking initial deadlock");

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // open net file
    std::ifstream netfile(Runtime::args_info.net_arg);
    if (!netfile) {
        abort(11, "could not read Petri net from file %s", _cfilename_(Runtime::args_info.net_arg));
    }

    // read net from file
    pnapi::PetriNet *net = Task::getNet();
    
    // get initial marking
    pnapi::Marking m0(*net);

    // check if initial marking activates a transition
    PNAPI_FOREACH(t, net->getTransitions()) {
        if (m0.activates(**t)) {
            // the net is not dead in every marking
            status("transition %s is enabled", (*t)->getName().c_str());
            return DEFINITELY_FALSE;
        }
    }

    // the net is dead in every marking (because it is in m0)
    return DEFINITELY_TRUE;
}


Tool_Megan_True::Tool_Megan_True(Task *t) : Megan(t) {}

result_t Tool_Megan_True::execute() {
    status("checking truth");
    return DEFINITELY_TRUE;
}


/********
 * Sara *
 ********/

Sara::Sara(Task *t) : Tool(t) {
    basedir = std::string(TOOL_PATH) + "/sara-1.10";
}

Tool_Sara_Reachability::Tool_Sara_Reachability(Task *t) : Sara(t) {}

result_t Tool_Sara_Reachability::execute() {
    status("checking reachability");

    // creating formula file
    kc::property tmp = properties[t->property_id];
    auto filename_formula = getTmpName(".formula");
    outfile = fopen(filename_formula.c_str(), "w");
    assert(outfile);
    fprintf(outfile, "FORMULA (");
    tmp->unparse(printer, kc::sara);
    fprintf(outfile, ");\n");
    fclose(outfile);
    status("created formula file %s", _cfilename_(filename_formula));

    // check if net file parameter is given
    if (not Runtime::args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(Runtime::args_info.net_arg);

    // translate formula
    auto filename_sara = getTmpName(".sara");
    auto call_translation_tool = basedir + "/bin/lola2sara --net=" + Runtime::args_info.net_arg + " --lola --formula=" + filename_formula + " > " + filename_sara;
    auto return_value_translation_tool = call_tool(call_translation_tool);

    if (return_value_translation_tool != 0) {
        return ERROR;
    }

    status("translated formula file");

    // call tool
    auto filename_output = getTmpName(".log");
    auto callstring = basedir + "/bin/sara --verbose --input=" + filename_sara + " > " + filename_output + " 2>&1";
    auto return_value_tool = call_tool(callstring);

    // Sara exists normally with code 0
    if (return_value_tool != 0) {
        return ERROR;
    }

    if (contains(filename_output, "fulfilled")) {
        return DEFINITELY_TRUE;
    }
    if (contains(filename_output, "hold")) {
        return DEFINITELY_FALSE;
    }

    return MAYBE;
}
