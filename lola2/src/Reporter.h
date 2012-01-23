#pragma once

#include "Socket.h"

class Reporter
{
    public:
        ~Reporter() {};
        void report(const char*);
};

class ReporterSocket : public Reporter
{
    private:
        Socket mySocket;

    public:
        ReporterSocket(u_short port, char* ip);
        ~ReporterSocket();
        void report(const char*);
};

class ReporterStream : public Reporter
{
    public:
        ReporterStream();
        void report(const char*);
};
