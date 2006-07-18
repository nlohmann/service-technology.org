#include <new>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <sstream>
#include <cstdio>
#include <set>
#include <cmath>
#include "newlogger.h"

using namespace std;

std::string toString(int x)
{
    //C++-like: (fehlerhaft: fuehrt zu trailing space)
    //std::stringstream os;
    //os << x << std::ends;
    //std::string s(os.str());
    //return s;
    
    // C-like:
    char s[40];
    sprintf(s, "%d", x);
    return string(s);
}

LogInfo::LogInfo() :
    type(), filepos(), callcount(0), allocated_mem(0)
{
}

bool LogInfo::compare_by_mem(const LogInfo& lhs, const LogInfo& rhs)
{
    return lhs.allocated_mem < rhs.allocated_mem;
}

TypeLogInfo::TypeLogInfo() :
    type(), callcount(0), allocated_mem(0)
{
}

bool TypeLogInfo::compare_by_mem(const TypeLogInfo& lhs, const TypeLogInfo& rhs)
{
    return lhs.allocated_mem < rhs.allocated_mem;
}

ReportRowFormat::ReportRowFormat() :
    type_length(0), filepos_length(0), callcount_length(0),
    allocated_mem_length(0)
{
}

void NewLogger::addInfo(std::string type, std::string filepos, size_t size)
{
    std::string key = filepos + ':' + type;
    log[key].type           = type;
    log[key].filepos        = filepos;
    log[key].callcount++;
    log[key].allocated_mem += size;
}



void NewLogger::printall()
{
    printall_by_typesize();
}

void NewLogger::printall_by_size()
{
    typedef bool (*log_by_size_compare_t)(const LogInfo&, const LogInfo&);
    typedef multiset<LogInfo, log_by_size_compare_t> log_by_size_t;
    log_by_size_t log_by_size(&LogInfo::compare_by_mem);
    size_t total_mem_usage = 0;
    size_t total_callcount = 0;
    
    for (log_t::const_iterator iter = log.begin(); iter != log.end();
        ++iter)
    {
        log_by_size.insert(iter->second);
        total_mem_usage += iter->second.allocated_mem;
        total_callcount += iter->second.callcount;
    }

    for (log_by_size_t::const_reverse_iterator iter = log_by_size.rbegin();
         iter != log_by_size.rend();
        ++iter)
    {
        std::cerr << iter->filepos << ':' << iter->type << " - called "
                  << iter->callcount
                  << " times. allocated "
                  << iter->allocated_mem
                  << " bytes" << std::endl;
    }

    cerr << endl;
    cerr << "total memory usage: " << total_mem_usage << " bytes; "
         << total_callcount << " total allocation calls" << endl;
    cerr << "(total memory usage in MB: " << (total_mem_usage/1024/1024)
         << ")" << endl;
}

void NewLogger::printReportLine(const ReportRowFormat& format,
    char line_character)
{
    std::string line =
        "+" + string(format.type_length          + 2, line_character) +
        "+" + string(format.filepos_length       + 2, line_character) +
        "+" + string(format.callcount_length     + 2, line_character) +
        "+" + string(format.allocated_mem_length + 2, line_character) + "+";

    cerr << line << endl;
}

void NewLogger::printReportDoubleLine(const ReportRowFormat& format)
{
    printReportLine(format, '=');
}

