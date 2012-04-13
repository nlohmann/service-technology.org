/*!
\file SimpleString.cc
\author Niels
\status new
*/

#include <config.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "InputOutput/SimpleString.h"


String::String() : s(strdup(""))
{
    assert(s);
}

String::String(const String &other) : s(strdup(other.s))
{
    assert(s);
}

/*!
\note The given string is NOT copied! Only the pointer is stored and it will be
      freed by the constructor. DO NOT use this constructor with const char*,
      string literals, or anything that should live longer than this object
*/
String::String(char* s) : s(s)
{
    assert(s);
}

String::String(char* st, bool) : s(strdup(st))
{
    assert(st);
    assert(s);
}

/*!
\pre memory for member s was allocated outside this object using malloc
\post memory for member s is released
*/
String::~String()
{
    free(s);
}

char* String::str() const
{
    return s;
}
