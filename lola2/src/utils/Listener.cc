/*!
\status approved 25.01.2012
\todo comment me
*/

#include <config.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <InputOutput/SimpleString.h>
#include <InputOutput/Socket.h>
#include <InputOutput/Reporter.h>

/// the reporter
extern Reporter *rep;

void signal_callback_handler(int signum) __attribute__((noreturn));
void signal_callback_handler(int signum)
{
    rep->message("caught signal: '%s'\n", strsignal(signum));
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
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
    
    rep = new ReporterStream();

    const int port = 5555;
    signal(SIGTERM, signal_callback_handler);

    Socket s(port);
    rep->message("listening on port %s", rep->markup(MARKUP_FILE, "%d", port).str());
    s.receive();

    return EXIT_SUCCESS;
}
