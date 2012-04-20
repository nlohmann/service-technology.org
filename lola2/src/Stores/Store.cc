/*!
\author Niels
\file Store.cc
\status new
*/

#include <string>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include "Core/Dimensions.h"
#include "Stores/Store.h"
#include "InputOutput/Reporter.h"


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

        rep->status("%10lu markings, %10lu edges, %8.0f markings/sec, %5d secs", markings, calls - 1, (last_period / (float)REPORT_FREQUENCY), (++intervals * REPORT_FREQUENCY));

/*
        if (benchmark == 0)
        {
            benchmark = markings;
        }
        
        std::string p = "";
        for (size_t i = 0; i < size_t(60.0 * (1.0 - ((last_period / (float)benchmark)))); ++i)
        {
            p += "*";
        }

        rep->status("%s %2.2f%%", rep->markup(MARKUP_UNIMPORTANT, p.c_str()).str(), fabs(100.0 * (1.0 - ((last_period / (float)benchmark)))));

*/
        last_markings = markings;
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
