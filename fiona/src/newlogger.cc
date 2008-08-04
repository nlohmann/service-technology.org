/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider                              *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    newlogger.cc
 *
 * \brief   redefinition of "new" for logging purposes
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "userconfig.h"
#ifdef LOG_NEW

#include <new>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <sstream>
#include <cstdio>
#include <set>
#include <cmath>
#include <cassert>
#include "newlogger.h"
#include "debug.h"

#undef new
#undef malloc
#undef calloc
#undef realloc
#undef free

using namespace std;


//! \brief constructor
LogInfo::LogInfo() :
    allocCallCount(0), deallocCallCount(0), allocated_mem(0),
    peak_allocated_mem(0) {
}


//! \brief constructor
//! \param type_ name of the class type
//! \param filepos_ source file position
LogInfo::LogInfo(const string& type_, const string& filepos_) :
    allocCallCount(0), deallocCallCount(0), allocated_mem(0),
    peak_allocated_mem(0), type(type_), filepos(filepos_) {
}


//! \brief Logs an allocation operation 
//! \param mem specifies number of allocated bytes by to be logged operation.
void LogInfo::logAllocation(size_t mem) {
    ++allocCallCount;
    allocated_mem += mem;
    peak_allocated_mem = max(allocated_mem, peak_allocated_mem);
}


//! \brief Logs a reallocation operation.
//! \param oldmemsize specifies number of allocated bytes by the old allocation
//! \param newmemsize specifies number of allocated bytes by the new allocation
void LogInfo::logReallocation(size_t oldmemsize, size_t newmemsize) {
    assert(oldmemsize <= allocated_mem);
    allocated_mem += (newmemsize - oldmemsize);
}


//! \brief Logs a deallocation operation 
//! \param mem specifies number of allocated bytes by to be logged operation.
void LogInfo::logDeallocation(size_t mem) {
    ++deallocCallCount;
    assert(mem <= allocated_mem);
    allocated_mem -= mem;
}


//! \brief Returns allocation call count
//! \return allocation call count
size_t LogInfo::getAllocCallCount() const {
    return allocCallCount;
}

//! \brief Returns deallocation call count
//! \return deallocation call count
size_t LogInfo::getDeallocCallCount() const {
    return deallocCallCount;
}


//! \brief returned peak allocated memory in bytes
//! \returns peak allocated memory in bytes
size_t LogInfo::getPeakAllocatedMem() const {
    return peak_allocated_mem;
}


//! \brief returnes the currently allocated memory in bytes
//! \returns currently allocated memory in bytes
size_t LogInfo::getAllocatedMem() const {
    return allocated_mem;
}


//! \brief Returns true iff peak_allocated_mem of lhs is smaller than
//!        peak_allocated_mem of rhs
//! \param lhs left loginfo
//! \param rhs right loginfo
//! \return returns whether the peak of lhs is smaller than the peak of rhs
bool LogInfo::compare_by_peakmem(const LogInfo* lhs, const LogInfo* rhs) {
    return lhs->peak_allocated_mem < rhs->peak_allocated_mem;
}


//! \brief constructor
TypeLogInfo::TypeLogInfo() :
    allocCallCount(0), deallocCallCount(0), allocated_mem(0),
            peak_allocated_mem(0) {
}


//! \brief Returns true iff peak_allocated_mem of lhs is smaller than
//!        peak_allocated_mem of rhs
//! \param lhs left TypeLogInfo
//! \param rhs right TypeLogInfo
//! \return returns whether the peak of lhs is smaller than the peak of rhs
bool TypeLogInfo::compare_by_peakmem(const TypeLogInfo& lhs,
                                     const TypeLogInfo& rhs) {
    return lhs.peak_allocated_mem < rhs.peak_allocated_mem;
}


//! \brief constructor
PointerInfo::PointerInfo() :
    allocated_mem(0), logInfo(NULL) {
}


//! \brief constructor
//! \param mem size of the memory that is being pointed at in bytes
//! \param info Information about the allocation call of this pointer
PointerInfo::PointerInfo(size_t mem, LogInfo* info) :
    allocated_mem(mem), logInfo(info) {
}


//! \brief returns the size of the memory pointed at in bytes
//! \return returns memory size
size_t PointerInfo::getAllocatedMem() const {
    return allocated_mem;
}


