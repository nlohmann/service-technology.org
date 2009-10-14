#ifndef VERBOSE_H
#define VERBOSE_H

/// unconditionally print a message
void message(const char* format, ...);

/// print a message if "--verbose" parameter was given
void status(const char* format, ...);

/// abort with an error message and an error code
void abort(unsigned short code, const char* format, ...);

#endif
