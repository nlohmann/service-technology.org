#include <cstdio>
#include "Reporter.h"

ReporterSocket::ReporterSocket(u_short port, char* ip)
    : mySocket(Socket(port, ip))
{
}

ReporterSocket::~ReporterSocket()
{
}

void ReporterSocket::report(const char* s)
{
    mySocket.send(s);
}



ReporterStream::ReporterStream()
{

}

void ReporterStream::report(const char* s)
{
    fprintf(stderr, "%s\n", s);
}
