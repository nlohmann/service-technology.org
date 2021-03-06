/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    newlogger.h
 *
 * \brief   redefinition of "new" for logging purposes
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 */

#ifndef NEWLOGGER_H_ 
#define NEWLOGGER_H_ 

#include "userconfig.h"
// #include "debug.h"
#ifdef LOG_NEW

#include <new>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
//#include <strstream>

/// prototypes for global operators new
void* operator new(size_t size, const std::string &file, int line);
void* operator new[](size_t size, const std::string &file, int line);

/// prototypes for global operators delete
void operator delete(void* mem);
void operator delete[](void* mem);

/// prototypes own memory (re/de)allocation funtions that log their operations.
void* mynew(size_t size,
            const std::string &file,
            int line,
            const std::string& type = "");

void* mycalloc(size_t n, size_t s, const std::string &file, int line);
void* myrealloc(void* oldptr, size_t newsize);
void mydelete(void* mem);

/// Declare own memory allocation function that does _not_ log. Needed because
/// we have overwritten standard malloc with own version that logs.
void* mynew_without_log(size_t size);


/// Collects memory allocation statistics for a particular type at a particular
/// source file position.
class LogInfo {
    private:
        /// number of memory allocation calls for given type at given source
        /// file position
        size_t allocCallCount;

        /// number of memory deallocation calls for given type at given source
        /// file position
        size_t deallocCallCount;

        /// number of currently allocated bytes for given type at given source
        /// file position
        size_t allocated_mem;

        /// peak number of bytes allocated for given type at given source file
        /// position
        size_t peak_allocated_mem;

    public:
        
        /// name of class/type
        std::string type;
         
        /// source file position
        std::string filepos; 

        /// Constructs empty LogInfo. Empty strings, other values 0.
        LogInfo();

        /// Constructs LogInfo with given type and filepos. All other values are
        /// initialized to 0.
        LogInfo(const std::string& type, const std::string& filepos);

        /// Logs an allocation operation. mem specifies number of allocated
        /// bytes by to be logged operation.
        void logAllocation(size_t mem);

        /// Logs a reallocation operation.
        void logReallocation(size_t oldmemsize, size_t newmemsize);

        /// Logs a deallocation operation. mem specifies number of allocated
        /// bytes by to be logged operation.
        void logDeallocation(size_t mem);

        /// Returns allocation call count.
        size_t getAllocCallCount() const;

        /// Returns deallocation call count.
        size_t getDeallocCallCount() const;

        /// Returns peak allocated memory in bytes.
        size_t getPeakAllocatedMem() const;

        /// Returns currently allocated memory in bytes.
        size_t getAllocatedMem() const;

        /// Returns true iff peak_allocated_mem of lhs is smaller than
        /// peak_allocated_mem of rhs.
        static bool compare_by_peakmem(const LogInfo* lhs, const LogInfo* rhs);
};


/// Collects memory allocation statistics for a particular type.
class TypeLogInfo {
    public:
        /// name of class/type
        std::string type; 

        /// number of memory allocation calls for given type
        size_t allocCallCount;

        /// number of memory deallocation calls for given type
        size_t deallocCallCount;

        /// total number of currently allocated bytes for given type
        size_t allocated_mem;

        /// peak number of bytes allocated for given type
        size_t peak_allocated_mem;

        /// Constructs empty TypeLogInfo. Empty strings, other values 0.
        TypeLogInfo();

        /// Returns true iff peak_allocated_mem of lhs is smaller than
        /// peak_allocated_mem of rhs.
        static bool compare_by_peakmem(const TypeLogInfo& lhs,
                                       const TypeLogInfo& rhs);
};


/// Saves information about a pointer.
class PointerInfo {
    private:
        /// The number of bytes allocated under the pointer.
        size_t allocated_mem;

        /// Holds information about the allocation call that returned the
        /// pointer.
        LogInfo* logInfo;

    public:
        /// We need a default constructor because PointerInfo is used as value
        /// in a map. Initializes everything to 0.
        PointerInfo();

