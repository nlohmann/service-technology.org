/*!
\file Handlers.h

\author Niels
\status new
*/

#pragma once

#include <ctime>

/*!
\brief Collection of event handlers

This struct encapsulates functionality to process events while running LoLA.
*/
struct Handlers
{
    private:
        /// timestamp of the start of LoLA
        static time_t start_time;

        /// a thread that runs the termination handler
        static pthread_t terminationHandler_thread;

        /// remote termination handler
        static void* remoteTerminationHandler(void*);

        /// signal termination handler
        static __attribute__((noreturn)) void signalTerminationHandler(int);

        /// exit handler
        static void exitHandler();

    public:
        /// print statistics
        static void statistics();

        /// handler for new
        static void newHandler();

        /// install the exit handler
        static void installExitHandler();

        /// install the termination handlers
        static void installTerminationHandlers();
};
