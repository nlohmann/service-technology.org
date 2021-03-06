/*!
\file Handlers.cc
\author Niels
\status new

\ingroup g_runtime

\brief Implementation of exit and termination handlers.
*/

#include <config.h>
#include <string>
#include <pthread.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <cmdline.h>
#include <Core/Dimensions.h>
#include <Core/Handlers.h>
#include <Net/Net.h>
#include <Net/Place.h>
#include <Net/Marking.h>
#include <Net/Transition.h>
#include <InputOutput/Socket.h>
#include <InputOutput/Reporter.h>
#include <InputOutput/SimpleString.h>

extern gengetopt_args_info args_info;

pthread_t Handlers::terminationHandler_thread;
time_t Handlers::start_time;


/*!
\param signum The signal to react to.
\note This function is only called for those signals that have been registered
      by calling signal() first - see Handlers::installTerminationHandlers().

\post LoLA is exited with exit code EXIT_TERMINATION.
*/
void Handlers::signalTerminationHandler(int signum)
{
    rep->message("caught termination signal: '%s'", strsignal(signum));
    exit(EXIT_TERMINATION);
}


/*!
The new handler is called in case memory allocation using "new" or "malloc"
fails. This function has little possibility to fix the problem, but can at
least end LoLA systematically.

\post LoLA is exited with exit code EXIT_TERMINATION.
*/
void Handlers::newHandler()
{
    rep->message("memory allocation failed");
    exit(EXIT_TERMINATION);
}


/*!
The termination handler allows to terminate LoLA by sending a predefined
secret via socket. Once the message is received, LoLA's execution is
terminated by calling sending the SIGUSR1 signals which are processed in
Handlers::signalTerminationHandler().

\return Always returns NULL - This function is only of type void* to be
        callable by pthread_create.

\post LoLA will eventually exit as the SIGUSR1 signal is sent.
*/
void *Handlers::remoteTerminationHandler(void *)
{
    Socket listener_socket(args_info.inputport_arg);
    char *sender = listener_socket.waitFor(args_info.remoteTermination_arg);
    assert(sender);
    rep->message("received %s packet (%s) from %s - shutting down",
                 rep->markup(MARKUP_BAD, "KILL").str(), rep->markup(MARKUP_IMPORTANT,
                 args_info.remoteTermination_arg).str(), rep->markup(MARKUP_FILE, sender).str());
    delete[] sender;

    // abort LoLA by sending SIGUSR1 signal
    kill(getpid(), SIGUSR1);
    return NULL;
}


/*!
Determine memory and time consumption. For the former, `ps` is called.

\note We assume `ps` to be callable by LoLA.

\todo If concurrent processes of the same binary run, the process id should be
used to find the process rather than the name.
\todo The configure script should check if the `ps` tool is present.
*/
void Handlers::statistics()
{
    std::string call = std::string("ps -o rss -o comm | ") + TOOL_GREP + " " +
                       PACKAGE + " | " + TOOL_AWK +
                       " '{ if ($1 > max) max = $1 } END { print max \" KB\" }'";
    FILE *ps = popen(call.c_str(), "r");
    unsigned int memory;
    int res = fscanf(ps, "%20u", &memory);
    assert(res != EOF);
    pclose(ps);
    rep->message("memory consumption: %u KB", memory);

    time_t now;
    time(&now);
    rep->message("time consumption: %.0lf seconds", difftime(now, start_time));
}


/*!
The exit handler allows to organize the termination of LoLA. This includes
closing files, reporting exit, and releasing memory.

\post Upcon completion, no other function is called.
*/
void Handlers::exitHandler()
{
    // release memory from command line parser
    cmdline_parser_free(&args_info);

    // shut down killer thread
    if (args_info.remoteTermination_given)
    {
        const int ret = pthread_cancel(terminationHandler_thread);
        if (ret == 0)
        {
            rep->status("killed listener thread");
        }
    }

    // tidy up
    Net::deleteNodes();
    Place::deletePlaces();
    Transition::deleteTransitions();
    Marking::deleteMarkings();

    // should be the very last call
    delete rep;
}


/*!
This function is a pure wrapper of the atexit function. It also tracks the
start time of LoLA for later time statistics.

\post Installs Handlers::exitHandler as exit handler.
*/
void Handlers::installExitHandler()
{
    // set the function to call on normal termination
    atexit(exitHandler);

    time(&start_time);
}


/*!
Installs handlers for the signals SIGTERM, SIGINT, SIGUSR1, and SIGUSR2. It
further sets up a remote termination thread in case the `--remoteTermination`
flag is used.

\post Installs signal handlers for SIGTERM, SIGINT, SIGUSR1, and SIGUSR2.
\post Starts a remote termination thread if `--remoteTermination` is given.
*/
void Handlers::installTerminationHandlers()
{
    // listen to software termination signal (kill)
    signal(SIGTERM, signalTerminationHandler);
    // listen to interrupt program (ctrl+c)
    signal(SIGINT, signalTerminationHandler);
    // listen to user-defined signal 1 (used for remote termination)
    signal(SIGUSR1, signalTerminationHandler);
    // listen to user-defined signal 2 (used for self termination)
    signal(SIGUSR2, signalTerminationHandler);

    // start up listener thread
    if (args_info.remoteTermination_given)
    {
        rep->status("enabling remote termination (%s)", rep->markup(MARKUP_PARAMETER,
                    "--remoteTermination").str());
        rep->status("setting up listener socket at port %s - secret is %s",
                    rep->markup(MARKUP_FILE, "%d", args_info.inputport_arg).str(),
                    rep->markup(MARKUP_IMPORTANT, args_info.remoteTermination_arg).str());

        const int ret = pthread_create(&terminationHandler_thread, NULL,
                                       remoteTerminationHandler, NULL);
        // LCOV_EXCL_START
        if (UNLIKELY(ret != 0))
        {
            rep->status("thread could not be created");
            rep->abort(ERROR_THREADING);
        }
        // LCOV_EXCL_STOP
    }
}
