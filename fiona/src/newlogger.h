#ifndef NEWLOGGER_H_ 
#define NEWLOGGER_H_ 

#include "userconfig.h"
#ifdef LOG_NEW

#include <new>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
//#include <strstream>

// Converts int to string.
std::string toString(int x);

// Collects memory allocation statistics for a particular type at a particular
// source file position.
class LogInfo
{
    private:
        // number of memory allocation calls for given type at given source
        // file position
        size_t allocCallCount;
        
        // number of memory deallocation calls for given type at given source
        // file position
        size_t deallocCallCount;
        
        // number of currently allocated bytes for given type at given source
        // file position
        size_t allocated_mem;

        // peak number of bytes allocated for given type at given source file
        // position
        size_t peak_allocated_mem;

    public:
        std::string type;    // name of class/type
        std::string filepos; // source file position
        
        // Constructs empty LogInfo. Empty strings, other values 0.
        LogInfo();

        // Constructs LogInfo with given type and filepos. All other values are
        // initialized to 0.
        LogInfo(const std::string& type, const std::string& filepos);
        
        // Logs an allocation operation. mem specifies number of allocated
        // bytes by to be logged operation.
        void logAllocation(size_t mem);

        // Logs a deallocation operation. mem specifies number of allocated
        // bytes by to be logged operation.
        void logDeallocation(size_t mem);

        // Returns allocation call count.
        size_t getAllocCallCount() const;

        // Returns deallocation call count.
        size_t getDeallocCallCount() const;

        // Returns peak allocated memory in bytes.
        size_t getPeakAllocatedMem() const;
        
        // Returns currently allocated memory in bytes.
        size_t getAllocatedMem() const;
        
        // Returns true iff peak_allocated_mem of lhs is smaller than
        // peak_allocated_mem of rhs.
        static bool compare_by_peakmem(const LogInfo* lhs, const LogInfo* rhs);
};

// Collects memory allocation statistics for a particular type.
class TypeLogInfo
{
    public:
        std::string type; // name of class/type

        // number of memory allocation calls for given type
        size_t allocCallCount;
        
        // number of memory deallocation calls for given type
        size_t deallocCallCount;
        
        // total number of currently allocated bytes for given type
        size_t allocated_mem;

        // peak number of bytes allocated for given type
        size_t peak_allocated_mem;

        // Constructs empty TypeLogInfo. Empty strings, other values 0.
        TypeLogInfo();

        // Returns true iff peak_allocated_mem of lhs is smaller than
        // peak_allocated_mem of rhs.
        static bool compare_by_peakmem(const TypeLogInfo& lhs,
            const TypeLogInfo& rhs);
};

// Saves information about a pointer.
class PointerInfo
{
    private:
        // The number of bytes allocated under the pointer.
        size_t allocated_mem;

        // Holds information about the allocation call that returned the
        // pointer.
        LogInfo* logInfo;

    public:
        // We need a default constructor because PointerInfo is used as value
        // in a map. Initializes everything to 0.
        PointerInfo();

        // Constructs PointerInfo.
        PointerInfo(size_t mem, LogInfo* info);

        // Returns number of allocated bytes under corresponding pointer.
        size_t getAllocatedMem() const;

        // Returns pointer to LogInfo structure containing information about
        // the allocation call that the returned the corresponding pointer.
        LogInfo* getLogInfo() const;
};

// Saves column widths of memory allocation report.
class ReportRowFormat
{
    public:

        // Length of longest entry in type column.
        size_t type_length;

        // Length of longest entry in filepos column.
        size_t filepos_length;

        // Length of longest entry in callcount column.
        size_t callcount_length;

        // Length of longest entry in allocated_mem column.
        size_t allocated_mem_length;

        // Constructs empty ReportRowFormat with all values initialized to 0.
        ReportRowFormat();
};


// Saves information of memory allocations and prints summary report.
class NewLogger
{
    private:
        // saves memory allocation information for each type at each relevant
        // source file position.
        // key format: "file:line:type""
        typedef std::map<std::string, LogInfo*> log_t;
        static log_t log;

        typedef std::map<const void*, PointerInfo> pointerLog_t;
        static pointerLog_t pointerLog;

        // prints a row of the report table with given data and format
        template <typename T1, typename T2, typename T3, typename T4>
        static void printReportRow(const T1& type,
            const T2& filepos, const T3& callcount, const T4& allocated_mem,
            const ReportRowFormat& format)
        {
            std::cerr << '|';
            std::cerr << std::setw(format.type_length) << type << '|';
            std::cerr << std::setw(format.filepos_length) << filepos << '|';
            std::cerr << std::setw(format.callcount_length) << callcount << '|';
            std::cerr << std::setw(format.allocated_mem_length)
                      << allocated_mem << '|';
            std::cerr << std::endl;
        }

        // prints a row of the report table with given data and format
        template <typename T1, typename T2, typename T3, typename T4,
                  typename T5, typename T6>
        static void printReportRow(const T1& type,
            const T2& filepos, const T3& allocCallCount,
            const T4& deallocCallCount,
            const T5& peakAllocatedMem, const T6& allocatedMem,
            const ReportRowFormat& format)
        {
            std::string callcountcell = toString(allocCallCount) + " (" +
                toString(allocCallCount - deallocCallCount) + ")";
            
            std::string memcell = toString(peakAllocatedMem) + " (" +
                toString(allocatedMem) + ")";
            
            printReportRow(type, filepos, callcountcell, memcell, format);
        }

        // prints a normal line for the report table
        static void printReportLine(const ReportRowFormat& format,
            char line_character = '-');

        // prints a double line for the report table
        static void printReportDoubleLine(const ReportRowFormat& format);

    public:
        // Destructs the NewLogger.
        ~NewLogger();

        // Adds memory allocation info to log for given type at fiven filepos.
        // Format of filepos is: "file:line". size is newly allocated memory in
        // bytes. pointer is the pointer returned by the allocation call.
        static void logAllocation(std::string type, std::string filepos,
            size_t size, const void* pointer);

        // Logs deallocation for given pointer.
        static void logDeallocation(const void* pointer);

        // Prints report table for all gathered data.
        static void printall();

        // Prints report table. Sorted by allocated memory, grouped by type.
        static void printall_by_typesize();
};

#endif // LOG_NEW
#endif // NEWLOGGER_H_
