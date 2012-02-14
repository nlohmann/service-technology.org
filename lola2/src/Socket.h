/*!
\file Socket.h
\author Niels
\status approved 25.01.2012
\ingroup g_reporting

\brief Socket class definition
*/

#pragma once

#include <config.h>
#include <sys/types.h>
#include "Dimensions.h"

#ifdef WIN32
#include <winsock.h>
#include <windows.h>
#include <stdint.h>
typedef uint32_t socklen_t;
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif


/*!
\brief class encapsulating Berkely Sockets (using UDP datagrams)
\ingroup g_reporting
*/
class Socket
{
    private:
        /// the socket
        const int sock;

        /// the address
        sockaddr_in address;

        /// whether we are listening (server) or sending (client)
        const unsigned listening : 1;

    public:
        /// create a socket - port is mandatory, destination address optional
        Socket(u_short port, const char* destination = NULL);

        /// close the socket
        ~Socket();

        /// receive incoming messages (does not return)
        __attribute__((noreturn)) void receive() const;

        /// wait for a specific message
        char* waitFor(const char* message) const;

        /// send a message
        void send(const char* message) const;
};
