#pragma once

#include <ctime>
#include <cstdio>
#include "JSON.h"
#include "cmdline.h"

struct Runtime {
    private:
        /// whether to report online
        static bool callHome;

        /// where to write result
        static FILE* output;

        /// a JSON object that logs runtime information
        static JSON log;

        /// timestamp of the start of the binary
        static time_t start_time;

        /// signal termination handler
        static __attribute__((noreturn)) void signalHandler(int);

        /// exit handler
        static void exitHandler();

        /// new handler
        static void newHandler();

    public:
        /// the command line parameters
        static gengetopt_args_info args_info;

        /// initialze struct and install handlers
        static void init(int argc, char** argv);

        /// set the output file
        static void setOutput(FILE *);

        /// do not report results
        static void setOffline();
};
