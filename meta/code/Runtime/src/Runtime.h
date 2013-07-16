#pragma once

#include <JSON.h>
#include <ctime>
#include <cstdio>
#include <ostream>
#include <string>

class Runtime {
    private:
        /// a JSON object that logs runtime information
        JSON _log;

        /// timestamp of the start of the binary
        time_t start_time;

        /// signal termination handler
        __attribute__((noreturn)) static void signalHandler(int);

        /// a pointer to a runtime object to allow logging from static functions
        static Runtime* _rt;

        /// exit handler
        static void exitHandler();

        /// new handler
        __attribute__((noreturn)) static void newHandler();

    private:
        /// whether to use colored output
        static bool _useColor;

        /// a buffer for messages
        char _buffer[1024];

        /// whether verbose messages should be shown
        bool _verbose;

        /// a stored exit code
        static int _exitcode;

    public:
        /// constructor
        Runtime();

        /// destructor
        ~Runtime();

        /// access to the log JSON object
        JSON& operator[](const std::string&);
        /// access to the log JSON object
        JSON& operator[](const char*);

        /// pass the command-line arguments
        void arguments(int argc, char** argv);

        /// unconditionally print a message
        void status(const char* format, ...);
        /// print a message if "--verbose" parameter was given
        void message(const char* format, ...);

        /// quit the program with a given exit code (similar to return in main)
        __attribute__((noreturn)) void exit(int);

        /// abort with an error message and an error code
        __attribute__((noreturn)) void error(int code, const char* format, ...);
};

extern Runtime rt;
