/*!
\status approved 25.01.2012
\todo comment me
*/

#include <config.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include "Socket.h"
#include "Reporter.h"

/// the reporter
Reporter* rep = new ReporterStream();

void signal_callback_handler(int signum)
{
    rep->message("caught signal: '%s'\n", strsignal(signum));
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    if (UNLIKELY(argc == 2 and !strcmp(argv[1], "--help")))
    {
        printf("No help\n");
        return EXIT_SUCCESS;
    }

    if (UNLIKELY(argc == 2 and !strcmp(argv[1], "--version")))
    {
        printf("No version\n");
        return EXIT_SUCCESS;
    }

    signal(SIGTERM, signal_callback_handler);

    rep->message("listening on port 1234");
    Socket s(1234);
    s.receive();

    return EXIT_SUCCESS;
}