void NewLogger::printall_by_typesize()
{
    // column names of report table
    std::string table_head_type = "type";
    std::string table_head_filepos = "filepos";
    std::string table_head_callcount = "callcount";
    std::string table_head_mem     = "allocated mem";

    // string to mark memory allocations for unknown type
    std::string table_type_no_type = "<no type>";

    // Format of report table. Saves column widths.
    ReportRowFormat format;
    format.type_length = max(table_head_type.size(), table_type_no_type.size());
    format.filepos_length       = table_head_filepos.size();
    format.callcount_length     = table_head_callcount.size();
    format.allocated_mem_length = table_head_mem.size();
    
    // Needed to determine column widths for callcount and memory column.
    size_t max_callcount     = 0;
    size_t max_allocated_mem = 0;
    
    // We use a multiset to sort memory allocation info by memory allocated.
    typedef bool (*log_by_size_compare_t)(const LogInfo&, const LogInfo&);
    typedef multiset<LogInfo, log_by_size_compare_t> log_by_size_t;
    log_by_size_t log_by_size(&LogInfo::compare_by_mem);

    // Saves total memory usage.
    size_t total_mem_usage = 0;

    // Saves total number of allocation calls.
    size_t total_callcount = 0;
    
    // determine allocated memory per type and sort by size (descending)
    typedef std::map<std::string, TypeLogInfo> typelog_t;
    typelog_t typelog;
    for (log_t::const_iterator iter = log.begin(); iter != log.end();
        ++iter)
    {
        log_by_size.insert(iter->second);
        total_mem_usage += iter->second.allocated_mem;
        total_callcount += iter->second.callcount;
        format.filepos_length =
            max(iter->second.filepos.size(), format.filepos_length);
        
        typelog[iter->second.type].type = iter->second.type;
        typelog[iter->second.type].allocated_mem += iter->second.allocated_mem;
        typelog[iter->second.type].callcount += iter->second.callcount;
    }

    // Another multimap to sort per type memory allocation info by size.
    typedef bool (*typelog_by_size_compare_t)(const TypeLogInfo&,
        const TypeLogInfo&);
    
    typedef multiset<TypeLogInfo, typelog_by_size_compare_t> typelog_by_size_t;
    typelog_by_size_t typelog_by_size(&TypeLogInfo::compare_by_mem);
    
    for (typelog_t::const_iterator iter = typelog.begin();
        iter != typelog.end(); ++iter)
    {
        typelog_by_size.insert(iter->second);
        
        format.type_length = max(iter->second.type.size(), format.type_length);
        max_callcount      = max(max_callcount, iter->second.callcount);
        max_allocated_mem  = max(max_allocated_mem, iter->second.allocated_mem);
    }
    
    // determine space needed for call count column
    format.callcount_length =
        max((size_t)floor(log10(max_callcount)) + 1, format.callcount_length);

    // determine space needed for allocated memory column
    format.allocated_mem_length = max((size_t)floor(log10(max_allocated_mem))+1,
        format.allocated_mem_length);

    // Right justify table cells.
    cerr << setiosflags(ios_base::right);

    // Print table head.
    printReportDoubleLine(format);
    printReportRow(table_head_type, table_head_filepos, table_head_callcount,
        table_head_mem, format);
    printReportDoubleLine(format);
    

    // For each type ...
    for (typelog_by_size_t::const_reverse_iterator typeiter =
         typelog_by_size.rbegin(); typeiter != typelog_by_size.rend();
         ++typeiter)
    {
        // Print summary for current type.
        string print_type = typeiter->type.size() != 0 ?
                                 typeiter->type : table_type_no_type;
        
        printReportRow(print_type, "total", typeiter->callcount,
            typeiter->allocated_mem, format);
        
        std::string current_type = typeiter->type;

        // Print "file:line"-entries for current type
        for (log_by_size_t::const_reverse_iterator iter = log_by_size.rbegin();
             iter != log_by_size.rend();
            ++iter)
        {
            if (iter->type != current_type)
                continue;

            printReportRow("", iter->filepos, iter->callcount,
                iter->allocated_mem, format);
        }

        printReportLine(format);
    }

    cerr << endl;
    cerr << "total memory usage: " << total_mem_usage << " bytes; "
         << total_callcount << " total allocation calls" << endl;
    cerr << "(total memory usage in MB: " << (total_mem_usage/1024/1024)
         << ")" << endl;
}

NewLogger::log_t NewLogger::log;

void * operator new (size_t size, const std::string &file, int line)
{
    //std::cout << file << ": " << line << std::endl;
    std::string filepos(file);
    filepos += ':';
    filepos += toString(line);
    NewLogger::addInfo("", filepos, size); 
    
    return ::new char[size];
}

void * operator new[] (size_t size, const std::string &file, int line)
{
    //std::cout << file << ": " << line << std::endl;
    std::string filepos(file);
    filepos += ':';
    filepos += toString(line);
    NewLogger::addInfo("", filepos, size); 
    
    return ::new char[size];
}
