/*!
\file Handlers.h

\author Niels
\status new
*/

#pragma once

/*!
\brief Collection of event handlers

This struct encapsulates functionality to process events while running LoLA.
*/
struct Handlers
{
    private:
        /// a thread that runs the termination handler
        static pthread_t terminationHandler_thread;

        /// remote termination handler
        static void* remoteTerminationHandler(void*);

        /// signal termination handler
        static __attribute__((noreturn)) void signalTerminationHandler(int);

        /// exit handler
        static void exitHandler();

    public:
        /// install the exit handler
        static void installExitHandler();

        /// install the termination handlers
        static void installTerminationHandlers();
};
