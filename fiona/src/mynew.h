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
 * \file    mynew.h
 *
 * \brief   redefinition of "new" for logging purposes
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 */

#ifndef MYNEW_H_ 
#define MYNEW_H_ 

#include "userconfig.h"
#include "config.h"

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
        filepos += intToString(line);                                      \
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