//! \brief returns the LogInfo if the pointer
//! \return LogInfo
LogInfo* PointerInfo::getLogInfo() const {
    return logInfo;
}


//! \brief Logs a reallocation operation for pointer associated with this
//!        PointerInfo 
//! \param newsize newsize is the new size of the associated memory after
///        the reallocation operation.
void PointerInfo::logReallocation(size_t newsize) {
    logInfo->logReallocation(allocated_mem, newsize);
    allocated_mem = newsize;
}


//! \brief Logs a deallocation operation for pointer associated with this
//!        PointerInfo
void PointerInfo::logDeallocation() {
    logInfo->logDeallocation(allocated_mem);
    allocated_mem = 0;
}


//! \brief constructor
ReportRowFormat::ReportRowFormat() :
    type_length(0), filepos_length(0), callcount_length(0),
            allocated_mem_length(0) {
}


NewLogger::log_t NewLogger::log;
NewLogger::pointerLog_t NewLogger::pointerLog;


//! \brief destructor
NewLogger::~NewLogger() {
    trace(TRACE_5, "NewLogger::~NewLogger() : start\n");
    for (log_t::const_iterator iter = log.begin(); iter != log.end(); ++iter) {
        delete iter->second;
    }
    trace(TRACE_5, "NewLogger::~NewLogger() : end\n");
}

//! \brief Adds memory allocation info to log for given type at fiven filepos
//! \param type type of allocation
//! \param filepos Format of filepos is: "file:line"
//! \param size size is newly allocated memory in bytes
//! \param pointer pointer is the pointer returned by the allocation call
void NewLogger::logAllocation(std::string type,
                              std::string filepos,
                              size_t size,
                              const void* pointer) {
    std::string key = filepos + ':'+ type;

    if (log.find(key) == log.end()) {
        log[key] = new LogInfo(type, filepos);
    }

    log[key]->logAllocation(size);

    if (pointerLog.find(pointer) != pointerLog.end()) {
        PointerInfo pinfo = pointerLog[pointer];
        LogInfo linfo = *(pinfo.getLogInfo());
        cerr << "ERROR at "<<__FILE__ << ':' << __LINE__ << ". "
        << "Memory allocated at " << linfo.filepos << " was not freed."
        << endl;
    }

    pointerLog[pointer] = PointerInfo(size, log[key]);
}


//! \brief logs a reallocation operation
//! \param oldptr old pointer adress
//! \param newptr new pointer adress
//! \param size size of the allocated memory
void NewLogger::logReallocation(const void* oldptr,
                                const void* newptr,
                                size_t newsize) {
    if (pointerLog.find(oldptr) == pointerLog.end())
        return;

    PointerInfo pinfo = pointerLog.find(oldptr)->second;
    pinfo.logReallocation(newsize);
    pointerLog[newptr] = pinfo;
    pointerLog.erase(oldptr);
}


//! \brief logs a deallocation
//! \param pointer adress of the deallocation
void NewLogger::logDeallocation(const void* pointer) {
    // There may be deletes from the C++ library. So just do nothing if we get
    // a pointer for which we have no log information.
    if (pointerLog.find(pointer) == pointerLog.end())
        return;

    PointerInfo pinfo = pointerLog.find(pointer)->second;
    pinfo.logDeallocation();
    pointerLog.erase(pointer);
}


//! \brief prints report table for all gathered data
void NewLogger::printall() {
    printall_by_typesize();
}


//! \brief prints a normal line for the report table
//! \param format format to be printed in
//! \param line_character line character
void NewLogger::printReportLine(const ReportRowFormat& format,
                                char line_character) {
    std::string line = "+" + string(format.type_length + 0, line_character) + "+"
                           + string(format.filepos_length + 0, line_character) + "+"
                           + string(format.callcount_length + 0, line_character) + "+"
                           + string(format.allocated_mem_length + 0, line_character) + "+";

    cerr << line << endl;
}


//! \brief prints a double line for the report table
//! \param format format to be printed in
void NewLogger::printReportDoubleLine(const ReportRowFormat& format) {
    printReportLine(format, '=');
}


