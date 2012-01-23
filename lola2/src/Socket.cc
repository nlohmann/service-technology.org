/*!
\file Socket.cc
\author Niels
\status new
\ingroup g_reporting

\brief Socket class implementation
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <cstdlib>

#include <unistd.h>
#include <netinet/in.h>

#include "Socket.h"

/*!
\todo If we really need this variable later, we might want to move it to
      the file Dimensions.h.
*/
size_t Socket::bufferSize = 1024;

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

\todo We might think of a default port (e.g., 5555) for LoLA.
\todo We might not want to call perror and exit in case an error occurs.

\author Niels
\status new
*/
Socket::Socket(u_short port, const char* destination) :
    sock(socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)),
    listening((destination == NULL)),
    buffer(NULL)
{
    if (-1 == sock)   /* if socket failed to initialize, exit */
    {
        printf("Error Creating Socket");
        exit(EXIT_FAILURE);
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
        if (-1 == bind(sock, (struct sockaddr*)&address, addressLength))
        {
            perror("error bind failed");
            close(sock);
            exit(EXIT_FAILURE);
        }
    }
}

/*!
\post The socket #sock is closed.
\post Memory allocated for the #buffer is released.

\author Niels
\status new
*/
Socket::~Socket()
{
    if (buffer)
    {
        delete[] buffer;
    }

    close(sock);
}


/*!
\post No postcondition: this function never terminates.

\todo We might want a function that receives a single message instead and care
      about the infinite loop somewhere else.
\todo We might not want to call perror and exit in case an error occurs.

\author Niels
\status new
*/
void Socket::receive()
{
    assert(listening);

    // initialize buffer
    if (!buffer)
    {
        buffer = new char[bufferSize];
    }

    ssize_t recsize;

    for (;;)
    {
        // receive data from the socket sock, stores it into buffer with length bufferSize,
        // sets no flags, receives from address specified in sa with length fromlen
        recsize = recvfrom(sock, (void*)buffer, bufferSize, 0, (struct sockaddr*)&address, &addressLength);

        if (recsize < 0)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        //        printf("recsize: %d\n ", (int)recsize);
        //        printf("datagram: %.*s\n", (int)recsize, buffer);
        printf("%.*s\n", (int)recsize, buffer);
    }
}


/*!
\param[in] message  The message to be sent. We do not care about its memeory
           management; that is, we do not release its memory.

\post The given message is sent to the socket #sock. As we are using UDP
      datagrams it is not guaranteed whether the message is actually received.

\todo We might not want to call perror and exit in case an error occurs.

\author Niels
\status new
*/
void Socket::send(const char* message) const
{
    int bytes_sent = sendto(sock, message, strlen(message), 0, (struct sockaddr*)&address, addressLength);

    if (bytes_sent < 0)
    {
        printf("Error sending packet: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
