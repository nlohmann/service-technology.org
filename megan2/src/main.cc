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
#include "verbose.h"
#include "Task.h"
#include "Tool.h"
#include "Runtime.h"

extern int yyerror(const char *);
std::vector<kc::property> properties;

extern const char *cmdline_parser_profile_values[];


int main(int argc, char* argv[]) { 
    Runtime::init(argc, argv);

    if (Runtime::args_info.inputs_num > 1) {
        abort(1, "at most one input file can be given");
    }

    status("using profile %s", _ctool_(cmdline_parser_profile_values[Runtime::args_info.profile_arg]));

    // initialize global variables
    yyin = stdin;
    yyout = stdout;

    if (Runtime::args_info.inputs_num == 1) {
        yyin = fopen(Runtime::args_info.inputs[0], "r");
        if (yyin == nullptr) {
            abort(8, "cannot open file '%s' for reading", Runtime::args_info.inputs[0]);
        } else {
            status("reading %s", _cfilename_(Runtime::args_info.inputs[0]));
        }
    } else {
        status("reading from standard input...");
    }

    // start the parser
    yyparse();
    status("parsed %d properties", properties.size());

    // simplify properties and create tasks
    for (auto& property : properties) {
        if (Runtime::args_info.profile_arg == profile_arg_sara) {
            property = property->rewrite(kc::sara_unfold);
        }

        // simplify property
        property = property->rewrite(kc::arrows);
        property = property->rewrite(kc::simplify);
        property = property->rewrite(kc::sides);
        property = property->rewrite(kc::simplify);

        // check type
        property->unparse(dummy_printer, kc::ctl);

        // extract property and create task
        property = property->rewrite(kc::problem);
        property->unparse(dummy_printer, kc::task);
    }
    status("created %d tasks", Task::queue.size());

    // solve tasks
    if (not Runtime::args_info.dry_given)
    {
        for (auto& task : Task::queue) {
            task->solve();
        }

        for (auto& task : Task::queue) {
            // MCC 2013
            std::cout << "FORMULA " << task->getName() << " ";
            switch (task->getSolution()) {
                case(DEFINITELY_TRUE): std::cout << "TRUE"; break;
                case(DEFINITELY_FALSE): std::cout << "FALSE"; break;
                case(NOT_IMPLEMENTED): std::cout << "DO_NOT_COMPETE"; break;
                default: std::cout << "CANNOT_COMPUTE"; break;
            }
            std::cout << " TECHNIQUES ";
            switch (Runtime::args_info.profile_arg) {
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
