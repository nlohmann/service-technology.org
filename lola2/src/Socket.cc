/*!
\file Socket.cc
\author Niels
\status approved 25.01.2012
\ingroup g_reporting

\brief Socket class implementation
*/

#include <errno.h>
#include <unistd.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include "Socket.h"
#include "Dimensions.h"

socklen_t Socket::addressLength = sizeof(sockaddr_in);


/*!
\param[in] port  The port to be used. Note that port numbers <1000 need root
           access.
\param[in] destination  If the socket should be used to send messages (client)
           then this parameter should hold an IPv4 address in the standard
           format "xxx.xxx.xxx.xxx". It defaults to NULL (server).

\post The member #listening is set according to whether we are sending
      (client) or receiving messages (server).
\post The socket #sock is created. If we are receiving (server), the it is
      also bound.
*/
Socket::Socket(u_short port, const char* destination)
    :
    sock(socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)),
    listening((destination == NULL)),
    buffer(NULL)
{
    if (UNLIKELY(-1 == sock))    /* if socket failed to initialize, exit */
    {
        fprintf(stderr, "%s: error creating socket\n", PACKAGE);
        if (errno != 0)
        {
            fprintf(stderr, "%s: last error message: %s\n", PACKAGE, strerror(errno));
        }
        exit(EXIT_ERROR);
    }

    // specify the address
    addressLength = sizeof(address);
    memset(&address, 0, addressLength);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = listening ? INADDR_ANY : inet_addr(destination);

    if (listening)
    {
        // bind the socket sock to the address specified in address
        if (UNLIKELY(-1 == bind(sock, (struct sockaddr*)&address, addressLength)))
        {
            close(sock);

            fprintf(stderr, "%s: bind failed\n", PACKAGE);
            if (errno != 0)
            {
                fprintf(stderr, "%s: last error message: %s\n", PACKAGE, strerror(errno));
            }
            exit(EXIT_ERROR);
        }
    }
}

/*!
\post The socket #sock is closed.
\post Memory allocated for the #buffer is released.
*/
Socket::~Socket()
{
    delete[] buffer;
    close(sock);
}


/*!
\post No postcondition: this function never terminates.

\todo We might want a function that receives a single message instead and care
      about the infinite loop somewhere else.
*/
__attribute__((noreturn)) void Socket::receive()
{
    assert(listening);

    // initialize buffer
    buffer = new char[UDP_BUFFER_SIZE];

    ssize_t recsize;

    for (;;)
    {
        // receive data from the socket sock, stores it into buffer with length UDP_BUFFER_SIZE,
        // sets no flags, receives from address specified in sa with length fromlen
        recsize = recvfrom(sock, reinterpret_cast<void*>(buffer), UDP_BUFFER_SIZE, 0, (struct sockaddr*)&address, &addressLength);

        if (UNLIKELY(recsize < 0))
        {
            fprintf(stderr, "%s: receive failed\n", PACKAGE);
            if (errno != 0)
            {
                fprintf(stderr, "%s: last error message: %s\n", PACKAGE, strerror(errno));
            }
            exit(EXIT_ERROR);
        }

        time_t now;
        time(&now);
        struct tm* current = localtime(&now);

        printf("%s: %02i:%02i:%02i: %.*s\n", PACKAGE, current->tm_hour, current->tm_min,
               current->tm_sec, static_cast<int>(recsize), buffer);
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
    ssize_t bytes_sent = sendto(sock, message, strlen(message), 0, (const struct sockaddr*)&address, addressLength);

    if (UNLIKELY(bytes_sent < 0))
    {
        fprintf(stderr, "%s: error sending packet: %s\n", PACKAGE, strerror(errno));
        if (errno != 0)
        {
            fprintf(stderr, "%s: last error message: %s\n", PACKAGE, strerror(errno));
        }
        exit(EXIT_ERROR);
    }
}
