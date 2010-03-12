#ifndef PNAPI_VERBOSE_H
#define PNAPI_VERBOSE_H

/*
#include <config.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <unistd.h>
*/

/// unconditionally print a message
// void message(const char* format, ...);

/// print a message if "--verbose" parameter was given
// void status(const char* format, ...);

/// abort with an error message and an error code
// __attribute__((noreturn)) void abort(unsigned short code, const char* format, ...);

/// verbosely display an error in a file (still experimental)
// void displayFileError(char* filename, int lineno, char* token);

#endif /* PNAPI_VERBOSE_H */
