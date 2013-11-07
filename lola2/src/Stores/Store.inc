/*!
\author Niels
\author Max Görner
\author Christian Koch
\file Store.inc
\status new
*/

#include <string>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <csignal>
#include <Core/Dimensions.h>
//#include <Stores/Store.h>
#include <Stores/EmptyStore.h>
#include <InputOutput/Reporter.h>
#include <cmdline.h>

extern gengetopt_args_info args_info;

template <typename T>
void *Store<T>::reporter_internal(void)
{
    static unsigned int intervals = 0;
    uint64_t last_markings = 0;
    uint64_t benchmark = 0;

    while (true)
    {
        sleep(REPORT_FREQUENCY);

        const uint64_t last_period = get_number_of_markings() - last_markings;
        const unsigned int time_elapsed = intervals * REPORT_FREQUENCY;
        last_markings = get_number_of_markings();
        ++intervals;

        if (args_info.search_arg == search_arg_findpath)
        {
            rep->status("%10llu tries, %10llu fired transitions, %5d secs", ((EmptyStore<T> *) this)->tries,
                        get_number_of_calls() - 1, time_elapsed);
        }
        else
        {
            rep->status("%10llu markings, %10llu edges, %8.0f markings/sec, %5d secs", get_number_of_markings(),
                        get_number_of_calls() - 1, (last_period / (float)REPORT_FREQUENCY), time_elapsed);
        }

        // early abortion
        if (args_info.timelimit_given and time_elapsed > (unsigned)args_info.timelimit_arg)
        {
            rep->status(rep->markup(MARKUP_IMPORTANT, "time limit reached - aborting").str());
            kill(getpid(), SIGUSR2);
        }

        if (args_info.markinglimit_given
                and get_number_of_markings() > (unsigned)args_info.markinglimit_arg)
        {
            rep->status(rep->markup(MARKUP_IMPORTANT, "marking limit reached - aborting").str());
            kill(getpid(), SIGUSR2);
        }
    }
}

template <typename T>
void *Store<T>::reporter_helper(void *context)
{
    // required indirection to mix C/C++ - see http://stackoverflow.com/questions/1151582/pthread-function-from-a-class for more information
    return ((Store *)context)->reporter_internal();
}

template <typename T>
Store<T>::Store(uint32_t _number_of_threads) : markings(0), calls(0),
    number_of_threads(_number_of_threads)
{
    calls = new uint64_t[number_of_threads]();
    markings = new uint64_t[number_of_threads]();
    const int ret = pthread_create(&reporter_thread, NULL, reporter_helper, this);
    if (UNLIKELY(ret != 0))
    {
        rep->status("thread could not be created");
        rep->abort(ERROR_THREADING);
    }
}

template <typename T>
void Store<T>::silence()
{
    void *void_pointer;
    int ret = pthread_cancel(reporter_thread);
    ret |= pthread_join(reporter_thread, &void_pointer);
    if (LIKELY(ret == 0))
    {
        rep->status("killed reporter thread");
    }
}

template <typename T>
Store<T>::~Store()
{
    silence();
    delete[] calls;
    delete[] markings;
}

template <typename T>
uint64_t Store<T>::get_number_of_markings()
{
    uint64_t result = 0;
    for (uint32_t thread_number = 0; thread_number < number_of_threads; thread_number++)
    {
        result += markings[thread_number];
    }
    return result;
}

template <typename T>
uint64_t Store<T>::get_number_of_calls()
{
    uint64_t result = 0;
    for (uint32_t thread_number = 0; thread_number < number_of_threads; thread_number++)
    {
        result += calls[thread_number];
    }
    return result;
}

template <typename T>
bool Store<T>::popState(NetState &)
{
    rep->status("popState method not yet implemented for this store");
    rep->abort(ERROR_COMMANDLINE);
}
