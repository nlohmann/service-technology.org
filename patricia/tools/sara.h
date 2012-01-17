#ifndef TOOLS_SARA_H
#define TOOLS_SARA_H

#include "types.h"

int sara_prepare(struct problem *problem);
enum VerificationState sara_interpret(int code, char *logfile);
struct tool *sara_tool(struct problem *problem);

#endif
