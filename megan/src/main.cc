#include <libgen.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <csignal>

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
#include "JSON.h"

extern int yyerror(const char *);
std::vector<kc::property> properties;

/// the command line parameters
gengetopt_args_info args_info;
extern const char *cmdline_parser_profile_values[];


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

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

void handler__signals(int signum) {
    JSON j;

    j.add("pid", getpid());
    j.add("signal", strsignal(signum));

    Socket et(5555, "nitpickertool.com");
    et.send(j);
    exit(3);
}

void handler__termination() {
    JSON j;

    j.add("pid", getpid());
    j.add("exit", "termination");

    Socket et(5555, "nitpickertool.com");
    et.send(j);
}

void callHome(int argc, char** argv) {
    JSON j;

    j.add("name", PACKAGE_NAME);
    j.add("package_version", PACKAGE_VERSION);
    j.add("svn_version", VERSION_SVN);
    j.add("pod", getpid());
    j.add("build_system", CONFIG_BUILDSYSTEM);
    j.add("hostname", CONFIG_HOSTNAME);
    j.add("architecture", SIZEOF_VOIDP * 8);

    JSON parameters;
    for (int i = 1; i < argc; ++i) {
        parameters.add(argv[i]);
    }
    j.add("parameters", parameters);

    Socket et(5555, "nitpickertool.com");
    et.send(j);
}

int main(int argc, char* argv[]) {
    signal(SIGSEGV, handler__signals);
    signal(SIGABRT, handler__signals);
    signal(SIGTERM, handler__signals);
    signal(SIGINT, handler__signals);
    atexit(handler__termination);

    callHome(argc, argv);
    evaluateParameters(argc, argv);
    
    status("using profile %s", _ctool_(cmdline_parser_profile_values[args_info.profile_arg]));

    // initialize global variables
    yyin = stdin;
    yyout = stdout;

    if (args_info.inputs_num == 1) {
        yyin = fopen(args_info.inputs[0], "r");
        if (yyin == NULL) {
            abort(8, "cannot open file '%s' for reading", args_info.inputs[0]);
        } else {
            status("reading %s", _cfilename_(args_info.inputs[0]));
        }
    } else {
        status("reading from standard input...");
    }

    // start the parser
    yyparse();
    status("parsed %d properties", properties.size());

    // simplify properties and create tasks
    for (size_t i = 0; i < properties.size(); ++i) {
        if (args_info.profile_arg == profile_arg_sara) {
            properties[i] = properties[i]->rewrite(kc::sara_unfold);
        }

        properties[i] = properties[i]->rewrite(kc::arrows);
        properties[i] = properties[i]->rewrite(kc::simplify);
        properties[i] = properties[i]->rewrite(kc::sides);
        properties[i] = properties[i]->rewrite(kc::simplify);

        properties[i]->unparse(dummy_printer, kc::task);
    }
    status("created %d tasks", Task::queue.size());

    // solve tasks
    if (not args_info.dry_given)
    {
        for (size_t i = 0; i < Task::queue.size(); ++i) {
            Task::queue[i]->solve();

            // MCC 2013
            std::cout << "FORMULA " << Task::queue[i]->getName() << " ";
            switch (Task::queue[i]->solution) {
                case(DEFINITELY_TRUE): std::cout << "TRUE"; break;
                case(DEFINITELY_FALSE): std::cout << "FALSE"; break;
                case(NOT_IMPLEMENTED): std::cout << "DO_NOT_COMPETE"; break;
                default: std::cout << "CANNOT_COMPUTE"; break;
            }
            std::cout << " TECHNIQUES ";
            switch (args_info.profile_arg) {
                case(profile_arg_lola):
                case(profile_arg_lola_optimistic):
                case(profile_arg_lola_optimistic_incomplete):
                case(profile_arg_lola_pessimistic):
                    std::cout << "EXPLICIT STATE_COMPRESSION STUBBORN_SETS"; break;
                case(profile_arg_sara):
                    std::cout << "TOPOLOGICAL STUBBORN_SETS SAT_SMT"; break;
                default: assert(false);
            }
            std::cout << std::endl;
        }
        status("processed %d tasks", Task::queue.size());
    }

    // tidy up
    fclose(yyin);
    yylex_destroy();

    message("done");

    return 0;
}
