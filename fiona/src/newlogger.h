#ifndef NEWLOGGER_H_ 
#define NEWLOGGER_H_ 

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
    public:
        std::string type;    // name of class/type
        std::string filepos; // source file position
        
        // number of memory allocation calls for given type at given source
        // file position
        size_t callcount;
        
        // number of bytes allocated for given type at given source file
        // position
        size_t allocated_mem;

        // Constructs empty LogInfo. Empty strings, other values 0.
        LogInfo();
        
        // Returns true iff allocated_mem of lhs is smaller than allocated mem
        // of rhs.
        static bool compare_by_mem(const LogInfo& lhs, const LogInfo& rhs);
};

// Collects memory allocation statistics for a particular type.
class TypeLogInfo
{
    public:
        std::string type; // name of class/type

        // total number of memory allocation calls for given type
        size_t callcount;

        // total number of bytes allocated for given type
        size_t allocated_mem;

        // Constructs empty TypeLogInfo. Empty strings, other values 0.
        TypeLogInfo();

        // Returns true iff allocated_mem of lhs is smaller than allocated mem
        // of rhs.
        static bool compare_by_mem(const TypeLogInfo& lhs,
            const TypeLogInfo& rhs);
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
        typedef std::map<std::string, LogInfo> log_t;
        static log_t log;

        // prints a row of the report table with given data and format
        template <typename T1, typename T2, typename T3, typename T4>
        static void printReportRow(const T1& type,
            const T2& filepos, const T3& callcount, const T4& allocated_mem,
            const ReportRowFormat& format)
        {
            std::cerr << '|';
            std::cerr << ' ' << std::setw(format.type_length) << type << " |";
            std::cerr << ' ' << std::setw(format.filepos_length) << filepos
                      << " |";
            std::cerr << ' ' << std::setw(format.callcount_length) << callcount
                      << " |";
            std::cerr << ' ' << std::setw(format.allocated_mem_length)
                      << allocated_mem << " |";
            std::cerr << std::endl;
        }

        // prints a normal line for the report table
        static void printReportLine(const ReportRowFormat& format,
            char line_character = '-');

        // prints a double line for the report table
        static void printReportDoubleLine(const ReportRowFormat& format);

    public:
        // Adds memory allocation info to log for given type at fiven filepos.
        // Format of filepos is: "file:line". size is newly allocated memory in
        // bytes.
        static void addInfo(std::string type, std::string filepos, size_t size);

        // Prints report table for all gathered data.
        static void printall();

        // [DEPRECATED] Prints old report. Prints number of calls and allocated
        // memory for each "type:file:line"-entry sorted by allocated memory in
        // descending order.
        static void printall_by_size();

        // Prints report table. Sorted by allocated memory, grouped by type.
        static void printall_by_typesize();
};

#endif
