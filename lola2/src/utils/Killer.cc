#include <config.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <InputOutput/Socket.h>
#include <InputOutput/Reporter.h>

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

    const int port = 5556;
    const char* hostname = "localhost";
    const char* secret = "goodbye";
    rep->message("sending %s packet (%s) to %s", rep->markup(MARKUP_BAD, "KILL").str(), rep->markup(MARKUP_IMPORTANT, secret).str(), rep->markup(MARKUP_FILE, "%s:%d", hostname, port).str());
    Socket s(port, hostname);
    s.send(secret);

    return EXIT_SUCCESS;
}
