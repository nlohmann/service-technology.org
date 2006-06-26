#ifndef NEWLOGGER_H_ 
#define NEWLOGGER_H_ 

#include <new>
#include <iostream>
#include <string>
#include <map>
//#include <strstream>

std::string toString(int x);

class LogInfo
{
    public:
        std::string filepos;
        size_t callcount;
        size_t allocated_mem;
        LogInfo();
        static bool compare_by_mem(const LogInfo& lhs, const LogInfo& rhs);
};


class NewLogger
{
    private:
        typedef std::map<std::string, LogInfo> log_t;
        static log_t log;
    public:
        static void addInfo(std::string filepos, size_t size);
        static void printall();
};

#endif