        /// Constructs PointerInfo.
        PointerInfo(size_t mem, LogInfo* info);

        /// Logs a reallocation operation for pointer associated with this
        /// PointerInfo. newsize is the new size of the associated memory after
        /// the reallocation operation.
        void logReallocation(size_t newsize);

        /// Logs a deallocation operation for pointer associated with this
        /// PointerInfo.
        void logDeallocation();

        /// Returns number of allocated bytes under corresponding pointer.
        size_t getAllocatedMem() const;

        /// Returns pointer to LogInfo structure containing information about
        /// the allocation call that the returned the corresponding pointer.
        LogInfo* getLogInfo() const;
};


/// Saves column widths of memory allocation report.
class ReportRowFormat {
    public:
        /// Length of longest entry in type column.
        size_t type_length;

        /// Length of longest entry in filepos column.
        size_t filepos_length;

        /// Length of longest entry in callcount column.
        size_t callcount_length;

        /// Length of longest entry in allocated_mem column.
        size_t allocated_mem_length;

        /// Constructs empty ReportRowFormat with all values initialized to 0.
        ReportRowFormat();
};


/// Saves information of memory allocations and prints summary report.
class NewLogger {
    private:
        /// saves memory allocation information for each type at each relevant
        /// source file position.
        /// key format: "file:line:type""
        typedef std::map<std::string, LogInfo*> log_t;
        static log_t log;

        typedef std::map<const void*, PointerInfo> pointerLog_t;
        static pointerLog_t pointerLog;

        /// prints a row of the report table with given data and format
        template<typename T1, typename T2, typename T3, typename T4> static void
                printReportRow(const T1& type,
                               const T2& filepos,
                               const T3& callcount,
                               const T4& allocated_mem,
                               const ReportRowFormat& format) {

            std::cerr << '|';
            std::cerr << std::setw(format.type_length) << type << '|';
            std::cerr << std::setw(format.filepos_length) << filepos << '|';
            std::cerr << std::setw(format.callcount_length) << callcount << '|';
            std::cerr << std::setw(format.allocated_mem_length)<< allocated_mem
                    << '|';
            std::cerr << std::endl;
        }

        /// prints a row of the report table with given data and format
        template<typename T1, typename T2, typename T3, typename T4, typename T5,
                 typename T6> static void printReportRow(const T1& type,
                                                         const T2& filepos,
                                                         const T3& allocCallCount,
                                                         const T4& deallocCallCount,
                                                         const T5& peakAllocatedMem,
                                                         const T6& allocatedMem,
                                                         const ReportRowFormat& format) {

            std::string callcountcell = intToString(allocCallCount) + " ("
                    +intToString(allocCallCount - deallocCallCount) + ")";

            std::string memcell = intToString(peakAllocatedMem) + " ("
                    +intToString(allocatedMem) + ")";

            printReportRow(type, filepos, callcountcell, memcell, format);
        }

        /// prints a normal line for the report table
        static void printReportLine(const ReportRowFormat& format,
                                    char line_character = '-');

        /// prints a double line for the report table
        static void printReportDoubleLine(const ReportRowFormat& format);

    public:
        /// Destructs the NewLogger.
        ~NewLogger();

        /// Adds memory allocation info to log for given type at fiven filepos.
        /// Format of filepos is: "file:line". size is newly allocated memory in
        /// bytes. pointer is the pointer returned by the allocation call.
        static void logAllocation(std::string type,
                                  std::string filepos,
                                  size_t size,
                                  const void* pointer);

        /// Logs deallocation operation.
        static void logReallocation(const void* oldptr,
                                    const void* newptr,
                                    size_t newsize);

        /// Logs deallocation for given pointer.
        static void logDeallocation(const void* pointer);

        /// Prints report table for all gathered data.
        static void printall();

        /// Prints report table. Sorted by allocated memory, grouped by type.
        static void printall_by_typesize();
        
};

#endif /// LOG_NEW
#endif /// NEWLOGGER_H_
