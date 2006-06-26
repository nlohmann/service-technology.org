#include <new>
#include <iostream>
#include <string>
#include <map>
//#include <strstream>
#include <cstdio>
#include <set>
#include "newlogger.h"

using namespace std;

std::string toString(int x)
{
    /*
        std::ostrstream os;
        os << x << std::ends;
        std::string s(os.str());
        return s;
    */
    char s[40];
    sprintf(s, "%d", x);
    return string(s);
    
}

LogInfo::LogInfo() :
    filepos(), callcount(0), allocated_mem(0)
{
}

bool LogInfo::compare_by_mem(const LogInfo& lhs, const LogInfo& rhs)
{
    return lhs.allocated_mem < rhs.allocated_mem;
}


void NewLogger::addInfo(std::string filepos, size_t size)
{
    log[filepos].filepos = filepos;
    log[filepos].callcount++;
    log[filepos].allocated_mem += size;
}



void NewLogger::printall()
{
    typedef bool (*log_by_size_compare_t)(const LogInfo&, const LogInfo&);
    typedef multiset<LogInfo, log_by_size_compare_t> log_by_size_t;
    log_by_size_t log_by_size(&LogInfo::compare_by_mem);
    size_t total_mem_usage = 0;
    
    for (log_t::const_iterator iter = log.begin(); iter != log.end();
        ++iter)
    {
        log_by_size.insert(iter->second);
        total_mem_usage += iter->second.allocated_mem;
    }

    for (log_by_size_t::const_reverse_iterator iter = log_by_size.rbegin();
         iter != log_by_size.rend();
        ++iter)
    {
        std::cerr << iter->filepos << " - called "
                  << iter->callcount
                  << " times. allocated "
                  << iter->allocated_mem
                  << " bytes" << std::endl;
    }

    cerr << endl;
    cerr << "total memory usage: " << total_mem_usage << endl;
    cerr << "(total memory usage in MB: " << (total_mem_usage/1024/1024) << ")" << endl;
}

NewLogger::log_t NewLogger::log;

void * operator new (size_t size, const std::string &file, int line)
{
    //std::cout << file << ": " << line << std::endl;
    std::string filepos(file);
    filepos += ':';
    filepos += toString(line);
    NewLogger::addInfo(filepos, size); 
    return ::new char [size];
}

void * operator new[] (size_t size, const std::string &file, int line)
{
    //std::cout << file << ": " << line << std::endl;
    std::string filepos(file);
    filepos += ':';
    filepos += toString(line);
    NewLogger::addInfo(filepos, size); 
    return ::new char[size];
}
