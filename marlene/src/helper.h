#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <string>
#include "cmdline.h"
#include "adapter.h"

// some initial values, which are the default values
extern Adapter::ControllerType contType;
extern unsigned int messageBound;
extern bool veryverbose;
extern unsigned int veryverboseindent;

extern gengetopt_args_info args_info;
extern void abort(unsigned int code, const char* format, ...);
extern void evaluate_command_line(int argc, char* argv[]);

extern void status(const char* format, ...);

extern std::string toString(int i);

#endif