//! \brief Prints report table. Sorted by allocated memory, grouped by type.
void NewLogger::printall_by_typesize() {
    // column names of report table
    std::string table_head1_type = "";
    std::string table_head1_filepos = "";
    std::string table_head1_callcount = "alloc calls";
    std::string table_head1_mem = "peak allocated mem";
    std::string table_head2_type = "type";
    std::string table_head2_filepos = "filepos";
    std::string table_head2_callcount = "(w/o dealloc)";
    std::string table_head2_mem = "(not deallocated)";

    // string to mark memory allocations for unknown type
    std::string table_type_no_type = "<no type>";

    // Format of report table. Saves column widths.
    ReportRowFormat format;
    format.type_length = max(max(table_head1_type.size(),
                                 table_head2_type.size()),
                             table_type_no_type.size());

    format.filepos_length = max(table_head1_filepos.size(),
                                table_head2_filepos.size());

    format.callcount_length = max(table_head1_callcount.size(),
                                  table_head2_callcount.size());

    format.allocated_mem_length = max(table_head1_mem.size(),
                                      table_head2_mem.size());

    // Needed to determine column widths for callcount and memory column.
    size_t max_callcount = 0;
    size_t max_allocated_mem = 0;

    // We use a multiset to sort memory allocation info by memory allocated.
    typedef bool (*log_by_size_compare_t)(const LogInfo*, const LogInfo*);
    typedef multiset<LogInfo*, log_by_size_compare_t> log_by_size_t;
    log_by_size_t log_by_size(&LogInfo::compare_by_peakmem);

    // Saves total peak allocated memory.
    size_t total_peak_allocated_mem = 0;

    // Saves total (still) allocated memory, that is, not deallocated memory.
    size_t total_allocated_mem = 0;

    // Saves total number of allocation calls.
    size_t total_alloccallcount = 0;

    // Saves total number of deallocation calls.
    size_t total_dealloccallcount = 0;

    // determine allocated memory per type and sort by size (descending)
    typedef std::map<std::string, TypeLogInfo> typelog_t;
    typelog_t typelog;
    for (log_t::const_iterator iter = log.begin(); iter != log.end(); ++iter) {
        log_by_size.insert(iter->second);

        total_peak_allocated_mem += iter->second->getPeakAllocatedMem();
        total_allocated_mem += iter->second->getAllocatedMem();
        total_alloccallcount += iter->second->getAllocCallCount();
        total_dealloccallcount += iter->second->getDeallocCallCount();

        format.filepos_length = max(iter->second->filepos.size(),
                                    format.filepos_length);

        typelog[iter->second->type].type = iter->second->type;

        typelog[iter->second->type].peak_allocated_mem
                += iter->second->getPeakAllocatedMem();

        typelog[iter->second->type].allocated_mem
                += iter->second->getAllocatedMem();

        typelog[iter->second->type].allocCallCount
                += iter->second->getAllocCallCount();

        typelog[iter->second->type].deallocCallCount
                += iter->second->getDeallocCallCount();
    }

    // Another multimap to sort per type memory allocation info by size.
    typedef bool (*typelog_by_size_compare_t)(const TypeLogInfo&,
                                              const TypeLogInfo&);

    typedef multiset<TypeLogInfo, typelog_by_size_compare_t> typelog_by_size_t;
    typelog_by_size_t typelog_by_size(&TypeLogInfo::compare_by_peakmem);

    for (typelog_t::const_iterator iter = typelog.begin();
         iter != typelog.end(); ++iter) {

        typelog_by_size.insert(iter->second);

        format.type_length = max(iter->second.type.size(), format.type_length);
        max_callcount = max(max_callcount, iter->second.allocCallCount);
        max_allocated_mem = max(max_allocated_mem,
                                iter->second.peak_allocated_mem);
    }

    // determine space needed for call count column
    format.callcount_length = max((size_t)floor(log10((double) max_callcount)) + 1,
                                  format.callcount_length);

    // determine space needed for allocated memory column
    format.allocated_mem_length = max((size_t)floor(log10((double) max_allocated_mem)) + 1,
                                      format.allocated_mem_length);

    // Right justify table cells.
    cerr << setiosflags(ios_base::right);

    // Print table head.
    printReportDoubleLine(format);
    printReportRow(table_head1_type, table_head1_filepos,
                   table_head1_callcount, table_head1_mem, format);
    printReportRow(table_head2_type, table_head2_filepos,
                   table_head2_callcount, table_head2_mem, format);
    printReportDoubleLine(format);

    // For each type ...
    for (typelog_by_size_t::const_reverse_iterator typeiter = typelog_by_size.rbegin();
         typeiter != typelog_by_size.rend(); ++typeiter) {
        // Print summary for current type.
        string print_type = typeiter->type.size() != 0 ? typeiter->type : table_type_no_type;

        printReportRow(print_type, "total", typeiter->allocCallCount,
                       typeiter->deallocCallCount,
                       typeiter->peak_allocated_mem, typeiter->allocated_mem,
                       format);

        std::string current_type = typeiter->type;

        // Print "file:line"-entries for current type
        for (log_by_size_t::const_reverse_iterator iter = log_by_size.rbegin();
             iter != log_by_size.rend(); ++iter) {

            if ((*iter)->type != current_type)
                continue;

            printReportRow("", (*iter)->filepos, (*iter)->getAllocCallCount(), (*iter)->getDeallocCallCount(), (*iter)->getPeakAllocatedMem(), (*iter)->getAllocatedMem(), format);
        }

        printReportLine(format);
    }

    cerr << endl;
    cerr << "total memory usage: " << total_peak_allocated_mem << " bytes; "
         << total_alloccallcount << " total allocation calls"<< endl;
    cerr << "(total memory usage in MB: "
         << (total_peak_allocated_mem/1024/1024) << ")" << endl;
    cerr << "total not deallocated memory: " << total_allocated_mem << " bytes; "
         << total_alloccallcount - total_dealloccallcount
         << " allocs w/o matching deallocs" << endl;

// debugging for memory leaks:
    for (NewLogger::pointerLog_t::iterator pointer = pointerLog.begin(); pointer != pointerLog.end(); pointer++) 
    {
        PointerInfo pinfo = pointer->second;
        LogInfo linfo = *(pinfo.getLogInfo());
        cerr << "ERROR at "<<__FILE__ << ':' << __LINE__ << ". "
            << "Memory allocated at " << linfo.filepos << " was not freed."
            << " (pointer: " << pointer->first << ")"
            << endl;
    }

}

