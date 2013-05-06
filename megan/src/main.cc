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
std::vector<kc::property> properties;

/// the command line parameters
gengetopt_args_info args_info;
extern const char *cmdline_parser_profile_values[];

/// the invocation string
std::string invocation;

FILE *outfile = NULL;


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
    fprintf(outfile, "%s", s);
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
    
    status("using profile %s", _ctool_(cmdline_parser_profile_values[args_info.profile_arg]));

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
