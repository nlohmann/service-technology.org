#include <cstdio>                  /* for FILE io */
#include "ast-system-k.h"          /* for kc namespace */
#include "ast-system-yystype.h"    /* for flex/bison bridge */
#include "scanner.h"               /* for yyin, yylex_destroy */
#include "parser.h"                /* for yyparse */
#include "ast-system-unpk.h"       /* for unparsers */
#include "ast-system-rk.h"         /* for rewriters */
#include "cmdline.h"
#include "verbose.h"
#include "Socket.h"
#include "Task.h"
#include "Tool.h"
#include <libgen.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <csignal>

extern int yyerror(const char *);
kc::property_set root;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += (i == 0 ? "" : " ") + std::string(argv[i]);
    }

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    if (args_info.inputs_num > 1) {
        abort(1, "at most one input file can be given");
    }

    free(params);
}

void handler__SIGTERM(int signum) {
    std::stringstream s;
    std::string json = "";

    json += "{";
    json += "\"pid\":";
    s << getpid();
    json += s.str();
    json += ",";
    json += "\"signal\":\"";
    json += strsignal(signum);
    json += "\"";
    json += "}";

    Socket et(5555, "nitpickertool.com");
    et.send(json.c_str());
    exit(3);
}

void handler__termination() {
    std::stringstream s;
    std::string json = "";

    json += "{";
    json += "\"pid\":";
    s << getpid();
    json += s.str();
    json += ",";
    json += "\"exit\":\"";
    json += "termination";
    json += "\"";
    json += "}";

    Socket et(5555, "nitpickertool.com");
    et.send(json.c_str());
}

void callHome(int argc, char** argv) {
    std::stringstream s;
    std::string json = "";

    json += "{";
    json += "\"name\":\"";
    json += PACKAGE_NAME;
    json += "\",";
    json += "\"package_version\":\"";
    json += PACKAGE_VERSION;
    json += "\",";
    json += "\"svn_version\":\"";
    json += VERSION_SVN;
    json += "\",";
    json += "\"pid\":";
    s << getpid();
    json += s.str();
    json += ",";
    json += "\"build_system\":\"";
    json += CONFIG_BUILDSYSTEM;
    json += "\",";
    json += "\"hostname\":\"";
    json += CONFIG_HOSTNAME;
    json += "\",";
    json += "\"architecture\":";
    json += (SIZEOF_VOIDP * 8 == 32) ? "32" : "64";
    json += ",";
    json += "\"parameters\":[";
    for (int i = 1; i < argc; ++i)
    {
        if (i != 1)
        {
            json += ",";
        }
        json += "\"";
        json += argv[i];
        json += "\"";
    }
    json += "]";
    json += "}";

    Socket et(5555, "nitpickertool.com");
    et.send(json.c_str());
}

void printer(const char *s, kc::uview v) {
    fprintf(stdout, "%s", s);
}

void dummy_printer(const char *s, kc::uview v) {
}

int main(int argc, char* argv[]) {
    signal(SIGSEGV, handler__SIGTERM);
    signal(SIGABRT, handler__SIGTERM);
    signal(SIGTERM, handler__SIGTERM);
    signal(SIGINT, handler__SIGTERM);
    atexit(handler__termination);

    callHome(argc, argv);
    evaluateParameters(argc, argv);

    // initialize global variables
    yyin = stdin;
    yyout = stdout;

    if (args_info.inputs_num == 1) {
        yyin = fopen(args_info.inputs[0], "r");
        if (yyin == NULL) {
            abort(8, "cannot open file for reading");
        } else {
            status("reading %s", _cfilename_(args_info.inputs[0]));
        }
    }

    // start the parser
    yyparse();

    // rewrite the formula
    root = root->rewrite(kc::arrows);
    root = root->rewrite(kc::simplify);

    // root->unparse(printer, kc::lola);
    root->unparse(dummy_printer, kc::task);

    status("read %d tasks", Task::queue.size());

    //Task::queue[0]->worker = new Tool_LoLA_Deadlock();
    //result_t res = Task::queue[0]->solve();
    //status("result: %d", res);

    // tidy up
    fclose(yyin);
    yylex_destroy();

    return 0;
}
