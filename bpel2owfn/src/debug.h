/*
 * debug.h
 *
 * Change log:
 * 
 * date        | author        | changes
 * ---------------------------------------------------------------------
 *  2005-11-09 | Gierds        | initial release
 *
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <iostream>
#include <fstream>

#define TRACE_ALWAYS 0
#define TRACE_WARNINGS 1
#define TRACE_INFORMATION 2
#define TRACE_DEBUG 3
#define TRACE_VERY_DEBUG 4

void trace(int trace_level, std::string message);
void trace(std::string message);

#endif

