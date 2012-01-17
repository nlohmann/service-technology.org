#ifndef TOOLS_H
#define TOOLS_H

#include "types.h"

typedef void* (*tool)(void *args);
typedef enum VerificationState (*interpreter)(int code, char *file);
typedef int (*preparer)(struct problem *problem);

struct tool {
  char *name;
  preparer prepare;
  interpreter interpret;
  char *program[];
};

enum VerificationState forknrun(char **args, interpreter interpret, char *logfile);
enum VerificationState run(struct problem *problem, struct tool *tool);

#endif
