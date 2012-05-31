/*!
\file Socket.cc
\author Niels
\status approved 25.01.2012
\ingroup g_reporting

\brief Socket class implementation
*/

#include <config.h>
#include <unistd.h>

#ifdef WIN32
#include <winsock.h>
#include <windows.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

#include <InputOutput/Socket.h>
#include <InputOutput/Reporter.h>


/*!
\param[in] port  The port to be used. Note that port numbers <1000 need root
           access.
\param[in] hostname  If the socket should be used to send messages (client)
           then this parameter should hold an a hostname or an IPv4 address in
           the standard format "xxx.xxx.xxx.xxx". It defaults to NULL (server).

\post The member #listening is set according to whether we are sending
      (client) or receiving messages (server).
\post The socket #sock is created. If we are receiving (server), the it is
      also bound.
*/
Socket::Socket(u_short port, const char* hostname) :
    sock(socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)),
    listening((hostname == NULL))
{
    // LCOV_EXCL_START
    if (UNLIKELY(sock == -1))
    {
        ReporterStream rep(true);
        rep.message("could not initialize socket at port %s", rep.markup(MARKUP_FILE, "%d", port).str());
        rep.abort(ERROR_NETWORK);
    }
    // LCOV_EXCL_STOP

    // specify the address
    memset(&address, 0, SIZEOF_SOCKADDR_IN);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (listening)
    {
        address.sin_addr.s_addr = INADDR_ANY;

        // bind the socket sock to the address specified in address
        // LCOV_EXCL_START
        if (UNLIKELY(bind(sock, (struct sockaddr*)&address, SIZEOF_SOCKADDR_IN) == -1))
        {
            close(sock);
            ReporterStream rep(true);
            rep.message("could not bind socket at port %s", rep.markup(MARKUP_FILE, "%d", port).str());
            rep.abort(ERROR_NETWORK);
        }
        // LCOV_EXCL_STOP
    }
    else
    {
        // resolve the hostname
        const hostent* record = gethostbyname(hostname);

        // LCOV_EXCL_START
        if (UNLIKELY(record == NULL))
        {
            ReporterStream rep(true);
            rep.message("host %s is not available", rep.markup(MARKUP_FILE, "%s", hostname).str());
            rep.abort(ERROR_NETWORK);
        }
        // LCOV_EXCL_STOP

        const in_addr* resolved_address = (in_addr*)record->h_addr;
        address.sin_addr.s_addr = inet_addr(inet_ntoa(* resolved_address));
    }
}

/*!
\post The socket #sock is closed.
*/
Socket::~Socket()
{
    close(sock);
}


/*!
\post No postcondition: this function never terminates.
\todo The reporter could be a parameter.
*/
__attribute__((noreturn)) void Socket::receive() const
{
    assert(listening);

    // the length of the address struct
    static socklen_t addressLength = SIZEOF_SOCKADDR_IN;

    /// a reporter for the received message
    ReporterStream rep(true);

    // a buffer for incoming messages
    char buffer[UDP_BUFFER_SIZE];

    while (true)
    {
        // receive data from the socket sock, stores it into buffer with
        // length UDP_BUFFER_SIZE, sets no flags, receives from address
        // specified in sa with length fromlen
        const ssize_t recsize = recvfrom(sock, reinterpret_cast<void*>(buffer), UDP_BUFFER_SIZE, 0, (struct sockaddr*)&address, &addressLength);

        // LCOV_EXCL_START
        if (UNLIKELY(recsize < 0))
        {
            rep.message("could not receive message");
            rep.abort(ERROR_NETWORK);
        }
        // LCOV_EXCL_STOP

        // get sender IP
        char display[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &address.sin_addr.s_addr, display, INET_ADDRSTRLEN);

        // get time
        time_t now;
        time(&now);
        struct tm* current = localtime(&now);

        rep.message("%s: %.*s", rep.markup(MARKUP_UNIMPORTANT, "%s @ %02i:%02i:%02i", display, current->tm_hour, current->tm_min, current->tm_sec).str(), static_cast<int>(recsize), buffer);
    }
}


/*!
\param[in] message  The message to be sent. We do not care about its memeory
           management; that is, we do not release its memory.

\post The given message is sent to the socket #sock. As we are using UDP
      datagrams it is not guaranteed whether the message is actually received.
*/
void Socket::send(const char* message) const
{
    const ssize_t bytes_sent = sendto(sock, message, strlen(message), 0, (const struct sockaddr*)&address, SIZEOF_SOCKADDR_IN);

    // LCOV_EXCL_START
    if (UNLIKELY(bytes_sent < 0))
    {
        ReporterStream rep(true);
        rep.message("could not send message '%s'", message);
        rep.abort(ERROR_NETWORK);
    }
    // LCOV_EXCL_STOP
}

/*!
\note This function is best called inside a thread.
\param[in] message  the target message to wait for
\return return the sender IP if a message is received that matches the target
        message
\post Memory for return value needs to be freed by caller.
*/
char* Socket::waitFor(const char* message) const
{
    assert(listening);

    // the length of the address struct
    static socklen_t addressLength = SIZEOF_SOCKADDR_IN;

    // a buffer for incoming messages
    char buffer[UDP_BUFFER_SIZE];

    while (true)
    {
        // receive data from the socket sock, stores it into buffer with
        // length UDP_BUFFER_SIZE, sets no flags, receives from address
        // specified in sa with length fromlen
        const ssize_t recsize = recvfrom(sock, reinterpret_cast<void*>(buffer), UDP_BUFFER_SIZE, 0, (struct sockaddr*)&address, &addressLength);

        // LCOV_EXCL_START
        if (UNLIKELY(recsize < 0))
        {
            ReporterStream rep(true);
            rep.message("could not receive message");
            rep.abort(ERROR_NETWORK);
        }
        // LCOV_EXCL_STOP

        char* received = NULL;
        const int bytes = asprintf(&received, "%.*s", static_cast<int>(recsize), buffer);
        assert(bytes != -1);
        assert(received);

        // the received message matches the target message
        if (!strcmp(received, message))
        {
            // get sender IP
            char* senderaddress = (char*)malloc(INET_ADDRSTRLEN * SIZEOF_CHAR);
            inet_ntop(AF_INET, &address.sin_addr.s_addr, senderaddress, INET_ADDRSTRLEN);
            return senderaddress;
        }
    }
}
