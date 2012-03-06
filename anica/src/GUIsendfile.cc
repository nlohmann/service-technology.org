#include <config.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <string>
#include <fstream>

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

    const int port = 5556;
    const char* hostname = "localhost";
    Socket s(port, hostname);

    std::ifstream f(argv[1]);
    std::string content = "";

    while(!f.eof()) {
        std::string line;
        getline(f, line);
        content += line;
    }

    s.send(content.c_str());
    rep->message("sent %d bytes to %s", content.length(), rep->markup(MARKUP_FILE, "%s:%d", hostname, port).str());

    return EXIT_SUCCESS;
}