//! \brief new not logging 'new' function
//! \param size size of the allocated memory
void* mynew_without_log(size_t size) {
    return malloc(size);
}


//! \brief new logging 'new' function
//! \param size size of the allocated memory
//! \param file filename
//! \param line line number
//! \param type type of the object
void* mynew(size_t size,
            const std::string &file,
            int line,
            const std::string &type) {
    std::string filepos(file);
    filepos += ':';
    filepos += intToString(line);
    void* ptr = mynew_without_log(size);
    NewLogger::logAllocation(type, filepos, size, ptr);
    return ptr;
}


//! \brief new logging unary 'new' operator
//! \param size size of the allocated memory
//! \param file filename
//! \param line line number
void * operator new(size_t size, const std::string &file, int line) {
    return mynew(size, file, line, "");
}


//! \brief new reallocation function
//! \param oldptr old pointer position
//! \param newsize newsize new size after reallocation
void* myrealloc(void* oldptr, size_t newsize) {
    void* newptr = realloc(oldptr, newsize);
    NewLogger::logReallocation(oldptr, newptr, newsize);
    return newptr;
}


//! \brief new logging unary 'new' operator for arrays
//! \param size size of the allocated memory
//! \param file filename
//! \param line line number
void * operator new[](size_t size, const std::string &file, int line) {
    return mynew(size, file, line, "");
}


//! \brief new allocation function for arrays
//! \param n size of a single object
//! \param s size of the arry
//! \param file filename
//! \param line line number
void* mycalloc(size_t n, size_t s, const std::string &file, int line) {
    std::string filepos(file);
    filepos += ':';
    filepos += intToString(line);
    void* ptr = calloc(n, s);
    NewLogger::logAllocation("", filepos, n*s, ptr);
    return ptr;
}


//! \brief new delete function
//! \param mem pointer at the memory to be deleted
void mydelete(void* mem) {
    NewLogger::logDeallocation(mem);
    free(mem);
}


//! \brief new unary delete operator
//! \param mem pointer at the memory to be deleted
void operator delete(void* mem) {
    mydelete(mem);
}


//! \brief new unary delete operator for arrays
//! \param mem pointer at the memory to be deleted
void operator delete[](void* mem) {
    mydelete(mem);
}

#endif // LOG_NEW
