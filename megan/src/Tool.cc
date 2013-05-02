#include <config.h>
#include <fstream>
#include <cstdio>
#include <pnapi/pnapi.h>
#include "Tool.h"
#include "verbose.h"
#include "cmdline.h"
#include "ast-system-unpk.h"       /* for unparsers */
#include "ast-system-rk.h"         /* for rewriters */

extern gengetopt_args_info args_info;
extern FILE *outfile;
extern std::vector<kc::property> properties;
extern void printer(const char *s, kc::uview v);

/// needed to evaluate exit status of system call
#define __WEXITSTATUS(status)   (((status) & 0xff00) >> 8)	

bool contains(std::string filename, std::string s) {
    std::string call_grep = std::string(TOOL_GREP) + " -q \"" + s + "\" " + filename;
    int return_value_grep = system(call_grep.c_str());
    return __WEXITSTATUS(return_value_grep);
}

std::string getTmpName() {
    char *tmp = tmpnam(NULL);
    return std::string(tmp);
}

Tool::Tool(Task* t) : t(t) {}


/********
 * LoLA *
 ********/

LoLA::LoLA(Task *t) : Tool(t) {
    basedir = std::string(TOOL_PATH) + "/lola-2.0-unreleased";
}

Tool_LoLA_Deadlock::Tool_LoLA_Deadlock(Task *t) : LoLA(t) {}


result_t Tool_LoLA_Deadlock::execute() {
    status("searching for deadlocks");

    // check if net file parameter is given
    if (not args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(args_info.net_arg);

    // call tool
    std::string filename_output = getTmpName();
    std::string call_tool = basedir + "/bin/lola --check=deadlock " + args_info.net_arg + " > " + filename_output + " 2>&1";
    status("calling %s", call_tool.c_str());
    int return_value_tool = system(call_tool.c_str());
    return_value_tool = __WEXITSTATUS(return_value_tool);

    if (return_value_tool == 2) {
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

result_t Tool_LoLA_Reachability::execute() {
    status("checking reachability");

    // creating formula file
    std::string filename_formula = getTmpName();
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
    if (not args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(args_info.net_arg);

    // call tool
    std::string filename_output = getTmpName();
    std::string call_tool = basedir + "/bin/lola --check=modelchecking " + args_info.net_arg + " --formula=" + filename_formula + " " + " > " + filename_output + " 2>&1";
    status("calling %s", call_tool.c_str());
    int return_value_tool = system(call_tool.c_str());
    return_value_tool = __WEXITSTATUS(return_value_tool);

    if (return_value_tool == 2) {
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
    status("reading net from file %s", _cfilename_(args_info.net_arg));
    pnapi::PetriNet net;
    netfile >> meta(pnapi::io::INPUTFILE, args_info.net_arg)
            >> (args_info.pnml_flag ? pnapi::io::pnml : pnapi::io::lola)
            >> net;

    // get initial marking
    pnapi::Marking m0(net);

    // check if initial marking activates a transition
    PNAPI_FOREACH(t, net.getTransitions()) {
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

    // rewriting formula
    kc::property tmp = properties[t->property_id];
    //tmp->print();
    
    tmp = tmp->rewrite(kc::sara_unfold);
    //tmp->print();
    tmp = tmp->rewrite(kc::simplify);
    status("rewrote formula");

    // creating formula file
    std::string filename_formula = getTmpName();
    outfile = fopen(filename_formula.c_str(), "w");
    assert(outfile);
    fprintf(outfile, "FORMULA (");
    tmp->unparse(printer, kc::sara);
    fprintf(outfile, ");\n");
    fclose(outfile);
    status("created formula file %s", _cfilename_(filename_formula));

    // check if net file parameter is given
    if (not args_info.net_given) {
        abort(10, "no Petri net file given via parameter %s", _cparameter_("--net"));
    }
    assert(args_info.net_arg);

    // translate formula
    std::string filename_sara = getTmpName();
    std::string call_ttool = basedir + "/bin/lola2sara --net=" + args_info.net_arg + " --lola --formula=" + filename_formula + " " + " > " + filename_sara;
    status("calling %s", call_ttool.c_str());
    int return_value_ttool = system(call_ttool.c_str());
    return_value_ttool = __WEXITSTATUS(return_value_ttool);

    if (return_value_ttool != 0) {
        //tmp->print();
        //exit(1);
        return ERROR;
    }

    status("translated formula file");

    // call tool
    std::string filename_output = getTmpName();
    std::string call_tool = basedir + "/bin/sara --input=" + filename_sara + " > " + filename_output + " 2>&1";
    status("calling %s", call_tool.c_str());
    int return_value_tool = system(call_tool.c_str());
    return_value_tool = __WEXITSTATUS(return_value_tool);

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
