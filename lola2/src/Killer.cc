#include <config.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include "Socket.h"
#include "Reporter.h"

/// the reporter
Reporter* rep = new ReporterStream();

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

    rep->message("sending KILL packet to port 5556");
    Socket s(5556, "127.0.0.1");
    s.send("foo");

    return EXIT_SUCCESS;
}
