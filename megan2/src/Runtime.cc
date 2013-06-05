#include <config.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <new>
#include <cerrno>

#include "Socket.h"
#include "Runtime.h"
#include "verbose.h"

#define EXIT_TERMINATION 3
#define HOME_SERVER "nitpickertool.com"
//#define HOME_SERVER "localhost"
#define HOME_PORT 5555

time_t Runtime::start_time;
JSON Runtime::log;
bool Runtime::callHome = true;
FILE *Runtime::output = NULL;
gengetopt_args_info Runtime::args_info;


__attribute__((noreturn)) void Runtime::signalHandler(int signum) {
    log["exit"]["signal"] = strsignal(signum);
    exit(EXIT_TERMINATION);
}


void Runtime::newHandler() {
    log["error"] = "memory allocation failed";
    exit(EXIT_TERMINATION);
}


void Runtime::exitHandler() {
    // record runtime
    time_t end_time;
    time(&end_time);
    log["runtime"]["end_time"] = static_cast<int>(end_time);
    log["runtime"]["seconds"] = difftime(end_time, start_time);

    // record last error
    if (errno != 0) {
        log["error"] = strerror(errno);
    }

    // write to file if required
    if (output != NULL) {
        fprintf(output, "%s\n", log.toString().c_str());
    }

    // report results
    if (callHome) {
        Socket s(HOME_PORT, HOME_SERVER);
        s.send(log.toString().c_str());
    }
}


void Runtime::init(int argc, char** argv) {
    // listen to software termination signal (kill)
    signal(SIGTERM, Runtime::signalHandler);
    // listen to interrupt program (ctrl+c)
    signal(SIGINT, Runtime::signalHandler);
    // listen to user-defined signal 1 (used for remote termination)
    signal(SIGUSR1, Runtime::signalHandler);
    // listen to user-defined signal 2 (used for self termination)
    signal(SIGUSR2, Runtime::signalHandler);

    // set the function to call on normal termination
    atexit(Runtime::exitHandler);

    // install new handler
    std::set_new_handler(Runtime::newHandler);

    // store the timestamp of the call
    time(&start_time);

    // initialize log
    log["pid"] = getpid();
    log["hostname"] = CONFIG_HOSTNAME;
    log["tool"]["name"] = PACKAGE_NAME;
    log["tool"]["package_version"] = PACKAGE_VERSION;
    log["tool"]["svn_version"] = VERSION_SVN;
    log["tool"]["build_system"] = CONFIG_BUILDSYSTEM;
    log["tool"]["architecture"] = SIZEOF_VOIDP * 8;
    log["runtime"]["start_time"] = static_cast<int>(start_time);
    log["call"]["binary"] = argv[0];
    for (int i = 1; i < argc; ++i) {
        log["call"]["parameters"].add(argv[i]);
    }

    // command line parser
    struct cmdline_parser_params* params = cmdline_parser_params_create();
    if (cmdline_parser(argc, argv, &Runtime::args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }
    free(params);
}

void Runtime::setOutput(FILE *_output) {
    output = _output;
}

void Runtime::setOffline() {
    callHome = false;
}
