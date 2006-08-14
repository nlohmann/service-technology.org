#ifndef MYNEW_H_ 
#define MYNEW_H_ 

#include "userconfig.h"

#ifdef LOG_NEW

#include <new>
#include <string>
#include <iostream>
#include "newlogger.h"
#include "debug.h"


// new should be substituted with NEW_NEW such that user defined operators new
// (with current file and line as parameters) are called. 
#define NEW_NEW new(__FILE__, __LINE__) 

// Substitute malloc, calloc, realloc and free with own versions that log.
#define malloc(size)       mynew(size, __FILE__, __LINE__)
#define calloc(n, s)       mycalloc(n, s, __FILE__, __LINE__)
#define realloc(ptr, size) myrealloc(ptr, size)
#define free(ptr)          mydelete(ptr)

// the body of user defined class operators new and new[]
#define NEW_OPERATOR_BODY(CLASSNAME) \
    {                                                                   \
        std::string filepos(file);                                      \
        filepos += ':';                                                 \
        filepos += toString(line);                                      \
        void* ptr = mynew_without_log(size);                            \
        NewLogger::logAllocation(#CLASSNAME, filepos, size, ptr);       \
        return ptr;                                                     \
    }

// definition of user defined class operators new and new[]
//
// Usage: If you want calls to operators new for class MyClass to be logged
// then add the following lines to public part of the declaration of the class
// MyClass:
//
//     #undef new
//     NEW_OPERATOR(MyClass)
//     #define new NEW_NEW
//
// Beware of copy-and-paste errors! Substitute MyClass with your class name.
// Otherwise calls to operator new of your class will be logged as calls for
// MyClass.
#define NEW_OPERATOR(CLASSNAME) \
    void* operator new (size_t size, const std::string &file, int line)   \
    NEW_OPERATOR_BODY(CLASSNAME)                                          \
    void* operator new[] (size_t size, const std::string &file, int line) \
    NEW_OPERATOR_BODY(CLASSNAME)


#else // LOG_NEW

// LOG_NEW is undefined. So leave new defined as new. And let NEW_OPERATOR() be
// void.

#define NEW_NEW new
#define NEW_OPERATOR(CLASSNAME)

#endif // LOG_NEW

#define new NEW_NEW

#endif // MYNEW_H_
