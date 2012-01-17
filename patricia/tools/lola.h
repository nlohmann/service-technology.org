#ifndef TOOLS_LOLA_H
#define TOOLS_LOLA_H

#include "types.h"

int lola_prepare(struct problem *problem);
enum VerificationState lola_interpret(int code, char *logfile);
struct tool *findpath_tool(struct problem *problem);
struct tool *statepredicate_tool(struct problem *problem);

#endif
