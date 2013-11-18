/*!
\file
\brief implementation of class String
\author Niels
\status new
\ingroup g_reporting
*/

#include <config.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <InputOutput/SimpleString.h>


/*!
\post Memory for s is allocated using strdup.
*/
String::String() : s(strdup(""))
{
    assert(s);
}

/*!
\param other  String object to copy from
\post Memory for s is allocated using strdup.
*/
String::String(const String &other) : s(strdup(other.s))
{
    assert(s);
}

/*!
\param s  Pointer to NULL-terminated string to store.
\note The given string is NOT copied! Only the pointer is stored and it will be
      freed by the constructor. DO NOT use this constructor with const char*,
      string literals, or anything that should live longer than this object
*/
String::String(char *s) : s(s)
{
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

/*!
\return the wrapped NULL-terminated string s
*/
const char *String::str() const
{
    return s;
}
