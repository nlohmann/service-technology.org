/*!
\file Handlers.h

\author Niels
\status new

\ingroup g_runtime

\brief Definition of exit and termination handlers.
*/

#pragma once

#include <ctime>

/*!
\brief Collection of event handlers

This struct encapsulates functionality to process events while running LoLA.

\ingroup g_runtime
*/
struct Handlers
{
private:
    /// timestamp of the start of LoLA
    static time_t start_time;

    /// a thread that runs the termination handler
    static pthread_t terminationHandler_thread;

    /// remote termination handler
    static void *remoteTerminationHandler(void *);

    /// signal termination handler
    static void signalTerminationHandler(int) __attribute__((noreturn));

    /// exit handler
    static void exitHandler();

public:
    /// print statistics
    static void statistics();

    /// handler for new
    static void newHandler() __attribute__((noreturn));

    /// install the exit handler
    static void installExitHandler();

    /// install the termination handlers
    static void installTerminationHandlers();
};
