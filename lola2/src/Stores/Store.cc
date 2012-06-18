/*!
\author Niels
\file Store.cc
\status new
*/

#include <string>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <csignal>
#include <Core/Dimensions.h>
#include <Stores/Store.h>
#include <Stores/EmptyStore.h>
#include <InputOutput/Reporter.h>
#include <cmdline.h>

extern gengetopt_args_info args_info;
extern Reporter* rep;

void* Store::reporter_internal(void)
{
    static unsigned int intervals = 0;
    uint64_t last_markings = 0;
    uint64_t benchmark = 0;

    while (true)
    {
        sleep(REPORT_FREQUENCY);

        const uint64_t last_period = markings - last_markings;
        const unsigned int time_elapsed = intervals * REPORT_FREQUENCY;
        last_markings = markings;
        ++intervals;

        if (args_info.search_arg == search_arg_findpath)
        {
            rep->status("%10llu tries, %10llu fired transitions, %5d secs", ((EmptyStore*)this)->tries, calls - 1, time_elapsed);
        }
        else
        {
            rep->status("%10llu markings, %10llu edges, %8.0f markings/sec, %5d secs", markings, calls - 1, (last_period / (float)REPORT_FREQUENCY), time_elapsed);
        }

        // early abortion
        if (args_info.timelimit_given and time_elapsed > (unsigned)args_info.timelimit_arg)
        {
            rep->status(rep->markup(MARKUP_IMPORTANT, "time limit reached - aborting").str());
            kill(getpid(), SIGUSR2);
        }

        if (args_info.markinglimit_given and markings > (unsigned)args_info.markinglimit_arg)
        {
            rep->status(rep->markup(MARKUP_IMPORTANT, "marking limit reached - aborting").str());
            kill(getpid(), SIGUSR2);
        }
    }
}

void* Store::reporter_helper(void* context)
{
    // required indirection to mix C/C++ - see http://stackoverflow.com/questions/1151582/pthread-function-from-a-class for more information
    return ((Store*)context)->reporter_internal();
}

Store::Store() : markings(0), calls(0)
{
    const int ret = pthread_create(&reporter_thread, NULL, reporter_helper, this);
    if (UNLIKELY(ret != 0))
    {
        rep->status("thread could not be created");
        rep->abort(ERROR_THREADING);
    }
}

Store::~Store()
{
    const int ret = pthread_cancel(reporter_thread);
    if (LIKELY(ret == 0))
    {
        rep->status("killed reporter thread");
    }
}

bool Store::searchAndInsert(NetState &ns, int thread)
{
    return searchAndInsert(ns);
}

void Store::finalize()
{

}
